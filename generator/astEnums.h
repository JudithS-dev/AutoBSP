#ifndef __AST_ENUMS_H__
#define __AST_ENUMS_H__

#include <stdint.h>

#include "moduleEnums.h"

/**
 * @brief Module kind types
 * 
 * Values: MODULE_OUTPUT, MODULE_INPUT
 */
typedef enum{
  MODULE_OUTPUT,
  MODULE_INPUT,
  MODULE_PWM_OUTPUT
} ast_module_kind_t;

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
} ast_module_output_t;

/**
 * @brief Structure representing input module parameters.
 * 
 * Consists of pull-up/pull-down configuration, and active level.
 */
typedef struct{
  gpio_pull_t  pull;
  level_t      active_level;
} ast_module_input_t;

/**
 * @brief Structure representing PWM module parameters.
 * 
 * Consists of pull-up/pull-down configuration, speed, active level, frequency, and duty cycle.
 */
typedef struct{
  gpio_pull_t  pull;
  gpio_speed_t speed;
  level_t      active_level;
  uint32_t     frequency;
  uint32_t     duty_cycle;
} ast_module_pwm_t;

/**
 * @brief Structure representing a module node in the AST.
 * 
 * Consists of node ID, line number, name, pin, module kind, module-specific data, and pointer to the next module node.
 */
typedef struct ast_module_node_s{
  unsigned int  node_id;
  int           line_nr;
  char*         name;
  pin_t         pin;
  bool          enable;
  ast_module_kind_t kind;
  union{
    ast_module_output_t  output;
    ast_module_input_t   input;
    ast_module_pwm_t     pwm;
  } data;
  
  struct ast_module_node_s* next;
} ast_module_node_t;

/**
 * @brief Structure for building module nodes in the AST.
 * 
 * Consists of flags indicating which parameters have been set and a pointer to the current module node being built.
 */
typedef struct ast_module_builder_s{
  bool name_set;
  bool pin_set;
  bool enable_set;
  
  bool kind_set;
  
  bool type_set;
  bool pull_set;
  bool speed_set;
  bool init_set;
  bool active_level_set;
  
  bool frequency_set;
  bool duty_cycle_set;
  
  ast_module_node_t *module;
  struct ast_module_builder_s *next;
} ast_module_builder_t;

/**
 * @brief Structure for building the DSL node in the AST.
 * 
 * Consists of flag indicating if controller has been set, controller type, and pointer to the module builder list.
 */
typedef struct{
  bool controller_set;
  controller_t controller;
  ast_module_builder_t *module_builders_root;
} ast_dsl_builder_t;


/* ---------- Finished AST Structure ---------- */

/**
 * @brief Structure representing finished build DSL node in the AST.
 * 
 * Consists of global parameters like controller type and pointer to a linked list of module nodes.
 * 
 */
typedef struct{
  controller_t   controller;
  ast_module_node_t* modules_root;
} ast_dsl_node_t;

#endif //__AST_ENUMS_H__