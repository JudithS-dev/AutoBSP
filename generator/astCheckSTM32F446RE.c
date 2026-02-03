#include "astCheckSTM32F446RE.h"

#include <stdlib.h>

#include "STM32F446RE.h"
#include "logging.h"
#include "astEnums2Str.h"

static const pin_cap_t* pincap_find_stm32f446re(char port, uint8_t num);
static void is_valid_stm32f446re_pin(const char *module_name, int line_nr, pin_t pin);
static void bind_pwm_pins_stm32f446re(ast_dsl_node_t* dsl_node);
static void bind_pwm_prescaler_period_stm32f446re(ast_dsl_node_t* dsl_node);
static void bind_uart_pins_stm32f446re(ast_dsl_node_t* dsl_node);


/* -------------------------------------------- */
/*       Validity checks for STM32F446RE        */
/* -------------------------------------------- */

/**
 * @brief Checks if all enabled modules have valid pins for the STM32F446RE controller.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Checks if the pins assigned to enabled modules are valid for the STM32F446RE (exist and support the required functionality).
 */
void ast_check_stm32f446re_valid_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_stm32f446re_valid_pins", 0, "DSL node is NULL.");
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      
      // ----- Perform checks for non-UART modules -----
      if(current_module->kind != MODULE_UART){
        // Check if pin is a valid STM32F446RE pin
        is_valid_stm32f446re_pin(current_module->name, current_module->line_nr, current_module->pin);
        
        // Check if pin is not marked as not usable
        pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
        if(cur_cap->not_usable)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        
        // Check if pin supports the module functionality
        switch(current_module->kind){
          case MODULE_INPUT:  if(!cur_cap->can_gpio_in)
                                log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Pin '%s' does not support GPIO_INPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_OUTPUT: if(!cur_cap->can_gpio_out)
                                log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Pin '%s' does not support GPIO_OUTPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_PWM_OUTPUT: if(cur_cap->pwm_count == 0)
                                    log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Pin '%s' does not support PWM_OUTPUT for module '%s'.",
                                              pin_to_string(current_module->pin),
                                              current_module->name);
                                  break;
          default:            log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Unknown module kind '%d' for module '%s'.",
                                        current_module->kind,
                                        current_module->name);
        }
        
      } else{ 
        // ----- Perform checks for UART modules -----
        
        // Check if pins are valid STM32F446RE pins
        is_valid_stm32f446re_pin(current_module->name, current_module->line_nr, current_module->pin);
        is_valid_stm32f446re_pin(current_module->name, current_module->line_nr, current_module->data.uart.rx_pin);
        
        // Check if pins are not marked as not usable
        pin_cap_t *tx_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
        if(tx_cap->not_usable)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "TX Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        pin_cap_t *rx_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->data.uart.rx_pin.port, (uint8_t)(current_module->data.uart.rx_pin.pin_number));
        if(rx_cap->not_usable)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "RX Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if tx and rx pins are on the same port (required by hardware) (except for PC12 and PD2)
        if( (current_module->pin.port == 'C') && (current_module->pin.pin_number == 12) &&
            (current_module->data.uart.rx_pin.port == 'D') && (current_module->data.uart.rx_pin.pin_number == 2)){
          // Special case: PC12 and PD2 are allowed to be used together for UART5
        } else if(current_module->pin.port != current_module->data.uart.rx_pin.port)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "TX Pin '%s' and RX Pin '%s' must be on the same port for module '%s'.",
                    pin_to_string(current_module->pin),
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if pins support UART functionality
        if(tx_cap->uart_count == 0)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "TX Pin '%s' does not support UART for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        if(rx_cap->uart_count == 0)
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "RX Pin '%s' does not support UART for module '%s'.",
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
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "TX Pin '%s' does not support UART TX functionality for module '%s'.",
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
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "RX Pin '%s' does not support UART RX functionality for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if databits is valid (STM32F4 only supports 8 or 9)
        if((current_module->data.uart.databits != 8) && (current_module->data.uart.databits != 9))
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Databits value '%u' is invalid for UART module '%s' on STM32F446RE. Supported values: 8, 9.",
                    current_module->data.uart.databits,
                    current_module->name);
        
        // Check if stopbits is valid (STM32F4 only supports 1 or 2)
        if((current_module->data.uart.stopbits != 1.0f) && (current_module->data.uart.stopbits != 2.0f))
          log_error("ast_check_stm32f446re_valid_pins", current_module->line_nr, "Stopbits value '%.1f' is invalid for UART module '%s' on STM32F446RE. Supported values: 1, 2.",
                    current_module->data.uart.stopbits,
                    current_module->name);
      }
      
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Validates if the given pin is valid for the STM32F446RE controller.
 * 
 * @param module_name Name of the module using the pin (for logging purposes).
 * @param line_nr Line number where the pin is defined (for logging purposes).
 * @param pin Pin to validate.
 * 
 * Checks if the pin follows the PXn format and is within the valid range for STM32F446RE.
 */
