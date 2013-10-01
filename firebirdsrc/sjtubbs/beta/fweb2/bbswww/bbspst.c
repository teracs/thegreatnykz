#include "www.h"

//get_title added by heack 2005-05-05

/*static char buftext[8192];*/

char *get_title(char *title)
{
	if(strcmp(getparm("title"),""))return getparm("title");
	else return 	void3(void1(title));
}
//this func will be used in the future
/*char *get_quote(char * file)
{
	char quote[2048]="";
	if (file[0])
		{
			int lines = 0;
			sprintf(quote,"%s【 在 %s 的大作中提到: 】\n", quote,userid);
			//printf("【 在 %s 的大作中提到: 】\n", userid);
			sprintf(path, "boards/%s/%s", board, file);
			fp = fopen(path, "r");
			if (fp)
			{
				for (i = 0; i < 3; i++)
					if (fgets(buf, 500, fp) == 0)
						break;
				while (1)
				{
					if (fgets(buf, 500, fp) == 0)
						break;
					if (!strncmp(buf, ": 【", 4))
						continue;
					if (!strncmp(buf, ": : ", 4))
						continue;
					if (!strncmp(buf, "--\n", 3))
						break;
					if (buf[0] == '\n')
						continue;
					if (lines++ >= 20)
					{
						sprintf(quote,"%s: (以下引言省略...)\n",quote);
						//printf(": (以下引言省略...)\n");
						break;
					}
					if (!strcasestr(buf, "</textarea>"))
						sprintf(quote,"%s: %s",quote, buf);
						//printf(": %s", buf);
				}
				fclose(fp);
			}
		}
	return quote;
}*/
//modified by kongyang 2005-5-27
/*
void set_text()
{
	
	sprintf(buftext,"%s",getparm("text"));
}
*/

static char *
setmodulefile(char *buf,char *boardname,char *filename)//added by chaser 06.05.07(../vote/boardname/modulename..)
{
   sprintf(buf,"vote/%s/%s",boardname,filename);
   return buf;
}
static int ismoduleexist(char *board)
{
   struct stat test;
   char buf[STRLEN];
   setmodulefile(buf,board,"modulename");
   lstat(buf,&test);
   if((test.st_size/sizeof(struct module_index))<1)
        return NA;
   else
        return YEA;
}

int
updateqzdenyuser(int district, char userid[IDLEN])
{
	FILE *fp;
	int i;
	char path[80], *exp, buf[256];
	struct deny denyuser[256];
	int denynum = 0;
	sprintf(path, "boards/deny_users_%d", district);
	fp = fopen(path, "r");
	if (fp == 0)
		return 0;
	while (fgets(buf, 80, fp) != NULL)
	{
		sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
			   denyuser[denynum].exp, denyuser[denynum].autoundeny,
			   denyuser[denynum].free_time);
		denynum++;
	}
	fclose(fp);
//
	fp = fopen(path, "w");
	if (fp == 0)
		return 0;
	for (i = 0; i < denynum; i++)
	{
		int m;

		exp = denyuser[i].exp;
		if (denyuser[i].id[0] == 0)
			continue;
		for (m = 0; exp[m]; m++)
		{
			if (exp[m] <= 32 && exp[m] > 0)
				exp[m] = '.';
		}
		if(strncmp(denyuser[i].id, userid, IDLEN) != 0)
		{
			fprintf(fp, "%-12s %-40s %s %16s\n", denyuser[i].id,
				denyuser[i].exp, denyuser[i].autoundeny,
				denyuser[i].free_time);
			//denynum++;
		}
	}
	fclose(fp);
	return 0;
}

