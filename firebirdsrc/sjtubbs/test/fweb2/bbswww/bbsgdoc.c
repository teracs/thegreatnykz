#include "www.h"

int bbsgdoc_main()
{

    char board[80], page_str[10];
    struct shortfile *x1;
    struct fileheader *fh;
    int page = -1, num = 0, from, to, i;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
    if (strlen(page_str) != 0)
        page = atoi(page_str);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("错误的讨论区");
    }
    else
    {
        if (is_normal_board)
            cache_age = 10 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    fh = (struct fileheader *) map_board_dir(board, ".DIGEST");
    if (fh == NULL)
        http_quit_print("目录错误");
    num = get_size_of_mapdir();
    from = page * PAGE_SIZE;
    to = from + PAGE_SIZE;
    if (from >= num || from < 0)
    {
        from = num - PAGE_SIZE;
        to = num;
    }
    else if (to > num)
    {
        from -= (to - num);
        to = num;
    }
    if (from < 0)
        from = 0;

    printf("<nobr><center>\n");
    printf
        ("<font class=\"title\">%s - [文摘区: %s]　　</font>板主[%s] 文章数[%d]<hr>\n",
         BBSNAME, board, userid_str(x1->BM), num);
    printf("<table width=610>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>序号<td>状态<td>作者<td>日期<td>标题\n");
    for (i = from; i != to; i++)
    {
        printf("<tr><td>%d<td>%s<td>%s",
               i + 1, flag_str(fh[i].accessed[0]),
               userid_str(fh[i].owner));
        printf("<td>%12.12s", Ctime(atoi(fh[i].filename + 2)) + 4);
        printf("<td><a href=bbsgcon?board=%s&file=%s>%s%40.40s</a>\n",
               board, fh[i].filename, strncmp(fh[i].title, "Re: ",
                                              4) ? "● " : "",
               void1(nohtml2(fh[i].title)));
    }
    printf("</table><hr>\n");
    if (from != 0)
        printf("<a href=bbsgdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbsgdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
    printf("<a href=bbsdoc?board=%s>本讨论区</a> ", board);
    printf("<a href=bbs0an?path=%s>精华区</a> ", anno_path_of(board));
    unmap_board_dir();
    printf("<html>");
    return 0;
}
