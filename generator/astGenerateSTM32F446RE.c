#include "astGenerateSTM32F446RE.h"

#include "logging.h"

#define STM32F446RE_MAX_PORT 'C' // Maximum port letter for STM32F446RE

static void generate_header_gpio_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_gpio_func(FILE* output_source, ast_dsl_node_t* dsl_node);

/**
 * @brief Generates the header file content for the STM32F446RE board support package (BSP).
 * 
 * @param output_header File pointer to the output header file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
void ast_generate_header_stm32f446re(FILE* output_header, ast_dsl_node_t* dsl_node){
  if(output_header == NULL)
    log_error("ast_generate_header_stm32f446re", 0, "Output header file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("ast_generate_header_stm32f446re", 0, "DSL node is NULL.");
  
  fprintf(output_header,"#ifndef __GENERATED_BSP_H__\n");
  fprintf(output_header,"#define __GENERATED_BSP_H__\n\n");
  
  fprintf(output_header,"#include <stdbool.h>\n\n");
  
  fprintf(output_header,"void BSP_Init(void);\n");
  
  generate_header_gpio_func(output_header, dsl_node);
  
  fprintf(output_header,"#endif // __GENERATED_BSP_H__");
}

static void generate_header_gpio_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_header_gpio_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_header_gpio_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_OUTPUT){
        // Generate function prototypes for output GPIOs
        fprintf(output_source, "\n/* GPIO OUTPUT: '%s' */\n", current_module->name);
        fprintf(output_source, "void BSP_%s_ON(void);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_OFF(void);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_TOGGLE(void);\n", current_module->name);
        fprintf(output_source, "void BSP_%s_SET(bool on);\n", current_module->name);
        fprintf(output_source, "bool BSP_%s_IS_ON(void);\n", current_module->name);
      }
      else if(current_module->kind == MODULE_INPUT){
        // Generate function prototypes for input GPIOs
        fprintf(output_source, "\n/* GPIO INPUT: '%s' */\n", current_module->name);
        fprintf(output_source, "bool BSP_%s_IS_ACTIVE(void);\n\n", current_module->name);
      }
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the source file content for the STM32F446RE board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
void ast_generate_source_stm32f446re(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("ast_generate_source_stm32f446re", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("ast_generate_source_stm32f446re", 0, "DSL node is NULL.");
  
  fprintf(output_source,"#include \"generated_bsp.h\"\n\n");
  
  fprintf(output_source,"#include \"stm32f4xx_hal.h\"\n\n");
  
  generate_source_init_func(output_source, dsl_node);
  generate_source_gpio_func(output_source, dsl_node);
}

/**
 * @brief Generates the source code for the BSP initialization function.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_init_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_init_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_init_func", 0, "DSL node is NULL.");
  
  fprintf(output_source,"/**\n");
  fprintf(output_source," * @brief Initializes the board support package (BSP) peripherals and GPIOs.\n");
  fprintf(output_source," * \n");
  fprintf(output_source," * This function enables the necessary GPIO clocks and configures the GPIO pins\n");
  fprintf(output_source," */\n");
  fprintf(output_source,"void BSP_Init(void){\n");
  
  // Enable all needed GPIO port clocks
  fprintf(output_source,"  /* Enable GPIO Ports Clock */\n");
  char current_port = 'A';
  while(current_port <= STM32F446RE_MAX_PORT){
    ast_module_node_t *current_module = dsl_node->modules_root;
    bool port_needed = false;
    while(current_module != NULL){
      if(current_module->kind == MODULE_OUTPUT || current_module->kind == MODULE_INPUT){
        if(current_module->enable && current_module->pin.port == current_port){
          port_needed = true;
          break;
        }
      }
      current_module = current_module->next;
    }
    if(port_needed)
      fprintf(output_source, "  __HAL_RCC_GPIO%c_CLK_ENABLE();\n", current_port);
    
    current_port++;
  }
  fprintf(output_source, "  \n");
  
  // Configure GPIO pins
  ast_module_node_t *current_module = dsl_node->modules_root;
  bool first_module = true;
  while(current_module != NULL){
    if(current_module->enable){
      if(first_module && (current_module->kind == MODULE_OUTPUT || current_module->kind == MODULE_INPUT)){
        fprintf(output_source, "  /* Configure GPIO pins with HAL_GPIO_Init() */\n");
        fprintf(output_source, "  GPIO_InitTypeDef GPIO_InitStruct = {0};\n");
        first_module = false;
      }
      if(current_module->kind == MODULE_OUTPUT){
        fprintf(output_source, "  \n  /* Configure OUTPUT GPIO pin: '%s' */\n", current_module->name);
        fprintf(output_source, "  GPIO_InitStruct.Pin = GPIO_PIN_%u;\n", current_module->pin.pin_number);
        fprintf(output_source, "  GPIO_InitStruct.Mode = ");
        switch(current_module->data.output.type){
          case GPIO_TYPE_PUSHPULL:  fprintf(output_source, "GPIO_MODE_OUTPUT_PP;\n"); break;
          case GPIO_TYPE_OPENDRAIN: fprintf(output_source, "GPIO_MODE_OUTPUT_OD;\n"); break;
          default:                  log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO type enum value '%d' for module '%s'", current_module->data.output.type, current_module->name);
        }
        fprintf(output_source, "  GPIO_InitStruct.Pull = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP;\n");   break;
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN;\n"); break;
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_NOPULL;\n");   break;
          default:             log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
        }
        fprintf(output_source, "  GPIO_InitStruct.Speed = ");
        switch(current_module->data.output.speed){
          case GPIO_SPEED_LOW:        fprintf(output_source, "GPIO_SPEED_FREQ_LOW;\n");        break;
          case GPIO_SPEED_MEDIUM:     fprintf(output_source, "GPIO_SPEED_FREQ_MEDIUM;\n");     break;
          case GPIO_SPEED_HIGH:       fprintf(output_source, "GPIO_SPEED_FREQ_HIGH;\n");       break;
          case GPIO_SPEED_VERY_HIGH:  fprintf(output_source, "GPIO_SPEED_FREQ_VERY_HIGH;\n");  break;
          default:                    log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO speed enum value '%d' for module '%s'", current_module->data.output.speed, current_module->name);
        }
        fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &GPIO_InitStruct);\n", current_module->pin.port);
        // Set initial level if specified
        if(current_module->data.output.init == GPIO_INIT_ON || current_module->data.output.init == GPIO_INIT_OFF){
          fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, ", current_module->pin.port, current_module->pin.pin_number);
          if(current_module->data.output.active_level == HIGH){
            if(current_module->data.output.init == GPIO_INIT_ON)
              fprintf(output_source, "GPIO_PIN_SET);\n");
            else
              fprintf(output_source, "GPIO_PIN_RESET);\n");
          }
          else{ // active_level == LOW
            if(current_module->data.output.init == GPIO_INIT_ON)
              fprintf(output_source, "GPIO_PIN_RESET);\n");
            else
              fprintf(output_source, "GPIO_PIN_SET);\n");
          }
        }
      }
      else if(current_module->kind == MODULE_INPUT){
        fprintf(output_source, "  \n  /*Configure INPUT GPIO pin: '%s' */\n", current_module->name);
        fprintf(output_source, "  GPIO_InitStruct.Pin = GPIO_PIN_%u;\n", current_module->pin.pin_number);
        fprintf(output_source, "  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;\n");
        fprintf(output_source, "  GPIO_InitStruct.Pull = ");
        switch(current_module->data.input.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP;\n");   break;
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN;\n"); break;
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_NOPULL;\n");   break;
          default:             log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.input.pull, current_module->name);
        }
        fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &GPIO_InitStruct);\n", current_module->pin.port);
      }
    }
    current_module = current_module->next;
  }
  
  fprintf(output_source,"}\n");
}

