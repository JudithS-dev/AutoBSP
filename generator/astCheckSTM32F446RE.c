#include "astCheckSTM32F446RE.h"

#include <stdlib.h>

#include "STM32F44RE.h"
#include "logging.h"
#include "astEnums2Str.h"

static const pin_cap_t* pincap_find_stm32f446re(char port, uint8_t num);
static void is_valid_stm32f446re_pin(pin_t pin);
static void bind_pwm_pins_stm32f446re(ast_dsl_node_t* dsl_node);
static void bind_pwm_prescaler_period_stm32f446re(ast_dsl_node_t* dsl_node);


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
  ast_module_node_t* current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      // Check if pin is a valid STM32F446RE pin
      is_valid_stm32f446re_pin(current_module->pin);
      // Check if pin supports the module functionality
      pin_cap_t *cur_cap = (pin_cap_t*)pincap_find_stm32f446re(current_module->pin.port, (uint8_t)(current_module->pin.pin_number));
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
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Validates if the given pin is valid for the STM32F446RE controller.
 * 
 * @param pin The pin to validate.
 * 
 * Checks if the pin follows the PXn format and is within the valid range for STM32F446RE.
 */
static void is_valid_stm32f446re_pin(pin_t pin){
  if(pin.identifier != PXn)
    log_error("is_valid_stm32f446re_pin", 0, "Invalid pin identifier '%s' for STM32F446RE. Expected 'PXn' format.", pin_to_string(pin));
  
  if((pin.port < 'A') || (pin.port > 'C'))
    log_error("is_valid_stm32f446re_pin", 0, "Invalid port '%c' for STM32F446RE. Valid ports: A, B, C.", pin.port);
  
  if(pin.pin_number > 15)
    log_error("is_valid_stm32f446re_pin", 0, "Invalid pin number '%u' for port '%c' on STM32F446RE. Valid range: 0-15.", pin.pin_number, pin.port);
  
  if((pin.port == 'B') && (pin.pin_number == 11))
    log_error("is_valid_stm32f446re_pin", 0, "Pin '%s' is not available on STM32F446RE.", pin_to_string(pin));
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
  bind_pwm_pins_stm32f446re(dsl_node);
  bind_pwm_prescaler_period_stm32f446re(dsl_node);
}

/**
 * @brief Binds PWM pins for STM32F446RE.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * Assigns timer numbers and channels to PWM output modules based on available options and usage.
 */
static void bind_pwm_pins_stm32f446re(ast_dsl_node_t* dsl_node){
  bool tim_used[15] = { false }; // TIM1 to TIM14
  
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
          
          log_info("bind_pwm_pins_stm32f446re", LOG_OTHER, 0, "Assigned TIM%d_CH%d (AF%d) to PWM module '%s' on pin '%s'.",
                    current_module->data.pwm.tim_number,
                    current_module->data.pwm.tim_channel,
                    current_module->data.pwm.gpio_af,
                    current_module->name,
                    pin_to_string(current_module->pin));
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