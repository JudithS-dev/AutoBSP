#ifndef __AST_HELPER_H__
#define __AST_HELPER_H__

#include <stdbool.h>

#include "astEnums.h"

bool has_enabled_gpio_module(ast_dsl_node_t* dsl_node);
bool has_enabled_pwm_module(ast_dsl_node_t* dsl_node);
bool has_enabled_uart_module(ast_dsl_node_t* dsl_node);

#endif // __AST_HELPER_H__