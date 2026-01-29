#include "astCheckESP32.h"

#include <stdlib.h>

#include "logging.h"
#include "astEnums2Str.h"

// TODO: write all functions for ESP32
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

  /*ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){

      // ----- Perform checks for non-UART modules -----
      if(current_module->kind != MODULE_UART){
        // Check if pin is a valid STM32F446RE pin
        is_valid_stm32f446re_pin(current_module->name, current_module->line_nr, current_module->pin);
        
        // Check if pin is not marked as not usable
        pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
        if(cur_cap->not_usable)
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        
        // Check if pin supports the module functionality
        switch(current_module->kind){
          case MODULE_INPUT:  if(!cur_cap->can_gpio_in)
                                log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Pin '%s' does not support GPIO_INPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_OUTPUT: if(!cur_cap->can_gpio_out)
                                log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Pin '%s' does not support GPIO_OUTPUT for module '%s'.",
                                          pin_to_string(current_module->pin),
                                          current_module->name);
                              break;
          case MODULE_PWM_OUTPUT: if(cur_cap->pwm_count == 0)
                                    log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Pin '%s' does not support PWM_OUTPUT for module '%s'.",
                                              pin_to_string(current_module->pin),
                                              current_module->name);
                                  break;
          default:            log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Unknown module kind '%d' for module '%s'.",
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
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "TX Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        pin_cap_t *rx_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->data.uart.rx_pin.port, (uint8_t)(current_module->data.uart.rx_pin.pin_number));
        if(rx_cap->not_usable)
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "RX Pin '%s' is marked as not usable on STM32F446RE for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if tx and rx pins are on the same port (required by hardware)
        if(current_module->pin.port != current_module->data.uart.rx_pin.port)
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "TX Pin '%s' and RX Pin '%s' must be on the same port for module '%s'.",
                    pin_to_string(current_module->pin),
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if pins support UART functionality
        if(tx_cap->uart_count == 0)
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "TX Pin '%s' does not support UART for module '%s'.",
                    pin_to_string(current_module->pin),
                    current_module->name);
        if(rx_cap->uart_count == 0)
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "RX Pin '%s' does not support UART for module '%s'.",
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
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "TX Pin '%s' does not support UART TX functionality for module '%s'.",
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
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "RX Pin '%s' does not support UART RX functionality for module '%s'.",
                    pin_to_string(current_module->data.uart.rx_pin),
                    current_module->name);
        
        // Check if databits is valid (STM32F4 only supports 8 or 9)
        if((current_module->data.uart.databits != 8) && (current_module->data.uart.databits != 9))
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Databits value '%u' is invalid for UART module '%s' on STM32F446RE. Supported values: 8, 9.",
                    current_module->data.uart.databits,
                    current_module->name);
        
        // Check if stopbits is valid (STM32F4 only supports 1 or 2)
        if((current_module->data.uart.stopbits != 1.0f) && (current_module->data.uart.stopbits != 2.0f))
          log_error("are_valid_stm32f446re_pins", current_module->line_nr, "Stopbits value '%.1f' is invalid for UART module '%s' on STM32F446RE. Supported values: 1, 2.",
                    current_module->data.uart.stopbits,
                    current_module->name);
      }
      
    }
    current_module = current_module->next;
  }*/
}


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
  /*bind_pwm_pins_esp32(dsl_node);
  bind_pwm_prescaler_period_esp32(dsl_node);
  bind_uart_pins_esp32(dsl_node);*/
}