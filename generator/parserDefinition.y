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
}

  /* -------------------------------------------- */
  /*       Definition of Datatypes (yylval)       */
  /* -------------------------------------------- */
%union{
  controller_t          u_controller;  // For val_controller
  char*                 u_str;         // For val_name
  pin_t                 u_pin;         // For val_pin
  bool_t                u_bool;        // For val_bool
  level_t               u_level;       // For val_level
  gpio_type_t           u_gpio_type;   // For val_gpio_type
  gpio_pull_helper_t    u_gpio_pull;   // For val_gpio_pull
  gpio_speed_helper_t   u_gpio_speed;  // For val_gpio_speed
  gpio_init_helper_t    u_gpio_init;   // For val_gpio_init
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
%token kw_gpio_type kw_gpio_pull kw_gpio_speed kw_gpio_init kw_gpio_active kw_gpio_enable
  
  /* ------------- Parameter values ------------- */
  /* Multiple used parameter values */
%token<u_bool>  val_bool
%token<u_level> val_level
%token val_none
  /* Supported microcontrollers */
%token<u_controller> val_controller
  /* GPIO specific parameter values */
%token<u_gpio_type>  val_gpio_type 
%token<u_gpio_pull>  val_gpio_pull 
%token<u_gpio_speed> val_gpio_speed
%token<u_gpio_init>  val_gpio_init 
  
  /* -------- Rules for dynamic patterns -------- */
%token<u_str> val_name 
%token<u_pin> val_pin

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
            | GPIO_PULL_PARAM
            | GPIO_ACTIVE_PARAM
            | GPIO_ENABLE_PARAM

OUTPUT_PARAMS:  OUTPUT_PARAMS OUTPUT_PARAM END
              | OUTPUT_PARAM END

OUTPUT_PARAM: NAME_PARAM
            | PIN_PARAM
            | GPIO_TYPE_PARAM
            | GPIO_PULL_PARAM
            | GPIO_SPEED_PARAM
            | GPIO_INIT_PARAM
            | GPIO_ACTIVE_PARAM
            | GPIO_ENABLE_PARAM

NAME_PARAM: kw_name ':' val_name  //TODO: free val_name after use

PIN_PARAM: kw_pin ':' val_pin

GPIO_TYPE_PARAM: kw_gpio_type ':' val_gpio_type

GPIO_PULL_PARAM:  kw_gpio_pull ':' val_gpio_pull
                | kw_gpio_pull ':' val_none

GPIO_SPEED_PARAM: kw_gpio_speed ':' val_gpio_speed
                | kw_gpio_speed ':' val_level

GPIO_INIT_PARAM:  kw_gpio_init ':' val_gpio_init
                | kw_gpio_init ':' val_none

GPIO_ACTIVE_PARAM: kw_gpio_active ':' val_level

GPIO_ENABLE_PARAM: kw_gpio_enable ':' val_bool


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
