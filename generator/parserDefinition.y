%{
  #include <stdio.h>
  
  #include "logging.h"
  #include "moduleEnums.h"
  
  int yylex();
  extern FILE *yyin;
  extern int yylineno;
  
  int yydebug = 0;
  void yyerror(const char *msg);
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

START:  kw_autobsp '{' FILE_CONTENTS '}'
      | /* empty */

FILE_CONTENTS: GLOBAL_PARAM MODULE_DEFS

GLOBAL_PARAM: kw_controller ':' val_controller END

MODULE_DEFS:  MODULE_DEFS MODULE_DEF
            | MODULE_DEF

MODULE_DEF: kw_input '{' INPUT_PARAMS '}'
          | kw_output '{' OUTPUT_PARAMS '}'

INPUT_PARAMS: INPUT_PARAMS INPUT_PARAM END
            | INPUT_PARAM END

INPUT_PARAM:  NAME_PARAM
            | PIN_PARAM
            | ENABLE_PARAM
            | GPIO_PULL_PARAM
            | GPIO_ACTIVE_PARAM

OUTPUT_PARAMS:  OUTPUT_PARAMS OUTPUT_PARAM END
              | OUTPUT_PARAM END

OUTPUT_PARAM: NAME_PARAM  //TODO: free val_name after use
            | PIN_PARAM
            | ENABLE_PARAM
            | GPIO_TYPE_PARAM
            | GPIO_PULL_PARAM
            | GPIO_SPEED_PARAM
            | GPIO_INIT_PARAM
            | GPIO_ACTIVE_PARAM


NAME_PARAM: kw_name ':' val_name                    { $$ = $3;                       }

PIN_PARAM: kw_pin ':' val_pin                       { $$ = $3;                       }

ENABLE_PARAM: kw_enable ':' val_bool                { $$ = $3;                       }

GPIO_TYPE_PARAM: kw_gpio_type ':' val_gpio_type     { $$ = $3;                       }

GPIO_PULL_PARAM:  kw_gpio_pull ':' val_gpio_pull    { $$ = helper_to_gpio_pull($3);  }
                | kw_gpio_pull ':' val_none         { $$ = GPIO_PULL_NONE;           }

GPIO_SPEED_PARAM: kw_gpio_speed ':' val_gpio_speed  { $$ = helper_to_gpio_speed($3); }
                | kw_gpio_speed ':' val_level       { $$ = level_to_gpio_speed($3);  }

GPIO_INIT_PARAM:  kw_gpio_init ':' val_gpio_init    { $$ = helper_to_gpio_init($3);  }
                | kw_gpio_init ':' val_none         { $$ = GPIO_INIT_NONE;           }

GPIO_ACTIVE_PARAM: kw_gpio_active ':' val_level     { $$ = $3;                       }


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
  if(argc < 2 || argc > 3){
    fprintf(stderr, "\nERROR 'main': Incorrect number of arguments.\n              Usage: %s <codefile_name> [<logfile_name>]\n", argv[0]);
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
  
  // Determine log file path
  char *log_file;
  if(argc == 3) // Use provided log path
    log_file = argv[2];
  else // Default to "AutoBSP.log" in the current directory
    log_file = "AutoBSP.log";
  
  init_logging(log_file);
  
  // Parse the input code
  log_info("START", LOG_OTHER, 0, "Start parsing the DSL code in '%s'", code_file);
  int ret_parse = yyparse();
  log_info("END", LOG_OTHER, 0, "Finished parsing the DSL code");
  
  if(ret_parse != 0)
    log_error("main", 0, "Parsing failed with error code %d", ret_parse);
  
  // Clean up
  close_logging();
  fclose(input);
  
  return 0;
}
