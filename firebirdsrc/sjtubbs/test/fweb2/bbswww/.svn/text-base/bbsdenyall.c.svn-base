#include "www.h"

int bbsdenyall_main()
{
    int i;
    char board[80];

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
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - 被封用户名单　　</font>[讨论区: %s]<hr><br>\n",
         BBSNAME, board);
    printf("本板共有 %d 人被封<br>", denynum);
    printf
        ("<table border=1><tr><td>序号<td>用户帐号<td>被封原因<td>原定解封日期<td>管理\n");
    for (i = 0; i < denynum; i++)
    {
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=bbsqry?userid=%s>%s</a>", denyuser[i].id,
               denyuser[i].id);
        printf("<td>%s\n", nohtml(denyuser[i].exp));
        printf("<td>%s\n", nohtml(denyuser[i].free_time));

        printf
            ("<td>[<a onclick='return confirm(\"确实解封吗?\")' href=bbsdenydel?board=%s&userid=%s>解封</a>]",
             board, denyuser[i].id);
    }
    printf("</table><hr>\n");
    printf
        ("[<a href=bbsdenyadd?board=%s>设定新的不可POST用户</a>]</center>\n",
         board);
    printf("\n</html>");
    return 0;
}
