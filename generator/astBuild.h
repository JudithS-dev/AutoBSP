#ifndef __AST_BUILD_H__
#define __AST_BUILD_H__

#include "astEnums.h"

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

#endif //__AST_BUILD_H__