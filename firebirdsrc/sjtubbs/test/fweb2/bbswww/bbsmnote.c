#include "www.h"

static int save_note2(char *path)
{
    FILE *fp;
    char buf[10000];

    fp = fopen(path, "w");
    strsncpy(buf, getparm("text"), 9999);
    fprintf(fp, "%s", buf);
    fclose(fp);
    printf("进板画面修改成功。<br>\n");
    printf("<a href='javascript:history.go(-2)'>返回</a>");
    printf("</html>");
    return 0;
}

int bbsmnote_main()
{
    FILE *fp;
    char *ptr, path[256], buf[10000], board[256];

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_quit_print("匆匆过客，请先登录");
    strsncpy(board, getparm("board"), 30);
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("你无权进行本操作");
    strsncpy(board, getbcache(board)->filename, 30);
    sprintf(path, "vote/%s/notes", board);
    if (!strcasecmp(getparm("type"), "update"))
        return save_note2(path);
    printf
        ("<font class=\"title\">%s - 编辑进板画面　　</font>[讨论区: %s]<hr>\n",
         BBSNAME, board);
    printf("<form method=post action=bbsmnote>\n");
    fp = fopen(path, "r");
    buf[0] = 0;
    if (fp)
    {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("<table width=610 border=1><tr><td>");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
    printf("%s", void1(buf));
    printf("</textarea></table>\n");
    printf("<input type=submit value=存盘> ");
    printf("<input type=reset value=复原>\n");
    printf("<hr>\n");
    printf("</html>");
    return 0;
}
