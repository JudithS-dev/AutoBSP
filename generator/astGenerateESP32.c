#include "astGenerateESP32.h"

#include <stdio.h>

#include "astHelper.h"
#include "logging.h"

static void generate_source_pwm_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);
static void generate_source_uart_init_declaration(FILE* output_source, ast_dsl_node_t* dsl_node);

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
  
  fprintf(output_source, "#include \"generated_bsp.h\"\n\n");
  
  if(has_enabled_uart_module(dsl_node))
    fprintf(output_source, "#include <string.h>\n\n");
  
  fprintf(output_source, "#include \"freertos/FreeRTOS.h\"\n");
  fprintf(output_source, "#include \"freertos/task.h\"\n\n");
  
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source, "#include \"driver/gpio.h\"\n");
  if(has_enabled_pwm_module(dsl_node))
    fprintf(output_source, "#include \"driver/ledc.h\"\n");
  if(has_enabled_uart_module(dsl_node))
    fprintf(output_source, "#include \"driver/uart.h\"\n");
  
  fprintf(output_source, "\n#include \"esp_err.h\"\n\n"); // Needed for ESP_ERROR_CHECK macro
  
  // Forward declarations of initialization functions
  if(has_enabled_gpio_module(dsl_node))
    fprintf(output_source, "static void BSP_Init_GPIO(void);\n");
  generate_source_pwm_init_declaration(output_source, dsl_node);
  generate_source_uart_init_declaration(output_source, dsl_node);
  
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
    if(current_module->enable && current_module->kind == MODULE_PWM_OUTPUT)
      fprintf(output_source, "static void BSP_Init_PWM_TIM%u(void);\n", current_module->data.pwm.tim_number);
    
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
    if(current_module->enable && current_module->kind == MODULE_UART)
      fprintf(output_source, "static void BSP_Init_UART_UART%u(void);\n", current_module->data.uart.usart_number);
    
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the BSP_Init function for the ESP32 board support package (BSP).
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
    if(current_module->enable && current_module->kind == MODULE_PWM_OUTPUT)
        fprintf(output_source, "  BSP_Init_PWM_TIM%u();\n", current_module->data.pwm.tim_number);
    
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
    if(current_module->enable && current_module->kind == MODULE_UART)
        fprintf(output_source, "  BSP_Init_UART_UART%u();\n", current_module->data.uart.usart_number);
    
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the GPIO initialization function for the ESP32 board support package (BSP).
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
        fprintf(output_source, "  \n  // Configure OUTPUT GPIO pin: '%s'\n", current_module->name);
        fprintf(output_source, "  const gpio_config_t cfg_%s = {\n", current_module->name);
        fprintf(output_source, "    .pin_bit_mask = (1ULL << GPIO_NUM_%u),\n", current_module->pin.pin_number);
        fprintf(output_source, "    .mode         = ");
        if(current_module->data.output.type == GPIO_TYPE_PUSHPULL)
          fprintf(output_source, "GPIO_MODE_OUTPUT,\n");
        else if(current_module->data.output.type == GPIO_TYPE_OPENDRAIN)
          fprintf(output_source, "GPIO_MODE_OUTPUT_OD,\n");
        else
          log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO type enum value '%d' for module '%s'", current_module->data.output.type, current_module->name);
        fprintf(output_source, "    .pull_up_en   = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP_ENABLE,\n");   break;
          case GPIO_PULL_DOWN: // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLUP_DISABLE,\n");   break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
        }
        fprintf(output_source, "    .pull_down_en = ");
        switch(current_module->data.output.pull){
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN_ENABLE,\n"); break;
          case GPIO_PULL_UP:   // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLDOWN_DISABLE,\n"); break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.output.pull, current_module->name);
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
        } else if(current_module->data.output.init == GPIO_INIT_OFF){
          if(current_module->data.output.active_level == HIGH)
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 0);\n", current_module->pin.pin_number);
          else // Active_level == LOW
            fprintf(output_source, "  gpio_set_level(GPIO_NUM_%u, 1);\n", current_module->pin.pin_number);
        }
      }
      else if(current_module->kind == MODULE_INPUT){
        fprintf(output_source, "  \n  // Configure INPUT GPIO pin: '%s'\n", current_module->name);
        fprintf(output_source, "  const gpio_config_t cfg_%s = {\n", current_module->name);
        fprintf(output_source, "    .pin_bit_mask = (1ULL << GPIO_NUM_%u),\n", current_module->pin.pin_number);
        fprintf(output_source, "    .mode         = GPIO_MODE_INPUT,\n");
        fprintf(output_source, "    .pull_up_en   = ");
        switch(current_module->data.input.pull){
          case GPIO_PULL_UP:   fprintf(output_source, "GPIO_PULLUP_ENABLE,\n");   break;
          case GPIO_PULL_DOWN: // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLUP_DISABLE,\n");   break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.input.pull, current_module->name);
        }
        fprintf(output_source, "    .pull_down_en = ");
        switch(current_module->data.input.pull){
          case GPIO_PULL_DOWN: fprintf(output_source, "GPIO_PULLDOWN_ENABLE,\n"); break;
          case GPIO_PULL_UP:   // Fallthrough
          case GPIO_PULL_NONE: fprintf(output_source, "GPIO_PULLDOWN_DISABLE,\n"); break;
          default:             log_error("generate_source_gpio_init_func", 0, "Unsupported GPIO pull enum value '%d' for module '%s'", current_module->data.input.pull, current_module->name);
        }
        fprintf(output_source, "    .intr_type    = GPIO_INTR_DISABLE\n");
        fprintf(output_source, "  };\n");
        fprintf(output_source, "  ESP_ERROR_CHECK(gpio_config(&cfg_%s));\n", current_module->name);
      }
    }
    current_module = current_module->next;
  }
  
  fprintf(output_source,"}\n");
}

