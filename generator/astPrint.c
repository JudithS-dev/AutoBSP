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
  
  FILE *file_dot = fopen("ast_graph.gv", "w");
  ast_print_helper(file_dot, dsl_node);
  fclose(file_dot);
}

/**
 * @brief Helper function to print the AST in DOT format.
 * 
 * @param pfDot File pointer to the DOT file.
 * @param dsl_node Pointer to the DSL AST node to be printed.
 */
static void ast_print_helper(FILE *pfDot, const dsl_node_t* dsl_node){
  fprintf(pfDot, "digraph AST {\n");
  fprintf(pfDot, "  node [shape=box];\n");
  
  // Print controller node
  fprintf(pfDot, "  Controller [label=\"Controller: %s\"];\n", controller_to_string(dsl_node->controller));
  
  // Print modules
  module_node_t *current_module = dsl_node->modules_root;
  while(current_module != NULL){
    // Print general module info
    char *pin_str = pin_to_string(current_module->pin);
    
    fprintf(pfDot, "  Module%d [label=\"", current_module->node_id);
    fprintf(pfDot, "Module ID: %d\\n",  current_module->node_id);
    fprintf(pfDot, "Name: %s\\n",       current_module->name);
    fprintf(pfDot, "Pin: %s\\n",        pin_str);
    fprintf(pfDot, "Enable: %s\\n",     bool_to_string(current_module->enable));
    fprintf(pfDot, "Kind: %s",          kind_to_string(current_module->kind));
    
    free(pin_str);
    
    // Print module-specific data
    switch(current_module->kind){
      case MODULE_OUTPUT: fprintf(pfDot, "\\nType: %s",        gpio_type_to_string(current_module->data.output.type));
                          fprintf(pfDot, "\\nPull: %s",        gpio_pull_to_string(current_module->data.output.pull));
                          fprintf(pfDot, "\\nSpeed: %s",       gpio_speed_to_string(current_module->data.output.speed));
                          fprintf(pfDot, "\\nInit: %s",        gpio_init_to_string(current_module->data.output.init));
                          fprintf(pfDot, "\\nActive Level: %s",level_to_string(current_module->data.output.active_level));
                          break;
      case MODULE_INPUT:  fprintf(pfDot, "\\nPull: %s",         gpio_pull_to_string(current_module->data.input.pull));
                          fprintf(pfDot, "\\nActive Level: %s", level_to_string(current_module->data.input.active_level));
                          break;
      default:  log_error("ast_print_helper", 0, "Unknown module kind enum value '%d'", current_module->kind);
                break; // This won't be reached due to log_error exiting
    }
    
    fprintf(pfDot, "\"];\n");
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