#include "astPrint.h"

#include "logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* --------------- AST Printing --------------- */
static void ast_print_helper(FILE *pfDot, const dsl_node_t* dsl_node);

/* ----------- Conversion functions ----------- */
static char* pin_to_string(pin_t pin);
static const char* controller_to_string(controller_t controller);
static const char* bool_to_string(bool value);
static const char* kind_to_string(module_kind_t kind);
static const char* gpio_type_to_string(gpio_type_t type);
static const char* gpio_pull_to_string(gpio_pull_t pull);
static const char* gpio_speed_to_string(gpio_speed_t speed);
static const char* gpio_init_to_string(gpio_init_t init);
static const char* level_to_string(level_t level);


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


/* -------------------------------------------- */
/*             Conversion functions             */
/* -------------------------------------------- */

/**
 * @brief Converts pin structure to string.
 * 
 * @param pin Pin structure.
 * @return String representation of the pin.
 * 
 * @note The returned string is dynamically allocated and should be freed by the caller.
 */
static char* pin_to_string(pin_t pin){
  char buffer[20];
  char* result;
  switch(pin.identifier){
    case GPIOn: snprintf(buffer, sizeof(buffer), "GPIO%d", pin.pin_number);
                result = strdup(buffer);
                if(result == NULL)
                  log_error("pin_to_string", 0, "Memory allocation failed for pin string.");
                return result;
    case PXn:   snprintf(buffer, sizeof(buffer), "P%c%d", pin.port, pin.pin_number);
                result = strdup(buffer);
                if(result == NULL)
                  log_error("pin_to_string", 0, "Memory allocation failed for pin string.");
                return result;
    default:  log_error("pin_to_string", 0, "Unknown pin identifier enum value '%d'", pin.identifier);
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts controller enum to string.
 * 
 * @param controller Controller enum value.
 * @return Corresponding string representation.
 */
static const char* controller_to_string(controller_t controller){
  switch(controller){
    case STM32F446RE: return "STM32F446RE";
    default:  log_error("controller_to_string", 0, "Unknown controller enum value '%d'", controller); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts boolean value to string.
 * 
 * @param value Boolean value.
 * @return Corresponding string representation ("true" or "false").
 */
static const char* bool_to_string(bool value){
  if(value)
    return "true";
  else
    return "false";
}

/**
 * @brief Converts module kind enum to string.
 * 
 * @param kind Module kind enum value.
 * @return Corresponding string representation.
 */
static const char* kind_to_string(module_kind_t kind){
  switch(kind){
    case MODULE_OUTPUT: return "OUTPUT";
    case MODULE_INPUT:  return "INPUT";
    default:  log_error("kind_to_string", 0, "Unknown module kind enum value '%d'", kind); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO type enum to string.
 * 
 * @param type GPIO type enum value.
 * @return Corresponding string representation.
 */
static const char* gpio_type_to_string(gpio_type_t type){
  switch(type){
    case GPIO_TYPE_PUSHPULL:    return "Push-Pull";
    case GPIO_TYPE_OPENDRAIN:   return "Open-Drain";
    default:  log_error("gpio_type_to_string", 0, "Unknown GPIO type enum value '%d'", type); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO pull enum to string.
 * 
 * @param pull GPIO pull enum value.
 * @return Corresponding string representation.
 */
static const char* gpio_pull_to_string(gpio_pull_t pull){
  switch(pull){
    case GPIO_PULL_UP:      return "Pull-Up";
    case GPIO_PULL_DOWN:    return "Pull-Down";
    case GPIO_PULL_NONE:    return "No Pull";
    default:  log_error("gpio_pull_to_string", 0, "Unknown GPIO pull enum value '%d'", pull); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO speed enum to string.
 * 
 * @param speed GPIO speed enum value.
 * @return Corresponding string representation.
 */
static const char* gpio_speed_to_string(gpio_speed_t speed){
  switch(speed){
    case GPIO_SPEED_LOW:        return "Low";
    case GPIO_SPEED_MEDIUM:     return "Medium";
    case GPIO_SPEED_HIGH:       return "High";
    case GPIO_SPEED_VERY_HIGH:  return "Very High";
    default:  log_error("gpio_speed_to_string", 0, "Unknown GPIO speed enum value '%d'", speed); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO init enum to string.
 * 
 * @param init GPIO init enum value.
 * @return Corresponding string representation.
 */
static const char* gpio_init_to_string(gpio_init_t init){
  switch(init){
    case GPIO_INIT_ON:     return "ON";
    case GPIO_INIT_OFF:    return "OFF";
    case GPIO_INIT_NONE:   return "No Init";
    default:  log_error("gpio_init_to_string", 0, "Unknown GPIO init enum value '%d'", init); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts level enum to string.
 * 
 * @param level Level enum value.
 * @return Corresponding string representation.
 */
static const char* level_to_string(level_t level){
  switch(level){
    case LOW:   return "LOW";
    case HIGH:  return "HIGH";
    default:  log_error("level_to_string", 0, "Unknown level enum value '%d'", level); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}