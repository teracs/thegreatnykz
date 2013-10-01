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

    char board[200];
    int i, total = 0;

    bzero(mybrd, sizeof(mybrd));
    bzero(mybrd2, sizeof(mybrd2));
    mybrdnum = 0;
    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!loginok)
        http_quit_print("超时或未登录，请重新login");
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    if (ismybrd(board) < 0)
        http_quit_print("你没有预定了这个讨论区");
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
    printf("<script>top.f2.location=top.f2.location</script>\n");
    printf("<a href='javascript:history.go(-1)'>快速返回</a>\n");
    printf("\n</html>");
    return 0;
}
