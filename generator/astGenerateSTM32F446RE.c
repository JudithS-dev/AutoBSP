#include "astGenerateSTM32F446RE.h"

#include "astHelper.h"
#include "logging.h"

#define STM32F446RE_MAX_PORT 'C' // Maximum port letter for STM32F446RE

static void generate_source_pwm_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);

static void generate_source_timer_handle_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_handle_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);

static void generate_source_BSP_init_function(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_pwm_init_call(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_init_call(FILE* output_source, ast_dsl_node_t* dsl_node);

static void generate_source_gpio_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_pwm_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_init_func(FILE* output_source, ast_dsl_node_t* dsl_node);

static void generate_source_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_gpio_output_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_gpio_input_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_pwm_output_func(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_func(FILE* output_source, ast_dsl_node_t* dsl_node);


/* -------------------------------------------- */
/*               Source functions               */
/* -------------------------------------------- */

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
  
  fprintf(output_source, "#include \"generated_bsp.h\"\n\n");
  
  if(has_enabled_uart_module(dsl_node))
    fprintf(output_source, "#include <string.h>\n\n");
  
  fprintf(output_source, "#include \"stm32f4xx_hal.h\"\n");
  
  if(has_enabled_pwm_module(dsl_node) || has_enabled_uart_module(dsl_node)){
    fprintf(output_source, "\nextern void Error_Handler(void);\n\n");
  }
  
  // Forward declarations of initialization functions
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source, "static void BSP_Init_GPIO(void);\n");
  generate_source_pwm_init_declaration(output_source, dsl_node);
  generate_source_uart_init_declaration(output_source, dsl_node);
  
  // Handle declarations for PWM and UART modules
  generate_source_timer_handle_declaration(output_source, dsl_node);
  generate_source_uart_handle_declaration(output_source, dsl_node);
  
  fprintf(output_source, "\n\n// ---------- INITIALIZATION FUNCTIONS ----------\n\n");
  
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
  generate_source_func(output_source, dsl_node);
}

/**
 * @brief Generates the PWM initialization function declarations for enabled PWM modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_pwm_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
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
}

/**
 * @brief Generates the UART initialization function declarations for enabled UART modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_uart_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
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
}

/**
 * @brief Generates the timer handle declarations for PWM modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_timer_handle_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_timer_handle_declaration", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_timer_handle_declaration", 0, "DSL node is NULL.");
  
  bool first_declaration = true;
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_PWM_OUTPUT){
        if(first_declaration){
          fprintf(output_source, "\n// Timer handle declarations for PWM modules\n");
          first_declaration = false;
        }
        fprintf(output_source, "static TIM_HandleTypeDef htim%u;\n", current_module->data.pwm.tim_number);
      }
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the UART handle declarations for UART modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_uart_handle_declaration(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_uart_handle_declaration", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_uart_handle_declaration", 0, "DSL node is NULL.");
  
  bool first_declaration = true;
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_UART){
        if(first_declaration){
          fprintf(output_source, "\n// UART handle declarations for UART modules\n");
          first_declaration = false;
        }
        fprintf(output_source, "static UART_HandleTypeDef huart%u;\n", current_module->data.uart.usart_number);
      }
    }
    current_module = current_module->next;
  }
}

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
  generate_source_pwm_init_call(output_source, dsl_node);
  generate_source_uart_init_call(output_source, dsl_node);
  fprintf(output_source,"}\n");
}

/**
 * @brief Generates the PWM initialization calls for enabled PWM modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_pwm_init_call(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_pwm_init_call", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_pwm_init_call", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_PWM_OUTPUT){
        fprintf(output_source, "  BSP_Init_PWM_TIM%u();\n", current_module->data.pwm.tim_number);
      }
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the UART initialization calls for enabled UART modules.
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_uart_init_call(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_uart_init_call", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_uart_init_call", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_UART){
        fprintf(output_source, "  BSP_Init_UART_U");
        if(current_module->data.uart.is_uart)
          fprintf(output_source, "ART%u();\n", current_module->data.uart.usart_number);
        else
          fprintf(output_source, "SART%u();\n", current_module->data.uart.usart_number);
      }
    }
    current_module = current_module->next;
  }
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
  fprintf(output_source,"static void BSP_Init_GPIO(void){\n");
  
  // Enable all needed GPIO port clocks
  fprintf(output_source,"  // Enable GPIO ports clock\n");
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
  while(current_module != NULL){
    if(current_module->enable){
      if(current_module->kind == MODULE_OUTPUT){
        fprintf(output_source, "  // Configure OUTPUT GPIO pin: '%s'\n", current_module->name);
        fprintf(output_source, "  GPIO_InitTypeDef cfg_%s = {\n", current_module->name);
        fprintf(output_source, "    .Pin   = GPIO_PIN_%u,\n", current_module->pin.pin_number);
        fprintf(output_source, "    .Mode  = ");
        switch(current_module->data.output.type){
          case GPIO_TYPE_PUSHPULL:  fprintf(output_source, "GPIO_MODE_OUTPUT_PP,\n"); break;
          case GPIO_TYPE_OPENDRAIN: fprintf(output_source, "GPIO_MODE_OUTPUT_OD,\n"); break;
          default:                  log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO type enum value '%d' for module '%s'", current_module->data.output.type, current_module->name);
        }
        fprintf(output_source, "    .Pull  = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP,\n");   break;
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN,\n"); break;
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_NOPULL,\n");   break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
        }
        fprintf(output_source, "    .Speed = ");
        switch(current_module->data.output.speed){
          case GPIO_SPEED_LOW:        fprintf(output_source, "GPIO_SPEED_FREQ_LOW,\n");        break;
          case GPIO_SPEED_MEDIUM:     fprintf(output_source, "GPIO_SPEED_FREQ_MEDIUM,\n");     break;
          case GPIO_SPEED_HIGH:       fprintf(output_source, "GPIO_SPEED_FREQ_HIGH,\n");       break;
          case GPIO_SPEED_VERY_HIGH:  fprintf(output_source, "GPIO_SPEED_FREQ_VERY_HIGH,\n");  break;
          default:                    log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO speed enum value '%d' for module '%s'", current_module->data.output.speed, current_module->name);
        }
        fprintf(output_source, "  };\n");
        fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &cfg_%s);\n", current_module->pin.port, current_module->name);
        // Set initial level if specified
        if(current_module->data.output.init == GPIO_INIT_ON){
          if(current_module->data.output.active_level == HIGH)
            fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_SET);\n", current_module->pin.port, current_module->pin.pin_number);
          else // active_level == LOW
            fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_RESET);\n", current_module->pin.port, current_module->pin.pin_number);
        } else if(current_module->data.output.init == GPIO_INIT_OFF){
          if(current_module->data.output.active_level == HIGH)
            fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_RESET);\n", current_module->pin.port, current_module->pin.pin_number);
          else // active_level == LOW
            fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_SET);\n", current_module->pin.port, current_module->pin.pin_number);
        }
      }
      else if(current_module->kind == MODULE_INPUT){
        fprintf(output_source, "  \n  // Configure INPUT GPIO pin: '%s'\n", current_module->name);
        fprintf(output_source, "  GPIO_InitTypeDef cfg_%s = {\n", current_module->name);
        fprintf(output_source, "    .Pin  = GPIO_PIN_%u,\n", current_module->pin.pin_number);
        fprintf(output_source, "    .Mode = GPIO_MODE_INPUT,\n");
        fprintf(output_source, "    .Pull = ");
        switch(current_module->data.input.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP,\n");   break;
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN,\n"); break;
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_NOPULL,\n");   break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.input.pull, current_module->name);
        }
        fprintf(output_source, "  };\n");
        fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &cfg_%s);\n", current_module->pin.port, current_module->name);
      }
    }
    current_module = current_module->next;
  }
  
  fprintf(output_source,"}\n");
}

/**
 * @brief Generates the PWM initialization function for the STM32F446RE board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_pwm_init_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_pwm_init_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_pwm_init_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_PWM_OUTPUT){
      // Generate PWM initialization function
      fprintf(output_source, "\n/**\n");
      fprintf(output_source, " * @brief Initializes the PWM on TIM%u for module '%s'.\n", current_module->data.pwm.tim_number, current_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "static void BSP_Init_PWM_TIM%u(void){\n", current_module->data.pwm.tim_number);
      
      fprintf(output_source, "  // Enable clocks\n");
      fprintf(output_source, "  __HAL_RCC_GPIO%c_CLK_ENABLE();\n", current_module->pin.port);
      fprintf(output_source, "  __HAL_RCC_TIM%u_CLK_ENABLE();\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  \n");
      
      fprintf(output_source, "  // Configure GPIO pin for PWM output\n");
      fprintf(output_source, "  GPIO_InitTypeDef GPIO_InitStruct = {0};\n");
      fprintf(output_source, "  GPIO_InitStruct.Pin       = GPIO_PIN_%u;\n", current_module->pin.pin_number);
      fprintf(output_source, "  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;\n");
      fprintf(output_source, "  GPIO_InitStruct.Pull      = ");
      switch(current_module->data.pwm.pull){
        case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP;\n");   break;
        case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN;\n"); break;
        case GPIO_PULL_NONE: fprintf(output_source, "GPIO_NOPULL;\n");   break;
        default:             log_error("generate_source_pwm_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.pwm.pull, current_module->name);
      }
      fprintf(output_source, "  GPIO_InitStruct.Speed     = ");
      switch(current_module->data.pwm.speed){
        case GPIO_SPEED_LOW:        fprintf(output_source, "GPIO_SPEED_FREQ_LOW;\n");        break;
        case GPIO_SPEED_MEDIUM:     fprintf(output_source, "GPIO_SPEED_FREQ_MEDIUM;\n");     break;
        case GPIO_SPEED_HIGH:       fprintf(output_source, "GPIO_SPEED_FREQ_HIGH;\n");       break;
        case GPIO_SPEED_VERY_HIGH:  fprintf(output_source, "GPIO_SPEED_FREQ_VERY_HIGH;\n");  break;
        default:                    log_error("generate_source_pwm_init_func", 0, "Unsupported GPIO speed enum value '%d' for module '%s'", current_module->data.pwm.speed, current_module->name);
      }
      fprintf(output_source, "  GPIO_InitStruct.Alternate = GPIO_AF%u_TIM%u;\n", current_module->data.pwm.gpio_af, current_module->data.pwm.tim_number);
      fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &GPIO_InitStruct);\n", current_module->pin.port);
      fprintf(output_source, "  \n");
      
      fprintf(output_source, "  // Configure TIM%u for PWM\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  htim%u.Instance               = TIM%u;\n", current_module->data.pwm.tim_number, current_module->data.pwm.tim_number);
      fprintf(output_source, "  htim%u.Init.Prescaler         = %u;\n", current_module->data.pwm.tim_number, current_module->data.pwm.prescaler);
      fprintf(output_source, "  htim%u.Init.CounterMode       = TIM_COUNTERMODE_UP;\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  htim%u.Init.Period            = %u;\n", current_module->data.pwm.tim_number, current_module->data.pwm.period);
      fprintf(output_source, "  htim%u.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  htim%u.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  if(HAL_TIM_PWM_Init(&htim%u) != HAL_OK)\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "    Error_Handler();\n");
      fprintf(output_source, "  \n");
      
      fprintf(output_source, "  // Configure PWM channel\n");
      fprintf(output_source, "  TIM_OC_InitTypeDef sConfigOC = {0};\n");
      fprintf(output_source, "  sConfigOC.OCMode        = TIM_OCMODE_PWM1;\n"); // Is always PWM1 (PWM2 would be inverted)
      fprintf(output_source, "  sConfigOC.Pulse         = 0;\n");
      fprintf(output_source, "  sConfigOC.OCPolarity    = ");
      if(current_module->data.pwm.active_level == HIGH)
        fprintf(output_source, "TIM_OCPOLARITY_HIGH;\n");
      else
        fprintf(output_source, "TIM_OCPOLARITY_LOW;\n");
      fprintf(output_source, "  sConfigOC.OCFastMode    = TIM_OCFAST_DISABLE;\n");
      fprintf(output_source, "  if(HAL_TIM_PWM_ConfigChannel(&htim%u, &sConfigOC, TIM_CHANNEL_%u) != HAL_OK)\n", current_module->data.pwm.tim_number, current_module->data.pwm.tim_channel);
      fprintf(output_source, "    Error_Handler();\n");
      fprintf(output_source, "  \n");
      
      fprintf(output_source, "  // Ensure PWM is stopped initially\n");
      fprintf(output_source, "  __HAL_TIM_SET_COMPARE(&htim%u, TIM_CHANNEL_%u, 0);\n", current_module->data.pwm.tim_number, current_module->data.pwm.tim_channel);
      
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the UART initialization functions for all enabled UART modules.
 * 
 * @param output_source The file pointer to the output source file.
 * @param dsl_node The root node of the DSL AST.
 */
static void generate_source_uart_init_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_uart_init_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_uart_init_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_UART){
      // Generate UART initialization function
      fprintf(output_source, "\n/**\n");
      fprintf(output_source, " * @brief Initializes the UART on %s%u for module '%s'.\n",
              current_module->data.uart.is_uart ? "UART" : "USART",
              current_module->data.uart.usart_number,
              current_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "static void BSP_Init_UART_U");
      if(current_module->data.uart.is_uart)
        fprintf(output_source, "ART%u(void){\n", current_module->data.uart.usart_number);
      else
        fprintf(output_source, "SART%u(void){\n", current_module->data.uart.usart_number);
      
      fprintf(output_source, "  // Enable GPIO port clock\n");
      fprintf(output_source, "  __HAL_RCC_GPIO%c_CLK_ENABLE();\n", current_module->pin.port);
      fprintf(output_source, "  \n");
      
      fprintf(output_source, "  // Configure GPIO pins for UART TX and RX\n");
      fprintf(output_source, "  GPIO_InitTypeDef GPIO_InitStruct = {0};\n");
      fprintf(output_source, "  GPIO_InitStruct.Pin       = GPIO_PIN_%u|GPIO_PIN_%u;\n", current_module->pin.pin_number, current_module->data.uart.rx_pin.pin_number);
      fprintf(output_source, "  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;\n");
      fprintf(output_source, "  GPIO_InitStruct.Pull      = GPIO_NOPULL;\n");
      fprintf(output_source, "  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;\n");
      fprintf(output_source, "  GPIO_InitStruct.Alternate = GPIO_AF%u_U", current_module->data.uart.gpio_af);
      if(current_module->data.uart.is_uart)
        fprintf(output_source, "ART%u;\n", current_module->data.uart.usart_number);
      else
        fprintf(output_source, "SART%u;\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  HAL_GPIO_Init(GPIO%c, &GPIO_InitStruct);\n  \n", current_module->pin.port);
      
      fprintf(output_source, "  // Enable UART%u peripheral clock\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  __HAL_RCC_U");
      if(current_module->data.uart.is_uart)
        fprintf(output_source, "ART%u_CLK_ENABLE();\n  \n", current_module->data.uart.usart_number);
      else
        fprintf(output_source, "SART%u_CLK_ENABLE();\n  \n", current_module->data.uart.usart_number);
      
      // UART configuration
      fprintf(output_source, "  // Configure UART%u\n", current_module->data.uart.usart_number);
      
      fprintf(output_source, "  huart%u.Instance          = U", current_module->data.uart.usart_number);
      if(current_module->data.uart.is_uart)
        fprintf(output_source, "ART%u;\n", current_module->data.uart.usart_number);
      else
        fprintf(output_source, "SART%u;\n", current_module->data.uart.usart_number);
      
      fprintf(output_source, "  huart%u.Init.BaudRate     = %u;\n", current_module->data.uart.usart_number, current_module->data.uart.baudrate);
      
      if(current_module->data.uart.databits == 8)
        fprintf(output_source, "  huart%u.Init.WordLength   = UART_WORDLENGTH_8B;\n", current_module->data.uart.usart_number);
      else if(current_module->data.uart.databits == 9)
        fprintf(output_source, "  huart%u.Init.WordLength   = UART_WORDLENGTH_9B;\n", current_module->data.uart.usart_number);
      else
        log_error("generate_source_uart_init_func", 0, "Unsupported databits value '%u' for UART module '%s'", current_module->data.uart.databits, current_module->name);
      
      if(current_module->data.uart.stopbits == 1.0f)
        fprintf(output_source, "  huart%u.Init.StopBits     = UART_STOPBITS_1;\n", current_module->data.uart.usart_number);
      else if(current_module->data.uart.stopbits == 2.0f)
        fprintf(output_source, "  huart%u.Init.StopBits     = UART_STOPBITS_2;\n", current_module->data.uart.usart_number);
      else
        log_error("generate_source_uart_init_func", 0, "Unsupported stopbits value '%u' for UART module '%s'", current_module->data.uart.stopbits, current_module->name);
      
      if(current_module->data.uart.parity == UART_PARITY_NONE)
        fprintf(output_source, "  huart%u.Init.Parity       = UART_PARITY_NONE;\n", current_module->data.uart.usart_number);
      else if(current_module->data.uart.parity == UART_PARITY_EVEN)
        fprintf(output_source, "  huart%u.Init.Parity       = UART_PARITY_EVEN;\n", current_module->data.uart.usart_number);
      else if(current_module->data.uart.parity == UART_PARITY_ODD)
        fprintf(output_source, "  huart%u.Init.Parity       = UART_PARITY_ODD;\n", current_module->data.uart.usart_number);
      else
        log_error("generate_source_uart_init_func", 0, "Unsupported parity enum value '%d' for UART module '%s'", current_module->data.uart.parity, current_module->name);
      
      fprintf(output_source, "  huart%u.Init.Mode         = UART_MODE_TX_RX;\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  huart%u.Init.HwFlowCtl    = UART_HWCONTROL_NONE;\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  huart%u.Init.OverSampling = UART_OVERSAMPLING_16;\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  if(HAL_UART_Init(&huart%u) != HAL_OK)\n", current_module->data.uart.usart_number);
      fprintf(output_source, "    Error_Handler();\n");
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
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
  generate_source_gpio_input_func(output_source, dsl_node);
  generate_source_pwm_output_func(output_source, dsl_node);
  generate_source_uart_func(output_source, dsl_node);
  
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
      fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", output_module->pin.port, output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? "SET" : "RESET");
      fprintf(output_source, "}\n\n");
      
      // Generate OFF function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Turns OFF the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Off(void){\n", output_module->name);
      fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, GPIO_PIN_%s);\n", output_module->pin.port, output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? "RESET" : "SET");
      fprintf(output_source, "}\n\n");
      
      // Generate TOGGLE function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Toggles the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Toggle(void){\n", output_module->name);
      fprintf(output_source, "  HAL_GPIO_TogglePin(GPIO%c, GPIO_PIN_%u);\n", output_module->pin.port, output_module->pin.pin_number);
      fprintf(output_source, "}\n\n");
      
      // Generate SET function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Sets the '%s' GPIO output to the specified state.\n", output_module->name);
      fprintf(output_source, " * @param on If true, turns the output on; otherwise, turns it off.\n");
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Set(bool on){\n", output_module->name);
      fprintf(output_source, "  HAL_GPIO_WritePin(GPIO%c, GPIO_PIN_%u, on ? GPIO_PIN_%s : GPIO_PIN_%s);\n", output_module->pin.port, output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? "SET" : "RESET",
              (output_module->data.output.active_level == HIGH) ? "RESET" : "SET");
      fprintf(output_source, "}\n\n");
      
      // Generate IS_ON function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Reads the current state of the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @return true if the output is ON; false otherwise.\n");
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_IsOn(void){\n", output_module->name);
      fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_%s);\n", output_module->pin.port, output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? "SET" : "RESET");
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates all source code functions for GPIO input modules for the STM32F446RE board support package (BSP).
 * 
 * @param output_source Pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node.
 */
static void generate_source_gpio_input_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_gpio_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_gpio_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_INPUT){
      ast_module_node_t *input_module = current_module;
      // Generate functions for input GPIOs
      fprintf(output_source, "\n\n// ---------- GPIO INPUT: '%s' ----------\n", input_module->name);
      
      // Generate IS_ACTIVE function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Checks if the '%s' GPIO input is in its active state.\n", input_module->name);
      fprintf(output_source, " * @return true if the input is active; false otherwise.\n");
      fprintf(output_source, " * @note Considers the active level configuration.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_IsActive(void){\n", input_module->name);
      if(input_module->data.input.active_level == HIGH){
        fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_SET);\n", input_module->pin.port, input_module->pin.pin_number);
      }
      else{ // active_level == LOW
        fprintf(output_source, "  return (HAL_GPIO_ReadPin(GPIO%c, GPIO_PIN_%u) == GPIO_PIN_RESET);\n", input_module->pin.port, input_module->pin.pin_number);
      }
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/** 
 * @brief Generates all source code functions for PWM output modules for the STM32F446RE board support package (BSP).
 * 
 * @param output_source Pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node.
 */
