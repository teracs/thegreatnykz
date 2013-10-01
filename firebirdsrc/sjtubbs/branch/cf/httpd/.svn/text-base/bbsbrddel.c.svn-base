#include "www.h"

static char mybrd[GOOD_BRC_NUM][80], mybrd2[GOOD_BRC_NUM][80];
static int mybrdnum = 0;

int ismybrd(char *board)
{
    int n;

    for (n = 0; n < mybrdnum; n++)
        if (!strcasecmp(mybrd[n], board))
            return n;
    return -1;
}

int bbsbrddel_main()
{
    //FILE *fp;
    char /*file[200],*/ board[200];
    int i, total = 0;

    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!loginok)
        http_fatal("超时或未登录，请重新login");
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    if (ismybrd(board) < 0)
        http_fatal("你没有预定了这个讨论区");
    total = 0;
    for (i = 0; i < mybrdnum; i++)
    {
        if (strcasecmp(board, mybrd[i]))
        {
            strcpy(mybrd2[total], mybrd[i]);
            total++;
        }
    }
    SaveMyBrdsToGoodBrd(currentuser.userid, mybrd2, total);
    printf("<script>top.f2.location=top.f2.location</script>\n", board);
    printf("<a href='javascript:history.go(-1)'>快速返回</a>\n");
    http_quit();
}
