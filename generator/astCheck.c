#include "astCheck.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "astCheckSTM32F446RE.h"
#include "astEnums2Str.h"

static void ast_check_required_dsl_params(ast_dsl_builder_t* dsl_builder);
static void ast_check_required_module_params(ast_module_builder_t* module_builder);

void ast_check_unique_enabled_names(ast_dsl_node_t* dsl_node);
void ast_check_unique_enabled_pins(ast_dsl_node_t* dsl_node);

static void check_pin_conflict(const ast_module_node_t* module1, const pin_t* pin1, const ast_module_node_t* module2, const pin_t* pin2);
static bool is_c_keyword(const char* name);
static int get_line_nr_of_module(ast_dsl_node_t* dsl_node, const char* module_name);

static int compare_modules_by_pin(const ast_module_node_t *a, const ast_module_node_t *b);

/* -------------------------------------------- */
/*           Required parameter checks          */
/* -------------------------------------------- */

/**
 * @brief Checks if all required parameters in the DSL builder and its module builders are set.
 * 
 * @param dsl_builder Pointer to the DSL builder.
 * @note Logs an error and exits if any required field is not set.
 */
void ast_check_required_params(ast_dsl_builder_t* dsl_builder){
  if(dsl_builder == NULL)
    log_error("ast_check_required_params", 0, "DSL builder is NULL.");
  
  ast_check_required_dsl_params(dsl_builder);
  
  ast_module_builder_t* current_builder = dsl_builder->module_builders_root;
  while(current_builder != NULL){
    ast_check_required_module_params(current_builder);
    current_builder = current_builder->next;
  }
}

/**
 * @brief Checks if all required parameters of the DSL builder are set.
 * 
 * @param dsl_builder Pointer to the DSL builder.
 * 
 * @note Logs an error and exits if any required field is not set.
 */
void ast_check_required_dsl_params(ast_dsl_builder_t* dsl_builder){
  if(dsl_builder == NULL)
    log_error("ast_check_required_dsl_params", 0, "DSL builder is NULL.");
  
  if(dsl_builder->controller_set == false)
    log_error("ast_check_required_dsl_params", 0, "Required field 'controller' is not set in DSL.");
}

/**
 * @brief Checks if all required parameters of the AST module builder are set.
 * 
 * @param module_builder Pointer to the AST module builder.
 * 
 * @note Logs an error and exits if any required field is not set.
 * @note Currently, only name and pin are required fields.
 */
void ast_check_required_module_params(ast_module_builder_t* module_builder){
  if(module_builder == NULL)
    log_error("ast_check_required_module_params", 0, "AST module builder is NULL.");
  if(module_builder->module == NULL)
    log_error("ast_check_required_module_params", 0, "Module in AST module builder is NULL.");
  
  ast_module_node_t* module = module_builder->module;
  
  // Check common fields
  if(module->line_nr == 0)
    log_error("ast_check_required_module_params", 0, "Required field 'line_nr' is not set for module '%s'. This is an internal error.",
              module->name == NULL ? "<NULL>" : module->name);
  
  if(module_builder->name_set == false || module->name == NULL)
    log_error("ast_check_required_module_params", 0, "Required field 'name' is not set for module defined in line number %d.",
              module->line_nr);
  
  // Check if pins are set (if UART, tx_pin and rx_pin must be set)
  if(module->kind == MODULE_UART){
    if(module_builder->pin_set == false)
      log_error("ast_check_required_module_params", module->line_nr, "Required field 'tx_pin' is not set for UART module '%s'.", 
                module->name == NULL ? "<NULL>" : module->name);
    if(module_builder->rx_pin_set == false)
      log_error("ast_check_required_module_params", module->line_nr, "Required field 'rx_pin' is not set for UART module '%s'.", 
                module->name == NULL ? "<NULL>" : module->name);
  } else{
    if(module_builder->pin_set == false)
      log_error("ast_check_required_module_params", module->line_nr, "Required field 'pin' is not set for module '%s'.", 
                module->name == NULL ? "<NULL>" : module->name);
  }
  
  // Check kind-specific fields if needed
  // Check if parameters of uart are reasonable
  if(module->kind == MODULE_UART){
    ast_module_uart_t* uart_data = &module->data.uart;
    if(uart_data->baudrate == 0)
      log_error("ast_check_required_module_params", module->line_nr, "UART module '%s' has invalid baudrate '0'.", module->name);
    if(uart_data->baudrate < 1200 || uart_data->baudrate > 1000000)
      log_error("ast_check_required_module_params", module->line_nr, "UART module '%s' has unsupported baudrate '%u'. Supported range is 1200 to 1,000,000.",
                module->name, uart_data->baudrate);
    if(uart_data->databits < 5 || uart_data->databits > 9)
      log_error("ast_check_required_module_params", module->line_nr, "UART module '%s' has unsupported databits '%u'. Supported range is 5 to 9.",
                module->name, uart_data->databits);
    if(uart_data->stopbits != 1 && uart_data->stopbits != 2 && uart_data->stopbits != 1.5)
      log_error("ast_check_required_module_params", module->line_nr, "UART module '%s' has unsupported stopbits '%f'. Supported values are 1 or 2.",
                module->name, uart_data->stopbits);
  }
}


