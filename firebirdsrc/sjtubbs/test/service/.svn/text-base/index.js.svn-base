var server = require("./server");
var router = require("./router");
var handles = require("./handles");
var cluster = require("cluster");
var numCPUs = require("os").cpus().length;


var handle = {};
handle["reply"] = handles.reply_handle;
handle["get_reply"] = handles.get_reply_handle;

if (cluster.isMaster) {
    // Fork workers.
    for (var i = 0; i < numCPUs; i++) {
        cluster.fork();
    }

    cluster.on('exit', function(worker, code, signal) {
        console.log('worker ' + worker.process.pid + ' died');
    });
} else {
    server.start(router, handle, 50000);
}
