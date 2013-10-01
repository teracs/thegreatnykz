var dbclient = require("mongodb").MongoClient;
var dburl = "mongodb://localhost:27017/bbs";
var dbcollection = "notify.reply";
var config_collection = "notify.config";
var status_collection = "notify.status";

function default_db_handle(err, item) {
    if (err)
        return console.dir(err);
    else
        db.close();
}
function reply_handle(request, socket) {
    if (typeof(request["to_user"]) != undefined) {
        dbclient.connect(dburl, function(err, db) {
	        if (err) {
                return console.dir(err);
            }
            db.collection(config_collection, function(err, config){
				if (err)
					console.dir(err);
				else
					config.findOne({user : request["to_user"], version : 1}, function (err, user) {
						if (err)
							console.dir(err);
						else if (user == null) {
							user = {};
							user["user"] = request["to_user"];
							user["version"] = 1;
							user["enable_reply_msg"] = 1;
							user["enable_reply_notify"] = 1;
							config.insert(user, {w:1}, function (err, item) {});
						}
						if (user["enable_reply_msg"] == 1) {
							request["time"] = new Date().getTime();
							db.collection(dbcollection, function(err, reply) {
								reply.insert(request, {w:1}, function (err, item) {});
							});
						}
						if (user["enable_reply_notify"] == 1) {
							db.collection(status_collection).update({user : request["to_user"]},{ user : request , status : 1}, {upsert : true}, function(err, item) { db.close();});

						}
			db.close()		});
			});
            socket.setTimeout(500);
            socket.end("{ \"status\" : \"success\" }");
        });
    }
    else {
		console.log("Reply Request: Missing to field");
		socket.setTimeout(500);
		socket.end("{ \"status\" : \"fail\" }");
    }
}

function get_reply_handle(request, socket) {
    if (typeof(request["user"]) != undefined) {
        dbclient.connect(dburl, function(err, db) {
	        if (err) {
                return console.dir(err);
            }
            db.collection(config_collection, function(err, config){
				if (err)
					console.dir(err);
				else
					config.findOne({user : request["user"], version : 1}, function (err, user) {
						if (err)
							console.dir(err);
						else if (user == null) {
							user = {};
							user["user"] = request["user"];
							user["version"] = 1;
							user["enable_reply_msg"] = 1;
							user["enable_reply_notify"] = 1;
							config.insert(user, {w:1}, function (err, item) { db.close();});
						}
						if (user["enable_reply_msg"] == 1) {
							db.collection(dbcollection, function(err, reply) {
								reply.find({to_user : request["user"]}, {type : 0, _id : 0}).toArray(function (err,items) { 
									var result = {};
									if (err != null) {
										console.log("query error "+err); 
										result["status"] = "fail";
										result["items"] = [];
									}
									else {
										result["status"] = "success";
										result["items"] = items;
									}
									data = JSON.stringify(result);
									db.close();
									socket.end(data);
								}); 
							});
						}
						if (user["enable_reply_notify"] == 1) {
							db.collection(status_collection).update({user : request["user"]},{ user : request , status : 1}, {upsert : true}, function(err, item) { db.close();});

						}
					});
			});
        });
    }
	else {
		db.close();
		console.log("Get Reply Request: Missing to field");
		socket.end();
	}
}
exports.reply_handle = reply_handle;
exports.get_reply_handle = get_reply_handle;
