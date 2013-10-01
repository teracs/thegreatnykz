#include "www.h"
static int save_sig(char *path)
{
    char *buf;
    FILE *fp;

    fp = fopen(path, "w");
    buf = getparm("text");
    fprintf(fp, "%s", buf);
    fclose(fp);
    printf("签名档修改成功。");
    printf("</html>");
    return 0;
}

int bbssig_main()
{
    FILE *fp;
    char *ptr, path[256], buf[10000];

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客不能设置签名档，请先登录");
    printf
        ("<center><font class=\"title\">%s - 设置签名档　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    sprintf(path, "home/%c/%c/%s/signatures",
            toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
            currentuser.userid);
    if (!strcasecmp(getparm("type"), "1"))
        return save_sig(path);
    printf("<form method=post action=bbssig>\n");
    printf("<input type=hidden name=type value=1>\n");
    fp = fopen(path, "r");
    bzero(buf, 10000);
    if (fp)
    {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("签名档每6行为一个单位, 可设置多个签名档.<table><tr><td>");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
    printf("%s", void1(buf));
    printf("</textarea></table><br><hr>\n");
    printf("<input type=submit value=存盘> ");
    printf("<input type=reset value=复原>\n");
    printf("</form>\n");
    printf("</html>");
    return 0;
}
