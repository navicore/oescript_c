//"use strict";
OeHttpClient = function (path) {
    console.log('initializing oescript_http to path ' + path);
    this.path = path;
};

OeHttpClient.prototype.exec = function (jsonstr, callback) {

    var request = new XMLHttpRequest(); //todo: move to 'new()'

    request.open('POST', this.path);
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            callback(request.responseText);
        }
    };
    request.setRequestHeader("Content-Type", "application/text");
    request.send(jsonstr);
};

