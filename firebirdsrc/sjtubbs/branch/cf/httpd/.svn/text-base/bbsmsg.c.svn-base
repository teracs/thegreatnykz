#include "www.h"

int bbsmsg_main()
{
    FILE *fp;
    char buf[512], path[512];

    init_all();
    if (!loginok)
        http_fatal("匆匆过客无法查看讯息, 请先登录");
    sethomefile(path, currentuser.userid, "msgfile.me");
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("没有任何讯息");
    printf("<center>%s -- 查看所有讯息 [使用者: %s]<hr color=green>\n",
           BBSNAME, currentuser.userid);
    printf("<table><tr><td><pre>\n");
    while (1)
    {
        char id[20] = "", *ptr;
        int pid = 1;

        memset(buf, 0x00, 256);
        if (fgets(buf, 256, fp) <= 0)
            break;
        //modified by rwzmm @ sjtubbs, string parse
        //        if (!strncmp(buf + 10, "发送", 4))
        //		{
        //            hprintf("%s", buf);
        //            continue;
        //        }

        if(strncmp(buf, "\033[0;1;44;36m", 12))	//如果不是这个字符串开头
        {
            hprintf("%s", buf);
            continue;
        }

        if(IsNumber(buf+35))	//从web发过来的讯息
        {
            sscanf(buf + 12, "%12s", id);
            ptr = buf + 24;
            ptr[82] = 0;
            sscanf(ptr + 98, "%d", &pid);
            printf("<a href=bbsqry?userid=%s>%-12s</a>", id, id);
            hprintf("%-82s ", ptr);
            printf("<a href=bbssendmsg?destid=%s>回讯息</a>\n", id);
        }
        else		//从telnet用户发过来的讯息
        {
            sscanf(buf + 12, "%12s", id);
            ptr = buf + 24;
            memset(ptr+60, 0x20, 17);		//把"ctrl z回复"字样掩盖掉
            ptr[77] = 0;

            sscanf(ptr + 98, "%d", &pid);
            printf("<a href=bbsqry?userid=%s>%-12s</a>", id, id);
            hprintf("%-77s ", ptr);
            printf("<a href=bbssendmsg?destid=%s>回讯息</a>\n", id);
        }

    }
    fclose(fp);
    printf("<br>\n");
    printf
    ("[<a onclick='return confirm(\"你真的要清除所有讯息吗?\")' href=bbsdelmsg>清除所有讯息</a>] ");
    printf("[<a href=bbsmailmsg>讯息寄回信箱</a>]");
    printf("</pre>");
    printf("</table");
    http_quit();
}

int IsNumber(char* pchar)
{
    if(pchar == NULL)
        return 0;

    if((*pchar >= '0') && (*pchar <= '9'))
        return 1;

    return 0;
}
