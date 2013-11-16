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

/** todo:
    - replace CONCAT with full regex manipulation support
 */

%{
#include <stdio.h>
#include "oejcl_parser.h"
#include "assert.h"
#include "oec_log.h"
#include "oec_values.h"
#include "string.h"

int oejcl_parse(Arena_T arena, char *buf, oejcl_ast **result) {

  int rc = 0;
  oejcl_parse_parm  pp;
  pp.emsg = NULL;
  pp.iserr = 0;

  pp.arena = arena;
  pp.buf = buf;
  pp.length = strlen(buf);
  pp.pos = 0;
  pp.result = 0;
  *result = NULL;
  yylex_init(&pp.yyscanner);
  yyset_extra(&pp, pp.yyscanner);
  yyparse(&pp, pp.yyscanner);
  *result = pp.result;
  if (pp.iserr) {
    OE_DLOG(0, "oejcl_parse error: %s\n", pp.emsg);
    rc = -1;
  }
  yylex_destroy(pp.yyscanner);

  return rc;
}

%}

%pure_parser
%parse-param {oejcl_parse_parm *parm}
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}

%token <strval> STRING
%token <intval> NUMBER
%token <strval> NAME
%token CONNECT DIS DOT VER EXTENTNAME USERNAME PASSWORD SESSION_ID UNSET
%token CMD ERROR MSG RESPONSE
%token TIMEOUT DURATION CORRELATION_ID TXN_ID LEASE_ID NRESULTS
%token START_BRACE END_BRACE END_STMT
%token CONS CAR CDR
%token WRITE READ TAKE TXN COMMIT CANCEL CANCELLEASE NENLISTED
%token START_PARAN END_PARAN START_BRACKET END_BRACKET PAIR_SEP MAP LIST STMT

%right ASSIGN
%left CONCAT

%type <ast> responseargs errorargs readargs writeargs cmdargs connectargs stmts stmt command cmd cmdname extentname read take write newtxn commit cancel assign data txnargs map list string txn items cid tid lid dur timeout connect dis ver vernum response errormsg msg username password sid nresults nenlisted

%start stmts

%%

stmts: stmt {
    $$ = $1; 
    parm->result = $$; 
    OE_TLOG(0, "oejcl parsed stmt batch\n");
}
| stmt stmts {
    $$ = $1; 
    $1->r = $2;
    parm->result = $$;
    OE_TLOG(0, "oejcl parsed multi stmt batch\n");
}
;

stmt: command END_STMT {
    $$ = new_oejcl_ast(parm->arena, STMT, $1, NULL); 
    OE_TLOG(0, "oejcl parsed command stmt\n");
}
| response END_STMT {
    $$ = new_oejcl_ast(parm->arena, STMT, $1, NULL); 
    OE_TLOG(0, "oejcl parsed response stmt\n");
}
| errormsg END_STMT {
    $$ = new_oejcl_ast(parm->arena, STMT, $1, NULL); 
    OE_TLOG(0, "oejcl parsed error stmt\n");
}
| txn END_STMT {
    $$ = new_oejcl_ast(parm->arena, STMT, $1, NULL); 
    OE_TLOG(0, "oejcl parsed txn block stmt\n");
}
| assign END_STMT {
    $$ = new_oejcl_ast(parm->arena, STMT, $1, NULL); 
    OE_TLOG(0, "oejcl parsed assign stmt\n");
}
;

command:
 connect  {OE_TLOG(0, "oejcl parsed conn command\n");     }
 | dis      {OE_TLOG(0, "oejcl parsed dis command\n");      }
 | read     {OE_TLOG(0, "oejcl parsed read command\n");     }
 | take     {OE_TLOG(0, "oejcl parsed take command\n");     }
 | write    {OE_TLOG(0, "oejcl parsed write command\n");    }
 | newtxn   {OE_TLOG(0, "oejcl parsed newtxn command\n");   }
 | commit   {OE_TLOG(0, "oejcl parsed commit command\n");   }
 | cancel   {OE_TLOG(0, "oejcl parsed cancel command\n");   }
;

