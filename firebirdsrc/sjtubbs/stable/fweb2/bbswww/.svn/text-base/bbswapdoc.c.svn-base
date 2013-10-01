#include "www.h"

int is_voting2(char *board)
{
    int num_of_vote;
    char controlfile[STRLEN];

    sprintf(controlfile, "vote/%s/control", board);
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    return num_of_vote;
}


int bbswapdoc_main()
{
    char board[80], page_str[10], post_str[10], filename[80];

    struct shortfile *x1;
    struct fileheader *fh;
    int content_width = 640, page = -1, num = 0, from, to, showtop = 0, i;
    int integral_width = 840, rightside_width;
    char ch;

    int district;
    char buf[256];
    int has_blink;

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
            cache_age = 10;
        cgi_head();
        html_head();
        print_style();
    }
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("目录错误");

    rightside_width = integral_width - content_width;
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
    if (num == to)
        showtop = 1;

    if (loginok)
        user_enter_board(board);

    sprintf(buf, "etc/boardlinks/%s", board);
    if (file_exist(buf))
        has_blink = 1;
    else
        has_blink = 0;

   
    district = board_in_which_dist(board);
    if (district <= 0)
        ch = '0';
    if (district <= 9)
        ch = district + '0';
    else
        ch = district - 10 + 'A';

    printf("<body>");
    printf
        ("<form name=form1 action='bbswapdoc' method=get onsubmit='this.page.value -= 1;return true'>\n<input type=hidden name=board value='%s'>",
         board);
    printf("%c区&nbsp;&nbsp;<font class=title>%s(%s)</font>",
           ch,x1->filename, x1->title + 11);
    
    printf("&nbsp;&nbsp;<a href=\"/wiki/%s板\">水源百科页面</a></br><hr>", board);

    printf("板主: %s</br>",userid_str(x1->BM));

#ifdef STATIC_URL
    if (from != 0)
        printf("<a href=bbswapdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbswapdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
#else
    if (from != 0)
        printf("<a href=bbswapdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbswapdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
#endif

    printf
        ("<input type=submit value=跳转到> 第 <input type=text onkeydown=\"return true;\" name=post size=4> 贴");

    printf("<hr>\n");

    for (i = from; i != to; i++)
    {
        sprintf(filename, "boards/%s/%s/%s", board,
                getfolderstr(fh[i].filename), fh[i].filename);
        printf("%d&nbsp;%s&nbsp;%s&nbsp;", i + 1,
               flag_str2(fh[i].accessed[0]), userid_str(fh[i].owner));
        printf("%12.12s</br>", Ctime(atoi(fh[i].filename + 2)) + 4);
#ifdef STATIC_URL
        printf("<a href=bbswaptcon,board,%s,reid,%d,file,%s.html>%s%s%s </a>%s<p>",
#else
        printf("<a href=bbswaptcon?board=%s&reid=%d&file=%s>%s%s%s </a>%s</br><p>",
#endif
               board,fh[i].reid, fh[i].filename,
               (fh[i].
                accessed[0] & FILE_NOREPLY) ?
               "<font color = blue><u>x</u></font>" : "",
               strncmp(fh[i].title, "Re: ", 4) ? "○ " : "",
               void1(nohtml2(fh[i].title)), eff_size(filename));
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
                        ("<font color=red>[提示]</font>%s",
                         userid_str(fh[i].owner));
                    break;
                case 'G':
                    printf
                        ("<font color=red>[通知]</font>%s",
                         userid_str(fh[i].owner));
                    break;
                case 'B':
                    printf
                        ("<font color=red>[板规]</font>%s",
                         userid_str(fh[i].owner));
                    break;
                default:
                    printf
                        ("<font color=red>[置底]</font>%s",
                         userid_str(fh[i].owner));
                }
                printf("&nbsp;%12.12s</br>", Ctime(atoi(fh[i].filename + 2)) + 4);

                sprintf(filename, "boards/%s/%s/%c%s", board,
                        getfolderstr(fh[i].filename), 'M',
                        fh[i].filename + 1);
                if (file_exist(filename))
		            printf("<a href=bbswaptcon?board=%s&reid=%d&file=%s>%s%s </a>%s</br><p>",
		                   board,fh[i].reid,
		                   fh[i].filename, strncmp(fh[i].title, "Re: ",
		                                               4) ? "○ " : "",
		                   void1(nohtml2(fh[i].title)), eff_size(filename));
                else
                	printf("<a href=bbstopcon?board=%s&file=T%s>%s%s </a>%s</br><p>",
                       board,fh[i].filename + 1, strncmp(fh[i].title, "Re: ",
                                                   4) ? "○ " : "",
                       void1(nohtml2(fh[i].title)), eff_size(filename));
                
            }
            unmap_board_dir();
        }
    }

    printf("<hr/>\n");
    if (is_voting2(board) > 0)
        printf
            ("<a href=bbsvotelist?board=%s><font style='font-size:12px; color:#f00000'>投票中(点击进行投票)</font></br></a>",
             board);

    printf("<a href=bbswappst?board=%s>发表文章</a> \n", board);
    printf("<a href='javascript:location=location'>刷新</a><br> ");

#ifdef STATIC_URL
    if (from != 0)
    {
        printf("<a href=bbswapdoc,board,%s,page,0.html>首页</a> ", board);
        printf("<a href=bbswapdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    }
    if (to != num)
    {
        printf("<a href=bbswapdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
        printf("<a href=bbswapdoc,board,%s.html>末页</a> ", board);
    }
    printf("<br><a href=bbswaptdoc,board,%s.html>主题模式</a> ", board);
    printf("<a href=bbswapbfind?board=%s>板内查询</a> \n", board);
    printf("<a href=bbsgdoc?board=%s>文摘区</a> ", board);
    printf
        ("<a href=bbs0an,path,%s.html><font color=red>精华区</font></a> ",
         anno_path_of(board));
#else
    if (from != 0)
    {
        printf("<a href=bbswapdoc?board=%s&page=0>首页</a> ", board);
        printf("<a href=bbswapdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    }
    if (to != num)
    {
        printf("<a href=bbswapdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
        printf("<a href=bbswapdoc?board=%s>末页</a> ", board);
    }
    printf("<br><a href=bbswaptdoc?board=%s>主题模式</a> ", board);
    printf("<a href=bbswapbfind?board=%s>板内查询</a> \n", board);

    printf("<a href=bbswapgdoc?board=%s>文摘区</a> ", board);
    printf("<a href=bbs0an?path=%s><font color=red>精华区</font></a> ",
           anno_path_of(board));

#endif
    printf("</form>\n");
    /*printf
        ("<script src=\"https://www.google-analytics.com/urchin.js\" type=\"text/javascript\">\n</script>\n<script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script></body>\n</html>");*/
    return 0;
}
