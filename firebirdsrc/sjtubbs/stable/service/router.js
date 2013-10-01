function route(handle, type, request, socket) {
    console.log("Router: receive request type "+type);
	if (typeof(handle[type]) == "function") {
        handle[type](request, socket);
	}
    else {
        console.log("Not request handle bound to type " + type);
        console.log(typeof(handle[type]));
        console.log(handle);
    }
}

exports.route = route;
