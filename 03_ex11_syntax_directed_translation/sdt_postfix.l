%{
#if defined(__has_include)
  #if __has_include("sdt_postfix.tab.h")
    #include "sdt_postfix.tab.h"
  #else
    #include "y.tab.h"
  #endif
#else
  #include "y.tab.h"
#endif
#include <string.h>
%}

%%
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }
[0-9]+                  { yylval.str = strdup(yytext); return NUM; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"*"                     { return '*'; }
"/"                     { return '/'; }
"("                     { return '('; }
")"                     { return ')'; }
[ \t]                   { ; }
\n                      { return 0; }
.                       { return yytext[0]; }
%%

int yywrap() { return 1; }
