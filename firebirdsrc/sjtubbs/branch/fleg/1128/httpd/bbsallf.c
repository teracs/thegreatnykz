#include "www.h"

int bbsallf_main()
{
    FILE *fp;
    char *ptr, board[256];
    static struct dir x[40000], xx;
    int i, start, total = 0;

    init_all();
    if (!currentuser.userlevel & PERM_POST)
        http_fatal("error");
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_fatal("没有找到文件");
    while (total < 40000)
    {
        if (fread(&xx, sizeof(xx), 1, fp) <= 0)
            break;
        if (!xx.active)
            continue;
        x[total] = xx;
        total++;
    }
    ptr = getparm("start");
    if (ptr[0] == 0)
    {
        start = total - 19;
    }
    else
    {
        start = atoi(ptr);
    }
    if (start > total - 19)
        start = total - 19;
    if (start < 0)
        start = 0;
    printf("<center><font class=\"title\">%s - 全站上载文件<hr>\n", BBSNAME);
    printf
    ("<table width=720><tr bgcolor=e8e8e8><td>序号<td>文件名<td>上载者<td>时间<td>大小<td>下载次数<td>说明<td>管理<td>备注\n");
    for (i = start; i < start + 20 && i < total; i++)
    {
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=/showfile?name=%s target=_blank>%s</a>",
               nohtml(x[i].showname), nohtml(x[i].showname));
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td>n/a");      // %d", x[i].click);
        printf("<td>%s", x[i].exp);
        printf
        ("<td><a onclick='return confirm(\"确定删除吗?\")' href=bbsudel?file=%s>删</a>\n",
         x[i].showname);
        printf("<td><a href=bbsfexp?name=%s>其他属性 </a>\n",
               nohtml(x[i].showname));
    }
    printf("</table><br>\n");
    printf("[<a href=bbsfdoc?board=%s>刷新</a>]\n", board, board, board);
    if (start > 0)
        printf("[<a href=bbsallf?start=%d>上一页</a>] ", start - 20);
    if (start < total - 19)
        printf("[<a href=bbsallf?start=%d>下一页</a>] ", start + 20);
    printf
    ("<form action=bbsallf>\n跳转到 <input name=start maxlength=4 size=4>\n</form>\n");
}
