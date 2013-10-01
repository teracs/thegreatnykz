#include "www.h"

int bbsfexp_main()
{
    FILE *fp;
    struct dir x;
    char name[256], url[256];

    init_all();
    strsncpy(name, getparm("name"), 40);
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_fatal("文件不存在1");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) == 0)
            break;
        if (!strcmp(x.showname, name))
        {
            printf("<center>%s -- 文件资料<hr color=green>\n", BBSNAME);
            printf("<table border=1 width=600>\n");
            printf("<tr><td>个人推荐等级<td>%d星\n", x.level + 1);
            printf("<tr><td>希望保留时间<td>%d天\n", x.live);
            printf("<tr><td>文件大小<td>%d字节\n", x.size);
            printf("<tr><td>文件类型<td>%s\n", x.type);
            printf("<tr><td>文件名称<td>%s\n", x.showname);
            printf("<tr><td>上载人<td>%s\n", userid_str(x.userid));
            printf("<tr><td>上载时间<td>%s\n", Ctime(x.date));
            printf("<tr><td>上载板面<td><a href=bbsdoc?board=%s>%s</a>\n",
                   x.board, x.board);
            printf("<tr><td>下载次数<td>%d\n", x.click);
            printf("<tr><td>文件说明<td>%s\n", nohtml(x.exp));
            sprintf(url, "http://%s/showfile?name=%s", BBSHOST,
                    nohtml(x.showname));
            printf
            ("<tr><td>URL地址<td><a href='%s' target=_blank>%s </a>\n",
             url, url);
            printf("</table><br>\n");
            printf
            ("文件资料的URL地址可在本站文章中直接引用，系统会自动展开。");
            http_quit();
        }
    }
    http_fatal("文件不存在");
}
