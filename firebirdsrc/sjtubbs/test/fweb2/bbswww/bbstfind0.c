#include "www.h"
#define TOPIC_MAX 2000
int bbstfind0_main()
{
    char board[80], reid_str[16];
    struct fileheader *fh, *fhs[TOPIC_MAX];
    int num = 0, offset, no_re = 0;
    unsigned int reid = 0;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(reid_str, getparm("reid"), 16);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("错误的讨论区");
    }
    else
    {
        if (is_normal_board)
            cache_age = 3 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    if (strlen(reid_str) != 0)
        reid = atoi(reid_str);
    else
        http_quit_print("错误参数reid");
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("目录错误");
    offset = get_size_of_mapdir() - 1;
    for (; offset >= 0; --offset)
        if (fh[offset].reid == reid)
        {
            fhs[num++] = &fh[offset];
            if (fh[offset].id == reid || num == TOPIC_MAX)
                break;
        }
    if (num == 0)
    {
        unmap_board_dir();
        http_quit_print("该主题不存在");
    }

    printf
        ("<center><font class=\"title\">%s - 同主题查找　　</font>[讨论区: %s] [主题 '%s']<hr>\n",
         BBSNAME, board, void1(nohtml2(fhs[num - 1]->title)));
    printf
        ("<table width=610><tr bgcolor=e8e8e8><td>序号<td>作者<td>日期<td>标题");
    for (offset = num - 1; offset >= 0; --offset)
    {
        struct fileheader *p;

        p = fhs[offset];
        printf("<tr><td>%d", num - offset);
        printf("<td>%s", userid_str(p->owner));
        printf("<td>%6.6s", Ctime(atoi(p->filename + 2)) + 4);
        printf("<td><a href=bbscon?board=%s&file=%s>%s</a>",
               board, p->filename, void1(nohtml2(p->title)));
    }
    printf("</table><hr>\n共找到 %d 篇\n", num);
    printf("<a href=bbsdoc?board=%s>本讨论区</a> ", board);
    printf("<a href=bbstcon?board=%s&reid=%d>本主题全部展开</a> ",
           board, fhs[0]->reid);
    unmap_board_dir();
    printf("</html>");
    return 0;
}