/* check whether currentuser should be undenied at bbs*/
int
seek_in_qzdenyfile(struct userec *user)
{
		char filename[STRLEN];
		char filename2[] = "boards/deny_users_";
		FILE   *fp;
		char	buf[STRLEN];
		char   *namep, *date;
		//char	  tmpbname[STRLEN];
		char	flag;
		time_t nowtime;
	
		int i = 0;
		for(i = 0; i < 18; i++)
			filename[i] = filename2[i];
		for(i = 0; i < SECNUM + 1; i ++)
		{
			if(i < 10)
			{
				filename[18] = i + 48;
				filename[19] = '\0';
			}
			else
			{
				filename[18] = i / 10 + 48;
				filename[19] = i % 10 + 48;
				filename[20] = '\0';
			}
			if ((fp = fopen(filename, "r")) == NULL)
			{
				continue;
			}
			while (fgets(buf, STRLEN, fp) != NULL)
			{
				namep = (char *) strtok(buf, ": \n\r\t");
				if (namep != NULL && ci_strcmp(namep, user->userid) == 0)
				{
					nowtime=time(0);
					getdatestring(nowtime,NA);
					flag = buf[55];
					date = (char *) (buf + 58);
					if (flag=='a' && (strncmp (datestring,date,16) >= 0))
					{
							FILE *fp;
							char path[80], title[80];
							time_t now = time(0);
							updateqzdenyuser(i,user->userid);
							struct userec *user = getuser (currentuser.userid);
							user->userlevel |= PERM_POST;
							save_user_data(user);
							currentuser = *user;
							sprintf(title, "[公告] 恢复%s在全站的POST权", user->userid);
							sprintf(path, "tmp/%d.tmp", getpid());
							fp = fopen(path, "w");
							fprintf(fp, "【本公告由自动发信系统自动张贴】\n\n");
							fprintf(fp,
								"\n  %s 网友：\n\n\t因封禁时间已过，现恢复您在全站的『发表文章』权力。\n\n"
								"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS", user->userid);
							fclose(fp);
#ifdef DENY_POST_BOARDNAME
							post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
									 "自动发信系统", "自动发信系统", -1, 0, now, now);
#endif
							post_article("syssecurity", title, path, "SJTUBBS",
									 "自动发信系统", "自动发信系统", -1, 0, now, now);
						
							post_mail(user->userid, title, path, user->userid, user->username,
								  "自动发信系统", -1);
							unlink(path);
					}
					break;
				}
			}
			fclose(fp);
		}
	
		return 0;
}

