#include "www.h"

int bbsanc_main()
{
    FILE *fp;
    char *board, path[512], buf[512];
    char prepage[512];
    char *temp;
    char *pre;
    int i;

    cache_age = 60 * 60;
    init_all();
    strsncpy(path, getparm("path"), 511);
    for (i = 0; i < (int) strlen(path) - 1; i++)
        if (path[i] == '/' && path[i + 1] == '/')
            http_quit_print("错误的文件名");
    board = getbfroma(path);
    buf[0] = 0;
    if (board[0] && getbcache(board)
        && !has_read_perm(&currentuser, board))
        http_quit_print("错误的文件名");
    if (board[0])
        sprintf(buf, "%s板", board);
    printf
        ("<center><font class=\"title\">%s - %s精华区文章阅读</font><hr>\n",
         BBSNAME, board);
    if (strstr(path, ".Search") || strstr(path, ".Names")
        || strstr(path, "..") || strstr(path, "./"))
        http_quit_print("错误的文件名");
    sprintf(buf, "0Announce%s", path);
    printf("<table class=\"show_border\" width=610>");

    printf("<tr><td><pre>");
    fp = fopen(buf, "r");
    if (fp == 0)
        http_quit_print("错误的文件名");
    while (1)
    {
        if (fgets(buf, 256, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre></table><hr>\n");
    if (board[0])
    {
        strcpy(prepage, url_quote(path));
        pre = prepage; temp = pre + 1;
        while(temp = strstr(temp, "%2F"))
        {
            pre = temp; 
            temp++;
        }    
        buf[pre-prepage] = 0;
        strncpy(buf, prepage, pre-prepage);
#ifdef STATIC_URL
        printf("[<a href=bbs0an,path,%s.html>返回上一页</a>]", buf);
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an,path,%s.html>%s文集首页</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc,board,%s.html>本讨论区</a>]", board);
#else
        printf("[<a href=bbs0an?path=%s>返回上一页</a>]", buf);
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an?path=%s>%s文集首页</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
#endif
    }
    printf("</center>\n");
    printf("\n</html>");
    return 0;
}
