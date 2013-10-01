#include "www.h"

static char mybrd[GOOD_BRC_NUM][80];
static int mybrdnum = 0;

static int ismybrd(char *board)
{
    int n;

    for (n = 0; n < mybrdnum; n++)
        if (!strcasecmp(mybrd[n], board))
            return n;
    return -1;
}

int bbsbrdadd_main()
{

    char board[200];

    bzero(mybrd, sizeof(mybrd));
    mybrdnum = 0;
    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!loginok)
        http_quit_print("超时或未登录，请重新login");
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    if (mybrdnum > 20)
        http_quit_print("您预定讨论区数目已达上限，不能增加预定");
    if (ismybrd(board) >= 0)
        http_quit_print("你已经预定了这个讨论区");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("此讨论区不存在");
    strcpy(mybrd[mybrdnum], board);
    SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum + 1);
    printf("<script>top.f2.location='bbsleft'</script>\n");
    printf
        ("预定讨论区成功<br><a href='javascript:history.go(-1)'>快速返回</a>");
    printf("\n</html>");
    return 0;
}
