#ifndef __AST_CHECK_H__
#define __AST_CHECK_H__

#include "astEnums.h"

// Required parameter checks
void ast_check_required_params(ast_dsl_builder_t* dsl_builder);

// Uniqueness checks
void ast_check_unique_enabled_names(ast_dsl_node_t* dsl_node);
void ast_check_unique_enabled_pins(ast_dsl_node_t* dsl_node);

// Sorting of modules
void ast_sort_modules_by_pin(ast_dsl_node_t *dsl_node);

#endif //__AST_CHECK_H__