static void generate_source_pwm_output_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_pwm_output_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_pwm_output_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_PWM_OUTPUT){
      ast_module_node_t *pwm_module = current_module;
      // Generate functions for PWM output modules
      fprintf(output_source, "\n\n// ---------- PWM OUTPUT: '%s' ----------\n", pwm_module->name);
      
      // Generate needed variables
      fprintf(output_source, "// Internal state for PWM module '%s'\n", pwm_module->name);
      fprintf(output_source, "static bool s_pwm_%s_running = false;\n", pwm_module->name);
      fprintf(output_source, "static uint16_t s_pwm_%s_duty_permille = %d; // Duty cycle in permille (0..1000)\n\n", pwm_module->name, pwm_module->data.pwm.duty_cycle);
      
      // Generate Start function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Starts the PWM signal generation for the '%s' module.\n", pwm_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Start(void){\n", pwm_module->name);
      fprintf(output_source, "  if(!s_pwm_%s_running){\n", pwm_module->name);
      fprintf(output_source, "    // Ensure the last set duty cycle is applied before starting\n");
      fprintf(output_source, "    BSP_%s_SetDuty(s_pwm_%s_duty_permille);\n    \n", pwm_module->name, pwm_module->name);
      fprintf(output_source, "    // Start PWM signal generation\n");
      fprintf(output_source, "    if(HAL_TIM_PWM_Start(&htim%u, TIM_CHANNEL_%u) != HAL_OK)\n", pwm_module->data.pwm.tim_number, pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "      Error_Handler();\n");
      fprintf(output_source, "    s_pwm_%s_running = true;\n", pwm_module->name);
      fprintf(output_source, "  }\n");
      fprintf(output_source, "}\n\n");
      
      // Generate Stop function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Stops the PWM signal generation for the '%s' module.\n", pwm_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Stop(void){\n", pwm_module->name);
      fprintf(output_source, "  if(s_pwm_%s_running){\n", pwm_module->name);
      fprintf(output_source, "    if(HAL_TIM_PWM_Stop(&htim%u, TIM_CHANNEL_%u) != HAL_OK)\n", pwm_module->data.pwm.tim_number, pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "      Error_Handler();\n");
      fprintf(output_source, "    s_pwm_%s_running = false;\n", pwm_module->name);
      fprintf(output_source, "  }\n  \n");
      fprintf(output_source, "  // Force output to inactive level\n");
      fprintf(output_source, "  __HAL_TIM_SET_COMPARE(&htim%u, TIM_CHANNEL_%u, 0);\n", pwm_module->data.pwm.tim_number, pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "}\n\n");
      
      // Generate SetDuty function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Sets the duty cycle for the '%s' PWM output.\n", pwm_module->name);
      fprintf(output_source, " * @param permille Duty cycle in permille (0..1000).\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_SetDuty(uint16_t permille){\n", pwm_module->name);
      fprintf(output_source, "  if(permille > 1000)\n");
      fprintf(output_source, "    permille = 1000;\n  \n");
      fprintf(output_source, "  s_pwm_%s_duty_permille = permille;\n  \n", pwm_module->name);
      if(pwm_module->data.pwm.active_level == LOW){
        fprintf(output_source, "  // Invert duty cycle for active LOW configuration\n");
        fprintf(output_source, "  permille = 1000u - permille;\n  \n");
      }
      fprintf(output_source, "  // ARR is the PWM top value\n");
      fprintf(output_source, "  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim%u);\n  \n", pwm_module->data.pwm.tim_number);
      fprintf(output_source, "  // Convert 0..1000 permille to timer compare value\n");
      fprintf(output_source, "  uint32_t crr = (arr * (uint32_t)permille + 500u) / 1000u; // Rounded calculation\n  \n");
      fprintf(output_source, "  if(crr > arr) crr = arr;\n  \n");
      fprintf(output_source, "  // Set the compare register to update duty cycle\n");
      fprintf(output_source, "  __HAL_TIM_SET_COMPARE(&htim%u, TIM_CHANNEL_%u, crr);\n", pwm_module->data.pwm.tim_number, pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "}\n\n");
      
      // Generate GetDuty function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Gets the current duty cycle for the '%s' PWM output.\n", pwm_module->name);
      fprintf(output_source, " * @return Duty cycle in permille (0..1000).\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "uint16_t BSP_%s_GetDuty(void){\n", pwm_module->name);
      fprintf(output_source, "  return s_pwm_%s_duty_permille;\n", pwm_module->name);
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

static void generate_source_uart_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_uart_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_uart_func", 0, "DSL node is NULL.");
  
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    if(current_module->enable && current_module->kind == MODULE_UART){
      ast_module_node_t *uart_module = current_module;
      // Generate functions for UART modules
      fprintf(output_source, "\n\n// ---------- UART: '%s' ----------\n", uart_module->name);
      
      // Generate Transmit-Char function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Transmits single character over the '%s' UART module.\n", uart_module->name);
      fprintf(output_source, " * @param ch Byte to transmit.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_TransmitChar(uint8_t ch){\n", uart_module->name);
      fprintf(output_source, "  HAL_UART_Transmit(&huart%u, &ch, 1, HAL_MAX_DELAY);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n\n");
      
      // Generate Transmit-Message function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Transmits a message over the '%s' UART module.\n", uart_module->name);
      fprintf(output_source, " * @param message Pointer to the null-terminated message string.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_TransmitMessage(const char* message){\n", uart_module->name);
      fprintf(output_source, "  if(message == NULL)\n");
      fprintf(output_source, "    return;\n  \n");
      fprintf(output_source, "  HAL_UART_Transmit(&huart%u, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n\n");
      
      // Generate Receive-Char function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Receives a single character from the '%s' UART module.\n", uart_module->name);
      fprintf(output_source, " * @param ch Pointer to the variable to store the received byte.\n");
      fprintf(output_source, " * @return true if a character was successfully received; false otherwise.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_ReceiveChar(uint8_t* ch){\n", uart_module->name);
      fprintf(output_source, "  if(ch == NULL)\n");
      fprintf(output_source, "    return false;\n  \n");
      fprintf(output_source, "  return (HAL_UART_Receive(&huart%u, ch, 1, HAL_MAX_DELAY) == HAL_OK);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n");
      
      // Generate Try-Receive-Char function
      fprintf(output_source, "\n/**\n");
      fprintf(output_source, " * @brief Tries to receive a single character from the '%s' UART module without blocking.\n", uart_module->name);
      fprintf(output_source, " * @param ch Pointer to the variable to store the received byte.\n");
      fprintf(output_source, " * @return true if a character was successfully received; false otherwise.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_TryReceiveChar(uint8_t* ch){\n", uart_module->name);
      fprintf(output_source, "  if(ch == NULL)\n");
      fprintf(output_source, "    return false;\n  \n");
      fprintf(output_source, "  return (HAL_UART_Receive(&huart%u, ch, 1, 0) == HAL_OK);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}
