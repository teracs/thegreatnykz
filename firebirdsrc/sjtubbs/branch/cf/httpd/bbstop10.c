#include "www.h"

int bbstop10_main()
{
    FILE *fp;
    int n;
    char s1[256];
    char brd[256], id[256], title[256], num[100];

	last_modified_time = file_time("etc/posts/day");
	cache_age = 15*60;
    init_all();
    /*	printf("<center>\n");
    	printf("<hr color=green>\n");
    	fp=fopen("etc/top10.txt", "r");
    	*/
    printf("<center>%s -- 今日十大热门话题\n<hr>\n", BBSNAME);
    fp = fopen("etc/posts/day", "r");
    if (fp == 0)
        http_fatal("can't read data");
    fgets(s1, 255, fp);
    fgets(s1, 255, fp);
    printf("<table width=640>\n");
    //      printf("<tr bgcolor='#f0f0f0'><td>名次<td>讨论区<td>标题<td>作者<td>跟贴人数\n");
    printf
    ("<tr bgcolor='#f0f0f0'><td>名次<td>讨论区<td>标题<td>作者<td>次数\n");
    for (n = 1; n <= 10; n++)
    {
        if (fgets(s1, 255, fp) <= 0)
            break;
        sscanf(s1 + 41, "%s", brd);
        sscanf(s1 + 118, "%s", id);
        sscanf(s1 + 97, "%s", num);
        if (fgets(s1, 255, fp) <= 0)
            break;
        strsncpy(title, s1 + 27, 60);
        printf("<tr bgcolor=%s>", n % 2 == 0 ? "#f0f0f0" : "#FFFFFF");
        printf
        ("<td>第 %d 名<td><a href=bbsdoc?board=%s>%s</a><td><a href='bbstfind?board=%s&title=%s '>%s</a><td><a href=bbsqry?userid=%s>%12s</a><td>%s\n",
         n, brd, brd, brd, nohtml3(title), void1(nohtml4(title)), id,
         id, num);
    }
    printf("</table><center>");
    http_quit();
}
