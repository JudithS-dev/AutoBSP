#include "astChecking.h"

#include <stdlib.h>
#include <string.h>
#include "logging.h"

char** existing_names = NULL;
unsigned int existing_names_count = 0;

extern dsl_node_t *ast_root; // Declared in parserDefiniton.y used for error reporting on module name duplicates

static int get_line_nr_of_module(const char* module_name);
static void check_module_name(module_node_t* module);
static bool is_c_keyword(const char* name);

/**
 * @brief Checks if the module being built in the AST module builder is complete.
 * 
 * @param builder Pointer to the AST module builder.
 * @note Logs an error and exits if any required field is not set.
 * @note Currently, only name and pin are required fields.
 */
void ast_check_module(ast_module_builder_t* builder){
  if(builder == NULL)
    log_error("ast_check_module", 0, "AST module builder is NULL.");
  if(builder->p_current_module == NULL)
    log_error("ast_check_module", 0, "Current module in AST module builder is NULL.");
  
  module_node_t* module = builder->p_current_module;
  
  // Check common fields
  if(module->line_nr == 0)
    log_error("ast_check_module", 0, "Module line number is not set for module '%s'.", 
              module->name == NULL ? "<NULL>" : module->name);
  
  if(builder->name_set == false || module->name == NULL)
    log_error("ast_check_module", 0, "Module name is not set for module ID '%d'.", module->node_id);
  check_module_name(module); // Check if name is valid and unique
  
  if(builder->pin_set == false)
    log_error("ast_check_module", 0, "Module pin is not set for module '%s'.", 
              module->name == NULL ? "<NULL>" : module->name);
  
  // Check kind-specific fields if needed
  // nothing for now as all fields are optional
}

/**
 * @brief Checks if the module name is valid and unique.
 * 
 * @param module Pointer to the module node to check.
 * @note Logs an error and exits if the name is a C keyword or if it is a duplicate.
 */
static void check_module_name(module_node_t* module){
  if(module == NULL)
    log_error("check_module_name", 0, "Module is NULL.");
  if(module->name == NULL)
    log_error("check_module_name", 0, "Module name is NULL.");
  
  // Check if name is a C keyword
  if(is_c_keyword(module->name))
    log_error("check_module_name", 0, "Module name '%s' is a C keyword.", module->name);
  
  // Check for duplicate names
  for(size_t i = 0; i < existing_names_count; i++){
    if(strcmp(existing_names[i], module->name) == 0)
      log_error("check_module_name", 0, "Duplicate module name '%s' found at module defined at line %d.\n"
                "                           Module with that name was already defined at line %d.",
                module->name,
                module->line_nr,
                get_line_nr_of_module(module->name));
  }
  
  // Add name to existing names list
  existing_names = (char**)realloc(existing_names, (existing_names_count + 1) * sizeof(char*));
  if(existing_names == NULL)
    log_error("ast_check_module", 0, "Could not allocate memory for existing module names.");
  existing_names[existing_names_count] = strdup(module->name);
  if(existing_names[existing_names_count] == NULL)
    log_error("ast_check_module", 0, "Could not allocate memory for existing module name.");
  existing_names_count++;
}

/**
 * @brief Checks if the given name is a C language keyword.
 * 
 * @param name Name to check.
 * @return true if the name is a C keyword, false otherwise.
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
 * @brief Retrieves the line number of the module with the given name from the AST root.
 * 
 * @param module_name Name of the module to search for.
 * @return Line number of the module if found.
 * @note Logs an error and exits if the module name is not found.
 */
static int get_line_nr_of_module(const char* module_name){
  if(module_name == NULL)
    log_error("get_line_nr_of_module", 0, "Module name is NULL.");
  if(ast_root == NULL)
    log_error("get_line_nr_of_module", 0, "AST root is NULL.");
  if(ast_root->modules_root == NULL)
    log_error("get_line_nr_of_module", 0, "AST root has no modules.");
  
  module_node_t* current = ast_root->modules_root;
  while(current != NULL){
    if(strcmp(current->name, module_name) == 0)
      return current->line_nr;
    current = current->next;
  }
  
  log_error("get_line_nr_of_module", 0, "Module name '%s' not found in AST.", module_name);
  return -1; // This line will never be reached due to log_error exiting the program
}

/**
 * @brief Deletes the list of existing module names.
 * 
 * @note Frees all allocated memory for existing names.
 */
void ast_check_free(){
  if(existing_names == NULL)
    log_error("ast_check_free", 0, "Existing names list is NULL.");
  
  for(size_t i = 0; i < existing_names_count; i++){
    if(existing_names[i] == NULL)
      log_error("ast_check_free", 0, "Existing name at index %u is NULL.", i);
    free(existing_names[i]);
    existing_names[i] = NULL;
  }
  
  free(existing_names);
  existing_names = NULL;
  existing_names_count = 0;
}
