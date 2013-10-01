var net = require("net");

function start(router, handle, port) {
    net.createServer(function (socket) {
            socket.on('data', function(data) { 
                try {       	
                    var request = JSON.parse(data);
                    router.route(handle, request["type"], request, socket);
                } catch (e) {        
                    console.log("json parse error:\ndata:\n" + data + "\nerror:\n" + e);
                }
            });
    }).listen(port);

    console.log("Server set up");
}

exports.start = start;
