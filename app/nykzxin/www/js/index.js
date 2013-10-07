var nykzuser;
$("#btn_login").on("click",function(){
  var d = $("#form_login").serializeJson();//{userid,password}
  nykzuser = new NYKZuser(d.userid,d.password);
  nykzuser.login(function(callback){
    if(nykzuser.isloggedin){
      renderHome(nykzuser.drupaluser);
      $.mobile.changePage($("#page_home"));
    }
    else{
      alert("登录失败！");
    }
  });
});

function gotoBoard(boardName,start){
    nykzuser.showBoard(boardName,start,function(posts){
        var pageid = renderBoard(posts);
        //$("#" + pageid).trigger("pagecreate");
        $.mobile.changePage($("#" + pageid));
    });
}

function gotoPost(boardName,filename,returnpageid){
  nykzuser.showPost(boardName,filename,function(post){
    var dialogid = renderPost(post,returnpageid);
    console.log(dialogid);
    $.mobile.changePage( "#" + dialogid);
  });
}
