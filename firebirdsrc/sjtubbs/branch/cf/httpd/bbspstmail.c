#include "www.h"

int bbspstmail_main()
{
    FILE *fp;
    int i;
    char userid[80], buf[512], path[512], file[512], title[80] = "";

    init_all();
    if (!loginok)
        http_fatal("匆匆过客不能写信，请先登录");
    if (!HAS_PERM(PERM_SYSOP))
    {
        if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_CHAT))
            http_fatal("抱歉, 你被封禁了发文权限, 无法发信.");
    }
    strsncpy(file, getparm("file"), 20);
    strsncpy(title, nohtml(getparm("title")), 50);
    strsncpy(userid, getparm("userid"), 40);
    if (file[0] != 'M' && file[0])
        http_fatal("错误的文件名");
    printf("<center>\n");
    printf("%s -- 寄语信鸽 [使用者: %s]<hr color=green>\n", BBSNAME,
           currentuser.userid);
    printf
    ("<table bgcolor=f0f0f0 style='BORDER: 2px solid; BORDER-COLOR: d8d8d8;'><tr><td>\n");
    printf("<form method=post action=bbssndmail?userid=%s>\n", userid);
    printf
    ("信件标题: <input type=text name=title size=40 maxlength=100 value='%s'> ",
     title);
    printf("发信人: &nbsp;%s<br>\n", currentuser.userid);
    printf("收信人: &nbsp;&nbsp<input type=text name=userid value='%s'>",
           nohtml(userid));
    printf("  使用签名档 ");
    printf("<input type=radio name=signature value=1 checked>1\n");
    printf("<input type=radio name=signature value=2>2\n");
    printf("<input type=radio name=signature value=3>3\n");
    printf("<input type=radio name=signature value=4>4\n");
    printf("<input type=radio name=signature value=5>5\n");
    printf("<input type=radio name=signature value=0>0\n");
    printf("备份<input type=checkbox name=backup>\n");
    printf("<br>\n");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n\n");
    if (file[0])
    {
        int lines = 0;

        printf("【 在 %s 的来信中提到: 】\n", userid);
        sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
                currentuser.userid, file);
        fp = fopen(path, "r");
        if (fp)
        {
            for (i = 0; i < 4; i++)
                if (fgets(buf, 500, fp) == 0)
                    break;
            while (1)
            {
                if (fgets(buf, 500, fp) == 0)
                    break;
                if (!strncmp(buf, ": 【", 4))
                    continue;
                if (!strncmp(buf, ": : ", 4))
                    continue;
                if (!strncmp(buf, "--\n", 3))
                    break;
                if (buf[0] == '\n')
                    continue;
                ;
                if (!strcasestr(buf, "</textarea>"))
                    printf(": %s", buf);
                if (lines++ > 20)
                {
                    printf(": (以下引言省略 ... ...)");
                    break;
                }
            }
            fprintf(fp, "\n");
            fclose(fp);
        }
    }
    printf("</textarea><br><br><div align=center>\n");
    printf("<input type=submit value=发送信件> ");
    printf("<input type=reset value=内容复原></form>\n");
    printf("</div></table>");
    http_quit();
}
