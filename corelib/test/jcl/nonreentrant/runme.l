%{
#include "runme.tab.h"
#include "runme.h"
#include <stdlib.h>
#include <string.h>

%}

%option noyywrap
%option nounput

%%


-?[0-9]+    { yylval.intval = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.strval = strdup(yytext); return NAME; }
"//".*  
[ \n\t]   { /* ignore white space */ }
.	{ yyerror("Mystery character\n"); }

%%
//.	{ yyerror("Mystery character %c\n", *yytext); }

