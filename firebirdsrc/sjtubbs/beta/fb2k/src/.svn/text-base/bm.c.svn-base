#include "bbs.h"

//next func change by alt@02.04.06 for auto del denyuser
int
addtodeny (char *uident, char *msg, int ischange, int info_type)
{
    char buf[50], strtosave[256], buf2[50], result[256]; //modifided by proto@sjtu 2006.10.29
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;
	int district, dist; //added by proto@sjtubbs 2006.10.29
	
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
	/* modified by rwzmm @ sjtubbs, only SPECIAL7 can manmal deny*/
    else if (buf2[0] == '.')
    {
		if(HAS_PERM(PERM_SPECIAL7))
        {
			fAutoUndeny = 0;
		}
		else
		{
			fAutoUndeny = 1;
		}
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
// modified by proto@sjtubbs to announce better denypost announcements 2006.10.29
    dist = board_in_which_dist(currboard);
	if ((dist <= 0) || ((currboard[0] == 'D') && (currboard[1] == 'i') && (currboard[2] == 's')))
		sprintf(result, "\n");
	else if ((dist <= 9) && (dist > 0))
	{
		district = dist + '0';
		sprintf(result, 
		        "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
				"\t或在 3 天内于 District%c 板提出投诉。\n\n",
		        currentuser.userid, district);
	}
	else if (dist>=10)
	{
		district = dist - 10 + 'A';
		sprintf(result, 
		        "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
				"\t或在 3 天内于 District%c 板提出投诉。\n\n",
		        currentuser.userid, district);
	}
	
    if (!ischange)
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t很抱歉地通知您，您已经被 %s 暂停在 %s 板的『发文』权限。\n\n"
                     "\t您被暂停在该板面发文权限的原因是： [%s]；\n"
                     "\t您被暂停在该板面发文权限的期限是： [%d] 天。\n"
					 "\t请您于 [%14.14s] 后向 %s 发站内信件，\n"
					 "\t申请恢复您在该板面的发文权限。\n\n"
                     "%s"
                     "\t谢谢您对本站板务工作的理解和配合！\n\n",
                     uident, currentuser.userid, currboard, buf, day, datestring, 
					 currentuser.userid, result);
        else
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t很抱歉地通知您，您已经被 %s 暂停在 %s 板的『发文』权限。\n\n"
                     "\t您被暂停在该板面发文权限的原因是： [%s]；\n"
                     "\t您被暂停在该板面发文权限的期限是： [%d] 天。\n"
                     "\t[%d] 天后，系统将自动恢复您在该板面的发文权限。\n\n"
					 "%s"
                     "\t谢谢您对本站板务工作的理解和配合！\n\n",
                     uident, currentuser.userid, currboard, buf, day, day, result);

    }
    else
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t关于您在 %s 板被暂停『发文』权限问题，现变更如下：\n\n"
                     "\t您被暂停在该板面发文权限的原因是： [%s]；\n"
                     "\t您被暂停在该板面发文权限的期限是： [%d] 天。\n"
					 "\t请您于 [%14.14s] 后向 %s 发站内信件，\n"
					 "\t申请恢复您在该板面的发文权限。\n\n"
					 "%s"
                     "\t谢谢您对本站板务工作的理解和配合！\n\n",
                     uident, currboard, buf, day, datestring, currentuser.userid, result);

        else
            sprintf (msg,
                     "\n  %s 网友: \n\n"
                     "\t关于您在 %s 板被暂停『发文』权限问题，现变更如下：\n\n"
                     "\t您被暂停在该板面发文权限的原因是： [%s]；\n"
                     "\t您被暂停在该板面发文权限的期限是： [%d] 天。\n"
                     "\t[%d] 天后，系统将自动恢复您在该板面的发文权限。\n\n"
					 "%s"
                     "\t谢谢您对本站板务工作的理解和配合！\n\n",
                     uident, currboard, buf, day, day, result);
    }
    if (info_type < 0)
    {
        if (!ischange)
        {
        	if(!fAutoUndeny)
    		{
				sprintf (msg,
						 "\n  %s 网友: \n\n"
						 "\t很抱歉地通知您，您已经被 %s 暂停在全站发文的权限。\n\n"
						 "\t您被暂停在本站发文权限的原因是： [%s]；\n"
						 "\t您被暂停在本站发文权限的期限是： [%d] 天。\n"
						 "\t请您于 [%22.22s] 之后在 sysop 板发文申请解封。\n\n"
						 "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
						 "\t或在 3 天内于 sysop 板提出投诉。\n\n"
						 "\t谢谢您对本站板务工作的理解和配合！\n\n",
						 uident, currentuser.userid, buf, day, datestring, currentuser.userid);
    		}
			else
			{
				sprintf (msg,
						 "\n  %s 网友: \n\n"
						 "\t很抱歉地通知您，您已经被 %s 暂停在全站发文的权限。\n\n"
						 "\t您被暂停在本站发文权限的原因是： [%s]；\n"
						 "\t您被暂停在本站发文权限的期限是： [%d] 天。\n"
						 "\t[%d] 天后，系统将自动恢复您在全站的发文权限。\n\n"
						 "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
						 "\t或在 3 天内于 sysop 板提出投诉。\n\n"
						 "\t谢谢您对本站板务工作的理解和配合！\n\n",
						 uident, currentuser.userid, buf, day, day, currentuser.userid);
			}
        }
        else
        {
        	if(!fAutoUndeny)
    		{
				sprintf (msg,
						 "\n  %s 网友: \n\n"
						 "\t关于您在全站发文权限被暂停的问题，现变更如下:\n\n"
						 "\t您被暂停在本站发文权限的原因是： [%s]；\n"
						 "\t您被暂停在本站发文权限的期限是： [%d] 天。\n"
						 "\t请您于 [%22.22s] 之后在 sysop 板发文申请解封。\n\n"
						 "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
						 "\t或在 3 天内于 sysop 板提出投诉。\n\n"
						 "\t谢谢您对本站板务工作的理解和配合！\n\n",
						 uident, buf, day, datestring, currentuser.userid);
    		}
			else
			{
				sprintf (msg,
						 "\n  %s 网友: \n\n"
						 "\t关于您在全站发文权限被暂停的问题，现变更如下:\n\n"
						 "\t您被暂停在本站发文权限的原因是： [%s]；\n"
						 "\t您被暂停在本站发文权限的期限是： [%d] 天。\n"
						 "\t[%d] 天后，系统将自动恢复您在全站的发文权限。\n\n"
						 "\t如果您对此封禁有异议，可向 %s 发站内信件申诉，\n"
						 "\t或在 3 天内于 sysop 板提出投诉。\n\n"
						 "\t谢谢您对本站板务工作的理解和配合！\n\n",
						 uident, buf, day, day, currentuser.userid);
			}
        }
// modify end by proto@sjtubbs 2006.10.29
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
control_user (void)
{
    char boardallow[STRLEN];
    int cnt;
	char buf[STRLEN];

    setbfile (boardallow, currboard, "board.allow");
    if (!HAS_PERM (PERM_SYSOP))
    {
        if (!chk_currBM (currBM))
            return DONOTHING;
        if (!dashf (boardallow))
            return DONOTHING;
    }
    while (1)
    {
        cnt = ListFileEdit (boardallow, "编辑『本板成员』名单", 1);
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
#endif
int
deny_user (void)
{
    char uident[STRLEN], ans[8], repbuf[STRLEN];
    char msgbuf[512]; //modified by proto@sjtubbs 2006.10.29
    int count, info_type;
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
    /*noted by pax 根据7站要求，斑竹可以在匿名板面封禁 2006-05-17
	if (check_anonyboard (currboard) == YEA && !HAS_PERM (PERM_SYSOP))
    {
        prints ("\n匿名板板主不知道发文者身份, 故不应知道该限制谁发表文章\n");
        prints ("因此设定无法 Post 的名单之权限不开放给板主使用\n");
        prints ("若有需要设定到此名单, 请向通知sysop处理\n");
        pressreturn ();
        clear ();
        return FULLUPDATE;
    }
	*/
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
            if (*uident != '\0' && getuser (uident))
            {
                if (addtodeny (uident, msgbuf, 0, info_type) == 1)
                {
                    sprintf (repbuf, "%s被暂停在%s板的发文权限",
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
        }
        else if ((*ans == 'C' || *ans == 'c'))
        {
            move (2, 0);
            namecomplete ("请输入被修改者 ID: ", uident);
            if (*uident != '\0')
            {
                if (addtodeny (uident, msgbuf, 1, info_type) == 1)
                {
                    sprintf (repbuf, "修改对%s被暂停发文权限的处理", uident);
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
            namecomplete ("请输入被暂停者 ID: ", uident);
            if (uident[0] != '\0' && SeekInNameList (uident))
            {
                if (deldeny (uident, info_type))
                {
                    sprintf (repbuf, "恢复%s在%s板的发文权限",
                             uident, currboard);
                    securityreport (repbuf);
                    sprintf (msgbuf, "\n  %s 网友：\n\n"
                             "\t因封禁时间已过，现恢复您在 [%s] 板的发表文章权限。\n\n"
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
    if ((digestmode && (type == 2 || type == 3 || type == 7 || type == 8))
            || (digestmode > 1 && type != 5 && type != 6))
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
		int opt;
		char msg[2];
		char fname[STRLEN];
		getdata(t_lines - 1, 0,"请输入操作丝路编号 1-4(默认为第一丝路):", msg, 2, DOECHO,  YEA);

		if (msg[0] == '\n' || msg[0] == '\0')
		msg[0] = '1';
		opt = atoi(msg)-1;
		if(!isdigit(msg[0]) || opt <0 || opt > 3 )
		{
			presskeyfor ("输入错误, 请按<Enter>继续...",t_lines - 1);
			saveline (t_lines - 1, 1);
			return DONOTHING;
		}
		else
		{
			if(opt==0)
			{
				sprintf(fname,".announcepath");	
			}
			else
			{
				sprintf(fname,".announcepath%d",opt);	
			}	/*end*/

			sethomefile (annpath, currentuser.userid, fname);
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
			fhdr.accessed[0] |= FILE_VISIT;  /* 将标志置位 */
			substitute_record(direct, &fhdr, sizeof(fhdr), num1);
            a_Import ("0Announce", currboard, &fhdr, annpath, YEA);
            break;
        case 6:
            a_Save ("0Announce", currboard, &fhdr, YEA, full);
            break;
        case 7:
            if (uinfo.mode != RMAIL)
                sprintf (genbuf, "boards/%s/%s/%s", currboard, getfolderstr(fhdr.filename), fhdr.filename);
            else
                sprintf (genbuf, "mail/%c/%c/%s/%s",
                         toupper (currentuser.userid[0]), toupper (currentuser.userid[1]), currentuser.userid,
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
