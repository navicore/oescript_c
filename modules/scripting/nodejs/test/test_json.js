var oescript = require('../build/Release/oescript');

var d = new oescript.OesnDispatcher(1, "main oescript thread");
d.start();

//var td = new oescript.OesnThreadDispatcher(9);
var td = new oescript.OesnThreadDispatcher(1);
td.start();

var s = new oescript.OesnStore(3, "test/data", true);

var k = new oescript.OesnKernel(d, td, s);

var jkernel = new oescript.OesnJsonKernel(k);

var json_put_cmd = "{                       \
	\"t\":	\"cmd\",                \
	\"n\":	\"put\",                    \
	\"cid\":	20,                         \
	\"dur\":	50000,                      \
	\"values\":	[\"tel\", \"456\"]          \
}"

var json_get_cmd = "{                       \
	\"t\":	\"cmd\",                \
	\"n\":	\"get\",                    \
	\"cid\":	28,                         \
	\"timeout\":	50000,                  \
	\"c\":	1,                      \
	\"rattrs\":	true,               \
	\"templates\":	[[\"tel\", \"456\"]]    \
}"

//var getcmd = JSON.parse(json_get_cmd, null);
var getcmd = JSON.parse(json_get_cmd);
console.log("getcmd:" + getcmd["cid"]);
assert(getcmd["cid"] == 28, "wrong cid!");

jkernel.exec( json_get_cmd, make_callback(json_get_cmd, "28") );
jkernel.exec( json_put_cmd, make_callback(json_put_cmd, "20") );
jkernel.exec( json_get_cmd, make_callback(json_get_cmd, "28") );
jkernel.exec( JSON.stringify(getcmd), make_callback(JSON.stringify(getcmd), "28") );

function make_callback(cmd, teststr) {
    return function(res_str) {
        var res = JSON.parse(res_str);
        console.log("cmd:" + cmd);
        console.log("res str:" + res_str);
        console.log("res cid:" + res["cid"]);
        console.log("teststr:" + teststr);
        assert(res, "no res!");
        assert(cmd, "no cmd!");
        assert(res_str.indexOf(teststr) != -1, "invalid res!");
    }
}

function AssertException(message) { this.message = message; }
AssertException.prototype.toString = function () {
  return 'AssertException: ' + this.message;
}

function assert(exp, message) {
  if (!exp) {
    throw new AssertException(message);
  }
}

putcmd2 = {};
putcmd2.t="cmd";
putcmd2.n="put";
putcmd2.cid=42;
putcmd2.dur=50000;
putcmd2.values=[];
putcmd2.values[0] = "tel";
putcmd2.values[1] = "789";

jkernel.exec( JSON.stringify(putcmd2), make_callback(JSON.stringify(putcmd2), "42") );

getcmd2 = {};
getcmd2.t="cmd";
getcmd2.n="get";
getcmd2.cid=29;
getcmd2.timeout=50000;
getcmd2.c=1;
getcmd2.rattrs=true;
getcmd2.templates=[];
getcmd2.templates[0] = [];
getcmd2.templates[0][0] = "tel";
getcmd2.templates[0][1] = "789";

jkernel.exec( JSON.stringify(getcmd2), make_callback(JSON.stringify(getcmd2), "29") );

