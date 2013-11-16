var oescript = require('../build/Release/oescript');

var d = new oescript.OesnDispatcher(1, "main oescript thread");
d.start();

//var td = new oescript.OesnThreadDispatcher(9);
var td = new oescript.OesnThreadDispatcher(1);
//var td = new oescript.OesnThreadDispatcher(0);
td.start();

var s = new oescript.OesnStore(3, "test/data", true);

var k = new oescript.OesnKernel(d, td, s);

var jkernel = new oescript.OesnJsonKernel(k);
assert(jkernel, "bad jkernel");

// begin oejson net

var loginmod = new oescript.OesnBuiltInLoginModule();
loginmod.addAccount("users", "esweeney", "200000");
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"connect"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"disconnect"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"put"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"get"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"start"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"utxn"}');
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"ulease"}');

var net = new oescript.OesnNet(d);
net.setName("tcp server");
var json = new oescript.OesnServer(d, "oejson://0.0.0.0:2555/myspace", net, s);
assert(json);
json.addLoginModule(loginmod);
json.addKernel("myspace", k);

// end oejson net

function AssertException(message) { this.message = message; }
AssertException.prototype.toString = function () {
  return 'AssertException: ' + this.message;
}

function assert(exp, message) {
  if (!exp) {
    throw new AssertException(message);
  }
}

var passport = require('passport')
  , LocalStrategy = require('passport-local').Strategy;

passport.use(new LocalStrategy(
  function(username, password, done) {
    User.findOne({ username: username }, function(err, user) {
      if (err) { return done(err); }
      if (!user) {
        return done(null, false, { message: 'Unknown user' });
      }
      if (!user.validPassword(password)) {
        return done(null, false, { message: 'Invalid password' });
      }
      return done(null, user);
    });
  }
));