response: RESPONSE START_PARAN responseargs END_PARAN {
	$$ = new_oejcl_ast(parm->arena, RESPONSE, $3, NULL);
	OE_TLOG(0, "oejcl parsed response w/args\n");
}
 |  RESPONSE START_PARAN responseargs data END_PARAN {
	$$ = new_oejcl_ast(parm->arena, RESPONSE, $3, $4);
	OE_TLOG(0, "oejcl parsed response w/args and data\n");
}
;

errormsg: ERROR START_PARAN errorargs END_PARAN {
	$$ = new_oejcl_ast(parm->arena, ERROR, $3, NULL);
	OE_TLOG(0, "oejcl parsed error w/args\n");
}
;

connect: CONNECT START_PARAN connectargs END_PARAN {
	$$ = new_oejcl_ast(parm->arena, CONNECT, $3, NULL);
	OE_TLOG(0, "oejcl parsed read\n");
}
;

dis: DIS START_PARAN END_PARAN{
	$$ = new_oejcl_ast(parm->arena, DIS, NULL, NULL);
    OE_TLOG(0, "oejcl parsed dis\n");
}
;

read: READ START_PARAN readargs data END_PARAN {
	$$ = new_oejcl_ast(parm->arena, READ, $3, $4);
	OE_TLOG(0, "oejcl parsed read\n");
}
;

take: TAKE START_PARAN readargs data END_PARAN {
	$$ = new_oejcl_ast(parm->arena, TAKE, $3, $4);
	OE_TLOG(0, "oejcl parsed take\n");
}
;

write: WRITE START_PARAN writeargs data END_PARAN {
	$$ = new_oejcl_ast(parm->arena, WRITE, $3, $4); 
	OE_TLOG(0, "oejcl parsed write\n");
}
;

newtxn: TXN START_PARAN txnargs END_PARAN {
	$$ = new_oejcl_ast(parm->arena, TXN, $3, NULL);
	OE_TLOG(0, "oejcl parsed newtxn\n");
}
;

commit: COMMIT START_PARAN cmdargs tid nenlisted END_PARAN {
    $5->r = $4;
    $4->r = $3;
	$$ = new_oejcl_ast(parm->arena, COMMIT, $5, NULL);
	OE_TLOG(0, "oejcl parsed commit\n");
}
;

//BUG!  reduce/reduce errors caused me to flaten this and not let cance
// work with leases that have a cid.  fixme.  need to readahead :(
cancel: CANCEL START_PARAN cmdargs tid END_PARAN {
    $4->r = $3;
	$$ = new_oejcl_ast(parm->arena, CANCEL, $4, NULL);
	OE_TLOG(0, "oejcl parsed cancel\n");
}
| CANCEL START_PARAN cmdargs lid END_PARAN {
    $4->r = $3;
	$$ = new_oejcl_ast(parm->arena, CANCELLEASE, $4, NULL);
	OE_TLOG(0, "oejcl parsed cancel lease\n");
}
;

assign: NAME ASSIGN string {
	$$ = new_oejcl_ast(parm->arena, ASSIGN,  new_oejcl_str_ast(parm->arena, NAME, $1), $3);
	OE_TLOG(0, "oejcl parsed str assign. assigning to %s\n", $1);
}
| NAME ASSIGN NUMBER {
    $$ = new_oejcl_ast(parm->arena, ASSIGN, new_oejcl_str_ast(parm->arena, NAME, $1), 
        new_oejcl_num_ast(parm->arena, NUMBER, $3)); 
    OE_TLOG(0, "oejcl parsed num assign. assigning %s\n", $1);
}
| NAME ASSIGN data {
    $$ = new_oejcl_ast(parm->arena, ASSIGN, new_oejcl_str_ast(parm->arena, NAME, $1), $3); 
    OE_TLOG(0, "oejcl parsed data assign\n");
}
| NAME ASSIGN command {
    $$ = new_oejcl_ast(parm->arena, ASSIGN, new_oejcl_str_ast(parm->arena, NAME, $1), $3); 
    OE_TLOG(0, "oejcl parsed stmt assign\n");
}
| NAME ASSIGN txn {
    $$ = new_oejcl_ast(parm->arena, ASSIGN, new_oejcl_str_ast(parm->arena, NAME, $1), $3); 
    OE_TLOG(0, "oejcl parsed txn block assign\n");
}

