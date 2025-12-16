#ifndef __AST_ENUMS_H__
#define __AST_ENUMS_H__

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

#endif //__AST_ENUMS_H__