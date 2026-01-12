%{
  #include <stdio.h>
  
  #include "logging.h"
  #include "astEnums.h"
  #include "astEnums2Str.h"
  #include "astBuild.h"
  #include "astCheck.h"
  #include "astPrint.h"
  #include "astGenerate.h"
  
  int yylex();
  extern FILE *yyin;
  extern int yylineno;
  
  int yydebug = 0;
  void yyerror(const char *msg);
  
  ast_dsl_builder_t *dsl_builder = NULL;
  ast_module_builder_t *current_module_builder = NULL;
%}

%define parse.error verbose

  /* -------------------------------------------- */
  /*               Type definitions               */
  /* -------------------------------------------- */
%code requires{
  typedef enum{
    GPIO_HELPER_PULL_UP,
    GPIO_HELPER_PULL_DOWN
  } gpio_pull_helper_t;
  
  typedef enum{
    GPIO_HELPER_SPEED_MEDIUM,
    GPIO_HELPER_SPEED_VERY_HIGH
  } gpio_speed_helper_t;
  
  typedef enum{
    GPIO_HELPER_INIT_ON,
    GPIO_HELPER_INIT_OFF
  } gpio_init_helper_t;
  
  /* Conversion helpers */
  static inline gpio_pull_t helper_to_gpio_pull(gpio_pull_helper_t h){
    switch(h){
      case GPIO_HELPER_PULL_UP:   return GPIO_PULL_UP;
      case GPIO_HELPER_PULL_DOWN: return GPIO_PULL_DOWN;
    }
    log_error("helper_to_gpio_pull", 0, "Invalid gpio_pull_helper_t value '%d'", h);
    return GPIO_PULL_NONE;  // This won't be reached due to log_error exiting
  }
  
  static inline gpio_speed_t helper_to_gpio_speed(gpio_speed_helper_t h){
    switch(h){
      case GPIO_HELPER_SPEED_MEDIUM:    return GPIO_SPEED_MEDIUM;
      case GPIO_HELPER_SPEED_VERY_HIGH: return GPIO_SPEED_VERY_HIGH;
    }
    log_error("helper_to_gpio_speed", 0, "Invalid gpio_speed_helper_t value '%d'", h);
    return GPIO_SPEED_LOW;  // This won't be reached due to log_error exiting
  }
  
  static inline gpio_speed_t level_to_gpio_speed(level_t l){
    switch(l){
      case LOW:    return GPIO_SPEED_LOW;
      case HIGH:   return GPIO_SPEED_HIGH;
    }
    log_error("level_to_gpio_speed", 0, "Invalid level_t value '%d'", l);
    return GPIO_SPEED_LOW;  // This won't be reached due to log_error exiting
  }
  
  static inline gpio_init_t helper_to_gpio_init(gpio_init_helper_t h){
    switch(h){
      case GPIO_HELPER_INIT_ON:  return GPIO_INIT_ON;
      case GPIO_HELPER_INIT_OFF: return GPIO_INIT_OFF;
    }
    log_error("helper_to_gpio_init", 0, "Invalid gpio_init_helper_t value '%d'", h);
    return GPIO_INIT_OFF;  // This won't be reached due to log_error exiting
  }
}

  /* -------------------------------------------- */
  /*       Definition of Datatypes (yylval)       */
  /* -------------------------------------------- */
%union{
  controller_t          u_controller;         // For val_controller
  char*                 u_str;                // For val_name
  pin_t                 u_pin;                // For val_pin
  bool                  u_bool;               // For val_bool
  level_t               u_level;              // For val_level
  gpio_type_t           u_gpio_type;          // For val_gpio_type
  gpio_pull_helper_t    u_helper_gpio_pull;   // For val_gpio_pull
  gpio_pull_t           u_gpio_pull;          // For parser use
  gpio_speed_helper_t   u_helper_gpio_speed;  // For val_gpio_speed
  gpio_speed_t          u_gpio_speed;         // For parser use
  gpio_init_helper_t    u_helper_gpio_init;   // For val_gpio_init
  gpio_init_t           u_gpio_init;          // For parser use
}

%start START

  /* -------------------------------------------- */
  /*               Token Definitions              */
  /* -------------------------------------------- */
  
  /* -------------- File structure -------------- */
%token kw_autobsp kw_output kw_input
  
  /* -------------- Parameter names ------------- */
  /* Multiple used parameter names */
