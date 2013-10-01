#include "www.h"

int bbsreg_main()
{
    unsigned int r;

    init_all();
    if (noregist())
        http_quit_print("对不起，本站暂停注册新用户");

    r = random();
    printf
        ("<nobr><center><font class=\"title\">%s - 新用户注册</font><hr>\n",
         BBSNAME);
    printf
        ("<font color=green>欢迎加入本站. 以下资料除帐号, 昵称, 星座以外, 系统会为您保密, 请如实填写.</font>");
    printf("<form name=\"form1\" method=post action=bbsdoreg>\n");
    printf("<table width=700>\n");
    printf
        ("<tr><td align=right>*请输入代号:<td align=left><input name=userid size=12 maxlength=12> (2-12字符, 必须全为英文字母) <input type=\"button\" value=\"检测用户名\" onclick=\"return chkid()\">\n");
    printf
        ("<tr><td align=right>*请输入密码:<td align=left><input type=password name=pass1 size=12 maxlength=12> (4-12字符)\n");
    printf
        ("<tr><td align=right>*请确认密码:<td align=left><input type=password name=pass2 size=12 maxlength=12>\n");
    printf
        ("<tr><td align=right>*请输入验证码:<td align=left><input type=text name=vcode maxlength=4><img src=\"bbsvcode?file=%x\" />\n	<input type=hidden name=vcodefile value=%x />",
         r, r);
    printf
        ("<tr><td align=right>*请输入昵称:<td align=left><input name=username size=20 maxlength=32> (2-30字符, 中英文不限)\n");
    printf
        ("<tr><td align=right>*请输入您的真实姓名:<td align=left><input name=realname size=20> (请用中文, 至少2个汉字)\n");
    printf
        ("<tr><td align=right>*学校系级或工作单位:<td align=left><input name=dept size=40 maxlength=64> (至少6个字符)\n");
    printf
        ("<tr><td align=right>*详细通讯地址:<td align=left><input name=address size=40 maxlength=64> (至少10个字符)\n");
    printf("<tr><td align=right>*您的出生年月:\n");
    printf("<td align=left><input name=year size=4 maxlength=4>年");
    printf("<input name=month size=2 maxlength=2>月");
    printf("<input name=day size=2 maxlength=2>日<br>");
    printf
        ("<tr><td align=right>请输入您的性别: <td align=left><input type=radio name=gender value=0 checked>男 ");
    printf("<input type=radio name=gender value=1>女<br>\n");
    printf("<tr><td align=right>星座显示方式: ");
    printf
        ("<td align=left><input type=radio name=xz value=0 checked>颜色区分性别 ");
    printf("<input type=radio name=xz value=1>不区分颜色 ");
    printf("<input type=radio name=xz value=2>不显示星座\n");
    printf
        ("<tr><td align=right>您的Email地址(可选):<td align=left><input name=email size=40 maxlength=64>\n");
    printf
        ("<tr><td align=right>您的联络电话(可选):<td align=left><input name=phone size=40>\n");
    printf("<tr><td align=right>上站留言(可选):<td align=left>");
    printf
        ("<textarea name=words rows=3 cols=40 wrap=physicle></textarea>");
    printf("</table><br><hr>\n");
    printf
        ("<input type=submit value=提交表格> <input type=reset value=重新填写>\n");
    printf("</form></center>\n");

    printf("<SCRIPT LANGUAGE=\"JavaScript\">\n");
    printf("function chkid(){\n");
    printf("strname=document.form1.userid.value;\n");
    printf("if (strname == \"\")\n");
    printf("{\n");
    printf("window.alert(\"请填写您的用户名！\");\n");
    printf("document.form1.userid.focus();\n");
    printf("return (false);\n");
    printf("} \n");
    printf("var url=\"bbschkid?userid=\"+strname\n");
    printf
        ("WriterWindow = window.open(url,\"writerWindow\",\"toolbar=no,width=300,height=150;,directories=no,status=no,scrollbars=no,resize=no,menubar=no\")\n");
    printf("}\n");
    printf("</SCRIPT>\n");
    printf("</html>");
    return 0;
}
