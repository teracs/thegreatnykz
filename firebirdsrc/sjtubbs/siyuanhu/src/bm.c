#include "bbs.h"
extern cmpbnames ();

//next func change by alt@02.04.06 for auto del denyuser
int
addtodeny (char *uident, char *msg, int ischange, int info_type)
{
    char buf[50], strtosave[256], buf2[50];
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;

    seek = SeekInNameList (uident);
    if (seek && !ischange)
    {
        move (3, 0);
        prints (" %s 已经在名单中。", uident);
        pressanykey ();
        return -1;
    }
    if (ischange && !seek)
    {
        move (3, 0);
        prints (" %s 不在名单中。", uident);
        pressreturn ();
        return -1;
    }
    while (1)
    {
        getdata (3, 0, "输入说明: ", buf, 40, DOECHO, YEA);
        if (killwordsp (buf) == 0)
            continue;
        tmp = buf;
        while (*tmp != '\0')
        {
            if (*tmp == ' ')
                *tmp = '.';
            tmp++;
        }
        break;
    }
    nowtime = time (0);
    getdatestring (nowtime, NA);
    getdata (4, 0,
             "输入封禁时间[缺省为 1 天, 0 为放弃, 手动解封在时间前加'.']: ",
             buf2, 4, DOECHO, YEA);
    if (buf2[0] == '\0')
        day = 1;
    else if (buf2[0] == '.')
    {
        fAutoUndeny = 0;
        day = atoi (buf2 + 1);
    }
    else
    {
        fAutoUndeny = 1;
        day = atoi (buf2);
    }
    if (day <= 0)
        return -1;
    if (day >= 30)
        day = 30;
    nowtime += day * 86400;
    getdatestring (nowtime, NA);
    sprintf (strtosave, "%-12s %-40s %s%16.16s点解", uident, buf,
             fAutoUndeny ? "(a) " : "(n) ", datestring);
    if (!ischange)
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t我很抱歉地告诉您，您已经被取消在 %s 板的『发文』权力。\n\n"
                     "\t您被封禁的原因是： [%s]\n\n"
                     "\t因为您的行为，决定停止您在本板的权利 [%d] 天\n\n"
                     "\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
                     uident, currboard, buf, day, datestring, currentuser.userid);
        else
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t我很抱歉地告诉您，您已经被取消在 %s 板的『发文』权力。\n\n"
                     "\t您被封禁的原因是： [%s]\n\n"
                     "\t因为您的行为，决定停止您在本板的权利 [%d] 天\n\n"
                     "\t [%d] 天后在本板发文章, 可自动解封。\n\n"
                     "\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                     uident, currboard, buf, day, day, currentuser.userid);

    }
    else
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t关于您在 %s 板被取消『发文』权力问题，现变更如下：\n\n"
                     "\t封禁的原因： [%s]\n\n"
                     "\t从现在开始，停止该权利时间： [%d] 天\n\n"
                     "\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
                     uident, currboard, buf, day, datestring, currentuser.userid);

        else
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t关于您在 %s 板被取消『发文』权力问题，现变更如下：\n\n"
                     "\t封禁的原因： [%s]\n\n"
                     "\t从现在开始，停止该权利时间： [%d] 天\n\n"
                     "\t [%d] 天后自动解封。\n\n"
                     "\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                     uident, currboard, buf, day, day, currentuser.userid);
    }
    if (info_type < 0)
    {
        if (!ischange)
        {
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t我很抱歉地告诉您，您已经被取消 在全站发文的权利\n\n"
                     "\t您被封禁的原因是： [%s]\n\n"
                     "\t因为您的行为，决定停止你在全站发文的权利 [%d] 天\n\n"
                     "\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
                     uident, buf, day, datestring, currentuser.userid);
        }
        else
        {
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t关于你全站发文权限被取消的问题，现变更如下:\n\n"
                     "\t封禁的原因： [%s]\n\n"
                     "\t从现在开始，停止该权利时间： [%d] 天\n\n"
                     "\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
                     uident, buf, day, datestring, currentuser.userid);
        }
        sprintf (genbuf, "boards/deny_users_%d", info_type * (-1));
    }
    else
        setbfile (genbuf, currboard, "deny_users");
    if (ischange)
        del_from_file (genbuf, uident);
    return add_to_file (genbuf, strtosave);
}

/* Modify End. */

