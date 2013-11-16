%{
#include <stdio.h>
#include <string.h>
#include "runme.h"

int oejcl_parse(char *buf, char **result)
{
    oejcl_parse_parm  pp;

    pp.buf = buf;
    pp.length = strlen(buf);
    pp.pos = 0;
    *result = 0;
    yylex_init(&pp.yyscanner);
    yyset_extra(&pp, pp.yyscanner);
    yyparse(&pp, pp.yyscanner);
    *result = (char *) pp.result;
    yylex_destroy(pp.yyscanner);
    return 0;
}

%}

%pure_parser
%parse-param {oejcl_parse_parm *parm}
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}

%union {
  int intval;
  char *strval;
}

%token <strval> NAME
%token <intval> NUMBER

%type <intval> number
%type <strval> name

%%

stmt: number | name {printf("got something\n");};
 | stmt stmt ';' {parm->result = YES; };

name: NAME {printf("got a name: %s\n", $$);};

number: NUMBER {printf("got a number: %i\n", $$);};

%%

void yyerror() {
  printf("wtf?\n");
}

int main(int argc, char *argv[])
{
  char *res;
  printf("processing: %s\n", argv[1]);
  int rc = oejcl_parse(argv[1], &res);
  printf("result = %s\n", res);

  return 0;    
}

