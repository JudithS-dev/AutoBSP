#include "astGenerateESP32.h"

#include <stdio.h>

#include "astHelper.h"
#include "logging.h"

//static void generate_source_pwm_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node); // TODO fix
//static void generate_source_uart_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);

static void generate_source_BSP_init_function(FILE* output_source, ast_dsl_node_t* dsl_node);
/*static void generate_source_pwm_init_call(FILE* output_source, ast_dsl_node_t* dsl_node); // TODO fix
static void generate_source_uart_init_call(FILE* output_source, ast_dsl_node_t* dsl_node); */

static void generate_source_gpio_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);
/*static void generate_source_pwm_init_func(FILE* output_source, ast_dsl_node_t* dsl_node); // TODO fix
static void generate_source_uart_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);*/

static void generate_source_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_gpio_output_func(FILE* output_source, ast_dsl_node_t* dsl_node);
/*static void generate_source_gpio_input_func(FILE* output_source, ast_dsl_node_t* dsl_node); // TODO fix
static void generate_source_pwm_output_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_func(FILE* output_source, ast_dsl_node_t* dsl_node); */


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
  
  fprintf(output_source,"#include \"generated_bsp.h\"\n\n");
  
  if(has_enabled_uart_module(dsl_node))
    fprintf(output_source,"#include <string.h>\n\n");
  
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source,"#include \"driver/gpio.h\"\n");
  //fprintf(output_source,"#include \"esp32_hal.h\"\n"); //TODO fix
  if(has_enabled_gpio_module(dsl_node) || has_enabled_pwm_module(dsl_node) || has_enabled_uart_module(dsl_node))
    fprintf(output_source,"\n");
  
  // Forward declarations of initialization functions
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source,"static void BSP_Init_GPIO(void);\n");
  //generate_source_pwm_init_declaration(output_source, dsl_node); // TODO fix
  //generate_source_uart_init_declaration(output_source, dsl_node);
  
  fprintf(output_source,"\n\n// ---------- INITIALIZATION FUNCTIONS ----------\n\n");
  
  // Generate BSP_Init function
  generate_source_BSP_init_function(output_source, dsl_node);
  
  // Generate GPIO initialization function if needed
  if(has_enabled_gpio_module(dsl_node))
    generate_source_gpio_init_func(output_source, dsl_node);
  
  /*if(has_enabled_pwm_module(dsl_node)) // TODO fix
    generate_source_pwm_init_func(output_source, dsl_node);*/
  
  /*if(has_enabled_uart_module(dsl_node)) // TODO fix
    generate_source_uart_init_func(output_source, dsl_node);*/
  
  // Generate usage functions for modules
  generate_source_func(output_source, dsl_node);
}

/**
 * @brief Generates the PWM initialization function declarations for enabled PWM modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
/*static void generate_source_pwm_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_pwm_init_declaration", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_pwm_init_declaration", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_PWM_OUTPUT){
        fprintf(output_source, "static void BSP_Init_PWM_TIM%u(void);\n", current_module->data.pwm.tim_number);
      }
    }
    current_module = current_module->next;
  }
}*/

/**
 * @brief Generates the UART initialization function declarations for enabled UART modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
/*static void generate_source_uart_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_uart_init_declaration", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_uart_init_declaration", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_UART){
        fprintf(output_source, "static void BSP_Init_UART_U");
        if(current_module->data.uart.is_uart)
          fprintf(output_source, "ART%u(void);\n", current_module->data.uart.usart_number);
        else
          fprintf(output_source, "SART%u(void);\n", current_module->data.uart.usart_number);
      }
    }
    current_module = current_module->next;
  }
}*/

/**
 * @brief Generates the BSP_Init function for the STM32F446RE board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_BSP_init_function(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_BSP_init_function", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_BSP_init_function", 0, "DSL node is NULL.");
  
  fprintf(output_source,"/**\n");
  fprintf(output_source," * @brief Initializes the board support package (BSP) peripherals and GPIOs.\n");
  fprintf(output_source," */\n");
  fprintf(output_source,"void BSP_Init(void){\n");
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source,"  BSP_Init_GPIO();\n");
  /*generate_source_pwm_init_call(output_source, dsl_node); // TODO fix
  generate_source_uart_init_call(output_source, dsl_node); */
  fprintf(output_source,"}\n");
}

