#include "www.h"

int bbsnot_main()
{
    FILE *fp;
    char buf[512], board[80], filename[80];

    cache_age = 10 * 60;
    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的板面");
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - 备忘录　　</font>[讨论区: %s]<hr>\n",
         BBSNAME, board);
    sprintf(filename, "vote/%s/notes", board);
    fp = fopen(filename, "r");
    if (fp == 0)
    {
        printf("<br>本讨论区尚无「进板画面」。\n");
        http_quit();
    }
    printf
        ("<table class=show_border width=610><tr><td><pre class=tight>\n");
    while (1)
    {
        char *s;

        bzero(buf, 512);
        if (fgets(buf, 512, fp) == 0)
            break;
        while (1)
        {
            unsigned int i;

            s = strstr(buf, "$userid");
            if (s == 0)
                break;
            for (i = 0; i < 7; i++)
                s[i] = 32;
            for (i = 0; i < strlen(currentuser.userid); i++)
                s[i] = currentuser.userid[i];
        }
        hhprintf("%s", buf);
    }
    fclose(fp);
    printf("</pre></table><hr>\n");
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>] ", board);
    if (has_BM_perm(&currentuser, board))
        printf("[<a href=bbsmnote?board=%s>编辑进板画面</a>]", board);
    printf("</center>\n");
    printf("</html>");
    return 0;
}
