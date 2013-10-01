/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: bbs.c,v 1.20 2004/07/25 05:05:47 bbs Exp $
*/

#include "bbs.h"
#include <time.h>
#if defined(BSD44)
#include <stdlib.h>

#elif defined(LINUX)
/* include nothing :-) */
#else

#include <rpcsvc/rstat.h>
#endif



int     mot;
struct postheader header;
extern int numofsig;
int     continue_flag;
int     readpost;
int     digestmode;
int returnmode;
int	local_article;
struct userec currentuser;
int     usernum=0;
char    currboard[STRLEN - BM_LEN];
char    currBM[BM_LEN - 1];
int     selboard = 0;
char	someoneID[31];
char    ReadPost[STRLEN] = "";
char    ReplyPost[STRLEN] = "";
//modified by rwzmm @ sjtubbs, reid
int current_article_reid = 0;
int reply_id = 0;
//modified end

char    save_filename[STRLEN];
int     FFLL = 0;

int 	getlist(char *,char **,int, char **, int, char **, int, char **, int);
char   * filemargin();
void    report();
void    board_usage();
void    cancelpost();
void    add_crossinfo();
int     thesis_mode();
/*For read.c*/
int     auth_search_down();
int     auth_search_up();
int     t_search_down();
int     t_search_up();
int     post_search_down();
int     post_search_up();
int     thread_up();
int     thread_down();
int     deny_user();
int 	post_cross(char islocal, int mode);
#ifdef BOARD_CONTROL
int     control_user();
#endif
int     show_author();
int     SR_first_new();
int     SR_last();
int     SR_first();
int     SR_read();
int     SR_read();
int     SR_author();
int     SR_BMfunc();
int     Q_Goodbye();
int     t_friends();
int     s_msg();
int     b_notes_passwd();
//int	read_letter();
int is_web_user(struct user_info *user);

extern int SR_BMDELFLAG;
extern int numboards;
extern time_t login_start_time;
extern char BoardName[];
extern int cmpbnames();
extern int toggle1, toggle2;
extern char fromhost[];

int noreply = 0;
#ifdef MARK_X_FLAG
int markXflag = 0;
#endif
int mailtoauther = 0;

char    genbuf[1024];
char    quote_title[120], quote_board[120];
char    quote_file[120], quote_user[120];

#ifndef NOREPLY
char    replytitle[STRLEN];
#endif

struct shortfile *getbcache();
int     totalusers, usercounter;

void rengedel_autoreport (char* title, char*str, char* userid, char* sysboard);


int
check_stuffmode()
{
    //modify by mudboy
    if (uinfo.mode == RMAIL || (uinfo.mode == READING
                                && seek_in_file("etc/macroboards",currboard)))
        //&& junkboard()))
        return YEA;
    else
        return NA;
}
/* add by cf */

