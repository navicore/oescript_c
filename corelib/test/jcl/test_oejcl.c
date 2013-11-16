#include "oejcl_parser.h"
#include "assert.h"
#include "oejcl_parser.tab.h"

int main(void) {

  Arena_T arena = Arena_new();
  
  oejcl_ast *res = NULL;
  assert(!res);
  oejcl_parse(arena, "myvar = 100;", &res);
  oejcl_parse(arena, "myvar = 101;read(tid=99 cid=123 timeout=1 [\"key1\":\"my secret\"]);", &res);
  oejcl_parse(arena, "txn (timeout=100000) {myvar = 102;read(tid=98 cid=223 timeout=2 [\"key2\":\"your secret\"]);};", &res);
  oejcl_parse(arena, "take(tid=38 cid=2123 timeout=22 [\"one\" \"two\"]);", &res);
  oejcl_parse(arena, "$myvar = test_$data[\"key999\"];", &res);

  oejcl_parse(arena, "$myvariable = \"seeme\";", &res);
  assert(res);
  assert(res->nodetype == STMT);

  oejcl_parse(arena, "connect(cid=99 version=1.0.9);", &res);

  oejcl_parse(arena, "response(cmd=read cid=109);", &res);

  Arena_dispose(arena);

  return 0;
}