static void is_valid_stm32f446re_pin(const char *module_name, int line_nr, pin_t pin){
  if(module_name == NULL)
    log_error("is_valid_stm32f446re_pin", line_nr, "Module name is NULL for pin '%s'.",
              pin_to_string(pin));
  
  if(pin.identifier != PXn)
    log_error("is_valid_stm32f446re_pin", line_nr, "Pin '%s' for module '%s' is not in PXn format (required for STM32F446RE).",
              pin_to_string(pin),
              module_name);
  
  if(((pin.port < 'A') || (pin.port > 'D')) && (pin.port != 'H'))
    log_error("is_valid_stm32f446re_pin", line_nr, "Invalid port '%c' for module '%s' on STM32F446RE. Valid ports: A, B, C, D, H.",
              pin.port,
              module_name);
  
  // Port D only has pin 2
  if((pin.port == 'D') && (pin.pin_number != 2))
    log_error("is_valid_stm32f446re_pin", line_nr, "Invalid pin number '%u' for port 'D' for module '%s' on STM32F446RE. Valid pins on port D: 2.",
              pin.pin_number,
              module_name);
  
  // Port H only has pin 0 and 1
  if((pin.port == 'H') && (pin.pin_number > 1))
    log_error("is_valid_stm32f446re_pin", line_nr, "Invalid pin number '%u' for port 'H' for module '%s' on STM32F446RE. Valid pins on port H: 0, 1.",
              pin.pin_number,
              module_name);
  
  if(pin.pin_number > 15)
    log_error("is_valid_stm32f446re_pin", line_nr, "Invalid pin number '%u' for port '%c' for module '%s' on STM32F446RE. Valid range: 0-15.", pin.pin_number, pin.port, module_name);
  
  if((pin.port == 'B') && (pin.pin_number == 11))
    log_error("is_valid_stm32f446re_pin", line_nr, "Pin '%s' for module '%s' is not usable on STM32F446RE.",
              pin_to_string(pin),
              module_name);
}


/* -------------------------------------------- */
/*      Backend specific parameter bindings     */
/* -------------------------------------------- */

/**
 * @brief Backend specific parameter bindings for STM32F446RE.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Binds parameters such as timer numbers and channels for PWM modules based on pin capabilities.
 */
void ast_check_stm32f446re_bind_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_stm32f446re_bind_pins", 0, "DSL node is NULL.");
  
  bind_pwm_pins_stm32f446re(dsl_node);
  bind_pwm_prescaler_period_stm32f446re(dsl_node);
  bind_uart_pins_stm32f446re(dsl_node);
}

/**
 * @brief Binds PWM pins for STM32F446RE.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Assigns timer numbers and channels to PWM output modules based on available options and usage.
 */
static void bind_pwm_pins_stm32f446re(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("bind_pwm_pins_stm32f446re", 0, "DSL node is NULL.");
  
  bool tim_used[15] = { false }; // TIM1 to TIM14 (0 unused)
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && (current_module->kind == MODULE_PWM_OUTPUT)){
      pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
      if(cur_cap->pwm_count == 0)
        log_error("bind_pwm_pins_stm32f446re", 0, "Pin '%s' does not support PWM for module '%s'.",
                  pin_to_string(current_module->pin),
                  current_module->name);
      
      // Pick the first PWM option whose timer is still free
      for(uint8_t i = 0; i < cur_cap->pwm_count; i++){
        pwm_opt_t *pwm_opt = &cur_cap->pwm[i];
        if(!tim_used[pwm_opt->tim]){
          // Assign this timer and channel to the module
          current_module->data.pwm.tim_number  = pwm_opt->tim;
          current_module->data.pwm.tim_channel = pwm_opt->ch;
          current_module->data.pwm.gpio_af     = pwm_opt->af;
          
          tim_used[pwm_opt->tim] = true; // Mark timer as used
          
          char* pin_str = pin_to_string(current_module->pin);
          log_info("bind_pwm_pins_stm32f446re", LOG_OTHER, 0, "Assigned TIM%d_CH%d (AF%d) to PWM module '%s' on pin '%s'.",
                    current_module->data.pwm.tim_number,
                    current_module->data.pwm.tim_channel,
                    current_module->data.pwm.gpio_af,
                    current_module->name,
                    pin_str);
          free(pin_str);
          break;
        }
      }
    }
    current_module = current_module->next;
  }
}

#define STM32F446RE_TIMER_CLOCK_HZ 84000000u
#define PWM_FIXED_ARR 999u
#define PWM_MAX_PRESCALER 0xFFFFu

