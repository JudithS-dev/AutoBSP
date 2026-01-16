#ifndef __AST_CHECK_STM32F446RE_H__
#define __AST_CHECK_STM32F446RE_H__

#include "astEnums.h"

// Validity checks for STM32F446RE
void ast_check_stm32f446re_valid_pins(ast_dsl_node_t* dsl_node);

// Backend specific parameter bindings for STM32F446RE
void ast_check_stm32f446re_bind_pins(ast_dsl_node_t* dsl_node);

#endif // __AST_CHECK_STM32F446RE_H__