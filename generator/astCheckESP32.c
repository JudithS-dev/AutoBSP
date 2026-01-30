#include "astCheckESP32.h"

#include <stdlib.h>

#include "ESP32.h"
#include "logging.h"
#include "astEnums2Str.h"

static const pin_cap_t* pincap_find_esp32(uint8_t num);
static void is_valid_esp32_pin(const char *module_name, int line_nr, pin_t pin);
static void bind_pwm_pins_esp32(ast_dsl_node_t* dsl_node);
//static void bind_uart_pins_esp32(ast_dsl_node_t* dsl_node);


/* -------------------------------------------- */
/*      Required parameter checks for ESP32     */
/* -------------------------------------------- */
/**
 * @brief Checks that unsupported parameters are not set for ESP32 modules.
 * 
 * @param dsl_builder Pointer to the DSL builder.
 * 
 * Checks that parameters not supported by ESP32 (e.g., speed, pull for PWM) are not set.
 */
void ast_check_esp32_required_params(ast_dsl_builder_t* dsl_builder){
  if(dsl_builder == NULL)
    log_error("ast_check_esp32_required_params", 0, "DSL builder is NULL.");
  
  ast_module_builder_t* current_builder = dsl_builder->module_builders_root;
  while(current_builder != NULL){
    // ESP32 doesn't support speed setting for modules
    if(current_builder->speed_set == true)
      log_error("ast_check_esp32_required_params", current_builder->module->line_nr,
                "Parameter 'speed' is not supported for ESP32 in module '%s'.",
                current_builder->module->name == NULL ? "<NULL>" : current_builder->module->name);
    
    // ESP32 doesn't support pull setting for PWM modules
    if(current_builder->module->kind == MODULE_PWM_OUTPUT){
      if(current_builder->pull_set == true)
        log_error("ast_check_esp32_required_params", current_builder->module->line_nr,
                  "Parameter 'pull' is not supported for PWM modules on ESP32 in module '%s'.",
                  current_builder->module->name == NULL ? "<NULL>" : current_builder->module->name);
    }
    
    current_builder = current_builder->next;
  }
}


/* -------------------------------------------- */
/*           Validity checks for ESP32          */
/* -------------------------------------------- */

/**
 * @brief Checks if all enabled modules have valid pins for the ESP32 controller.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Checks if the pins assigned to enabled modules are valid for the ESP32 (exist and support the required functionality).
 */