/**
 * @brief Binds PWM prescaler and period for STM32F446RE.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Calculates and assigns prescaler and period values for PWM output modules based on target frequency.
 */
static void bind_pwm_prescaler_period_stm32f446re(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("bind_pwm_prescaler_period_stm32f446re", 0, "DSL node is NULL.");
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && (current_module->kind == MODULE_PWM_OUTPUT)){
      uint32_t target_freq = current_module->data.pwm.frequency;
      if(target_freq == 0){
        log_error("bind_pwm_prescaler_period_stm32f446re", current_module->line_nr, "PWM frequency cannot be zero for module '%s'.",
                  current_module->name);
      }
      
      // Set period to 999 for 0.1% resolution
      current_module->data.pwm.period = PWM_FIXED_ARR;
      
      // Calculate prescaler
      const uint32_t denom = (target_freq * (PWM_FIXED_ARR + 1u));
      if(denom == 0)
        log_error("bind_pwm_prescaler_period_stm32f446re", current_module->line_nr, "Invalid calculation for prescaler for module '%s'.",
                  current_module->name);
      if(denom > STM32F446RE_TIMER_CLOCK_HZ)
        log_error("bind_pwm_prescaler_period_stm32f446re", current_module->line_nr, "Target frequency too low for PWM module '%s'.",
                  current_module->name);
      uint32_t prescaler = (STM32F446RE_TIMER_CLOCK_HZ / denom) - 1u;
      if(prescaler > PWM_MAX_PRESCALER)
        log_error("bind_pwm_prescaler_period_stm32f446re", current_module->line_nr, "Calculated prescaler too high for PWM module '%s'.",
                  current_module->name);
      current_module->data.pwm.prescaler = (uint16_t)prescaler;
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Binds UART pins for STM32F446RE.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Assigns USART numbers and GPIO alternate function numbers to UART modules based on available options and usage.
 */
static void bind_uart_pins_stm32f446re(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("bind_uart_pins_stm32f446re", 0, "DSL node is NULL.");
  
  bool usart_used[7] = { false }; // UART/USART1 to UART/USART6 (0 unused)
  
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && (current_module->kind == MODULE_UART)){
      pin_cap_t *tx_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
      pin_cap_t *rx_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->data.uart.rx_pin.port, (uint8_t)(current_module->data.uart.rx_pin.pin_number));
      // Find common USART options between tx and rx pins
      bool usart_assigned = false;
      for(uint8_t i = 0; i < tx_cap->uart_count && !usart_assigned; i++){
        uart_opt_t *tx_opt = &tx_cap->uart[i];
        for(uint8_t j = 0; j < rx_cap->uart_count; j++){
          uart_opt_t *rx_opt = &rx_cap->uart[j];
          if((tx_opt->usart == rx_opt->usart) && (tx_opt->is_uart == rx_opt->is_uart)){
            // Check if this USART is already used
            if(!usart_used[tx_opt->usart]){
              // Assign this USART to the module
              current_module->data.uart.usart_number = tx_opt->usart;
              current_module->data.uart.is_uart      = tx_opt->is_uart;
              if(tx_opt->af != rx_opt->af)
                log_error("bind_uart_pins_stm32f446re", current_module->line_nr, "INTERNAL ERROR: TX pin '%s' and RX pin '%s' for UART module '%s' have different AF numbers.",
                          pin_to_string(current_module->pin),
                          pin_to_string(current_module->data.uart.rx_pin),
                          current_module->name);
              current_module->data.uart.gpio_af      = tx_opt->af; // TX and RX should have the same AF number
              usart_used[tx_opt->usart] = true; // Mark USART as used
              usart_assigned = true;
              break;
            }
          }
        }
      }
    }
    current_module = current_module->next;
  }
}


/* -------------------------------------------- */
/*               Helper functions               */
/* -------------------------------------------- */

/**
 * @brief Finds the pin capabilities for a given port and pin number on the STM32F446RE.
 * 
 * @param port The port character (e.g., 'A', 'B', 'C').
 * @param num The pin number (0-15).
 * @return const pin_cap_t* Pointer to the pin capabilities structure.
 */
static const pin_cap_t* pincap_find_stm32f446re(char port, uint8_t num){
  for(uint16_t i = 0; i < PINCAP_STM32F446RE_LEN; i++){
    if(PINCAP_STM32F446RE[i].pin.port == port && PINCAP_STM32F446RE[i].pin.num == num)
      return &PINCAP_STM32F446RE[i];
  }
  log_error("pincap_find_stm32f446re", 0, "Pin 'P%c%d' not found in STM32F446RE pin capabilities.", port, num);
  return NULL; // This line will never be reached due to log_error exiting the program
}