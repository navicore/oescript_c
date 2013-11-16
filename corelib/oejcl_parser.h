/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef	__OEJCL_H__
#define __OEJCL_H__

#include "arena.h"

typedef struct ast_s {
  int nodetype;
  struct ast_s *l;
  struct ast_s *r;
} oejcl_ast;

typedef struct numval_s {
  int nodetype;
  double number;
} numval;

typedef struct strval_s {
  int nodetype;
  char *str;
} strval;

extern oejcl_ast *new_oejcl_ast(Arena_T, int, oejcl_ast*, oejcl_ast*);
extern oejcl_ast *new_oejcl_num(Arena_T, double);
extern oejcl_ast *new_oejcl_num_ast(Arena_T, int, double);
extern oejcl_ast *new_oejcl_str(Arena_T, char *);
extern oejcl_ast *new_oejcl_str_ast(Arena_T, int, char *);
extern oejcl_ast *new_oejcl_map_ast(Arena_T, char *, char *, oejcl_ast*);
extern oejcl_ast *new_oejcl_list_ast(Arena_T, char *, oejcl_ast*);

typedef struct parse_parm_s
{
  void *yyscanner; /* state of the lexer */

  int iserr;
  char *emsg;

  char *buf;  /* buffer we read from */
  int pos;    /* current position in buf */
  int length; /* length of buf */
  Arena_T arena;
  oejcl_ast *result;

} oejcl_parse_parm;

int oejcl_parse(Arena_T arena, char *buf, oejcl_ast **result);

union YYSTYPE_u {
  int intval;
  char *strval;
  oejcl_ast *ast;
};

#define YYSTYPE union YYSTYPE_u
#define YY_EXTRA_TYPE oejcl_parse_parm *
#define YYERROR_VERBOSE

/*
** forward declaration of lexer/parser functions 
** so the compiler shuts up about warnings
*/
int yylex(YYSTYPE *, void *);
int yylex_init(void **);
int yylex_destroy(void *);
void yyset_extra(YY_EXTRA_TYPE, void *);
int yyparse(oejcl_parse_parm *, void *);
//void yyerror();
//void yyerror(char *, ...);
void yyerror(oejcl_parse_parm *, void *, char *);

#endif /* !__OESCR_H__ */

#ifdef __cplusplus
}
#endif 