void ast_check_esp32_valid_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_esp32_valid_pins", 0, "DSL node is NULL.");
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      
      // ----- Perform checks for non-UART modules -----
      if(current_module->kind != MODULE_UART){
        // Check if pin is a valid ESP32 pin
        is_valid_esp32_pin(current_module->name, current_module->line_nr, current_module->pin);
        
        // Check if pin is not marked as not usable
        pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_esp32((uint8_t)(current_module->pin.pin_number));
        if(cur_cap->not_usable)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Pin '%s' is marked as not usable on ESP32 for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        
        // Check if pin supports the module functionality
        switch(current_module->kind){
          case MODULE_INPUT:  if(!cur_cap->can_gpio_in)
                                log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Pin '%s' does not support GPIO_INPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_OUTPUT: if(!cur_cap->can_gpio_out)
                                log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Pin '%s' does not support GPIO_OUTPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_PWM_OUTPUT: if(!cur_cap->can_gpio_out) // PWM-signal can be routed to all output-capable pins on ESP32
                                    log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Pin '%s' does not support PWM_OUTPUT for module '%s'.",
                                              pin_to_string(current_module->pin),
                                              current_module->name);
                                  break;
          default:            log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Unknown module kind '%d' for module '%s'.",
                                        current_module->kind,
                                        current_module->name);
        }
        
        // Check if pins support Pull-Up/Pull-Down functionality (not supported on Pins GPIO34-GPIO35)
        if(current_module->pin.pin_number >= 34){
          if(current_module->kind == MODULE_INPUT){
            if(current_module->data.input.pull != GPIO_PULL_NONE)
              log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Pin '%s' does not support Pull-Up/Pull-Down functionality for GPIO_INPUT module '%s'.",
                        pin_to_string(current_module->pin),
                        current_module->name);
          } else{
            log_error("ast_check_esp32_valid_pins", current_module->line_nr, "INTERNAL ERROR: The pin '%s' is invalid for non-input module '%s'. An error should have been raised earlier.",
                      pin_to_string(current_module->pin),
                      current_module->name);
          }
        }
        
      } else{ 
        // ----- Perform checks for UART modules -----
        
        // Check if pins are valid ESP32 pins
        is_valid_esp32_pin(current_module->name, current_module->line_nr, current_module->pin);
        is_valid_esp32_pin(current_module->name, current_module->line_nr, current_module->data.uart.rx_pin);
        
        // Check if pins are not marked as not usable
        pin_cap_t *tx_cap = (pin_cap_t*)pincap_find_esp32((uint8_t)(current_module->pin.pin_number));
        if(tx_cap->not_usable)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "TX Pin '%s' is marked as not usable on ESP32 for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        pin_cap_t *rx_cap = (pin_cap_t*)pincap_find_esp32((uint8_t)(current_module->data.uart.rx_pin.pin_number));
        if(rx_cap->not_usable)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "RX Pin '%s' is marked as not usable on ESP32 for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if pins support UART functionality
        /*if(tx_cap->uart_count == 0) // TODO: Maybe add UART support check later (at least check if input/output capable)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "TX Pin '%s' does not support UART for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        if(rx_cap->uart_count == 0)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "RX Pin '%s' does not support UART for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if pins support tx and rx functionality
        bool tx_supports_tx = false;
        for(uint8_t i = 0; i < tx_cap->uart_count; i++){
          if(tx_cap->uart[i].is_tx){
            tx_supports_tx = true;
            break;
          }
        }
        if(!tx_supports_tx)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "TX Pin '%s' does not support UART TX functionality for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        
        bool rx_supports_rx = false;
        for(uint8_t i = 0; i < rx_cap->uart_count; i++){
          if(rx_cap->uart[i].is_tx == false){
            rx_supports_rx = true;
            break;
          }
        }
        if(!rx_supports_rx)
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "RX Pin '%s' does not support UART RX functionality for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        */
        
        // Check if databits is valid (STM32F4 only supports 8 or 9)
        /*if((current_module->data.uart.databits != 8) && (current_module->data.uart.databits != 9)) // TODO: Adjust checks for ESP32
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Databits value '%u' is invalid for UART module '%s' on STM32F446RE. Supported values: 8, 9.",
                    current_module->data.uart.databits,
                    current_module->name);
        
        // Check if stopbits is valid (STM32F4 only supports 1 or 2)
        if((current_module->data.uart.stopbits != 1.0f) && (current_module->data.uart.stopbits != 2.0f))
          log_error("ast_check_esp32_valid_pins", current_module->line_nr, "Stopbits value '%.1f' is invalid for UART module '%s' on STM32F446RE. Supported values: 1, 2.",
                    current_module->data.uart.stopbits,
                    current_module->name);*/
      }
      
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Validates if the given pin is valid for the ESP32 controller.
 * 
 * @param module_name Name of the module using the pin (for logging purposes).
 * @param line_nr Line number where the pin is defined (for logging purposes).
 * @param pin Pin to validate.
 * 
 * Checks if the pin follows the GPIOn format and is within the valid range for ESP32.
 */
