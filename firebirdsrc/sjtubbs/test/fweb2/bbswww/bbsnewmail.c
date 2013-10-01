#include "www.h"

int bbsnewmail_main()
{
    FILE *fp;
    struct fileheader x;
    int total = 0, total2 = 0;
    char *ptr, dir[80];

    init_all();
    if (!loginok)
        http_quit_print("您尚未登录, 请先登录");
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - 新邮件列表　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("目前您的信箱没有任何信件");
    printf("<table width=610>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>序号<td>状态<td>发信人<td>日期<td>信件标题\n");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        total++;
        if (x.accessed[0] & FILE_READ)
            continue;
        printf("<tr><td>%d<td>N", total);
        ptr = strtok(x.owner, " (");
        if (ptr == 0)
            ptr = " ";
        ptr = nohtml(ptr);
        printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
        printf("<td>%6.6s", Ctime(atoi(x.filename + 2)) + 4);
        printf("<td><a href=bbsmailcon?file=%s&num=%d>", x.filename,
               total - 1);
        if (strncmp("Re: ", x.title, 4))
            printf("★ ");
        hprintf("%42.42s", void1(x.title));
        printf(" </a>\n");
        total2++;
    }
    fclose(fp);
    printf("</table><hr>\n");
    printf("您的信箱共有%d封信件, 其中新信%d封.", total, total2);
    printf("</html>");
    return 0;
}
