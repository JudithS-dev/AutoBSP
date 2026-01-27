%{
 #include <stdio.h>
 
 #include "logging.h"
 #include "moduleEnums.h"
 #include "parserInC.h"
 
 #include "lexerHelper.h"
 #include "lexerErrorHelper.h"
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
PIN_PATTERN_PXn     P[A-Z]([0-9]|1[0-5])
PIN_PATTERN_GPIOn   GPIO([0-9]|[1-9][0-9])
U_NUMBER_PATTERN    [0-9]|[1-9][0-9]+

  /* Comment patterns */
COM_LINE            \/\/
COM_BLOCK_START     \/\*
COM_BLOCK_END       \*\/

  /* Special chars */
SINGLE_CHARS        [\{\}:;]
WHITESPACE          [\ \n\t]+
COMMENT_CHARS       [\/\*]
INVALID_TOKENS      ([^ \n\t\{\}:\/\*;]+|{COMMENT_CHARS})

%%
  /* Rules */
  /* -------------------------------------------- */
  /*          Rules for context switching         */
  /* -------------------------------------------- */

  /* Line comments */
{COM_LINE}                  { log_info("{COM_LINE}", LOG_LEXER_CONDITION, yylineno, "Entering line comment");
                              BEGIN(COMMENT_LINE);
                            }
<COMMENT_LINE>\n            { log_info("\\n", LOG_LEXER_CONDITION, yylineno, "Exiting line comment");
                              BEGIN(INITIAL);
                            }
<COMMENT_LINE>.             { /* ignore everything  */ }

  /* Block comments */
<INITIAL,COMMENT_BLOCK>{COM_BLOCK_START}    { log_info("{COM_BLOCK_START}", LOG_LEXER_CONDITION, yylineno, "Entering block comment");
                                              yy_push_state(YYSTATE);
                                              BEGIN(COMMENT_BLOCK);   }
<COMMENT_BLOCK>{COM_BLOCK_END}              { log_info("{COM_BLOCK_END}", LOG_LEXER_CONDITION, yylineno, "Exiting block comment");
                                              yy_pop_state();
                                            }
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
"PWM_OUTPUT"        { log_info("\"PWM_OUTPUT\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_pwm_output;
                    }
"UART"              { log_info("\"UART\"",    LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_uart;
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
                      return kw_enable;
                    }

  /* PWM specific parameter names */
"frequency"         { log_info("\"frequency\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_pwm_frequency;
                    }
"duty"              { log_info("\"duty\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_pwm_duty;
                    }

  /* UART specific parameter names */
"tx_pin"            { log_info("\"tx_pin\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_tx_pin;
                    }
"rx_pin"            { log_info("\"rx_pin\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_rx_pin;
                    }
"baudrate"          { log_info("\"baudrate\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_baudrate;
                    }
"databits"          { log_info("\"databits\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_databits;
                    }
"stopbits"          { log_info("\"stopbits\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_stopbits;
                    }
"parity"            { log_info("\"parity\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return kw_parity;
                    }

  /* -------------- Parameter values ------------- */
  /* Multiple used parameter values */
  /* Used for 'enable' and '?lock?' */
"true"              { log_info("\"true\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_bool = true;
                      return val_bool;
                    }
"false"             { log_info("\"false\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_bool = false;
                      return val_bool;
                    }
  /* Used for 'active' and 'speed' */
"low"               { log_info("\"low\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_level = LOW;
                      return val_level;
                    }
"high"              { log_info("\"high\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_level = HIGH;
                      return val_level;
                    }
  /* Used for 'pull' 'init' and '?trigger?' */
"none"              { log_info("\"none\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_none;
                    }

  /* Supported microcontrollers */
"STM32F446RE"       { log_info("\"STM32F446RE\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_controller = STM32F446RE;
                      return val_controller;
                    }

  /* GPIO specific parameter values */
"pushpull"          { log_info("\"pushpull\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_gpio_type = GPIO_TYPE_PUSHPULL;
                      return val_gpio_type;
                    }
"opendrain"         { log_info("\"opendrain\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_gpio_type = GPIO_TYPE_OPENDRAIN;
                      return val_gpio_type;
                    }
"up"                { log_info("\"up\"",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_pull = GPIO_HELPER_PULL_UP;
                      return val_gpio_pull;
                    }
"down"              { log_info("\"down\"",      LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_pull = GPIO_HELPER_PULL_DOWN;
                      return val_gpio_pull;
                    }
"medium"            { log_info("\"medium\"",    LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_speed = GPIO_HELPER_SPEED_MEDIUM;
                      return val_gpio_speed;
                    }
"very_high"         { log_info("\"very_high\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_speed = GPIO_HELPER_SPEED_VERY_HIGH;
                      return val_gpio_speed;
                    }
"on"                { log_info("\"on\"",        LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_init = GPIO_HELPER_INIT_ON;
                      return val_gpio_init;
                    }
"off"               { log_info("\"off\"",       LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_gpio_init = GPIO_HELPER_INIT_OFF;
                      return val_gpio_init;
                    }

  /* UART specific parameter values */
"1.5"               { log_info("\"1.5\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      return val_uart_stopbit_1_5;
                    }
"even"              { log_info("\"even\"", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_uart_parity = UART_HELPER_PARITY_EVEN;
                      return val_uart_parity;
                    }
"odd"               { log_info("\"odd\"",  LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                      yylval.u_helper_uart_parity = UART_HELPER_PARITY_ODD;
                      return val_uart_parity;
                    }

  /* -------------------------------------------- */
  /*           Rules for dynamic patterns         */
  /* -------------------------------------------- */

  /* Multiple used patterns */
{NAME_PATTERN}        { log_info("{NAME_PATTERN}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        // Duplicate the string and remove quotes
                        yylval.u_str = extract_name(yytext);
                        return val_name;
                      }
{PIN_PATTERN_PXn}     { log_info("{PIN_PATTERN_PXn}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        yylval.u_pin.identifier = PXn;
                        yylval.u_pin.port = get_port(yytext);
                        if(yylval.u_pin.port == '\0')
                          log_info("{PIN_PATTERN_PXn}", LOG_LEXER_CONVERSION, yylineno, "Did not find port in pin text '%s'", yytext);
                        else
                          log_info("{PIN_PATTERN_PXn}", LOG_LEXER_CONVERSION, yylineno, "Found port '%c' in pin text '%s'", yylval.u_pin.port, yytext);
                        
                        yylval.u_pin.pin_number = get_pin_number_PXn(yytext);
                        log_info("{PIN_PATTERN_PXn}", LOG_LEXER_CONVERSION, yylineno, "Found pin number '%u' in pin text '%s'", yylval.u_pin.pin_number, yytext);
                        return val_pin;
                      }
{PIN_PATTERN_GPIOn}   { log_info("{PIN_PATTERN_GPIOn}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        yylval.u_pin.identifier = GPIOn;
                        yylval.u_pin.port = '\0'; /* ESP pins do not have a port letter */
                        yylval.u_pin.pin_number = get_pin_number_GPIOn(yytext);
                        log_info("{PIN_PATTERN_GPIOn}", LOG_LEXER_CONVERSION, yylineno, "Found pin number '%u' in pin text '%s'", yylval.u_pin.pin_number, yytext);
                        return val_pin;
                      }

{U_NUMBER_PATTERN}    { log_info("{U_NUMBER_PATTERN}", LOG_LEXER_TOKEN, yylineno, "Found '%s'", yytext);
                        yylval.u_nr = (unsigned int)atoi(yytext);
                        log_info("{U_NUMBER_PATTERN}", LOG_LEXER_CONVERSION, yylineno, "Converted '%s' to number '%u'", yytext, yylval.u_nr);
                        return val_nr;
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
