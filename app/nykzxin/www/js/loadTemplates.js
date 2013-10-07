var tmpl = {
  'base': 'template/base.tmpl.js',
  'login': 'template/login.tmpl.js',
  'home': 'template/home.tmpl.js',
  'board': 'template/board.tmpl.js',
  'post': 'template/post.dialog.tmpl.js',
}
$.ajaxSetup({async:false});
_.each(tmpl,function(url,id){
  $.get(url,function(data){
    tmpl[id] = _.template(data);
  });
});
$.ajaxSetup({async:true});

$("body")
  .append('<div id="page_login" data-role="page">')
  .append('<div id="page_home" data-role="page">')
  .append('<div id="page_board" data-role="page">');
//login
var content = {
  hasPannel:true,
  pageid:"page_login",
  header:"登陆-南阳客栈信",
  content:tmpl.login(),
  footer:"<h1>南阳客栈信</h1>",
};
$("#" + content.pageid).replaceWith(tmpl.base(content));

//home
function renderHome(drupaluser){
  console.log(drupaluser);
  var content = {
    hasPannel:true,
    pageid:"page_home",
    header:"主页-南阳客栈信",
    content:tmpl.home({drupaluser:drupaluser}),
    footer:"<h1>南阳客栈信</h1>",
  };
  $("#" + content.pageid).replaceWith(tmpl.base(content));
}

function renderBoard(board){
  var pageid = "page_" + new Date().getTime();
  board.returnpageid = pageid;
  var content = {
    hasPannel:true,
    pageid:pageid,
    header:board.board + " 板",
    content:tmpl.board(board),
    footer:'<a href="#" data-icon="plus">发表文章</a>\
    <a href="#" data-icon="arrow-u" onclick="gotoBoard(\'' + board.board + '\',' + (board.start-20) +')">上一页</a>\
    <a href="#" data-icon="arrow-d" onclick="gotoBoard(\'' + board.board + '\',' + (board.start+20) +')">下一页</a>',
  };
  $("body").append(tmpl.base(content));
  console.log("renderBoard:",board);
  return content.pageid;
}

function renderPost(post,returnpageid){
  var content = {
    hasPannel:false,
    pageid:"dialog_" + new Date().getTime(),
    header:"阅读文章",
    content:"<pre>" + post.post + "</pre>",
    footer:post.filename + " @ " + post.boardName,
    returnpage:returnpageid
  };
  $("body").append(tmpl.post(content));
  return content.pageid;
}
