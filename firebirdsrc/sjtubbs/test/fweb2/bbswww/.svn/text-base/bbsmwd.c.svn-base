#include "www.h"

static int do_fwd(struct fileheader *x, char *target)
{
    char title[512], path[200];

    sprintf(path, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,
            x->filename);
    if (!file_exist(path))
        http_quit_print("文件内容已丢失, 无法转寄");
    sprintf(title, "[转寄] %s", x->title);
    title[60] = 0;
    post_mail(target, title, path, currentuser.userid,
              currentuser.username, fromhost, -1);
    printf("文章已转寄给'%s'<br>\n", nohtml(target));
    printf("[<a href='javascript:history.go(-2)'>返回</a>]");
    return 0;
}

int bbsmwd_main()
{
    struct fileheader *x;
    char board[80], file[80], target[80];

    init_all();

    strsncpy(file, getparm("file"), 30);
    strsncpy(target, getparm("target"), 30);
    if (!loginok)
        http_quit_print("匆匆过客不能进行本项操作");
    if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_CHAT))
        http_quit_print("您被封禁了发信权，不能完成本次操作");

    x = get_file_ent(board, file, 0);
    if (x == 0)
        http_quit_print("错误的文件名");
    printf
        ("<center><font class=\"title\">%s - 转寄/推荐给好友　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    if (target[0])
    {
        if (!strstr(target, "@"))
        {
            if (!getuser(target))
                http_quit_print("错误的使用者帐号");
            strcpy(target, getuser(target)->userid);
        }
        return do_fwd(x, target);
    }
    printf("<table><tr><td>\n");
    printf("信件标题: %s<br>\n", nohtml(x->title));
    printf("信件作者: %s<br>\n", x->owner);

    printf("<form action=bbsmwd method=post>\n");

    printf("<input type=hidden name=file value=%s>", file);
    printf
        ("把文章转寄给 <input name=target size=30 maxlength=30 value=%s> (请输入对方的id或email地址). <br>\n",
         currentuser.email);
    printf("<input type=submit value=确定转寄></form>");
    printf("</html>");
    return 0;
}
