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

//是否登录通过用户的isloggedin判断
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

NYKZuser.prototype.boardIndex = function(callback){
  var self = this;
  if(!callback) callback = function(){};
  self.ajaxAutoFail("get",self.endpoint + "/nykzboard",null,function(boards){
    console.log(boards);
    callback(boards);
  });
}

NYKZuser.prototype.userIndex = function(callback){
  var self = this;
  if(!callback) callback = function(){};
  self.ajaxAutoFail("get",self.endpoint + "/nykzuser",null,function(users){
    console.log(users);
    callback(users);
  });
}

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

NYKZuser.prototype.ajaxAutoFail = function(method, url, data, callback,recursive){
  var self = this;
  $.ajax({
    url: url,
    type: method,
    data : data? JSON.stringify(data):data,
    contentType: "application/json; charset=utf-8",
  }).success(function(data,statustext,xhr){
    callback(data,xhr);
  }).fail(function(xhr,statustext,description){
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
