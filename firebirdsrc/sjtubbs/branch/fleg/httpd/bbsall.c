#include "www.h"

static int cmpboard(b1, b2)
struct boardheader *b1, *b2;
{
    return strcasecmp(b1->filename, b2->filename);
}

int bbsall_main()
{
    struct shortfile data[MAXBOARD], *x;
    int i, total = 0;
    char *ptr;

    init_all();
    printf("<style type=text/css>A {color: #0000f0}</style>");
    for (i = 0; i < MAXBOARD; i++)
    {
        x = &(shm_bcache->bcache[i]);
        if (x->filename[0] <= 32 || x->filename[0] > 'z')
            continue;
        if (!has_read_perm(&currentuser, x->filename))
            continue;
        memcpy(&data[total], x, sizeof(struct shortfile));
        total++;
    }
    qsort(data, total, sizeof(struct shortfile), cmpboard);
    printf("<center>\n");
    printf("%s -- 全部讨论区 [讨论区数: %d]<hr color=green>", BBSNAME,
           total);
    printf("<table width=610>\n");
    printf("<tr><td>序号<td>讨论区名称<td>类别<td>中文描述<td>板主\n");
    for (i = 0; i < total; i++)
    {
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=bbsdoc?board=%s>%s</a>", data[i].filename,
               data[i].filename);
        printf("<td>%6.6s", data[i].title + 1);
        printf("<td><a href=bbsdoc?board=%s>%s</a>", data[i].filename,
               data[i].title + 7);
        ptr = strtok(data[i].BM, " ,;");
        printf("<td>%s</a>", ptr == 0 ? "诚征板主中" : userid_str(ptr));
    }
    printf("</table></center><br><br>\n");
    http_quit();
}
