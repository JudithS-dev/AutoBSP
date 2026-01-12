#ifndef __AST_GENERATE_STM32F446RE_H__
#define __AST_GENERATE_STM32F446RE_H__

#include <stdio.h>
#include "astEnums.h"

void ast_generate_header_stm32f446re(FILE* output_header, ast_dsl_node_t* dsl_node);
void ast_generate_source_stm32f446re(FILE* output_source, ast_dsl_node_t* dsl_node);

#endif // __AST_GENERATE_STM32F446RE_H__