%token kw_controller kw_name kw_pin
  /* GPIO specific parameter names */
%token kw_gpio_type kw_gpio_pull kw_gpio_speed kw_gpio_init kw_gpio_active kw_enable
  
  /* ------------- Parameter values ------------- */
  /* Multiple used parameter values */
%token <u_bool>  val_bool
%token <u_level> val_level
%token val_none
  /* Supported microcontrollers */
%token <u_controller> val_controller
  /* GPIO specific parameter values */
%token <u_gpio_type>  val_gpio_type 
%token <u_helper_gpio_pull>  val_gpio_pull 
%token <u_helper_gpio_speed> val_gpio_speed
%token <u_helper_gpio_init>  val_gpio_init 
  
  /* -------- Rules for dynamic patterns -------- */
%token <u_str> val_name 
%token <u_pin> val_pin

  /* -------------------------------------------- */
  /*   Definition of datatypes of non-terminals   */
  /* -------------------------------------------- */
%type <u_controller> CONTROLLER_PARAM
%type <u_str>        NAME_PARAM
%type <u_pin>        PIN_PARAM
%type <u_bool>       ENABLE_PARAM
%type <u_gpio_type>  GPIO_TYPE_PARAM
%type <u_gpio_pull>  GPIO_PULL_PARAM
%type <u_gpio_speed> GPIO_SPEED_PARAM
%type <u_gpio_init>  GPIO_INIT_PARAM
%type <u_level>      GPIO_ACTIVE_PARAM

%%

  /* -------------------------------------------- */
  /*                Grammar rules                 */
  /* -------------------------------------------- */

START:  kw_autobsp  { if(dsl_builder != NULL)
                        log_error("START", 0, "DSL builder has already been set.");
                      dsl_builder = ast_new_dsl_builder();
                    }
        '{' FILE_CONTENTS '}'
      | /* empty */

FILE_CONTENTS: GLOBAL_PARAMS MODULE_DEFS

GLOBAL_PARAMS: GLOBAL_PARAMS GLOBAL_PARAM END
              | GLOBAL_PARAM END

GLOBAL_PARAM: CONTROLLER_PARAM  { if(dsl_builder == NULL)
                                    log_error("GLOBAL_PARAM", yylineno, "DSL builder is NULL when setting controller.");
                                  ast_dsl_builder_set_controller(yylineno, dsl_builder, $1);
                                }

CONTROLLER_PARAM: kw_controller ':' val_controller  { $$ = $3;
                                                      log_info("CONTROLLER_PARAM", LOG_PARSER_FOUND, yylineno, "Found controller parameter with value '%s'", controller_to_string($3));
                                                    }

MODULE_DEFS:  MODULE_DEFS MODULE_DEF
            | MODULE_DEF

MODULE_DEF: kw_input  { /* Start new input module builder */
                        log_info("MODULE_DEF", LOG_PARSER_FOUND, yylineno, "Found input module definition.");
                        if(current_module_builder != NULL)
                          log_error("MODULE_DEF", yylineno, "Previous module builder not finalized before starting new input module.");
                        current_module_builder = ast_new_module_builder(yylineno);
                        ast_module_builder_set_kind(yylineno, current_module_builder, MODULE_INPUT);
                      }
            '{' INPUT_PARAMS '}'  { /* Append the current module builder to the DSL builder */
                                    ast_dsl_builder_append_module_builder(yylineno, dsl_builder, current_module_builder);
                                    current_module_builder = NULL;
                                  }
          | kw_output { /* Start new output module builder */
                        log_info("MODULE_DEF", LOG_PARSER_FOUND, yylineno, "Found output module definition.");
                        if(current_module_builder != NULL)
                          log_error("MODULE_DEF", yylineno, "Previous module builder not finalized before starting new output module.");
                        current_module_builder = ast_new_module_builder(yylineno);
                        ast_module_builder_set_kind(yylineno, current_module_builder, MODULE_OUTPUT);
                      }
              '{' OUTPUT_PARAMS '}' { /* Append the current module builder to the DSL builder */
                                      ast_dsl_builder_append_module_builder(yylineno, dsl_builder, current_module_builder);
                                      current_module_builder = NULL;
                                    }

INPUT_PARAMS: INPUT_PARAMS INPUT_PARAM END
            | INPUT_PARAM END

