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
		http_quit_print("ÎÄ¼şÄÚÈİÒÑ¶ªÊ§, ÎŞ·¨×ªÔØ");
	sprintf(path2, "tmp/%d.tmp", getpid());
	fp2 = fopen(path2, "w");
	for (i = 0; i < 3; i++)
		if (fgets(buf, 256, fp) == 0)
			break;
	fprintf(fp2, "[37;1m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô [32m%s [37mÌÖÂÛÇø ¡¿\n",
		board);
	fprintf(fp2, "[37;1m¡¾ Ô­ÎÄÓÉ [32m%s [37mËù·¢±í ¡¿[m\n\n",
		x->owner);
	while (1)
	{
		if (fgets(buf, 256, fp) == 0)
			break;
		fprintf(fp2, "%s", buf);
	}
	fclose(fp);
	fclose(fp2);
	if (!strstr(x->title, "×ªÔØ"))
		sprintf(title, "%.65s(×ªÔØ)", x->title);
	else
		sprintf(title, "%s", x->title);

/*  
    if (!strncmp(x->title, "[×ªÔØ]", 6))
    {
        sprintf(title, "%s", x->title);
    }
    else
    {
        sprintf(title, "[×ªÔØ] %.55s", x->title);
    }
    */
	post_article(board2, title, path2, currentuser.userid,
		     currentuser.username, fromhost, -1, 0, now, now);
	unlink(path2);
	printf("'%s' ÒÑ×ªÌùµ½ %s °å.<br>\n", nohtml(title), board2);
	printf("<input type=\"button\" name=\"a\" onclick=\"history.go(-2)\" value=\"[·µ»Ø]\">\n");
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
		http_quit_print("´Ò´Ò¹ı¿Í²»ÄÜ½øĞĞ±¾Ïî²Ù×÷");
	if (!has_read_perm(&currentuser, board))
		http_quit_print("´íÎóµÄÌÖÂÛÇø");
	x = get_file_ent(board, file, 1);
	if (x == 0)
		http_quit_print("´íÎóµÄÎÄ¼şÃû");
	if(!strncasecmp(board,target,16)) {
		http_quit_print("ÌÖÂÛÇøÏàÍ¬");

		}
	
	if(shm_bcache->all_no_post_end >= shm_bcache->all_no_post_begin) {
	if (!HAS_PERM(PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
	{
		if ((unsigned int)tm->tm_hour >= shm_bcache->all_no_post_begin && (unsigned int)tm->tm_hour < shm_bcache->all_no_post_end)
		{
			printf("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±£¬È«Õ¾Ö»¶Á¡£",
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
					("±¾Õ¾ÓÚÃ¿ÈÕ%dÊ±ÖÁ%dÊ±ÔİÍ£Ğ£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏŞ¡£\n\n          Èç¹ûÓĞÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
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
			printf("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±£¬È«Õ¾Ö»¶Á¡£",
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
					("±¾Õ¾ÓÚÃ¿ÈÕ%dÊ±ÖÁ´ÎÈÕ%dÊ±ÔİÍ£Ğ£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏŞ¡£\n\n          Èç¹ûÓĞÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
					 shm_bcache->no_post_begin, shm_bcache->no_post_end);
				http_quit();

			}
		}
	}



}
	printf("<center><font class=\"title\">%s - ×ªÔØÎÄÕÂ¡¡¡¡</font>[Ê¹ÓÃÕß: %s]<hr>\n", BBSNAME, currentuser.userid);
	if (target[0])
	{
		if (!has_post_perm(&currentuser, target))
			http_quit_print
				("´íÎóµÄÌÖÂÛÇøÃû³Æ»òÄãÃ»ÓĞÔÚ¸Ã°å·¢ÎÄµÄÈ¨ÏŞ");
#ifdef	POST_AFTER_ONE_DAY
		if (too_young(&currentuser, target))
			http_quit_print
				("Äú×¢²áÊ±¼ä»¹²»Âú24Ğ¡Ê±£¬ÔİÊ±²»ÄÜÔÚ¸Ã°å·¢±íÎÄÕÂ.Èç¹ûÓĞÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
#endif
		return do_ccc(x, board, target);
	}
	printf("<table><tr><td>\n");
	printf("<font color=red>×ªÌù·¢ÎÄ×¢ÒâÊÂÏî:<br>\n");
	printf("±¾Õ¾¹æ¶¨Í¬ÑùÄÚÈİµÄÎÄÕÂÑÏ½ûÔÚ 4 ¸ö»ò 4 ¸öÒÔÉÏÌÖÂÛÇøÄÚÖØ¸´·¢±í¡£");
	printf("Î¥Õß½«±»·â½ûÔÚ±¾Õ¾·¢ÎÄµÄÈ¨Àû<br><br></font>\n");
	printf("ÎÄÕÂ±êÌâ: %s<br>\n", nohtml(x->title));
	printf("ÎÄÕÂ×÷Õß: %s<br>\n", x->owner);
	printf("Ô­ÌÖÂÛÇø: %s<br>\n", board);
	printf("<form action=bbsccc method=post>\n");
	printf("<input type=hidden name=board value=%s>", board);
	printf("<input type=hidden name=file value=%s>", file);
	printf("×ªÔØµ½ <input name=\"target\" size=30 maxlength=30> ÌÖÂÛÇø. ");
	printf("<input type=submit value=È·¶¨></form>");
	printf("\n<script language=\"javascript\">\n<!--\n");
	printf("document.all(\"target\").focus();\n-->\n</script>\n");
	printf("\n</html>");
	return 0;
}


