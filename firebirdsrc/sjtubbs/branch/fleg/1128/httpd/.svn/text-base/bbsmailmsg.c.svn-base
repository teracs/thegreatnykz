#include "www.h"

//modified by rwzmm@sjtubbs
//delete .msgdir while delete .msgfile.me


int bbsmailmsg_main()
{
    char filename[80];
    char msgdirfile[80];

    init_all();
    if (!loginok)
        http_fatal("匆匆过客不能处理讯息，请先登录");
    sprintf(filename, "home/%c/%s/msgfile.me",
            toupper(currentuser.userid[0]), currentuser.userid);
    post_mail(currentuser.userid, "所有讯息备份", filename,
              currentuser.userid, currentuser.username, fromhost, -1);

    sprintf(msgdirfile, "home/%c/%s/msgfile.me",
            toupper(currentuser.userid[0]), currentuser.userid);

    unlink(filename);
    unlink(msgdirfile);

    printf("讯息备份已经寄回您的信箱<br>\n");
    printf("<a href='javascript:history.go(-2)'>快速返回</a>");
    http_quit();
}