/**
 * @brief Generates the PWM initialization function for the ESP32 board support package (BSP).
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
      
      fprintf(output_source, "  // Configure LEDC timer TIM%u for PWM\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "  const ledc_timer_config_t cfg_timer = {\n");
      fprintf(output_source, "    .speed_mode       = LEDC_HIGH_SPEED_MODE,\n"); // always use high speed (4 high speed modes on ESP32)
      fprintf(output_source, "    .duty_resolution  = LEDC_TIMER_10_BIT,\n");    // always use 10-bit resolution, as it maps well to 0..1000 permille
      fprintf(output_source, "    .timer_num        = LEDC_TIMER_%u,\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "    .freq_hz          = %u,\n", current_module->data.pwm.frequency);
      fprintf(output_source, "    .clk_cfg          = LEDC_AUTO_CLK\n");         // always use auto: LEDC_AUTO_CLK (selects the source clock automatically)
      fprintf(output_source, "  };\n");
      fprintf(output_source, "  ESP_ERROR_CHECK(ledc_timer_config(&cfg_timer));\n  \n");
      
      fprintf(output_source, "  // Configure LEDC channel\n");
      fprintf(output_source, "  const ledc_channel_config_t cfg_channel = {\n");
      fprintf(output_source, "    .gpio_num   = GPIO_NUM_%u,\n", current_module->pin.pin_number);
      fprintf(output_source, "    .speed_mode = LEDC_HIGH_SPEED_MODE,\n");           // always use high speed (4 high speed modes on ESP32)
      fprintf(output_source, "    .channel    = LEDC_CHANNEL_%u,\n", current_module->data.pwm.tim_channel);
      fprintf(output_source, "    .timer_sel  = LEDC_TIMER_%u,\n", current_module->data.pwm.tim_number);
      fprintf(output_source, "    .duty       = 0,\n");                              // always 0 at init and set duty later
      fprintf(output_source, "    .hpoint     = 0,\n");                              // always 0 (start of the PWM period)
      fprintf(output_source, "    .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,\n"); // always LEDC_SLEEP_MODE_NO_ALIVE_NO_PD (light sleep is not supported by generator)
      if(current_module->data.pwm.active_level == HIGH)
        fprintf(output_source, "    .flags = { .output_invert = 0 }\n"); // active high
      else
        fprintf(output_source, "    .flags = { .output_invert = 1 }\n"); // active low
      fprintf(output_source, "  };\n");
      fprintf(output_source, "  ESP_ERROR_CHECK(ledc_channel_config(&cfg_channel));\n  \n");
      
      fprintf(output_source, "  // Ensure PWM is stopped initially\n");
      fprintf(output_source, "  ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u, 0));\n", current_module->data.pwm.tim_channel);
      fprintf(output_source, "  ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u));\n", current_module->data.pwm.tim_channel);
      
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
      fprintf(output_source, " * @brief Initializes the UART on UART%u for module '%s'.\n",
              current_module->data.uart.usart_number,
              current_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "static void BSP_Init_UART_UART%u(void){\n", current_module->data.uart.usart_number);
      
      fprintf(output_source, "  // Install UART%u driver\n", current_module->data.uart.usart_number);
      fprintf(output_source, "  ESP_ERROR_CHECK(uart_driver_install(");
      fprintf(output_source, "UART_NUM_%u, ", current_module->data.uart.usart_number);
      fprintf(output_source, "1024, ");     // RX buffer size
      fprintf(output_source, "1024, ");     // TX buffer size
      fprintf(output_source, "0, ");        // No event queue
      fprintf(output_source, "NULL, ");     // No event queue handle
      fprintf(output_source, "0));\n  \n"); // No interrupt allocation flags
      
      fprintf(output_source, "  // Set communication parameters\n");
      fprintf(output_source, "  const uart_config_t cfg_uart = {\n");
      fprintf(output_source, "    .baud_rate = %u,\n", current_module->data.uart.baudrate);
      fprintf(output_source, "    .data_bits = UART_DATA_%u_BITS,\n", current_module->data.uart.databits);
      fprintf(output_source, "    .parity    = ");
      switch(current_module->data.uart.parity){
        case UART_PARITY_NONE: fprintf(output_source, "UART_PARITY_DISABLE,\n"); break;
        case UART_PARITY_EVEN: fprintf(output_source, "UART_PARITY_EVEN,\n");   break;
        case UART_PARITY_ODD:  fprintf(output_source, "UART_PARITY_ODD,\n");    break;
        default:               log_error("generate_source_uart_init_func", 0, "Unsupported UART parity enum value '%d' for module '%s'", current_module->data.uart.parity, current_module->name);
      }
      fprintf(output_source, "    .stop_bits = ");
      if(current_module->data.uart.stopbits == 1.0f)
        fprintf(output_source, "UART_STOP_BITS_1,\n");
      else if(current_module->data.uart.stopbits == 1.5f)
        fprintf(output_source, "UART_STOP_BITS_1_5,\n");
      else if(current_module->data.uart.stopbits == 2.0f)
        fprintf(output_source, "UART_STOP_BITS_2,\n");
      else
        log_error("generate_source_uart_init_func", 0, "Unsupported UART stopbits value '%u' for module '%s'", current_module->data.uart.stopbits, current_module->name);
      fprintf(output_source, "    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,\n"); // always disable flow control for UART (not supported by generator)
      fprintf(output_source, "    .source_clk = UART_SCLK_DEFAULT\n");        // always use default clock
      fprintf(output_source, "  };\n");
      fprintf(output_source, "  ESP_ERROR_CHECK(uart_param_config(UART_NUM_%u, &cfg_uart));\n  \n", current_module->data.uart.usart_number);
      
      fprintf(output_source, "  // Set communication pins\n");
      fprintf(output_source, "  ESP_ERROR_CHECK(uart_set_pin(");
      fprintf(output_source, "UART_NUM_%u, ", current_module->data.uart.usart_number);
      fprintf(output_source, "GPIO_NUM_%u, ", current_module->pin.pin_number); // TX pin
      fprintf(output_source, "GPIO_NUM_%u, ", current_module->data.uart.rx_pin.pin_number); // RX pin
      fprintf(output_source, "UART_PIN_NO_CHANGE, ");    // RTS pin (not used)
      fprintf(output_source, "UART_PIN_NO_CHANGE));\n"); // CTS pin (not used)
      
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates the usage functions for the modules for the ESP32 board support package (BSP).
 * 
 * @param output_source File pointer to the output source file.
 * @param dsl_node Pointer to the DSL AST node containing configuration data.
 */
