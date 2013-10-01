#include "www.h"

int bbsclear_main()
{
    char board[80], start[80], buf[256];

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(start, getparm("start"), 32);
    if (!loginok)
        http_quit_print("匆匆过客无法执行此项操作, 请先登录");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    brc_init(currentuser.userid, board);
    brc_clear();
    brc_update(currentuser.userid, board);
    sprintf(buf, "bbsdoc?board=%s&start=%s", board, start);
    refreshto(buf, 0);
    printf("\n</html>");
    return 0;

}
