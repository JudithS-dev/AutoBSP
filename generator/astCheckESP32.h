#ifndef __AST_CHECK_ESP32_H__
#define __AST_CHECK_ESP32_H__

#include "astEnums.h"

// Required parameter checks for ESP32
void ast_check_esp32_required_params(ast_dsl_builder_t* dsl_builder);

// Validity checks for ESP32
void ast_check_esp32_valid_pins(ast_dsl_node_t* dsl_node);

// Backend specific parameter bindings for ESP32
void ast_check_esp32_bind_pins(ast_dsl_node_t* dsl_node);

#endif // __AST_CHECK_ESP32_H__