#include "www.h"

int bbsnotepad_main()
{
    FILE *fp;
    char buf[256];

    init_all();
    printf("%s -- 留言板 [日期: %6.6s]<hr color=green><pre>\n", BBSNAME,
           Ctime(time(0)) + 4);
    fp = fopen("etc/notepad", "r");
    if (fp == 0)
    {
        printf("今天的留言板为空");
        http_quit();
    }
    while (1)
    {
        if (fgets(buf, 255, fp) == 0)
            break;
        hprintf("%s", buf);
    }
    fclose(fp);
    http_quit();
}