/* -------------------------------------------- */
/*               Uniqueness checks              */
/* -------------------------------------------- */
/**
 * @brief Checks all enabled modules for uniqueness and validity.
 * 
 * @param dsl_node Pointer to the DSL node.
 */
void ast_check_all_enabled_modules(ast_dsl_node_t* dsl_node){
  ast_check_unique_enabled_names(dsl_node);
  ast_check_unique_enabled_pins(dsl_node);
  
  // Perform microcontroller specific checks
  switch(dsl_node->controller){
    case STM32F446RE: ast_check_stm32f446re_valid_pins(dsl_node);
                      break;
    default:          log_error("ast_check_all_enabled_modules", 0, "Unsupported controller '%s' for uniqueness checks.", controller_to_string(dsl_node->controller));
  }
}

/**
 * @brief Checks if enabled module names are unique and not C keywords.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * @note Logs an error and exits if duplicate names or C keywords are found.
 */
void ast_check_unique_enabled_names(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_unique_enabled_names", 0, "DSL node is NULL.");
  if(dsl_node->modules_root == NULL)
    log_error("ast_check_unique_enabled_names", 0, "DSL node has no modules.");
  
  char** existing_names = NULL;
  unsigned int existing_names_count = 0;
  ast_module_node_t* current = dsl_node->modules_root;
  while(current != NULL){
    if(current->enable){
      // Check if name is a C keyword
      if(is_c_keyword(current->name))
        log_error("ast_check_unique_enabled_names", current->line_nr, "Module name '%s' is a C keyword.", current->name);
      
      // Check for duplicate names
      for(unsigned int i = 0; i < existing_names_count; i++){
        if(existing_names == NULL)
          log_error("ast_check_unique_enabled_names", 0, "Internal error: existing names list is NULL.");
        if(strcmp(existing_names[i], current->name) == 0)
          log_error("ast_check_unique_enabled_names", 0,
                    "Duplicate enabled module name '%s' found.\n"
                    "                           Module with that name was already defined at line %d.",
                    current->name,
                    get_line_nr_of_module(dsl_node, current->name));
      }
      
      // Add name to existing names list
      existing_names = (char**)realloc(existing_names, (existing_names_count + 1) * sizeof(char*));
      if(existing_names == NULL)
        log_error("ast_check_unique_enabled_names", 0, "Could not allocate memory for existing module names.");
      existing_names[existing_names_count] = strdup(current->name);
      if(existing_names[existing_names_count] == NULL)
        log_error("ast_check_unique_enabled_names", 0, "Could not allocate memory for existing module name.");
      existing_names_count++;
    }
    current = current->next;
  }
  
  // Free existing names list
  for(unsigned int i = 0; i < existing_names_count; i++){
    if(existing_names[i] == NULL)
      log_error("ast_check_unique_enabled_names", 0, "Internal error: existing name at index %u is NULL.", i);
    free(existing_names[i]);
    existing_names[i] = NULL;
  }
  free(existing_names);
}

/**
 * @brief Checks if enabled module pins are unique.
 * 
 * @param dsl_node Pointer to the DSL node.
 * 
 * @note Logs an error and exits if duplicate pins are found.
 * @note Supports both regular modules (single pin) and UART modules (tx_pin and rx_pin).
 * @note For UART modules, checks that tx_pin and rx_pin are different.
 */