static void generate_source_func(FILE* output_source, ast_dsl_node_t* dsl_node){
  if(output_source == NULL)
    log_error("generate_source_func", 0, "Output source file pointer is NULL.");
  if(dsl_node == NULL)
    log_error("generate_source_func", 0, "DSL node is NULL.");
  
  // Generate delay function
  fprintf(output_source, "\n\n// ---------- Delay function ----------\n");
  fprintf(output_source, "/**\n");
  fprintf(output_source, " * @brief Delays execution for a specified number of milliseconds.\n");
  fprintf(output_source, " * @param ms Number of milliseconds to delay.\n");
  fprintf(output_source, " */\n");
  fprintf(output_source, "void BSP_DelayMs(uint32_t ms){\n");
  fprintf(output_source, "  vTaskDelay(pdMS_TO_TICKS(ms));\n");
  fprintf(output_source, "}\n");

  // Generate functions for each module kind
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
 * @brief Generates all source code functions for GPIO output modules for the ESP32 board support package (BSP).
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
      // Generate internal state variable (ESP32 doesn't support gpio_get_level for output pins)
      fprintf(output_source, "// Internal state variable for output module '%s'\n", output_module->name);
      fprintf(output_source, "static bool s_output_%s_is_on = %s;\n\n", output_module->name,
              (output_module->data.output.init == GPIO_INIT_ON) ? "true" : "false" );
      // Generate ON function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Turns ON the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_On(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 1 : 0);
      fprintf(output_source, "  s_output_%s_is_on = true;\n", output_module->name);
      fprintf(output_source, "}\n\n");
      
      // Generate OFF function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Turns OFF the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Off(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, %u);\n", output_module->pin.pin_number,
              (output_module->data.output.active_level == HIGH) ? 0 : 1);
      fprintf(output_source, "  s_output_%s_is_on = false;\n", output_module->name);
      fprintf(output_source, "}\n\n");
      
      // Generate TOGGLE function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Toggles the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Toggle(void){\n", output_module->name);
      fprintf(output_source, "  (void)gpio_set_level(GPIO_NUM_%u, !s_output_%s_is_on);\n", output_module->pin.pin_number, output_module->name);
      fprintf(output_source, "  s_output_%s_is_on = !s_output_%s_is_on;\n", output_module->name, output_module->name);
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
      fprintf(output_source, "  s_output_%s_is_on = on;\n", output_module->name);
      fprintf(output_source, "}\n\n");
      
      // Generate IS_ON function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Reads the current state of the '%s' GPIO output.\n", output_module->name);
      fprintf(output_source, " * @return true if the output is ON; false otherwise.\n");
      fprintf(output_source, " * @note The electrical GPIO level is derived from the configured active level.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "bool BSP_%s_IsOn(void){\n", output_module->name);
      fprintf(output_source, "  return s_output_%s_is_on;\n", output_module->name);
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/**
 * @brief Generates all source code functions for GPIO input modules for the ESP32 board support package (BSP).
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
        fprintf(output_source, "  return (gpio_get_level(GPIO_NUM_%u) == 1);\n", input_module->pin.pin_number);
      }
      else{ // active_level == LOW
        fprintf(output_source, "  return (gpio_get_level(GPIO_NUM_%u) == 0);\n", input_module->pin.pin_number);
      }
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}

/** 
 * @brief Generates all source code functions for PWM output modules for the ESP32 board support package (BSP).
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
      fprintf(output_source, "#define BSP_PWM_%s_MAX_SCALED_DUTY ((1U << 10) - 1U) // 10-bit resolution (0..1023)\n\n", pwm_module->name);
      
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
      fprintf(output_source, "    uint32_t scaled_duty = ((uint32_t)s_pwm_%s_duty_permille * BSP_PWM_%s_MAX_SCALED_DUTY + 500u) / 1000u; // Rounded calculation\n", pwm_module->name, pwm_module->name);
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u, scaled_duty));\n    \n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "    // Start PWM signal generation\n");
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u));\n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "    s_pwm_%s_running = true;\n", pwm_module->name);
      fprintf(output_source, "  }\n");
      fprintf(output_source, "}\n\n");
      
      // Generate Stop function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Stops the PWM signal generation for the '%s' module.\n", pwm_module->name);
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_Stop(void){\n", pwm_module->name);
      fprintf(output_source, "  if(s_pwm_%s_running){\n", pwm_module->name);
      fprintf(output_source, "    // Force output to inactive level by setting duty to 0\n");
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u, 0));\n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u));\n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "    s_pwm_%s_running = false;\n", pwm_module->name);
      fprintf(output_source, "  }\n");
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
      
      fprintf(output_source, "  // Only affect the PWM output if the PWM is currently running\n");
      fprintf(output_source, "  if(s_pwm_%s_running){\n", pwm_module->name);
      fprintf(output_source, "    uint32_t scaled_duty = ((uint32_t)permille * BSP_PWM_%s_MAX_SCALED_DUTY + 500u) / 1000u; // Rounded calculation\n", pwm_module->name);
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u, scaled_duty));\n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "    ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_%u));\n", pwm_module->data.pwm.tim_channel);
      fprintf(output_source, "  }\n");
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
      fprintf(output_source, "  (void)uart_write_bytes(UART_NUM_%u, (const char*)&ch, 1);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n\n");
      
      // Generate Transmit-Message function
      fprintf(output_source, "/**\n");
      fprintf(output_source, " * @brief Transmits a message over the '%s' UART module.\n", uart_module->name);
      fprintf(output_source, " * @param message Pointer to the null-terminated message string.\n");
      fprintf(output_source, " */\n");
      fprintf(output_source, "void BSP_%s_TransmitMessage(const char* message){\n", uart_module->name);
      fprintf(output_source, "  if(message == NULL)\n");
      fprintf(output_source, "    return;\n  \n");
      fprintf(output_source, "  (void)uart_write_bytes(UART_NUM_%u, message, strlen(message));\n", uart_module->data.uart.usart_number);
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
      fprintf(output_source, "  return (uart_read_bytes(UART_NUM_%u, ch, 1, portMAX_DELAY) == 1);\n", uart_module->data.uart.usart_number);
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
      fprintf(output_source, "  return (uart_read_bytes(UART_NUM_%u, ch, 1, 0) == 1);\n", uart_module->data.uart.usart_number);
      fprintf(output_source, "}\n");
    }
    current_module = current_module->next;
  }
}
