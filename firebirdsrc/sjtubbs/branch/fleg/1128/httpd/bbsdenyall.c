#include "www.h"
static struct deny
{
    char id[80];
    char exp[80];
    char free_time[30];
}
denyuser[256];
static int denynum = 0;

static int loaddenyuser(char *board)
{
    FILE *fp;
    char path[80], buf[256], tmp[50];

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "r");
    if (fp == 0)
        return;
    while (denynum < 100)
    {
        if (fgets(buf, 80, fp) == 0)
            break;
        sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
               denyuser[denynum].exp, tmp, denyuser[denynum].free_time);
        //      sscanf(buf, "%s %s %d", denyuser[denynum].id, denyuser[denynum].exp, &denyuser[denynum].free_time);
        denynum++;
    }
    fclose(fp);
}

int bbsdenyall_main()
{
    int i;
    char board[80];

    init_all();
    if (!loginok)
        http_fatal("您尚未登录, 请先登录");
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    if (!has_BM_perm(&currentuser, board))
        http_fatal("你无权进行本操作");
    loaddenyuser(board);
    printf("<center>\n");
    printf("<font class=\"title\">%s - 被封用户名单　　</font>[讨论区: %s]<hr><br>\n",
           BBSNAME, board);
    printf("本板共有 %d 人被封<br>", denynum);
    printf
    ("<table border=1><tr><td>序号<td>用户帐号<td>被封原因<td>原定解封日期<td>管理\n");
    for (i = 0; i < denynum; i++)
    {
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=bbsqry?userid=%s>%s</a>", denyuser[i].id,
               denyuser[i].id);
        printf("<td>%s\n", nohtml(denyuser[i].exp));
        printf("<td>%s\n", nohtml(denyuser[i].free_time));
        //printf("<td>%s\n", Ctime(denyuser[i].free_time)+4);
        printf
        ("<td>[<a onclick='return confirm(\"确实解封吗?\")' href=bbsdenydel?board=%s&userid=%s>解封</a>]",
         board, denyuser[i].id);
    }
    printf("</table><hr>\n");
    printf
    ("[<a href=bbsdenyadd?board=%s>设定新的不可POST用户</a>]</center>\n",
     board);
    http_quit();
}
