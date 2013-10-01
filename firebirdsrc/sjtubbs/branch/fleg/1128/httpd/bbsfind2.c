#include "www.h"

int bbsfind2_main()
{
    FILE *fp;
    char board[256], match[256];
    struct dir x[20000], xx;
    int i, start, total = 0;

    init_all();
    if (!currentuser.userlevel & PERM_BASIC)
        http_fatal("error");
    strsncpy(match, getparm("match"), 30);
    start = atoi(getparm("start"));
    if (match[0] == 0 || match[0] == '.')
    {
        printf("<form action=bbsfind2>\n");
        printf("匹配字符串: <input name=match>\n");
        printf("<input type=submit value='确定'>\n");
        printf("</form>\n");
        http_quit();
    }
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_fatal("没有找到文件");
    while (total < 20000)
    {
        if (fread(&xx, sizeof(xx), 1, fp) <= 0)
            break;
        if (!xx.active)
            continue;
        if (!strcasestr(xx.showname, match))
            continue;
        x[total] = xx;
        total++;
    }
    printf("<center><font class=\"title\">%s - 全站上载文件查询</font><hr>\n", BBSNAME);
    printf
    ("<table width=720><tr bgcolor=e8e8e8><td>序号<td>文件名<td>上载者<td>时间<td>大小<td>下载次数<td>说明<td>管理<td>备注\n");
    for (i = start; i < total; i++)
    {
        if (i - start >= 1000)
            break;
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=/showfile?name=%s target=_blank>%s</a>",
               nohtml(x[i].showname), nohtml(x[i].showname));
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td> ");        // %d", x[i].click);
        printf("<td>%s", x[i].exp);
        printf
        ("<td><a onclick='return confirm(\"确定删除吗?\")' href=bbsudel?file=%s>删</a>\n",
         x[i].showname);
        printf("<td><a href=bbsfexp?name=%s>其他属性 </a>\n",
               nohtml(x[i].showname));
    }
    printf("</table><br>\n");
    if (total - start > 1000)
        printf("too many files<br>\n");
    printf("[<a href=bbsfdoc?board=%s>刷新</a>]\n", board, board, board);
}
