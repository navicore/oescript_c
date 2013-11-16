/* The contents of this file are subject to the Apache License
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License 
 * from the file named COPYING and from http://www.apache.org/licenses/.
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * The Original Code is OeScript.
 *
 * The Initial Developer of the Original Code is OnExtent, LLC.
 * Portions created by OnExtent, LLC are Copyright (C) 2008-2011
 * OnExtent, LLC. All Rights Reserved.
 *
 */

%{
#include "oejcl_parser.tab.h"
#include "oec_values.h"
#include "oejcl_parser.h"
#include "assert.h"
#include <stdlib.h>
#include <string.h>

#define PARM    yyget_extra(yyscanner)

#define YY_INPUT(buffer, res, max_size)             \
do {                                                \
  if (PARM->pos >= PARM->length)                    \
  res = YY_NULL;                                    \
  else                                              \
    {                                               \
      res = PARM->length - PARM->pos;               \
      res > (int)max_size ? res = max_size : 0;     \
      memcpy(buffer, PARM->buf + PARM->pos, res);   \
      PARM->pos += res;                             \
    }                                               \
  } while (0)

%}

%option reentrant bison-bridge
%option noyywrap
%option nounput

%%


"("         { return START_PARAN;   }
")"         { return END_PARAN;     }
"["         { return START_BRACKET; }
"]"         { return END_BRACKET;   }
"{"         { return START_BRACE;   }
"}"         { return END_BRACE;     }
":"         { return PAIR_SEP;      }
";"         { return END_STMT;      }
"+"         { return CONCAT;        }
"="         { return ASSIGN;        }
"."         { return DOT;           }

"cons"	    { return CONS;          }
"car"	    { return CAR;           }
"cdr"	    { return CDR;           }

"dis"       { return DIS;           }
"connect"	{ return CONNECT;       }

"write"	    { return WRITE;         }
"read"	    { return READ;          }
"take"	    { return TAKE;          }
"commit"    { return COMMIT;        }
"cancel"    { return CANCEL;        }

"response"  { return RESPONSE;      }
"error"     { return ERROR;         }
"extentname" { return EXTENTNAME;   }
"msg"       { return MSG;           }
"cmd"       { return CMD;           }

"txn"       { return TXN;           }

"username"  { return USERNAME;      }
"password"  { return PASSWORD;      }
"dur"	    { return DURATION;      }
"timeout"	{ return TIMEOUT;       }
"cid"	    { return CORRELATION_ID;}
"tid"	    { return TXN_ID;        }
"lid"	    { return LEASE_ID;      }
"sid"	    { return SESSION_ID;    }
"nresults"	{ return NRESULTS;      }
"enlisted"	{ return NENLISTED;     }

-?[0-9]+    { yylval->intval = atoi(yytext); return NUMBER; }
["](\\.|[^\\"])*["] { yylval->strval = oec_unquote(yytext, strlen(yytext)); return STRING; }
[a-zA-Z_$][a-zA-Z0-9_$]* { yylval->strval = strdup(yytext); return NAME; }
"//".*  
[ \n\t]   { /* ignore white space */ }
.	{ yyerror(PARM, yyscanner, "Mystery character\n"); }

%%
//.	{ yyerror("Mystery character %c\n", *yytext); }

