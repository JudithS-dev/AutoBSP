%{
 #include <stdio.h>
%}

%option noyywrap nounput noinput

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
PULL_PATTERN          (up|down|none)
SPEED_PATTERN         ({HIGH_LOW_PATTERN}|medium|very_high)
INIT_PATTERN          (on|off|none)
ACTIVE_PATTERN        ({HIGH_LOW_PATTERN})

  /* Single chars */
SINGLE_CHARS (\{|\}|:)

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

  /* Single chars */
{SINGLE_CHARS}     { printf("%s", yytext); }


(\ |\n|\t)          { printf("%s", yytext); }  /* TODO: ignoring all whitespace */ 
.                   { printf("\nERROR: unknown '%c'\n", yytext[0]); }

%%

int main() {
  yylex();
  return 0;
}
