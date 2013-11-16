%{
#include "runme.tab.h"
#include "runme.h"
#include <stdlib.h>
#include <string.h>

#define PARM    yyget_extra(yyscanner)

#define YY_INPUT(buffer, res, max_size)             \
do {                                                \
    if (PARM->pos >= PARM->length)                  \
        res = YY_NULL;                              \
    else                                            \
    {                                               \
        res = PARM->length - PARM->pos;             \
        res > (int)max_size ? res = max_size : 0;   \
        memcpy(buffer, PARM->buf + PARM->pos, res); \
        PARM->pos += res;                           \
    }                                               \
} while (0)

%}

%option reentrant bison-bridge
%option noyywrap
%option nounput


%%


-?[0-9]+    { yylval->intval = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval->strval = strdup(yytext); return NAME; }
"//".*  
[ \n\t]   { /* ignore white space */ }
.	{ yyerror(); }

%%

