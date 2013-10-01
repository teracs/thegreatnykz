#include "www.h"

int bbswapgdoc_main()
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

    printf
        ("<font class=\"title\">%s(%s)</font></br>板主:%s</br>[文摘区]\n",
         board,x1->title + 11, userid_str(x1->BM));
    if (from != 0)
        printf("<a href=bbswapgdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbswapgdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);

	printf("<br><hr>");
    for (i = from; i != to; i++)
    {
        printf("%d&nbsp; %s&nbsp; %s&nbsp;",
               i + 1, flag_str(fh[i].accessed[0]),
               userid_str(fh[i].owner));
        printf("%12.12s</br>", Ctime(atoi(fh[i].filename + 2)) + 4);
        printf("<a href=bbswapgcon?board=%s&file=%s>%s%40.40s</a></br><p>",
               board, fh[i].filename, strncmp(fh[i].title, "Re: ",
                                              4) ? "● " : "",
               void1(nohtml2(fh[i].title)));
    }
    if (from != 0)
        printf("<a href=bbswapgdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbswapgdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
    printf("<a href=bbswapdoc?board=%s>本讨论区</a> ", board);
    printf("<a href=bbs0an?path=%s>精华区</a> ", anno_path_of(board));
    unmap_board_dir();
    printf("<html>");
    return 0;
}