int topfile_post(ent,fhdr,direct)
int ent;
struct fileheader *fhdr;
char *direct;
{
    if(!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if(fhdr->accessed[0] & FILE_TOP1)
    {
        fhdr->accessed[0] &= ~FILE_TOP1;
        dele_digest_top(fhdr->filename,direct,TOPFILE_DIR);
    }
    else
    {
        struct fileheader topfile;
        char *ptr, buf[64];
        memcpy(&topfile, fhdr, sizeof(topfile));
        topfile.filename[0] = 'T';
        topfile.accessed[0] |= FILE_NOREPLY;
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s%s", buf, topfile.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] |= FILE_TOP1;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        sprintf(&genbuf[512], "%s%s", buf, fhdr->filename);

        strcpy(ptr, TOPFILE_DIR);
        if (get_topfile_num(buf, sizeof(topfile)) >= MAX_TOP)
        { //++
            //--	if (get_num_records(buf, sizeof(topfile)) >= 2*MAX_TOP) {
            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("抱歉，你的置底文章已经超过 %d 篇，无法再加入...\n", MAX_TOP);
            pressanykey();
            return PARTUPDATE;
        }
        link(&genbuf[512], genbuf);
        append_record(buf, &topfile, sizeof(topfile));
        fhdr->accessed[0] |= FILE_TOP1;
        fhdr->accessed[0] &= ~FILE_DELETED;
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);
    return PARTUPDATE;
}

/* topfile_post() */

#ifndef BBSD
void
get_load(load)
double  load[];
{
#if defined(LINUX)
    FILE   *fp;
    fp = fopen("/proc/loadavg", "r");
    if (!fp)
        load[0] = load[1] = load[2] = 0;
    else
    {
        float   av[3];
        fscanf(fp, "%g %g %g", av, av + 1, av + 2);
        fclose(fp);
        load[0] = av[0];
        load[1] = av[1];
        load[2] = av[2];
    }
#elif defined(BSD44)
    getloadavg(load, 3);
#else

    struct statstime rs;
    rstat("localhost", &rs);
    load[0] = rs.avenrun[0] / (double) (1 << 8);
    load[1] = rs.avenrun[1] / (double) (1 << 8);
    load[2] = rs.avenrun[2] / (double) (1 << 8);
#endif
}
#endif

int
set_safe_record()
{
    struct userec tmp;
    extern int ERROR_READ_SYSTEM_FILE;
    if (get_record(PASSFILE, &tmp, sizeof(currentuser), usernum) == -1)
    {
        char    buf[STRLEN];
        sprintf(buf, "Error:Read Passfile %4d %12.12s", usernum, currentuser.userid);
        report(buf);
        ERROR_READ_SYSTEM_FILE = YEA;
        abort_bbs();
        return -1;
    }
    currentuser.numposts = tmp.numposts;
    currentuser.userlevel = tmp.userlevel;
    currentuser.numlogins = tmp.numlogins;
    currentuser.stay = tmp.stay;
    return;
}

char   *
sethomepath(buf, userid)
char   *buf, *userid;
{
    sprintf(buf, "home/%c/%s", toupper(userid[0]), userid);
    return buf;
}
/*Add by SmallPig*/
//modified by rwzmm @ sjtubbs, reid

void
setqtitle(stitle)
char   *stitle;
{
    FFLL = 1;
    if (strncmp(stitle, "Re: ", 4) != 0 )
    {
        sprintf(ReplyPost, "Re: %s", stitle);
        strcpy(ReadPost, stitle);
    }
    else
    {
        strcpy(ReplyPost, stitle);
        strcpy(ReadPost, ReplyPost + 4);
    }
}

void
setqreid(reid)
unsigned int reid;
{
	FFLL = 1;
	reply_id = reid;	
}
//modified end


/* Added by old@sjtu.edu to access master of district 2001.2.1 */

/*added by aegis@010921 to modify chk_currDM()*/
//mudboy modify to WHICHDIST(char *userid) from WHICHDIST()
int WHICHDIST(char *userid)
{
    char datafile[80]= BBSHOME"/etc/DISTmaster.ini";
    FILE *fp;
    int counts=1;
    char t[256],*t1, *t2, *t3;
    fp= fopen(datafile, "r");
    if(fp== NULL)
        return 12;
    while(fgets(t, 255, fp)&& counts <= 11)
    {
        t1= strtok(t, " \t\n");
        //mudboy add follow two lines
        if(t1== NULL||t1[0]== '#')
            continue;
        if (!isalpha(t1[0]))
        {
            counts++;
            continue;
        }
        t2= strtok(NULL, " \t\n");
        if (t2!=NULL)
            t3= strtok(NULL, " \t\n");
        else
            t3=NULL;
        //mudboy modify
        //       if(t1[0]== '#'|| t1== NULL) continue;
        //       if ((t1!=NULL&&!strcasecmp(t1,currentuser.userid))||(t2!=NULL&&!strcasecmp(t2,currentuser.userid))||(t3!=NULL&&!strcasecmp(t3,currentuser.userid))){

        if ((t1!=NULL&&!strcasecmp(t1,userid))||(t2!=NULL&&!strcasecmp(t2,userid))||(t3!=NULL&&!strcasecmp(t3,userid)))
        {
            //end
            fclose(fp);
            return(counts);
        }
        counts++;
    }
    fclose(fp);
    return(counts);
}


int chk_currDM()
{
    char BoardsFile[256], category;
    int fd, total, index, Dindex, DMLevel;
    struct boardheader *buffer;
    struct stat st;

    char CateInSect[SectNum][5] =
        {"a", "bc", "d", "efg", "hij", "kl", "o", "p","qrs","tu","vw", "x"};
    /* {"a", "bcd", "efg", "hij", "kl", "mn", "op", "qr","s"};*/
    /****************************************************************************/
    /*  采用类似bbs2www的办法，如果修改了讨论区的分类，请视情况修改以下定义     */
    /*                                                                          */
    /*  分类码和分类菜单在menu.ini中设定,Firebird BBS 2.52中缺省的分类有:       */
    /*  0【站内】、C【本校】、A【求救】、E【资讯】、I【校园】、N【电脑】、R【   */
    /*  系统】、a【休闲】、e【文艺】、i【学科】、m【音乐】、n【社交】、r【运动  */
    /*  】、s【职棒】、x【谈天】、f【新闻】等类                                 */
    /****************************************************************************/

    sprintf (BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open (BoardsFile, O_RDONLY);
    if (fd == -1)
        return NA;
    (void) fstat (fd, &st);
    total = st.st_size / sizeof (struct boardheader);
    buffer = (struct boardheader *) calloc ((size_t) total, sizeof (struct boardheader));
    if (buffer == NULL)
    {
        close (fd);
        return NA;
    }
    if (read (fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close (fd);
        return NA;
    }
    close (fd);

    for (index = 0; index < total; index++)
    {
        if(!strcmp(buffer[index].filename,currboard))
            break;
    }
    if(index == total)
        return NA;

    category = buffer[index].title[0];

    for ( Dindex = 0; Dindex < 12; Dindex++ )
    {
        if (strchr (CateInSect[Dindex], category) != NULL)
            break;
    }

    if ( Dindex == 12 )
        return NA;

    //DMLevel = 4 * ( HAS_PERM(PERM_DIST100) ? 1 : 0 ) + 2 * ( HAS_PERM(PERM_DIST010) ? 1 : 0 )+ ( HAS_PERM(PERM_DIST001) ? 1 : 0 )+1;
    //        DMLevel = WHICHDIST();//changed by aegis@010921
    DMLevel = WHICHDIST(currentuser.userid);//mudboy modify
    if ( Dindex == DMLevel )
        return YEA;
    else
        return NA;
}


int
chk_currBM(BMstr)
char    BMstr[STRLEN - 1];
{
    char   *ptr;
    char    BMstrbuf[STRLEN - 1];
    //   if (in_mail) return YEA;
    if (digestmode==9)
        return NA;
    //   if (!strcmp(currboard,"deleted")||!strcmp(currboard,"junk")||!strcmp(currboard,"rangedel")||!strcmp(currboard,"syssecurity"))
    //      return NA;

    if (HAS_PERM(PERM_BLEVELS))
        return YEA;
    if ( HAS_PERM(PERM_SPECIAL7)&& chk_currDM() == YEA )
        return YEA;
    if (!HAS_PERM(PERM_BOARDS/*|PERM_PERSONAL*/))
        return NA;

    strcpy(BMstrbuf, BMstr);
    ptr = strtok(BMstrbuf, ",: ;|&()\0\n");
    while (1)
    {
        if (ptr == NULL)
            return NA;
        if (!strcmp(ptr, currentuser.userid))
            return YEA;
        ptr = strtok(NULL, ",: ;|&()\0\n");
    }
}

void
setquotefile(filepath)
char    filepath[];
{
    strcpy(quote_file, filepath);
}

char   *
sethomefile(buf, userid, filename)
char   *buf, *userid, *filename;
{
    sprintf(buf, "home/%c/%s/%s", toupper(userid[0]), userid, filename);
    return buf;
}

char   *
setuserfile(buf, filename)
char   *buf, *filename;
{
    sprintf(buf, "home/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, filename);
    return buf;
}

char   *
setbpath(buf, boardname)
char   *buf, *boardname;
{
    strcpy(buf, "boards/");
    strcat(buf, boardname);
    return buf;
}

char   *
setbdir(buf, boardname)
char   *buf, *boardname;
{
    char    dir[STRLEN];
    switch (digestmode)
    {
    case NA:
        strncpy(dir, DOT_DIR, STRLEN);
        break;
    case YEA:
        strncpy(dir, DIGEST_DIR, STRLEN);
        break;
    case 2:
        strncpy(dir, THREAD_DIR, STRLEN);
        break;
    case 3:
        strncpy(dir, MARKED_DIR, STRLEN);
        break;
    case 4:
        strncpy(dir, AUTHOR_DIR, STRLEN);
        break;
    case 9:
    case 5: /* 同作者 */
    case 6: /* 同作者 */
    case 7: /* 标题关键字 */
    }
    dir[STRLEN - 1] = '\0';
    if( digestmode == 5 || digestmode == 6)
        sprintf(buf, "boards/%s/SOMEONE.%s.DIR.%d",boardname,someoneID,digestmode-5);
    else if ( digestmode == 7 )
        sprintf(buf, "boards/%s/KEY.%s.DIR",boardname,currentuser.userid);
	else if (digestmode == 9)
	{
		if (returnmode == 1 || returnmode == 2)
			sprintf(buf, "boards/%s/SOMEONE.%s.RETURN_DIR.%d",boardname,someoneID,returnmode);	
		else if(returnmode == 3)
			sprintf(buf, "boards/%s/KEY.%s.RETURN_DIR",boardname,currentuser.userid);
		else
			sprintf(buf,"boards/%s/%s", boardname, RETURN_DIR);
	}
    else
        sprintf(buf, "boards/%s/%s", boardname, dir);
    return buf;
}

char   *
setbfile(buf, boardname, filename)
char   *buf, *boardname, *filename;
{
    sprintf(buf, "boards/%s/%s", boardname, filename);
    return buf;
}

void setmfile(char *buf,char *userid, char *filename)
{
    sprintf(buf,"mail/%c/%s/%s",toupper(userid[0]),userid, filename);
}

char *
setmdir(buf,userid)
char *buf,*userid;
{
    sprintf(buf,"mail/%c/%s/.DIR",toupper(userid[0]),userid);
    return buf;
}

int
deny_me( char * bname)
{
    char    buf[STRLEN];
    setbfile(buf, bname, "deny_users");

    //      changed by alt@02.03.09 for autoUnDeby
    //      return seek_in_file(buf, currentuser.userid);
    return seek_in_denyfile(buf, currentuser.userid);
}


//add by alt@02.03.09
int
seek_in_denyfile(filename, seekstr)
char    filename[STRLEN], seekstr[STRLEN];
{
    FILE   *fp;
    char    buf[STRLEN];
    char   *namep, *date;
    //char    tmpbname[STRLEN];
    char	flag;
    time_t nowtime;

    if ((fp = fopen(filename, "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, ": \n\r\t");
        if (namep != NULL && ci_strcmp(namep, seekstr) == 0)
        {
            fclose(fp);
            nowtime=time(0);
            getdatestring(nowtime,NA);
            flag = buf[55];
            date = (char *) (buf + 58);
            if (flag=='a' && strncmp (datestring,date,16) >= 0)
            {
                if ( del_from_file(filename, seekstr))
                {
                    char    msgbuf[STRLEN];
                    char    repbuf[STRLEN];
                    sprintf(repbuf, "恢复%s在%s板的发文权限",
                            seekstr,currboard);
                    sprintf(msgbuf, "\n  %s 网友：\n\n"
                            "\t因封禁时间已过，现恢复您在 [%s] 板的『发表文章』权力。\n\n"
                            "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                            seekstr, currboard,BBSID);
#ifdef DENY_POST_BOARDNAME
					if (strncmp(currboard, "BMTraining", 10) == 0)
					{
						autoreport(repbuf,msgbuf,seekstr,TRAIN_ANNOUNCE_BOARDNAME);
					}
					else
					{
						autoreport(repbuf,msgbuf,seekstr,DENY_POST_BOARDNAME);
					}

#else

                    autoreport(repbuf,msgbuf,seekstr,NULL);
#endif

                    return 0;
                }
            }
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/*Add by SmallPig*/
void
shownotepad()
{
    modify_user_mode(NOTEPAD);
    ansimore("etc/notepad",YEA);
    return;
}

int
uleveltochar(buf, lvl)
char   *buf;
unsigned int lvl;
{
    if (!(lvl & PERM_BASIC))
    {
        strcpy(buf, "--------- ");
        return 0;
    }
    if (lvl < PERM_DEFAULT)
    {
        strcpy(buf, "- -------- ");
        return 1;
    }
    buf[10]= '\0';
    buf[9] = (lvl & (PERM_BOARDS)) ? 'B' : ' ';
    buf[8] = (lvl & (PERM_CLOAK)) ? 'C' : ' ';
    buf[7] = (lvl & (PERM_SEECLOAK)) ? '#' : ' ';
    buf[6] = (lvl & (PERM_XEMPT)) ? 'X' : ' ';
    buf[5] = (lvl & (PERM_CHATCLOAK)) ? 'M' : ' ';
    buf[4] = (lvl & (PERM_ACCOUNTS)) ? 'A' : ' ';
    buf[3] = (lvl & (PERM_ANNOUNCE)) ? 'N' : ' ';
    buf[2] = (lvl & (PERM_OBOARDS)) ? 'O' : ' ';
    buf[1] = (lvl & (PERM_666LIFE)) ? 'D' : ' ';
    buf[0] = (lvl & (PERM_SYSOP)) ? 'S' : ' ';
    return 1;
}

void
printutitle()
{
    move(2, 0);
    prints("\033[1;44m 编 号  使用者代号   %-19s #%-4s #%-4s %8s    %-12s  \033[m\n",
#if defined(ACTS_REALNAMES)
           HAS_PERM(PERM_SYSOP) ? "真实姓名" : "使用者昵称",
#else
           "使用者昵称",
#endif
           (toggle2 == 0) ? "上站" :  "文章" ,
           (toggle2 == 0) ? "时数" :  "信件" ,
           HAS_PERM(PERM_SEEULEVELS) ? " 等  级 " : "",
           (toggle1 == 0) ? "最近光临日期" :
           (toggle1 == 1) ? "最近光临地点" : "帐号建立日期");
}


int
g_board_names(fhdrp)
struct boardheader *fhdrp;
{
    if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(fhdrp->level)
            || (fhdrp->level & PERM_NOZAP))
    {
        AddToNameList(fhdrp->filename);
    }
    return 0;
}

void
make_blist()
{
    CreateNameList();
    apply_boards(g_board_names);
}

int
Select()
{
    modify_user_mode(SELECT);
    do_select(0, NULL, genbuf);
    return 0;
}

int
junkboard()
{
    if (strstr(JUNKBOARDS, currboard))
        return 1;
    else
        return seek_in_file("etc/junkboards",currboard);
}

int
Post()
{
    if (!selboard)
    {
        prints("\n\n先用 (S)elect 去选择一个讨论区。\n");
        pressreturn();
        clear();
        return 0;
    }
#ifndef NOREPLY
    *replytitle = '\0';
#endif

    do_post();
    return 0;
}

int
Postfile(filename, nboard, posttitle, mode)
char   *filename, *nboard, *posttitle;
int     mode;
{
    char    bname[STRLEN];
    char    dbname[STRLEN];
    struct boardheader fh;
    if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, nboard) <= 0)
    {
        sprintf(bname, "%s 讨论区找不到", nboard);
        report(bname);
        return -1;
    }
    in_mail = NA;
    strcpy(quote_board, nboard);
    strcpy(dbname, currboard);
    strcpy(currboard, nboard);
    strcpy(quote_file, filename);
    strcpy(quote_title, posttitle);
    post_cross('l', mode);
    strcpy(currboard, dbname);
    return;
}

int
get_a_boardname(bname, prompt)
char   *bname, *prompt;
{
    struct boardheader fh;
    make_blist();
    namecomplete(prompt, bname);
    if (*bname == '\0')
    {
        return 0;
    }
    if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname) <= 0)
    {
        move(1, 0);
        prints("错误的讨论区名称\n");
        pressreturn();
        move(1, 0);
        return 0;
    }
    return 1;
}
/* Add by SmallPig */
int
do_cross(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    char    bname[STRLEN];
    char    dbname[STRLEN];
    char    ispost[10];
    if(digestmode)
        return DONOTHING;
    set_safe_record();
    if (!HAS_PERM(PERM_POST))
        return DONOTHING;
    if (uinfo.mode != RMAIL)
        sprintf(genbuf, "boards/%s/%s", currboard, fileinfo->filename);
    else
        sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fileinfo->filename);
    strncpy(quote_file, genbuf, sizeof(quote_file));
    quote_file[sizeof(quote_file) - 1] = '\0';
    strcpy(quote_title, fileinfo->title);

    clear();
    prints("\033[1;33m请珍惜本站资源，请在转载后删除不必要的文章，谢谢！ \n\033[1;32m并且，你不能将本文转载到本板，如果你觉得不方便的话，请与站长联系。\033[m\n\n");
    prints("您选择转载的文章是: [\033[1;33m%s\033[m]\n",quote_title);
    if(!get_a_boardname(bname, "您如果确定要转载的话，请输入要转贴的讨论区名称(取消转载请按回车): "))
    {
        return FULLUPDATE;
    }
    if(!strcmp(bname,currboard)&&uinfo.mode != RMAIL)
    {
        move(8,8);
        prints("对不起，本文就在您要转载的板面上，所以无需转载。");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    move(3, 0);
    clrtoeol();
    prints("转载 ' %s ' 到 %s 板 ", quote_title, bname);
    move(4, 0);
    getdata(4, 0, "(S)转信 (L)本站 (A)取消? [L]: ", ispost, 9, DOECHO, YEA);
    if(ispost[0]=='a'||ispost[0]=='A')
        prints("取消");
    else
    {
        strcpy(quote_board, currboard);
        strcpy(dbname, currboard);
        strcpy(currboard, bname);
        if(ispost[0]!='s'&&ispost[0]!='S')
            ispost[0]='L';
        if (post_cross(ispost[0], 0) == -1)
        {
            pressreturn();
            move(2, 0);
            strcpy(currboard, dbname);
            return FULLUPDATE;
        }
        strcpy(currboard, dbname);
        prints("\n已把文章 \'%s\' 转贴到 %s 板\n", quote_title, bname);
    }
    move(2, 0);
    pressreturn();
    return FULLUPDATE;
}

void
readtitle()
{
    struct shortfile *bp;
    int i,j,bnum,tuid;
    struct user_info uin;
    char tmp[40],bmlists[3][16];
    extern int t_cmpuids();

    char    header[STRLEN], title[STRLEN];
    char    readmode[11];
    bp = getbcache(currboard);
    memcpy(currBM, bp->BM, BM_LEN - 1);
    bnum = 0;
    for (i =0,j =0 ;currBM[i] != '\0';i++)
    {
        if( currBM[i] == ' ' )
        {
            bmlists[bnum][j] = '\0';
            bnum ++;
            j = 0;
        }
        else
            bmlists[bnum][j++] = currBM[i];
    }
    bmlists[bnum++][j] = '\0';
    if (currBM[0] == '\0' || currBM[0] == ' ')
    {
        strcpy(header, "诚征板主中");
    }
    else
    {
        strcpy(header, "板主: ");
        for(i = 0 ; i< bnum; i++)
        {
            tuid=getuser(bmlists[i]);
            search_ulist(&uin,t_cmpuids,tuid);
            if (uin.active && uin.pid && !uin.invisible )
            {
                sprintf(tmp,"\033[32m%s\033[33m ",bmlists[i]);
            }
            else if(uin.active && uin.pid && uin.invisible && (HAS_PERM(PERM_SEECLOAK)||usernum == uin.uid))
            {
                sprintf(tmp,"\033[36m%s\033[33m ",bmlists[i]);
            }
            else
                sprintf(tmp,"%s ",bmlists[i]);
            strcat(header,tmp);
        }
    }
    if (chkmail())
        //		strcpy(title, "[您有信件，按 M 看新信]");
        strcpy(title, "[您有信件，按 L 看新信]");
    else if ((bp->flag & VOTE_FLAG))
        sprintf(title, "※投票中,按 v 进入投票※");
    else
        strcpy(title, bp->title + 8);

    showtitle(header, title);
    prints("离开[\033[1;32m←\033[m,\033[1;32me\033[m] 选择[\033[1;32m↑\033[m,\033[1;32m↓\033[m] 阅读[\033[1;32m→\033[m,\033[1;32mRtn\033[m] 发表文章[\033[1;32mCtrl-P\033[m] 砍信[\033[1;32md\033[m] 备忘录[\033[1;32mTAB\033[m] 求助[\033[1;32mh\033[m]\n");
    if (digestmode == 0)
    {
        if (DEFINE(DEF_THESIS))	/* youzi 1997.7.8 */
            strcpy(readmode, "主题");
        else
            strcpy(readmode, "一般");
    }
    else if (digestmode == 1)
        strcpy(readmode, "文摘");
    else if (digestmode == 2)
        strcpy(readmode, "主题");
    else if (digestmode == 3)
        strcpy(readmode,"MARK");
    else if (digestmode == 4)
        strcpy(readmode,"原作");
    else if (digestmode == 7)
        strcpy(readmode,"标题关键字");
    //{ 回收站, add by mudboy
    else if (digestmode == 9)
        strcpy(readmode,"回收");
    //}
    if (DEFINE(DEF_THESIS) && digestmode == 0)
        prints("\033[1;37;44m 编号   %-12s %6s %-38s[%4s式看板] \033[m\n", "刊 登 者", "日  期", " 标  题", readmode);
    else if (digestmode == 5 || digestmode == 6)
        prints("\033[1;37;44m 编号   %-12s %6s %-10s (关键字: \033[32m%-12s\033[37m) [\033[33m%s\033[37m同作者阅读] \033[m\n","刊 登 者", "日  期", " 标  题", someoneID,(digestmode==5)?"模糊":"精确");
    else if (digestmode == 7 )
        prints("\033[1;37;44m 编号   %-12s %6s %-32s[%10s式看板] \033[m\n", "刊 登 者", "日  期", " 标  题", readmode);
    else
        prints("\033[1;37;44m 编号   %-12s %6s %-40s[%4s模式] \033[m\n", "刊 登 者", "日  期", " 标  题", readmode);
    clrtobot();
}

char   *
readdoent(num, ent)    // 文章列表
int     num;
struct fileheader *ent;
{
    char path[64];
    struct stat st;
    char msg[17];
    static char buf[128];
    struct shortfile *bp;
    time_t  filetime;
    char   *date;
    char   *TITLE, color[10];
    int     type;
#ifdef COLOR_POST_DATE

    struct tm *mytm;
#endif

    type = brc_unread(ent->filename) ?
           (!DEFINE(DEF_NOT_N_MASK)?'N':'+') : ' ';
    if ((ent->accessed[0] & FILE_DIGEST) /* && HAS_PERM(PERM_MARKPOST) */ )
    {
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if (ent->accessed[0] & FILE_MARKED)
    {
        switch (type)
        {
        case ' ':
            type = 'm';
            break;
        case 'N':
        case '+':
            type = 'M';
            break;
        case 'g':
            type = 'b';
            break;
        case 'G':
            type = 'B';
            break;
        }
    }
    if(ent->accessed[0] & FILE_DELETED  && chk_currBM(currBM))
    {
        if(brc_unread(ent->filename))
            type = 'X'; // deardragon 0729
        else
            type = 'x';
    }
	//add by rwzmm @ sjtubbs
	//show range deleted files in recycle bin
	if(!(ent->accessed[0] & FILE_DELETED) && (digestmode == 9) && (HAS_PERM(PERM_SPECIAL7)))
	{
		if(brc_unread(ent->filename))
			type = 'D';
		else
			type = 'd';
	}
    /*added by cf*/
    if(ent->accessed[0] & FILE_TOP1)
    {
        type='#';
    }
    /*end*/
    bp = getbcache(currboard);
    if ( ent->accessed[0] & FILE_NOREPLY || bp->flag & NOREPLY_FLAG)
        noreply = 1;
    else
        noreply = 0;
    filetime = atoi(ent->filename + 2);
    if (filetime > 740000000)
    {
        date = ctime(&filetime) + 4;
    }
    else
    {
        date = "";
    }

#ifdef COLOR_POST_DATE
    mytm = localtime(&filetime);
    strftime(buf, 5, "%w", mytm);
    sprintf(color, "\033[1;%dm", 30 + atoi(buf) + 1);
#else

    strcpy(color, "");
#endif

    /* Re-Write By Excellent */

    /* 990604.edwardc 顺便重排一下缩排 .. */

    TITLE = ent->title;

    if ( TITLE[47] )
        strcpy(TITLE + 44, "...");  /* 把多馀的 string 砍掉 */

    if (FFLL == 0)
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
            sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s%-.48s ", num, type,
                    ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
        }
        else
        {
            sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s◆ %-.45s ", num, type,
                    ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
        }
    }
    else
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
			if(reply_id != 0)
			{
				if (ent->reid == reply_id)
				{
					sprintf(buf,
							" \033[1;36m%4d\033[m %c %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
							num, type, ent->owner, color, date,
							noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s%-.48s ", num, type,
							ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
			else
			{
				if ((strncmp(ReplyPost, ent->title, 40) == 0)&& (ent->reid == reply_id))
				{
					sprintf(buf,
						" \033[1;36m%4d\033[m %c %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
						num, type, ent->owner, color, date,
						noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s%-.48s ", num, type,
						ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
        }
        else
        {
			if(reply_id != 0)
			{
				if (ent->reid == reply_id)
				{
					sprintf(buf,
						" \033[1;32m%4d\033[m %c %-12.12s %s%6.6s.%s\033[1;32m◆ %-.45s\033[m "
						,num, type, ent->owner, color, date,
						noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s◆ %-.45s ",num,type,
						ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
			else
			{
				if ((strncmp(ReadPost, ent->title, 40) == 0) && (ent->reid == reply_id))
				{
					sprintf(buf,
							" \033[1;32m%4d\033[m %c %-12.12s %s%6.6s.%s\033[1;32m◆ %-.45s\033[m "
							,num, type, ent->owner, color, date,
							noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %4d %c %-12.12s %s%6.6s\033[m %s◆ %-.45s ",num,type,
							ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
        }
    }
    /*added by cf*/

	setbdir(path,currboard);
	if((stat(path,&st)!=-1) && ((st.st_size/sizeof(struct fileheader))< num ))
	{
		ent->accessed[0] |= FILE_TOP1;
		if((ent->accessed[0]& FILE_MARKED)&&(ent->accessed[0]&FILE_DIGEST))
			strcpy(msg,"[板规]");
		else if(ent->accessed[0] & FILE_DIGEST)
			strcpy(msg,"[通知]");
		else if(ent->accessed[0] & FILE_MARKED)
			strcpy(msg,"[提示]");
		else
			strcpy(msg,"[置底]");
		msg[17]=0;
		sprintf(buf, " \033[1;31m%6s\033[m %-12.12s %s%6.6s\033[m %s◆ %-.45s ",
				msg,ent->owner, color, date," ", TITLE);
	}
    /*end*/
    noreply = 0;
    return buf;
}

char    currfile[STRLEN];

int
cmpfilename(fhdr)
struct fileheader *fhdr;
{
    if (!strncmp(fhdr->filename, currfile, STRLEN))
        return 1;
    return 0;
}

int
cmpdigestfilename(digest_name,fhdr)
char *digest_name;
struct fileheader *fhdr ;
{
    if(!strcmp(fhdr->filename,digest_name))
        return 1 ;
    return 0 ;
}     /* comapare file names for dele_digest function. Luzi 99.3.30 */

int cpyfilename( struct fileheader *fhdr )
{
    char    buf[STRLEN];
    time_t  tnow;
    struct tm *now;

    tnow = time(0);
    now = localtime(&tnow);
    sprintf(buf, "-%s", fhdr->owner);
    strncpy(fhdr->owner, buf, IDLEN + 2);
    sprintf(buf, "<< 本文被 %s 於 %d/%d %d:%02d:%02d 删除 >>",
            currentuser.userid, now->tm_mon + 1, now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec);
    strncpy(fhdr->title, buf, STRLEN);
    fhdr->filename[STRLEN - 1] = 'L';
    fhdr->filename[STRLEN - 2] = 'L';
    return 0;
}

int
read_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    char   *t;
    char    buf[512];
    int     ch;
    int     cou;
    brc_addlist(fileinfo->filename);
    if (fileinfo->owner[0] == '-')
        return FULLUPDATE;
    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
    if(!dashf(genbuf))
    {
        clear();
        move(10,30);
        prints("对不起，本文内容丢失！");
        pressanykey();
        return FULLUPDATE; //deardragon 0729
    }
    strncpy(quote_file, genbuf, sizeof(quote_file));
    strcpy(quote_board, currboard);

	//modified by rwzmm @ sjtubbs, reid
	current_article_reid = fileinfo->reid;
	//modified end.
    strcpy(quote_title, fileinfo->title);
    quote_file[255] = fileinfo->filename[STRLEN - 2];
    strcpy(quote_user, fileinfo->owner);
#ifndef NOREPLY

    ch = ansimore(genbuf, NA);
#else

    ch = ansimore(genbuf, YEA);
#endif
#ifndef NOREPLY

    move(t_lines - 1, 0);
    clrtoeol();
    if (haspostperm(currboard))
    {
        prints("\033[1;44;31m[阅读文章]  \033[33m回信 R │ 结束 Q,← │上一封 ↑│下一封 <Space>,↓│主题阅读 ^X或p \033[m");
    }
    else
    {
        prints("\033[1;44;31m[阅读文章]  \033[33m结束 Q,← │上一封 ↑│下一封 <Space>,<Enter>,↓│主题阅读 ^X 或 p \033[m");
    }

    /* Re-Write By Excellent */

    FFLL = 1;
	reply_id = fileinfo->reid;
    if (strncmp(fileinfo->title, "Re:", 3) != 0)
    {
        strcpy(ReplyPost, "Re: ");
        strncat(ReplyPost, fileinfo->title, STRLEN - 4);
        strcpy(ReadPost, fileinfo->title);
    }
    else
    {
        strcpy(ReplyPost, fileinfo->title);
        for (cou = 0; cou < STRLEN; cou++)
            ReadPost[cou] = ReplyPost[cou + 4];
    }

    refresh();
#ifndef BBSD

    sleep(1);
#endif

    if (!(ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_PGUP))
        ch = egetch();

    switch (ch)
    {
    case 'N':
    case 'Q':
    case 'n':
    case 'q':
    case KEY_LEFT:
        break;
    case ' ':
    case 'j':
    case KEY_RIGHT:
        if (DEFINE(DEF_THESIS))
        {	/* youzi */

            sread(0, 0, fileinfo);
            break;
        }
        else
            return READ_NEXT;
    case KEY_DOWN:
    case KEY_PGDN:
        return READ_NEXT;
    case KEY_UP:
    case KEY_PGUP:
        return READ_PREV;
    case 'Y':
    case 'R':
    case 'y':
    case 'r':
        {
            struct shortfile *bp;
            bp = getbcache(currboard);
            noreply = fileinfo->accessed[0] & FILE_NOREPLY
                      ||bp->flag & NOREPLY_FLAG ;
            local_article=(fileinfo->filename[STRLEN-2]=='L');
            if(!noreply || HAS_PERM(PERM_SYSOP) ||chk_currBM(currBM))
                do_reply(fileinfo->title,fileinfo->owner, fileinfo->reid);
            else
            {
                clear();
                prints("\n\n    对不起, 该文章有不可 RE 属性, 你不能回复(RE) 这篇文章.    ");
                pressreturn();
                clear();
            }
        }
        break;
    case Ctrl('R'):
                    post_reply(ent, fileinfo, direct);
        break;
    case 'g':
        digest_post(ent, fileinfo, direct);
        break;
    case Ctrl('U'):
                    sread(1, 1, fileinfo);
        break;
    case Ctrl('N'):
                    locate_the_post(fileinfo, fileinfo->title,5,0,1);
        sread(1, 0, fileinfo);
        break;
    case Ctrl('S'):
                case Ctrl('X'):
                    case 'p':		/* Add by SmallPig */
                            sread(0, 0, fileinfo);
        break;
    case Ctrl('A'):	/* Add by SmallPig */
                    //这里是在文章列表前面查询使用者的ctrl a
                    //调用了show author函数
                    clear();
        show_author(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case 'S':		/* by youzi */
        if (HAS_PERM(PERM_MESSAGE))
{
            clear();
            s_msg();
        }
        break;
    default:
        break;
    }
#endif
    return FULLUPDATE;
}

int
skip_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    brc_addlist(fileinfo->filename);
    return GOTO_NEXT;
}

int
do_select(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    char    bname[STRLEN], bpath[STRLEN];
    struct stat st;
    //nextline add by alt@sjtu
    if (digestmode==9)
        return DONOTHING;
    move(0, 0);
    clrtoeol();
    prints("选择一个讨论区 (英文字母大小写皆可)\n");
    prints("输入讨论区名 (按空白键自动搜寻): ");
    clrtoeol();

    make_blist();
    namecomplete((char *) NULL, bname);
    if(*bname == '\0')
        return FULLUPDATE;
    setbpath(bpath, bname);
    if (/* (*bname == '\0') || */(stat(bpath, &st) == -1))
    {
        move(2, 0);
        prints("不正确的讨论区.\n");
        pressreturn();
        return FULLUPDATE;
    }
    if (!(st.st_mode & S_IFDIR))
    {
        move(2, 0);
        prints("不正确的讨论区.\n");
        pressreturn();
        return FULLUPDATE;
    }
#ifdef BOARD_CONTROL
	//modified by rwzmm @sjtubbs
	//仲裁板面阅读限制
    if((!HAS_PERM(PERM_SYSOP)) || (!strncmp(bname, "Arbitrator", 10))
		|| (!strncmp(currboard, "OscarAward", 10)))
    {
        setbfile(genbuf, bname, "board.allow");
        if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid))
        {
            clear();
            move(5,10);
            prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
            pressanykey();
            return FULLUPDATE;
        }
    }
#endif
    selboard = 1;
    brc_initial(bname);
	/*add addtional security to board master,log all enter*/
	if(0==strcmp("master",currboard)) 
	{
		report("enter board master");
	}
	

    move(0, 0);
    clrtoeol();
    move(1, 0);
    clrtoeol();
    setbdir(direct, currboard);
    return NEWDIRECT;
}

void
do_acction(type)
int type;
{
    char    buf[STRLEN];
    move(t_lines - 1, 0);
    clrtoeol();
    prints("\033[1;5m系统处理标题中, 请稍候...\033[m\n");
    refresh();
    switch(type)
    {
    case 2:
        sprintf(buf, "bin/thread %s", currboard);
        system(buf);
        break;
    case 3:
    case 4:
    case 5: /* 同作者 */
    case 6: /* 同作者  精确 */
    case 7: /* 标题关键字 */
        marked_all(type-3);
        break;
    }
}

int acction_mode()
{
    int type;
    extern char currdirect[STRLEN];
    char ch[4];

    if (digestmode != NA)
    {
        if(digestmode == 5 || digestmode == 6 )
        {
            sprintf(genbuf,"boards/%s/SOMEONE.%s.DIR.%d",currboard,someoneID,digestmode-5);
            unlink(genbuf);
        }
        else if (digestmode == 7 )
        {
            sprintf(genbuf,"boards/%s/KEY.%s.DIR",currboard,currentuser.userid);
            unlink(genbuf);
        }
        digestmode = NA;
        setbdir(currdirect, currboard);
    }

    else
    {
        returnmode = 0;
		saveline(t_lines - 1, 0);
        move(t_lines - 1, 0);
        clrtoeol();
        ch[0] = '\0';
        //{ 回收站, change by mudboy
        //		        getdata(t_lines - 1, 0, "切换模式到: 1)文摘 2)同主题 3)被 m 文章 4)原作 5)同作者 6)标题关键字 [1]: ", ch, 3, DOECHO, YEA);
        if (!chk_currBM(currBM))
            getdata(t_lines - 1, 0, "切换模式到: 1)文摘 2)同主题 3)被 m 文章 4)原作 5)同作者 6)标题关键字 [1]: ", ch, 3, DOECHO, YEA);
        else
            getdata(t_lines - 1, 0, "切换模式到:1)文摘2)同主题3)被m文章4)原作5)同作者6)标题关键字9)回收站 [1]: ", ch, 3, DOECHO, YEA);
        //}
        if ( ch[0] == '\0' )
            ch[0] = '1';
        type = atoi(ch);

        //{ 回收站, change by mudboy
        //	        if (type < 1 || type > 6 ) {
        if (type < 1 || (type > 6 && (type != 9)) || ((type == 9) && (!chk_currBM(currBM))))
        {
            //}
            saveline(t_lines - 1, 1);
            return PARTUPDATE;
        }
        else if ( type == 6 )
        {
            getdata(t_lines-1,0,"您想查找的文章标题关键字: ",
                    someoneID,30,DOECHO,YEA);
            if(someoneID[0] == '\0')
            {
                saveline(t_lines-1,1);
                return PARTUPDATE;
            }
            type = 7;
        }
        else if ( type == 5 )
        {
            getdata(t_lines-1,0,"您想查找哪位网友的文章? ",someoneID,13,DOECHO,YEA);
            if(someoneID[0] == '\0' )
            {
                saveline(t_lines -1 , 1);
                return PARTUPDATE;
            }
            getdata(t_lines-1,37, "精确查找按 Y， 模糊查找请回车[Enter]",ch,2,DOECHO,YEA);
            if( ch[0] == 'y' || ch[0] == 'Y')
                type = 6;
        }

        digestmode = type;
        setbdir(currdirect, currboard);
        if( digestmode != 1 )
            do_acction(type);
        if (!dashf(currdirect))
        {
            digestmode = NA;
            setbdir(currdirect, currboard);
            return PARTUPDATE;
        }
    }
    return NEWDIRECT;
}


/*changed by cf*/
/*int dele_digest(dname, direc)*/
int dele_digest_top(dname, direc,flag)
/*end*/
char   *dname;
char   *direc;
char   *flag;	//added by cf
{
    char    digest_name[STRLEN];
    char    new_dir[STRLEN];
    char    buf[STRLEN];
    char   *ptr;
    struct fileheader fh;
    int     pos;
    strncpy(digest_name, dname,STRLEN);
    strcpy(new_dir, direc);
    if(!strcmp(flag,DIGEST_DIR))	//added by cf
        digest_name[0] = 'G';
    else if(!strcmp(flag,TOPFILE_DIR))				//cf
        digest_name[0] = 'T';	//cf
    else if(!strcmp(flag,DOT_DIR))
        digest_name[0] = 'M';

    ptr = strrchr(new_dir, '/') + 1;
    /*	strcpy(ptr, DIGEST_DIR); */	//replaced with next line cf
    strcpy(ptr, flag);
    strncpy(buf, currfile,STRLEN);
    strncpy(currfile, digest_name,STRLEN);
    pos = search_record(new_dir, &fh, sizeof(fh), cmpdigestfilename, digest_name);
    if (pos <= 0)
    {
        return;
    }
    delete_file(new_dir, sizeof(struct fileheader), pos, cmpfilename);
    strncpy(currfile, buf,STRLEN);
    *ptr = '\0';
    sprintf(buf, "%s%s", new_dir, digest_name);
    unlink(buf);
    return;
}

int
digest_post(ent, fhdr, direct)
int     ent;
struct fileheader *fhdr;
char   *direct;
{

    // 回收站, nextline add by mudboy
    //if (digestmode==9) return DONOTHING;
    if (!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if (digestmode == YEA)
        return DONOTHING;

    if (fhdr->accessed[0] & FILE_DIGEST)
    {
        fhdr->accessed[0] = (fhdr->accessed[0] & ~FILE_DIGEST);
        /*dele_digest(fhdr->filename, direct); replaced with next line*/
        dele_digest_top(fhdr->filename, direct, DIGEST_DIR); //add by cf
    }
    else
    {
        struct fileheader digest;
        char   *ptr, buf[64];
        memcpy(&digest, fhdr, sizeof(digest));
        digest.filename[0] = 'G';
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s%s", buf, digest.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        digest.accessed[0] = 0;
        sprintf(&genbuf[512], "%s%s", buf, fhdr->filename);
        link(&genbuf[512], genbuf);
        strcpy(ptr, DIGEST_DIR);
        if (get_num_records(buf, sizeof(digest)) > MAX_DIGEST)
        {
            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("抱歉，你的文摘文章已经超过 %d 篇，无法再加入...\n", MAX_DIGEST);
            pressanykey();
            return PARTUPDATE;
        }
        append_record(buf, &digest, sizeof(digest));
        fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
        fhdr->accessed[0] &= ~FILE_DELETED;
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);
    return PARTUPDATE;
}
#ifndef NOREPLY
int
do_reply(title,id, reid)
char   *title,*id;
unsigned int reid;
{
    strcpy(replytitle, title);
    post_article(currboard,id, &reid);
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int
garbage_line(str)
char   *str;
{
    int     qlevel = 0;
    while (*str == ':' || *str == '>')
    {
        str++;
        if (*str == ' ')
            str++;
        if (qlevel++ >= 1)
            return 1;
    }
    while (*str == ' ' || *str == '\t')
        str++;
    if (qlevel >= 1)
        if (strstr(str, "提到:\n") || strstr(str, ": 】\n") || strncmp(str, "==>", 3) == 0 || strstr(str, "的文章 □"))
            return 1;
    return (*str == '\n');
}
/* this is a 陷阱 for bad people to cover my program to his */

int
Origin2(text)
char    text[256];
{
    char    tmp[STRLEN];
    sprintf(tmp, ":·%s %s·[FROM:", BoardName, BBSHOST);
    if (strstr(text, tmp))
        return 1;
    else
        return 0;
}

/* When there is an old article that can be included -jjyang */
void do_quote(char *filepath, char quote_mode)
{
    FILE   *inf, *outf;
    char   *qfile, *quser;
    char    buf[256], *ptr;
    char    op;
    int     bflag, i;

    qfile = quote_file;
    quser = quote_user;
    bflag = strncmp(qfile, "mail", 4);
    outf = fopen(filepath, "w");
    if (quote_mode!='\0'&&*qfile!='\0'&&(inf=fopen(qfile, "r"))!=NULL)
    {
        op = quote_mode;
        if (op != 'N')
        {
            fgets(buf, 256, inf);
            if ((ptr = strrchr(buf, ')')) != NULL)
            {
                ptr[1] = '\0';
                if ((ptr = strchr(buf, ':')) != NULL)
                {
                    quser = ptr + 1;
                    while (*quser == ' ')
                        quser++;
                }
            }
            if (bflag)
                fprintf(outf, "\n【 在 %-.55s 的大作中提到: 】\n", quser);
            else
                fprintf(outf, "\n【 在 %-.55s 的来信中提到: 】\n", quser);
            if (op == 'A')
            {
                while (fgets(buf, 256, inf) != NULL)
                {
                    fprintf(outf, ": %s", buf);
                }
            }
            else if (op == 'R')
            {
                while (fgets(buf, 256, inf) != NULL)
                {
                    if (buf[0] == '\n')
                        break;
                }
                while (fgets(buf, 256, inf) != NULL)
                {
                    if (Origin2(buf))
                        continue;
                    fprintf(outf, "%s", buf);
                }
            }
            else
            {
                while (fgets(buf, 256, inf) != NULL)
                {
                    if (buf[0] == '\n')
                        break;
                }
                i = 0;
                while (fgets(buf, 256, inf) != NULL)
                {
                    if (strcmp(buf, "--\n") == 0)
                        break;
                    if (buf[250] != '\0')
                        strcpy(buf + 250, "\n");
                    if (!garbage_line(buf))
                    {
                        if(op == 'S' && i >= 10 )
                        {
                            fprintf(outf,": .................（以下省略）");
                            break;
                        }
                        i ++;
                        fprintf(outf, ": %s", buf);
                    }
                }
            }
        }
        fprintf(outf, "\n");
        fclose(inf);
    }
    *quote_file = '\0';
    *quote_user = '\0';
    if (!(currentuser.signature == 0 || header.chk_anony == 1))
    {
        addsignature(outf, 1);
    }
    fclose(outf);
}
/* Add by SmallPig */
void getcross(char *filepath, int mode)
{
    FILE   *inf, *of;
    char    buf[256];
    char    owner[248];
    int     count, owner_found = 0;
    time_t  now;

    now = time(0);
    inf = fopen(quote_file, "r");
    of = fopen(filepath, "w");
    if (inf == NULL || of == NULL)
    {
        report("Cross Post error");
        return;
    }
    if (mode == 0)
    {
        if (in_mail == YEA)
        {
            in_mail = NA;
            write_header(of, 1 /* 不写入 .posts */ );
            in_mail = YEA;
        }
        else
            write_header(of, 1 /* 不写入 .posts */ );
        if (fgets(buf, 256, inf) != NULL)
        {
            if(strncmp(&buf[2],"信人: ",6))
            {
                owner_found = 0;
                strcpy(owner, "Unkown User");
            }
            else
            {
                for(count=8;buf[count]!=' '&&buf[count]!='\n'&&buf[count]!='\0';count++)
                    owner[count - 8] = buf[count];
                owner[count - 8] = '\0';
                owner_found = 1;
            }
        }
        if (in_mail == YEA)
            fprintf(of, "\033[1;37m【 以下文字转载自 \033[32m%s \033[37m的信箱 】\n",
                    currentuser.userid);
        else
            fprintf(of, "\033[1;37m【 以下文字转载自 \033[32m%s \033[37m讨论区 】\n",
                    quote_board);
        if(owner_found)
        { /* Clear Post header */
            if(fgets(buf, 256, inf) != NULL)
                if (buf[0] != '\n')
                    fgets(buf, 256, inf);
            fprintf(of, "【 原文由\033[32m %s\033[37m 所发表 】\033[m\n", owner);
        }
        else
            fseek( inf, (long) 0, SEEK_SET);
    }
    else if (mode == 1)
    {
        fprintf(of,"\033[1;41;33m发信人: %s (自动发信系统), 信区: %s%*s\033[m\n",
                BBSID,quote_board,48-strlen(BBSID)-strlen(quote_board)," ");
        fprintf(of, "标  题: %s\n", quote_title);
        getdatestring(now,NA);
        fprintf(of, "发信站: %s\033[33m自动发信系统\033[m(%s)\n\n",
                BoardName, datestring);
    }
    else if (mode == 2)
    {
        write_header(of, 0 /* 写入 .posts */ );
    }
    while (fgets(buf, 256, inf) != NULL)
    {
        if ((strstr(buf,"【 以下文字转载自 ")&&strstr(buf, "讨论区 】"))
                || (strstr(buf, "【 原文由") && strstr(buf, "所发表 】")))
            continue;
        else
            fprintf(of, "%s", buf);
    }
    fclose(inf);
    fclose(of);
    *quote_file = '\0';
}


int
do_post()
{
    *quote_file = '\0';
    *quote_user = '\0';
    local_article = YEA;
    return post_article(currboard,(char *)NULL, NULL);
}
/*ARGSUSED*/
int
post_reply(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    char    uid[STRLEN];
    char    title[STRLEN];
    char   *t;
    FILE   *fp;
    if (!strcmp(currentuser.userid, "guest"))
        return DONOTHING;
    clear();
    if (check_maxmail())
    {
        pressreturn();
        return FULLUPDATE;
    }
    modify_user_mode(SMAIL);

    /* indicate the quote file/user */
    setbfile(quote_file, currboard, fileinfo->filename);
    strcpy(quote_user, fileinfo->owner);

    /* find the author */
    if (!getuser(quote_user))
    {
        genbuf[0] = '\0';
        fp = fopen(quote_file, "r");
        if (fp != NULL)
        {
            fgets(genbuf, 255, fp);
            fclose(fp);
        }
        t = strtok(genbuf, ":");
        if (strncmp(t, "发信人", 6) == 0 ||
                strncmp(t, "寄信人", 6) == 0 ||
                strncmp(t, "Posted By", 9) == 0 ||
                strncmp(t, "作  者", 6) == 0)
        {
            while (t != NULL)
            {
                t = (char *) strtok(NULL, " \r\t\n<>");
                if (t == NULL)
                    break;
                if (!invalidaddr(t))
                    break;
            }
            if (t != NULL)
                strncpy(uid, t, STRLEN);
        }
        else
        {
            prints("对不起，该帐号已经不存在。\n");
            pressreturn();
        }
    }
    else
        strcpy(uid, quote_user);
    /* make the title */
    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' ||
            fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, STRLEN - 5);

    /* edit, then send the mail */
    switch (do_send(uid, title))
    {
    case -1:
        prints("系统无法送信\n");
        break;
    case -2:
        prints("送信动作已经中止\n");
        break;
    case -3:
        prints("使用者 '%s' 无法收信\n", uid);
        break;
    case -4:
        prints("使用者 '%s' 无法收信，信箱已满\n",uid);
        break;
    default:
        prints("信件已成功地寄给原作者 %s\n", uid);
    }
    pressreturn();
    in_mail = NA;
    return FULLUPDATE;
}
/* Add by SmallPig */
int post_cross(char islocal, int mode)
{
    struct fileheader postfile;
    struct shortfile *bp;
    char    filepath[STRLEN], fname[STRLEN];
    char    buf[256], buf4[STRLEN], whopost[IDLEN + 2];
    int     fp, count;
    time_t  now;
	char tempbuffer[STRLEN];

    if (!haspostperm(currboard) && !mode)
    {
        prints("\n\n您尚无权限在 %s 板发表文章，取消转载\n", currboard);
        return -1;
    }
	setbfile(tempbuffer, currboard, "board.allow");
	if(dashf(tempbuffer)&&!seek_in_file(tempbuffer,currentuser.userid))
	{
        prints("\n\n您不是 %s 板成员，取消转载\n", currboard);
        return -1;
	}
		
    memset(&postfile, 0, sizeof(postfile));
    strncpy(save_filename, fname, STRLEN);
    now = time(0);
    sprintf(fname, "M.%d.A", now);
    if (!mode)
    {
        if (!strstr(quote_title,"(转载)"))
            sprintf(buf4,"%.70s(转载)",quote_title);
        else
            strcpy(buf4, quote_title);
    }
    else
        strcpy(buf4, quote_title);
    strncpy(save_title, buf4, STRLEN);
    setbfile(filepath, currboard, fname);
    count = 0;
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        now++;
        sprintf(fname, "M.%d.A", now);
        setbfile(filepath, currboard, fname);
        if (count++ > MAX_POSTRETRY)
            return -1;
    }
    close(fp);
    strcpy(postfile.filename, fname);
    if (mode == 1)
        strcpy(whopost, BBSID);
    else
        strcpy(whopost, currentuser.userid);
    strncpy(postfile.owner, whopost, STRLEN);
    setbfile(filepath, currboard, postfile.filename);
    bp = getbcache(currboard);
    local_article = YEA;
    if ((islocal == 'S' || islocal == 's') && (bp->flag & OUT_FLAG))
        local_article = NA;
    modify_user_mode(POSTING);
    getcross(filepath, mode);
    strncpy(postfile.title, save_title, STRLEN);

	//modified by rwzmm @ jtubbs, reid
	postfile.id = now;
	postfile.reid = now;
	   
	//modified end

    if (local_article == YEA || !(bp->flag & OUT_FLAG))
    {
        postfile.filename[STRLEN - 1] = 'L';
        postfile.filename[STRLEN - 2] = 'L';
    }
    else
    {
        postfile.filename[STRLEN - 1] = 'S';
        postfile.filename[STRLEN - 2] = 'S';
        outgo_post(&postfile, currboard);
    }
    setbdir(buf, currboard);
    if (append_record(buf, &postfile, sizeof(postfile)) == -1)
    {
        if (!mode)
        {
            sprintf(buf, "cross_posting '%s' on '%s': append_record failed!",
                    postfile.title, quote_board);
        }
        else
        {
            sprintf(buf, "Posting '%s' on '%s': append_record failed!",
                    postfile.title, quote_board);
        }
        report(buf);
        pressreturn();
        clear();
        return 1;
    }
    if (!mode)
    {
        add_crossinfo(filepath, 1);
        sprintf(buf, "cross_posted '%s' on '%s'", postfile.title, currboard);
        report(buf);
    }
    return 1;
}

void
add_crossinfo(filepath, mode)
char   *filepath;
int     mode;
{
    FILE   *fp;
    int     color;
    color = (currentuser.numlogins % 7) + 31;
    if ((fp = fopen(filepath, "a")) == NULL)
        return;
    fprintf(fp, "--\n\033[m\033[1;%2dm※ 转%s:·%s %s·[FROM: %-.20s]\033[m\n"
            ,color, (mode == 1) ? "载" : "寄", BoardName, BBSHOST, fromhost);
    fclose(fp);
    return;
}

int
show_board_notes(bname, command)
char    bname[30];
{
    char    buf[256];
    sprintf(buf, "vote/%s/notes", bname);
    if (dashf(buf))
    {
        if (command == 1)
        {
            ansimore2(buf, NA, 0, 19);
        }
        else
        {
            ansimore(buf);
        }
        return 1;
    }
    else if (dashf("vote/notes"))
    {
        if (command == 1)
        {
            ansimore2("vote/notes", NA, 0, 19);
        }
        else
        {
            ansimore("vote/notes", NA, 0, 19);
        }
        return 1;
    }
    return -1;
}

int
show_user_notes()
{
    char    buf[256];
    setuserfile(buf, "notes");
    if (dashf(buf))
    {
        ansimore(buf);
        return FULLUPDATE;
    }
    clear();
    move(10, 15);
    prints("您尚未在 InfoEdit->WriteFile 编辑个人备忘录。\n");
    pressanykey();
    return FULLUPDATE;
}


int
outgo_post(fh, board)
struct fileheader *fh;
char   *board;
{
    char    buf[256];
    sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board,
            fh->filename, header.chk_anony ? board : currentuser.userid,
            header.chk_anony ? "我是匿名天使" : currentuser.username, save_title);
    file_append("innd/out.bntp", buf);

}

//created by rwzmm @ sjtubbs. 新注册ID 24小时内不能POST
int	too_young()
{
	if(strcmp(currboard, "BBSHelp") == 0)
		return 0;

	if(strcmp(currboard, "sysop") == 0)
		return 0;

	if((time (0) - currentuser.firstlogin) > 86400)
		return 0;
	
	return 1;
}

int
post_article(char* postboard, char* mailid, unsigned int* p_reid)
{
    struct fileheader postfile;
    struct shortfile *bp;
    char    filepath[STRLEN], fname[STRLEN], buf[STRLEN];
    int     fp, aborted, count;
    time_t  now = time(0);
    static  time_t  lastposttime = 0;
    static  int  failure = 0;

    modify_user_mode(POSTING);
    //added by soff 防止灌水机 2000.10.08
    if((abs(now-lastposttime)<3 || failure >= 9999)&&
            (!HAS_PERM(PERM_SYSOP)))
    {
        clear();
        move(5,10);
        failure++;
        if(failure > 9999)
        {
            if( failure >= 10020 )
                abort_bbs();
            prints("对不起，您在被劝阻多次的情况下，仍不断试图发文。");
            move(6,10);
            prints("您目前被系统认定为灌水机，请退出后重新登陆！[%d/20]",
                   failure - 9999);
        }
        else
        {
            prints("您太辛苦了，先喝杯咖啡歇会儿，3 秒钟后再发表文章。\n");
            if( failure > 5 )
            {
                move(6,10);
                prints("您在被劝阻的情况下仍旧试图发表文章。[%d/20]",
                       failure-5);
                if (failure >= 25)
                {
                    securityreport("多次试图发表文章，被系统判定为灌水机");
                    failure = 9999;
                }
            }
            lastposttime = now;
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    if (!haspostperm(postboard))
    {
        move(5, 10);
        clrtobot();
        if (digestmode == NA)
        {
            prints("此讨论区是唯读的, 或是您尚无权限在此发表文章。");
        }
        else
        {
            prints("目前是文摘或主题模式, 不能发表文章 (按 ENTER 后再按左键可离开此模式)。");
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    }
#ifdef	POST_AFTER_ONE_DAY
	if(too_young())
	{
		move(5, 10);
        clrtobot();
		prints("您注册时间还不满24小时，暂时不能在本板发表文章。\n\n          如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
		pressreturn();
        clear();
        return FULLUPDATE;
	}
#endif
    memset(&postfile, 0, sizeof(postfile));
    clear();
    show_board_notes(postboard, 1);
    bp = getbcache(postboard);
    if(bp->flag&OUT_FLAG && replytitle[0] == '\0')
        local_article = NA;
#ifndef NOREPLY

    if (replytitle[0] != '\0')
    {
        if (strncmp(replytitle, "Re: ", 4) == 0)
            strcpy(header.title, replytitle);
        else
            sprintf(header.title, "Re: %s", replytitle);
        header.reply_mode = 1;
    }
    else
#endif

    {
        header.title[0] = '\0';
        header.reply_mode = 0;

    }
    strcpy(header.ds, postboard);
    header.postboard = YEA;
    if (post_header(&header) == YEA)
    {
        strcpy(postfile.title, header.title);
        strncpy(save_title, postfile.title, STRLEN);
        strncpy(save_filename, fname, STRLEN);
    }
    else
        return FULLUPDATE;
    now = time(0);
    lastposttime = now;
    sprintf(fname, "M.%d.A", now);
    setbfile(filepath, postboard, fname);
    count = 0;
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        now++;
        sprintf(fname, "M.%d.A", now);
        setbfile(filepath, postboard, fname);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(fp, 0644);	/* youzi 1999.1.8 */
    close(fp);
    strcpy(postfile.filename, fname);
    in_mail = NA;
    strncpy(postfile.owner, (header.chk_anony) ?
            postboard : currentuser.userid, STRLEN);
    setbfile(filepath, postboard, postfile.filename);
    modify_user_mode(POSTING);
    do_quote(filepath, header.include_mode);
    aborted = vedit(filepath, YEA, YEA);
    strncpy(postfile.title, save_title, STRLEN);
    //if ((local_article == YEA) || !(bp->flag & OUT_FLAG)) {
    if (local_article == YEA)
    {
        postfile.filename[STRLEN - 1] = 'L';
        postfile.filename[STRLEN - 2] = 'L';
    }
    else
    {
        postfile.filename[STRLEN - 1] = 'S';
        postfile.filename[STRLEN - 2] = 'S';
        outgo_post(&postfile, postboard);
    }
    /* Anony=0; *//* Inital For ShowOut Signature */
    if (aborted == -1)
    {
        unlink(filepath);
        clear();
        return FULLUPDATE;
    }
    setbdir(buf, postboard);
    if ( noreply )
    {
        postfile.accessed[0] |= FILE_NOREPLY;
        noreply = 0;
    }

	//modified by rwzmm @ sjtubbs, reid
	postfile.id = now;
	if (mailid == NULL)  
	{
		if(p_reid == NULL)//
		{
			postfile.reid = now;
			postfile.id = now;
		}
		else
		{
			postfile.reid = *p_reid;
		}
	}
	else
	{
		if(p_reid == NULL)
		{
			postfile.reid =0 ;
			postfile.id = 0;
		}
		else
		{
			postfile.reid = *p_reid;
		}
//		postfile.id = current_article_reid;
	}
	//modified end

#ifdef MARK_X_FLAG
    if( markXflag )
    {
        postfile.accessed[0] |= FILE_DELETED;
        markXflag = 0;
    }
    else
        postfile.accessed[0] &= ~FILE_DELETED;
#endif

    if( mailtoauther )
    {
        if(header.chk_anony)
            prints("对不起，您不能在匿名板使用寄信给原作者功能。");
        else if(!mail_file(filepath,mailid,postfile.title))
            prints("信件已成功地寄给原作者 %s",mailid);
        else
            prints("信件邮寄失败，%s 无法收信。",mailid);
        mailtoauther = 0;
        pressanykey();
    }
    if (append_record(buf, &postfile, sizeof(postfile)) == -1)
    {
        sprintf(buf, "posting '%s' on '%s': append_record failed!",
                postfile.title, currboard);
        report(buf);
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    brc_addlist(postfile.filename);

    sprintf(buf, "posted '%s' on '%s'", postfile.title, currboard);
    report(buf);

    if (!junkboard())
    {
        set_safe_record();
        currentuser.numposts++;
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
    }
    return FULLUPDATE;
}

int IsTheFileOwner(struct fileheader *fileinfo)
{
    char    buf[512];
    int	posttime;

    if(fileinfo->owner[0] == '-'||strstr(fileinfo->owner, "."))
        return 0;
    if(strcmp(currentuser.userid, fileinfo->owner))
        return 0;
    strcpy(buf,&(fileinfo->filename[2]));
    buf[strlen(buf)-2] = '\0';
    posttime = atoi(buf);
    if( posttime < currentuser.firstlogin )
        return 0;
    return 1;
}

int change_title(char *fname, char *title)
{
    FILE   *fp, *out;
    char    buf[256], outname[STRLEN];
    int newtitle = 0 ;

    if ((fp = fopen(fname, "r")) == NULL)
        return 0;

    sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid, uinfo.pid);
    if ((out = fopen(outname, "w")) == NULL)
        return 0;

    while ((fgets(buf, 256, fp)) != NULL)
    {
        if (!strncmp(buf, "标  题: ", 8) && newtitle ==0)
        {
            fprintf(out, "标  题: %s\n", title);
            newtitle = 1;
            continue;
        }
        fputs(buf, out);
    }
    fclose(fp);
    fclose(out);
    rename(outname, fname);
    chmod(fname, 0644);
    return;
}

int edit_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char    buf[512];
    char   *t;
    extern char currmaildir[STRLEN];
    int		isBM = 0;

    if (!in_mail)
    {
        isBM = chk_currBM(currBM)?YEA:NA;
        /* wujian 2000.8.1 */
        //if (digestmode != NA && isBM == NA ||digestmode==9) return DONOTHING;
        if (digestmode != NA && isBM == NA)
            return DONOTHING;
        /* end */
        if (isBM == NA)
        {
            struct shortfile *bp;
            if(!IsTheFileOwner(fileinfo))
                return DONOTHING;
            bp = getbcache(currboard);
			//modified by littlesword@SJTUbbs,positive_anony
            if (((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG) )&& !strcmp(fileinfo->owner, currboard))
                return DONOTHING;
        }
    }
    modify_user_mode(EDIT);
    clear();
    if (in_mail)
        strcpy(buf, currmaildir);
    else
        strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
    if (vedit(genbuf, NA, NA) == -1)
        return FULLUPDATE;
#ifdef MARK_X_FLAG

    if( markXflag )
    {
        fileinfo->accessed[0] |= FILE_DELETED;
        markXflag = 0;
    }
    else
        fileinfo->accessed[0] &= ~FILE_DELETED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
#endif

    if (!in_mail)
    {
        sprintf(genbuf, "edited post '%s' on %s", fileinfo->title, currboard);
        report(genbuf);
    }
    return FULLUPDATE;
}

int getnam(char* direct, int num, char* id)
{
    FILE *fp;
    int size;
    struct fileheader ff;
    size = sizeof(struct fileheader);
    fp=fopen(direct,"r");
    fseek(fp,(num-1)*size,SEEK_SET);
    strcpy(id,"none.");
    if (fread(&ff,size,1,fp))
        strcpy(id,ff.filename);
    fclose(fp);
}

int
edit_title(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    struct stat st;
    char    buf[STRLEN];
	int top_ent= 0;
	char dirpath[STRLEN];
	char toppath[STRLEN];
	struct stat dirst;
	int real_file_num = 0;
	int top_mode = 0;

    char id1[20], id2[20]; //added by zhch, 12.19, to fix T d T bug.
    getnam(direct,ent,id1);

    if (!in_mail && !chk_currBM(currBM))
    {
        struct shortfile *bp;
        if(digestmode != NA)
            return DONOTHING;
        if(!IsTheFileOwner(fileinfo))
            return DONOTHING;
        bp = getbcache(currboard);
		if (((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG) )&& !strcmp(fileinfo->owner, currboard))
                return DONOTHING;
    }
    /*add by cf*/
	/* modified by rwzmm @ sjtubbs*/
    if(stat(direct,&st)==-1/* || (st.st_size/sizeof(struct fileheader) < ent)*/)
        return DONOTHING;
    /*end */
	if(st.st_size/sizeof(struct fileheader) < ent)
	{	
		//modify the title of the top article
		top_mode = 1; 
		setbfile(dirpath, currboard, DOT_DIR);
		stat(dirpath, &dirst);
		real_file_num = dirst.st_size/sizeof(struct fileheader);
		top_ent = ent - real_file_num;
		move(t_lines - 1, 0);
		clrtoeol();
		if(askyn("你确认要修改置底文章标题吗", NA, NA) == NA)
			return PARTUPDATE;
	}


    strcpy(buf,fileinfo->title);
    getdata(t_lines - 1, 0, "新文章标题: ", buf, 50, DOECHO, NA);
    if(!strcmp(buf,fileinfo->title))
        return PARTUPDATE;
    check_title(buf);
    if (buf[0] != '\0')
    {
        char    tmp[STRLEN * 2], *t;
        getnam(direct, ent,id2);
        if(strcmp(id1,id2))
            return PARTUPDATE;
        strcpy(fileinfo->title, buf);
        strcpy(tmp, direct);
        if ((t = strrchr(tmp, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);
        change_title(genbuf, buf);
		if(top_mode == 0)
		{
			substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
		}
		else
		{
			setbfile(toppath, currboard, TOPFILE_DIR);
			substitute_record(toppath, fileinfo, sizeof(*fileinfo), top_ent);
		}
    }
    return PARTUPDATE;
}

int
underline_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    // 回收站, nextline add by mudboy
    // if (digestmode==9) return DONOTHING;
    if(!chk_currBM(currBM)&&!IsTheFileOwner(fileinfo))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[0] & FILE_NOREPLY)
    {
        fileinfo->accessed[0] &= ~FILE_NOREPLY;
    }
    else
    {
        fileinfo->accessed[0] |= FILE_NOREPLY;
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
makeDELETEDflag(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    // 回收站, nextline add by mudboy
    //if (digestmode==9) return DONOTHING;
    if (!(chk_currBM(currBM))||
            fileinfo->accessed[0] & (FILE_MARKED|FILE_DIGEST))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[0] & FILE_DELETED)
    {
        fileinfo->accessed[0] &= ~FILE_DELETED;
    }
    else
    {
        fileinfo->accessed[0] |= FILE_DELETED;
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
mark_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    // 回收站, nextline add by mudboy
    // if (digestmode==9) return DONOTHING;
    if (!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[0] & FILE_MARKED)
        fileinfo->accessed[0] &= ~FILE_MARKED;
    else
    {
        fileinfo->accessed[0] |= FILE_MARKED;
        fileinfo->accessed[0] &= ~FILE_DELETED;
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int del_range(int ent, struct fileheader *fileinfo, char *direct)
{
    char    num[8];
    int     inum1, inum2;
	int realdel;

	realdel = 0;

    if (uinfo.mode == READING)
    {
        if (!chk_currBM(currBM))
        {
            return DONOTHING;
        }
    }
    //回收站, nextline changed by mudboy
    if (digestmode > 1)
        return DONOTHING;
    //   if (digestmode > 1 && digestmode!=9) return DONOTHING;
    getdata(t_lines - 1, 0, "首篇文章编号: ", num, 6, DOECHO, YEA);
    inum1 = atoi(num);
    if (inum1 <= 0)
    {
        move(t_lines - 1, 50);
        prints("错误编号...");
        egetch();
        return PARTUPDATE;
    }
    getdata(t_lines - 1, 25, "末篇文章编号: ", num, 6, DOECHO, YEA);
    inum2 = atoi(num);
    if (inum2 < inum1 + 1)
    {
        move(t_lines - 1, 50);
        prints("错误区间...");
        egetch();
        return PARTUPDATE;
    }
    move(t_lines - 1, 50);
    if (askyn("确定删除", NA, NA) == YEA)
    {
        delete_range(direct, inum1, inum2, &realdel);
        fixkeep(direct, inum1, inum2);
        if (uinfo.mode == READING)
        {
			char titlebuf[128];
			char content[1024];
			char returnpath[STRLEN];
			struct stat dirst;
			int return_file_num = 0;
			
            sprintf(genbuf, "Range delete %d-%d on %s", inum1, inum2, currboard);
            securityreport(genbuf);
			setbfile(returnpath, currboard, RETURN_DIR);
			if((stat(returnpath, &dirst)) != -1)
			{
				return_file_num = dirst.st_size / (sizeof(struct fileheader));
			}
			sprintf(titlebuf, "%s在%s板区段删文 %d-%d", currentuser.userid, currboard, inum1, inum2);
			sprintf(content, "\n\t\t%s 在 %s 板实际区段删除文章 %d 篇。\n\n\t\t文章在回收站的位置：%d 篇 － %d 篇。\n", 
				currentuser.userid, currboard, realdel,  return_file_num - realdel + 1, return_file_num);
			rengedel_autoreport(titlebuf, content, NULL, TRAIN_ANNOUNCE_BOARDNAME);
        }
        else
        {
            sprintf(genbuf, "Range delete %d-%d in mailbox", inum1, inum2);
        }
        report(genbuf);
        return DIRCHANGED;
    }
    move(t_lines - 1, 50);
    clrtoeol();
    prints("放弃删除...");
    egetch();
    return PARTUPDATE;
}
int
UndeleteArticle(ent,fileinfo,direct)  /* undelete 一篇文章 Leeward 98.05.18 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char *p, buf[1024];
    char UBoard[48] = "deleted";
    char UTitle[128];
    struct fileheader UFile;
    int i;
#ifdef KEEP_DELETED_HEADER

    int kind = 0;
#endif

    FILE *fp;

    if (strcmp(currboard, "deleted")&&strcmp(currboard,"junk"))
    {
#ifdef KEEP_DELETED_HEADER
        if( fileinfo->owner[0] != '-')
            return DONOTHING;
        kind = 1;
#else

        return DONOTHING;
#endif

    }
    sprintf(buf, "boards/%s/%s", currboard, fileinfo->filename);
    fp = fopen(buf, "r");
    if (!fp)
        return DONOTHING;

    strcpy(UTitle, fileinfo->title);
    if (p = strrchr(UTitle, '-'))
    { /* create default article title */
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i --)
        {
            if (UTitle[i] != ' ')
                break;
            else
                UTitle[i] = 0;
        }
    }

    i = 0;
    while (!feof(fp) && i < 2)
    {
        fgets(buf, 1024, fp);
        if (feof(fp))
            break;
        if (strstr(buf, "发信人: ") && (p = strstr(buf, "), 信区: ")))
        {
            i ++;
            strcpy(UBoard, p + 9);
            if (p = strchr(UBoard, ' '))
                *p = 0;
            else if (p = strchr(UBoard, '\n'))
                *p = 0;
        }
        else if (strstr(buf, "标  题: "))
        {
            i ++;
            strcpy(UTitle, buf + 8);
            if (p = strchr(UTitle, '\n'))
                *p = 0;
        }
    }
    fclose(fp);

#ifdef KEEP_DELETED_HEADER

    if(kind)
    {
        strncpy(fileinfo->owner,&(fileinfo->owner[1]),IDLEN + 2 );
        strncpy(fileinfo->title,UTitle,STRLEN);
        fileinfo->filename[STRLEN - 1] = 'L';
        fileinfo->filename[STRLEN - 2] = 'L';
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        if (!strcmp(fileinfo->owner,currentuser.userid))
        {
            set_safe_record();
            if (!junkboard())
                currentuser.numposts++;
            substitute_record(PASSFILE, &currentuser,
                              sizeof(currentuser), usernum);
        }
        else if (!strstr(fileinfo->owner, ".") && BMDEL_DECREASE)
        {
            int     id;
            if (id = getuser(fileinfo->owner))
                if (!junkboard())
                {
                    lookupuser.numposts++;
                    substitute_record(PASSFILE, &lookupuser,
                                      sizeof(struct userec), id);
                }
        }
        return DIRCHANGED;
    }
#endif

    if (fileinfo->accessed[0] & FILE_FORWARDED)
    {
        if(SR_BMDELFLAG!=YEA)
        {
            clear();
            move(2,0);
            prints("本文已恢复过了\n");
            pressreturn();
        }
        return FULLUPDATE;
    }

    UFile.accessed[0] = 0;
    strcpy(UFile.owner, fileinfo->owner);
    strcpy(UFile.title, UTitle);
    strcpy(UFile.filename, fileinfo->filename);

    sprintf(buf, "/bin/cp -f boards/%s/%s boards/%s/%s",
            currboard, fileinfo->filename, UBoard, UFile.filename);
    system(buf);

    sprintf(buf, "boards/%s/.DIR", UBoard);
    append_record(buf, &UFile, sizeof(UFile));

    fileinfo->accessed[0] |= FILE_FORWARDED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    sprintf(buf,"undeleted %s's “%s” on %s", UFile.owner, UFile.title, UBoard);
    report(buf);
    if(SR_BMDELFLAG != YEA)
    {
        clear();
        move(2,0);
        prints("'%s' 已恢复到 %s 板 \n", UFile.title, UBoard);
        pressreturn();
    }
    return FULLUPDATE;
}

int del_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char    buf[512];
    char    usrid[STRLEN];
    char   *t;
    int     owned=0, fail, IScurrent=0;
    int 	posttime;
    /*changed by cf*/
    if (fileinfo->accessed[0] & (FILE_MARKED|FILE_DIGEST))
        return DONOTHING;
    // if (fileinfo->accessed[0] & (FILE_MARKED | FILE_DIGEST | FILE_TOP1)) return DONOTHING;

    /*end*/
    //回收站, nextline changed by mudboy
    //   if (digestmode>1||!strcmp(currboard,"deleted")||!strcmp(currboard,"junk"))
    if (digestmode>1 ||!strcmp(currboard,"deleted")||!strcmp(currboard,"junk")
            && digestmode!=9)
        return DONOTHING;
#ifdef SPECIALID

    if (!strcmp(currentuser.userid, SPECIALID1) || !strcmp(currentuser.userid, SPECIALID2) )
        return DONOTHING;
#endif

    if (fileinfo->owner[0] == '-')
        return DONOTHING;
    strcpy(usrid, fileinfo->owner);
    if ( !strstr(usrid, ".") )
        IScurrent = !strcmp(usrid, currentuser.userid);

    strcpy(buf,&(fileinfo->filename[2]));
    buf[strlen(buf)-2] = '\0';
    posttime = atoi(buf);
    if( !IScurrent )
    {
        owned = getuser(usrid);
        if( owned!=0 )
        {
            if(posttime < lookupuser.firstlogin)
                owned = 0;
        }
    }
    else
        owned =  posttime > currentuser.firstlogin;

    if (!chk_currBM(currBM))
    {
        struct shortfile *bp;
        if (! (owned && IScurrent))
            return DONOTHING;
        bp = getbcache(currboard);
		if (((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG) )&& !strcmp(fileinfo->owner, currboard))
                return DONOTHING;
    }
    if (!SR_BMDELFLAG)
    {
        sprintf(genbuf, "删除文章 [%-.55s]", fileinfo->title);
        if (askyn(genbuf, NA, YEA) == NA)
        {
            move(t_lines - 1, 0);
            prints("放弃删除文章...");
            clrtoeol();
            egetch();
            return PARTUPDATE;
        }
    }
    strncpy(currfile, fileinfo->filename, STRLEN);
#ifndef KEEP_DELETED_HEADER

    fail = delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
#else

    fail = update_file(direct, sizeof(struct fileheader), ent, cmpfilename,
                       cpyfilename);
#endif

    if (!fail)
    {
        //{ 回收站, add by mudboy
        if (digestmode==0)
        {
            fileinfo->accessed[0] = FILE_DELETED;
            setbfile(buf,currboard,RETURN_DIR);
            sprintf(genbuf, "%-32.32s - %s", fileinfo->title, currentuser.userid);
            strncpy(fileinfo->title, genbuf, STRLEN);
            append_record(buf,fileinfo,sizeof(struct fileheader));
        }
        else
        {
            strcpy(buf, direct);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(genbuf, "Del '%s' on '%s'", fileinfo->title, currboard);
            report(genbuf);
            cancelpost(currboard, currentuser.userid, fileinfo, owned && IScurrent);
            sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
            unlink(genbuf);
        }
        //}

        strcpy(buf, direct);
        if ((t = strrchr(buf, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "Del '%s' on '%s'", fileinfo->title, currboard);
        report(genbuf);
        /*
              cancelpost(currboard, currentuser.userid, fileinfo, owned && IScurrent);
         
              sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
         
        #ifndef KEEP_DELETED_HEADER
              unlink(genbuf);
        #endif
        	  */
        if ( !junkboard() && !digestmode)
        {
            if (owned && IScurrent)
            {
                set_safe_record();
                if (currentuser.numposts > 0 )
                    currentuser.numposts--;
                substitute_record(PASSFILE,&currentuser,sizeof(currentuser),usernum);
            }
            else if ( owned && BMDEL_DECREASE)
            {
                if (lookupuser.numposts > 0)
                    lookupuser.numposts--;
                substitute_record(PASSFILE,&lookupuser,sizeof(struct userec),owned);
            }
        }

        return DIRCHANGED;

    }
    else if(SR_BMDELFLAG)
    {
        return -1;
    }
    move(t_lines - 1, 0);
    prints("删除失败...");
    clrtoeol();
    egetch();
    return PARTUPDATE;
}

int flag_clearto(int ent, char *direct, int clearall)
{
    int fd, i;
    struct fileheader f_info;
    if(uinfo.mode != READING)
        return DONOTHING;
    if( (fd = open(direct, O_RDONLY, 0)) == -1 )
        return DONOTHING;
    for(i = 0; clearall || i < ent; i ++)
    {
        if(read(fd, &f_info, sizeof(f_info)) != sizeof(f_info))
            break;
        brc_addlist(f_info.filename);
    }
    close(fd);
    return PARTUPDATE;
}

int new_flag_clearto(int ent, struct fileheader *fileinfo, char *direct)
{
    return flag_clearto(ent, direct, NA);
}

int new_flag_clear(int ent, struct fileheader *fileinfo, char *direct)
{
    return flag_clearto(ent, direct, YEA);
}

/* Added by netty to handle post saving into (0)Announce */
int full_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return Save_post(ent, fileinfo, direct,1);
}

int part_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return Save_post(ent, fileinfo, direct,0);
}

int Save_post(int ent, struct fileheader *fileinfo, char *direct, int full)
{
    if (!chk_currBM(currBM))
        return DONOTHING;
    return (a_Save("0Announce", currboard, fileinfo, NA, full));
}
/* Added by netty to handle post saving into (0)Announce */
int Import_post(int ent, struct fileheader *fileinfo, char *direct)
{
    FILE *fn;
    char annpath[512];
    // 回收站, nextline add by mudboy
    // if (digestmode==9) return DONOTHING;
    if (!HAS_PERM(PERM_BOARDS/*|PERM_PERSONAL*/))
        return DONOTHING;
    if(fileinfo->accessed[0] & FILE_VISIT)
    {
        if(askyn("文章曾经放入了精华区, 现在还要再放入吗",YEA,YEA)==NA)
            return 1;
    }
    else
    {
        fileinfo->accessed[0] |= FILE_VISIT;  /* 将标志置位 */
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    }
    sethomefile(annpath, currentuser.userid,".announcepath");
    if((fn = fopen(annpath, "r")) == NULL )
    {
        presskeyfor("对不起, 你没有设定丝路. 请先用 f 设定丝路.",t_lines-1);
        return 1;
    }
    fscanf(fn,"%s",annpath);
    fclose(fn);
    if (!dashd(annpath))
    {
        presskeyfor("你设定的丝路已丢失, 请重新用 f 设定.",t_lines-1);
        return 1;
    }
    return (a_Import("0Announce", currboard, fileinfo, annpath, NA));
}

int
check_notespasswd()
{
    FILE   *pass;
    char    passbuf[20], prepass[STRLEN];
    char    buf[STRLEN];
    setvfile(buf, currboard, "notespasswd");
    if ((pass = fopen(buf, "r")) != NULL)
    {
        fgets(prepass, STRLEN, pass);
        fclose(pass);
        prepass[strlen(prepass) - 1] = '\0';
        getdata(2, 0, "请输入秘密备忘录密码: ", passbuf, 19, NOECHO, YEA);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return NA;
        if (!checkpasswd(prepass, passbuf))
        {
            move(3, 0);
            prints("错误的秘密备忘录密码...");
            pressanykey();
            return NA;
        }
    }
    return YEA;
}

int
show_b_secnote()
{
    char    buf[256];
    clear();
    setvfile(buf, currboard, "secnotes");
    if (dashf(buf))
    {
        if (!check_notespasswd())
            return FULLUPDATE;
        clear();
        ansimore(buf, NA);
    }
    else
    {
        move(3, 25);
        prints("此讨论区尚无「秘密备忘录」。");
    }
    pressanykey();
    return FULLUPDATE;
}

int
show_b_note()
{
    clear();
    if (show_board_notes(currboard, 2) == -1)
    {
        move(3, 30);
        prints("此讨论区尚无「备忘录」。");
        pressanykey();
    }
    return FULLUPDATE;
}

int
into_announce()
{
    char	found[STRLEN];
    if (a_menusearch("0Announce", currboard, found))
    {
        a_menu("",found,
               ((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)
                 || HAS_PERM(PERM_OBOARDS)) ? PERM_BOARDS : 0),0);
        //return FULLUPDATE;
        return MODECHANGED;
    }
    return DONOTHING;
}
/*
int into_myAnnounce()
{
        Personal("*");
        return FULLUPDATE;
}
int into_PAnnounce()
{
        Personal(NULL);
        return FULLUPDATE;
}
*/

/*
int Personal(char *userid)
{
    char    found[256], lookid[IDLEN+6];
    int     id;

    if(!userid)
    {
        clear();
        move(2, 0);
        usercomplete( "您想看谁的个人文集: " , lookid);
        if (lookid[0] == '\0')
        {
            clear();
            return 1;
        }
    }
    else
        strcpy(lookid, userid);
    if(lookid[0] == '*')
    {
        sprintf(lookid,"/%c/%s",
                toupper(currentuser.userid[0]),currentuser.userid);
    }
    else
    {
        if (!(id = getuser(lookid)))
        {
            lookid[1] = toupper(lookid[0]);
            if(lookid[1] < 'A' || lookid[1] > 'Z')
                lookid[0] = '\0';
            else
            {
                lookid[0] = '/';
                lookid[2] = '\0';
            }
        }
        else
        {
            sprintf(lookid, "/%c/%s",
                    toupper(lookupuser.userid[0]),lookupuser.userid);
        }
    }
    sprintf(found,"0Announce/groups/GROUP_0/PersonalCorpus/%s",lookid);
    if(!dashd(found))
        sprintf(found,"0Announce/groups/GROUP_0/PersonalCorpus");
    a_menu("",found,((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)
                      || HAS_PERM(PERM_OBOARDS)) ? PERM_BOARDS : 0),0);
    return 1;
}
*/


#ifdef INTERNET_EMAIL
int
forward_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_forward(ent, fileinfo, direct));
}

int
forward_u_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_u_forward(ent, fileinfo, direct));
}
#endif

extern int
    mainreadhelp();
extern int b_vote();
extern int b_results();
extern int b_vote_maintain();
extern int b_notes_edit();
extern int Rangefunc();
extern int return_del();/* 回收文章 回收站, add by mudboy*/
extern int return_range(); /* 回收站区段恢复文章, add by mudboy*/
extern int return_find();

struct one_key read_comms[] =
    {
        '_', underline_post,
        'w', makeDELETEDflag,
        'Y', UndeleteArticle,
        'r', read_post,
        'K', skip_post,
        '#', topfile_post,  //add by cf
        /*	'u', skip_post, *//* Key 'u' use to query user's info */
        'd', del_post,
        'D', del_range,
        'm', mark_post,
        'E', edit_post,
        Ctrl('G'), acction_mode,
        '`', acction_mode,
        'g', digest_post,
        'T', edit_title,
        's', do_select,
        Ctrl('C'), do_cross,
        Ctrl('P'), do_post,
        'C', new_flag_clearto,	/* 清除未读标记到当前位置 */
        'c', new_flag_clear,  /* 清全部未读标记 */
#ifdef INTERNET_EMAIL
        'F', forward_post,
        'U', forward_u_post,
        Ctrl('R'), post_reply,
#endif
        'i', full_Save_post,
        'J', part_Save_post,
        'I', Import_post,
        'R', b_results,
        'v', b_vote,
        'V', b_vote_maintain,
        'W', b_notes_edit,
        Ctrl('W'), b_notes_passwd,
        'h', mainreadhelp,
        Ctrl('J'), mainreadhelp,
        KEY_TAB, show_b_note,
        'z', show_b_secnote,
        'x', into_announce,
        //	'X', into_myAnnounce,
        //	Ctrl('X'), into_PAnnounce,
        'a', auth_search_down,
        'A', auth_search_up,
        '/', t_search_down,
        '?', t_search_up,
        '\'', post_search_down,
        '\"', post_search_up,
        ']', thread_down,
        '[', thread_up,
        Ctrl('D'), deny_user,
#ifdef BOARD_CONTROL
        Ctrl('K'), control_user,
#endif
        Ctrl('A'), show_author,
        Ctrl('N'), SR_first_new,
        'n', SR_first_new,
        '\\', SR_last,
        '=', SR_first,
        Ctrl('S'), SR_read,
        'p', SR_read,
        Ctrl('U'), SR_author,
        'b', SR_BMfunc,
        'B', Rangefunc,
        Ctrl('T'), acction_mode,
        't', thesis_mode,	/* youzi 1997.7.8 */
        '!', Q_Goodbye,
        'S', s_msg,
        'f', t_friends,
        'o', return_del,   /* 回收文章 回收站, add by mudboy*/
		'X', return_find,	
		'O', return_range,  /* 从回收站区段恢复文章, add by rwzmm @ sjtubbs */
                   '\0', NULL
               };

int
Read()
{
    char    buf[STRLEN];
    char    notename[STRLEN];
    time_t  usetime;
    struct stat st;
    extern struct BCACHE *brdshm;
    int iPos;
    if (!selboard)
    {
        move(2, 0);
        prints("请先选择讨论区\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    in_mail = NA;
    brc_initial(currboard);
    setbdir(buf, currboard);

    setvfile(notename, currboard, "notes");
    if (stat(notename, &st) != -1)
    {
        if (st.st_mtime < (time(NULL) - 7 * 86400))
        {
            utimes(notename, NULL);
            setvfile(genbuf, currboard, "noterec");
            unlink(genbuf);
        }
    }
#ifdef ALWAYS_SHOW_BRDNOTE
    if (dashf(notename))
        ansimore3(notename, YEA);
#else

    if (vote_flag(currboard, '\0', 1 /* 检查读过新的备忘录没 */ ) == 0)
    {
        if (dashf(notename))
        {
            ansimore3(notename, YEA);
            vote_flag(currboard, 'R', 1 /* 写入读过新的备忘录 */ );
        }
    }
#endif

#ifdef BOARD_CONTROL
//    if(!HAS_PERM(PERM_SYSOP)/*&&!chk_currBM(currBM)*/)
	//modified by rwzmm @ sjtubbs
	if((!HAS_PERM(PERM_SYSOP)) || (!strncmp(currboard, "Arbitrator", 10)) 
		|| (!strncmp(currboard, "OscarAward", 10)))
    {
        setbfile(notename, currboard, "board.allow");
        if(dashf(notename)&&!seek_in_file(notename,currentuser.userid))
        {
            clear();
            move(5,10);
            prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
            pressanykey();
            return 0;
        }
    }
#endif
    usetime = time(0);
    //i_read(READING, buf, readtitle, readdoent, &read_comms[0], sizeof(struct fileheader));
    //board_usage(currboard, time(0) - usetime);
    /* inboard user count  by ZV 2002.12.13 */
    for (iPos=0; iPos<MAXBOARD; iPos++)
        if (!strcmp(brdshm->bcache[iPos].filename, currboard))
        {
            brdshm->inboard[iPos]++;
            break;
        }

    /* 进入板面 */
    i_read(READING, buf, readtitle, readdoent, &read_comms[0], sizeof(struct fileheader));
    /* 退出板面 */
    brdshm->inboard[iPos]--;

    /* 统计时间 */
    board_usage(currboard, time(0) - usetime);
    brc_update();
    return 0;
}
/*Add by SmallPig*/
void
notepad()
{
    char    tmpname[STRLEN], note1[4];
    char    note[3][STRLEN - 4];
    char    tmp[STRLEN];
    FILE   *in;
    int     i, n;
    time_t  thetime = time(0);
    extern int talkrequest;


    clear();
    move(0, 0);
    prints("开始你的留言吧！大家正拭目以待....\n");
    modify_user_mode(WNOTEPAD);
    sprintf(tmpname, "tmp/notepad.%s.%05d", currentuser.userid, uinfo.pid);
    if ((in = fopen(tmpname, "w")) != NULL)
    {
        for (i = 0; i < 3; i++)
            memset(note[i], 0, STRLEN - 4);
        while (1)
        {
            for (i = 0; i < 3; i++)
            {
                getdata(1 + i, 0, ": ", note[i], STRLEN - 5, DOECHO, NA);
                if (note[i][0] == '\0')
                    break;
            }
            if (i == 0)
            {
                fclose(in);
                unlink(tmpname);
                return;
            }
            getdata(5, 0, "是否把你的大作放入留言板 (Y)是的 (N)不要 (E)再编辑 [Y]: ", note1, 3, DOECHO, YEA);
            if (note1[0] == 'e' || note1[0] == 'E')
                continue;
            else
                break;
        }
        if (note1[0] != 'N' && note1[0] != 'n')
        {
            sprintf(tmp, "\033[1;32m%s\033[37m（%.18s）", currentuser.userid, currentuser.username);

            fprintf(in, "\033[1;34m▕\033[44m▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔\033[36m酸\033[32m甜\033[33m苦\033[31m辣\033[37m板\033[34m▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔\033[44m◣\033[m\n");
            getdatestring(thetime,NA);
            fprintf(in, "\033[1;34m▕\033[32;44m %-44s\033[32m在 \033[36m%23.23s\033[32m 离开时留下的话  \033[m\n", tmp, datestring+6);
            for (n = 0; n < i; n++)
            {
                if (note[n][0] == '\0')
                    break;
                fprintf(in, "\033[1;34m▕\033[33;44m %-75.75s\033[1;34m\033[m \n", note[n]);
            }
            fprintf(in,  "\033[1;34m▕\033[44m ───────────────────────────────────── \033[m \n");
            catnotepad(in, "etc/notepad");
            fclose(in);
            rename(tmpname, "etc/notepad");
        }
        else
        {
            fclose(in);
            unlink(tmpname);
        }
    }
    if (talkrequest)
    {
        talkreply();
    }
    clear();
    return;
}
/* youzi quick goodbye */
int
Q_Goodbye()
{
    extern int started;
    extern int *zapbuf;
    char    fname[STRLEN];
    //	char	title[STRLEN];
    int     logouts;

    free(zapbuf);

    setuserfile(fname, "msgfile");


#ifdef LOG_MY_MESG

    unlink(fname);
    setuserfile(fname, "msgfile.me");
#endif

    /* edwardc.990423 讯息浏览器 */
    if (dashf(fname))
    {
        clear();
        mesgmore(fname, NA, 0, 9999);
    }
    clear();
    prints("\n\n\n\n");

    setuserfile(fname, "notes");
    if (dashf(fname))
        ansimore(fname, YEA);

    setuserfile(fname, "logout");
    if (dashf(fname))
    {
        logouts = countlogouts(fname);
        if (logouts >= 1)
        {
            user_display(fname,
                         (logouts == 1) ? 1 : (currentuser.numlogins % (logouts)) + 1, YEA);
        }
    }
    else
    {
        if (fill_shmfile(2, "etc/logout", "GOODBYE_SHMKEY"))
            show_goodbyeshm();
    }
    pressanykey();
    report("exit");
    /*********************************** remove by alt@
    	if( count_user()<=1 && strcmp(currentuser.userid, "guest"))
    	{
    		setuserfile(fname, "msgfile");
    #ifdef LOG_MY_MESG
    		unlink(fname);
    		setuserfile(fname, "msgfile.me");
    #endif
    		if(DEFINE(DEF_NOTMSGFRIEND) && dashf(fname))
    		{
    			move(23,0);
    			clrtoeol();
    			if( askyn("是否将此次收到的讯息寄回信箱", YEA, NA) == YEA )
    			{
    #ifdef TIMECOUNTER
    				getnowdatestring();
    #else
    				getdatestring(time(0), NA);
    #endif
    				sprintf( title, "[%14.14s]所有讯息备份", datestring );
    				mail_file(fname, currentuser.userid, title);
    			}
    		}
    		unlink(fname);
    	}
    **********************************************************/
    if (started)
    {
        time_t  stay;
        stay = time(0) - login_start_time;
        sprintf(genbuf, "Stay:%3ld (%s)", stay / 60, currentuser.username);
        log_usies("EXIT ", genbuf);
        u_exit();
    }
    sleep(1);
#ifndef BBSD

    reset_tty();
#endif

    exit(0);
    return -1;
}

int
Goodbye()
{
    char    sysoplist[20][15], syswork[20][21], buf[STRLEN];
    int     i, num_sysop, choose;
    FILE   *sysops;
    char	*ptr;
    *quote_file = '\0';
    i = 0;
    if ((sysops = fopen("etc/sysops", "r")) != NULL)
    {
        while (fgets(buf, STRLEN, sysops) != NULL && i <= 19)
        {
            if(buf[0]=='#')
                continue;
            ptr = strtok(buf," \n\r\t");
            if(ptr)
            {
                strncpy(sysoplist[i],ptr,14);
                ptr = strtok(NULL, " \n\r\t");
                if(ptr)
                {
                    strncpy(syswork[i],ptr,20);
                }
                else
                    strcpy(syswork[i],"[职务不明]");
                i ++;
            }
        }
        fclose(sysops);
    }

    num_sysop = i;
    move(1, 0);
    alarm(0);
    clear();
    move(0, 0);
    prints("你就要离开 %s ，可有什麽建议吗？\n", BoardName);
    prints("[\033[1;33m1\033[m] 寄信给管理人员\n");
    prints("[\033[1;33m2\033[m] 按错了啦，我还要玩\n");
#ifdef USE_NOTEPAD

    if (strcmp(currentuser.userid, "guest") != 0)
    {
        prints("[\033[1;33m3\033[m] 写写\033[1;32m留\033[33m言\033[35m板\033[m罗\n");
    }
#endif
    prints("[\033[1;33m4\033[m]\033[1;32m 给朋友发个消息 ;)\033[m\n");
    prints("[\033[1;33m5\033[m] 不寄罗，要离开啦\n");
    sprintf(buf, "你的选择是 [\033[1;32m5\033[m]：");
    getdata(8, 0, buf, genbuf, 4, DOECHO, YEA);
    clear();
    choose = genbuf[0] - '0';
    if (choose == 1)
    {
        prints("     站长的 ID    负 责 的 职 务\n");
        prints("     ============ =====================\n");
        for (i = 1; i <= num_sysop; i++)
        {
            prints("[\033[1;33m%2d\033[m] %-12s %s\n", i, sysoplist[i - 1]
                   ,syswork[i - 1]);
        }
        prints("[\033[1;33m%2d\033[m] 还是走了罗！\n",num_sysop + 1);
        sprintf(buf, "你的选择是 [\033[1;32m%d\033[m]：", num_sysop + 1);
        getdata(num_sysop + 5, 0, buf, genbuf, 4, DOECHO, YEA);
        choose = atoi(genbuf);
        if (choose >= 1 && choose <= num_sysop)
            do_send(sysoplist[choose - 1], "使用者寄来的建议信");
        choose = -1;
    }
    if (choose == 2)
        return FULLUPDATE;
    if (strcmp(currentuser.userid, "guest") != 0)
    {
#ifdef USE_NOTEPAD
        if (HAS_PERM(PERM_POST) && choose == 3)
            notepad();
#endif

        if( choose == 4)
            friend_wall();
    }
    return Q_Goodbye();

}

void
do_report(filename,s)
char   *s,*filename;
{
    char    buf[512];
    time_t  dtime;
    time(&dtime);
    getdatestring(dtime,NA);
    sprintf(buf, "%s %16.16s %s\n", currentuser.userid, datestring+6, s);
    file_append(filename, buf);
}
void
report(s)
char	*s;
{
    do_report("trace",s);
}

void
gamelog(s)
char 	*s;
{
    do_report("game/trace",s);
}

void
board_usage(mode, usetime)
char   *mode;
time_t  usetime;
{
    time_t  now;
    char    buf[256];
    now = time(0);
    getdatestring(now,NA);
    sprintf(buf, "%.22s USE %-20.20s Stay: %5ld (%s)\n", datestring, mode, usetime, currentuser.userid);
    file_append("use_board", buf);
}


int
Info()
{
    modify_user_mode(XMENU);
    ansimore("Version.Info", YEA);
    clear();
    return 0;
}

int
Conditions()
{
    modify_user_mode(XMENU);
    ansimore("COPYING", YEA);
    clear();
    return 0;
}

int
Welcome()
{
    char    ans[3];
    modify_user_mode(XMENU);
    if (!dashf("Welcome2"))
        ansimore("Welcome", YEA);
    else
    {
        clear();
        stand_title("观看进站画面");
        for (;;)
        {
            getdata(1, 0, "(1)特殊进站公布栏  (2)本站进站画面 ? : ",
                    ans, 2, DOECHO, YEA);

            /* skyo.990427 modify  按 Enter 跳出  */
            if (ans[0] == '\0')
            {
                clear();
                return 0;
            }
            if (ans[0] == '1' || ans[0] == '2')
                break;
        }
        if (ans[0] == '1')
            ansimore("Welcome", YEA);
        else
            ansimore("Welcome2", YEA);
    }
    clear();
    return 0;
}

int
cmpbnames(bname, brec)
char   *bname;
struct fileheader *brec;
{
    if (!ci_strncmp(bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

// 回收站, next func changed by mudboy
void cancelpost(char *board, char *userid, struct fileheader *fh, int owned)
{
    struct fileheader postfile;
    FILE   *fin, *fout;
    char    from[STRLEN], path[STRLEN];
    char    fname[STRLEN], *ptr, *brd;
    int     len, count;
    time_t  now;

    setbfile(genbuf, board, fh->filename);
    if ((fin = fopen(genbuf, "r")) != NULL)
    {
#ifdef BACK_D_TO_RANGEDEL //added by aegis@1028
        if (owned==BACK_D_TO_RANGEDEL)
            //                   brd="rangedel";
            brd= (digestmode==9 && (fh->accessed[0]!=0))? "deleted" : "rangedel";
        else
            brd = (owned) ? "junk" : "deleted";
        if (digestmode==9 && (fh->accessed[0]==0))
            brd="rangedel";
#else

        brd = (owned) ? "junk" : "deleted";
#endif

        now = time(0);
        sprintf(fname, "M.%d.A", now);
        setbfile(genbuf, brd, fname);
        count = 0;
        while (dashf(genbuf))
        {
            now++;
            sprintf(fname, "M.%d.A", now);
            setbfile(genbuf, brd, fname);
            if (count++ > MAX_POSTRETRY)
            {
                fclose(fin);
                return;
            }
        }
        if ((fout = fopen(genbuf, "w")) != NULL)
        {
            memset(&postfile, 0, sizeof(postfile));
            sprintf(genbuf, "%-32.32s - %s", fh->title, userid);
            strcpy(postfile.filename, fname);
            strncpy(postfile.owner, fh->owner, IDLEN + 2);
            strncpy(postfile.title, genbuf, STRLEN);
            postfile.filename[STRLEN - 1] = 'D';
            postfile.filename[STRLEN - 2] = 'D';
        }
        from[0] = '\0';
        while (fgets(genbuf, sizeof(genbuf), fin) != NULL)
        {
            if (fout != NULL)
            {
                fputs(genbuf, fout);
            }
            len = strlen(genbuf) - 1;
            genbuf[len] = '\0';
            if (len <= 8)
            {
                break;
            }
            else if (strncmp(genbuf, "发信人: ", 8) == 0)
            {
                if ((ptr = strrchr(genbuf, ')')) != NULL)
                {
                    *ptr = '\0';
                    if ((ptr = strrchr(genbuf, '(')) != NULL)
                        strcpy(from, ptr + 1);
                }
            }
            else if (strncmp(genbuf, "转信站: ", 8) == 0)
            {
                strncpy(path, genbuf + 8, sizeof(path));
                path[sizeof(path) - 1] = '\0';
            }
        }
        if (fout != NULL)
        {
            while (fgets(genbuf, sizeof(genbuf), fin) != NULL)
                fputs(genbuf, fout);
        }
        fclose(fin);
        if (fout != NULL)
        {
            fclose(fout);
            //	 setbdir(genbuf, brd);
            setbfile(genbuf, brd,DOT_DIR);
            append_record(genbuf, &postfile, sizeof(postfile));
        }
        if (strrchr(fh->owner, '.'))
            return;
        if((fh->filename[STRLEN - 1] == 'S') && (fh->filename[STRLEN - 2] == 'S')
                && (atoi(fh->filename + 2) > now - 14 * 86400))
        {
            sprintf(genbuf, "%s\t%s\t%s\t%s\t%s\n",
                    board, fh->filename, fh->owner, from, fh->title);
            if ((fin = fopen("innd/cancel.bntp", "a")) != NULL)
            {
                fputs(genbuf, fin);
                fclose(fin);
            }
        }
    }
}

int
thesis_mode()
{
    int     id;    //, i;
    unsigned int pbits;
    //i = 'W' - 'A';
    id = getuser(currentuser.userid);
    pbits = lookupuser.userdefine;
    //pbits ^= (1 << i);
    pbits ^= DEF_THESIS;
    lookupuser.userdefine = pbits;
    currentuser.userdefine = pbits;
    substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
    update_utmp();
    return FULLUPDATE;
}
#ifdef BACKUP_RANGE_DELETE
/* add by quickmouse 2000.5.27 */
int NotBackupBoard(void)
{
    if(!strcmp(currboard,"junk") || !strcmp(currboard,"deleted"))
        return 1;
    return seek_in_file("etc/notbackupboards",currboard);
}
/* end of add */
#endif
/* Add by everlove 07/08/2001 制作合集 */
Add_Combine(board,fileinfo,has_yinyan)
char *board;
struct fileheader *fileinfo;
int has_yinyan;
{
    FILE *fp;
    char buf[STRLEN];
    char temp2[1024];

    sprintf(buf,"tmp/%s.combine",currentuser.userid);
    fp=fopen(buf,"at");
    fprintf(fp,"\033[1;32m☆──────────────────────────────────────☆\033[m\n");
    {
        FILE *fp1;
        char buf[80];
        char *s_ptr,*e_ptr;
        int blankline=0;

        setbfile(buf,board,fileinfo->filename);
        fp1=fopen(buf,"rt");
        if (fgets(temp2, 256, fp1)!=NULL)
        {
            e_ptr=strchr(temp2,',');
            if (e_ptr!=NULL)
                *e_ptr='\0';
            s_ptr=&temp2[7];
            fprintf(fp, "    \033[0;1;32m%s \033[m于",s_ptr);
        }
        fgets(temp2, 256, fp1);
        if (fgets(temp2, 256, fp1)!=NULL)
        {
            e_ptr=strchr(temp2,',');
            if (e_ptr!=NULL)
                *(--e_ptr)='\0';
            s_ptr=strchr(temp2,'(');
            if (s_ptr==NULL)
                s_ptr=temp2;
            else
                s_ptr++;
            fprintf(fp, " \033[1;36m%s\033[m 提到：\n",s_ptr);
        }
        while (!feof(fp1))
        {
            fgets(temp2, 256, fp1);
            if ((unsigned)*temp2<'\x1b')
            {
                if (blankline)
                    continue;
                else
                    blankline=1;
            }
            else
                blankline=0;
            if (!has_yinyan && ((strstr(temp2, "【")) || (*temp2==':')))
                continue;
            if ((*temp2==':') && ((strstr(temp2, "【"))||(*(temp2+2)==':')))
                continue;
            if (strncmp(temp2,"--",2)==0)
                break;
            fputs(temp2, fp);
        }
        fclose(fp1);
    }
    fprintf(fp,"\n" );
    fclose(fp);
}
/* The End */

//{ 回收站, add by mudboy
int
return_del(ent,fileinfo, direct)
      int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    int i,fail,ed=0;
    char buf[512];
    char UTitle[128];
    char *p;
    struct fileheader d;
    if (digestmode!=9)
        return DONOTHING;
	if((digestmode == 9) && (returnmode != 0))
		return DONOTHING;
	if(fileinfo->accessed[0] & FILE_TOP1)
		return DONOTHING;
	memcpy(&d,fileinfo,sizeof(d));
    strncpy(currfile, fileinfo->filename, STRLEN);

    strcpy(UTitle, fileinfo->title);
    if (p = strrchr(UTitle, '-'))
    { /* create default article title */
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i --)
        {
            if (UTitle[i] != ' ')
                break;
            else
                UTitle[i] = 0;
        }
    }
    strncpy(fileinfo->title,UTitle,STRLEN);

    fail = delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
    sprintf(buf,"boards/%s/%s",currboard,DOT_DIR);
    fileinfo->accessed[0] = 0;
    append_record(buf,fileinfo,sizeof(struct fileheader));
    if((d.accessed[0] & FILE_DELETED)&& (!junkboard()))
    {
        if (strcmp(d.owner,currentuser.userid))
        {
            if((ed=getuser(d.owner))>0)
            {
                lookupuser.numposts++;
                substitute_record(PASSFILE,&lookupuser,sizeof(struct userec),ed);
            }
        }
        else
        {
            set_safe_record();
            currentuser.numposts++;
            substitute_record(PASSFILE,&currentuser,sizeof(currentuser),usernum);
        }
    }
    return DIRCHANGED;
}
//}



//add by rwzmm @ sjtubbs, range move files from return box to the board
int
return_range(ent,fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
	char    num[9];
    int     inum1, inum2;
	
	if (digestmode!=9)
        return DONOTHING;
	if((digestmode == 9) && (returnmode != 0))
		return DONOTHING;

	getdata(t_lines - 1, 0, "首篇文章编号: ", num, 7, DOECHO, YEA);
    inum1 = atoi(num);
    if (inum1 <= 0)
    {
        move(t_lines - 1, 50);
        prints("错误编号...");
        egetch();
        return PARTUPDATE;
    }
    getdata(t_lines - 1, 25, "末篇文章编号: ", num, 7, DOECHO, YEA);
    inum2 = atoi(num);
    if (inum2 < inum1 + 1)
    {
        move(t_lines - 1, 50);
        prints("错误区间...");
        egetch();
        return PARTUPDATE;
    }

	move(t_lines - 1, 50);
    if (askyn("确定恢复？", NA, NA) == YEA)
    {
        return_file_range(direct, inum1, inum2);
        fixkeep(direct, inum1, inum2);
        return DIRCHANGED;
    }
    move(t_lines - 1, 50);
    clrtoeol();
    prints("放弃...");
    egetch();
    return PARTUPDATE;

}


int return_file_range(char *filename, int id1, int id2)
{
    struct fileheader fhdr;
    char    tmpfile[STRLEN], deleted[STRLEN];
    int     fdr, fdw, fd, count;
//	char buf[512];
	char* p;
	char UTitle[128];
	int i = 0;
	int ed = 0;
    //char    genbuf[STRLEN];

    tmpfilename(filename, tmpfile, deleted);
    if((fd=open(".dellock",O_RDWR|O_CREAT|O_APPEND, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);

    if ((fdr = open(filename, O_RDONLY, 0)) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if((fdw=open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        close(fdr);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    count = 1;
    while(read(fdr, &fhdr, sizeof(fhdr))==sizeof(fhdr))
    {

        if (count < id1 || count > id2)
        {
            if ((safewrite(fdw,(char *)&fhdr,sizeof(fhdr))==-1))
            {
                close(fdw);
                unlink(tmpfile);
                close(fdr);
                flock(fd, LOCK_UN);
                close(fd);
                return -1;
            }
        }
        else
        {
            char   *t;
            char    buf[STRLEN], fullpath[STRLEN];
            strcpy(buf, filename);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(fullpath, "%s/%s", buf, fhdr.filename);

			if((fhdr.accessed[0] & FILE_DELETED)&& (!junkboard()))
			{
				if (strcmp(fhdr.owner,currentuser.userid))
				{
					if((ed=getuser(fhdr.owner))>0)
					{
						lookupuser.numposts++;
						substitute_record(PASSFILE,&lookupuser,sizeof(struct userec),ed);
					}
				}
				else
				{
					set_safe_record();
					currentuser.numposts++;
					substitute_record(PASSFILE,&currentuser,sizeof(currentuser),usernum);
				}
			}

            fhdr.accessed[0] = 0;
            setbfile(buf,currboard,DOT_DIR);
		
			strcpy(UTitle, fhdr.title);
			if (p = strrchr(UTitle, '-'))
			{ /* create default article title */
				*p = 0;
				for (i = strlen(UTitle) - 1; i >= 0; i --)
				{
					if (UTitle[i] != ' ')
						break;
					else
						UTitle[i] = 0;
				}
			}
			strncpy(fhdr.title,UTitle,STRLEN);
            append_record(buf, &fhdr, sizeof(fhdr));

        }
        count++;
    }
    close(fdw);
    close(fdr);
    if (rename(filename, deleted) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if (rename(tmpfile, filename) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

//add end

//add by rwzmm @ sjtubbs, find files of the same author or the same title
int
return_find(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
	int type;
	extern char currdirect[STRLEN];
    char ch[4];

	//char formerbuf[STRLEN*2];

	if(digestmode != 9)
		return DONOTHING;

	saveline (t_lines - 1, 0);
	move (t_lines - 1, 0);
	clrtoeol ();
	ch[0] = '\0';


    getdata (t_lines - 1, 0, "查询模式: 1)同作者 2)标题关键字 [1]: ", 
		ch, 3, DOECHO, YEA);
	if (ch[0] == '\0')
		ch[0] = '1';
	type = atoi (ch);
	if (type < 1 || type > 2 )
	{
		saveline (t_lines - 1, 1);
		return PARTUPDATE;
	}
	else if(type == 2)	//标题关键字
	{
		getdata (t_lines - 1, 0, "您想查找的文章标题关键字: ",
			someoneID, 30, DOECHO, YEA);
		if (someoneID[0] == '\0')
		{
			saveline (t_lines - 1, 1);
			return PARTUPDATE;
		}
		type = 3;	//warning
	}

	else	//作者关键字
	{
		getdata (t_lines - 1, 0, "您想查找哪位网友的文章? ", someoneID,
			13, DOECHO, YEA);
		if (someoneID[0] == '\0')
		{
			saveline (t_lines - 1, 1);
			return PARTUPDATE;
		}
		getdata (t_lines - 1, 37,
			"精确查找按 Y， 模糊查找请回车[Enter]", ch, 2, DOECHO,
			YEA);
		if (ch[0] == 'y' || ch[0] == 'Y')
			type = 2;
		else
			type = 1;
	}
	
	returnmode = type;

	return_search_all(type);
	setbdir(currdirect, currboard);

	if (!dashf(currdirect))
	{
		digestmode = 9;
		returnmode = 0;
		setbdir(currdirect, currboard);
		return PARTUPDATE;
	}

	return NEWDIRECT;
}



//add by rwzmm at sjtubbs
void rengedel_autoreport (title, str, userid, sysboard)
char *title;
char *str;
char *userid;
char *sysboard;
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report (title);
    sprintf (fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
    if ((se = fopen (fname, "w")) != NULL)
    {
        fprintf (se, "%s", str);
        fclose (se);
        if (userid != NULL)
            mail_file (fname, userid, title);
        if (sysboard != NULL)
            Postfile (fname, sysboard, title, 1);
//        Postfile (fname, currboard, title, 1);
        unlink (fname);
        modify_user_mode (savemode);
    }

	return;
}

int is_web_user(struct user_info *user)
{
	return (10001==user->mode);
}

