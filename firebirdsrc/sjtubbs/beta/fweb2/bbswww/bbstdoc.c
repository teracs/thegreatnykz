#include "www.h"

int
bbstdoc_main()
{
	char board[80], page_str[10],post_str[10] ;
	struct shortfile *x1;
	struct fileheader *fh, **fhs;
	int page = -1, num = 0, from, to, i, num2 = 0, showtop = 0;

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
	if(to==num2) showtop=1;
	x1 = getbcache(board);
	strcpy(board, x1->filename);
	printf("<body>\n");
	printf("<nobr><center>\n");
	printf("<font class=\"title\">%s - 主题阅读　　</font>[讨论区: %s] 板主[%s] 文章%d, 主题%d个<hr>\n", BBSNAME, board, userid_str(x1->BM), num, num2);
	printf("<table width=670>\n");
	printf("<tr bgcolor=e8e8e8><td>序号<td>状态<td>作者<td width=100>日期<td>标题\n");
	for (i = from; i < to; i++)
	{
		printf("<tr><td>%d<td>%s<td>%s",
		       i + 1, flag_str(fhs[i]->accessed[0]),
		       userid_str(fhs[i]->owner));
		printf("<td>%12.12s", Ctime(atoi(fhs[i]->filename + 2)) + 4);
#ifdef STATIC_URL
		printf("<td><a href=bbstcon,board,%s,reid,%d.html>○ %s </a>",
		       board, fhs[i]->reid, void1(nohtml2(fhs[i]->title)));
#else
		printf("<td><a href=bbstcon?board=%s&reid=%d>○ %s </a>",
		       board, fhs[i]->reid, void1(nohtml2(fhs[i]->title)));
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
				sprintf(filename, "boards/%s/%s/%s", board,  getfolderstr(fh[i].filename),
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
#ifdef STATIC_URL
	if (from != 0)
		printf("<a href=bbstdoc,board,%s,page,%d.html>上一页</a> ", board,
		       (from % PAGE_SIZE ==
			0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
	if (to != num2)
		printf("<a href=bbstdoc,board,%s,page,%d.html>下一页</a> ", board,
		       to / PAGE_SIZE);
#else	
	if (from != 0)
		printf("<a href=bbstdoc?board=%s&page=%d>上一页</a> ", board,
		       (from % PAGE_SIZE ==
			0) ? (from / PAGE_SIZE - 1) : (from / PAGE_SIZE));
	if (to != num2)
		printf("<a href=bbstdoc?board=%s&page=%d>下一页</a> ", board,
		 to / PAGE_SIZE);
#endif
	printf("<a href=bbsnot?board=%s>进板画面</a> ", board);
	printf("<a href=bbsdoc?board=%s>一般模式</a> ", board);
	printf("<a href=bbsgdoc?board=%s>文摘区</a> ", board);
	printf("<a href=bbs0an?path=%s>精华区</a> ", anno_path_of(board));
	printf("<a href=bbspst?board=%s>发表文章</a> <br>\n", board);
	printf("<form action=bbstdoc method=get onsubmit='this.page.value-=1;return true'><input type=hidden name=board value='%s'>\n", board);
	printf("<input type=submit value=跳转到> 第 <input type=text name=post size=4> 帖");//modified by pax
	printf("</form><script src=\"http://www.google-analytics.com/urchin.js\" type=\"text/javascript\">\n</script>\n<script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script>\n</body>");
	printf("</html>");
	return 0;
}

char *
stat1(struct fileheader *data, int from, int total)
{
	static char buf[256];
	char *ptr = data[from].title;
	int i, re = 0, click = *(int *) (data[from].title + 73);

	for (i = from; i < total; i++)
	{
		if (!strncmp(ptr, data[i].title + 4, 40))
		{
			re++;
			data[i].title[0] = 0;
			click += *(int *) (data[i].title + 73);
		}
	}
	sprintf(buf, "<font color=%s>%d</font>/<font color=%s>%d</font>",
		re > 9 ? "red" : "black", re, click > 499 ? "red" : "black",
		click);
	return buf;
}
