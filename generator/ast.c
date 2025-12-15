#include "ast.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"

unsigned int global_ast_node_counter = 0;

/**
 * @brief Creates a new AST module builder and initializes its fields.
 * 
 * @param lineNr Line number where the module is defined.
 * @return Pointer to the newly created AST module builder.
 */
ast_module_builder_t* ast_new_module_builder(int line_nr){
  ast_module_builder_t *builder = (ast_module_builder_t*)malloc(sizeof(ast_module_builder_t));
  if(builder == NULL)
    log_error("ast_new_module_builder", 0, "Could not allocate memory for new AST module builder.");
  
  // Initialize all fields of builder to default values
  builder->name_set         = false;
  builder->pin_set          = false;
  builder->enable_set       = false;
  builder->kind_set         = false;
  builder->type_set         = false;
  builder->pull_set         = false;
  builder->speed_set        = false;
  builder->init_set         = false;
  builder->active_level_set = false;
  
  
  builder->p_current_module = (module_node_t*)malloc(sizeof(module_node_t));
  if(builder->p_current_module == NULL)
    log_error("ast_new_module_builder", 0, "Could not allocate memory for new AST module node.");
  
  builder->p_current_module->node_id = global_ast_node_counter++;
  builder->p_current_module->line_nr = line_nr;
  builder->p_current_module->name  = NULL;
  return builder;
}


/* -------------------------------------------- */
/*             Common module setters            */
/* -------------------------------------------- */

/**
 * @brief Sets the name of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param name Name to set for the module.
 * 
 * @note Logs an error and exits if the name has already been set or if any parameter is NULL.
 * @note The name is duplicated to ensure it is stored correctly in the module node.
 */
