#include "www.h"

int bbssec_main()
{
    int i;

    cache_age = 60 * 60 * 24;
    init_all();
    printf("<body topmargin=0 leftmargin=0>");
    printf
        ("<style type=text/css>A {font-size:12x; color: #0000f0}</style>");
    printf("<center><br>\n");
    printf("<font class=\"title\">%s - 分类讨论区列表</font><hr>\n",
           BBSNAME);
    printf("<table>\n");
    printf("<tr bgcolor='e8e8e8'><td>区号<td>类别<td>描述\n");
    for (i = 0; i < SECNUM; i++)
    {
#ifdef STATIC_URL
        printf("<tr><td>%c<td><a href=bbsboa,sec,%d.html>%s</a>",
               i < 10 ? i + '0' : i + 'A' - 10, i, secname[i][0]);
        printf("<td><a href=bbsboa,sec,%d.html>%s</a>\n", i,
               secname[i][1]);
#else
        printf("<tr><td>%c<td><a href=bbsboa?sec=%d>%s</a>",
               i < 10 ? i + '0' : i + 'A' - 10, i, secname[i][0]);
        printf("<td><a href=bbsboa?sec=%d>%s</a>\n", i, secname[i][1]);
#endif

    }
    printf("</table>\n");
    printf("<hr>\n");
    printf("[<a href=bbsall>全部讨论区</a>]");
    printf("[<a href=bbs0an>精华公布栏</a>]");
    printf("[<a href=bbsusr>使用者列表</a>]");
    printf("</center></td>\n");
    printf("</center>\n");
    printf("</html>");
    return 0;
}
