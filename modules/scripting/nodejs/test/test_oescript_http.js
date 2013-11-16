require('../lib/oescript_http.js');

var poster = new OeHttpClient('/myoescript');
assert(poster, 'no poster');

function assert(exp, message) {
  if (!exp) {
    throw new AssertException(message);
  }
}