INPUT_PARAM:  NAME_PARAM        { if(!current_module_builder)
                                    log_error("INPUT_PARAM", yylineno, "No current module builder to set name.");
                                  ast_module_builder_set_name(yylineno, current_module_builder, $1);
                                  if($1)
                                    free($1); // Free the in the lexer allocated string
                                  else
                                    log_error("INPUT_PARAM", yylineno, "INPUT_PARAM: Name parameter is NULL.");
                                }
            | PIN_PARAM         { if(!current_module_builder)
                                    log_error("INPUT_PARAM", yylineno, "No current module builder to set pin.");
                                  ast_module_builder_set_pin(yylineno, current_module_builder, $1);
                                }
            | ENABLE_PARAM      { if(!current_module_builder)
                                    log_error("INPUT_PARAM", yylineno, "No current module builder to set enable.");
                                  ast_module_builder_set_enable(yylineno, current_module_builder, $1);
                                }
            | GPIO_PULL_PARAM   { if(!current_module_builder)
                                    log_error("INPUT_PARAM", yylineno, "No current module builder to set GPIO pull.");
                                  ast_module_builder_set_input_pull(yylineno, current_module_builder, $1);
                                }
            | GPIO_ACTIVE_PARAM { if(!current_module_builder)
                                    log_error("INPUT_PARAM", yylineno, "No current module builder to set GPIO active level.");
                                  ast_module_builder_set_input_active_level(yylineno, current_module_builder, $1);
                                }

OUTPUT_PARAMS:  OUTPUT_PARAMS OUTPUT_PARAM END
              | OUTPUT_PARAM END

OUTPUT_PARAM: NAME_PARAM          { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set name.");
                                    ast_module_builder_set_name(yylineno, current_module_builder, $1);
                                    if($1)
                                      free($1); // Free the in the lexer allocated string
                                    else
                                      log_error("OUTPUT_PARAM", yylineno, "OUTPUT_PARAM: Name parameter is NULL.");
                                  }
            | PIN_PARAM           { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set pin.");
                                    ast_module_builder_set_pin(yylineno, current_module_builder, $1);
                                  }
            | ENABLE_PARAM        { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set enable.");
                                    ast_module_builder_set_enable(yylineno, current_module_builder, $1);
                                  }
            | GPIO_TYPE_PARAM     { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set GPIO type.");
                                    ast_module_builder_set_output_type(yylineno, current_module_builder, $1);
                                  }
            | GPIO_PULL_PARAM     { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set GPIO pull.");
                                    ast_module_builder_set_output_pull(yylineno, current_module_builder, $1);
                                  }
            | GPIO_SPEED_PARAM    { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set GPIO speed.");
                                    ast_module_builder_set_output_speed(yylineno, current_module_builder, $1);
                                  }
            | GPIO_INIT_PARAM     { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set GPIO init.");
                                    ast_module_builder_set_output_init(yylineno, current_module_builder, $1);
                                  }
            | GPIO_ACTIVE_PARAM   { if(!current_module_builder)
                                      log_error("OUTPUT_PARAM", yylineno, "No current module builder to set GPIO active level.");
                                    ast_module_builder_set_output_active_level(yylineno, current_module_builder, $1);
                                  }


NAME_PARAM: kw_name ':' val_name                    { $$ = $3;
                                                      log_info("NAME_PARAM", LOG_PARSER_FOUND, yylineno, "Found name parameter with value '%s'", $3);
                                                    }

PIN_PARAM: kw_pin ':' val_pin                       { $$ = $3;
                                                      log_info("PIN_PARAM", LOG_PARSER_FOUND, yylineno, "Found pin parameter with value '%s'", pin_to_string($3));
                                                    }

ENABLE_PARAM: kw_enable ':' val_bool                { $$ = $3;
                                                      log_info("ENABLE_PARAM", LOG_PARSER_FOUND, yylineno, "Found enable parameter with value '%s'", bool_to_string($3));
                                                    }

GPIO_TYPE_PARAM: kw_gpio_type ':' val_gpio_type     { $$ = $3;
                                                      log_info("GPIO_TYPE_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO type parameter with value '%s'", gpio_type_to_string($3));
                                                    }

GPIO_PULL_PARAM:  kw_gpio_pull ':' val_gpio_pull    { $$ = helper_to_gpio_pull($3);
                                                      log_info("GPIO_PULL_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO pull parameter with value '%s'", gpio_pull_to_string($$));
                                                    }
                | kw_gpio_pull ':' val_none         { $$ = GPIO_PULL_NONE;
                                                      log_info("GPIO_PULL_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO pull parameter with value '%s'", gpio_pull_to_string($$));
                                                    }

