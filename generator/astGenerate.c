#include "astGenerate.h"

#include <stdio.h>

#include "astGenerateSTM32F446RE.h"
#include "astGenerateESP32.h"
#include "astHelper.h"
#include "logging.h"

static void generate_header(FILE* output_header, ast_dsl_node_t* dsl_node);
static void generate_header_gpio_output_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_header_gpio_input_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_header_pwm_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_header_uart_func(FILE* output_source, ast_dsl_node_t* dsl_node);

/* -------------------------------------------- */
/*           File management functions          */
/* -------------------------------------------- */

/**
 * @brief Generates board support package (BSP) code files based on the provided DSL AST node.
 * 
 * @param output_path Path to the directory where the generated code files will be created.
 * @param dsl_node Pointer to the DSL AST node from which code will be generated.
 * 
 * @note If dsl_node is NULL, an error is logged.
 * @note If output_path is NULL, an error is logged.
 */
void ast_generate_code(const char* output_path, ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_generate_code", 0, "DSL node is NULL.");
  if(output_path == NULL)
    log_error("ast_generate_code", 0, "Output path is NULL.");
  
  // Generate header file
  char header_path[256];
  snprintf(header_path, sizeof(header_path), "%s/generated_bsp.h", output_path);
  
  FILE* output_header = fopen(header_path, "w");
  if(output_header == NULL)
    log_error("ast_generate_code", 0, "Failed to open header file for writing: '%s'", header_path);
  
  generate_header(output_header, dsl_node);
  
  fclose(output_header);
  
  
  // Generate source file
  char source_path[256];
  snprintf(source_path, sizeof(source_path), "%s/generated_bsp.c", output_path);
  
  FILE* output_source = fopen(source_path, "w");
  if(output_source == NULL)
    log_error("ast_generate_code", 0, "Failed to open source file for writing: '%s'", source_path);
  
  switch(dsl_node->controller){ // Dispatch to controller-specific source code generator
    case STM32F446RE: ast_generate_source_stm32f446re(output_source, dsl_node); break;
    case ESP32:       ast_generate_source_esp32(output_source, dsl_node); break;
    default:          log_error("ast_generate_code", 0, "Unsupported controller type enum value '%d'", dsl_node->controller);
  }
  
  fclose(output_source);
}


/* -------------------------------------------- */
/*               Header functions               */
/* -------------------------------------------- */

/**
 * @brief Generates the header file content (Is the same for all controllers).
 * 
 * @param output_header File pointer to the output header file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_header(FILE* output_header, ast_dsl_node_t* dsl_node){
  if(output_header == NULL)
    log_error("generate_header", 0, "Output header file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header", 0, "DSL node is NULL.");
  
  fprintf(output_header,"#ifndef __GENERATED_BSP_H__\n");
  fprintf(output_header,"#define __GENERATED_BSP_H__\n");
  
  fprintf(output_header,"\n#include <stdbool.h>");
  fprintf(output_header,"\n#include <stdint.h>");
  
  fprintf(output_header,"\n\n// Initialization function\n");
  fprintf(output_header,"void BSP_Init(void);\n\n");
  fprintf(output_header,"// Delay function\n");
  fprintf(output_header,"void BSP_DelayMs(uint32_t ms);\n");
  
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
          log_error("generate_header", 0, "Unsupported module kind enum value '%d' for module '%s'", current_module->kind, current_module->name);
      }
    }
    current_module = current_module->next;
  }
  
  fprintf(output_header,"\n#endif // __GENERATED_BSP_H__");
}

/**
 * @brief Generates the header file content for GPIO output functions.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_header_gpio_output_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_header_gpio_output_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header_gpio_output_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_OUTPUT){
      // Generate function prototypes for output GPIOs
      fprintf(output_source, "\n// GPIO OUTPUT: '%s'\n", current_module->name);
      fprintf(output_source, "void BSP_%s_On(void);\n", current_module->name);
      fprintf(output_source, "void BSP_%s_Off(void);\n", current_module->name);
      fprintf(output_source, "void BSP_%s_Toggle(void);\n", current_module->name);
      fprintf(output_source, "void BSP_%s_Set(bool on);\n", current_module->name);
      fprintf(output_source, "bool BSP_%s_IsOn(void);\n", current_module->name);
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the header file content for GPIO input functions.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_header_gpio_input_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_header_gpio_input_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header_gpio_input_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_INPUT){
      // Generate function prototypes for input GPIOs
      fprintf(output_source, "\n// GPIO INPUT: '%s'\n", current_module->name);
      fprintf(output_source, "bool BSP_%s_IsActive(void);\n", current_module->name);
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the header file content for PWM functions.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_header_pwm_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_header_pwm_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header_pwm_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_PWM_OUTPUT){
        // Generate function prototypes for PWM output modules
        fprintf(output_source, "\n// PWM OUTPUT: '%s'\n", current_module->name);
        fprintf(output_source, "void BSP_%s_Start(void);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_Stop(void);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_SetDuty(uint16_t permille);\n", current_module->name);
        fprintf(output_source, "uint16_t BSP_%s_GetDuty(void);\n", current_module->name);
      }
    }
    current_module = current_module->next;
  }
}

static void generate_header_uart_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_header_uart_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header_uart_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_UART){
        // Generate function prototypes for UART modules
        fprintf(output_source, "\n// UART: '%s'\n", current_module->name);
        fprintf(output_source, "void BSP_%s_TransmitChar(uint8_t ch);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_TransmitMessage(const char *msg);\n", current_module->name);
        fprintf(output_source, "bool BSP_%s_ReceiveChar(uint8_t *ch);\n", current_module->name);
        fprintf(output_source, "bool BSP_%s_TryReceiveChar(uint8_t *ch);\n", current_module->name);
      }
    }
    current_module = current_module->next;
  }
}