%{
 #include <stdio.h>

 #include "lexer_helper.h"
 #include "logging.h"

 #include "parserInC.h"
%}

  /* activate line number tracking, stack usage (for context switches), and disable unused functions */
%option yylineno stack noyywrap noinput nounput noyy_top_state

%x COMMENT_LINE 
%x COMMENT_BLOCK

  /* -------------------------------------------- */
  /*               Dynamic patterns               */
  /* -------------------------------------------- */

  /*  Multiple used patterns*/
NAME_PATTERN        \"[A-Za-z_][A-Za-z0-9_]*\"
PIN_PATTERN         P[A-Z]((0-9)|1[0-5])

  /* Comment patterns */
COM_LINE            \/\/
COM_BLOCK_START     \/\*
COM_BLOCK_END       \*\/

  /* Special chars */
SINGLE_CHARS        [\{\}:]
WHITESPACE          [\ \n\t]+
COMMENT_CHARS       [\/\*]
INVALID_TOKENS      ([^ \n\t\{\}:\/\*]+|{COMMENT_CHARS})

%%
  /* Rules */
  /* -------------------------------------------- */
  /*          Rules for context switching         */
  /* -------------------------------------------- */

  /* Line comments */
{COM_LINE}                  { BEGIN(COMMENT_LINE);     }
<COMMENT_LINE>\n            { BEGIN(INITIAL);          }
<COMMENT_LINE>.             { /* ignore everything  */ }

  /* Block comments */
<INITIAL,COMMENT_BLOCK>{COM_BLOCK_START}    { yy_push_state(YYSTATE);
                                              BEGIN(COMMENT_BLOCK);   }
<COMMENT_BLOCK>{COM_BLOCK_END}              { yy_pop_state();         }
<COMMENT_BLOCK>(.|\n)                       { /* ignore everything */ }



  /* -------------------------------------------- */
  /*           Rules for fixed patterns           */
  /* -------------------------------------------- */

  /* -------------- File structure -------------- */
"AutoBSP"           { log_info("\"AutoBSP\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_autobsp;
                    }
"OUTPUT"            { log_info("\"OUTPUT\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_output;
                    }
"INPUT"             { log_info("\"INPUT\"",   LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_input;
                    }

  /* -------------- Parameter names -------------- */
  /* Multiple used parameter names */
"controller"        { log_info("\"controller\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_controller;
                    }
"name"              { log_info("\"name\"",       LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_name;
                    }
"pin"               { log_info("\"pin\"",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_pin;
                    }

  /* GPIO specific parameter names */
"type"              { log_info("\"type\"",   LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_type;
                    }
"pull"              { log_info("\"pull\"",   LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_pull;
                    }
"speed"             { log_info("\"speed\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_speed;
                    }
"init"              { log_info("\"init\"",   LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_init;
                    }
"active"            { log_info("\"active\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_active;
                    }
"enable"            { log_info("\"enable\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_gpio_enable;
                    }
  /* -------------- Parameter values ------------- */
  /* Multiple used parameter values */
  /* Used for 'enable' and '?lock?' */
"true"              { log_info("\"true\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_bool;
                    }
"false"             { log_info("\"false\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_bool;
                    }
  /* Used for 'active' and 'speed' */
"low"               { log_info("\"low\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_level;
                    }
"high"              { log_info("\"high\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_level;
                    }
  /* Used for 'pull' 'init' and '?trigger?' */
"none"              { log_info("\"none\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_none;
                    }

  /* Supported microcontrollers */
"STM32F446RE"       { log_info("\"STM32F446RE\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_controller;
                    }

  /* GPIO specific parameter values */
"pushpull"          { log_info("\"pushpull\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_type;
                    }
"opendrain"         { log_info("\"opendrain\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_type;
                    }
"up"                { log_info("\"up\"",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_pull;
                    }
"down"              { log_info("\"down\"",      LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_pull;
                    }
"medium"            { log_info("\"medium\"",    LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_speed;
                    }
"very_high"         { log_info("\"very_high\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_speed;
                    }
"on"                { log_info("\"on\"",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_init;
                    }
"off"               { log_info("\"off\"",       LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_gpio_init;
                    }

  /* -------------------------------------------- */
  /*           Rules for dynamic patterns         */
  /* -------------------------------------------- */

  /* Multiple used patterns */
{NAME_PATTERN}        { log_info("{NAME_PATTERN}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        return val_name;
                      }
{PIN_PATTERN}         { log_info("{PIN_PATTERN}",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        return val_pin;
                      }
  
  /* Special chars */
{SINGLE_CHARS}        { log_info("{SINGLE_CHARS}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext); 
                        return yytext[0];
                      }

  /* Whitespace */
{WHITESPACE}          { /* ignore all whitespace */ }   
{INVALID_TOKENS}      { const char* unknown_word = yytext; 
                        log_error_no_exit("Lexer", 0, " Unknown token '%s' found at line %d", unknown_word, yylineno);
                        print_closest_keywords(unknown_word);
                        close_logging();
                        exit(1);
                      }
.                     { log_error(".", yylineno, "This should not happen, because unknown characters are handled by 'INVALID_TOKENS'. Found '%s' at line %d", yytext, yylineno); }

%%
