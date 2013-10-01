#include "www.h"

int bbswapgcon_main()
{
    FILE *fp;
    char buf[512], board[80], file[80], filename[80];
    struct fileheader *pfh, *next, *pre;
    int num;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
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

    printf
        ("<font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]<hr>",
         BBSNAME, board);
    if (strncmp(file, "G.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");
    if (map_board_dir(board, ".DIGEST") == 0)
        http_quit_print("此讨论区不存在或者为空");
    pfh = find_in_map_dir(file);
    num = get_offset_of_mapdir();
    if (NULL == pfh)
    {
        unmap_board_dir();
        http_quit_print("此文不存在");
    }
    pre = pre_in_map_dir();
    if (NULL != pre)
    {
        printf("[<a href=bbsgcon?board=%s&file=%s>上一篇</a>]",
               board, pre->filename);
    }
    printf("[<a href=bbswapdoc?board=%s>本讨论区</a>]", board);
    printf("[<a href=bbswapgdoc?board=%s&page=%d>文摘区</a>]", board,
           num / PAGE_SIZE);
    next = next_in_map_dir();
    if (NULL != next)
    {
        printf("[<a href=bbswapgcon?board=%s&file=%s>下一篇</a>]",
               board, next->filename);
    }
    printf("[<a href='bbswaptcon?board=%s&reid=%d'>同主题阅读</a>]\n",
           board, pfh->reid);
    printf("<pre>");
    sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(filename, "r");
    if (fp == 0)
    {
        unmap_board_dir();
        http_quit_print("本文不存在或者已被删除");
    }
    while (1)
    {
        if (fgets(buf, 512, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre>\n");
    if (NULL != pre)
        printf("[<a href=bbswapgcon?board=%s&file=%s>上一篇</a>]",
               board, pre->filename);
    printf("[<a href=bbswapdoc?board=%s>本讨论区</a>]", board);
    printf("[<a href=bbswapgdoc?board=%s&page=%d>文摘区</a>]", board,
           num / PAGE_SIZE);
    if (NULL != next)
        printf("[<a href=bbswapgcon?board=%s&file=%s>下一篇</a>]",
               board, next->filename);
    printf("[<a href='bbswaptcon?board=%s&reid=%d'>同主题阅读</a>]\n",
           board, pfh->reid);
    unmap_board_dir();
    printf("<html>");
    return 0;
}
