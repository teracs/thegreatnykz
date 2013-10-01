#include "www.h"

int bbstopb10_main()
{
    FILE *fp;
    struct shortfile *b;
    char buf[256], tmp[256], name[256], cname[256], cc[256];
    int i, r;

    last_modified_time = file_time("0Announce/bbslist/board2");
    cache_age = 60 * 60 * 24;
    init_all();
    printf("<body topmargin=0 leftmargin=0>");
    fp = fopen("0Announce/bbslist/board2", "r");
    if (fp == 0)
        http_quit_print("error 1");
    printf
        ("<br><center><font color=red style='font-size:18px'><b>%s - 热门讨论区(TOP20)</b></font><hr><br>\n",
         BBSNAME);
    printf("<table>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>名次<td>板名<td>中文板名<td>板主<td>人气\n");
    for (i = 0; i <= 20; i++)
    {
        char *ptr, BM[80] = "";

        if (fgets(buf, 150, fp) == 0)
            break;
        if (i == 0)
            continue;
        r = sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp,
                   cname, cc);
        if (r == 6)
        {
            b = getbcache(name);
            if (b)
                strcpy(BM, b->BM);
            ptr = strtok(BM, " ,;");
            if (ptr == 0)
                ptr = "诚征板主中";
            printf
                ("<tr><td>%d<td><a href=bbsdoc?board=%s>%s</a><td width=200><a href=bbsdoc?board=%s>%s</a><td>%s<td>%s\n",
                 i, name, name, name, cname, userid_str(ptr), cc);
        }
    }
    printf("</table><hr>\n");
    printf("<a href=bbsanc?path=/bbslist/board2>全部板面排序</a>\n");
    printf("</center>\n");
    fclose(fp);
    printf("</html>");
    return 0;
}