static void generate_source_gpio_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_gpio_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_gpio_func", 0, "DSL node is NULL.");

  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_OUTPUT){
        // Generate functions for output GPIOs
        fprintf(output_source, "\n\n// ---------- GPIO OUTPUT: '%s' ----------\n", current_module->name);
        // Generate ON function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Turns ON the '%s' GPIO output.\n", current_module->name);
        fprintf(output_source, " * @note Considers the active level configuration.\n");
        fprintf(output_source, " */\n");
        fprintf(output_source, "void BSP_%s_ON(void){\n", current_module->name);
        fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", current_module->pin.port, current_module->pin.pin_number,
                (current_module->data.output.active_level == HIGH) ? "SET" : "RESET");
        fprintf(output_source, "}\n\n");
        
        // Generate OFF function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Turns OFF the '%s' GPIO output.\n", current_module->name);
        fprintf(output_source, " * @note Considers the active level configuration.\n");
        fprintf(output_source, " */\n");
        fprintf(output_source, "void BSP_%s_OFF(void){\n", current_module->name);
        fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", current_module->pin.port, current_module->pin.pin_number,
                (current_module->data.output.active_level == HIGH) ? "RESET" : "SET");
        fprintf(output_source, "}\n\n");
        
        // Generate TOGGLE function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Toggles the '%s' GPIO output.\n", current_module->name);
        fprintf(output_source, " */\n");
        fprintf(output_source, "void BSP_%s_TOGGLE(void){\n", current_module->name);
        fprintf(output_source, "  HAL_GPIO_TogglePin(GPIO%c, GPIO_PIN_%u);\n", current_module->pin.port, current_module->pin.pin_number);
        fprintf(output_source, "}\n\n");
        
        // Generate SET function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Sets the '%s' GPIO output to the specified state.\n", current_module->name);
        fprintf(output_source, " * @param on If true, turns the output on; otherwise, turns it off.\n");
        fprintf(output_source, " * @note Considers the active level configuration.\n");
        fprintf(output_source, " */\n");
        fprintf(output_source, "void BSP_%s_SET(bool on){\n", current_module->name);
        fprintf(output_source, "  if(on){\n");
        fprintf(output_source, "    HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", current_module->pin.port, current_module->pin.pin_number,
                (current_module->data.output.active_level == HIGH) ? "SET" : "RESET");
        fprintf(output_source, "  }\n");
        fprintf(output_source, "  else{\n");
        fprintf(output_source, "    HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", current_module->pin.port, current_module->pin.pin_number,
                (current_module->data.output.active_level == HIGH) ? "RESET" : "SET");
        fprintf(output_source, "  }\n");
        fprintf(output_source, "}\n\n");
        
        // Generate IS_ON function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Reads the current state of the '%s' GPIO output.\n", current_module->name);
        fprintf(output_source, " * @return true if the output is ON; false otherwise.\n");
        fprintf(output_source, " * @note Considers the active level configuration.\n");
        fprintf(output_source, " */\n");
        fprintf(output_source, "bool BSP_%s_IS_ON(void){\n", current_module->name);
        fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_%s);\n", current_module->pin.port, current_module->pin.pin_number,
                (current_module->data.output.active_level == HIGH) ? "SET" : "RESET");
        fprintf(output_source, "}\n");
      } else if(current_module->kind == MODULE_INPUT){
        // Generate functions for input GPIOs
        fprintf(output_source, "\n\n// ---------- GPIO INPUT: '%s' ----------\n", current_module->name);
        
        // Generate IS_ACTIVE function
        fprintf(output_source, "/**\n");
        fprintf(output_source, " * @brief Checks if the '%s' GPIO input is in its active state.\n", current_module->name);
        fprintf(output_source, " * @return true if the input is active; false otherwise.\n");
        fprintf(output_source, " * @note Considers the active level configuration.\n");
        fprintf(output_source, " */\n");
        fprintf(output_source, "bool BSP_%s_IS_ACTIVE(void){\n", current_module->name);
        if(current_module->data.input.active_level == HIGH){
          fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_SET);\n", current_module->pin.port, current_module->pin.pin_number);
        }
        else{ // active_level == LOW
          fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_RESET);\n", current_module->pin.port, current_module->pin.pin_number);
        }
        fprintf(output_source, "}\n");
      } else{
        log_error("generate_source_gpio_func", 0, "Unsupported module kind enum value '%d' for module '%s'", current_module->kind, current_module->name);
      }
    }
    current_module = current_module->next;
  }
}