int
deldeny (char *uident, int info_type)
{
    char fn[STRLEN];

    if (info_type < 0)
        sprintf (fn, "boards/deny_users_%d", info_type * (-1));
    else if (info_type == 3)
        setbfile (fn, currboard, "board.allow");
    else
        setbfile (fn, currboard, "deny_users");
    return del_from_file (fn, uident);
}

#ifdef BOARD_CONTROL
int
control_user ()
{
    char boardallow[STRLEN];
    int cnt;
	char buf[STRLEN];
	int allow_reject;
	//clear();
	//modify_user_mode(MARKET);
	move(t_lines - 1,0);
	if (askyn ("编辑好友名单/坏人名单？", YEA, NA) == YEA)
	{
    setbfile (boardallow, currboard, "board.allow");
 	allow_reject=1;
	}
	else{
    setbfile (boardallow, currboard, "board.reject");
	allow_reject=0;
	} 
	if (!HAS_PERM (PERM_SYSOP))
    {
		if (!chk_currBM (currBM)){
			
			presskeyfor("对不起, 你没有权限.",t_lines-1);
			return DONOTHING;}
		if (!dashf (boardallow))
		{
			presskeyfor("对不起, 你没有权限.",t_lines-1);
			return DONOTHING;}
    }
    while (1)
    {
        cnt = ListFileEdit (boardallow, "编辑『本板成员』名单", allow_reject,allow_reject);
        if (cnt == 0)
        {
            prints ("\n本板成员名单为空，系统将自动取消本板的成员板面属性\n");
            if (askyn ("您确定要取消本板的成员板面属性吗", NA, NA) == YEA)
            {
                unlink (boardallow);
				//add by rwzmm @ sjtubbs, security report of board control
				sprintf(buf, "删除 %s 板板面成员名单", currboard);
				securityreport(buf);
                break;
            }
        }
        else
		{
			sprintf(buf, "编辑 %s 板板面成员名单", currboard);
			securityreport (buf);
            break;
		}

    }
    return FULLUPDATE;
}

