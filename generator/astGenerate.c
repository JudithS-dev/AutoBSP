#include "astGenerate.h"

#include <stdio.h>

#include "logging.h"
#include "astGenerateSTM32F446RE.h"

/**
 * @brief Generates board support package (BSP) code files based on the provided DSL AST node.
 * 
 * @param output_path Path to the directory where the generated code files will be created.
 * @param dsl_node Pointer to the DSL AST node from which code will be generated.
 * 
 * @note If dsl_node is NULL, an error is logged.
 * @note If output_path is NULL, an error is logged.
 */
void ast_generate_code(const char* output_path, ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_generate_code", 0, "DSL node is NULL.");
  if(output_path == NULL)
    log_error("ast_generate_code", 0, "Output path is NULL.");
  
  // Create output files
  char header_path[256];
  char source_path[256];
  snprintf(header_path, sizeof(header_path), "%s/generated_bsp.h", output_path);
  snprintf(source_path, sizeof(source_path), "%s/generated_bsp.c", output_path);
  
  FILE* output_header = fopen(header_path, "w");
  if(output_header == NULL)
    log_error("ast_generate_code", 0, "Failed to open header file for writing: '%s'", header_path);
  
  FILE* output_source = fopen(source_path, "w");
  if(output_source == NULL)
    log_error("ast_generate_code", 0, "Failed to open source file for writing: '%s'", source_path);
  
  // Check controller type and generate code accordingly
  switch(dsl_node->controller){
    case STM32F446RE: ast_generate_header_stm32f446re(output_header, dsl_node);
                      ast_generate_source_stm32f446re(output_source, dsl_node);
                      break;
    default:          log_error("ast_generate_code", 0, "Unsupported controller type enum value '%d'", dsl_node->controller);
  }
  
  // Close files
  fclose(output_header);
  fclose(output_source);
}