%{
#include "y.tab.h"
%}

%%
[ \t]                           { ; }
\n                              { return 0; }
"&&"                            { return AND; }
"||"                            { return OR; }
"!"                             { return NOT; }
"<="                            { return LE; }
">="                            { return GE; }
"=="                            { return EQ; }
"!="                            { return NE; }
"<"                             { return LT; }
">"                             { return GT; }
"("                             { return '('; }
")"                             { return ')'; }
[0-9]+                          { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*          { return ID; }
.                               { return yytext[0]; }
%%

int yywrap() { return 1; }