int
reject_user ()
{
    char boardreject[STRLEN];
    int cnt;
	char buf[STRLEN];


    setbfile (boardreject, currboard, "board.reject");
    if (!HAS_PERM (PERM_SYSOP))
    {
        if (!chk_currBM (currBM))
            return DONOTHING;
        if (!dashf (boardreject))
            return DONOTHING;
    }
    while (1)
    {
        cnt = ListFileEdit (boardreject, "编辑『禁入人员』名单", 1,0);
        if (cnt == 0)
        {
            prints ("\n本板成员名单为空，系统将自动取消本板的禁入人员板面属性\n");
            if (askyn ("您确定要取消本板的禁入人员板面属性吗", NA, NA) == YEA)
            {
                unlink (boardreject);
				//add by rwzmm @ sjtubbs, security report of board control
				sprintf(buf, "删除 %s 板板面禁入人员名单", currboard);
				securityreport(buf);
                break;
            }
        }
        else
		{
			sprintf(buf, "编辑 %s 板板面禁入人员名单", currboard);
			securityreport (buf);
            break;
		}

    }
    return FULLUPDATE;
}
#endif
int
deny_user ()
{
    char uident[STRLEN], ans[8], repbuf[STRLEN];
    char msgbuf[256];
    int count, info_type;
	char control_buffer[256] = {0};
    //added by hongliang on Nov. 23th, 2003 for display multiple pages of deny_users
    static int	currentpage;
    static char	tmpbname[STRLEN];

    if ( strcmp(tmpbname, currBM) )
    {
        //The current board is not the last board, so we want the beginning of the deny_users list
        currentpage = 0;
        strcpy(tmpbname, currBM);
    }

    if (!chk_currBM (currBM))
        return DONOTHING;
	if(digestmode != 0)
		return DONOTHING;
    clear ();
   /* if (check_anonyboard (currboard) == YEA && !HAS_PERM (PERM_SYSOP))
    {
        prints ("\n匿名板板主不知道发文者身份, 故不应知道该限制谁发表文章\n");
        prints ("因此设定无法 Post 的名单之权限不开放给板主使用\n");
        prints ("若有需要设定到此名单, 请向通知sysop处理\n");
        pressreturn ();
        clear ();
        return FULLUPDATE;
    }*/
    info_type = 1;
    while (1)
    {
        clear ();
        prints ("设定本板的 [禁止发文] 名单\n");
        setbfile (genbuf, currboard, "deny_users");
        //modified by hongliang
        count = listfilecontent2 (genbuf, 3, currentpage);
        if (count)
        {
            getdata (1, 0,
                     "(A)增加 (D)删除 (C)改变 (R)全部删除 [J] 下一页 [K] 上一页 or (E)离开[E]: ",
                     ans, 7, DOECHO, YEA);
        }
        else
        {
            getdata (1, 0, "(A)增加 (R)删除名单 [J] 下一页 [K] 上一页 or (E)离开 [E]: ",
                     ans, 7, DOECHO, YEA);
        }
        if (*ans == 'A' || *ans == 'a')
        {
            move (2, 0);
            usercomplete ("输入准备加入名单的使用者ID: ", uident);

			//add by pax 061006 只能封k名单内的id
			setbfile(control_buffer, currboard, "board.allow");
            if (*uident != '\0' && getuser (uident))
            {
			if (!dashf(control_buffer)||seek_in_file(control_buffer,uident))//add by pax 061004
			{
                if (addtodeny (uident, msgbuf, 0, info_type) == 1)
                {
                    sprintf (repbuf, "%s被取消在%s板的发文权限",
                             uident, currboard);
                    securityreport (repbuf);
                    if (msgbuf[0] != '\0')
                    {
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
                      if(strncmp(currboard, "BMTraining", 10) == 0)
					  {
						autoreport (repbuf, msgbuf, uident,TRAIN_ANNOUNCE_BOARDNAME);
					  }
					  else
					  {
						autoreport (repbuf, msgbuf, uident,DENY_POST_BOARDNAME);
					  }
					
#else

                        autoreport (repbuf, msgbuf, uident, NULL);
#endif

                    }

                }
            }
			else
			{
				prints ("\n此帐号不是本板成员\n");
				prints ("若有封禁此帐号, 请向通知sysop处理\n");
				pressreturn ();
				clear ();
			}
			}
        }
        else if ((*ans == 'C' || *ans == 'c'))
        {
            move (2, 0);
            namecomplete ("请输入被修改者 ID: ", uident);
            if (*uident != '\0')
            {
                if (addtodeny (uident, msgbuf, 1, info_type) == 1)
                {
                    sprintf (repbuf, "修改对%s被取消发文权限的处理", uident);
                    securityreport (repbuf);
                    if (msgbuf[0] != '\0')
                    {
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
						if(strncmp(currboard, "BMTraining", 10) == 0)
						{
							autoreport (repbuf, msgbuf, uident, TRAIN_ANNOUNCE_BOARDNAME);
						}
						else
						{
							autoreport (repbuf, msgbuf, uident, DENY_POST_BOARDNAME);
						}
#else

                        autoreport (repbuf, msgbuf, uident, NULL);
#endif

                    }

                }
            }
        }
        else if ((*ans == 'D' || *ans == 'd') && count)
        {
            move (2, 0);
            namecomplete ("请输入被取消者 ID: ", uident);
            if (uident[0] != '\0' && SeekInNameList (uident))
            {
                if (deldeny (uident, info_type))
                {
                    sprintf (repbuf, "恢复%s在%s板的发文权限",
                             uident, currboard);
                    securityreport (repbuf);
                    sprintf (msgbuf, "\n  %s 网友：\n\n"
                             "\t因封禁时间已过，现恢复您在 [%s] 板的发表文章权力。\n\n"
                             "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                             uident, currboard, currentuser.userid);
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
					if(strncmp(currboard, "BMTraining", 10) == 0)
					{
				    	autoreport (repbuf, msgbuf, uident, TRAIN_ANNOUNCE_BOARDNAME);
					}
                    else
					{
						autoreport (repbuf, msgbuf, uident, DENY_POST_BOARDNAME);
					}
#else

                    autoreport (repbuf, msgbuf, uident, NULL);
#endif

                }
            }
        }
        else if (*ans == 'R' || *ans == 'r')
        {
            setbfile (genbuf, currboard, "deny_users");
            unlink (genbuf);
            sprintf (repbuf, "删除 [%s 板禁止发文]名单", currboard);
            securityreport (repbuf);

#ifdef DENY_POST_BOARDNAME	//added by aegis@010930

            sprintf (msgbuf, "\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                     currentuser.userid);
			if (strncmp(currboard, "BMTraining", 10) == 0)
			{
				autoreport (repbuf, msgbuf, NULL, TRAIN_ANNOUNCE_BOARDNAME);
			}
            else
			{
				autoreport (repbuf, msgbuf, NULL, DENY_POST_BOARDNAME);
			}
#endif

            return FULLUPDATE;
        }
        else if ( *ans == 'J' || *ans == 'j' )
        {
            currentpage	+= 20;
        }
        else if ( *ans == 'K' || *ans == 'k' )
        {
            currentpage	-= 20;
        }
        else
            break;
    }
    clear ();
    return FULLUPDATE;
}

int
Rangefunc (int ent, struct fileheader *fileinfo, char *direct)
{
    struct fileheader fhdr;
    char annpath[512];
    char buf[STRLEN], ans[8], info[STRLEN], bname[STRLEN], dbname[STRLEN];
    char items[8][7] =
        { "保留", "文摘", "不可RE", "删除", "精华区", "暂存", "转载",
          "恢复"
        };
    int type, num1, num2, i, max = 8, full = 0;
    int fdr, ssize;
    extern int SR_BMDELFLAG;
    extern char quote_file[120], quote_title[120], quote_board[120];

    //回收站, nextline add by mudboy
    if (digestmode != 0)
        return DONOTHING;
    if (uinfo.mode != READING)
        return DONOTHING;
    if (!chk_currBM (currBM))
        return DONOTHING;
    saveline (t_lines - 1, 0);
    if (strcmp (currboard, "deleted") && strcmp (currboard, "junk"))
        max = 7;
    strcpy (info, "区段:");
    for (i = 0; i < max; i++)
    {
        sprintf (buf, " %d)%s", i + 1, items[i]);
        strcat (info, buf);
    }
    strcat (info, " [0]: ");
    getdata (t_lines - 1, 0, info, ans, 2, DOECHO, YEA);
    type = atoi (ans);
    if (type <= 0 || type > max)
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    if (digestmode && (type == 2 || type == 3 || type == 7 || type == 8)
            || digestmode > 1 && type != 5 && type != 6)
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    move (t_lines - 1, 0);
    clrtoeol ();
    prints ("区段操作: %s", items[type - 1]);
    getdata (t_lines - 1, 20, "首篇文章编号: ", ans, 6, DOECHO, YEA);
    num1 = atoi (ans);
    if (num1 > 0)
    {
        getdata (t_lines - 1, 40, "末片文章编号: ", ans, 6, DOECHO, YEA);
        num2 = atoi (ans);
    }
    if (num1 <= 0 || num2 <= 0 || num2 <= num1)
    {
        move (t_lines - 1, 60);
        prints ("操作错误...");
        egetch ();
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    if (type != 7)
    {
        sprintf (info, "区段 [%s] 操作范围 [ %d -- %d ]，确定吗",
                 items[type - 1], num1, num2);
        if (askyn (info, NA, YEA) == NA)
        {
            saveline (t_lines - 1, 1);
            return DONOTHING;
        }
    }
    if (type == 5)
    {
        FILE *fn;

        sethomefile (annpath, currentuser.userid, ".announcepath");
        if ((fn = fopen (annpath, "r")) == NULL)
        {
            presskeyfor ("对不起, 你没有设定丝路. 请先用 f 设定丝路.",
                         t_lines - 1);
            saveline (t_lines - 1, 1);
            return DONOTHING;
        }
        fscanf (fn, "%s", annpath);
        fclose (fn);
        if (!dashd (annpath))
        {
            presskeyfor ("你设定的丝路已丢失, 请重新用 f 设定.", t_lines - 1);
            saveline (t_lines - 1, 1);
            return DONOTHING;
        }
    }
    else if (type == 6)
    {
        if (askyn ("[收入暂存档模式: 一般/精简] 采用精简方式吗", YEA, YEA)
                == NA)
            full = 1;
    }
    else if (type == 7)
    {
        clear ();
        prints ("\n\n您将进行区段转载。转载范围是：[%d -- %d]\n", num1, num2);
        prints ("当前板面是：[ %s ] \n", currboard);
        if (!get_a_boardname (bname, "请输入要转贴的讨论区名称: "))
            return FULLUPDATE;
        if (!strcmp (bname, currboard) && uinfo.mode != RMAIL)
        {
            prints ("\n\n对不起，本文就在您要转载的板面上，所以无需转载。\n");
            pressreturn ();
            clear ();
            return FULLUPDATE;
        }
        if (askyn ("确定要转载吗", NA, NA) == NA)
            return FULLUPDATE;
    }
    if ((fdr = open (direct, O_RDONLY, 0)) == -1)
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    ssize = sizeof (struct fileheader);
    if (lseek (fdr, (num1 - 1) * ssize, SEEK_SET) == -1)
    {
        close (fdr);
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    while (read (fdr, &fhdr, ssize) == ssize)
    {
        if (num1 > num2)
            break;
        switch (type)
        {
        case 1:
            mark_post (num1, &fhdr, direct);
            break;
        case 2:
            digest_post (num1, &fhdr, direct);
            break;
        case 3:
            underline_post (num1, &fhdr, direct);
            break;
        case 4:
            {
                int result;

                SR_BMDELFLAG = YEA;
                result = del_post (num1, &fhdr, direct);
                SR_BMDELFLAG = NA;
                if (result == -1)
                {
                    close (fdr);
                    return DIRCHANGED;
                }
                if (result != DONOTHING)
                {
                    lseek (fdr, (-1) * ssize, SEEK_CUR);
                    num1--;
                    num2--;
                }
                break;
            }
        case 5:
            a_Import ("0Announce", currboard, &fhdr, annpath, YEA);
            break;
        case 6:
            a_Save ("0Announce", currboard, &fhdr, YEA, full);
            break;
        case 7:
            if (uinfo.mode != RMAIL)
                sprintf (genbuf, "boards/%s/%s", currboard, fhdr.filename);
            else
                sprintf (genbuf, "mail/%c/%s/%s",
                         toupper (currentuser.userid[0]), currentuser.userid,
                         fhdr.filename);
            strncpy (quote_file, genbuf, sizeof (quote_file));
            quote_file[sizeof (quote_file) - 1] = '\0';
            strcpy (quote_title, fhdr.title);
            strcpy (quote_board, currboard);
            strcpy (dbname, currboard);
            strcpy (currboard, bname);
            post_cross ('l', 0);
            strcpy (currboard, dbname);
            break;
        case 8:
            SR_BMDELFLAG = YEA;
            UndeleteArticle (num1, &fhdr, direct);
            SR_BMDELFLAG = NA;
            break;
        }
        num1++;
    }
    close (fdr);
    saveline (t_lines - 1, 1);
    return DIRCHANGED;
}

/*add by pax@syh 2005-10-9*/
/*
模版发文
模版输入和变更
*/

int
model_edit ()
{
    int aborted;
    char ans[7], buf[STRLEN], buf2[STRLEN],fname[80];
    int ch, num, confirm;
  //  extern struct BCACHE *brdshm;

   if (!chk_currBM (currBM))
        return DONOTHING;

   // modify_user_mode (MODELEDIT);//模版修改
	 modify_user_mode (ADMIN);
    clear ();
    move (1, 0);
    prints ("本板模版文件\n\n");
	setbfile (buf2, currboard, "board.model");
    sprintf (buf, "(E)编辑 (D)删除 ? [E]: ");
    getdata (4, 0, buf, ans, 2, DOECHO, YEA);
    if (ans[0] == 'D' || ans[0] == 'd')
    {
        sprintf (buf, "你确定要删除 %s 的模版文件吗", currboard);
        confirm = askyn (buf, NA, NA);
        if (confirm != 1)
        {
            prints ("取消删除行动\n");
            pressreturn ();
            clear ();
            return;
        }
        {
            char secu[STRLEN];
            sprintf (secu, "删除 %s 的模版文件", currboard);
            securityreport (secu);
        }
        unlink (buf2);
        prints ("已删除 %s 的模版文件\n", currboard);
        pressreturn ();
        clear ();
        return;
    }
    modify_user_mode (EDITSFILE);
    aborted = vedit (buf2, NA, YEA);	/* 不添加文件头, 允许修改头部信息 */
    clear ();
    if (aborted != -1)
    {
        prints ("%s 的模版文件已经更新", currboard);
        {
            char secu[STRLEN];
            sprintf (secu, "修改%s 的模版文件", currboard);
            securityreport (secu);
        }

    }
    pressreturn ();
}
/*end by pax*/