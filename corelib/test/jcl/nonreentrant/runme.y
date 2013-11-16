%{
#include <stdio.h>
#include "runme.h"
%}

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
 | stmt stmt

name: NAME {printf("got a name: %s\n", $$);};

number: NUMBER {printf("got a number: %i\n", $$);};

%%

int main()
{
  printf("> "); 
  yyparse();
}

int yyerror(char *s)
{
  fprintf(stderr, "error: %s\n", s);
  return 0;
}

