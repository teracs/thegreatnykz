#include "www.h"

static int is_voting(char *board)
{
    int num_of_vote;
    char controlfile[STRLEN];

    sprintf(controlfile, "vote/%s/control", board);
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    return num_of_vote;
}

static char *get_anno_path(char *board)
{
    FILE *fp;
    static char buf[256], buf1[80], buf2[80];

    fp = fopen("0Announce/.Search", "r");
    if (fp == 0)
        return "";
    snprintf(buf, 255, "%s%c", board, ':');
    while (1)
    {
        if (fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2) <= 0)
            break;
        if (!strcasecmp(buf1, buf))
        {
            sprintf(buf, "/%s", buf2);
            fclose(fp);
            return buf;
        }
    }
    fclose(fp);
    return "";
}

int fbbsdoc_main()
{
    char board[80], page_str[10], post_str[10], filename[80];

    struct shortfile *x1, *bl;
    struct fileheader *fh;
    int content_width = 640, page = -1, num = 0, from, to, showtop = 0, i;
    int integral_width = 840, rightside_width;
    char ch;
    char *t = NULL;
    int district;
    char buf[256];
    int has_blink, has_annorec;
    FILE *fp;
    char *anno_path, *file_path, *title;
    char anno_file_path[256];
    int count;

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

    anno_path = get_anno_path(board);
    sprintf(buf, "0Announce%s/announce_recommend", anno_path);
    if (file_exist(buf))
        has_annorec = 1;
    else
        has_annorec = 0;

    js_head();
    printf("<body>");
    printf("<script type='text/javascript' src='/app/inboardlist.js'></script>\n");
    printf("<script>\n");
    printf(" $( function() { $('#board_info').accordion(); inboardlist();});");
    printf("</script>\n");
    printf
        ("<form name=form1 action='bbsdoc' method=get onsubmit='this.page.value -= 1;return true'>\n<input type=hidden name=board value='%s'>",
         board);
    printf("<center>\n");
    printf("<table><tr><td>");
    printf("<td><font class=title><b>%s(%s)</b></font>",
           x1->filename, x1->title + 11);
    
    printf("&nbsp;&nbsp;<a href=\"/wiki/%s板\">水源百科页面</a>", board);

    printf("</table>\n<nobr>\n");

    printf
        ("<table width=%d><tr style='height:14px'><td align=left width=38%%> 板主: %s",
         content_width, userid_str(x1->BM));
    if (is_voting(board) > 0)
        printf
            ("<td align=middle width=17%%><a href=bbsvotelist?board=%s><font style='font-size:12px; color:#f00000'>投票中(点击进行投票)</font></a></td>",
             board);
    printf("<td align=right width=25%%>");
#ifdef STATIC_URL
    if (from != 0)
        printf("<a href=bbsdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbsdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
#else
    if (from != 0)
        printf("<a href=bbsdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    if (to != num)
        printf("<a href=bbsdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
#endif

    printf
        ("<a onclick='return confirm(\"您的确要把这个板加入预定讨论区吗\")' href=bbsbrdadd?board=%s>订阅</a>/\n",
         board);
    printf
        ("<a onclick='return confirm(\"您的确要把这个板从预定讨论区中去掉吗\")' href=bbsbrddel?board=%s>退订</a>\n",
         board);
    printf("  <a href=\"bbsrss?board=%s\">RSS</a>", board);
    printf("<td align=right width=20%%>");
    printf
        ("<input type=submit value=跳转到> 第 <input type=text onkeydown=\"return true;\" name=post size=4> 贴");
    printf("</table>\n");

    district = board_in_which_dist(board);
    if (district <= 0)
        ch = '0';
    if (district <= 9)
        ch = district + '0';
    else
        ch = district - 10 + 'A';

    printf("<table width=%d><tbody><tr><td style='height:14px'>\n",
           content_width);
    show_bm_words(board, content_width - 30);
    printf("</td><td align='right'>%c区</td>", ch);
    printf("</tr></tbody></table>");

    printf("<hr>\n");

    printf("<table border='0' cellspacing='0' cellpadding='0'><tr><td>");

    printf("<table width=%d>\n", content_width);
    printf("	<tr bgcolor='e8e8e8'>\n");
    printf
        ("		<td width='40'>序号</td><td width='10'>&nbsp;</td><td width='100'>作者</td><td width='90'>日期</td><td>标题</td>\n");
    printf("	</tr>\n");

    for (i = from; i != to; i++)
    {
        sprintf(filename, "boards/%s/%s/%s", board,
                getfolderstr(fh[i].filename), fh[i].filename);
        printf("<tr><td>%d<td>%s<td>%s", i + 1,
               flag_str2(fh[i].accessed[0]), userid_str(fh[i].owner));
        printf("<td>%12.12s", Ctime(atoi(fh[i].filename + 2)) + 4);

#ifdef STATIC_URL
        printf("<td><a href=bbscon,board,%s,file,%s.html>%s%s%s </a>%s",
#else
        printf("<td><a href=bbscon?board=%s&file=%s>%s%s%s </a>%s",
#endif
               board, fh[i].filename,
               (fh[i].accessed[1] & FILE_NOREPLY2) ? "<font color=\"red\"><u>x</u></font>" :
               ((fh[i].accessed[0] & FILE_NOREPLY) ?
               "<font color = blue><u>x</u></font>" : ""),
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
                        ("<tr><td><font color=red>[提示]</font><td><td>%s",
                         userid_str(fh[i].owner));
                    break;
                case 'G':
                    printf
                        ("<tr><td><font color=red>[通知]</font><td><td>%s",
                         userid_str(fh[i].owner));
                    break;
                case 'B':
                    printf
                        ("<tr><td><font color=red>[板规]</font><td><td>%s",
                         userid_str(fh[i].owner));
                    break;
                default:
                    printf
                        ("<tr><td><font color=red>[置底]</font><td><td>%s",
                         userid_str(fh[i].owner));
                }
                printf("<td>%12.12s", Ctime(atoi(fh[i].filename + 2)) + 4);

                sprintf(filename, "boards/%s/%s/%c%s", board,
                        getfolderstr(fh[i].filename), 'M',
                        fh[i].filename + 1);
                printf("<td><a href=%s?board=%s&file=%c%s>%s%s </a>%s",
                       file_exist(filename) ? "bbscon" : "bbstopcon",
                       board, file_exist(filename) ? 'M' : 'T',
                       fh[i].filename + 1, strncmp(fh[i].title, "Re: ",
                                                   4) ? "○ " : "",
                       void1(nohtml2(fh[i].title)), eff_size(filename));
            }
            unmap_board_dir();
        }
    }
    printf("</table>\n");
    printf("</td>\n");

    if (1)
    {
        printf("<td valign='top'>\n");

	printf("<div id='board_info'>\n");
        if (has_annorec)
        {
            printf("<h3 bgcolor='e8e8e8'>板主推荐</h3>\n");
	    printf("<div style='overfall: auto; width: %d;'>", rightside_width);

            count = 0;
            sprintf(buf, "0Announce%s/announce_recommend", anno_path);
            fp = fopen(buf, "r");
            if (fp != NULL)
            {
                while (fgets(buf, 255, fp) != NULL)
                {
                    if (buf[0] == ';' || buf[0] == '#' || buf[0] == ' ')
                        continue;
                    file_path = strtok(buf, " \t\r\n");
                    title = strtok(NULL, "\r\n");
                    if (file_path == NULL || file_path[0] == '#' || title == NULL)
                        continue;
                    if (strstr(file_path, ".."))
                        continue;
                    sprintf(anno_file_path, "0Announce%s%s", anno_path, file_path);
                    printf("<p align='left' bgcolor='#f6f6f6'>");
                    if (file_isdir(anno_file_path))
                        printf("<a href=\"bbs0an,path,%s.html\">%s</a>", url_quote(anno_file_path + 9), nohtml2(title));
                    else
                        printf("<a href=\"bbsanc,path,%s.html\">%s</a>", url_quote(anno_file_path + 9), nohtml2(title));
                    printf("</p>\n");
                    count++;
                    if (count >= 10)
                        break;
                }
                fclose(fp);
            }

	    printf("</div>\n");

        }

        if (has_blink)
        {
            printf("<h3 bgcolor='e8e8e8'>友情链接</h3>\n");
	    printf("<div style='overfall: auto; width: %d;'>", rightside_width);

            count = 0;
            sprintf(buf, "etc/boardlinks/%s", board);
            fp = fopen(buf, "r");
            if (fp != NULL)
            {
                while (fgets(buf, 80, fp) != NULL)
                {
                    if (buf[0] == ';' || buf[0] == '#' || buf[0] == ' '
                        || strlen(buf) > 16)
                        continue;
                    t = strchr(buf, '\n');
                    if (t)
                        *t = '\0';
                    bl = getbcache(buf);
                    if (bl == NULL)
                        continue;
                    printf("<p align='left' bgcolor='#f6f6f6'>");
                    printf
                        ("<a href='/bbsdoc?board=%s' target='_self'>%s<br/></a>%s",
                         bl->filename, bl->filename, bl->title + 11);
                    printf("</p>\n");
                    count++;
                    if (count >= 5)
                        break;
                }
                fclose(fp);
            }

	    printf("</div>\n");
        }


	//inboard list
	printf("<h3>板面在线人员</h3>\n");
	printf("<div id='inboard' style='overfall: auto; width: %d;'>", rightside_width);
	printf("</div>\n");

	printf("</div>\n");
        printf("</td>\n");
    }

    printf("</tr>\n</table>\n");

    printf("<hr/>\n");

    printf("<a href=bbspst?board=%s>发表文章</a> \n", board);
    printf("<a href='javascript:location=location'>刷新</a> ");

#ifdef STATIC_URL
    if (from != 0)
    {
        printf("<a href=bbsdoc,board,%s,page,0.html>首页</a> ", board);
        printf("<a href=bbsdoc,board,%s,page,%d.html>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    }
    if (to != num)
    {
        printf("<a href=bbsdoc,board,%s,page,%d.html>下一页</a> ", board,
               to / PAGE_SIZE);
        printf("<a href=bbsdoc,board,%s.html>末页</a> ", board);
    }
    printf("<a href=bbstdoc,board,%s.html>主题模式</a> ", board);
    printf("<a href=bbsbfind?board=%s>板内查询</a><br> \n", board);
    printf("<a href=bbsfdoc2?board=%s>新上载区</a> ", board);
    printf("<a href=bbsnot?board=%s>进板画面</a> ", board);
    printf("<a href=bbsgdoc?board=%s>文摘区</a> ", board);
    printf
        ("<a href=bbs0an,path,%s.html><font color=red>精华区</font></a> ",
         anno_path_of(board));
    printf("<a href=bbsmdoc?board=%s>管理模式</a> ", board);
    printf("<a href=/%s.tar.gz>下载精华区</a> ", board);
#else
    if (from != 0)
    {
        printf("<a href=bbsdoc?board=%s&page=0>首页</a> ", board);
        printf("<a href=bbsdoc?board=%s&page=%d>上一页</a> ", board,
               (from % PAGE_SIZE ==
                0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
    }
    if (to != num)
    {
        printf("<a href=bbsdoc?board=%s&page=%d>下一页</a> ", board,
               to / PAGE_SIZE);
        printf("<a href=bbsdoc?board=%s>末页</a> ", board);
    }
    printf("<a href=bbstdoc?board=%s>主题模式</a> ", board);
    printf("<a href=bbsbfind?board=%s>板内查询</a><br> \n", board);
    printf("<a href=bbsfdoc2?board=%s>新上载区</a> ", board);
    printf("<a href=bbsnot?board=%s>进板画面</a> ", board);
    printf("<a href=bbsgdoc?board=%s>文摘区</a> ", board);
    printf("<a href=bbs0an?path=%s><font color=red>精华区</font></a> ",
           anno_path_of(board));
    printf("<a href=bbsmdoc?board=%s>管理模式</a> ", board);
    printf("<a href=/%s.tar.gz>下载精华区</a> ", board);
#endif
    printf("</form>\n");
    /*printf
        ("<script src=\"https://www.google-analytics.com/urchin.js\" type=\"text/javascript\">\n</script>\n<script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script></body>\n</html>");*/
    return 0;
}
