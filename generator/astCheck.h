#ifndef __AST_CHECK_H__
#define __AST_CHECK_H__

#include "astEnums.h"

void ast_check_dsl(dsl_node_t* dsl);
void ast_check_module(ast_module_builder_t* builder);
void ast_check_free();

void ast_sort_modules_by_pin(dsl_node_t *dsl_node);

#endif //__AST_CHECK_H__