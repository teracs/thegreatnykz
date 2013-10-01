#include "www.h"

int save_bmail(char *plan)
{
    char buf[10000];
    FILE *fp;

    fp = fopen(plan, "w");
    strsncpy(buf, getparm("text"), 9999);
    fprintf(fp, "%s", buf);
    fclose(fp);
    printf("修改成功。");
    http_quit();
}

int bbsblockmail_main()
{
    FILE *fp;
    char *ptr, plan[256], buf[10000];

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_fatal("匆匆过客不能设置，请先登录");
    sprintf(plan, "home/%c/%s/blockmail", toupper(currentuser.userid[0]),
            currentuser.userid);
    if (!strcasecmp(getparm("type"), "update"))
        save_bmail(plan);
    printf("<font class=\"title\">%s - 邮件设置　　</font>[使用者: %s]<hr>\n", BBSNAME,
           currentuser.userid);
    printf(" 过滤来源地址包含以下字符串的email(目前仅限于站外信件)<br>");
    printf("<form method=post action=bbsblockmail?type=update>\n");
    fp = fopen(plan, "r");
    if (fp)
    {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("<table width=610><tr><td align=center>");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
    printf("%s", void1(buf));
    printf("</textarea></table>\n");
    printf("<input type=submit value=存盘> ");
    printf("<input type=reset value=复原>\n");
    printf("<hr>\n");
    http_quit();
}
