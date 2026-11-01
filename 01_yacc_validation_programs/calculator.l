%{
/*
 * ============================================================================
 * LEX SPECIFICATION FOR DESKTOP CALCULATOR (calculator.l)
 * ============================================================================
 * Tokenizes operands, operators, parentheses and newlines for calculator.y.
 * Passes numerical values via yylval.
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "calculator.tab.h"
%}

%%
[ \t\r]+                            { /* ignore whitespace */ }
[0-9]+(\.[0-9]+)?                   {
                                        yylval.val = atof(yytext);
                                        return NUMBER;
                                    }
"+"                                 { return '+'; }
"-"                                 { return '-'; }
"*"                                 { return '*'; }
"/"                                 { return '/'; }
"%"                                 { return '%'; }
"^"                                 { return '^'; }
"("                                 { return '('; }
")"                                 { return ')'; }
\n                                  { return '\n'; }
.                                   { return yytext[0]; }
%%

int yywrap() {
    return 1;
}
