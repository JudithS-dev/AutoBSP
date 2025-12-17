#include "astPrint.h"

#include "logging.h"
#include "astEnums2Str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* --------------- AST Printing --------------- */
static void ast_print_helper(FILE *pfDot, const dsl_node_t* dsl_node);


/* -------------------------------------------- */
/*                 AST Printing                 */
/* -------------------------------------------- */

/**
 * @brief Prints the AST represented by the given DSL node to a DOT file and creates shell scripts to generate and remove PNG and DOT files.
 * 
 * @param dsl_node Pointer to the DSL AST node to be printed.
 */
void ast_print(const dsl_node_t* dsl_node){
  log_info("ast_print", LOG_OTHER, 0, "Start AST printing...");
  
  if(dsl_node == NULL)
    log_error("ast_print", 0, "DSL node is NULL, cannot print AST.");
  
  // Create shell script to remove PNG and DOT files (is at beginning to avoid not creating it if error occurs during AST print)
  FILE *file_remove = fopen("removePNGandDOT.sh", "w");
  fprintf(file_remove, "rm -f ast_graph.gv ast_graph.png");
  fclose(file_remove);
  
  FILE *file_dot = fopen("ast_graph.gv", "w");
  ast_print_helper(file_dot, dsl_node);
  fclose(file_dot);
  
  // Create shell script to generate PNG from DOT
  FILE *file_create = fopen("createPNGfromDOT.sh", "w");
  fprintf(file_create, "dot ast_graph.gv -Tpng -o ast_graph.png\n");
  fclose(file_create);
}

/**
 * @brief Helper function to print the AST in DOT format.
 * 
 * @param pfDot File pointer to the DOT file.
 * @param dsl_node Pointer to the DSL AST node to be printed.
 */
static void ast_print_helper(FILE *pfDot, const dsl_node_t* dsl_node){
  fprintf(pfDot, "digraph AST {\n");
  fprintf(pfDot, "  splines=ortho;\n");
  fprintf(pfDot, "  node [shape=plain, fontname=\"Helvetica\"];\n");
  fprintf(pfDot, "  edge [arrowhead=vee];\n");
  
  // Print controller node
  fprintf(pfDot,
  "  Controller [label=<<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"8\" BGCOLOR=\"#E8E8E8\">"
  "<TR><TD><B>Controller</B></TD></TR><TR><TD ALIGN=\"LEFT\">%s</TD></TR></TABLE>>];\n",
  controller_to_string(dsl_node->controller)
  );
  
  // Print modules
  module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
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
    fprintf(pfDot, "<TR><TD><B>%s</B></TD></TR>", current_module->name);
    
    // General module attributes as bullet points
    fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>ID:</B> %d</TD></TR>",     current_module->node_id);
    fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Pin:</B> %s</TD></TR>",    pin_str);
    fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Enable:</B> %s</TD></TR>", bool_to_string(current_module->enable));
    fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Kind:</B> %s</TD></TR>",   kind_to_string(current_module->kind));
    
    free(pin_str);
    
    // Print module-specific data
    switch(current_module->kind){
      case MODULE_OUTPUT: fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Type:</B> %s</TD></TR>",
                                          gpio_type_to_string(current_module->data.output.type));
                          fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Pull:</B> %s</TD></TR>",
                                          gpio_pull_to_string(current_module->data.output.pull));
                          fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Speed:</B> %s</TD></TR>",
                                          gpio_speed_to_string(current_module->data.output.speed));
                          fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Init:</B> %s</TD></TR>",
                                          gpio_init_to_string(current_module->data.output.init));
                          fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Active Level:</B> %s</TD></TR>",
                                          level_to_string(current_module->data.output.active_level));
                          break;
      case MODULE_INPUT:  fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Pull:</B> %s</TD></TR>",
                                          gpio_pull_to_string(current_module->data.input.pull));
                          fprintf(pfDot, "<TR><TD ALIGN=\"LEFT\">&#8226; <B>Active Level:</B> %s</TD></TR>",
                                          level_to_string(current_module->data.input.active_level));
                          break;
      default:  log_error("ast_print_helper", 0, "Unknown module kind enum value '%d'", current_module->kind);
                break; // This won't be reached due to log_error exiting
    }
    
    fprintf(pfDot, "</TABLE>>];\n");
    current_module = current_module->next;
  }
  
  // Print edges from controller to modules
  current_module = dsl_node->modules_root;
  while(current_module != NULL){
    fprintf(pfDot, "  Controller -> Module%d;\n", current_module->node_id);
    current_module = current_module->next;
  }
  
  fprintf(pfDot, "}\n");
}


