#include "www.h"
char *stat1();

int bbstdoc_main()
{
    FILE *fp;
    char board[80], dir[80];
    struct shortfile *x1;
    struct fileheader *data;
    int i, start, total2 = 0, total, sum = 0;

    init_all();
    strsncpy(board, getparm("board"), 32);
    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("错误的讨论区");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    printf("<body>\n");
    //u_info->board=getbnum(board);
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("错误的讨论区目录");
    total = file_size(dir) / sizeof(struct fileheader);
    data = calloc(sizeof(struct fileheader), total);
    if (data == 0)
        http_fatal("内存溢出");
    total = fread(data, sizeof(struct fileheader), total, fp);
    fclose(fp);
    for (i = 0; i < total; i++)
        if (strncmp(data[i].title, "Re:", 3))
            total2++;
    start = atoi(getparm("start"));
    if (strlen(getparm("start")) == 0 || start > total2 - 19)
        start = total2 - 19;
    if (start < 0)
        start = 0;
    printf("<nobr><center>\n");
    printf
    ("%s -<font class=\"title\"> 主题阅读　　</font>[讨论区: %s] 板主[%s] 文章%d, 主题%d个<hr>\n",
     BBSNAME, board, userid_str(x1->BM), total, total2);
    if (total <= 0)
        http_fatal("本讨论区目前没有文章");
    printf("<table width=670>\n");
    //printf("<tr bgcolor=e8e8e8><td>序号<td>状态<td>作者<td width=72>日期<td>标题<td>回帖/人气\n");
    printf
    ("<tr bgcolor=e8e8e8><td>序号<td>状态<td>作者<td width=100>日期<td>标题<td>回帖/人气\n");
    for (i = 0; i < total; i++)
    {
        if (!strncmp(data[i].title, "Re:", 3))
            continue;
        if (data[i].title[0] == 0)
            continue;
        sum++;
        if (sum - 1 < start)
            continue;
        if (sum - 1 > start + 19)
            break;
        printf("<tr><td>%d<td>%s<td>%s",
               sum, flag_str(data[i].accessed[0]),
               userid_str(data[i].owner));
        printf("<td>%12.12s", Ctime(atoi(data[i].filename + 2)) + 4);
        printf("<td><a href=bbstcon?board=%s&file=%s>○ %s </a><td>%s",
               board, data[i].filename,
               void1(nohtml2(data[i].title)), stat1(data, i, total));
    }
    printf("</table><hr>\n");
    if (start > 0)
        printf("<a href=bbstdoc?board=%s&start=%d>上一页</a> ", board,
               start - 19);
    if (start < total2 - 19)
        printf("<a href=bbstdoc?board=%s&start=%d>下一页</a> ", board,
               start + 19);
    printf("<a href=bbsnot?board=%s>进板画面</a> ", board);
    printf("<a href=bbsdoc?board=%s>一般模式</a> ", board);
    printf("<a href=bbsgdoc?board=%s>文摘区</a> ", board);
    printf("<a href=bbs0an?path=%s>精华区</a> ", anno_path_of(board));
    //      printf("<a href=/an/%s.tar.gz>下载精华区</a> ", board);
    printf("<a href=bbspst?board=%s>发表文章</a> <br>\n", board);
    free(data);
    printf
    ("<form action=bbstdoc?board=%s method=post onsubmit='this.start.value-=1'>\n",
     board);
    printf
    ("<input type=submit value=跳转到> 第 <input type=text name=start size=4> 篇");
    printf("</form>\n");
    http_quit();
}

char *stat1(struct fileheader *data, int from, int total)
{
    static char buf[256];
    char *ptr = data[from].title;
    int i, re = 0, click = *(int *) (data[from].title + 73);

    for (i = from; i < total; i++)
    {
        if (!strncmp(ptr, data[i].title + 4, 40))
        {
            re++;
            data[i].title[0] = 0;
            click += *(int *) (data[i].title + 73);
        }
    }
    sprintf(buf, "<font color=%s>%d</font>/<font color=%s>%d</font>",
            re > 9 ? "red" : "black", re, click > 499 ? "red" : "black",
            click);
    return buf;
}
