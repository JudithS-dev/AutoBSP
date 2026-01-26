#include "astBuild.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "astCheck.h"
#include "astEnums2Str.h"

unsigned int global_ast_node_counter = 0;

static void ast_dsl_node_append_module_node(ast_dsl_node_t* dsl_node, ast_module_node_t* module_node);

static void ast_initialize_module(ast_module_node_t* module);


/* -------------------------------------------- */
/*           Constructors of builders           */
/* -------------------------------------------- */

/**
 * @brief Creates a new AST DSL builder and initializes its fields.
 * 
 * @return Pointer to the newly created AST DSL builder.
 */
ast_dsl_builder_t* ast_new_dsl_builder(){
  ast_dsl_builder_t* dsl_builder = (ast_dsl_builder_t*)calloc(1, sizeof(ast_dsl_builder_t));
  if(dsl_builder == NULL)
    log_error("ast_new_dsl_builder", 0, "Could not allocate memory for new DSL builder.");
  
  // Explicitly initialize fields
  dsl_builder->controller_set = false;
  dsl_builder->module_builders_root   = NULL;
  return dsl_builder;
}

/**
 * @brief Creates a new AST module builder and initializes its fields.
 * 
 * @param lineNr Line number where the module is defined.
 * @return Pointer to the newly created AST module builder.
 */
ast_module_builder_t* ast_new_module_builder(int line_nr){
  ast_module_builder_t *module_builder = (ast_module_builder_t*)calloc(1, sizeof(ast_module_builder_t));
  if(module_builder == NULL)
    log_error("ast_new_module_builder", 0, "Could not allocate memory for new AST module builder.");
  
  // Explicitly initialize all fields of builder to default values
  module_builder->name_set         = false;
  module_builder->pin_set          = false;
  module_builder->enable_set       = false;
  module_builder->kind_set         = false;
  module_builder->type_set         = false;
  module_builder->pull_set         = false;
  module_builder->speed_set        = false;
  module_builder->init_set         = false;
  module_builder->active_level_set = false;
  module_builder->frequency_set    = false;
  module_builder->duty_cycle_set   = false;
  module_builder->rx_pin_set       = false;
  module_builder->baudrate_set     = false;
  module_builder->databits_set     = false;
  module_builder->stopbits_set     = false;
  module_builder->parity_set       = false;
  module_builder->next             = NULL;
  
  
  module_builder->module = (ast_module_node_t*)calloc(1, sizeof(ast_module_node_t));
  if(module_builder->module == NULL)
    log_error("ast_new_module_builder", 0, "Could not allocate memory for new AST module node.");
  
  // Explicitly initialize fields of the current module node
  module_builder->module->node_id = global_ast_node_counter++;
  module_builder->module->line_nr = line_nr;
  module_builder->module->name  = NULL;
  module_builder->module->next  = NULL;
  return module_builder;
}


/* -------------------------------------------- */
/*     Conversion from builder to AST nodes     */
/* -------------------------------------------- */

/**
 * @brief Converts the AST DSL builder to an AST DSL node and all its modules.
 * 
 * @param dsl_builder Pointer to the AST DSL builder to convert.
 * @return Pointer to the newly created AST DSL node.
 * 
 * @note Frees the DSL builder and its module builders after conversion.
 */
ast_dsl_node_t *ast_convert_dsl_builder_to_dsl_node(ast_dsl_builder_t* dsl_builder){
  if(dsl_builder == NULL)
    log_error("ast_convert_dsl_builder_to_dsl_node", 0, "DSL builder is NULL.");
  
  // Create DSL node
  ast_dsl_node_t* dsl_node = (ast_dsl_node_t*)calloc(1, sizeof(ast_dsl_node_t));
  if(dsl_node == NULL)
    log_error("ast_convert_dsl_builder_to_dsl_node", 0, "Could not allocate memory for DSL node.");
  
  // Set DSL node fields from builder
  dsl_node->controller = dsl_builder->controller;
  dsl_node->modules_root = NULL;
  
  // Convert each module builder to module node and append to DSL node
  ast_module_builder_t *current_builder, *next_builder;
  ast_module_node_t *current_module;
  
  current_builder = dsl_builder->module_builders_root;
  while(current_builder != NULL){
    current_module = current_builder->module;
    current_builder->module = NULL; // Prevent double free
    
    ast_dsl_node_append_module_node(dsl_node, current_module);
    
    next_builder = current_builder->next;
    free(current_builder);
    current_builder = next_builder;
  }
  
  // Free the DSL builder itself
  free(dsl_builder);
  
  return dsl_node;
}

