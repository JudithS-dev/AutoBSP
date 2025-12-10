%{
  #include <stdio.h>
  
  #include "logging.h"
  
  int yylex();
  extern FILE *yyin;
  extern int yylineno;
  
  int yydebug = 0;
  void yyerror(const char *msg);
%}

%define parse.error verbose

%start START

%%

START: /* empty */

%%

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
