var oescript = require('../build/Release/oescript');

var d = new oescript.OesnDispatcher(1, "main oescript thread");
d.start();

//var td = new oescript.OesnThreadDispatcher(9);
var td = new oescript.OesnThreadDispatcher(1);
td.start();

var s = new oescript.OesnStore(3, "test/data", true);

var k = new oescript.OesnKernel(d, td, s);

var net = new oescript.OesnNet(d);

//var sslnet = new oescript.OesnSslNet(d, "test/cert", "test/pkey");

var json = new oescript.OesnServer(d, "oejson://localhost:5555", net, s);

//var sjson = new oescript.OesnServer(d, "oejson://localhost:5556", sslnet, s);

var loginmod = new oescript.OesnBuiltInLoginModule();
loginmod.addAccount("users", "esweeney", "123456");
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"connect"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"disconnect"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"put"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"get"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"start"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"utxn"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"ulease"}')

json.addLoginModule(loginmod);
json.addKernel("myspace", k);

//sjson.addLoginModule(loginmod);
//sjson.addKernel("myspace", k);