static void is_valid_esp32_pin(const char *module_name, int line_nr, pin_t pin){
  if(module_name == NULL)
    log_error("is_valid_esp32_pin", line_nr, "Module name is NULL for pin '%s'.",
              pin_to_string(pin));
  
  if(pin.identifier != GPIOn)
    log_error("is_valid_esp32_pin", line_nr, "Pin '%s' for module '%s' is not in GPIOn format (required for ESP32).",
              pin_to_string(pin),
              module_name);
  
  if((pin.port != '\0'))
    log_error("is_valid_esp32_pin", line_nr, "INTERNAL ERROR: Pin '%s' for module '%s' has invalid port '%c' for ESP32 (should be '\\0').",
              pin_to_string(pin),
              module_name,
              pin.port);
  
  if(pin.pin_number >= 36) // ESP32 has GPIO0 to GPIO35
    log_error("is_valid_esp32_pin", line_nr, "Invalid pin number '%u' for port '%c' for module '%s' on ESP32. Valid range: 0-35.", pin.pin_number, pin.port, module_name);
  
  // Unusable pins on ESP32: GPIO6 to GPIO11 (used for flash memory)
  if((pin.pin_number >= 6) && (pin.pin_number <= 11))
    log_error("is_valid_esp32_pin", line_nr, "Pin '%s' for module '%s' is not usable on ESP32 (used for flash memory).",
              pin_to_string(pin),
              module_name);
  
  // Not existing pins on ESP32: GPIO20 and GPIO24 and GPIO28-GPIO31
  if(pin.pin_number == 20 || pin.pin_number == 24 || (pin.pin_number >= 28 && pin.pin_number <= 31))
    log_error("is_valid_esp32_pin", line_nr, "Pin '%s' for module '%s' does not exist on ESP32.",
              pin_to_string(pin),
              module_name);
}


/* -------------------------------------------- */
/*      Backend specific parameter bindings     */
/* -------------------------------------------- */

/**
 * @brief Backend specific parameter bindings for ESP32.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Binds parameters such as timer numbers and channels for PWM modules based on pin capabilities.
 */
void ast_check_esp32_bind_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_esp32_bind_pins", 0, "DSL node is NULL.");
  bind_pwm_pins_esp32(dsl_node);
  /*bind_uart_pins_esp32(dsl_node);*/
}

/**
 * @brief Binds PWM pins for ESP32.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Assigns timer numbers and channels to PWM output modules by iteration.
 */
static void bind_pwm_pins_esp32(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("bind_pwm_pins_esp32", 0, "DSL node is NULL.");
  
  const uint8_t MAX_TIMERS = 4; // ESP32 has 4 LEDC timers (0-3)
  uint8_t nr_tim_used = 0;
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && (current_module->kind == MODULE_PWM_OUTPUT)){
      pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_esp32((uint8_t)(current_module->pin.pin_number));
      if(cur_cap->can_gpio_out == false)
        log_error("bind_pwm_pins_esp32", 0, "Pin '%s' does not support PWM for module '%s'.",
                  pin_to_string(current_module->pin),
                  current_module->name);
      
      // Pick the first PWM option whose timer is still free
      if(nr_tim_used >= MAX_TIMERS)
        log_error("bind_pwm_pins_esp32", current_module->line_nr, "All available PWM timers are already assigned. Cannot assign PWM module '%s' on pin '%s'.",
                  current_module->name,
                  pin_to_string(current_module->pin));
      
      current_module->data.pwm.tim_number = nr_tim_used; // Assign next free timer number
      current_module->data.pwm.tim_channel = nr_tim_used; // For simplicity, use same number for channel (channels are shared between timers on ESP32)
      current_module->data.pwm.gpio_af   = 0; // Not used on ESP32
      current_module->data.pwm.prescaler = 0; // Not used on ESP32
      current_module->data.pwm.period    = 0; // Not used on ESP32
      
      nr_tim_used++;
    }
    current_module = current_module->next;
  }
}


/* -------------------------------------------- */
/*               Helper functions               */
/* -------------------------------------------- */

/**
 * @brief Finds the pin capabilities for a given port and pin number on the ESP32.
 * 
 * @param num The pin number (0-35).
 * @return const pin_cap_t* Pointer to the pin capabilities structure.
 */
static const pin_cap_t* pincap_find_esp32(uint8_t num){
  for(uint16_t i = 0; i < PINCAP_ESP32_LEN; i++){
    if(PINCAP_ESP32[i].pin.num == num)
      return &PINCAP_ESP32[i];
  }
  log_error("pincap_find_esp32", 0, "Pin 'GPIO%d' not found in ESP32 pin capabilities.", num);
  return NULL; // This line will never be reached due to log_error exiting the program
}