GPIO_SPEED_PARAM: kw_gpio_speed ':' val_gpio_speed  { $$ = helper_to_gpio_speed($3);
                                                      log_info("GPIO_SPEED_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO speed parameter with value '%s'", gpio_speed_to_string($$));
                                                    }
                | kw_gpio_speed ':' val_level       { $$ = level_to_gpio_speed($3);
                                                      log_info("GPIO_SPEED_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO speed parameter with value '%s'", gpio_speed_to_string($$));
                                                    }

GPIO_INIT_PARAM:  kw_gpio_init ':' val_gpio_init    { $$ = helper_to_gpio_init($3);
                                                      log_info("GPIO_INIT_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO init parameter with value '%s'", gpio_init_to_string($$));
                                                    }
                | kw_gpio_init ':' val_none         { $$ = GPIO_INIT_NONE;
                                                      log_info("GPIO_INIT_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO init parameter with value '%s'", gpio_init_to_string($$));
                                                    }

GPIO_ACTIVE_PARAM: kw_gpio_active ':' val_level     { $$ = $3;
                                                      log_info("GPIO_ACTIVE_PARAM", LOG_PARSER_FOUND, yylineno, "Found GPIO active level parameter with value '%s'", level_to_string($$));
                                                    }


END: ';'
    | /* empty */

%%

  /* -------------------------------------------- */
  /*                  C functions                 */
  /* -------------------------------------------- */

void yyerror(const char *msg){
  printf("Error in line %d: %s\n", yylineno, msg);
}

int main(int argc, char *argv[]){
  // Check for correct number of arguments (./AutoBSP <codefile> [<output_path>])
  if(argc < 2 || argc > 3){
    fprintf(stderr, "\nERROR 'main': Incorrect number of arguments.\n              Usage: %s <codefile_name> [<output_path>]\n", argv[0]);
    return 1;
  }
  
  // Get the program file path
  char *code_file = argv[1];
  FILE *input = fopen(code_file, "r");
  if(!input){
    fprintf(stderr, "\nERROR 'main': Error opening code file '%s'\n", code_file);
    return 1;
  }
  yyin = input;
  
  // Determine path for all output files
  char *output_path;
  if(argc == 3) // Use provided output path
    output_path = argv[2];
  else // Default to "output" directory
    output_path = "output";
  
  // Initialize logging
  init_logging(output_path);
  
  // Parse the input code
  log_info("START", LOG_OTHER, 0, "Start parsing the DSL code in '%s'", code_file);
  int ret_parse = yyparse();
  log_info("END", LOG_OTHER, 0, "Finished parsing the DSL code");
  
  if(ret_parse != 0)
    log_error("main", 0, "Parsing failed with error code %d", ret_parse);
  
  if(dsl_builder == NULL)
    log_error("main", 0, "Failed to generate DSL builder from parsed code.");
  
  // Check for required parameters in the DSL builder
  log_info("main", LOG_OTHER, 0, "Performing DSL builder checks for required parameters");
  ast_check_required_params(dsl_builder);
  
  // Build the AST from the DSL builder (and also frees the builders)
  log_info("main", LOG_OTHER, 0, "Building the AST from the DSL builder");
  ast_dsl_node_t* ast_root = ast_convert_dsl_builder_to_dsl_node(dsl_builder);
  
  // Check if enabled names are unique
  log_info("main", LOG_OTHER, 0, "Checking enabled modules for unique names in the AST");
  ast_check_unique_enabled_names(ast_root);
  
  // Check if enabled pins are unique
  log_info("main", LOG_OTHER, 0, "Checking enabled modules for unique pins in the AST");
  ast_check_unique_enabled_pins(ast_root);
  
  // Sort modules by pin number for better readability
  log_info("main", LOG_OTHER, 0, "Sorting modules by pin number for better readability");
  ast_sort_modules_by_pin(ast_root);
  
  // Print the generated AST
  log_info("main", LOG_OTHER, 0, "Printing the generated AST to DOT and PNG files");
  ast_print(output_path, ast_root);
  
  // Generate code from the AST
  log_info("main", LOG_OTHER, 0, "Generating code from the AST");
  ast_generate_code(output_path, ast_root);
  
  // Clean up
  ast_free_dsl_node(ast_root);
  close_logging();
  fclose(input);
  
  return 0;
}
