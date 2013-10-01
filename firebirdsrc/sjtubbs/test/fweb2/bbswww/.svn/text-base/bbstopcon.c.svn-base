#include "www.h"

int bbstopcon_main()
{
    FILE *fp;
    char buf[512], board[80], file[80], filename[80], *ptr;

    int swap = -1;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("错误的讨论区");
    }
    else
    {
        if (is_normal_board)
            cache_age = 30 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    printf("<body>");
    printf("<center>\n");
    strcpy(board, getbcache(board)->filename);

    printf
        ("<font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]<hr>",
         BBSNAME, board);
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");

    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    printf("<table class=show_border width=610>\n");

    printf("<tr><td>\n<pre>");

    sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_quit_print("本文不存在或者已被删除");
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
    printf("</html>");
    return 0;
}