void ast_module_builder_set_name(int line_nr, ast_module_builder_t* builder, const char* name){
  if(builder == NULL)
    log_error("ast_module_builder_set_name", 0, "AST module builder is NULL.");
  if(name == NULL)
    log_error("ast_module_builder_set_name", 0, "Module name is NULL.");
  
  if(builder->name_set)
    log_error("ast_module_builder_set_name", line_nr, "Module name has already been set to '%s'.",
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  
  builder->p_current_module->name = strdup(name);
  if(builder->p_current_module->name == NULL)
    log_error("ast_module_builder_set_name", 0, "Could not allocate memory for module name.");
  builder->name_set = true;
}

/**
 * @brief Sets the pin of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param pin Pin to set for the module.
 * 
 * @note Logs an error and exits if the pin has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_pin(int line_nr, ast_module_builder_t* builder, pin_t pin){
  if(builder == NULL)
    log_error("ast_module_builder_set_pin", 0, "AST module builder is NULL.");
  
  if(builder->pin_set)
    log_error("ast_module_builder_set_pin", line_nr, "Module pin of module '%s' has already been set to Port: '%c' Pin: '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->pin.port,  
              builder->p_current_module->pin.pin_number);
  
  builder->p_current_module->pin = pin;
  builder->pin_set = true;
}

/**
 * @brief Sets the enable flag of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param enable Enable flag to set for the module.
 * 
 * @note Logs an error and exits if the enable flag has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_enable(int line_nr, ast_module_builder_t* builder, bool enable){
  if(builder == NULL)
    log_error("ast_module_builder_set_enable", 0, "AST module builder is NULL.");
  
  if(builder->enable_set)
    log_error("ast_module_builder_set_enable", line_nr, "Module enable of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->enable);
  
  builder->p_current_module->enable = enable;
  builder->enable_set = true;
}

/**
 * @brief Sets the kind of the module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param kind Kind to set for the module.
 * 
 * @note Logs an error and exits if the kind has already been set or if any parameter is NULL.
 */
void ast_module_builder_set_kind(int line_nr, ast_module_builder_t* builder, module_kind_t kind){
  if(builder == NULL)
    log_error("ast_module_builder_set_kind", 0, "AST module builder is NULL.");
  
  if(builder->kind_set)
    log_error("ast_module_builder_set_kind", line_nr, "Module kind of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->kind);
  
  builder->p_current_module->kind = kind;
  builder->kind_set = true;
}


/* -------------------------------------------- */
/*        Output module specific setters        */
/* -------------------------------------------- */


/**
 * @brief Sets the output type of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param type Output type to set for the output module.
 * 
 * @note Logs an error and exits if the type has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_type(int line_nr, ast_module_builder_t* builder, gpio_type_t type){
  if(builder == NULL)
    log_error("ast_module_builder_set_output_type", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_type", line_nr, "Cannot set output type for non-output module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->type_set)
    log_error("ast_module_builder_set_output_type", line_nr, "Output module type of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->data.output.type);
  
  builder->p_current_module->data.output.type = type;
  builder->type_set = true;
}

/**
 * @brief Sets the output pull of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param pull Output pull to set for the output module.
 * 
 * @note Logs an error and exits if the pull has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_pull(int line_nr, ast_module_builder_t* builder, gpio_pull_t pull){
  if(builder == NULL)
    log_error("ast_module_builder_set_output_pull", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_pull", line_nr, "Cannot set output pull for non-output module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->pull_set)
    log_error("ast_module_builder_set_output_pull", line_nr, "Output module pull of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->data.output.pull);
  
  builder->p_current_module->data.output.pull = pull;
  builder->pull_set = true;
}

/**
 * @brief Sets the output speed of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param speed Output speed to set for the output module.
 * 
 * @note Logs an error and exits if the speed has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_speed(int line_nr, ast_module_builder_t* builder, gpio_speed_t speed){
  if(builder == NULL)
    log_error("ast_module_builder_set_output_speed", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_speed", line_nr, "Cannot set output speed for non-output module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->speed_set)
    log_error("ast_module_builder_set_output_speed", line_nr, "Output module speed of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->data.output.speed);
  
  builder->p_current_module->data.output.speed = speed;
  builder->speed_set = true;
}

/**
 * @brief Sets the output initialization of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param init Output initialization to set for the output module.
 * 
 * @note Logs an error and exits if the init has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_init(int line_nr, ast_module_builder_t* builder, gpio_init_t init){
  if(builder == NULL)
    log_error("ast_module_builder_set_output_init", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_init", line_nr, "Cannot set output init for non-output module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->init_set)
    log_error("ast_module_builder_set_output_init", line_nr, "Output module init of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name,
              builder->p_current_module->data.output.init);
  
  builder->p_current_module->data.output.init = init;
  builder->init_set = true;
}

/**
 * @brief Sets the output active level of the output module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param active_level Output active level to set for the output module.
 * 
 * @note Logs an error and exits if the active level has already been set, if the module kind is not output, or if any parameter is NULL.
 */
void ast_module_builder_set_output_active_level(int line_nr, ast_module_builder_t* builder, level_t active_level){
  if(builder == NULL)
    log_error("ast_module_builder_set_output_active_level", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_OUTPUT)
    log_error("ast_module_builder_set_output_active_level", line_nr, "Cannot set output active level for non-output module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->active_level_set)
    log_error("ast_module_builder_set_output_active_level", line_nr, "Output module active level of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name,
              builder->p_current_module->data.output.active_level);
  
  builder->p_current_module->data.output.active_level = active_level;
  builder->active_level_set = true;
}


/* -------------------------------------------- */
/*        Input module specific setters         */
/* -------------------------------------------- */

/**
 * @brief Sets the input pull of the input module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param pull Input pull to set for the input module.
 * 
 * @note Logs an error and exits if the pull has already been set, if the module kind is not input, or if any parameter is NULL.
 */
void ast_module_builder_set_input_pull(int line_nr, ast_module_builder_t* builder, gpio_pull_t pull){
  if(builder == NULL)
    log_error("ast_module_builder_set_input_pull", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_INPUT)
    log_error("ast_module_builder_set_input_pull", line_nr, "Cannot set input pull for non-input module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->pull_set)
    log_error("ast_module_builder_set_input_pull", line_nr, "Input module pull of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name, 
              builder->p_current_module->data.input.pull);
  
  builder->p_current_module->data.input.pull = pull;
  builder->pull_set = true;
}

/**
 * @brief Sets the input active level of the input module in the AST module builder.
 * 
 * @param line_nr Line number where the function is called for logging purposes.
 * @param builder Pointer to the AST module builder.
 * @param active_level Input active level to set for the input module.
 * 
 * @note Logs an error and exits if the active level has already been set, if the module kind is not input, or if any parameter is NULL.
 */
void ast_module_builder_set_input_active_level(int line_nr, ast_module_builder_t* builder, level_t active_level){
  if(builder == NULL)
    log_error("ast_module_builder_set_input_active_level", 0, "AST module builder is NULL.");
  
  if(builder->p_current_module->kind != MODULE_INPUT)
    log_error("ast_module_builder_set_input_active_level", line_nr, "Cannot set input active level for non-input module '%s'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name);
  if(builder->active_level_set)
    log_error("ast_module_builder_set_input_active_level", line_nr, "Input module active level of module '%s' has already been set to '%d'.", 
              builder->p_current_module->name == NULL ? "<NULL>" : builder->p_current_module->name,
              builder->p_current_module->data.input.active_level);
  
  builder->p_current_module->data.input.active_level = active_level;
  builder->active_level_set = true;
}