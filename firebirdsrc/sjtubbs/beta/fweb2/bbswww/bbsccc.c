#include "www.h"

static int
do_ccc(struct fileheader *x, char *board, char *board2)
{
	FILE *fp, *fp2;
	char title[512], buf[512], path[200], path2[200], i;
	time_t now = time(0);

	sprintf(path, "boards/%s/%s/%s", board, getfolderstr(x->filename), x->filename);
	fp = fopen(path, "r");
	if (fp == 0)
		http_quit_print("文件内容已丢失, 无法转载");
	sprintf(path2, "tmp/%d.tmp", getpid());
	fp2 = fopen(path2, "w");
	for (i = 0; i < 3; i++)
		if (fgets(buf, 256, fp) == 0)
			break;
	fprintf(fp2, "[37;1m【 以下文字转载自 [32m%s [37m讨论区 】\n",
		board);
	fprintf(fp2, "[37;1m【 原文由 [32m%s [37m所发表 】[m\n\n",
		x->owner);
	while (1)
	{
		if (fgets(buf, 256, fp) == 0)
			break;
		fprintf(fp2, "%s", buf);
	}
	fclose(fp);
	fclose(fp2);
	if (!strstr(x->title, "转载"))
		sprintf(title, "%.65s(转载)", x->title);
	else
		sprintf(title, "%s", x->title);

/*  
    if (!strncmp(x->title, "[转载]", 6))
    {
        sprintf(title, "%s", x->title);
    }
    else
    {
        sprintf(title, "[转载] %.55s", x->title);
    }
    */
	post_article(board2, title, path2, currentuser.userid,
		     currentuser.username, fromhost, -1, 0, now, now);
	unlink(path2);
	printf("'%s' 已转贴到 %s 板.<br>\n", nohtml(title), board2);
	printf("<input type=\"button\" name=\"a\" onclick=\"history.go(-2)\" value=\"[返回]\">\n");
	printf("<script>document.all(\"a\").focus();</script>");
	return 0;
}

int
bbsccc_main()
{
	struct fileheader *x;
	char board[80], file[80], target[80];
	time_t now = time(0);
	struct tm *tm;
	tm = localtime(&now);
	init_all();
	strsncpy(board, getparm("board"), 30);
	strsncpy(file, getparm("file"), 30);
	strsncpy(target, getparm("target"), 30);
	if (!loginok)
		http_quit_print("匆匆过客不能进行本项操作");
	if (!has_read_perm(&currentuser, board))
		http_quit_print("错误的讨论区");
	x = get_file_ent(board, file, 1);
	if (x == 0)
		http_quit_print("错误的文件名");
	if(!strncasecmp(board,target,16)) {
		http_quit_print("讨论区相同");

		}
	
	if(shm_bcache->all_no_post_end >= shm_bcache->all_no_post_begin) {
	if (!HAS_PERM(PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
	{
		if ((unsigned int)tm->tm_hour >= shm_bcache->all_no_post_begin && (unsigned int)tm->tm_hour < shm_bcache->all_no_post_end)
		{
			printf("本站于每日 %d 时至 %d 时，全站只读。",
				   shm_bcache->all_no_post_begin, shm_bcache->all_no_post_end);
			http_quit();
		}

	}

	if (!HAS_PERM(PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |PERM_BOARDS))
	{

		if ((unsigned int)tm->tm_hour >= shm_bcache->no_post_begin && (unsigned int)tm->tm_hour < shm_bcache->no_post_end)
		{
			if (!insjtunetwork(fromhost))
			{

				printf
					("本站于每日%d时至%d时暂停校外用户的发文权限。\n\n          如果有疑问请在 BBSHelp 或者 sysop 板查阅相关通知与说明。",
					 shm_bcache->no_post_begin, shm_bcache->no_post_end);
				http_quit();

			}
		}
	}
} else {
if (!HAS_PERM(PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
	{
		if ((unsigned int)tm->tm_hour >= shm_bcache->all_no_post_begin || (unsigned int)tm->tm_hour < shm_bcache->all_no_post_end)
		{
			printf("本站于每日 %d 时至次日 %d 时，全站只读。",
				   shm_bcache->all_no_post_begin, shm_bcache->all_no_post_end);
			http_quit();
		}

	}

	if (!HAS_PERM(PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |PERM_BOARDS))
	{

		if ((unsigned int)tm->tm_hour >= shm_bcache->no_post_begin || (unsigned int)tm->tm_hour < shm_bcache->no_post_end)
		{
			if (!insjtunetwork(fromhost))
			{

				printf
					("本站于每日%d时至次日%d时暂停校外用户的发文权限。\n\n          如果有疑问请在 BBSHelp 或者 sysop 板查阅相关通知与说明。",
					 shm_bcache->no_post_begin, shm_bcache->no_post_end);
				http_quit();

			}
		}
	}



}
	printf("<center><font class=\"title\">%s - 转载文章　　</font>[使用者: %s]<hr>\n", BBSNAME, currentuser.userid);
	if (target[0])
	{
		if (!has_post_perm(&currentuser, target))
			http_quit_print
				("错误的讨论区名称或你没有在该板发文的权限");
#ifdef	POST_AFTER_ONE_DAY
		if (too_young(&currentuser, target))
			http_quit_print
				("您注册时间还不满24小时，暂时不能在该板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
		return do_ccc(x, board, target);
	}
	printf("<table><tr><td>\n");
	printf("<font color=red>转贴发文注意事项:<br>\n");
	printf("本站规定同样内容的文章严禁在 4 个或 4 个以上讨论区内重复发表。");
	printf("违者将被封禁在本站发文的权利<br><br></font>\n");
	printf("文章标题: %s<br>\n", nohtml(x->title));
	printf("文章作者: %s<br>\n", x->owner);
	printf("原讨论区: %s<br>\n", board);
	printf("<form action=bbsccc method=post>\n");
	printf("<input type=hidden name=board value=%s>", board);
	printf("<input type=hidden name=file value=%s>", file);
	printf("转载到 <input name=\"target\" size=30 maxlength=30> 讨论区. ");
	printf("<input type=submit value=确定></form>");
	printf("\n<script language=\"javascript\">\n<!--\n");
	printf("document.all(\"target\").focus();\n-->\n</script>\n");
	printf("\n</html>");
	return 0;
}


