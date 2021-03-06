#include "www.h"

static int no_re = 0;

int bbstopcon_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    /*char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];*/
    struct fileheader x;
    int num, total;
    int swap;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    //u_info->board=getbnum(board);
    printf("<font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]<hr>", BBSNAME, board);
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数2");
    sprintf(dir, "boards/%s/.TOPFILE", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("此讨论区不存在或者为空");
    //{add by alt@sjtu for 屏蔽回收站文章
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    printf
    ("<table class=show_border width=610>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    if (strcmp(x.filename, file) != 0)
        http_fatal("本文不存在或已被删除,或者己被删除在回收站中");
    fclose(fp);
    //}
    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("本文不存在或者已被删除");
    while (1)
    {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "发信人: ", 8))
        {
            ptr = strdup(buf);
            id = strtok(ptr + 8, " ");
            s = strtok(0, "");
            if (id == 0)
                id = " ";
            if (s == 0)
                s = "\n";
            if (strlen(id) < 13 && getuser(id))
            {
                printf("发信人: %s", userid_str(id));
                hprintf("%s", s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, "--", 2))
            pic_mod = 1;
        if (!strncmp(buf, ": ", 2))
        {
            printf("<font color=808080>");
            swap = pic_mod;
            pic_mod = 0;
        }
        hhprintf("%s", void1(buf));
        if (!strncmp(buf, ": ", 2))
        {
            printf("</font>");
            pic_mod = swap;
        }
    }
    fclose(fp);
    printf("</pre>\n</table><hr>\n");
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    http_quit();
}