int
bbspst_main()
{
	FILE *fp;
	int i;
	char userid[80], dir[80], buf[512], path[512], file[512], board[512],
		reid_str[16], title[80] = "";
	int check=0;
	char ch[3];
	struct fileheader f;
	struct shortfile *bp;

	time_t now = time(0);
	struct tm *tm;
	tm = localtime(&now);

	init_all();
	/*buftext[0]='\0';*/
	if(cgiFormSuccess==cgiFormString("check",ch,2))
	{
		check=1;
	}
	if(check == 0)
	{
		strsncpy(board, getparm("board"), 20);
	}
	else
	{
		cgiFormString("board",board,20);
	}
	if (!loginok)
		http_quit_print("匆匆过客不能发表文章，请先登录");
	seek_in_qzdenyfile(&currentuser);
	if (!has_post_perm(&currentuser, board) && (strcmp(board,"WelcomeNewer")!= 0 || strcmp(currentuser.userid,"guest") == 0))
		http_quit_print("错误的讨论区或者您无权在此讨论区发表文章");
#ifdef	POST_AFTER_ONE_DAY
	if (too_young(&currentuser, board) && (strcmp(board,"WelcomeNewer")!= 0 || strcmp(currentuser.userid,"guest") == 0))
		http_quit_print
			("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
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
		if ((unsigned int)tm->tm_hour >= shm_bcache->all_no_post_begin ||(unsigned int) tm->tm_hour < shm_bcache->all_no_post_end)
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
	//这里取得被回复的文件名。如果是发表新文章，那么file = NULL
	if(check == 0)
	{
		strsncpy(file, getparm("file"), 20);
	}
	else
	{
		cgiFormString("file",file,20);
	}

	bzero(reid_str, 16);

	if (file[0])
	{			// Re mode.
		if (file[0] != 'M')
			http_quit_print("错误的文件名");
		sprintf(dir, "boards/%s/.DIR", getbcache(board)->filename);
		fp = fopen(dir, "r");
		if (fp == 0)
			http_quit_print("错误的目录");
		while (1)
		{
			if (fread(&f, sizeof(f), 1, fp) <= 0)
			{
				break;
			}

			if (!strcmp(f.filename, file))
			{
				char *ptr;

				sprintf(reid_str, "%d", f.reid);

				if ((f.accessed[0] & FILE_NOREPLY)
				    || (getbcache(board)->
					flag & NOREPLY_FLAG))
				{
					fclose(fp);
					http_quit_print("本文为不可回复");
				}
				ptr = f.title;
				if (!strncmp(ptr, "Re: ", 4))
					ptr += 4;
				sprintf(title, "Re: %s", ptr);
				title[64] = 0;
				strsncpy(userid, f.owner, 60);
				fclose(fp);
				goto L2;
			}
		}
		fclose(fp);
		http_quit_print("错误的文件名");
	}
      L2:
	bp = getbcache(board);
	printf("<center>\n");
	printf("<font class=\"title\">%s - 发表文章　　</font>[使用者: %s]<hr>\n", BBSNAME, currentuser.userid);
	printf("<body onkeydown=\"ctlent()\">");
	printf("<table class=show_border bgcolor=f0f0f0>\n");
	printf("<tr><td>");
	printf("<font color=green>发文注意事项: <br>\n");
	printf("本站规定同样内容的文章严禁在 4 个或 4 个以上讨论区内重复发表。详细内容参见有关站规。<br>\n");
	printf("发文时应慎重考虑文章内容是否适合公开场合发表，请勿肆意灌水。谢谢您的合作。<br>\n</font>");
	printf("本站最近已开放附件上载功能，欢迎使用。\n");
	printf("<tr><td><form name=\"form1\" method=post action=bbssnd enctype='multipart/form-data'>\n");
	printf("<input type=hidden name='board' value='%s'><input type=hidden name='file' value='%s'><input type=hidden name='reidstr' value='%s'>", board, file,reid_str);
	printf("使用标题: <input type=text name=\"title\" size=40 maxlength=100 tabindex=\"1\" value='%s'>", get_title(title));
	printf(" 讨论区: [%s]", board);
        if((bp->flag2[0] & MODULE_FLAG) && ismoduleexist(board))
        {
                printf("<a href='bbsmodule?board=%s&file=%s'>  使用模板 </a><br>\n",board,file);
        }
	else
	{
		printf("<br>");
	}
	printf(" 设置本文属性 ");
	if (bp->flag & ANONY_FLAG)
	{
		printf("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 checked>匿名");
		printf("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0>不匿名<br>\n");
	}
	if (bp->flag & POSITIVE_ANONY_FLAG)
	{
		printf("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 >匿名");
		printf("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0 checked>不匿名<br>\n");
	}
	/*
	   else{
	   printf("<input style='border: 1 solid #d0f0c0' type=radio name=allowre value=1 checked>可回复");
	   printf("<input style='border: 1 solid #d0f0c0' type=radio name=allowre value=0>不可回复<br>\n");
	   }
	 */
	printf("作者：%s", currentuser.userid);
	printf("  使用签名档 ");
	printf("<input type=radio name=signature value=1 checked>1");
	printf("<input type=radio name=signature value=2>2");
	printf("<input type=radio name=signature value=3>3");
	printf("<input type=radio name=signature value=4>4");
	printf("<input type=radio name=signature value=5>5");
	printf("<input type=radio name=signature value=0>0");
	printf(" <input type=checkbox name=autocr checked>自动换行");
	printf(" <a target=_balnk href=bbssig>查看签名档</a> ");
	printf(" <a href=bbsupload?board=%s>上传附件</a><br>", board);
	printf("<script src=\"/js/attach.js\" language=\"javascript\" type=\"text/javascript\"></script>");
	//printf("<div align=\"center\">输入完毕后，可以直接按Ctrl+Enter快速发表文章</div>\n");
	printf("\n<textarea id=text name=text rows=20 cols=80 wrap=physicle tabindex=\"2\" onkeydown=\"ctlent()\">");
/*	set_text(); */
	/*if(strcmp(getparm("url"),""))
	{
		if((p=strtok(buftext,"\n")))
		{
			printf("%s\n",p);
			while((p=strtok(NULL,"\n")))printf("%s\n",p);
			
		}
		printf("%s\n",getparm("url"));
		
	}*/
/*	printf("%s",buftext);*/
	printf("%s",getparm("text"));
	if(check==1)
	{
		char mpath[STRLEN];
		char module[90];
/*		struct module_index module[3];
		sprintf(mpath,"vote/%s/%s",board,"modulename");
		fp=fopen(mpath,"r");
		while(fread(&module[number],sizeof(struct module_index),1,fp)!=0)
  	       {
        		number++;
  		}
   		fclose(fp);*/
		cgiFormString("level",module,80);
		strcat(module,"_M");
		sprintf(mpath,"vote/%s/%s",board,module);
		fp=fopen(mpath,"r");
		printf("\n");
		if(fp==NULL) http_quit_print("无法打开文件");
		while(fgets(buf,500,fp)!=NULL)
		{
			printf("%s",buf);
		}
		fclose(fp);
	}
	if (file[0])
		{
			int lines = 0;
			
			printf("\n\n【 在 %s 的大作中提到: 】\n", userid);
			sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
			fp = fopen(path, "r");
			if(fp==NULL) http_quit_print("无法打开文件2");
			if (fp)
			{
				for (i = 0; i < 3; i++)
					if (fgets(buf, 500, fp) == 0)
						break;
				while (1)
				{
					if (fgets(buf, 500, fp) == 0)
						break;
					if (!strncmp(buf, ": 【", 4))
						continue;
					if (!strncmp(buf, ": : ", 4))
						continue;
					if (!strncmp(buf, "--\n", 3))
						break;
					if (buf[0] == '\n')
						continue;
					if (lines++ >= 20)
					{
						
						printf(": (以下引言省略...)\n");
						break;
					}
					if (!strcasestr(buf, "</textarea>"))
						
						printf(": %s", buf);
				}
				fclose(fp);
			}
		}
	    
	printf("</textarea>\n");
	printf("<tr><td class=post align=center>");
	printf("<br><input type=submit value=发表文章> ");
	printf("<input type=reset value=内容复原>\n");
	

	//printf("<table class=show_border bgcolor=f0f0f0>\n");
	//added by heack 2005-05-05
	printf("<br>上载附件: <input type=file name=up>");
    printf("<input type=hidden name=MAX_FILE_SIZE value=1048577>");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<br>个人推荐评价: ");
    printf("<input type=radio name=level value=0 checked>1星 ");
    printf("<input type=radio name=level value=1>2星 ");
    printf("<input type=radio name=level value=2>3星 ");
    printf("<input type=radio name=level value=3>4星 ");
    printf("<input type=radio name=level value=4>5星 ");
	 printf
    ("<input name=live type=hidden value=180><br>\n");
    printf
    ("<br>附件的简要说明: <input name=exp size=60 maxlength=72>\n");

    printf("<hr><br>\n");
    printf("<input type=submit value='开始上传文件'> \n");
	
	printf("</table>");
	printf("</body>");
	
    	printf("</form>\n");
    

	//added by kongyang 2005-06-06
	printf("\n<script language=\"javascript\">\n<!--\n");
	printf("function ctlent()\n");
	printf("{\n");
	printf("k = event.keyCode;	\n");
	printf("if((event.ctrlKey&&event.keyCode==13))\n");
	printf("{document.form1.submit();}\n");
	//printf("event.keyCode=0;\n");
	printf("if(event.ctrlKey&&k==87)\n");
	printf("{alert(\"为防止您不小心关闭当前窗口，故弹出此对话框\");}}\n");
	printf("-->\n");
	printf("</script>\n");
	printf("</html>");
	return 0;
}
