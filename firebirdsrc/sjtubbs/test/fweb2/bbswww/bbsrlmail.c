#include "www.h"

int bbsrlmail_main()
{

    FILE *ftemp;
    char filename[28], buffer[51], firstchar, buf[512];

    cache_age = 3000;
    init_all();
    if (!loginok)
    {
        redirect("/");
        http_quit();
    }
    printf
        ("<nobr><center><font class=\"title\">%s - 邮箱认证 (%s)</font><hr>\n",
         BBSNAME, currentuser.userid);
    printf("<pre>");
    ftemp = fopen(REAL_NAME_EMAIL_RULE_DSC, "r");
    if (ftemp == 0)
        http_quit_print("错误的文件名");
    while (1)
    {
        if (fgets(buf, 256, ftemp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(ftemp);
    printf("</pre><br>");

    firstchar = toupper(currentuser.userid[0]);
    sprintf(filename, "home/%c/%c/%s/real_email", firstchar,
            toupper(currentuser.userid[1]), currentuser.userid);

    if ((ftemp = fopen(filename, "r")) == NULL)
    {

        printf("<form method=post action=bbsdorlmail>\n");
        printf("<table width=600>\n");
        printf
            ("<tr><td align=right>*请输入邮箱:<td align=left><input name=email size=39 maxlength=39> (少于40字符)\n");

        printf("</table><br><hr>\n");
        printf
            ("<input type=submit value=提交表格> <input type=reset value=重新填写>\n");
        printf("</form></center>");
        printf
            ("<a class=left href=bbslogout target=_top>[注销本次登录]</a>\n                <hr class=left width=94px align=left>\n");
        printf("</html>");
        return 0;
    }
    else
    {
        while (fgets(buffer, 32, ftemp))
        {
            printf
                ("<font color=red>您的帐号尚未激活邮箱，未激活邮箱的账号不能发文。<br>");
            printf("<form method=post action=bbsdorlmail>\n");
            printf("<table width=600>\n");
            printf
                ("<tr><td align=right>*您想重新绑定的邮箱:<td align=left><input name=email size=39 maxlength=39> (少于40字符)\n");

            printf("</table><br><hr>\n");
            printf
                ("<input type=submit value=提交表格> <input type=reset value=重新填写>\n");
            printf("</form></center>");
            printf
                ("<a class=left href=bbslogout target=_top>[注销本次登录]</a>\n                <hr class=left width=94px align=left>\n");
            fclose(ftemp);
        }
        printf("</html>");
        return 0;
    }

}
