#include "www.h"
#define MAX_PLAN_SIZE 255*17
static int save_plan(char *plan)
{
    FILE *fp;
    char buf[MAX_PLAN_SIZE];
    char buf2[MAX_PLAN_SIZE];
    size_t i, j = 0, len;

    memset(buf2, 0x00, MAX_PLAN_SIZE);

    fp = fopen(plan, "w");
    strsncpy(buf, getparm("text"), MAX_PLAN_SIZE - 1);
    len = strlen(buf);
    if (len > 0)
    {
        for (i = 0; i < (len - 1); i++)
        {
            if (!((buf[i] == 0x0d) && (buf[i + 1] == 0x0a)))
            {
                buf2[j] = buf[i];
                j++;
            }
        }
        buf2[j] = buf[len - 1];
        fwrite(buf2, j + 1, 1, fp);
    }
    fclose(fp);
    printf("个人说明档修改成功。");
    printf("</html>");
    return 0;
}

int bbsplan_main()
{
    FILE *fp;
    char *ptr, plan[256], buf[10000] = { 0 };

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_quit_print("匆匆过客不能设置说明档，请先登录");
    sprintf(plan, "home/%c/%c/%s/plans", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    if (!strcasecmp(getparm("type"), "update"))
        return save_plan(plan);
    printf
        ("<font class=\"title\">%s - 设置个人说明档　　</font>[%s]<hr>\n",
         BBSNAME, currentuser.userid);
    printf("<form method=post action=bbsplan>\n");
    printf("<input type=hidden name=type value=update>\n");
    fp = fopen(plan, "r");
    if (fp)
    {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("<table><tr><td>");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
    printf("%s", void1(buf));
    printf("</textarea></table>\n");
    printf("<br><hr>\n");
    printf("<input type=submit value=存盘> ");
    printf("<input type=reset value=复原>\n");
    printf("</html>");
    return 0;
}