void ast_check_unique_enabled_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_unique_enabled_pins", 0, "DSL node is NULL.");
  if(dsl_node->modules_root == NULL)
    log_error("ast_check_unique_enabled_pins", 0, "DSL node has no modules.");
  
  ast_module_node_t* current = dsl_node->modules_root;
  while(current != NULL){
    if(current->enable){
      // For UART modules, check that tx_pin and rx_pin are different
      if(current->kind == MODULE_UART){
        if(current->pin.port == current->data.uart.rx_pin.port &&
            current->pin.pin_number == current->data.uart.rx_pin.pin_number){
          log_error("ast_check_unique_enabled_pins", current->line_nr,
                    "UART module '%s' has the same pin for tx_pin and rx_pin (Port %c Pin %d).",
                    current->name, current->pin.port, current->pin.pin_number);
        }
      }
      
      // Check for duplicate pins with other enabled modules
      ast_module_node_t* checker = current->next;
      while(checker != NULL){
        if(checker->enable){
          // Check pins based on module type
          if(current->kind == MODULE_UART && checker->kind == MODULE_UART){
            // Both are UART modules -> check all combinations
            check_pin_conflict(current, &current->pin, checker, &checker->pin);
            check_pin_conflict(current, &current->pin, checker, &checker->data.uart.rx_pin);
            check_pin_conflict(current, &current->data.uart.rx_pin, checker, &checker->pin);
            check_pin_conflict(current, &current->data.uart.rx_pin, checker, &checker->data.uart.rx_pin);
          } else if(current->kind == MODULE_UART && checker->kind != MODULE_UART){
            // Current is UART, checker is regular module
            check_pin_conflict(current, &current->pin, checker, &checker->pin);
            check_pin_conflict(current, &current->data.uart.rx_pin, checker, &checker->pin);
          } else if(current->kind != MODULE_UART && checker->kind == MODULE_UART){
            // Current is regular module, checker is UART
            check_pin_conflict(current, &current->pin, checker, &checker->pin);
            check_pin_conflict(current, &current->pin, checker, &checker->data.uart.rx_pin);
          } else {
            // Both are regular modules
            check_pin_conflict(current, &current->pin, checker, &checker->pin);
          }
        }
        checker = checker->next;
      }
    }
    current = current->next;
  }
}

/**
 * @brief Helper function to check if two pins conflict.
 * 
 * @param module1 Pointer to the first module node for error reporting.
 * @param pin1 Pointer to the first pin.
 * @param module2 Pointer to the second module node for error reporting.
 * @param pin2 Pointer to the second pin.
 */
static void check_pin_conflict(const ast_module_node_t* module1, const pin_t* pin1, const ast_module_node_t* module2, const pin_t* pin2){
  if(pin1->port == pin2->port && pin1->pin_number == pin2->pin_number){
    log_error("ast_check_unique_enabled_pins", 0,
              "Duplicate enabled module pin found: Port %c Pin %d is used by both module '%s' (line %d) and module '%s' (line %d).",
              pin1->port,
              pin1->pin_number,
              module1->name,
              module1->line_nr,
              module2->name,
              module2->line_nr);
  }
}

/**
 * @brief Checks if the given name is a C language keyword.
 * 
 * @param name Name to check.
 * @return true if the name is a C keyword, false otherwise.
 * 
 * @note Does not log errors, just returns false if name is c-keyword.
 */
static bool is_c_keyword(const char* name){
  if(name == NULL) 
    log_error("is_c_keyword", 0, "Module name is NULL.");
  
  static const char* c_keywords[] = {
    "alignas", "alignof", "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int",
    "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct",
    "switch",  "typedef", "union", "unsigned", "void", "volatile", "while",
    "_Alignas", "_Alignof", "_Atomic", "_BitInt", "_Bool", "_Complex", "_Decimal128", "_Decimal32",
    "_Decimal64", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local",
    NULL
  };
  
  for(int i = 0; c_keywords[i] != NULL; i++){
    if(strcmp(name, c_keywords[i]) == 0)
      return true;
  }
  return false;
}

