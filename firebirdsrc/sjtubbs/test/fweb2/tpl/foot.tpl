<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
{{>header}}
<style type="text/css">
A {color: #0000FF}
</style>
</head>
<body>
<div id='foot'>
<table width="100%">
<tr>
<td>时间[{{time_now}}] 在线[<a href="bbsusr" target="f3">{{user_num}}</a>] 帐号[<a href="bbsqry?userid={{user_id}}" target="f3">{{user_id}}</a>]{{#mail}} 信箱[<a href=bbsmail target=f3>{{mail_total}}{{#mail_new}}<font color=red>(新信{{mail_unread}})</font>{{/mail_new}}</a>]{{/mail}}{{#tip}}&nbsp;板面[<a href="/bbsdoc?board={{tip_board}}" target="f3">{{tip_board}}</a>]&nbsp;<b>TIP</b>[{{tip_content}}]{{/tip}}</td>
<td align="right">本次停留[{{stay_hour}}小时{{stay_minute}}分]</td>
</tr>
</table>
<form><input type='hidden' name='msg' value='{{msg_value}}' id='cmsg'></form>
<script type="text/javascript">if(!top.f4_func)settimeout("document.location = document.location",60000);
</script></div>
</body>
</html>