txn: TXN START_PARAN txnargs END_PARAN START_BRACE stmts END_BRACE {
    $$ = new_oejcl_ast(parm->arena, TXN, $3, $6); 
    OE_TLOG(0, "oejcl parsed txn block\n");
}
;

data:
 START_BRACKET END_BRACKET {
     $$ = new_oejcl_ast(parm->arena, (int) NULL, NULL, NULL); 
}
| START_BRACKET items END_BRACKET {
     $$ = $2; OE_TLOG(0, "oejcl parsed data\n");
}
;

items:
  list { 
     $$ = new_oejcl_ast(parm->arena, LIST, $1, NULL); 
     OE_TLOG(0, "oejcl parsed list\n");
}
| map { 
    $$ = new_oejcl_ast(parm->arena, MAP, $1, NULL); 
    OE_TLOG(0, "oejcl parsed map\n");
}
;

list: string {
    $$ = $1; 
    OE_TLOG(0, "oejcl parsed one list str\n");
}
| string list {
    $$ = $1; 
    $1->r = $2; 
    OE_TLOG(0, "oejcl parsed multi list str\n");
}
;

map: string PAIR_SEP string {
    $$ = $1; 
    $1->r = $3; 
    OE_TLOG(0, "oejcl parsed one map item\n");
}
| string PAIR_SEP string map {
    $$ = $1; 
    $1->r = $3; 
    $3->r = $4; 
    OE_TLOG(0, "oejcl parsed multi map items\n");
}
;

string: STRING {
    $$ = new_oejcl_str_ast(parm->arena, STRING, $1); 
    OE_TLOG(0, "oejcl parsed a string: %s\n", $1);
}
| STRING CONCAT STRING {
    $$ = new_oejcl_str_ast(parm->arena, STRING, oec_str_concat(parm->arena, 2, $1, $3)); 
    OE_TLOG(0, "oejcl parsed a concat string\n");
}
| NAME {
    $$ = new_oejcl_str_ast(parm->arena, NAME, $1); 
    OE_TLOG(0, "oejcl parsed unresolved name: %s\n", $1);
}
;

/**
| NAME START_BRACKET STRING END_BRACKET {
    OE_TLOG(0, "oejcl parsed a map inner ref\n");
}//todo: resolve var ref
| NAME START_BRACKET NUMBER END_BRACKET {
    OE_TLOG(0, "oejcl parsed a list inner ref\n");
}//todo: resolve var ref
| NAME START_BRACKET NAME END_BRACKET {
    OE_TLOG(0, "oejcl parsed a variable inner ref\n");
}//todo:resolve var ref
*/

