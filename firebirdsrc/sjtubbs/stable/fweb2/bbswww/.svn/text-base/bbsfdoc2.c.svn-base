#include "www.h"

int bbsfdoc2_main()
{
    FILE *fp;
    char *ptr, board[30], dirbuf[256];
    int dirsize, n_read, i, start;
    struct dir x[20];

    cache_age = 60 * 60;
    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    ptr = getparm("start");
    sprintf(dirbuf, UPLOAD_PATH2 "/%s", board);
    dirsize = file_size(dirbuf) / sizeof(struct dir);
    if (0 == dirsize)
    {
        char buf[256];

        snprintf(buf, 256,
                 "上载区为空，<a href=bbsupload?board=%s>上载文件</a>",
                 board);
        http_quit_print(buf);
    }

    if (ptr[0] == 0)
    {
        start = dirsize - 20;
    }
    else
    {
        start = atoi(ptr);
        if (start >= dirsize)
            start = dirsize - 20;
    }
    if (start < 0)
        start = 0;

    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_quit_print("没有找到文件");
    fseek(fp, start * sizeof(struct dir), SEEK_SET);
    n_read = fread(x, sizeof(struct dir), 20, fp);
    fclose(fp);

    printf
        ("<center><font class=\"title\">%s - 板内软件下载　　</font>[讨论区: %s]<hr>\n",
         BBSNAME, board);
    printf
        ("<table width=800 border=1><tr><td>序号<td>文件名<td>上载者<td>时间<td>大小<td>说明<td>备注<td>管理\n");
    for (i = 0; i < n_read; i++)
    {
        printf("<tr><td>%d", start + 1 + i);
        printf("<td><a href=" UPLOAD_FILE_URL_HEADER
               "/%s/%s", x[i].board, x[i].showname);
        printf(" target=_blank>%s </a>", x[i].showname);
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td><a href=bbsfexp2?fid=%d&board=%s", start + i,
               x[i].board);
        printf(">more </a>\n");
        printf("<td width='33%%'>%s ", nohtml(x[i].exp));
        printf
            ("<td><a onclick='return confirm(\"确定?\")' href=bbsudel2?fid=%d&file=%s&board=%s",
             start + i, x[i].showname, x[i].board);
        printf(">删除</a>\n");
    }
    printf("</table><br>\n");
    printf
        ("[<a href=bbsfdoc2?board=%s&start=%d>刷新</a>] [<a href=bbsdoc?board=%s>本讨论区</a>] [<a href=bbsupload?board=%s>上载文件</a>]",
         board, start, board, board);
    printf("[<a href=bbsfdoc2?board=%s&start=0>首页</a>] ", board);
    if (0 != start)
        printf("[<a href=bbsfdoc2?board=%s&start=%d>上一页</a>] ",
               board, ((start - 20) < 0) ? 0 : (start - 20));
    if (start + 20 < dirsize)
        printf("[<a href=bbsfdoc2?board=%s&start=%d>下一页</a>] ",
               board, start + 20);
    printf("[<a href=bbsfdoc2?board=%s>末页</a>] ", board);
    printf("[<a href=bbsfddoc?board=%s>上载区管理</a>] ", board);
    printf
        ("<form name=form1 action=bbsfdoc2 method=post onsubmit='this.start.value-=1' >\n");
    printf("<input type=hidden name=board value=%s>\n", board);
    printf
        ("<input type=submit value=跳转到> 第 <input type=text name=start size=4 onkeydown=\"return true;\" >条 ");
    printf("</form>\n");
    return 0;
}
