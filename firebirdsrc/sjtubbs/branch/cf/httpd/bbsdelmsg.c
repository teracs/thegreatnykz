#include "www.h"

//modified by rwzmm@sjtubbs
//delete .msgdir while delete .msgfile.me
int bbsdelmsg_main()
{
    char path[80];
    char path_2[80];
    init_all();
    if (!loginok)
        http_fatal("匆匆过客不能处理讯息, 请先登录");
    sethomefile(path, currentuser.userid, "msgfile.me");
    unlink(path);

    sethomefile(path_2, currentuser.userid, ".msgdir");
    unlink(path_2);

    printf("已删除所有讯息备份");
}
