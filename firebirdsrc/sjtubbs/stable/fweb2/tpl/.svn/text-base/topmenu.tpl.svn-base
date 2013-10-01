<!DOCTYPE html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=GB2312">
<title>饮水思源</title>
<link rel="stylesheet" type="text/css" href="/file/bbs/slidemenu.css" />
<script type="text/javascript" src="/file/bbs/slidemenu.js"></script>
<script type="text/javascript" src="/file/bbs/bbsleft.js"></script>
<style>
BODY {
        FONT-SIZE: 12px;
        color: #FFFFFF;
	 		background-color: #014DA2;
	 		
}
#left{
    padding-top: 2px;
	float:left;
	overflow: hidden;
}
#right {
	float: left;
	height:120px;
	width:500px;
	overflow: hidden;
}
form {
	margin:0px;
	padding: 0px;
	}
#righttop {
	
	}
 a:link {  color: #ACE4F8;; text-decoration: none;}
 a:hover {text-decoration: underline overline;}
 a:visited { text-decoration:none; color: #ACE4F8;}
td,ul,li {
margin:0px;padding:0px;
}
input {
	font-size: 12px;
	}
.submenu {
	padding-top: 15px;
	vertical-align: top;
	line-height: 20px;
	}
.submenu a {
	background-image: url('/file/bbs/link.gif');
	background-repeat: no-repeat;
	padding-left: 32px;
}
img {
	border: none;
	}
div {
	float: left;
	}
</style>
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-144567-2']);
  _gaq.push(['_setDomainName', 'none']);
  _gaq.push(['_setAllowLinker', true]);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
</head>
<body onload="slideMenu.build('sm',390,10,10,1);">
<input type=hidden name="temp" id="temp" value="">
<iframe id="framehide" name="framehide" width="0" height="0" src="" frameborder="0" scrolling="no"></iframe>
<div id="left">
{{#guest}}
 <form action=bbslogin target=_top method=post>
               帐号 <input style='height:14px;' type=text name=id maxlength=12 size=8><br>
               密码 <input style='height:14px;' type=password name=pw maxlength=12 size=8><br>
               <input style='width:45px;' type=submit value=登录>
	       <input style='width:45px;' name="button" type="button" onClick="open('bbsreg', '', 'width=620,height=550')" value="注册" class="button">
</form>
<form action=bbssel target=f3>
选择讨论区: <br><input type=text name=board maxlength=15 size=8> <input type=submit value=go>
</form>
{{/guest}}
{{#loginin}}
用户: <a class=left href=bbsqry?userid={{user_id}} target=f3>{{user_id}}</a><br>级别: {{user_level}}<br><a class=left href=bbslogout target=_top>[注销本次登录]</a>
                <form action=bbssel target=f3 style='margin:0px;padding-top:10px;'>
选择讨论区: <br><input type=text name=board maxlength=15 size=8> <input type=submit value=go></form>
{{/loginin}}
</div>
<div id="right">
<div id="righttop">
<img src=/file/bbs/link0.gif><a target=f3 href=bbs0an>精华公布栏</a>
<img src=/file/bbs/link0.gif><a target=f3 href=php/bbsindex.html>WEB导读</a>
<img src=/file/bbs/link0.gif><a target=f3 href=wiki>水源百科</a>
<img src=/file/bbs/link0.gif><a target=f3 href=bbstopb10>热门讨论区</a>
<img src=/file/bbs/telnet.gif><a href='telnet:yssy.org'>Telnet登录</a>
{{#fillform}}<a style='color:red; font-size:15px' target=f3 href=bbsform><b>填写注册单</b></a>{{/fillform}}
</div>
<div id="rightmain">
<ul class=sm id=sm>
{{#fav}}
<li>

<table summary="" width="400px">
<tr><td width=10px ><img src="/file/bbs/folder.gif">我的收藏</td><td class="fav">
<div id="main.hck" style="height:80px;overflow:auto;width:96%;">
Loading...
</div>
<script type="text/javascript" >
submenu("main.hck",1);
</script>
</td></tr>
</table>
</li>
{{/fav}}
<li><table summary="" width="380px">
<tr><td width=10px><img src="/file/bbs/folder.gif" >分类讨论</td><td class="submenu">
			  <a  target=f3 href=bbsboa?sec=0> 本站系统 </a>
           <a  target=f3 href=bbsboa?sec=1> 上海交大</a>
           <a  target=f3 href=bbsboa?sec=2> 学子院校</a>
           <a  target=f3 href=bbsboa?sec=3> 电脑技术</a><br>

           <a  target=f3 href=bbsboa?sec=4> 学术科学</a>
           <a  target=f3 href=bbsboa?sec=5> 艺术文化</a>
           <a  target=f3 href=bbsboa?sec=6> 体育运动</a>
           <a  target=f3 href=bbsboa?sec=7> 休闲娱乐</a><br>
           <a  target=f3 href=bbsboa?sec=8> 知性感性</a>

           <a  target=f3 href=bbsboa?sec=9> 社会信息</a>
           <a  target=f3 href=bbsboa?sec=10> 社团群体</a>
           <a  target=f3 href=bbsboa?sec=11> 游戏专区</a>
           </td></tr>
</table></li>
{{#mail}}
<li>
<table summary="" width="400px">
<tr><td width=10px ><img src="/file/bbs/folder.gif">站内交流</td><td class="submenu">
 <a target=f3 href=bbsnewmail> 阅览新邮件</a>

               <a target=f3 href=bbsmail> 所有邮件</a>
               <a target=f3 href=bbspstmail> 发送邮件</a><br>
               <a href=bbsfriend target=f3> 在线好友</a>

<a href=bbsusr target=f3> 环顾四方</a>
<a href=bbsqry target=f3> 查询网友</a>
<br>
<a target=f3 href=bbsnotepad>看看留言板</a>
<a target=f3 href=bbsalluser>所有使用者</a>
<a href=bbsshowinfo target=f3> 在线情况分析</a>
</td></tr>
</table>
</li>
{{/mail}}
{{#tbox}}
<li>
<table summary="" width="400px">
<tr><td width=10px ><img src="/file/bbs/folder.gif">个人工具</td><td class="submenu">
 <a  target=f3 href=bbsinfo> 个人资料</a>
               <a  target=f3 href=bbsplan> 改说明档</a>
               <a  target=f3 href=bbssig> 改签名档</a><br>
               <a  target=f3 href=bbspwd> 修改密码</a>
               <a  target=f3 href=bbsparm> 修改个人参数</a>
               <a  target=f3 href=bbsmywww> WWW个人定制</a><br>

               <a  target=f3 href=bbsnick> 临时改昵称</a>
               <a  target=f3 href=bbsfall> 设定好友</a>
</td></tr>
</table>
</li>
{{/tbox}}
{{#sn}}
<li>
<table summary="" width="400px">
<tr><td width=10px ><img src="/file/bbs/folder.gif">占位置</td><td>
&nbsp;新功能开发中...
</td></tr>
</table>
</li>
{{/sn}}
</ul>
</div>
</div>
</body>
</html>
