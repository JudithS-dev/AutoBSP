#include "astCheck.h"

#include <stdlib.h>
#include <string.h>
#include "logging.h"

static void ast_check_required_dsl_params(ast_dsl_builder_t* dsl_builder);
static void ast_check_required_module_params(ast_module_builder_t* module_builder);

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
  
  if(module_builder->pin_set == false)
    log_error("ast_check_required_module_params", module->line_nr, "Required field 'pin' is not set for module '%s'.", 
              module->name == NULL ? "<NULL>" : module->name);
  
  // Check kind-specific fields if needed
  // nothing for now as all fields are optional
}


/* -------------------------------------------- */
/*               Uniqueness checks              */
/* -------------------------------------------- */

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
 */
void ast_check_unique_enabled_pins(ast_dsl_node_t* dsl_node){
  if(dsl_node == NULL)
    log_error("ast_check_unique_enabled_pins", 0, "DSL node is NULL.");
  if(dsl_node->modules_root == NULL)
    log_error("ast_check_unique_enabled_pins", 0, "DSL node has no modules.");
  
  ast_module_node_t* current = dsl_node->modules_root;
  while(current != NULL){
    if(current->enable){
      ast_module_node_t* checker = current->next;
      while(checker != NULL){
        if(checker->enable){
          if(current->pin.port == checker->pin.port && current->pin.pin_number == checker->pin.pin_number){
            log_error("ast_check_unique_enabled_pins", 0,
                      "Duplicate enabled module pin found: Port %c Pin %d is used by both module '%s' (line %d) and module '%s' (line %d).",
                      current->pin.port,
                      current->pin.pin_number,
                      current->name,
                      current->line_nr,
                      checker->name,
                      checker->line_nr);
          }
        }
        checker = checker->next;
      }
    }
    current = current->next;
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