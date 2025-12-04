%{
 #include <stdio.h>

 #include "lexer_helper.h"
%}

  /* activate line number tracking, stack usage (for context switches), and disable unused functions */
%option yylineno stack noyywrap noinput nounput noyy_top_state

%x COMMENT_LINE 
%x COMMENT_BLOCK

  /* -------------------------------------------- */
  /*               Dynamic patterns               */
  /* -------------------------------------------- */

  /*  Multiple used patterns*/
IDENTIFIER_PATTERN    \"[A-Za-z_][A-Za-z0-9_]*\"
PIN_PATTERN           P[A-Z]((0-9)|1[0-5])

  /* Comment patterns */
COM_LINE            \/\/
COM_BLOCK_START     \/\*
COM_BLOCK_END       \*\/

  /* Special chars */
SINGLE_CHARS    [\{\}:]
WHITESPACE      [\ \n\t]+
COMMENT_CHARS   [\/\*]
INVALID_TOKENS  ([^ \n\t\{\}:\/\*]+|{COMMENT_CHARS})

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
"AutoBSP"           { printf("AutoBSP");      }
"OUTPUT"            { printf("OUTPUT");       }
"INPUT"             { printf("INPUT");        }

  /* -------------- Parameter names -------------- */
  /* Multiple used parameter names */
"controller"        { printf("controller");   }
"name"              { printf("name");         }
"pin"               { printf("pin");          }

  /* GPIO specific parameter names */
"type"              { printf("type");         }
"pull"              { printf("pull");         }
"speed"             { printf("speed");        }
"init"              { printf("init");         }
"active"            { printf("active");       }
"enable"            { printf("enable");       }

  /* -------------- Parameter values ------------- */
  /* Multiple used parameter values */
  /* Used for 'enable' and '?lock?' */
"true"              { printf("true");         }
"false"             { printf("false");        }
  /* Used for 'active' and 'speed' */
"low"               { printf("low");          }
"high"              { printf("high");         }
  /* Used for 'pull' 'init' and '?trigger?' */
"none"              { printf("none");         }

  /* Supported microcontrollers */
"STM32F446RE"       { printf("STM32F446RE");  }

  /* GPIO specific parameter values */
"pushpull"          { printf("pushpull");     }
"opendrain"         { printf("opendrain");    }
"up"                { printf("up");           }
"down"              { printf("down");         }
"medium"            { printf("medium");       }
"very_high"         { printf("very_high");    }
"on"                { printf("on");           }
"off"               { printf("off");          }


  /* -------------------------------------------- */
  /*           Rules for dynamic patterns         */
  /* -------------------------------------------- */

  /* Multiple used patterns */
{IDENTIFIER_PATTERN}  { printf("%s", yytext); }
{PIN_PATTERN}         { printf("%s", yytext); }

  /* Special chars */
{SINGLE_CHARS}        { printf("%s", yytext); }
{WHITESPACE}          { printf("%s", yytext); }  /* TODO: ignoring all whitespace */ 
{INVALID_TOKENS}      { const char* unknown_word = yytext; 
                        fprintf(stderr, "ERROR: Unknown token '%s' at line %d\n", unknown_word, yylineno);
                        print_closest_keywords(unknown_word);
                        exit(1);
                      }
.                     { fprintf(stderr, "ERROR: This should not happen, because unknown characters are handled by 'INVALID_TOKENS'. Found '%s' at line %d\n", yytext, yylineno); 
                        exit(1);
                      }

%%

int main() {
  yylex();
  return 0;
}
