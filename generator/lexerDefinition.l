%{
 #include <stdio.h>

 #include "lexer_helper.h"
%}

%option noyywrap nounput noinput yylineno

  /* -------------------------------------------- */
  /*               Dynamic patterns               */
  /* -------------------------------------------- */

  /*  Multiple used */
IDENTIFIER_PATTERN    \"[A-Za-z_][A-Za-z0-9_]*\"
PIN_PATTERN           P[A-Z]((0-9)|1[0-5])
ENABLE_PATTERN        (true|false)
HIGH_LOW_PATTERN      (high|low)

  /* supported microcontrollers */
CONTROLLER_PATTERN    (STM32F446RE)

  /* GPIO specific */
TYPE_PATTERN          (pushpull|opendrain)
  /* TODO: none is not a valid option for pull and init, fix it */
PULL_PATTERN          (up|down|none)
SPEED_PATTERN         ({HIGH_LOW_PATTERN}|medium|very_high)
INIT_PATTERN          (on|off|none)
ACTIVE_PATTERN        ({HIGH_LOW_PATTERN})

  /* Special chars */
SINGLE_CHARS    [\{\}:]
WHITESPACE      [\ \n\t]+
INVALID_TOKENS  [^ \n\t\{\}:]+

  /* -------------------------------------------- */
  /*                Fixed patterns                */
  /* -------------------------------------------- */

AUTO_BSP        AutoBSP
CONTROLLER      controller
OUTPUT          OUTPUT
INPUT           INPUT

NAME            name
PIN             pin
TYPE            type
PULL            pull
SPEED           speed
INIT            init
ACTIVE          active
ENABLE          enable



%%
  /* Rules */

  /* -------------------------------------------- */
  /*           Rules for fixed patterns           */
  /* -------------------------------------------- */

  /* TODO: Replace pattern-names with patters e.g. instead of '{AUTO_BSP}' use '"AutoBSP"' directly */
{AUTO_BSP}          { printf("AutoBSP");    }
{CONTROLLER}        { printf("controller"); }
{OUTPUT}            { printf("OUTPUT");     }
{INPUT}             { printf("INPUT");      }
{NAME}              { printf("name");       }
{PIN}               { printf("pin");        }
{TYPE}              { printf("type");       }
{PULL}              { printf("pull");       }
{SPEED}             { printf("speed");      }
{INIT}              { printf("init");       }
{ACTIVE}            { printf("active");     }
{ENABLE}            { printf("enable");     }

  /* -------------------------------------------- */
  /*           Rules for dynamic patterns         */
  /* -------------------------------------------- */

  /* Multiple used */
{IDENTIFIER_PATTERN}  { printf("%s", yytext); }
{PIN_PATTERN}         { printf("%s", yytext); }
{ENABLE_PATTERN}      { printf("%s", yytext); }

  /* supported microcontrollers */
{CONTROLLER_PATTERN}  { printf("%s", yytext); }

  /* GPIO specific */
{TYPE_PATTERN}        { printf("%s", yytext); }
{PULL_PATTERN}        { printf("%s", yytext); }
{SPEED_PATTERN}       { printf("%s", yytext); }
{INIT_PATTERN}        { printf("%s", yytext); }
{ACTIVE_PATTERN}      { printf("%s", yytext); }

  /* Special chars */
{SINGLE_CHARS}        { printf("%s", yytext); }
{WHITESPACE}          { printf("%s", yytext); }  /* TODO: ignoring all whitespace */ 
{INVALID_TOKENS}      { const char* unknown_word = yytext; 
                        const char* suggested_word = find_closest_keyword(unknown_word);
                        if(suggested_word)
                          fprintf(stderr, "ERROR: Unknown token '%s' at line %d\n       Did you mean  '%s'?\n", unknown_word, yylineno, suggested_word);
                        else
                          fprintf(stderr, "ERROR: Unknown token '%s' at line %d\n", unknown_word, yylineno);
                        exit(1);
                      }
.                     { fprintf(stderr, "ERROR: This should not happen, because unknown characters are handled b< 'INVALID_TOKENS'. Found '%s' at line %d\n", yytext, yylineno); 
                        exit(1);
                      }

%%

int main() {
  yylex();
  return 0;
}