static void ast_dsl_node_append_module_node(ast_dsl_node_t* dsl_node, ast_module_node_t* module_node){
  if(dsl_node == NULL)
    log_error("ast_dsl_node_append_module_node", 0, "DSL node is NULL.");
  if(module_node == NULL)
    log_error("ast_dsl_node_append_module_node", 0, "Module node is NULL.");
  
  // Append module to the end of the linked list
  if(dsl_node->modules_root == NULL){
    dsl_node->modules_root = module_node;
  } else{
    ast_module_node_t* current = dsl_node->modules_root;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = module_node;
  }
}

void ast_free_dsl_node(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_free_dsl_node", 0, "DSL node is NULL.");
  
  // Free all module nodes
  ast_module_node_t* current_module = dsl_node->modules_root;
  ast_module_node_t* next_module;
  
  while(current_module != NULL){
    next_module = current_module->next;
    
    // Free module name
    if(current_module->name != NULL)
      free(current_module->name);
    
    // Free the module node itself
    free(current_module);
    
    current_module = next_module;
  }
  
  // Free the DSL node itself
  free(dsl_node);
}


/* -------------------------------------------- */
/*             DSL builder functions            */
/* -------------------------------------------- */
/**
 * @brief Sets the controller of the DSL builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param dsl_builder Pointer to the DSL builder.
 * @param controller Controller to set for the DSL.
 * 
 * @note Logs an error and exits if the controller has already been set or if any parameter is NULL.
 */
void ast_dsl_builder_set_controller(int line_nr, ast_dsl_builder_t* dsl_builder, controller_t controller){
  if(dsl_builder == NULL)
    log_error("ast_dsl_builder_set_controller", 0, "DSL builder is NULL.");
  
  if(dsl_builder->controller_set)
    log_error("ast_dsl_builder_set_controller", line_nr, "Trying to set controller to '%s'.\n"
              "                                                 But controller has already been set to '%s'.",
              controller_to_string(controller),
              controller_to_string(dsl_builder->controller));
  
  dsl_builder->controller = controller;
  dsl_builder->controller_set = true;
}

/**
 * @brief Appends a module builder to the DSL builder's module builder list.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param dsl_builder Pointer to the DSL builder.
 * @param module_builder Pointer to the module builder to append.
 * 
 * @note Logs an error and exits if any parameter is NULL.
 */
void ast_dsl_builder_append_module_builder( int line_nr, ast_dsl_builder_t* dsl_builder, ast_module_builder_t* module_builder){
  if(dsl_builder == NULL)
    log_error("ast_dsl_builder_append_module_builder", line_nr, "DSL builder is NULL.");
  if(module_builder == NULL)
    log_error("ast_dsl_builder_append_module_builder", line_nr, "Module builder is NULL.");
  
  // Append module builder to the end of the linked list
  if(dsl_builder->module_builders_root == NULL){
    dsl_builder->module_builders_root = module_builder;
  } else{
    ast_module_builder_t* current = dsl_builder->module_builders_root;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = module_builder;
  }
}


/* -------------------------------------------- */
/*         Module builder common setters        */
/* -------------------------------------------- */

/**
 * @brief Sets the name of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param name Name to set for the module.
 * 
 * @note Logs an error and exits if the name has already been set or if any parameter is NULL.
 * @note The name is duplicated to ensure it is stored correctly in the module node.
 */
