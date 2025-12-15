#ifndef __AST_H__
#define __AST_H__

#include "moduleEnums.h"

/**
 * @brief Module kind types
 * 
 * Values: MODULE_OUTPUT, MODULE_INPUT
 */
typedef enum{
  MODULE_OUTPUT,
  MODULE_INPUT
} module_kind_t;

/**
 * @brief Structure representing output module parameters.
 * 
 * Consists of GPIO type, pull-up/pull-down configuration, speed, initial state, and active level.
 */
typedef struct{
  gpio_type_t  type;
  gpio_pull_t  pull;
  gpio_speed_t speed;
  gpio_init_t  init;
  level_t      active_level;
} module_output_t;

/**
 * @brief Structure representing input module parameters.
 * 
 * Consists of pull-up/pull-down configuration, and active level.
 */
typedef struct{
  gpio_pull_t  pull;
  level_t      active_level;
} module_input_t;

/**
 * @brief Structure representing a module node in the AST.
 * 
 * Consists of node ID, line number, name, pin, module kind, module-specific data, and pointer to the next module node.
 */
typedef struct module_node_s{
  unsigned int  node_id;
  int           line_nr;
  char*         name;
  pin_t         pin;
  bool          enable;
  module_kind_t kind;
  union{
    module_output_t  output;
    module_input_t   input;
  } data;
  
  struct module_node_s* next;
} module_node_t;

/**
 * @brief Structure representing the root DSL node.
 * 
 * Consists of global parameters like controller type and pointer to a linked list of module nodes.
 * 
 */
typedef struct{
  bool           controller_set;
  controller_t   controller;
  module_node_t* modules_root;
} dsl_node_t;


/**
 * @brief Structure for building module nodes in the AST.
 * 
 * Consists of flags indicating which parameters have been set and a pointer to the current module node being built.
 */
typedef struct{
  bool name_set;
  bool pin_set;
  bool enable_set;
  
  bool kind_set;
  
  bool type_set;
  bool pull_set;
  bool speed_set;
  bool init_set;
  bool active_level_set;
  
  module_node_t *p_current_module;
} ast_module_builder_t;

// Constructors and destructors
dsl_node_t* ast_new_dsl_node();
void ast_free_dsl_node(dsl_node_t* dsl_node);
ast_module_builder_t* ast_new_module_builder(int line_nr);
module_node_t* ast_free_module_builder(ast_module_builder_t* builder);

// DSL node functions
void ast_dsl_node_set_controller(int line_nr, dsl_node_t* dsl_node, controller_t controller);
void ast_dsl_node_append_module( int line_nr, dsl_node_t* dsl_node, module_node_t* module);

// Common module setters
void ast_module_builder_set_name(  int line_nr, ast_module_builder_t* builder, const char* name);
void ast_module_builder_set_pin(   int line_nr, ast_module_builder_t* builder, pin_t pin);
void ast_module_builder_set_enable(int line_nr, ast_module_builder_t* builder, bool enable);
void ast_module_builder_set_kind(  int line_nr, ast_module_builder_t* builder, module_kind_t kind);

// Output module specific setters
void ast_module_builder_set_output_type(        int line_nr, ast_module_builder_t* builder, gpio_type_t type);
void ast_module_builder_set_output_pull(        int line_nr, ast_module_builder_t* builder, gpio_pull_t pull);
void ast_module_builder_set_output_speed(       int line_nr, ast_module_builder_t* builder, gpio_speed_t speed);
void ast_module_builder_set_output_init(        int line_nr, ast_module_builder_t* builder, gpio_init_t init);
void ast_module_builder_set_output_active_level(int line_nr, ast_module_builder_t* builder, level_t level);

// Input module specific setters
void ast_module_builder_set_input_pull(        int line_nr, ast_module_builder_t* builder, gpio_pull_t pull);
void ast_module_builder_set_input_active_level(int line_nr, ast_module_builder_t* builder, level_t level);

#endif //__AST_H__