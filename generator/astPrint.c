#include "astPrint.h"

#include "logging.h"
#include "astEnums2Str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void ast_print_helper(FILE *pfDot, const ast_dsl_node_t* dsl_node, bool print_enabled_only);

/* -------------------------------------------- */
/*                 AST Printing                 */
/* -------------------------------------------- */

/**
 * @brief Prints the AST represented by the given DSL node to a DOT file and creates shell scripts to generate and remove PNG and DOT files.
 * 
 * @param output_path Path to the directory where the output files will be created.
 * @param dsl_node Pointer to the DSL AST node to be printed.
 */
void ast_print(const char* output_path, const ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_print", 0, "DSL node is NULL, cannot print AST.");
  
  // Create shell script to remove PNG and DOT files (is at beginning to avoid not creating it if error occurs during AST print)
  char remove_script_path[256];
  snprintf(remove_script_path, sizeof(remove_script_path), "%s/removePNGandDOT.sh", output_path);
  FILE *file_remove = fopen(remove_script_path, "w");
  if(file_remove == NULL)
    log_error("ast_print", 0, "Failed to create remove script at path: '%s'", remove_script_path);
  fprintf(file_remove, "rm -f %s/ast_graph_complete.gv %s/ast_graph_complete.png\n", output_path, output_path);
  fprintf(file_remove, "rm -f %s/ast_graph_enabled_only.gv %s/ast_graph_enabled_only.png\n", output_path, output_path);
  fclose(file_remove);
  
  char dot_file_complete[256];
  snprintf(dot_file_complete, sizeof(dot_file_complete), "%s/ast_graph_complete.gv", output_path);
  FILE *file_dot = fopen(dot_file_complete, "w");
  if(file_dot == NULL)
    log_error("ast_print", 0, "Failed to open DOT file for writing: '%s'", dot_file_complete);
  ast_print_helper(file_dot, dsl_node, false);
  fclose(file_dot);

  char dot_file_enabled[256];
  snprintf(dot_file_enabled, sizeof(dot_file_enabled), "%s/ast_graph_enabled_only.gv", output_path);
  file_dot = fopen(dot_file_enabled, "w");
  if(file_dot == NULL)
    log_error("ast_print", 0, "Failed to open DOT file for writing: '%s'", dot_file_enabled);
  ast_print_helper(file_dot, dsl_node, true);
  fclose(file_dot);
  
  // Create shell script to generate PNG from DOT
  char create_script_path[256];
  snprintf(create_script_path, sizeof(create_script_path), "%s/createPNGfromDOT.sh", output_path);
  FILE *file_create = fopen(create_script_path, "w");
  if(file_create == NULL)
    log_error("ast_print", 0, "Failed to create create script at path: '%s'", create_script_path);
  fprintf(file_create, "dot %s/ast_graph_complete.gv -Tpng -o %s/ast_graph_complete.png\n", output_path, output_path);
  fprintf(file_create, "dot %s/ast_graph_enabled_only.gv -Tpng -o %s/ast_graph_enabled_only.png\n", output_path, output_path);
  fclose(file_create);
}

/**
 * @brief Helper function to print the AST in DOT format.
 * 
 * @param pfDot File pointer to the DOT file.
 * @param dsl_node Pointer to the DSL AST node to be printed.
 * @param print_enabled_only If true, only enabled modules are printed.
 */
static void ast_print_helper(FILE *pfDot, const ast_dsl_node_t* dsl_node, bool print_enabled_only){
  fprintf(pfDot, "digraph AST {\n");
  fprintf(pfDot, "  splines=ortho;\n");
  fprintf(pfDot, "  node [shape=plain, fontname=\"Helvetica\"];\n");
  fprintf(pfDot, "  edge [arrowhead=vee];\n");
  
  // Print controller node
  fprintf(pfDot,
  "  Controller [label=<<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"8\" BGCOLOR=\"#E8E8E8\">"
  "\n    <TR><TD><B>Controller</B></TD></TR>\n    <TR><TD ALIGN=\"LEFT\">%s</TD></TR>\n    </TABLE>>];\n",
  controller_to_string(dsl_node->controller)
  );
  
  // Print modules
  ast_module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    // Skip disabled modules if print_enabled_only is true
    if(print_enabled_only && current_module->enable == false){
      current_module = current_module->next;
      continue;
    }
    // Determine background color based on module kind and enable status
    const char *colour = "#FFFFFF";
    if(current_module->kind == MODULE_OUTPUT)
      colour = current_module->enable ? "#B7D9F7" : "#EEF6FD";
    else // MODULE_INPUT
      colour = current_module->enable ? "#C6EFC6" : "#F2FBF2";
    
    // Print general module info
    char *pin_str = pin_to_string(current_module->pin);
    
    fprintf(pfDot, "  Module%d [label=<<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"6\" BGCOLOR=\"%s\">", 
                      current_module->node_id, colour);
    
    // Module name as header
    fprintf(pfDot, "\n    <TR><TD><B>%s</B> (%d)</TD></TR>", current_module->name, current_module->node_id);
    
    // General module attributes as bullet points
    fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Kind:</B> %s</TD></TR>",   kind_to_string(current_module->kind));
    fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Pin:</B> %s</TD></TR>",    pin_str);
    fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Enable:</B> %s</TD></TR>", bool_to_string(current_module->enable));
    
    free(pin_str);
    
    // Print module-specific data
    switch(current_module->kind){
      case MODULE_OUTPUT: fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Type:</B> %s</TD></TR>",
                                          gpio_type_to_string(current_module->data.output.type));
                          fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Pull:</B> %s</TD></TR>",
                                          gpio_pull_to_string(current_module->data.output.pull));
                          fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Speed:</B> %s</TD></TR>",
                                          gpio_speed_to_string(current_module->data.output.speed));
                          fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Init:</B> %s</TD></TR>",
                                          gpio_init_to_string(current_module->data.output.init));
                          fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Active Level:</B> %s</TD></TR>",
                                          level_to_string(current_module->data.output.active_level));
                          break;
      case MODULE_INPUT:  fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Pull:</B> %s</TD></TR>",
                                          gpio_pull_to_string(current_module->data.input.pull));
                          fprintf(pfDot, "\n    <TR><TD ALIGN=\"LEFT\">&#8226; <B>Active Level:</B> %s</TD></TR>",
                                          level_to_string(current_module->data.input.active_level));
                          break;
      default:  log_error("ast_print_helper", 0, "Unknown module kind enum value '%d'", current_module->kind);
                break; // This won't be reached due to log_error exiting
    }
    
    fprintf(pfDot, "\n    </TABLE>>];\n");
    current_module = current_module->next;
  }
  
  // Print edges from controller to modules
  fprintf(pfDot, "\n");
  current_module = dsl_node->modules_root;
  while(current_module != NULL){
    // Skip disabled modules if print_enabled_only is true
    if(print_enabled_only && current_module->enable == false){
      current_module = current_module->next;
      continue;
    }
    fprintf(pfDot, "  Controller -> Module%d;\n", current_module->node_id);
    current_module = current_module->next;
  }
  
  fprintf(pfDot, "}\n");
}