void ast_module_builder_set_name(int line_nr, ast_module_builder_t* module_builder, const char* name){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_name", 0, "AST module builder is NULL.");
  if(name == NULL)
    log_error("ast_module_builder_set_name", 0, "Module name is NULL.");
  
  if(module_builder->name_set)
    log_error("ast_module_builder_set_name", line_nr, "Trying to set module name to '%s'.\n"
              "                                              But module name has already been set to '%s'.",
              name,
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  
  module_builder->module->name = strdup(name);
  if(module_builder->module->name == NULL)
    log_error("ast_module_builder_set_name", 0, "Could not allocate memory for module name.");
  module_builder->name_set = true;
}

/**
 * @brief Sets the pin of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param pin Pin to set for the module.
 * 
 * @note Logs an error and exits if the pin has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_pin(int line_nr, ast_module_builder_t* module_builder, pin_t pin){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pin", 0, "AST module builder is NULL.");
  
  if(module_builder->pin_set)
    log_error("ast_module_builder_set_pin", line_nr, "Trying to set module pin of module '%s' to '%s'.\n"
              "                                             But module pin has already been set to '%s'.",
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              pin_to_string(pin),
              pin_to_string(module_builder->module->pin));
  
  module_builder->module->pin = pin;
  module_builder->pin_set = true;
}

/**
 * @brief Sets the enable flag of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param enable Enable flag to set for the module.
 * 
 * @note Logs an error and exits if the enable flag has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_enable(int line_nr, ast_module_builder_t* module_builder, bool enable){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_enable", 0, "AST module builder is NULL.");
  
  if(module_builder->enable_set)
    log_error("ast_module_builder_set_enable", line_nr, "Trying to set module enable of module '%s' to '%s'.\n"
              "                                                But module enable has already been set to '%s'.",
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              bool_to_string(enable),
              bool_to_string(module_builder->module->enable));
  
  module_builder->module->enable = enable;
  module_builder->enable_set = true;
}

/**
 * @brief Sets the kind of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param kind Kind to set for the module.
 * 
 * @note Logs an error and exits if the kind has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_kind(int line_nr, ast_module_builder_t* module_builder, ast_module_kind_t kind){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_kind", 0, "AST module builder is NULL.");
  
  if(module_builder->kind_set)
    log_error("ast_module_builder_set_kind", line_nr, "Trying to set module kind of module '%s' to '%s'.\n"
              "                                              But module kind has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              kind_to_string(kind),
              kind_to_string(module_builder->module->kind));
  
  module_builder->module->kind = kind;
  module_builder->kind_set = true;
  
  // Initialize the union data based on the kind
  ast_initialize_module(module_builder->module);
}

/**
 * @brief Initializes the module node's union data based on its kind.
 * 
 * @param module Pointer to the module node to initialize.
 * 
 * @note Logs an error and exits if the module kind is unknown or if the module pointer is NULL.
 */
static void ast_initialize_module(ast_module_node_t* module){
  if(module == NULL)
    log_error("ast_initialize_module", 0, "Module node is NULL.");
  
  module->enable = true; // Default to enabled
  switch(module->kind){
    case MODULE_OUTPUT:  // Initialize output-specific fields to default values
                        module->data.output.type         = GPIO_TYPE_PUSHPULL;
                        module->data.output.pull         = GPIO_PULL_NONE;
                        module->data.output.speed        = GPIO_SPEED_MEDIUM;
                        module->data.output.init         = GPIO_INIT_OFF;
                        module->data.output.active_level = HIGH;
                        break;
    case MODULE_INPUT:  // Initialize input-specific fields to default values
                        module->data.input.pull         = GPIO_PULL_NONE;
                        module->data.input.active_level = HIGH;
                        break;
    case MODULE_PWM_OUTPUT: // Initialize PWM-specific fields to default values
                        module->data.pwm.pull          = GPIO_PULL_NONE;
                        module->data.pwm.speed         = GPIO_SPEED_MEDIUM;
                        module->data.pwm.active_level  = HIGH;
                        module->data.pwm.frequency     = 1000; // Default frequency 1kHz
                        module->data.pwm.duty_cycle    = 50;   // Default duty cycle 50%
                        module->data.pwm.tim_number    = 0;
                        module->data.pwm.tim_channel   = 0;
                        module->data.pwm.gpio_af       = 0;
                        module->data.pwm.prescaler     = 0;
                        module->data.pwm.period        = 0;
                        break;
    case MODULE_UART:   // Initialize UART-specific fields to default values
                        module->data.uart.baudrate     = 115200; // Default baudrate 115200
                        module->data.uart.databits     = 8;      // Default databits
                        module->data.uart.stopbits     = 1;      // Default stopbits
                        module->data.uart.parity       = UART_PARITY_NONE;
                        module->data.uart.usart_number = 0;
                        module->data.uart.is_uart      = false;
                        module->data.uart.gpio_af      = 0;
                        break;
    default:
      log_error("ast_initialize_module", 0, "Unknown module kind for module '%s'.", 
                module->name == NULL ? "<NULL>" : module->name);
  }
}


/* -------------------------------------------- */
/*    Module builder output specific setters    */
/* -------------------------------------------- */

/**
 * @brief Sets the output type of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param type Output type to set for the output module.
 * 
 * @note Logs an error and exits if the type has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_type(int line_nr, ast_module_builder_t* module_builder, gpio_type_t type){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_output_type", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_type", line_nr, "Cannot set output type for non-output module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->type_set)
    log_error("ast_module_builder_set_output_type", line_nr, "Trying to set output type of module '%s' to '%s'.\n"
              "                                                     But output type has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_type_to_string(type),
              gpio_type_to_string(module_builder->module->data.output.type));
  
  module_builder->module->data.output.type = type;
  module_builder->type_set = true;
}

/**
 * @brief Sets the output pull of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param pull Output pull to set for the output module.
 * 
 * @note Logs an error and exits if the pull has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_pull(int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_output_pull", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_pull", line_nr, "Cannot set output pull for non-output module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->pull_set)
    log_error("ast_module_builder_set_output_pull", line_nr, "Trying to set output pull of module '%s' to '%s'.\n"
              "                                                     But output pull has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_pull_to_string(pull),
              gpio_pull_to_string(module_builder->module->data.output.pull));
  
  module_builder->module->data.output.pull = pull;
  module_builder->pull_set = true;
}

/**
 * @brief Sets the output speed of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param speed Output speed to set for the output module.
 * 
 * @note Logs an error and exits if the speed has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_speed(int line_nr, ast_module_builder_t* module_builder, gpio_speed_t speed){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_output_speed", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_speed", line_nr, "Cannot set output speed for non-output module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->speed_set)
    log_error("ast_module_builder_set_output_speed", line_nr, "Trying to set output speed of module '%s' to '%s'.\n"
              "                                                      But output speed has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_speed_to_string(speed),
              gpio_speed_to_string(module_builder->module->data.output.speed));
  
  module_builder->module->data.output.speed = speed;
  module_builder->speed_set = true;
}

/**
 * @brief Sets the output initialization of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param init Output initialization to set for the output module.
 * 
 * @note Logs an error and exits if the init has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_init(int line_nr, ast_module_builder_t* module_builder, gpio_init_t init){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_output_init", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_init", line_nr, "Cannot set output init for non-output module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->init_set)
    log_error("ast_module_builder_set_output_init", line_nr, "Trying to set output init of module '%s' to '%s'.\n"
              "                                                     But output init has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_init_to_string(init),
              gpio_init_to_string(module_builder->module->data.output.init));
  
  module_builder->module->data.output.init = init;
  module_builder->init_set = true;
}

/**
 * @brief Sets the output active level of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param active_level Output active level to set for the output module.
 * 
 * @note Logs an error and exits if the active level has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_active_level(int line_nr, ast_module_builder_t* module_builder, level_t active_level){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_output_active_level", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_active_level", line_nr, "Cannot set output active level for non-output module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->active_level_set)
    log_error("ast_module_builder_set_output_active_level", line_nr, "Trying to set output active level of module '%s' to '%s'.\n"
              "                                                             But output active level has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              level_to_string(active_level),
              level_to_string(module_builder->module->data.output.active_level));
  
  module_builder->module->data.output.active_level = active_level;
  module_builder->active_level_set = true;
}


/* -------------------------------------------- */
/*     Module builder input specific setters    */
/* -------------------------------------------- */

/**
 * @brief Sets the input pull of the input module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param pull Input pull to set for the input module.
 * 
 * @note Logs an error and exits if the pull has already been set, if the module kind is not input, or if any parameter is NULL.
 */
void ast_module_builder_set_input_pull(int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_input_pull", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_INPUT)
    log_error("ast_module_builder_set_input_pull", line_nr, "Cannot set input pull for non-input module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->pull_set)
    log_error("ast_module_builder_set_input_pull", line_nr, "Trying to set input pull of module '%s' to '%s'.\n"
              "                                                    But input pull has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_pull_to_string(pull),
              gpio_pull_to_string(module_builder->module->data.input.pull));
  
  module_builder->module->data.input.pull = pull;
  module_builder->pull_set = true;
}

/**
 * @brief Sets the input active level of the input module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param active_level Input active level to set for the input module.
 * 
 * @note Logs an error and exits if the active level has already been set, if the module kind is not input, or if any parameter is NULL.
 */
void ast_module_builder_set_input_active_level(int line_nr, ast_module_builder_t* module_builder, level_t active_level){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_input_active_level", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_INPUT)
    log_error("ast_module_builder_set_input_active_level", line_nr, "Cannot set input active level for non-input module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->active_level_set)
    log_error("ast_module_builder_set_input_active_level", line_nr, "Trying to set input active level of module '%s' to '%s'.\n"
              "                                                            But input active level has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              level_to_string(active_level),
              level_to_string(module_builder->module->data.input.active_level));
  
  module_builder->module->data.input.active_level = active_level;
  module_builder->active_level_set = true;
}


/* -------------------------------------------- */
/*      Module builder pwm specific setters     */
/* -------------------------------------------- */

/**
 * @brief Sets the PWM pull of the PWM module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param pull PWM pull to set for the PWM module.
 * 
 * @note Logs an error if the pull has already been set, if the module kind is not PWM, or if any parameter is NULL.
 */
void ast_module_builder_set_pwm_pull(int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pwm_pull", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_PWM_OUTPUT)
    log_error("ast_module_builder_set_pwm_pull", line_nr, "Cannot set pwm pull for non-pwm module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->pull_set)
    log_error("ast_module_builder_set_pwm_pull", line_nr, "Trying to set pwm pull of module '%s' to '%s'.\n"
              "                                                    But pwm pull has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_pull_to_string(pull),
              gpio_pull_to_string(module_builder->module->data.pwm.pull));
  
  module_builder->module->data.pwm.pull = pull;
  module_builder->pull_set = true;
}

/**
 * @brief Sets the PWM speed of the PWM module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param speed PWM speed to set for the PWM module.
 * 
 * @note Logs an error if the speed has already been set, if the module kind is not PWM, or if any parameter is NULL.
 */
void ast_module_builder_set_pwm_speed(int line_nr, ast_module_builder_t* module_builder, gpio_speed_t speed){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pwm_speed", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_PWM_OUTPUT)
    log_error("ast_module_builder_set_pwm_speed", line_nr, "Cannot set pwm speed for non-pwm module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->speed_set)
    log_error("ast_module_builder_set_pwm_speed", line_nr, "Trying to set pwm speed of module '%s' to '%s'.\n"
              "                                                    But pwm speed has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              gpio_speed_to_string(speed),
              gpio_speed_to_string(module_builder->module->data.pwm.speed));
  
  module_builder->module->data.pwm.speed = speed;
  module_builder->speed_set = true;
}

/**
 * @brief Sets the PWM active level of the PWM module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param level PWM active level to set for the PWM module.
 * 
 * @note Logs an error if the active level has already been set, if the module kind is not PWM, or if any parameter is NULL.
 */
void ast_module_builder_set_pwm_active_level(int line_nr, ast_module_builder_t* module_builder, level_t level){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pwm_active_level", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_PWM_OUTPUT)
    log_error("ast_module_builder_set_pwm_active_level", line_nr, "Cannot set pwm active level for non-pwm module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->active_level_set)
    log_error("ast_module_builder_set_pwm_active_level", line_nr, "Trying to set pwm active level of module '%s' to '%s'.\n"
              "                                                         But pwm active level has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name,
              level_to_string(level),
              level_to_string(module_builder->module->data.pwm.active_level));
  
  module_builder->module->data.pwm.active_level = level;
  module_builder->active_level_set = true;
}

/**
 * @brief Sets the PWM frequency of the PWM module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param frequency PWM frequency to set for the PWM module.
 * 
 * @note Logs an error if the frequency has already been set, if the module kind is not PWM, or if any parameter is NULL.
 */
void ast_module_builder_set_pwm_frequency(int line_nr, ast_module_builder_t* module_builder, uint32_t frequency){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pwm_frequency", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_PWM_OUTPUT)
    log_error("ast_module_builder_set_pwm_frequency", line_nr, "Cannot set pwm frequency for non-pwm module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->frequency_set)
    log_error("ast_module_builder_set_pwm_frequency", line_nr, "Trying to set pwm frequency of module '%s' to '%u'.\n"
              "                                                     But pwm frequency has already been set to '%u'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              frequency,
              module_builder->module->data.pwm.frequency);
  
  module_builder->module->data.pwm.frequency = frequency;
  module_builder->frequency_set = true;
}

/**
 * @brief Sets the PWM duty cycle of the PWM module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param duty_cycle PWM duty cycle to set for the PWM module.
 * 
 * @note Logs an error if the duty cycle has already been set, if the module kind is not PWM, or if any parameter is NULL.
 */
void ast_module_builder_set_pwm_duty(int line_nr, ast_module_builder_t* module_builder, uint32_t duty_cycle){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_pwm_duty", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_PWM_OUTPUT)
    log_error("ast_module_builder_set_pwm_duty", line_nr, "Cannot set pwm duty cycle for non-pwm module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->duty_cycle_set)
    log_error("ast_module_builder_set_pwm_duty", line_nr, "Trying to set pwm duty cycle of module '%s' to '%u'.\n"
              "                                                    But pwm duty cycle has already been set to '%u'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              duty_cycle,
              module_builder->module->data.pwm.duty_cycle);
  
  module_builder->module->data.pwm.duty_cycle = duty_cycle;
  module_builder->duty_cycle_set = true;
}


/* -------------------------------------------- */
/*    Module builder UART specific setters      */
/* -------------------------------------------- */
/**
 * @brief Sets the UART TX pin of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param tx_pin UART TX pin to set for the UART module.
 * 
 * @note Logs an error if the tx pin has already been set, if the module kind is not UART, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_tx_pin(int line_nr, ast_module_builder_t* module_builder, pin_t tx_pin){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_tx_pin", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_tx_pin", line_nr, "Cannot set uart tx pin for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->pin_set)
    log_error("ast_module_builder_set_uart_tx_pin", line_nr, "Trying to set uart tx pin of module '%s' to '%s'.\n"
              "                                                    But uart tx pin has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              pin_to_string(tx_pin),
              pin_to_string(module_builder->module->pin));
  
  module_builder->module->pin = tx_pin;
  module_builder->pin_set = true;
}

/**
 * @brief Sets the UART RX pin of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param rx_pin UART RX pin to set for the UART module.
 * 
 * @note Logs an error if the rx pin has already been set, if the module kind is not UART, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_rx_pin(int line_nr, ast_module_builder_t* module_builder, pin_t rx_pin){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_rx_pin", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_rx_pin", line_nr, "Cannot set uart rx pin for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->rx_pin_set)
    log_error("ast_module_builder_set_uart_rx_pin", line_nr, "Trying to set uart rx pin of module '%s' to '%s'.\n"
              "                                                    But uart rx pin has already been set to '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              pin_to_string(rx_pin),
              pin_to_string(module_builder->module->data.uart.rx_pin));
  
  module_builder->module->data.uart.rx_pin = rx_pin;
  module_builder->rx_pin_set = true;
}

/**
 * @brief Sets the UART baud rate of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param baudrate UART baud rate to set for the UART module.
 * 
 * @note Logs an error if the baud rate has already been set, if the module kind is not UART, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_baudrate(int line_nr, ast_module_builder_t* module_builder, uint32_t baudrate){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_baudrate", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_baudrate", line_nr, "Cannot set uart baudrate for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->baudrate_set)
    log_error("ast_module_builder_set_uart_baudrate", line_nr, "Trying to set uart baudrate of module '%s' to '%u'.\n"
              "                                                     But uart baudrate has already been set to '%u'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              baudrate,
              module_builder->module->data.uart.baudrate);
  
  module_builder->module->data.uart.baudrate = baudrate;
  module_builder->baudrate_set = true;
}

/**
 * @brief Sets the UART data bits of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param databits UART data bits to set for the UART module.
 * 
 * @note Logs an error if the data bits have already been set, if the module kind is not UART, databits exceed uint8_t max, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_databits(int line_nr, ast_module_builder_t* module_builder, uint32_t databits){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_databits", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_databits", line_nr, "Cannot set uart databits for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->databits_set)
    log_error("ast_module_builder_set_uart_databits", line_nr, "Trying to set uart databits of module '%s' to '%u'.\n"
              "                                                     But uart databits has already been set to '%u'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              databits,
              module_builder->module->data.uart.databits);
  
  if(databits > UINT8_MAX) // Validate that databits fit in uint8_t
    log_error("ast_module_builder_set_uart_databits", line_nr, "UART databits value '%u' exceeds maximum allowed value of '%u'.",
              databits,
              UINT8_MAX);
  
  module_builder->module->data.uart.databits = databits;
  module_builder->databits_set = true;
}

/**
 * @brief Sets the UART stop bits of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param stopbits UART stop bits to set for the UART module.
 * 
 * @note Logs an error if the stop bits have already been set, if the module kind is not UART, stopbits exceed uint8_t max, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_stopbits(int line_nr, ast_module_builder_t* module_builder, uint32_t stopbits){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_stopbits", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_stopbits", line_nr, "Cannot set uart stopbits for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->stopbits_set)
    log_error("ast_module_builder_set_uart_stopbits", line_nr, "Trying to set uart stopbits of module '%s' to '%u'.\n"
              "                                                     But uart stopbits has already been set to '%u'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name, 
              stopbits,
              module_builder->module->data.uart.stopbits);
  
  if(stopbits > UINT8_MAX) // Validate that stopbits fit in uint8_t
    log_error("ast_module_builder_set_uart_stopbits", line_nr, "UART stopbits value '%u' exceeds maximum allowed value of '%u'.",
              stopbits,
              UINT8_MAX);
  
  module_builder->module->data.uart.stopbits = stopbits;
  module_builder->stopbits_set = true;
}

/**
 * @brief Sets the UART parity of the UART module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param module_builder Pointer to the AST module builder.
 * @param parity UART parity to set for the UART module.
 * 
 * @note Logs an error if the parity has already been set, if the module kind is not UART, or if any parameter is NULL.
 */
void ast_module_builder_set_uart_parity(  int line_nr, ast_module_builder_t* module_builder, uart_parity_t parity){
  if(module_builder == NULL)
    log_error("ast_module_builder_set_uart_parity", 0, "AST module builder is NULL.");
  
  if(module_builder->module->kind != MODULE_UART)
    log_error("ast_module_builder_set_uart_parity", line_nr, "Cannot set uart parity for non-uart module '%s'.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  if(module_builder->parity_set)
    log_error("ast_module_builder_set_uart_parity", line_nr, "Trying to set uart parity of module '%s'.\n"
              "                                                     But uart parity has already been set.", 
              module_builder->module->name == NULL ? "<NULL>" : module_builder->module->name);
  
  module_builder->module->data.uart.parity = parity;
  module_builder->parity_set = true;
}