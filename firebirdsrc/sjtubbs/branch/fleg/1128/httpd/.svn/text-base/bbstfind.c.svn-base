#include "www.h"

int bbstfind_main()
{
    FILE *fp;
    char buf[1024], title[80], board[80], dir[80], first_file[80] = "";
    struct shortfile *x1;
    struct fileheader x;
    int sum = 0;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(title, getparm("title"), 42);
    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("错误的讨论区");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    sprintf(buf, "bbsman?board=%s&mode=1", board);
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("错误的讨论区目录");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) == 0)
            break;
        sum++;
        if (!strncmp(title, rtrim(x.title), 40) ||
                (!strncmp(title, rtrim(x.title + 4), 40)
                 && !strncmp(x.title, "Re: ", 4)))
        {
            strcpy(first_file, x.filename);
            break;
        }
    }
    fclose(fp);
    if (first_file[0])
    {
        char buf[256];

        snprintf(buf, 255, "bbstcon?board=%s&file=%s", board, first_file);
        redirect(buf);
    }
    else
    {
        printf("主题错误或已删除");
    }
    http_quit();
}