errorargs:  {$$ = NULL;}
 | errorargs cmd {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed err cmd\n");}
 | errorargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed err cid\n");}
 | errorargs msg {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed err msg\n");}
 ;

responseargs:  {$$ = NULL;}
 | responseargs cmd {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res cmd\n");}
 | responseargs lid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res lid\n");}
 | responseargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res cid\n");}
 | responseargs tid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res tid\n");}
 | responseargs sid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res sid\n");}
 | responseargs nresults {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed res nresults\n");}
 ;

txnargs:  {$$ = NULL;}
 | txnargs timeout {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed txn timeout\n");}
 | txnargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed txn cid\n");}
 | txnargs tid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed txn tid\n");}
 ;

cmdargs:  {$$ = NULL;}
 | cmdargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed cmdargs cid\n");}
 ;

//txncommitargs:  {$$ = NULL;}
// | txncommitargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed txn commit cid\n");}
// | txncommitargs tid {$$ = $2, $2->r = $1; OE_TLOG(0, "oejcl parsed txn commit tid\n");}
// ;

writeargs:  {$$ = NULL;} 
 | writeargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed write cid\n");}
 | writeargs tid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed write tid\n");}
 | writeargs dur {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed write dur\n");}
 ;

readargs:  {$$ = NULL; OE_TLOG(0, "oejcl parsed empty read args\n");} 
 | readargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed read cid\n");}
 | readargs tid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed read tid\n");}
 | readargs timeout {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed read timeout\n");}
 ;

connectargs:  {$$ = NULL;}
 | connectargs cid {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed conn cid\n");}
 | connectargs ver {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed conn ver\n");}
 | connectargs extentname {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed conn extentname\n");}
 | connectargs username {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed conn username\n");}
 | connectargs password {$$ = $2; $2->r = $1; OE_TLOG(0, "oejcl parsed conn pwd\n");}
 ;

ver: VER ASSIGN vernum {
    $$ = $3; 
    OE_TLOG(0, "oejcl parsed ver\n");
}
;

vernum: NUMBER DOT NUMBER DOT NUMBER {
    $$ = new_oejcl_ast(parm->arena, VER, NULL, NULL); 
    OE_TLOG(0, "oejcl parsed vernum\n");
}
;

cid: CORRELATION_ID ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, CORRELATION_ID, $3);
    OE_TLOG(0, "oejcl parsed cid %i\n", $3);
}
;

tid: TXN_ID ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, TXN_ID, $3); 
    OE_TLOG(0, "oejcl parsed tid %i\n", $3);
}
;

lid: LEASE_ID ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, LEASE_ID, $3); 
    OE_TLOG(0, "oejcl parsed lid %i\n", $3);
}
;

sid: SESSION_ID ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, SESSION_ID, $3); 
    OE_TLOG(0, "oejcl parsed sid %i\n", $3);
}
;

dur: DURATION ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, DURATION,$3); 
    OE_TLOG(0, "oejcl parsed dur %i\n", $3);
}
;

nresults: NRESULTS ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, NRESULTS,$3); 
    OE_TLOG(0, "oejcl parsed nresults %i\n", $3);
}
;

nenlisted: NENLISTED ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, NENLISTED,$3); 
    OE_TLOG(0, "oejcl parsed nenlisted %i\n", $3);
}
;

timeout: TIMEOUT ASSIGN NUMBER {
    $$ = new_oejcl_num_ast(parm->arena, TIMEOUT,$3); 
    OE_TLOG(0, "oejcl parsed timeout %i\n", $3);
}
;

msg: MSG ASSIGN STRING {
    $$ = new_oejcl_str_ast(parm->arena, MSG, $3); 
    OE_TLOG(0, "oejcl parsed msg %s\n", $3);
}
;

cmd: CMD ASSIGN cmdname {
    $$ = new_oejcl_ast(parm->arena, CMD,  
                       new_oejcl_ast(parm->arena, $3, NULL, NULL),  //cmdname is a type (int)
                       NULL); 
    OE_TLOG(0, "oejcl parsed cmd param\n");
}
;

extentname: EXTENTNAME ASSIGN STRING {
    $$ = new_oejcl_str_ast(parm->arena, EXTENTNAME, $3); 
    OE_TLOG(0, "oejcl parsed extentname param\n");
}
;

username: USERNAME ASSIGN STRING {
    $$ = new_oejcl_str_ast(parm->arena, USERNAME, $3); 
    OE_TLOG(0, "oejcl parsed username param\n");
}
;

password: PASSWORD ASSIGN STRING {
    $$ = new_oejcl_str_ast(parm->arena, PASSWORD, $3); 
    OE_TLOG(0, "oejcl parsed pwd param\n");
}
;

cmdname:
 CONNECT  {$$ = CONNECT;}
 | DIS      {$$ = DIS;}
 | READ     {$$ = READ;}
 | TAKE     {$$ = TAKE;}
 | WRITE    {$$ = WRITE;}
 | TXN      {$$ = TXN;}
 | COMMIT   {$$ = COMMIT;}
 | CANCEL   {$$ = CANCEL;}
 ;

%%

void yyerror(oejcl_parse_parm *pp, void *scanner, char *msg) {
  pp->iserr = 1;
  pp->emsg =  oec_cpy_str(pp->arena, msg);
}


