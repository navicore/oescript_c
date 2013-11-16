#ifndef	__RUNME_H__
#define __RUNME_H__

static char *YES = "yes";
static char *NO = "no";

typedef struct      parse_parm_s
{
    void            *yyscanner;
    char            *buf;
    int             pos;
    int             length;
    char *          result;
}                   oejcl_parse_parm;

int     oejcl_parse(char *buf, char **result);

union tagYYSTYPE {
  int intval;
  char *strval;
};

#define YYSTYPE union tagYYSTYPE

#define YY_EXTRA_TYPE   oejcl_parse_parm *

int     yylex(YYSTYPE *, void *);
int     yylex_init(void **);
int     yylex_destroy(void *);
void    yyset_extra(YY_EXTRA_TYPE, void *);
int     yyparse(oejcl_parse_parm *, void *);
void    yyerror();


#endif /* !__RUNME_H__ */

