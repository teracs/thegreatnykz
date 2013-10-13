var nykzuser;
$("body").on("click","#btn_login",function(){
  var d = $("#form_login").serializeJson();//{userid,password}
  nykzuser = new NYKZuser(d.userid,d.password,"http://dev-nykz2.scaret.in");
  nykzuser.login(function(callback){
    if(nykzuser.isloggedin){
      nykzuser.boardIndex(function(boards){
        window.boards = boards;
        var content = renderHome(nykzuser.drupaluser);
        pageStack.push(content);
      });
    }
    else{
      alert("登录失败！");
    }
  });
});

function gotoBoard(boardName,start){
    nykzuser.showBoard(boardName,start,function(posts){
        var content = renderBoard(posts);
        //$("#" + pageid).trigger("pagecreate");
        pageStack.push(content);
    });
}

function gotoPost(boardName,filename,returnpageid){
  nykzuser.showPost(boardName,filename,function(post){
    var content = renderPost(post,returnpageid);
    console.log(content);
    pageStack.push(content);
  });
}

function onAjaxAutoFailStart(){
  $.mobile.loading( "show" );
}

function onAjaxAutoFailStop(){
  $.mobile.loading( "hide" );
}
