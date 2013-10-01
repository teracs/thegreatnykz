#include "www.h"

int save_keyword(char *path)
{
    char buf[10000];
    FILE *fp;

    fp = fopen(path, "w");
    strsncpy(buf, getparm("text"), 9999);
    buf[80] = 0;
    fprintf(fp, "%s", buf);
    fclose(fp);
    printf("关键字修改成功。<br>\n");
    printf("<a href='javascript:history.go(-2)'>返回</a>");
    printf("</html>");
    return 0;
}

int bbsmnote4_main()
{
    FILE *fp;
    char path[256], buf[10000], board[256];

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_quit_print("匆匆过客，请先登录");
    strsncpy(board, getparm("board"), 30);
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("你无权进行本操作");
    strsncpy(board, getbcache(board)->filename, 30);
    sprintf(path, "vote/%s/keyword", board);
    if (!strcasecmp(getparm("type"), "update"))
        return save_keyword(path);
    printf
        ("<font class=\"title\">%s - 编辑关键字　　</font>[讨论区: %s]<hr>\n",
         BBSNAME, board);
    printf("<form method=post action=bbsmnote4>\n");
    printf("<input type=hidden name=board value=\"%s\"/>\n", board);
    printf("<input type=hidden name=type value=\"update\"/>\n");
    fp = fopen(path, "r");
    buf[0] = 0;
    if (fp)
    {
        fgets(buf, 256, fp);
        fclose(fp);
    }
    printf
        ("请输入关键字，用半角空格隔开: <input type=text name=text maxlength=80 size=80 value=\"%s\"/> \n",
         void1(nohtml(buf)));
    printf("<input type=\"submit\" value=\"存盘\"/> ");
    printf("<input type=\"reset\" value=\"复原\"/>\n");
    printf("<hr>\n");
    printf("</html>");
    return 0;
}