/**
 * @brief Generates the GPIO initialization function for the STM32F446RE board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_gpio_init_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_gpio_init_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_gpio_init_func", 0, "DSL node is NULL.");
  
  fprintf(output_source,"\n/**\n");
  fprintf(output_source," * @brief Initializes the GPIO pins (inputs and outputs).\n");
  fprintf(output_source," */\n");
  fprintf(output_source,"static void BSP_Init_GPIO(void){");
  
  // Configure GPIO pins
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_OUTPUT){
        fprintf(output_source, "  \n  /* Configure OUTPUT GPIO pin: '%s' */\n", current_module->name);
        fprintf(output_source, "  gpio_config_t cfg_%s = {\n", current_module->name);
        fprintf(output_source, "    .pin_bit_mask = (1ULL << GPIO_NUM_%u),\n", current_module->pin.pin_number);
        fprintf(output_source, "    .mode         = ");
        if(current_module->data.output.type == GPIO_TYPE_PUSHPULL)
          fprintf(output_source, "GPIO_MODE_OUTPUT,\n");
        else if(current_module->data.output.type == GPIO_TYPE_OPENDRAIN)
          fprintf(output_source, "GPIO_MODE_OUTPUT_OD,\n");
        else
          log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO type enum value '%d' for module '%s'", current_module->data.output.type, current_module->name);
        fprintf(output_source, "    .pull_up_en   = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP_ENABLE,\n");   break;
          case GPIO_PULL_DOWN: // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLUP_DISABLE,\n");   break;
          default:             log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
        }
        fprintf(output_source, "    .pull_down_en = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN_ENABLE,\n"); break;
          case GPIO_PULL_UP:   // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLDOWN_DISABLE,\n"); break;
          default:             log_error("ast_generate_source_stm32f446re", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
        }
        fprintf(output_source, "    .intr_type    = GPIO_INTR_DISABLE\n");
        fprintf(output_source, "  };\n");
        fprintf(output_source, "  ESP_ERROR_CHECK(gpio_config(&cfg_%s));\n", current_module->name);
        // Set initial level if specified
        if(current_module->data.output.init == GPIO_INIT_ON){
          if(current_module->data.output.active_level == HIGH)
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 1);\n", current_module->pin.pin_number);
          else // Active_level == LOW
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 0);\n", current_module->pin.pin_number);
        }
        else if(current_module->data.output.init == GPIO_INIT_OFF){
          if(current_module->data.output.active_level == HIGH)
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 0);\n", current_module->pin.pin_number);
          else // Active_level == LOW
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 1);\n", current_module->pin.pin_number);
        }
      }
      else if(current_module->kind == MODULE_INPUT){
        fprintf(output_source, "  \n  /* Configure INPUT GPIO pin: '%s' */\n", current_module->name);
        fprintf(output_source, "  GPIO_InitStruct.Pin  = GPIO_PIN_%u;\n", current_module->pin.pin_number);
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

/**
 * @brief Generates the usage functions for the modules for the STM32F446RE board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_func", 0, "DSL node is NULL.");
  
  generate_source_gpio_output_func(output_source, dsl_node);
  /*generate_source_gpio_input_func(output_source, dsl_node); // TODO fix
  generate_source_pwm_output_func(output_source, dsl_node);
  generate_source_uart_func(output_source, dsl_node);*/
  
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
          log_error("generate_source_func", 0, "Unsupported module kind enum value '%d' for module '%s'", current_module->kind, current_module->name);
      }
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates all source code functions for GPIO output modules for the STM32F446RE board support package (BSP).
 * 
 * @param output_source Pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node.
 */
static void generate_source_gpio_output_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_gpio_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_gpio_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_OUTPUT){
      ast_module_node_t *output_module = current_module;
      // Generate functions for output GPIOs
      fprintf(output_source, "\n\n// ---------- GPIO OUTPUT: '%s' ----------\n", output_module->name);
      // Generate ON function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Turns ON the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_On(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 1 : 0);
      fprintf(output_source, "}\n\n");
      
      // Generate OFF function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Turns OFF the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Off(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 0 : 1);
      fprintf(output_source, "}\n\n");
      
      // Generate TOGGLE function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Toggles the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Toggle(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, !gpio_get_level(GPIO_NUM_%u));\n", output_module->pin.pin_number, output_module->pin.pin_number);
      fprintf(output_source, "}\n\n");
      
      // Generate SET function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Sets the '%s' GPIO output to the specified state.\n", output_module->name);
      fprintf(output_source, " * @param on If true, turns the output on; otherwise, turns it off.\n");
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Set(bool on){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, on ? %u : %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 1 : 0,
              (output_module->data.output.active_level == HIGH) ? 0 : 1);
      fprintf(output_source, "}\n\n");
      
      // Generate IS_ON function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Reads the current state of the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @return true if the output is ON; false otherwise.\n");
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_IsOn(void){\n", output_module->name);
      fprintf(output_source, "  return (gpio_get_level(GPIO_NUM_%u) == %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 1 : 0);
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}
