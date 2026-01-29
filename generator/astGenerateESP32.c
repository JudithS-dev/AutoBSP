#include "astGenerateESP32.h"

#include <stdio.h>

#include "logging.h"

/* -------------------------------------------- */
/*               Header functions               */
/* -------------------------------------------- */

/**
 * @brief Generates the header file content for the ESP32 board support package (BSP).
 * 
 * @param output_header File pointer to the output header file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
void ast_generate_header_esp32(FILE* output_header, ast_dsl_node_t* dsl_node){
  if(output_header == NULL)
    log_error("ast_generate_header_esp32", 0, "Output header file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("ast_generate_header_esp32", 0, "DSL node is NULL.");
  
  /*fprintf(output_header,"#ifndef __GENERATED_BSP_H__\n");
  fprintf(output_header,"#define __GENERATED_BSP_H__\n");
  
  fprintf(output_header,"\n#include <stdbool.h>");
  
  if(has_enabled_pwm_module(dsl_node) || has_enabled_uart_module(dsl_node))
    fprintf(output_header,"\n#include <stdint.h>");
  
  fprintf(output_header,"\n\nvoid BSP_Init(void);\n");
  
  generate_header_gpio_output_func(output_header, dsl_node);
  generate_header_gpio_input_func(output_header, dsl_node);
  generate_header_pwm_func(output_header, dsl_node);
  generate_header_uart_func(output_header, dsl_node);
  
  // Check for unsupported module kinds
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      switch(current_module->kind){
        case MODULE_OUTPUT:
        case MODULE_INPUT:
        case MODULE_PWM_OUTPUT:
        case MODULE_UART: break;
        default:
          log_error("ast_generate_header_stm32f446re", 0, "Unsupported module kind enum value '%d' for module '%s'", current_module->kind, current_module->name);
      }
    }
    current_module = current_module->next;
  }
  
  fprintf(output_header,"\n#endif // __GENERATED_BSP_H__");*/
}

/* -------------------------------------------- */
/*               Source functions               */
/* -------------------------------------------- */

/**
 * @brief Generates the source file content for the ESP32 board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
void ast_generate_source_esp32(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("ast_generate_source_esp32", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("ast_generate_source_esp32", 0, "DSL node is NULL.");
  
  /*fprintf(output_source,"#include \"generated_bsp.h\"\n\n");
  
  if(has_enabled_uart_module(dsl_node))
    fprintf(output_source,"#include <string.h>\n\n");
  
  fprintf(output_source,"#include \"stm32f4xx_hal.h\"\n");
  
  if(has_enabled_pwm_module(dsl_node) || has_enabled_uart_module(dsl_node)){
    fprintf(output_source,"\nextern void Error_Handler(void);\n\n");
  }
  
  // Forward declarations of initialization functions
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source,"static void BSP_Init_GPIO(void);\n");
  generate_source_pwm_init_declaration(output_source, dsl_node);
  generate_source_uart_init_declaration(output_source, dsl_node);
  
  // Handle declarations for PWM and UART modules
  generate_source_timer_handle_declaration(output_source, dsl_node);
  generate_source_uart_handle_declaration(output_source, dsl_node);
  
  fprintf(output_source,"\n\n// ---------- INITIALIZATION FUNCTIONS ----------\n\n");
  
  // Generate BSP_Init function
  generate_source_BSP_init_function(output_source, dsl_node);
  
  // Generate GPIO initialization function if needed
  if(has_enabled_gpio_module(dsl_node))
    generate_source_gpio_init_func(output_source, dsl_node);
  
  if(has_enabled_pwm_module(dsl_node))
    generate_source_pwm_init_func(output_source, dsl_node);
  
  if(has_enabled_uart_module(dsl_node))
    generate_source_uart_init_func(output_source, dsl_node);
  
  // Generate usage functions for modules
  generate_source_func(output_source, dsl_node);*/
}