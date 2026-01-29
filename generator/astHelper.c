#include "astHelper.h"

#include "logging.h"

#include <stdlib.h>

bool value_has_enabled_gpio = false;
bool value_has_enabled_pwm = false;
bool value_has_enabled_uart = false;

bool value_set_has_enabled_gpio = false;
bool value_set_has_enabled_pwm = false;
bool value_set_has_enabled_uart = false;

/* -------------------------------------------- */
/*               Helper functions               */
/* -------------------------------------------- */

/**
 * @brief Checks if there is at least one enabled GPIO module (input or output) in the DSL node.
 * 
 * @param dsl_node Pointer to the DSL AST node.
 * @return true if there is at least one enabled GPIO module; false otherwise.
 */
bool has_enabled_gpio_module(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("has_enabled_gpio_module", 0, "DSL node is NULL.");
  
  // Use cached value if available
  if(value_set_has_enabled_gpio)
    return value_has_enabled_gpio;
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && (current_module->kind == MODULE_OUTPUT || current_module->kind == MODULE_INPUT)){
      value_has_enabled_gpio = true;
      value_set_has_enabled_gpio = true;
      return true;
    }
    current_module = current_module->next;
  }
  value_has_enabled_gpio = false;
  value_set_has_enabled_gpio = true;
  return false;
}

/**
 * @brief Checks if there is at least one enabled PWM output module in the DSL node.
 * 
 * @param dsl_node Pointer to the DSL AST node.
 * @return true if there is at least one enabled PWM output module; false otherwise.
 */
bool has_enabled_pwm_module(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("has_enabled_pwm_module", 0, "DSL node is NULL.");
  
  // Use cached value if available
  if(value_set_has_enabled_pwm)
    return value_has_enabled_pwm;
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_PWM_OUTPUT){
      value_has_enabled_pwm = true;
      value_set_has_enabled_pwm = true;
      return true;
    }
    current_module = current_module->next;
  }
  value_has_enabled_pwm = false;
  value_set_has_enabled_pwm = true;
  return false;
}

/**
 * @brief Checks if there is at least one enabled UART module in the DSL node.
 * 
 * @param dsl_node Pointer to the DSL AST node.
 * @return true if there is at least one enabled UART module; false otherwise.
 */
bool has_enabled_uart_module(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("has_enabled_uart_module", 0, "DSL node is NULL.");
  
  // Use cached value if available
  if(value_set_has_enabled_uart)
    return value_has_enabled_uart;
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_UART){
      value_has_enabled_uart = true;
      value_set_has_enabled_uart = true;
      return true;
    }
    current_module = current_module->next;
  }
  value_has_enabled_uart = false;
  value_set_has_enabled_uart = true;
  return false;
}