#include "www.h"

int bbsplan_main()
{
    FILE *fp;
    char *ptr, plan[256], buf[10000];

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_fatal("匆匆过客不能设置说明档，请先登录");
    sprintf(plan, "home/%c/%s/plans", toupper(currentuser.userid[0]),
            currentuser.userid);
    if (!strcasecmp(getparm("type"), "update"))
        save_plan(plan);
    printf("%s -- 设置个人说明档 [%s]<hr color=green>\n", BBSNAME,
           currentuser.userid);
    printf("<form method=post action=bbsplan?type=update>\n");
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
    printf("<input type=submit value=存盘> ");
    printf("<input type=reset value=复原>\n");
    printf("<hr>\n");
    http_quit();
}

int save_plan(char *plan)
{
    FILE *fp;
    char buf[10000];
	char buf2[10000];
	int i = 0;
	int j = 0;
	memset(buf2, 0x00, 10000);

    fp = fopen(plan, "w");
    strsncpy(buf, getparm("text"), 9999);
	
	for(i = 0; i < (strlen(buf) - 1); i++)
	{
		if(!((buf[i] == 0x0d) && (buf[i+1] == 0x0a)))
		{
			buf2[j] = buf[i];
			j++;
		}
	}
    fprintf(fp, "%s", buf2);
    fclose(fp);
    printf("个人说明档修改成功。");
    http_quit();
}
