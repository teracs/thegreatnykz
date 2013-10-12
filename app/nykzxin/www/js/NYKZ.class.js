NYKZuser = function(userid,password,host,path2endpoint){
  this.host = host ? host : "http://dev-nykz.scaret.in";
  this.path2endpoint = path2endpoint ? path2endpoint : "nykz";
  this.endpoint = this.host + '/' + this.path2endpoint;
  this.userid = userid;
  this.password = password;
  this.isloggedin = false;
  this.drupaluser = null;
}

NYKZuser.prototype.setCSRFToken = function(callback){
  this.ajaxAutoFail("get",this.host + "/services/session/token", null, function(token){
    $.ajaxSetup({headers:{"X-CSRF-token":token}});
    callback(token);
  });
}

//callback没有参数
NYKZuser.prototype.login = function(callback){
  var self = this;
  if(!callback) callback = function(){}
  self.setCSRFToken(function(){
    var req = {userid:self.userid,password:self.password};
    self.ajaxAutoFail("post",self.endpoint + "/nykzuser/login",req,function(drupaluser){
      if(drupaluser instanceof Array && !drupaluser[0]){
        callback();
      }
      else{
        self.isloggedin = true;
        self.drupaluser = drupaluser;
        console.log("user " + drupaluser.name + " logged in");
        self.setCSRFToken(function(){
          callback();
        });
      }
    });
  });
}

//callback([username,username,username])
NYKZuser.prototype.userIndex = function(callback){
  var self = this;
  if(!callback) callback = function(){};
  self.ajaxAutoFail("get",self.endpoint + "/nykzuser",null,function(users){
    console.log(users);
    callback(users);
  });
}

//callback({userid,username,gender...})
NYKZuser.prototype.userGet = function(userid,callback){
  var self = this;
  if(!callback) callback = function(){};
  self.ajaxAutoFail("get",self.endpoint + "/nykzuser/" + userid,null,function(user){
    console.log(user[userid]);
    callback(user[userid]);
  });
}

//callback({boardName:{filename,title,allow,level...}}})
NYKZuser.prototype.boardIndex = function(callback){
  var self = this;
  if(!callback) callback = function(){};
  self.ajaxAutoFail("get",self.endpoint + "/nykzboard",null,function(boards){
    console.log(boards);
    _.each(boards,function(board,boardName){
      if(board.level){
        delete boards[boardName];
      }
    });
    callback(boards);
  });
}

//callback({board,start,posts[{title,filename,owner,id,reid...}...]});
NYKZuser.prototype.showBoard = function(boardName,start,callback){
  var self = this;
  if(!callback) callback = function(){};
  var req = {
    boardName:boardName,
    start:start,
  };
  self.ajaxAutoFail("post",self.endpoint + "/nykzboard/show",req,function(posts){
    console.log(posts);
    callback(posts);
  });
}

//callback({boardName,filename,post})
NYKZuser.prototype.showPost = function(boardName,filename,callback){
  var self = this;
  if(!callback) callback = function(){};
  var req = {
    boardName:boardName,
    filename:filename,
  };
  self.ajaxAutoFail("post",self.endpoint + "/nykzpost/show",req,function(post){
    console.log(post);
    callback(post);
  });
}

//callback({boardName,filename,post})
NYKZuser.prototype.createPost = function(boardName,title,content,reid,callback){
  var self = this;
  if(!callback) callback = function(){};
  if(!reid) reid=null;
  var req = {
    boardName:boardName,
    title:title,
    content:content,
    reid:reid
  };
  self.ajaxAutoFail("post",self.endpoint + "/nykzpost/create",req,function(post){
    console.log(post);
    callback(post);
  });
}

//如果有onAjaxAutoFailStart和onAjaxAutoFailStop，则会调用
NYKZuser.prototype.ajaxAutoFail = function(method, url, data, callback,recursive){
  var self = this;
  if(typeof onAjaxAutoFailStart == "function") onAjaxAutoFailStart();
  $.ajax({
    url: url,
    type: method,
    data : data? JSON.stringify(data):data,
    contentType: "application/json; charset=utf-8",
  }).success(function(data,statustext,xhr){
    if(typeof onAjaxAutoFailStop == "function") onAjaxAutoFailStop();
    callback(data,xhr);
  }).fail(function(xhr,statustext,description){
    if(typeof onAjaxAutoFailStop == "function") onAjaxAutoFailStop();
    if(description.search("CSRF")!= -1 && !recursive){
      self.setCSRFToken(function(){
        self.ajaxAutoFail(method, url, data, callback, true);
      });
    }
    else{
      alert(statustext + " : " + description);
      console.error(arguments);
    }
  });
}
