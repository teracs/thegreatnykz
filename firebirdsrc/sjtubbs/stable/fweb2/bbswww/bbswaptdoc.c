#include "www.h"

int bbswaptdoc_main()
{
    char board[80], page_str[10], post_str[10];
    struct shortfile *x1;
    struct fileheader *fh, **fhs;
    int page = -1, num = 0, from, to, i, num2 = 0, showtop = 0, j, replynum;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
    strsncpy(post_str, getparm("post"), 10);

    if (strlen(page_str) != 0)
        page = atoi(page_str);

    else if (strlen(post_str) != 0)
        page = (atoi(post_str) - 1) / 20;

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
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("目录错误");
    num = get_size_of_mapdir();
    fhs = (struct fileheader **) mp_alloc(sizeof(struct fileheader *) *
                                          num);
    for (i = 0; i < num; ++i)
    {
        if (fh[i].id == fh[i].reid)
        {
            fhs[num2++] = &fh[i];
        }
    }
    from = page * PAGE_SIZE;
    to = from + PAGE_SIZE;
    if (from >= num2 || from < 0)
    {
        from = num2 - PAGE_SIZE;
        to = num2;
    }
    else if (to > num2)
    {
        from -= (to - num2);
        to = num2;
    }
    if (from < 0)
        from = 0;
    if (num2 == 0)
    {
        unmap_board_dir();
        http_quit_print("本讨论区目前没有文章");
    }
    if (to == num2)
        showtop = 1;
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    printf("<body>\n");

    printf
        ("<font class=\"title\">%s(%s)</font></br>板主:%s</br>[同主题模式]\n",
         board,x1->title + 11, userid_str(x1->BM));
#ifdef STATIC_URL
    if (from != 0)
        printf("<a href=bbswaptdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num2)
        printf("<a href=bbswaptdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
#else
    if (from != 0)
        printf("<a href=bbswaptdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num2)
        printf("<a href=bbswaptdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
#endif
	printf("<br><hr>");
    for (i = from; i < to; i++)
    {
        replynum = 0;
        for (j = num - 1; j >=0; j--)
            if (fh[j].reid == fhs[i]->id)
            {
               if (fh[j].id == fhs[i]->id)
                   break;
               replynum++;
            }    
        printf("%d&nbsp; %s&nbsp; %s&nbsp;",
               i + 1, flag_str(fhs[i]->accessed[0]),
               userid_str(fhs[i]->owner));
        printf("%12.12s&nbsp;(%d回复)</br>", Ctime(atoi(fhs[i]->filename + 2)) + 4, replynum);
#ifdef STATIC_URL
        printf("<a href=bbswaptcon,board,%s,reid,%d.html>○ %s </a></br><p>",
               board, fhs[i]->reid, void1(nohtml2(fhs[i]->title)));
#else
        printf("<a href=bbswaptcon?board=%s&reid=%d>○ %s </a></br><p>",
               board, fhs[i]->reid, void1(nohtml2(fhs[i]->title)), replynum);
#endif
    }
    unmap_board_dir();
    if (showtop)
    {
        fh = (struct fileheader *) map_board_dir(board, ".TOPFILE");
        if (NULL != fh)
        {
            int size = get_size_of_mapdir();
            char *ptr;

            for (i = 0; i < size; i++)
            {
                char filename[80];

                ptr = flag_str2(fh[i].accessed[0]);
                sprintf(filename, "boards/%s/%s/%s", board,
                        getfolderstr(fh[i].filename), fh[i].filename);
                switch (ptr[0])
                {
                case 'M':
                    printf
                        ("<font color=red>[提示]</font>&nbsp;%s",
                         userid_str(fh[i].owner));
                    break;
                case 'G':
                    printf
                        ("<font color=red>[通知]</font>&nbsp;%s",
                         userid_str(fh[i].owner));
                    break;
                case 'B':
                    printf
                        ("<font color=red>[板规]</font>&nbsp;%s",
                         userid_str(fh[i].owner));
                    break;
                default:
                    printf
                        ("<font color=red>[置底]</font>&nbsp;%s",
                         userid_str(fh[i].owner));
                }
                printf("&nbsp;%12.12s</br>", Ctime(atoi(fh[i].filename + 2)) + 4);

                printf
                    ("<a href=bbstopcon?board=%s&file=%s>%s%s </a>%s</br><p>",
                     board, fh[i].filename, strncmp(fh[i].title, "Re: ",
                                                    4) ? "○ " : "",
                     void1(nohtml2(fh[i].title)), eff_size(filename));

            }
            unmap_board_dir();
        }
    }
    printf("</table><hr>\n");
#ifdef STATIC_URL
    if (from != 0)
        printf("<a href=bbswaptdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num2)
        printf("<a href=bbswaptdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
#else
    if (from != 0)
        printf("<a href=bbswaptdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num2)
        printf("<a href=bbswaptdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
#endif
    printf("<a href=bbswapdoc?board=%s>一般模式</a> ", board);
    printf("<a href=bbswapgdoc?board=%s>文摘区</a> ", board);
    printf("<a href=bbs0an?path=%s>精华区</a> ", anno_path_of(board));
    printf("<a href=bbswappst?board=%s>发表文章</a> <br>\n", board);
    printf
        ("<form action=bbstdoc method=get onsubmit='this.page.value-=1;return true'><input type=hidden name=board value='%s'>\n",
         board);
    printf
        ("<input type=submit value=跳转到> 第 <input type=text name=post size=4> 帖");
    /*printf
        ("</form><script src=\"https://www.google-analytics.com/urchin.js\" type=\"text/javascript\">\n</script>\n<script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script>\n</body>");*/
    printf("</html>");
    return 0;
}


