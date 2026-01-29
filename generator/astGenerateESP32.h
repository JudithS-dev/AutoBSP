#ifndef __AST_CHECK_ESP32_H__
#define __AST_CHECK_ESP32_H__

#include <stdio.h>
#include "astEnums.h"

void ast_generate_source_esp32(FILE* output_source, ast_dsl_node_t* dsl_node);

#endif // __AST_CHECK_ESP32_H__