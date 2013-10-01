/*
#include "www.h"

int top10_main()
{
    FILE *fp;
    int n;
    char s1[256];
    char brd[256], id[256], title[256], num[100];

    init_no_http();
    cgi_head();
    fp = fopen("etc/top10.txt", "r");
    if (fp == 0)
        http_fatal("can't read data");
    fgets(s1, 255, fp);
    fgets(s1, 255, fp);
    for (n = 1; n <= 10; n++)
    {
        if (fgets(s1, 255, fp) <= 0)
            break;
        sscanf(s1 + 45, "%s", brd);
        sscanf(s1 + 122, "%s", id);
        sscanf(s1 + 102, "%s", num);
        if (fgets(s1, 255, fp) <= 0)
            break;
        strsncpy(title, s1 + 27, 60);
        printf("%s %s %s %s\n", brd, id, num, trim(title));
    }
}
*/
