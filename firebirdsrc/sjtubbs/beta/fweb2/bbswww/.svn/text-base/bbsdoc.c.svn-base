#include "www.h"


static int 
is_voting(char *board)
{
	int num_of_vote;
	char controlfile[STRLEN];
	sprintf(controlfile,"vote/%s/control",board);
	num_of_vote = get_num_records (controlfile, sizeof (struct votebal));
	return num_of_vote;
}

int
bbsdoc_main()
{
	char board[80], page_str[10], post_str[10], filename[80];
	struct shortfile *x1;
	struct fileheader *fh;
	int width = 720, page = -1, num = 0, from, to, showtop = 0, i;

	silence = 1;		/* trick to make init_all() not to print http header */
	init_all();
	strsncpy(board, getparm("board"), 32);
	strsncpy(page_str, getparm("page"), 10);
	strsncpy(post_str, getparm("post"), 10);
	if (strlen(page_str) != 0)
		page = atoi(page_str);

	//added by pax@yssy 06-04-27
	else if (strlen(post_str) != 0)
		page = (atoi(post_str)-1)/20;
    //end added

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

	if (my_font_mode == 0)
		width = 670;

	printf("<body>");
	printf("<form name=form1 action='bbsdoc' method=get onsubmit='this.page.value -= 1;return true'>\n<input type=hidden name=board value='%s'>", board);
	printf("<center>\n");
	printf("<table><tr><td>");
	printf("<td><font class=title><b>%s(%s)</b></font></table>\n",
	       x1->filename, x1->title + 11);

	printf("<nobr>\n");

	printf("<table width=%d><tr style='height:14px'><td align=left width=40%%> 板主: %s", width, userid_str(x1->BM));
	if (is_voting(board)>0)
        printf("<td align=middle width=20%%><a href=bbsvotelist?board=%s><font style='font-size:12px; color:#f00000'>投票中(点击进行投票)</font></a></td>",board);
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

	printf("<a onclick='return confirm(\"您的确要把这个板加入预定讨论区吗\")' href=bbsbrdadd?board=%s>订阅</a>/\n", board);
	printf("<a onclick='return confirm(\"您的确要把这个板从预定讨论区中去掉吗\")' href=bbsbrddel?board=%s>退订</a>\n", board);
	printf("  <a href=\"bbsrss?board=%s\">RSS</a>", board);
	printf("<td align=right width=25%%>");
	printf("<input type=submit value=跳转到> 第 <input type=text onkeydown=\"return true;\" name=post size=4> 贴");
	printf("</table>\n");
	show_bm_words(board, width);
	printf("<hr>\n");
	printf("<table width=%d>\n", width);
	printf("<tr bgcolor='e8e8e8'><td>序号<td>状态<td>作者<td width=72>日期<td>标题\n");


	for (i = from; i != to; i++)
	{
		sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(fh[i].filename), fh[i].filename);
		printf("<tr><td>%d<td>%s<td>%s",
		       i + 1, flag_str2(fh[i].accessed[0]),
		       userid_str(fh[i].owner));
		printf("<td>%12.12s", Ctime(atoi(fh[i].filename + 2)) + 4);

		//modified by acdsee,add x flag
#ifdef STATIC_URL
		printf("<td><a href=bbscon,board,%s,file,%s.html>%s%s%s </a>%s",
#else
		printf("<td><a href=bbscon?board=%s&file=%s>%s%s%s </a>%s",
#endif
		       board, fh[i].filename,
		       (fh[i].
			accessed[0] & FILE_NOREPLY) ?
		       "<font color = blue><u>x</u></font>" : "&nbsp;&nbsp;",
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
				sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(fh[i].filename),
					fh[i].filename);
				switch (ptr[0])
				{
				case 'M':
					printf("<tr><td><font color=red>[提示]</font><td><td>%s", userid_str(fh[i].owner));
					break;
				case 'G':
					printf("<tr><td><font color=red>[通知]</font><td><td>%s", userid_str(fh[i].owner));
					break;
				case 'B':
					printf("<tr><td><font color=red>[板规]</font><td><td>%s", userid_str(fh[i].owner));
					break;
				default:
					printf("<tr><td><font color=red>[置底]</font><td><td>%s", userid_str(fh[i].owner));
				}
				printf("<td>%12.12s",
				       Ctime(atoi(fh[i].filename + 2)) + 4);


				printf("<td><a href=bbstopcon?board=%s&file=%s>&nbsp;&nbsp;%s%s </a>%s", board, fh[i].filename, strncmp(fh[i].title, "Re: ", 4) ? "○ " : "", void1(nohtml2(fh[i].title)), eff_size(filename));

			}
			unmap_board_dir();
		}
	}
	printf("</table><hr>\n");
	//printf("<a href=bbsfdoc?board=%s>旧上载区</a> ", board);
	printf("<a href=bbspst?board=%s>发表文章</a> \n", board);
	printf("<a href='javascript:location=location'>刷新</a> ");
	//if (is_voting(board)>0&&has_post_perm(&currentuser, board)&&loginok)
	//	printf("<a href=bbsvotelist?board=%s>投票</a> ", board);
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
	printf("<a href=bbs0an,path,%s.html><font color=red>精华区</font></a> ", anno_path_of(board));
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
	printf("<a href=bbs0an?path=%s><font color=red>精华区</font></a> ", anno_path_of(board));
	printf("<a href=bbsmdoc?board=%s>管理模式</a> ", board);
	printf("<a href=/%s.tar.gz>下载精华区</a> ", board);
#endif
	printf("</form>\n");
	printf("<script src=\"http://www.google-analytics.com/urchin.js\" type=\"text/javascript\">\n</script>\n<script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script></body>\n</html>");
	return 0;
}
