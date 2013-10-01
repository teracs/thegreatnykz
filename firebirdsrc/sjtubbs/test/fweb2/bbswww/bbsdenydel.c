#include "www.h"
int bbsdenydel_main()
{
    int i;
    char board[80], *userid;

    bzero(denyuser, sizeof(denyuser));
    denynum = 0;
    init_all();
    if (!loginok)
        http_quit_print("您尚未登录, 请先登录");
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("你无权进行本操作");
    loaddenyuser(board);
    userid = getparm("userid");
    for (i = 0; i < denynum; i++)
    {
        if (!strcasecmp(denyuser[i].id, userid))
        {
            denyuser[i].id[0] = 0;
            savedenyuser(board);
            printf("已经给 %s 解封. <br>\n", userid);
            inform(board, userid);
            printf("[<a href=bbsdenyall?board=%s>返回被封名单</a>]",
                   board);
            http_quit();
        }
    }
    http_quit_print("这个用户不在被封名单中");
    return 0;
}
