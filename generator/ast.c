#include "ast.h"

#include <stdlib.h>

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
  builder->type_set         = false;
  builder->pull_set         = false;
  builder->speed_set        = false;
  builder->init_set         = false;
  builder->active_level_set = false;
  builder->enable_set       = false;
  
  
  builder->p_current_module = (module_node_t*)malloc(sizeof(module_node_t));
  if(builder->p_current_module == NULL)
    log_error("ast_new_module_builder", 0, "Could not allocate memory for new AST module node.");
  
  builder->p_current_module->node_id = global_ast_node_counter++;
  builder->p_current_module->line_nr = line_nr;
  builder->p_current_module->name  = NULL;
  return builder;
}


