%{
#include "y.tab.h"
%}

%%
[0-9]+                          { return NUM; }
[0-9]+\.[0-9]+                  { return NUM; }
[0-9]+(\.[0-9]+)?[eE][+-]?[0-9]+ { return NUM; }
[ \t]                           { ; }
\n                              { return 0; }
"+"                             { return '+'; }
"-"                             { return '-'; }
"*"                             { return '*'; }
"/"                             { return '/'; }
"("                             { return '('; }
")"                             { return ')'; }
.                               { return yytext[0]; }
%%

int yywrap() { return 1; }
