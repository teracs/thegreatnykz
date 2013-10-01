#include "www.h"

int bbsfind_main()
{
    char user[32], title3[80], title[80], title2[80];
    int day, day2, flag;

    init_all();
    if (get_load(0) > 20.)
        http_fatal("抱歉，因系统负荷过高目前无法使用本功能，请稍后再试");
    strsncpy(user, getparm("user"), 13);
    strsncpy(title, getparm("title"), 50);
    strsncpy(title2, getparm("title2"), 50);
    strsncpy(title3, getparm("title3"), 50);
    day = atoi(getparm("day"));
    day2 = atoi(getparm("day2"));
    flag = atoi(getparm("flag"));
    if (!flag)
    {
        printf("<center>\n");
        printf("<hr>\n");
        printf("<table>\n");
        printf("<form action=bbsfind method=post>\n");
        printf("<input type=hidden name=flag value=1>\n");
        printf
        ("<tr><td>文章作者: <td><input maxlength=12 size=12 type=text name=user> (不填查找所有作者)<br>\n");
        printf
        ("<tr><td>标题含有: <td><input maxlength=60 size=20 type=text name=title>");
        printf
        (" AND <input maxlength=60 size=20 type=text name=title2><br>\n");
        printf
        ("<tr><td>标题不含: <td><input maxlength=60 size=20 type=text name=title3><br>\n");
        printf
        ("<tr><td>查找据今: <td><input maxlength=5 size=5 type=text name=day value=0> 至\n");
        printf
        ("<input maxlength=5 size=5 type=text name=day2 value=7> 天以内的文章\n");
        printf("</table><hr>\n");
        printf("<input type=submit value=提交查询>\n");
    }
    else
    {
        if (day > 9999)
            day = 9999;
        if (day2 > 9999)
            day2 = 9999;
        search(user, title, title2, title3, day * 86400, day2 * 86400);
    }
    http_quit();
}

int search(char *id, char *pat, char *pat2, char *pat3, int dt, int dt2)
{
    FILE *fp;
    char board[256], dir[256];
    int total, now = time(0), i, sum = 0, n, t;
    struct fileheader x;

    printf("<center><font class=\"title\">%s - 站内文章查询结果</font></center> <br>\n", BBSNAME);
    printf("作者: %s ", id);
    printf("标题含有: '%s' ", nohtml(pat));
    if (pat2[0])
        printf("和 '%s' ", nohtml(pat2));
    if (pat3[0])
        printf("不含 '%s'", nohtml(pat3));
    printf("时间: 据今 %d-%d 天<br><hr>\n", dt / 86400,
           dt2 / 86400);
    for (i = 0; i < MAXBOARD; i++)
    {
        total = 0;
        strcpy(board, shm_bcache->bcache[i].filename);
        if (!has_read_perm(&currentuser, board))
            continue;
        sprintf(dir, "boards/%s/.DIR", board);
        fp = fopen(dir, "r");
        if (fp == 0)
            continue;
        n = 0;
        printf
		("<table class=show_border width=610>\n");
        while (1)
        {
            n++;
            if (fread(&x, sizeof(x), 1, fp) <= 0)
                break;
            t = atoi(x.filename + 2);
            if (id[0] != 0 && strcasecmp(x.owner, id))
                continue;
            if (pat[0] && !strcasestr(x.title, pat))
                continue;
            if (abs(now - t) < dt)
                continue;
            if (abs(now - t) > dt2)
                continue;
            if (pat2[0] && !strcasestr(x.title, pat2))
                continue;
            if (pat3[0] && strcasestr(x.title, pat3))
                continue;
            printf("<tr><td>%d<td><a href=bbsqry?userid=%s>%s</a>",
                   n, x.owner, x.owner);
            printf("<td>%6.6s", Ctime(atoi(x.filename + 2)) + 4);
            printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%s</a>\n",
                   board, x.filename, n - 1, nohtml(x.title));
            total++;
            sum++;
            if (sum > 1999)
            {
                printf("</table> ....");
                http_quit();
            }
        }
        printf("</table>\n");
        if (total == 0)
            continue;
        printf("<br>以上%d篇来自 <a href=bbsdoc?board=%s>%s</a><br><br>\n",
               total, board, board);
    }
    printf("一共找到%d篇文章符合查找条件<br>\n", sum);
}