/**
 * @brief Retrieves the line number of the module with the given name from the DSL node.
 * 
 * @param dsl_node Pointer to the DSL node.
 * @param module_name Name of the module to search for.
 * @return Line number of the module if found.
 * 
 * @note Logs an error and exits if the module name is not found.
 */
static int get_line_nr_of_module(ast_dsl_node_t* dsl_node, const char* module_name){
  if(module_name == NULL)
    log_error("get_line_nr_of_module", 0, "Module name is NULL.");
  if(dsl_node == NULL)
    log_error("get_line_nr_of_module", 0, "AST root is NULL.");
  if(dsl_node->modules_root == NULL)
    log_error("get_line_nr_of_module", 0, "AST root has no modules.");
  
  ast_module_node_t* current = dsl_node->modules_root;
  while(current != NULL){
    if(strcmp(current->name, module_name) == 0)
      return current->line_nr;
    current = current->next;
  }
  
  log_error("get_line_nr_of_module", 0, "Module name '%s' not found in AST.", module_name);
  return -1; // This line will never be reached due to log_error exiting the program
}


/* -------------------------------------------- */
/*              Sorting of modules              */
/* -------------------------------------------- */

/**
 * @brief Sorts the modules in the DSL node by their pin (port and pin number).
 * 
 * @param dsl_node Pointer to the DSL node.
 * @note Uses insertion sort algorithm for simplicity.
 */
void ast_sort_modules_by_pin(ast_dsl_node_t *dsl_node){
  if(dsl_node == NULL)
    log_error("ast_sort_modules_by_pin", 0, "DSL node is NULL.");
  
  if(dsl_node->modules_root == NULL)
    log_error("ast_sort_modules_by_pin", 0, "DSL node has no modules.");
  
  if(dsl_node->modules_root->next == NULL)
    return; // Single element, already sorted
  
  ast_module_node_t *sorted = NULL;
  ast_module_node_t *current = dsl_node->modules_root;
  
  while(current != NULL){
    ast_module_node_t *next = current->next;
    
    // If sorted list is empty put current element as first 
    if(sorted == NULL){
      current->next = NULL;
      sorted = current;
      current = next;
      continue;
    }
    
    // If smaller than first element, insert at beginning
    if(compare_modules_by_pin(current, sorted) < 0){
      current->next = sorted;
      sorted = current;
      current = next;
      continue;
    }
    
    // Else, find correct position in sorted list
    ast_module_node_t *search = sorted;
    while(search->next != NULL && compare_modules_by_pin(search->next, current) < 0)
      search = search->next;
    
    // Insert current after last smaller element found
    current->next = search->next;
    search->next = current;
    
    current = next;
  }
  
  dsl_node->modules_root = sorted;
}

/**
 * @brief Compares two module nodes by their pin (port and pin number).
 * 
 * @param a Pointer to the first module node.
 * @param b Pointer to the second module node.
 * @return Negative value if a < b, positive value if a > b, zero if equal.
 */
static int compare_modules_by_pin(const ast_module_node_t *a, const ast_module_node_t *b){
  // Compare port (case-insensitive, A < B < C etc.)
  int port_diff = (int)a->pin.port - (int)b->pin.port;
  if(port_diff != 0)
    return port_diff;
  
  // Ports are equal, compare pin number
  if(a->pin.pin_number < b->pin.pin_number)
    return -1;
  else if (a->pin.pin_number > b->pin.pin_number)
    return 1;
  else
    return 0;
}

/* -------------------------------------------- */
/*     Backend specific parameter bindings      */
/* -------------------------------------------- */
/**
 * @brief Binds backend specific parameters based on the microcontroller.
 * 
 * @param dsl_node Pointer to the DSL node.
 */
void ast_bind_backend_specific_params(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_bind_backend_specific_params", 0, "DSL node is NULL.");
  
  // Perform microcontroller specific bindings
  switch(dsl_node->controller){
    case STM32F446RE: ast_check_stm32f446re_bind_pins(dsl_node);
                      break;
    default:          log_error("ast_bind_backend_specific_params", 0, "Unsupported controller '%s' for backend specific parameter bindings.", controller_to_string(dsl_node->controller));
  }
}