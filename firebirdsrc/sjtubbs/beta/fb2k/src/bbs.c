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
#include <stdarg.h>
#include <sys/uio.h>
#include <time.h>
#if defined(BSD44)
#include <stdlib.h>

#elif defined(LINUX)
/* include nothing :-) */
#else

#include <rpcsvc/rstat.h>
#endif

//added by proto@sjtubbs 2006.10.16
FILE *ftemp;
char filename[28],buffer[51],firstchar;
//added end by proto@sjtubbs 2006.10.16

int     mot;
struct postheader header;
extern int numofsig;
extern char lastboard[80];


int     continue_flag;
int     readpost;
int     digestmode;
int returnmode;
int	local_article;
struct userec currentuser;
char blog_user_id[32];
char blog_id[32];
char blog_catId[32];
///////////////////////////
char module_name[NAMELEN];//added by chaser
int writeinto_modulefile;
int write_module = 0;
extern char jhpath[1024];
//////////////////////////
int     usernum=0;
char    currboard[STRLEN - BM_LEN];
char    currBM[BM_LEN - 4];
int     selboard = 0;
char	someoneID[31];
char    ReadPost[STRLEN] = "";
char    ReplyPost[STRLEN] = "";
//modified by rwzmm @ sjtubbs, reid
int current_article_reid = 0;
unsigned int reply_id = 0;
//modified end

extern struct BCACHE *brdshm;

int     FFLL = 0;

/*For read.c*/
#ifdef BOARD_CONTROL
int     control_user(void);
#endif


extern int SR_BMDELFLAG;
extern int numboards;
extern time_t login_start_time;
extern char BoardName[];
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
/////////////////////////////////////////////////
char module_file[NAMELEN];//added by chaser
/////////////////////////////////////////////////
#ifndef NOREPLY
char    replytitle[STRLEN];
#endif

struct shortfile *getbcache(char *bname);
int     totalusers, usercounter;

void rengedel_autoreport (char* title, char*str, char* userid, char* sysboard);


int
check_stuffmode(void)
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

int topfile_post(int ent, struct fileheader *fhdr, char *direct)
{
    if(!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if(fhdr->accessed[0] & FILE_TOP1)
    {
		char buffer[128]; //added by proto@sjtubbs 2006.11.30
        fhdr->accessed[0] &= ~FILE_TOP1;
        dele_digest_top(fhdr->filename,direct,TOPFILE_DIR);
		//added by proto@sjtubbs 2006.11.30
		sprintf(buffer, "untopped the file '%s' on '%s'", fhdr->title, currboard);
    	report(buffer);
		//added end
    }
    else
    {
        struct fileheader topfile;
        char *ptr, buf[64], buffer[128]; //modified by proto@sjtubbs 2006.11.30
        memcpy(&topfile, fhdr, sizeof(topfile));
        topfile.filename[0] = 'T';
        topfile.accessed[0] |= FILE_NOREPLY;
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s%s/%s", buf, getfolderstr(topfile.filename), topfile.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] |= FILE_TOP1;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        sprintf(&genbuf[512], "%s%s/%s", buf, getfolderstr(fhdr->filename), fhdr->filename);

        strcpy(ptr, TOPFILE_DIR);
        if (get_topfile_num(buf, sizeof(topfile)) >= MAX_TOP)
        { //++
            //--	if (get_num_records(buf, sizeof(topfile)) >= 2*MAX_TOP) {
            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("±§Ç¸£¬ÄãµÄÖÃµ×ÎÄÕÂÒÑ¾­³¬¹ý %d Æª£¬ÎÞ·¨ÔÙ¼ÓÈë...\n", MAX_TOP);
            pressanykey();
            return PARTUPDATE;
        }
        link(&genbuf[512], genbuf);
        append_record(buf, &topfile, sizeof(topfile));
        fhdr->accessed[0] |= FILE_TOP1;
        fhdr->accessed[0] &= ~FILE_DELETED;
		//added by proto@sjtubbs 2006.11.30
    	sprintf(buffer, "topped the file '%s' on '%s'", topfile.title, currboard);
    	report(buffer);
		//added end
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
set_safe_record(void)
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
    return 0;
}

char   *
sethomepath(char *buf, char *userid)
{
    sprintf(buf, "home/%c/%c/%s", toupper(userid[0]), toupper(userid[1]), userid);
    return buf;
}
/*Add by SmallPig*/
//modified by rwzmm @ sjtubbs, reid

void
setqtitle(char *stitle)
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
setqreid(unsigned int reid)
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
        return SectNum;
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


int chk_currDM(void)
{
    char BoardsFile[256], category;
    int fd, total, index, Dindex, DMLevel;
    struct boardheader *buffer;
    struct stat st;

    char CateInSect[SectNum][5] =
        {"a", "bc", "d", "efg", "hij", "kl", "o", "p","qrs","tu","vw", "x"};
    /* {"a", "bcd", "efg", "hij", "kl", "mn", "op", "qr","s"};*/
    /****************************************************************************/
    /*  ²ÉÓÃÀàËÆbbs2wwwµÄ°ì·¨£¬Èç¹ûÐÞ¸ÄÁËÌÖÂÛÇøµÄ·ÖÀà£¬ÇëÊÓÇé¿öÐÞ¸ÄÒÔÏÂ¶¨Òå     */
    /*                                                                          */
    /*  ·ÖÀàÂëºÍ·ÖÀà²Ëµ¥ÔÚmenu.iniÖÐÉè¶¨,Firebird BBS 2.52ÖÐÈ±Ê¡µÄ·ÖÀàÓÐ:       */
    /*  0¡¾Õ¾ÄÚ¡¿¡¢C¡¾±¾Ð£¡¿¡¢A¡¾Çó¾È¡¿¡¢E¡¾×ÊÑ¶¡¿¡¢I¡¾Ð£Ô°¡¿¡¢N¡¾µçÄÔ¡¿¡¢R¡¾   */
    /*  ÏµÍ³¡¿¡¢a¡¾ÐÝÏÐ¡¿¡¢e¡¾ÎÄÒÕ¡¿¡¢i¡¾Ñ§¿Æ¡¿¡¢m¡¾ÒôÀÖ¡¿¡¢n¡¾Éç½»¡¿¡¢r¡¾ÔË¶¯  */
    /*  ¡¿¡¢s¡¾Ö°°ô¡¿¡¢x¡¾Ì¸Ìì¡¿¡¢f¡¾ÐÂÎÅ¡¿µÈÀà                                 */
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
    free(buffer);

    for ( Dindex = 0; Dindex < SectNum; Dindex++ )
    {
        if (strchr (CateInSect[Dindex], category) != NULL)
            break;
    }

    if ( Dindex == SectNum)
        return NA;

    //DMLevel = 4 * ( HAS_PERM(PERM_DIST100) ? 1 : 0 ) + 2 * ( HAS_PERM(PERM_DIST010) ? 1 : 0 )+ ( HAS_PERM(PERM_DIST001) ? 1 : 0 )+1;
    //        DMLevel = WHICHDIST();//changed by aegis@010921
    DMLevel = WHICHDIST(currentuser.userid);//mudboy modify
    if ( Dindex == DMLevel )
        return YEA;
    else
        return NA;
}

//add by zsong, for the whole master of a directory
int chk_parentBM(void)
{
	int mypos = 0;
	int parent_pos=0;
	char   *ptr;
    char    BMstrbuf[STRLEN - 1];
	for(mypos=0; mypos<MAXBOARD; mypos++)
	{
		if (!strcmp(brdshm->bcache[mypos].filename, currboard))
			break;
	}
	if(mypos==MAXBOARD)
		return NA;
	parent_pos = brdshm->bcache[mypos].group - 1;
	if(parent_pos < 0)
		return NA;
    strncpy(BMstrbuf, brdshm->bcache[parent_pos].BM, STRLEN-4);
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
//add end

int
chk_currBM(char *BMstr)
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
	//add by zsong, for the whole master of a directory
	if(chk_parentBM() == YEA)
		return YEA;
	//add end
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
setquotefile(char *filepath)
{
    strcpy(quote_file, filepath);
}

char   *
sethomefile(char *buf, char *userid, char *filename)
{
    sprintf(buf, "home/%c/%c/%s/%s", toupper(userid[0]), toupper(userid[1]), userid, filename);
    return buf;
}

char   *
setuserfile(char *buf, char *filename)
{
    sprintf(buf, "home/%c/%c/%s/%s", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]), currentuser.userid, filename);
    return buf;
}

char   *
setbpath(char *buf, char *boardname)
{
    strcpy(buf, "boards/");
    strcat(buf, boardname);
    return buf;
}

char   *
setbdir(char *buf, char *boardname)
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
    case 5: /* Í¬×÷Õß */
    case 6: /* Í¬×÷Õß */
    case 7: /* ±êÌâ¹Ø¼ü×Ö */
	break;
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
setbfile(char *buf, char *boardname, char *filename)
{
	int len = strlen(filename);
	if (len>10 && filename[1]=='.' && filename[len-2]=='.' && isdigit(filename[len-5]) && isdigit(filename[len-4]) && isdigit(filename[len-3]))
		sprintf(buf, "boards/%s/%s/%s", boardname, getfolderstr(filename), filename);
	else
		sprintf(buf, "boards/%s/%s", boardname, filename);
    return buf;
}

char *
setmodulefile(char *buf, char *boardname, char *filename)
//added by chaser 06.05.07(../vote/boardname/modulename..)
{
   sprintf(buf,"vote/%s/%s",boardname,filename);
   return buf;
}

void setmfile(char *buf,char *userid, char *filename)
{
    sprintf(buf,"mail/%c/%c/%s/%s",toupper(userid[0]),toupper(userid[1]),userid, filename);
}

char *
setmdir(char *buf, char *userid)
{
    sprintf(buf,"mail/%c/%c/%s/.DIR",toupper(userid[0]),toupper(userid[1]),userid);
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
seek_in_denyfile(char *filename, char *seekstr)
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
        if (namep != NULL && strcasecmp(namep, seekstr) == 0)
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
                    sprintf(repbuf, "»Ö¸´%sÔÚ%s°åµÄ·¢ÎÄÈ¨ÏÞ",
                            seekstr,currboard);
                    sprintf(msgbuf, "\n  %s ÍøÓÑ£º\n\n"
                            "\tÒò·â½ûÊ±¼äÒÑ¹ý£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º·¢±íÎÄÕÂ¡»È¨Á¦¡£\n\n"
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
shownotepad(void)
{
    modify_user_mode(NOTEPAD);
    ansimore("etc/notepad",YEA);
    return;
}

int
uleveltochar(char *buf, unsigned int lvl)
{
    if (!(lvl & PERM_BASIC))
    {
        strcpy(buf, "--------- ");
        return 0;
    }
    if (lvl < PERM_DEFAULT)
    {
        strcpy(buf, "--------- ");
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
printutitle(void)
{
    move(2, 0);
    prints("\033[1;44m ±à ºÅ  Ê¹ÓÃÕß´úºÅ   %-19s #%-4s #%-4s %8s    %-12s  \033[m\n",
#if defined(ACTS_REALNAMES)
           HAS_PERM(PERM_SYSOP) ? "ÕæÊµÐÕÃû" : "Ê¹ÓÃÕßêÇ³Æ",
#else
           "Ê¹ÓÃÕßêÇ³Æ",
#endif
           (toggle2 == 0) ? "ÉÏÕ¾" :  "ÎÄÕÂ" ,
           (toggle2 == 0) ? "Ê±Êý" :  "ÐÅ¼þ" ,
           HAS_PERM(PERM_SEEULEVELS) ? " µÈ  ¼¶ " : "",
           (toggle1 == 0) ? "×î½ü¹âÁÙÈÕÆÚ" :
           (toggle1 == 1) ? "×î½ü¹âÁÙµØµã" : "ÕÊºÅ½¨Á¢ÈÕÆÚ");
}


int
g_board_names(struct shortfile *fhdrp)
{
    if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(fhdrp->level)
            || (fhdrp->level & PERM_NOZAP))
    {
        AddToNameList(fhdrp->filename);
    }
    return 0;
}

void
make_blist(void)
{
    CreateNameList();
    apply_boards(g_board_names);
}

int
Select(void)
{
    modify_user_mode(SELECT);
    do_select(0, NULL, genbuf);
    return 0;
}

int
junkboard(void)
{
	struct shortfile *bp;
	bp = getbcache(currboard);
	if(bp->flag&NOPOST_FLAG) 
		return 1;
    if (strstr(JUNKBOARDS, currboard))
        return 1;
    else
        return seek_in_file("etc/junkboards",currboard);
}

int
Post(void)
{
    if (!selboard)
    {
        prints("\n\nÏÈÓÃ (S)elect È¥Ñ¡ÔñÒ»¸öÌÖÂÛÇø¡£\n");
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
Postfile(char *filename, char *nboard, char *posttitle, int mode)
{
    char    bname[STRLEN];
    char    dbname[STRLEN];
    struct boardheader fh;
	if (nboard != NULL)
    if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, nboard) <= 0)
    {
        sprintf(bname, "%s ÌÖÂÛÇøÕÒ²»µ½", nboard);
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
    return 0;
}

int
get_a_boardname(char *bname, char *prompt)
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
        prints("´íÎóµÄÌÖÂÛÇøÃû³Æ\n");
        pressreturn();
        move(1, 0);
        return 0;
    }
    return 1;
}
/* Add by SmallPig */
int
do_cross(int ent, struct fileheader *fileinfo, char *direct)
{
    char    bname[STRLEN];
    char    dbname[STRLEN];
    char    ispost[10];
	time_t  now = time(0);
	int     digbackup;
	struct tm *tm;

    if(digestmode && digestmode != 9)
        return DONOTHING;
	digbackup = digestmode;
    set_safe_record();
    if (!HAS_PERM(PERM_POST))
        return DONOTHING;
    if (uinfo.mode != RMAIL)
        sprintf(genbuf, "boards/%s/%s/%s", currboard, getfolderstr(fileinfo->filename), fileinfo->filename);
    else
        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]), currentuser.userid, fileinfo->filename);
    strncpy(quote_file, genbuf, sizeof(quote_file));
    quote_file[sizeof(quote_file) - 1] = '\0';
    strcpy(quote_title, fileinfo->title);

    clear();

//	int item = 0;
	tm = localtime (&now);
	if(brdshm->all_no_post_end >= brdshm->all_no_post_begin) {
	if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8)) 
	{
		
		if((unsigned int)tm->tm_hour >= brdshm->all_no_post_begin && (unsigned int)tm->tm_hour < brdshm->all_no_post_end) 
		{
			move(5,10);
			clrtobot();
			prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±È«Õ¾Ö»¶Á¡£", brdshm->all_no_post_begin, brdshm->all_no_post_end);
			pressreturn();
        		clear();
        		return FULLUPDATE;	
			}


		}
	
    if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8|PERM_BOARDS)) 
	{
		if((unsigned int)tm->tm_hour >= brdshm->no_post_begin && (unsigned int)tm->tm_hour < brdshm->no_post_end) 
		{
			if( !insjtunetwork(fromhost) ) 
			{
				move(5, 10);
        			clrtobot();
				prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",brdshm->no_post_begin,brdshm->no_post_end);
				pressreturn();
        			clear();
        			return FULLUPDATE;			
			}		
		}
    }
		} else {
				if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8)) 
	{
		
		if((unsigned int)tm->tm_hour >= brdshm->all_no_post_begin || (unsigned int)tm->tm_hour < brdshm->all_no_post_end) 
		{
			move(5,10);
			clrtobot();
			prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±È«Õ¾Ö»¶Á¡£", brdshm->all_no_post_begin, brdshm->all_no_post_end);
			pressreturn();
        		clear();
        		return FULLUPDATE;	
			}


		}
	
    if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8|PERM_BOARDS)) 
	{
		if((unsigned int)tm->tm_hour >= brdshm->no_post_begin || (unsigned int)tm->tm_hour < brdshm->no_post_end) 
		{
			if( !insjtunetwork(fromhost) ) 
			{
				move(5, 10);
        			clrtobot();
				prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",brdshm->no_post_begin,brdshm->no_post_end);
				pressreturn();
        			clear();
        			return FULLUPDATE;			
			}		
		}
    }


			}
    prints("\033[1;33mÇëÕäÏ§±¾Õ¾×ÊÔ´£¬ÇëÔÚ×ªÔØºóÉ¾³ý²»±ØÒªµÄÎÄÕÂ£¬Ð»Ð»£¡ \n\033[1;32m²¢ÇÒ£¬Äã²»ÄÜ½«±¾ÎÄ×ªÔØµ½±¾°å£¬Èç¹ûÄã¾õµÃ²»·½±ãµÄ»°£¬ÇëÓëÕ¾³¤ÁªÏµ¡£\033[m\n\n");
    prints("ÄúÑ¡Ôñ×ªÔØµÄÎÄÕÂÊÇ: [\033[1;33m%s\033[m]\n",quote_title);
    if(!get_a_boardname(bname, "ÄúÈç¹ûÈ·¶¨Òª×ªÔØµÄ»°£¬ÇëÊäÈëÒª×ªÌùµÄÌÖÂÛÇøÃû³Æ(È¡Ïû×ªÔØÇë°´»Ø³µ): "))
    {
        return FULLUPDATE;
    }
    if(!strcmp(bname,currboard)&&uinfo.mode != RMAIL)
    {
        move(8,8);
        prints("¶Ô²»Æð£¬±¾ÎÄ¾ÍÔÚÄúÒª×ªÔØµÄ°åÃæÉÏ£¬ËùÒÔÎÞÐè×ªÔØ¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    move(3, 0);
    clrtoeol();
    prints("×ªÔØ ' %s ' µ½ %s °å ", quote_title, bname);
    move(4, 0);
    getdata(4, 0, "(S)×ªÐÅ (L)±¾Õ¾ (A)È¡Ïû? [L]: ", ispost, 9, DOECHO, YEA);
    if(ispost[0]=='a'||ispost[0]=='A')
        prints("È¡Ïû");
    else
    {
        strcpy(quote_board, currboard);
        strcpy(dbname, currboard);
        strcpy(currboard, bname);
		digbackup = digestmode;
		digestmode = 0;
        if(ispost[0]!='s'&&ispost[0]!='S')
            ispost[0]='L';
        if (post_cross(ispost[0], 0) == -1)
        {
            pressreturn();
            move(2, 0);
            strcpy(currboard, dbname);
			digestmode = digbackup;
            return FULLUPDATE;
        }
        strcpy(currboard, dbname);
        prints("\nÒÑ°ÑÎÄÕÂ \'%s\' ×ªÌùµ½ %s °å\n", quote_title, bname);
    }
    move(2, 0);
    pressreturn();
	digestmode = digbackup;
    return FULLUPDATE;
}

void
readtitle(void)
{
    struct shortfile *bp;
    int i,j,bnum,tuid;
    struct user_info uin;
    char tmp[40],bmlists[3][16];
    extern int t_cmpuids(int uid, struct user_info *up);

    char    header[STRLEN], title[STRLEN];
    char    readmode[11];
    bp = getbcache(currboard);
    memcpy(currBM, bp->BM, BM_LEN - 4);
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
        strcpy(header, "³ÏÕ÷°åÖ÷ÖÐ");
    }
    else
    {
        strcpy(header, "°åÖ÷: ");
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
        //		strcpy(title, "[ÄúÓÐÐÅ¼þ£¬°´ M ¿´ÐÂÐÅ]");
        strcpy(title, "[ÄúÓÐÐÅ¼þ£¬°´ L ¿´ÐÂÐÅ]");
    else if ((bp->flag & VOTE_FLAG))
        sprintf(title, "¡ùÍ¶Æ±ÖÐ,°´ v ½øÈëÍ¶Æ±¡ù");
    else
        strcpy(title, bp->title + 8);

    showtitle(header, title);
    prints("Àë¿ª[\033[1;32m¡û\033[m,\033[1;32me\033[m] Ñ¡Ôñ[\033[1;32m¡ü\033[m,\033[1;32m¡ý\033[m] ÔÄ¶Á[\033[1;32m¡ú\033[m,\033[1;32mRtn\033[m] ·¢±íÎÄÕÂ[\033[1;32mCtrl-P\033[m] ¿³ÐÅ[\033[1;32md\033[m] ±¸ÍüÂ¼[\033[1;32mTAB\033[m] ÇóÖú[\033[1;32mh\033[m]\n");
    if (digestmode == 0)
    {
        if (DEFINE(DEF_THESIS))	/* youzi 1997.7.8 */
            strcpy(readmode, "Ö÷Ìâ");
        else
            strcpy(readmode, "Ò»°ã");
    }
    else if (digestmode == 1)
        strcpy(readmode, "ÎÄÕª");
    else if (digestmode == 2)
        strcpy(readmode, "Ö÷Ìâ");
    else if (digestmode == 3)
        strcpy(readmode,"MARK");
    else if (digestmode == 4)
        strcpy(readmode,"Ô­×÷");
    else if (digestmode == 7)
        strcpy(readmode,"±êÌâ¹Ø¼ü×Ö");
    //{ »ØÊÕÕ¾, add by mudboy
    else if (digestmode == 9)
        strcpy(readmode,"»ØÊÕ");
    //}
    if (DEFINE(DEF_THESIS) && digestmode == 0)
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-38s[%4sÊ½¿´°å] \033[m\n", "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    else if (digestmode == 5 || digestmode == 6)
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-10s (¹Ø¼ü×Ö: \033[32m%-12s\033[37m) [\033[33m%s\033[37mÍ¬×÷ÕßÔÄ¶Á] \033[m\n","¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", someoneID,(digestmode==5)?"Ä£ºý":"¾«È·");
    else if (digestmode == 7 )
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-32s[%10sÊ½¿´°å] \033[m\n", "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    else
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-40s[%4sÄ£Ê½] \033[m\n", "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    clrtobot();
}

char *getstring_byaccess(char *result,int num,int a, int b, int c)
{
	int i;
	i=num;
	if(i<=0)
		i=-i;
	if(i<=a)
		snprintf(result,80,"[1;32m%3d[m",i);
	else if(i<=b)
		snprintf(result,80,"[1;33m%3d[m",i);
	else if(i<=c)
		snprintf(result,80,"[1;35m%3d[m",i);
	else
		snprintf(result,80,"[1;31m%3d[m",i);
	return result;
}

char   *
readdoent(int num, void *vent)    // ÎÄÕÂÁÐ±í
{
	struct fileheader *ent = (struct fileheader *)vent;
    char path[64]/*,accessstring[256]*/;
    struct stat st;
    char msg[17];
    static char buf[128];
    struct shortfile *bp;
    time_t  filetime;
    char   *date;
    char   *TITLE, color[10];
    int     type;
    int     inAnnounce = 0; // whether the article is in 0announce 
#ifdef COLOR_POST_DATE

    struct tm *mytm;
#endif
	bp = getbcache(currboard);
    type = brc_unread(ent->filename) ?
           (!DEFINE(DEF_NOT_N_MASK)?'N':'+') : ' ';
	if((bp->flag&PUSH_FLAG)&&ent->pnum>0&&ent->pnum<10000)
	{
		if(strcmp(lastboard	,currboard))
    		{
    			brc_getpushrecord();
    		}
		if(brc_pushunread(ent))
		{
			if(type=='+')
				type='*';
			else
				type='p';
		}
	}
	
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
    
    if (ent->accessed[0] & FILE_VISIT && chk_currBM(currBM))
    {
    	inAnnounce = 1;
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
     if((ent->accessed[1] & FILE_RECOMMENDED) /*&& (digestmode != 9 )*/)
    {
        type='*';
    }
    /*end*/
    
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

    TITLE = ent->title;

    if (TITLE[47] )
        strcpy(TITLE + 44, "...");  /* °Ñ¶àâÅµÄ string ¿³µô */

	if (FFLL == 0)
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
            sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ", num,
				inAnnounce == 1 ? "\033[4m" : "",
				type,
				inAnnounce == 1 ? "\033[m" : "",
                ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
        }
        else
        {
            sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ", num,
				inAnnounce == 1 ? "\033[4m" : "",
				type,
				inAnnounce == 1 ? "\033[m" : "",
                ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
        }
    }
    else
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
			if(reply_id != 0)
			{
				if ((ent->reid == reply_id) && (ent->reid == ent->id))
				{
					sprintf(buf,
							" \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m%-.48s\033[m ",
							num,
							inAnnounce == 1 ? "\033[4m" : "",
							type,
							inAnnounce == 1 ? "\033[m" : "",
							ent->owner, color, date,
							noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else if ((ent->reid == reply_id) && (ent->reid != ent->id))
				{
					sprintf(buf,
							" \033[1;36m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
							num,
							inAnnounce == 1 ? "\033[4m" : "",
							type,
							inAnnounce == 1 ? "\033[m" : "",
							ent->owner, color, date,
							noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ", num,
							inAnnounce == 1 ? "\033[4m" : "",
							type,
							inAnnounce == 1 ? "\033[m" : "",
							ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
			else
			{
				if ((strncmp(ReplyPost, ent->title, 40) == 0) && (ent->reid == reply_id))
				{
					sprintf(buf,
						" \033[1;36m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
						num,
						inAnnounce == 1 ? "\033[4m" : "",
						type,
						inAnnounce == 1 ? "\033[m" : "",
						ent->owner, color, date,
						noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ", num,
						inAnnounce == 1 ? "\033[4m" : "",
						type,
						inAnnounce == 1 ? "\033[m" : "",
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
						" \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m¡ô %-.45s\033[m "
						,num,
						inAnnounce == 1 ? "\033[4m" : "",
						type,
						inAnnounce == 1 ? "\033[m" : "",
						ent->owner, color, date,
						noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",num,
						inAnnounce == 1 ? "\033[4m" : "",
						type,
						inAnnounce == 1 ? "\033[m" : "",
						ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
			else
			{
				if ((strncmp(ReadPost, ent->title, 40) == 0) && (ent->reid == reply_id))
				{
					sprintf(buf,
							" \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m¡ô %-.45s\033[m "
							,num,
							inAnnounce == 1 ? "\033[4m" : "",
							type,
							inAnnounce == 1 ? "\033[m" : "",
							ent->owner, color, date,
							noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
				else
				{
					sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",num,
							inAnnounce == 1 ? "\033[4m" : "",
							type,
							inAnnounce == 1 ? "\033[m" : "",
							ent->owner, color, date, noreply?"\033[0;1;4;33mx\033[m":" ",TITLE);
				}
			}
        }
    
  	}

    /*added by cf*/

	setbdir(path,currboard);
	if((stat(path,&st)!=-1) && ((int)(st.st_size/sizeof(struct fileheader))< num ))
	{
		ent->accessed[0] |= FILE_TOP1;
		if((ent->accessed[0]& FILE_MARKED)&&(ent->accessed[0]&FILE_DIGEST))
			strcpy(msg,"[°å¹æ]");
		else if(ent->accessed[0] & FILE_DIGEST)
			strcpy(msg,"[Í¨Öª]");
		else if(ent->accessed[0] & FILE_MARKED)
			strcpy(msg,"[ÌáÊ¾]");
		else
			strcpy(msg,"[ÖÃµ×]");
		msg[17]=0;

		int weekday = ((filetime + 28800) / 86400 + 4) % 7;
		sprintf(color, "\033[1;%dm", 31 + weekday);

		sprintf(buf, " \033[1;31m%6s\033[m  %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",
				msg,ent->owner, color, date," ", TITLE);
	}
    /*end*/
    noreply = 0;
    return buf;
}

char    currfile[STRLEN];
int
cmpfilename(void *vfhdr)
{
	struct fileheader *fhdr = (struct fileheader *) vfhdr;
    if (!strncmp(fhdr->filename, currfile, STRLEN))
        return 1;
    return 0;
}

int
cmpdigestfilename(char *digest_name, void *vfhdr)
{
	struct fileheader *fhdr = (struct fileheader *) vfhdr;
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
    sprintf(buf, "<< ±¾ÎÄ±» %s ÓÚ %d/%d %d:%02d:%02d É¾³ý >>",
            currentuser.userid, now->tm_mon + 1, now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec);
    strncpy(fhdr->title, buf, STRLEN);
    fhdr->filename[STRLEN - 1] = 'L';
    fhdr->filename[STRLEN - 2] = 'L';
    return 0;
}


int do_reply_push (struct fileheader *ptitle,int ent,char *direct)
{	
	/* struct fileheader postfile; */
	char /* buf[STRLEN],*/filepath[STRLEN],content[80],pushbuf[2048],pfilename[80];
	FILE *fp;

	snprintf(pfilename,80,"%s",ptitle->filename);
	pfilename[0]='P';
	snprintf(filepath, 80,"boards/%s/%s/%s", currboard, getfolderstr(pfilename), pfilename);
       getdata (t_lines - 2, 0, "ÍÆÎÄ: ", content, 72, DOECHO, YEA);
	if(!strcmp(content,""))
		return -1;
	format_pushbuf(content, pushbuf);
	if((fp = fopen(filepath,"a")) == NULL) {
			return -1;
	}
	fprintf(fp,"%s\n",pushbuf);
	fclose(fp);
	if(ptitle->pnum>10000)
		ptitle->pnum=0;
	ptitle->pnum++;
	substitute_record(direct, ptitle, sizeof(struct fileheader), ent);
	brc_updatepush (ptitle);
	return 0;
		/*substitute_record(direct, oheader, sizeof(*oheader), ent);
   
		return 1;
	
	
	memset(&postfile, 0, sizeof(postfile));
	setbfile(buf, currboard, PUSH_DIR);
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
    	report(buf);*/
}



int
read_pushpost(int ent, struct fileheader *fileinfo, char *direct)
{
    char   *t;
    char    buf[512],pushfilename[80];
    int     ch;
    int     cou;
    brc_addlist(fileinfo->filename);
    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
	snprintf(pushfilename,80,"%s",fileinfo->filename);
	pushfilename[0]='P';
    sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(pushfilename), pushfilename);
	brc_updatepush (fileinfo);
    if(!dashf(genbuf))
    {
        clear();
        move(10,30);
        prints("¶Ô²»Æð£¬±¾ÎÄÄÚÈÝ¶ªÊ§£¡(%s)", genbuf);
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
        prints("\033[1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m»ØÐÅ R ©¦ ½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X»òp \033[m");
    }
    else
    {
        prints("\033[1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,<Enter>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X »ò p \033[m");
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
    case 'a':
    case 'A':
		{
			struct shortfile *bp;
                	extern struct shortfile *getbcache (char *bname);

             		bp = getbcache (currboard);
                	noreply = fileinfo->accessed[0] & FILE_NOREPLY
                          || bp->flag & NOREPLY_FLAG;
                	if (!noreply || HAS_PERM (PERM_SYSOP) || chk_currBM (currBM))
                	{
                    		local_article = !(fileinfo->filename[STRLEN - 1] == 'S');
		    		if(bp->flag & PUSH_FLAG)
					do_reply_push (fileinfo,ent,direct);
		    		else 
                    			do_reply (fileinfo->title, fileinfo->owner, fileinfo->reid);
                	}
                	else
                	{
                    		clear ();
                    		move (5, 6);
                    		prints
                    			("¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.");
                    		pressreturn ();
                	}
                	break;
        	}
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
                    //ÕâÀïÊÇÔÚÎÄÕÂÁÐ±íÇ°Ãæ²éÑ¯Ê¹ÓÃÕßµÄctrl a
                    //µ÷ÓÃÁËshow authorº¯Êý
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
read_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char   *t;
    char    buf[512];
    int     ch;
    int     cou;
	/*struct fileheader *pushfileinfo;*/
    brc_addlist(fileinfo->filename);
    if (fileinfo->owner[0] == '-')
        return FULLUPDATE;
    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(fileinfo->filename), fileinfo->filename);
    if(!dashf(genbuf))
    {
        clear();
        move(10,30);
        prints("¶Ô²»Æð£¬±¾ÎÄÄÚÈÝ¶ªÊ§£¡(%s)", genbuf);
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
        prints("\033[1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m»ØÐÅ R ©¦ ½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X»òp \033[m");
    }
    else
    {
        prints("\033[1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,<Enter>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X »ò p \033[m");
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
    case 'a':
    case 'A':
	{
		/*pushfileinfo=(struct fileheader*)malloc(sizeof(struct fileheader));
		pushfileinfo->accessed[0]=fileinfo->accessed[0];
		pushfileinfo->accessed[1]=fileinfo->accessed[1];
		pushfileinfo->accessed[2]=fileinfo->accessed[2];
		pushfileinfo->accessed[3]=fileinfo->accessed[3];
		strcpy(pushfileinfo->filename,fileinfo->filename);
		pushfileinfo->filename[0]='P';
		pushfileinfo->id=fileinfo->id;
		pushfileinfo->level=fileinfo->level;
		strcpy(pushfileinfo->owner,fileinfo->owner);
		pushfileinfo->pnum=fileinfo->pnum;*/
		if(fileinfo->pnum>10000)
			fileinfo->pnum=0;
		if(fileinfo->pnum>0)
			read_pushpost(ent, fileinfo, direct);
		break;
    	}
    case 'Y':
    case 'R':
    case 'x':
    case 'X':
		{
			struct shortfile *bp;
                	extern struct shortfile *getbcache (char *bname);

             		bp = getbcache (currboard);
                	noreply = fileinfo->accessed[0] & FILE_NOREPLY
                          || bp->flag & NOREPLY_FLAG;
                	if (!noreply || HAS_PERM (PERM_SYSOP) || chk_currBM (currBM))
                	{
                    		local_article = !(fileinfo->filename[STRLEN - 1] == 'S');
		    		if(bp->flag & PUSH_FLAG)
					do_reply_push (fileinfo,ent,direct);
		    		else 
                    			do_reply (fileinfo->title, fileinfo->owner, fileinfo->reid);
                	}
                	else
                	{
                    		clear ();
                    		move (5, 6);
                    		prints
                    			("¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.");
                    		pressreturn ();
                	}
                	break;
        	}
    case 'y':
    case 'r':
        {
            struct shortfile *bp;
            bp = getbcache(currboard);
            noreply = fileinfo->accessed[0] & FILE_NOREPLY
                      ||bp->flag & NOREPLY_FLAG ;
            local_article=(fileinfo->filename[STRLEN-2]=='L');
            if(!noreply || HAS_PERM(PERM_SYSOP) ||chk_currBM(currBM)){
                do_reply(fileinfo->title,fileinfo->owner, fileinfo->reid);
/*		if(bp->flag & PUSH_FLAG)
			do_reply(fileinfo->title,fileinfo->owner, fileinfo->reid); 
		else 
		    do_reply(fileinfo->title,fileinfo->owner, fileinfo->reid);*/
	    } else
            {
                clear();
                prints("\n\n    ¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.    ");
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
                    //ÕâÀïÊÇÔÚÎÄÕÂÁÐ±íÇ°Ãæ²éÑ¯Ê¹ÓÃÕßµÄctrl a
                    //µ÷ÓÃÁËshow authorº¯Êý
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
skip_post(int ent, struct fileheader *fileinfo, char *direct)
{
    brc_addlist(fileinfo->filename);
    return GOTO_NEXT;
}

int
do_select(int ent, struct fileheader *fileinfo, char *direct)
{
    char    bname[STRLEN], bpath[STRLEN];
    struct stat st;
    //nextline add by alt@sjtu
    if (digestmode==9)
        return DONOTHING;
    move(0, 0);
    clrtoeol();
    prints("Ñ¡ÔñÒ»¸öÌÖÂÛÇø (Ó¢ÎÄ×ÖÄ¸´óÐ¡Ð´½Ô¿É)\n");
    prints("ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ");
    clrtoeol();

    make_blist();
    namecomplete((char *) NULL, bname);
    if(*bname == '\0')
        return FULLUPDATE;
    setbpath(bpath, bname);
    if (/* (*bname == '\0') || */(stat(bpath, &st) == -1))
    {
        move(2, 0);
        prints("²»ÕýÈ·µÄÌÖÂÛÇø.\n");
        pressreturn();
        return FULLUPDATE;
    }
    if (!(st.st_mode & S_IFDIR))
    {
        move(2, 0);
        prints("²»ÕýÈ·µÄÌÖÂÛÇø.\n");
        pressreturn();
        return FULLUPDATE;
    }
#ifdef BOARD_CONTROL
	//modified by rwzmm @sjtubbs
	//ÖÙ²Ã°åÃæÔÄ¶ÁÏÞÖÆ
    if((!HAS_PERM(PERM_SYSOP)) || (!strncmp(bname, "Arbitrator", 10))
		|| (!strncmp(currboard, "OscarAward", 10)))
    {
        setbfile(genbuf, bname, "board.allow");
        if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid))
        {
            clear();
            move(5,10);
            prints("ºÜ±§Ç¸, ±¾°åÃæÊôÓÚÍÅÌå°åÃæ£¬¶øÄú²»ÊÇ¸Ã°å³ÉÔ±");
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
do_acction(int type)
{
    char    buf[STRLEN];
    move(t_lines - 1, 0);
    clrtoeol();
    prints("\033[1;5mÏµÍ³´¦Àí±êÌâÖÐ, ÇëÉÔºò...\033[m\n");
    refresh();
    switch(type)
    {
    case 2:
        sprintf(buf, "bin/thread %s", currboard);
        system(buf);
        break;
    case 3:
    case 4:
    case 5: /* Í¬×÷Õß */
    case 6: /* Í¬×÷Õß  ¾«È· */
    case 7: /* ±êÌâ¹Ø¼ü×Ö */
        marked_all(type-3);
        break;
    }
}

int acction_mode(void)
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
        //{ »ØÊÕÕ¾, change by mudboy
        //		        getdata(t_lines - 1, 0, "ÇÐ»»Ä£Ê½µ½: 1)ÎÄÕª 2)Í¬Ö÷Ìâ 3)±» m ÎÄÕÂ 4)Ô­×÷ 5)Í¬×÷Õß 6)±êÌâ¹Ø¼ü×Ö [1]: ", ch, 3, DOECHO, YEA);
        if (!chk_currBM(currBM))
            getdata(t_lines - 1, 0, "ÇÐ»»Ä£Ê½µ½: 1)ÎÄÕª 2)Í¬Ö÷Ìâ 3)±» m ÎÄÕÂ 4)Ô­×÷ 5)Í¬×÷Õß 6)±êÌâ¹Ø¼ü×Ö [1]: ", ch, 3, DOECHO, YEA);
        else
            getdata(t_lines - 1, 0, "ÇÐ»»Ä£Ê½µ½:1)ÎÄÕª2)Í¬Ö÷Ìâ3)±»mÎÄÕÂ4)Ô­×÷5)Í¬×÷Õß6)±êÌâ¹Ø¼ü×Ö9)»ØÊÕÕ¾ [1]: ", ch, 3, DOECHO, YEA);
        //}
        if ( ch[0] == '\0' )
            ch[0] = '1';
        type = atoi(ch);

        //{ »ØÊÕÕ¾, change by mudboy
        //	        if (type < 1 || type > 6 ) {
        if (type < 1 || (type > 6 && (type != 9)) || ((type == 9) && (!chk_currBM(currBM))))
        {
            //}
            saveline(t_lines - 1, 1);
            return PARTUPDATE;
        }
        else if ( type == 6 )
        {
            getdata(t_lines-1,0,"ÄúÏë²éÕÒµÄÎÄÕÂ±êÌâ¹Ø¼ü×Ö: ",
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
            getdata(t_lines-1,0,"ÄúÏë²éÕÒÄÄÎ»ÍøÓÑµÄÎÄÕÂ? ",someoneID,13,DOECHO,YEA);
            if(someoneID[0] == '\0' )
            {
                saveline(t_lines -1 , 1);
                return PARTUPDATE;
            }
            getdata(t_lines-1,37, "¾«È·²éÕÒ°´ Y£¬ Ä£ºý²éÕÒÇë»Ø³µ[Enter]",ch,2,DOECHO,YEA);
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
int dele_digest_top(char *dname, char *direc, char *flag)
/*end*/
              
              
             	             
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
        return -1;
    }
    delete_file(new_dir, sizeof(struct fileheader), pos, cmpfilename);
    strncpy(currfile, buf,STRLEN);
    *ptr = '\0';
    sprintf(buf, "%s%s/%s", new_dir, getfolderstr(digest_name), digest_name);
    unlink(buf);
    return 0;
}

int
digest_post(int ent, struct fileheader *fhdr, char *direct)
{

    // »ØÊÕÕ¾, nextline add by mudboy
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
        sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(digest.filename), digest.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        digest.accessed[0] = 0;
        sprintf(&genbuf[512], "%s/%s/%s", buf, getfolderstr(fhdr->filename), fhdr->filename);
        link(&genbuf[512], genbuf);
        strcpy(ptr, DIGEST_DIR);
        if (get_num_records(buf, sizeof(digest)) > MAX_DIGEST)
        {
            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("±§Ç¸£¬ÄãµÄÎÄÕªÎÄÕÂÒÑ¾­³¬¹ý %d Æª£¬ÎÞ·¨ÔÙ¼ÓÈë...\n", MAX_DIGEST);
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
do_reply(char *title,char *id,unsigned int reid)
{
    strcpy(replytitle, title);
    post_article(currboard,id, &reid,NA);
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int
garbage_line(char *str)
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
        if (strstr(str, "Ìáµ½:\n") || strstr(str, ": ¡¿\n") || strncmp(str, "==>", 3) == 0 || strstr(str, "µÄÎÄÕÂ ¡õ"))
            return 1;
    return (*str == '\n');
}
/* this is a ÏÝÚå for bad people to cover my program to his */

int
Origin2(char *text)
{
    char    tmp[STRLEN];
    sprintf(tmp, ":¡¤%s %s¡¤[FROM:", BoardName, BBSHOST);
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
    FILE   *module;

    qfile = quote_file;
    quser = quote_user;
    bflag = strncmp(qfile, "mail", 4);
    outf = fopen(filepath, "a"); 
//added by chaser 06.05.08
    if(*module_file!='\0')
    {
	strcat(module_file,"_M");
	setmodulefile(buf,currboard,module_file);
        if((module=fopen(buf,"r"))!=NULL)
        {
	    while(fgets(buf,256,module) !=NULL)
	   {
		fprintf(outf,buf);
	   }
	   fclose(module);
	   *module_file='\0';
        }
     }
//
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
                fprintf(outf, "\n¡¾ ÔÚ %-.55s µÄ´ó×÷ÖÐÌáµ½: ¡¿\n", quser);
            else
                fprintf(outf, "\n¡¾ ÔÚ %-.55s µÄÀ´ÐÅÖÐÌáµ½: ¡¿\n", quser);
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
                            fprintf(outf,": .................£¨ÒÔÏÂÊ¡ÂÔ£©");
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
    int	    cross_trace_level = 1;

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
            write_header(of, 1 /* ²»Ð´Èë .posts */ );
            in_mail = YEA;
        }
        else
            write_header(of, 1 /* ²»Ð´Èë .posts */ );
        if (fgets(buf, 256, inf) != NULL)
        {
            if(strncmp(&buf[2],"ÐÅÈË: ",6))
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
            fprintf(of, "\033[1;37m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô \033[32m%s \033[37mµÄÐÅÏä ¡¿\n",
                    currentuser.userid);
        else
            fprintf(of, "\033[1;37m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô \033[32m%s \033[37mÌÖÂÛÇø ¡¿\n",
                    quote_board);
        if(owner_found)
        { /* Clear Post header */
            if(fgets(buf, 256, inf) != NULL)
                if (buf[0] != '\n')
                    fgets(buf, 256, inf);
            fprintf(of, "¡¾ Ô­ÎÄÓÉ\033[32m %s\033[37m Ëù·¢±í ¡¿\033[m\n", owner);
        }
        else
            fseek( inf, (long) 0, SEEK_SET);
    }
    else if (mode == 1)
    {
        fprintf(of,"\033[1;41;33m·¢ÐÅÈË: %s (×Ô¶¯·¢ÐÅÏµÍ³), ÐÅÇø: %s%*s\033[m\n",
                BBSID,quote_board,48-strlen(BBSID)-strlen(quote_board)," ");
        fprintf(of, "±ê  Ìâ: %s\n", quote_title);
        getdatestring(now,NA);
        fprintf(of, "·¢ÐÅÕ¾: %s\033[33m×Ô¶¯·¢ÐÅÏµÍ³\033[m(%s)\n\n",
                BoardName, datestring);
    }
    else if (mode == 2)
    {
        write_header(of, 0 /* Ð´Èë .posts */ );
    }
    while (fgets(buf, 256, inf) != NULL)
    {
        if (cross_trace_level <= 0)
        {
            if (strstr(buf,"¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô ") && strstr(buf, "ÌÖÂÛÇø ¡¿") ||
                strstr(buf, "¡¾ Ô­ÎÄÓÉ") && strstr(buf, "Ëù·¢±í ¡¿"))
                continue;
        }
	else
	{
            if (strstr(buf, "¡¾ Ô­ÎÄÓÉ") && strstr(buf, "Ëù·¢±í ¡¿"))
	        --cross_trace_level;
	}
        fprintf(of, "%s", buf);
    }
    fclose(inf);
    fclose(of);
    *quote_file = '\0';
}


int
do_post(void)
{
    *quote_file = '\0';
    *quote_user = '\0';
    local_article = YEA;
    return post_article(currboard,(char *)NULL, NULL,0);
}
/*ARGSUSED*/
int
post_reply(int ent, struct fileheader *fileinfo, char *direct)
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
        if (strncmp(t, "·¢ÐÅÈË", 6) == 0 ||
                strncmp(t, "¼ÄÐÅÈË", 6) == 0 ||
                strncmp(t, "Posted By", 9) == 0 ||
                strncmp(t, "×÷  Õß", 6) == 0)
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
            prints("¶Ô²»Æð£¬¸ÃÕÊºÅÒÑ¾­²»´æÔÚ¡£\n");
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
        prints("ÏµÍ³ÎÞ·¨ËÍÐÅ\n");
        break;
    case -2:
        prints("ËÍÐÅ¶¯×÷ÒÑ¾­ÖÐÖ¹\n");
        break;
    case -3:
        prints("Ê¹ÓÃÕß '%s' ÎÞ·¨ÊÕÐÅ\n", uid);
        break;
    case -4:
        prints("Ê¹ÓÃÕß '%s' ÎÞ·¨ÊÕÐÅ£¬ÐÅÏäÒÑÂú\n",uid);
        break;
    default:
        prints("ÐÅ¼þÒÑ³É¹¦µØ¼Ä¸øÔ­×÷Õß %s\n", uid);
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
        prints("\n\nÄúÉÐÎÞÈ¨ÏÞÔÚ %s °å·¢±íÎÄÕÂ£¬È¡Ïû×ªÔØ\n", currboard);
        return -1;
    }
	setbfile(tempbuffer, currboard, "board.allow");
	if((dashf(tempbuffer)&&!seek_in_file(tempbuffer,currentuser.userid)) && !mode)
	{
        prints("\n\nÄú²»ÊÇ %s °å³ÉÔ±£¬È¡Ïû×ªÔØ\n", currboard);
        return -1;
	}
		
    memset(&postfile, 0, sizeof(postfile));
    now = time(0);
    sprintf(fname, "M.%ld.A", now);
    if (!mode)
    {
        if (!strstr(quote_title,"(×ªÔØ)"))
            sprintf(buf4,"%.70s(×ªÔØ)",quote_title);
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
        sprintf(fname, "M.%ld.A", now);
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
add_crossinfo(char *filepath, int mode)
{
    FILE   *fp;
    int     color;
    color = (currentuser.numlogins % 7) + 31;
    if ((fp = fopen(filepath, "a")) == NULL)
        return;
    fprintf(fp, "--\n\033[m\033[1;%2dm¡ù ×ª%s:¡¤%s %s¡¤[FROM: %-.46s]\033[m\n"
            ,color, (mode == 1) ? "ÔØ" : "¼Ä", BoardName, BBSHOST, fromhost);
    fclose(fp);
    return;
}

int
show_board_notes(char *bname, int command)
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
            ansimore(buf,YEA);
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
            ansimore("vote/notes", YEA);
        }
        return 1;
    }
    return -1;
}

int
show_user_notes(void)
{
    char    buf[256];
    setuserfile(buf, "notes");
    if (dashf(buf))
    {
        ansimore(buf,NA);
        return FULLUPDATE;
    }
    clear();
    move(10, 15);
    prints("ÄúÉÐÎ´ÔÚ InfoEdit->WriteFile ±à¼­¸öÈË±¸ÍüÂ¼¡£\n");
    pressanykey();
    return FULLUPDATE;
}


void
outgo_post(struct fileheader *fh, char *board)
{
    char    buf[256];
    sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board,
            fh->filename, header.chk_anony ? board : currentuser.userid,
            header.chk_anony ? "ÎÒÊÇÄäÃûÌìÊ¹" : currentuser.username, save_title);
    file_append("innd/out.bntp", buf);
	return;
}

//created by rwzmm @ sjtubbs. ÐÂ×¢²áID 24Ð¡Ê±ÄÚ²»ÄÜPOST
int	too_young(void)
{
	if(strcmp(currboard, "BBSHelp") == 0)
		return 0;

	if(strcmp(currboard, "sysop") == 0)
		return 0;

	if((time (0) - currentuser.firstlogin) > 86400)
		return 0;
	
	return 1;
}

#ifdef KEYWORD_FILTER
void keywordfilter(char* filepath, struct BCACHE* pCache)
{
	FILE* fp = NULL;
	struct stat st;
	int item = 0;
	char* filecontent= NULL;
	char* noheadcontent = NULL;
	char* ptr;
	int i = 0;
	int k = 0;
	if(pCache == NULL)
		return;
	if(filepath == NULL)
		return;
	if(stat(filepath, &st) <0)
		return;
	filecontent = malloc(st.st_size+1);
	memset(filecontent, 0x00, st.st_size+1);
	if(filecontent == NULL)
		return;
	fp = fopen(filepath, "r");
	if(fp == NULL)
	{
		free(filecontent);
		return;
	}
	fread(filecontent, st.st_size, 1, fp);
	fclose(fp);

	noheadcontent = filecontent;
	k=0;
	while(k<3)
	{
		if((*noheadcontent) == '\n')
		{
			k++;
		}	
		noheadcontent ++;
		if(noheadcontent - filecontent >= st.st_size)//this file is less than 3lines?
		{
			noheadcontent = filecontent;
			break;
		}
	}
	
	for(item=0; item<MAX_ITEM; item++)
	{
		if(strlen(pCache->wordfilter[item]) < 4)
			break;
		ptr = NULL;
		while((ptr = strstr(noheadcontent, pCache->wordfilter[item])))
		{
			for(i=0; i<(int)strlen(pCache->wordfilter[item]); i++)
			{
				ptr[i] = '*';
			}
		}
	}
	fp = fopen(filepath, "w");
	fwrite(filecontent, st.st_size, 1, fp);
	fclose(fp);
	free(filecontent);
	return;
}
#endif

/*#define NOPOST_OUTSIDE */
int
post_article(char* postboard, char* mailid, unsigned int* p_reid,int is_module)//add the parameter(int is_module) to post modulefile
{
    struct fileheader postfile;
    struct shortfile *bp;
	extern struct BCACHE *brdshm;
    char    filepath[STRLEN], fname[STRLEN], buf[STRLEN];
    int     fp, aborted, count;
    time_t  now = time(0);
    static  time_t  lastposttime = 0;
    static  int  failure = 0;
	struct tm *tm;
//	int item = 0;
	tm = localtime (&now);
	if(brdshm->all_no_post_end >= brdshm->all_no_post_begin) {
	if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8)) 
	{
		
		if((unsigned int)tm->tm_hour >= brdshm->all_no_post_begin && (unsigned int)tm->tm_hour < brdshm->all_no_post_end) 
		{
			move(5,10);
			clrtobot();
			prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±È«Õ¾Ö»¶Á¡£", brdshm->all_no_post_begin, brdshm->all_no_post_end);
			pressreturn();
        		clear();
        		return FULLUPDATE;	
			}


		}
	
    if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8|PERM_BOARDS)) 
	{
		if((unsigned int)tm->tm_hour >= brdshm->no_post_begin && (unsigned int)tm->tm_hour < brdshm->no_post_end) 
		{
			if( !insjtunetwork(fromhost) ) 
			{
				move(5, 10);
        			clrtobot();
				prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",brdshm->no_post_begin,brdshm->no_post_end);
				pressreturn();
        			clear();
        			return FULLUPDATE;			
			}		
		}
    }
		} else {
				if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8)) 
	{
		
		if((unsigned int)tm->tm_hour >= brdshm->all_no_post_begin || (unsigned int)tm->tm_hour < brdshm->all_no_post_end) 
		{
			move(5,10);
			clrtobot();
			prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±È«Õ¾Ö»¶Á¡£", brdshm->all_no_post_begin, brdshm->all_no_post_end);
			pressreturn();
        		clear();
        		return FULLUPDATE;	
			}


		}
	
    if(!HAS_PERM(PERM_SYSOP|PERM_SPECIAL6|PERM_SPECIAL7|PERM_SPECIAL8|PERM_BOARDS)) 
	{
		if((unsigned int)tm->tm_hour >= brdshm->no_post_begin || (unsigned int)tm->tm_hour < brdshm->no_post_end) 
		{
			if( !insjtunetwork(fromhost) ) 
			{
				move(5, 10);
        			clrtobot();
				prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",brdshm->no_post_begin,brdshm->no_post_end);
				pressreturn();
        			clear();
        			return FULLUPDATE;			
			}		
		}
    }


			}
	modify_user_mode(POSTING);
    //added by soff ·ÀÖ¹¹àË®»ú 2000.10.08
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
            prints("¶Ô²»Æð£¬ÄúÔÚ±»È°×è¶à´ÎµÄÇé¿öÏÂ£¬ÈÔ²»¶ÏÊÔÍ¼·¢ÎÄ¡£");
            move(6,10);
            prints("ÄúÄ¿Ç°±»ÏµÍ³ÈÏ¶¨Îª¹àË®»ú£¬ÇëÍË³öºóÖØÐÂµÇÂ½£¡[%d/20]",
                   failure - 9999);
        }
        else
        {
            prints("ÄúÌ«ÐÁ¿àÁË£¬ÏÈºÈ±­¿§·ÈÐª»á¶ù£¬3 ÃëÖÓºóÔÙ·¢±íÎÄÕÂ¡£\n");
            if( failure > 5 )
            {
                move(6,10);
                prints("ÄúÔÚ±»È°×èµÄÇé¿öÏÂÈÔ¾ÉÊÔÍ¼·¢±íÎÄÕÂ¡£[%d/20]",
                       failure-5);
                if (failure >= 25)
                {
                    securityreport("¶à´ÎÊÔÍ¼·¢±íÎÄÕÂ£¬±»ÏµÍ³ÅÐ¶¨Îª¹àË®»ú");
                    failure = 9999;
                }
            }
            lastposttime = now;
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    }
	//if(digestmode > 2) //edit by clist
	//{
	//	move(5, 10);
	//	clrtobot();
	//	prints("Ä¿Ç°ÊÇÎÄÕª, ²»ÄÜ·¢±íÎÄÕÂ (°´ ENTER ºóÔÙ°´×ó¼ü¿ÉÀë¿ª´ËÄ£Ê½)¡£");
	//	pressreturn();
	//	clear();
	//	return FULLUPDATE;
	//}
	if(!haspostperm(postboard) && (strcmp(currboard,"WelcomeNewer")!= 0 || strcmp(currentuser.userid,"guest") == 0) )
    {
        move(5, 10);
        clrtobot();
        prints("´ËÌÖÂÛÇøÊÇÎ¨¶ÁµÄ, »òÊÇÄúÉÐÎÞÈ¨ÏÞÔÚ´Ë·¢±íÎÄÕÂ¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
#ifdef  REAL_NAME_EMAIL
	if(!HAS_PERM(PERM_REALNAME) && (strcmp(currboard,"WelcomeNewer")!= 0 || strcmp(currentuser.userid,"guest") == 0))
	{
//modified by proto@sjtubbs 2006.10.16
	firstchar=toupper(currentuser.userid[0]);
    sprintf(filename,"home/%c/%c/%s/real_email",firstchar, toupper(currentuser.userid[1]), currentuser.userid);

    if((ftemp=fopen(filename,"r"))==NULL)
	{
		move(5, 10);
        clrtobot();
		prints("ÄúÉÐÎ´°ó¶¨Email£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
	}
    else
    {
		move(5, 10);
        clrtobot();
		prints("ÄúÉÐÎ´¼¤»îEmail£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ¡£\n\n          Çëµ½ÒÔÏÂÐÅÏäÀïÊÕÐÅ²¢µã»÷¼¤»îÁ´½Ó:\n\n          ");
        while(fgets(buffer,32,ftemp))
        {
         prints ("%s\n",buffer);
        }
		prints("\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
        fclose(ftemp);
    }
//modified end by proto@sjtubbs 2006.10.16
		pressreturn();
        clear();
        return FULLUPDATE;
	}
#endif
#ifdef	POST_AFTER_ONE_DAY
	if(too_young() && (strcmp(currboard,"WelcomeNewer")!= 0 || strcmp(currentuser.userid,"guest") == 0))
	{
		move(5, 10);
		clrtobot();
		prints("Äú×¢²áÊ±¼ä»¹²»Âú24Ð¡Ê±£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ.\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
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
    	//if(bp->flag&PUSH_FLAG)
    	//{
    	//}
	//else
        //{	
        	if (strncmp(replytitle, "Re: ", 4) == 0)
            		strcpy(header.title, replytitle);
        	else
            		sprintf(header.title, "Re: %s", replytitle);
        	header.reply_mode = 1;
	//}
    }
    else
#endif

    {
        header.title[0] = '\0';
        header.reply_mode = 0;

    }
    strcpy(header.ds, postboard);
    if(is_module)//added by chaser to write module name 06.05.08
    {
	strcpy(header.title,module_name);
    }
    header.postboard = YEA;
    if (post_header(&header,is_module) == YEA)
    {
        strcpy(postfile.title, header.title);
        strncpy(save_title, postfile.title, STRLEN);
    }
    else
        return FULLUPDATE;
    now = time(0);
    lastposttime = now;
    if(is_module)//added by chaser 06.05.08
    {
	sprintf(fname,"%s%s",module_name,"_M");
    }
    else
    {
    sprintf(fname, "M.%ld.A", now);
    }
    if(!is_module)
    {
	setbfile(filepath, postboard, fname);
    }
    else
    {
	setmodulefile(filepath,postboard,fname);
    }   
    count = 0;
    if(!is_module)//added by chaser 06.05.08
    {
	 while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
            now++;
            sprintf(fname, "M.%ld.A", now);
            setbfile(filepath, postboard, fname);
            if (count++ > MAX_POSTRETRY)
            {
               return -1;
            }
        }
    }
    else
   {
	fp=open(filepath, O_CREAT | O_WRONLY, 0644);
    }    
    fchmod(fp, 0644);	/* youzi 1999.1.8 */
    close(fp);
    strcpy(postfile.filename, fname);
    in_mail = NA;
    strncpy(postfile.owner, (header.chk_anony) ?
            postboard : currentuser.userid, STRLEN);
    if(!is_module)//added by chaser
    {
	setbfile(filepath, postboard, postfile.filename);
    }
    else
    {
	setmodulefile(filepath,postboard,postfile.filename);
    }
    modify_user_mode(POSTING);
    do_quote(filepath, header.include_mode);
    if(p_reid == NULL)
    {
	if(!is_module)//added by chaser
        aborted = vedit_reid(filepath, YEA, YEA,now);
	else
	aborted = vedit_reid(filepath, NA, YEA,now);
    }
    else {
	aborted = vedit_reid(filepath, YEA, YEA,*p_reid);
    }
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
	if(is_module)//added by chaser whether to write into the module name
	{
	   writeinto_modulefile=0;
	}
        clear();
        return FULLUPDATE;
    }
    writeinto_modulefile=1;
#ifdef KEYWORD_FILTER
	keywordfilter(filepath, brdshm);
#endif
    setbdir(buf, postboard);
    if(digestmode>0)
		sprintf(buf, "boards/%s/%s", currboard, DOT_DIR);
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
postfile.accessed[2] = 0;
    if( mailtoauther )
    {
        if(header.chk_anony)
            prints("¶Ô²»Æð£¬Äú²»ÄÜÔÚÄäÃû°åÊ¹ÓÃ¼ÄÐÅ¸øÔ­×÷Õß¹¦ÄÜ¡£");
        else if(!mail_file(filepath,mailid,postfile.title))
            prints("ÐÅ¼þÒÑ³É¹¦µØ¼Ä¸øÔ­×÷Õß %s",mailid);
        else
            prints("ÐÅ¼þÓÊ¼ÄÊ§°Ü£¬%s ÎÞ·¨ÊÕÐÅ¡£",mailid);
        mailtoauther = 0;
        pressanykey();
    }
    if(!is_module)//added by chaser
    {
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
    }

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
    int		newtitle = 0 ;

    if ((fp = fopen(fname, "r")) == NULL)
        return 0;
	
    sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid, uinfo.pid);
    if ((out = fopen(outname, "w")) == NULL)
        return 0;
	while ((fgets(buf, 256, fp)) != NULL)  
	{  
	   if (!strncmp(buf, "±ê  Ìâ: ", 8) && newtitle ==0)  
	   {  
		   fprintf(out, "±ê  Ìâ: %s\n", title);  
		   newtitle = 1;  
		   continue;  
	   }  
	   fputs(buf, out);  
	}
    fclose(fp);
    fclose(out);
    rename(outname, fname);
    chmod(fname, 0644);
    return 1;
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

	if (in_mail)
		sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
	else
		sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(fileinfo->filename), fileinfo->filename);


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

void getnam(char* direct, int num, char* id)
{
    FILE *fp;
    int size;
    struct fileheader ff;
    size = sizeof(struct fileheader);
    fp=fopen(direct,"r");
	if(fp==NULL) return;
    fseek(fp,(num-1)*size,SEEK_SET);
    strcpy(id,"none.");
    if (fread(&ff,size,1,fp))
        strcpy(id,ff.filename);
    fclose(fp);
}

int
edit_title(int ent, struct fileheader *fileinfo, char *direct)
{
//////////////////////////added by proto@sjtubbs
	FILE *fp;
    char *currenttime;
//////////////////////////added end by proto@sjtubbs
    struct stat st;
    char    buf[STRLEN];
    char buftitle[STRLEN];
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
	if((int)(st.st_size/sizeof(struct fileheader)) < ent)
	{	
		//modify the title of the top article
		//modified by rwzmm @ sjtubbs
		if (!in_mail && !chk_currBM(currBM))
			return  DONOTHING;
		//modify end
		top_mode = 1; 
		setbfile(dirpath, currboard, DOT_DIR);
		stat(dirpath, &dirst);
		real_file_num = dirst.st_size/sizeof(struct fileheader);
		top_ent = ent - real_file_num;
		move(t_lines - 1, 0);
		clrtoeol();
		if(askyn("ÄãÈ·ÈÏÒªÐÞ¸ÄÖÃµ×ÎÄÕÂ±êÌâÂð", NA, NA) == NA)
			return PARTUPDATE;
	}


    strcpy(buf,fileinfo->title);
    getdata(t_lines - 1, 0, "ÐÂÎÄÕÂ±êÌâ: ", buf, 50, DOECHO, NA);
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

		if(top_mode == 0)
		{
			substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
		}
		else
		{
			setbfile(toppath, currboard, TOPFILE_DIR);
			substitute_record(toppath, fileinfo, sizeof(*fileinfo), top_ent);
		}

////////////////////////////////////////////////////added by proto@sjtubbs
		sprintf(buftitle, "boards/%s/%s/%s", currboard, getfolderstr(fileinfo->filename), fileinfo->filename);
		change_title(buftitle, buf);
		if ((fp = fopen(buftitle, "a+")) == NULL)
			return 0;
		getdatestring(time(0), NA);
		currenttime=datestring+6;
	    
		fprintf (fp,
				 "\n[1;36m¡ù ÐÞ¸Ä±êÌâ:¡¤%s ÓÚ %16.16s ÐÞ¸Ä±¾ÎÄ¡¤[FROM: %-.46s][m\n",
				  currentuser.userid, currenttime,
				  currentuser.lasthost);
		fclose(fp);
//////////////////////////////////////////////////added end by proto@sjtubbs
    }
   return PARTUPDATE;
}

int
underline_post(int ent, struct fileheader *fileinfo, char *direct)
{
    // »ØÊÕÕ¾, nextline add by mudboy
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
/* canceled by cf
int
makeDELETEDflag(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
    // »ØÊÕÕ¾, nextline add by mudboy
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
*/
/* add by rwzmm @ sjtubbs, show file info*/
int
show_file_info(int ent, struct fileheader *fileinfo, char *direct)
{
	char url[512] = {0};
	char filepath[STRLEN] = {0};

/*	char output[STRLEN]= {0};*/
	struct stat file_data;

	if(digestmode != 0)
		return DONOTHING;
	sprintf(filepath, BBSHOME"/boards/%s/%s/%s", currboard, getfolderstr(fileinfo->filename), fileinfo->filename);
	if(fileinfo->filename[0] == 'T')
	{
		sprintf(url, "http://"BBSHOST"/bbstopcon?board=%s&file=%s", currboard, fileinfo->filename);
	}
	else
	{
		sprintf(url, "http://"BBSHOST"/bbscon?board=%s&file=%s", currboard, fileinfo->filename);
	}
	if(stat(filepath, &file_data)<0)
	{
		clear();
		move(10, 30);
		prints("¶Ô²»Æð£¬µ±Ç°ÎÄÕÂ²»´æÔÚ£¡\n", filepath);
		pressanykey();
		clear();
		return FULLUPDATE;
	}
	clear();
	move(0, 0);
	prints("ÒûË®Ë¼Ô´BBSÎÄÕÂÐÅÏ¢\n\n");
	prints("ÎÄÕÂ±êÌâ: %s\n\n", fileinfo->title);
	prints("ÎÄÕÂ×÷Õß: %s\n\n", fileinfo->owner);
	prints("ÎÄÕÂ´óÐ¡: %d ×Ö½Ú\n\n", file_data.st_size);
	prints("ÎÄÕÂÁ´½Ó: \n%s\n\n", url);
	pressanykey();
	clear();
	return FULLUPDATE;
}
/* add end */

int
mark_post(int ent, struct fileheader *fileinfo, char *direct)
{
    // »ØÊÕÕ¾, nextline add by mudboy
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
int
recommend_post(int ent, struct fileheader *fileinfo, char *direct)
{
#ifdef LOGHANDLER
	char query_string[4096],title_ok[256];
	int sqllen = 0;
#endif
    // »ØÊÕÕ¾, nextline add by mudboy
    // if (digestmode==9) return DONOTHING;
    if (!HAS_PERM(PERM_SPECIAL7))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[1] & FILE_RECOMMENDED) 
    {
        fileinfo->accessed[1] &= ~FILE_RECOMMENDED;
#ifdef LOGHANDLER
		/*del post from recommend table*/
		sqllen = snprintf(query_string,sizeof(query_string),LOGHANDLER_SQL"DELETE FROM `recommend` WHERE `board`='%s' AND `reid`='%d' AND filename = '%s'\n",currboard,fileinfo->reid,fileinfo->filename);
		loghander_write(query_string,sqllen);
#endif

    }
    else
    {
        fileinfo->accessed[1] |= FILE_RECOMMENDED;
        fileinfo->accessed[0] &= ~FILE_DELETED;
		
#ifdef LOGHANDLER
		/*add post into recommend table,added by cf,20050511*/
		mysql_escape_string(title_ok, fileinfo->title,strlen(fileinfo->title));
		sqllen = snprintf(query_string,sizeof(query_string),LOGHANDLER_SQL"INSERT INTO `recommend` ( `id` , `board` , `user_id` , `reid` , `title` ,`filename`,  `height` ) VALUES ('', '%s', '%s/%s', '%d', '%s', '%s', '0')\n",currboard,fileinfo->owner,currentuser.userid, fileinfo->reid,title_ok, fileinfo->filename);
		loghander_write(query_string,sqllen);
#endif
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    sleep(1);
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
    //»ØÊÕÕ¾, nextline changed by mudboy
    if (digestmode > 1)
        return DONOTHING;
    //   if (digestmode > 1 && digestmode!=9) return DONOTHING;
    getdata(t_lines - 1, 0, "Ê×ÆªÎÄÕÂ±àºÅ: ", num, 6, DOECHO, YEA);
    inum1 = atoi(num);
    if (inum1 <= 0)
    {
        move(t_lines - 1, 50);
        prints("´íÎó±àºÅ...");
        egetch();
        return PARTUPDATE;
    }
    getdata(t_lines - 1, 25, "Ä©ÆªÎÄÕÂ±àºÅ: ", num, 6, DOECHO, YEA);
    inum2 = atoi(num);
    if (inum2 < inum1 + 1)
    {
        move(t_lines - 1, 50);
        prints("´íÎóÇø¼ä...");
        egetch();
        return PARTUPDATE;
    }
    move(t_lines - 1, 50);
    if (askyn("È·¶¨É¾³ý", NA, NA) == YEA)
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
			sprintf(titlebuf, "%sÔÚ%s°åÇø¶ÎÉ¾ÎÄ %d-%d", currentuser.userid, currboard, inum1, inum2);
			sprintf(content, "\n\t\t%s ÔÚ %s °åÊµ¼ÊÇø¶ÎÉ¾³ýÎÄÕÂ %d Æª¡£\n\n\t\tÎÄÕÂÔÚ»ØÊÕÕ¾µÄÎ»ÖÃ£º%d Æª £­ %d Æª¡£\n", 
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
    prints("·ÅÆúÉ¾³ý...");
    egetch();
    return PARTUPDATE;
}
int
UndeleteArticle(int ent, struct fileheader *fileinfo, char *direct)  /* undelete Ò»ÆªÎÄÕÂ Leeward 98.05.18 */
        
                            
             
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
    sprintf(buf, "boards/%s/%s/%s", currboard, getfolderstr(fileinfo->filename), fileinfo->filename);
    fp = fopen(buf, "r");
    if (!fp)
        return DONOTHING;

    strcpy(UTitle, fileinfo->title);
    if ((p = strrchr(UTitle, '-')))
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
        if (strstr(buf, "·¢ÐÅÈË: ") && (p = strstr(buf, "), ÐÅÇø: ")))
        {
            i ++;
            strcpy(UBoard, p + 9);
            if ((p = strchr(UBoard, ' ')))
                *p = 0;
            else if ((p = strchr(UBoard, '\n')))
                *p = 0;
        }
        else if (strstr(buf, "±ê  Ìâ: "))
        {
            i ++;
            strcpy(UTitle, buf + 8);
            if ((p = strchr(UTitle, '\n')))
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
            prints("±¾ÎÄÒÑ»Ö¸´¹ýÁË\n");
            pressreturn();
        }
        return FULLUPDATE;
    }

    UFile.accessed[0] = 0;
    strcpy(UFile.owner, fileinfo->owner);
    strcpy(UFile.title, UTitle);
    strcpy(UFile.filename, fileinfo->filename);

    sprintf(buf, "/bin/cp -f boards/%s/%s/%s boards/%s/%s/%s",
            currboard, getfolderstr(fileinfo->filename), fileinfo->filename, UBoard, getfolderstr(UFile.filename), UFile.filename);
    system(buf);

    sprintf(buf, "boards/%s/.DIR", UBoard);
    append_record(buf, &UFile, sizeof(UFile));

    fileinfo->accessed[0] |= FILE_FORWARDED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    sprintf(buf,"undeleted %s's ¡°%s¡± on %s", UFile.owner, UFile.title, UBoard);
    report(buf);
    if(SR_BMDELFLAG != YEA)
    {
        clear();
        move(2,0);
        prints("'%s' ÒÑ»Ö¸´µ½ %s °å \n", UFile.title, UBoard);
        pressreturn();
    }
    return FULLUPDATE;
}
#ifdef LOGHANDLER
/* loghandler log file fd */
static int loghandler_fd = -1;
/* timeindex of current log file fd */
static int loghandler_timeindex = -1;
static int
loghandler_open_new(void)
{
	int randomindex,timeindex;
	char path[1024];
	randomindex = getpid()%LOGHANDLER_RANDOMINDEX_NUM;
	timeindex = \
		time(NULL)/LOGHANDLER_ROTATING_TIME%LOGHANDLER_TIMEINDEX_NUM;
	snprintf(path,1024,LOGHANDLER_PATH_FORMT,timeindex,randomindex);
	if(loghandler_fd > -1)
	{
		close(loghandler_fd);
	}
	loghandler_fd = open(path,O_CREAT|O_WRONLY|O_APPEND);
	if(loghandler_fd > -1)
	{
		loghandler_timeindex = timeindex;
	}
	return loghandler_fd;
}
int
loghandler_get_fd(void)
{
	int timeindex;
	if(loghandler_fd < 0)
	{
		return loghandler_open_new();
	}	
	timeindex = \
		time(NULL)/LOGHANDLER_ROTATING_TIME%LOGHANDLER_TIMEINDEX_NUM;
	if(timeindex != loghandler_timeindex)
	{
		return loghandler_open_new();
	}
	return loghandler_fd;
}
int
loghander_write(char *line, int len)
/*sql: must be ended with '\n'*/
{
	int fd;
	if ('\n' != line[len-1]) return -2;
	fd = loghandler_get_fd();
	if(fd<0)
		return -1;
	else
	{
		return write(fd,line,len);
	}
}
int
loghandler_writev(int linenum, ...)
/* write multi-line logs at a time by an atomic way
 * linenum: how many lines to write
 * following format:
 * char * line, int len (,char * line, int len)+
 * line: the line to write
 * len: len of the line, must be ended with '\n'
 * return bytes being written, -1 on ERROR
 */
{
	va_list ap;
	struct iovec *pvec;
	int i, ret=0, fd, len;
	char *line;
	fd = loghandler_get_fd();
	if(fd<0) return -1;
	pvec = (struct iovec *) malloc(linenum*sizeof(struct iovec));
	if(!pvec) return -1;
	va_start(ap,linenum);
	for(i=0;i<linenum;++i)
	{
		line = va_arg(ap,char *);
		len = va_arg(ap,int);
		if (line[len-1] != '\n')
		{
			ret = -1;
			break;
		}
		pvec[i].iov_base = line;
		pvec[i].iov_len = len;
	}
	va_end(ap);
	if(ret<0)
	{ 
		free(pvec);
		return ret;
	}
	ret = writev(fd,pvec,linenum);
	free(pvec);
	return ret;
}

int
mysql_escape_string(char *dst, char *src, int len)
{
	int i;
	char *tmpdst = dst;
	for(i=0;i<len;i++)
	{
		switch(src[i])
		{
			case '\n':
				*tmpdst++='\\'; *tmpdst++='n'; break;
			case '\\':
				*tmpdst++='\\'; *tmpdst++='\\'; break;
			case '\r':
				*tmpdst++='\\'; *tmpdst++='r'; break;
			case '\0':
				*tmpdst++='\\'; *tmpdst++='0'; break;
			case '\'':
				*tmpdst++='\\'; *tmpdst++='\''; break;
			case '\"':
				*tmpdst++='\\'; *tmpdst++='\"'; break;
			case '\b':
				*tmpdst++='\\'; *tmpdst++='b'; break;
			case '\t':
				*tmpdst++='\\'; *tmpdst++='t'; break;
			case 0x1A: /* ctrl+Z */
				*tmpdst++='\\'; *tmpdst++='Z'; break;
			case '%':
				*tmpdst++='\\'; *tmpdst++='%'; break;
			case '_':
				*tmpdst++='\\'; *tmpdst++='_'; break;
			default:
				*tmpdst++=src[i]; break;
		}
	}
	*tmpdst++='\0';
	return tmpdst - dst -1;
}
int
del_log2db(char *user_id, char *filename,char *board_name)
{
	char query_string[4096];
	int len;
	len = snprintf(query_string,sizeof(query_string),LOGHANDLER_SQL"insert into junk (del_user,time,filename,board) values ('%s',FROM_UNIXTIME(%ld),'%s','%s')\n",user_id,time(NULL),filename,board_name);
	return loghander_write(query_string,len);
}
#endif

/*int del_push(int ent, struct fileheader *fileinfo, char *direct)
{
	char pfilename[80],filepath[80],askbuf[128];

	if (!chk_currBM(currBM))
    	{
                return DONOTHING;
    	}
   	sprintf(askbuf, "É¾³ýËùÓÐÍÆÎÄ [%-.55s]", fileinfo->title);
       if (askyn(askbuf, NA, YEA) == NA)
       {
       	move(t_lines - 1, 0);
       	prints("·ÅÆúÉ¾³ýÍÆÎÄ...");
       	clrtoeol();
       	egetch();
       	return PARTUPDATE;
       }
    	
	snprintf(pfilename,80,"%s",fileinfo->filename);
	pfilename[0]='P';
	snprintf(filepath, 80,"boards/%s/%s", currboard, pfilename);
	if(remove(filepath)!=0)
		return -1;
	fileinfo->pnum=0;
	substitute_record(direct, fileinfo, sizeof(struct fileheader), ent);
}
*/
int del_pushsure(int ent, struct fileheader *fileinfo, char *direct)
{
	char pfilename[80],filepath[80]/*,askbuf[128]*/;

	if (!chk_currBM(currBM))
    	{
                return DONOTHING;
    	}
	snprintf(pfilename,80,"%s",fileinfo->filename);
	pfilename[0]='P';
	snprintf(filepath, 80,"boards/%s/%s/%s", currboard, getfolderstr(pfilename), pfilename);
	if(remove(filepath)!=0)
		return -1;
	fileinfo->pnum=0;
	substitute_record(direct, fileinfo, sizeof(struct fileheader), ent);
	return 0;
}

int del_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char    buf[512];
    char    usrid[STRLEN];
    char   *t;
    int     owned=0, fail, IScurrent=0;
    int 	posttime;
    /*changed by cf*/
    if (fileinfo->accessed[0] & (FILE_MARKED|FILE_DIGEST|FILE_TOP1))
        return DONOTHING;
    // if (fileinfo->accessed[0] & (FILE_MARKED | FILE_DIGEST | FILE_TOP1)) return DONOTHING;

    /*end*/
    //»ØÊÕÕ¾, nextline changed by mudboy
    //   if (digestmode>1||!strcmp(currboard,"deleted")||!strcmp(currboard,"junk"))
    if (digestmode>1 || ((!strcmp(currboard,"deleted")||!strcmp(currboard,"junk"))&&digestmode!=9))
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
        sprintf(genbuf, "É¾³ýÎÄÕÂ [%-.55s]", fileinfo->title);
        if (askyn(genbuf, NA, YEA) == NA)
        {
            move(t_lines - 1, 0);
            prints("·ÅÆúÉ¾³ýÎÄÕÂ...");
            clrtoeol();
            egetch();
            return PARTUPDATE;
        }
    }
	if(fileinfo->pnum>0&&fileinfo->pnum<10000)
		del_pushsure(ent,fileinfo,direct);
    strncpy(currfile, fileinfo->filename, STRLEN);
#ifndef KEEP_DELETED_HEADER

    fail = delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
#else

    fail = update_file(direct, sizeof(struct fileheader), ent, cmpfilename,
                       cpyfilename);
#endif

    if (!fail)
    {
        //{ »ØÊÕÕ¾, add by mudboy
        if (digestmode==0)
        {
            fileinfo->accessed[0] = FILE_DELETED;
            setbfile(buf,currboard,RETURN_DIR);
            sprintf(genbuf, "%-32.32s - %s", fileinfo->title, currentuser.userid);
            strncpy(fileinfo->title, genbuf, STRLEN);
            append_record(buf,fileinfo,sizeof(struct fileheader));
#ifdef LOGHANDLER            
		del_log2db(currentuser.userid, fileinfo->filename,currboard);
#endif
        }
        else
        {
            strcpy(buf, direct);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(genbuf, "Del '%s' on '%s'", fileinfo->title, currboard);
            report(genbuf);
            cancelpost(currboard, currentuser.userid, fileinfo, owned && IScurrent);
			sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(fileinfo->filename), fileinfo->filename);
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
    prints("É¾³ýÊ§°Ü...");
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
/*added by chaser to change module*/
int do_module()//
{
    char ans[3];
    struct shortfile *pb=getbcache(currboard);
    if(!chk_currBM(currBM))
        return DONOTHING;
    if(!(pb->flag2[0] & MODULE_FLAG))
     {
         move(t_lines-1, 0);
         prints("±¾°åÏÖÔÚ²»Ö§³ÖÄ£°å");
	 pressanykey();
     }
     else
     {
	struct module_index module[3];
	while(1)
	{
	    int count=0;
            clear();
	    count=showmodulename(module);
	    move(0,0);
	    prints("±à¼­·¢ÎÄÄ£°å");
	    write_module=1;
	    if(0<count && count <3)
	    {
                    getdata (1, 0,
                           "(A)Ôö¼Ó (D)É¾³ý (C)¸Ä±ä (R)È«²¿É¾³ý (E)Àë¿ª[E]: ",
                            ans, 2, DOECHO, YEA);
            }
	   else if(count == 3)
	   {
	           getdata(1,0,"(D)É¾³ý (C)¸Ä±ä (R)È«²¿É¾³ý (E)Àë¿ª[E]: ",
		            ans,2,DOECHO,YEA);
           }
           else
	   {
                   getdata (1, 0, "(A)Ôö¼Ó (E)Àë¿ª [E]: ",
                            ans, 2, DOECHO, YEA);
           }
	   if((ans[0] == 'A' || ans[0] == 'a') && count!=3)
	   {
		add_module(module,count);
	   }
	   else if((ans[0] == 'D' || ans[0] == 'd') && count!=0)
	   {
		del_module(module,count);	
	   }
	   else if((ans[0] == 'R' || ans[0] == 'r') && count!=0)
	   {
		del_all_module(module,count);
	   }
	   else if((ans[0] == 'C' || ans[0] == 'c') && count!=0)
	   {
		change_module(module,count);
	   }
	   else
		break;
	}
     }
     write_module=0;
     clear();
     return FULLUPDATE;

}
/*added by chaser to add module*/
int add_module(struct module_index *modulename, int count)
{
    char name[NAMELEN];
    getdata(2,0,"ÊäÈë±êÌâ: ", name,NAMELEN-4,DOECHO, YEA);
    if(!check_modulename(count,modulename,name)||strlen(name)==0)
    {
	move (2,0);
	clrtoeol();
	prints("±êÌâÖØ¸´»òÕßÃ»ÓÐÊäÈë");
	pressanykey();
	return NA;
    }
    else
    {
        FILE *fp;
	char buf[STRLEN];
	setmodulefile(buf,currboard,"modulename");
	clear();
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        strcpy(module_name,name);
        if(post_article(currboard,(char *)NULL, NULL,1)!=-1)
	{
	    if(writeinto_modulefile==1)
	    {
		struct module_index moduleadded;
		strcat(name,"_M");
		strcpy(moduleadded.filename,name);
		setmodulefile(buf,currboard,"modulename");
		fp=fopen(buf,"a");
                fwrite(&moduleadded,sizeof(struct module_index),1,fp);
                fclose(fp);
	        move(1,0);
		if(count == 2)
		{
		   prints("Ìí¼Ó³É¹¦,ÒÑ¾­´ïµ½ÉÏÏÞ");
		}
		else
		{
		   prints("Ìí¼Ó³É¹¦");
		}
		   pressanykey();
	    }
	}
    }
    *module_name='\0';
    return YEA;
}
/*added by chaser to delete module*/
int del_module(struct module_index *modulename, int count)
{
   char ans[3];
   char buf[STRLEN];
   FILE *fp;
   int i;
   int j;
   setmodulefile(buf,currboard,"modulename");
   getdata(2,0,"ÊäÈëÒªÉ¾³ýµÄÐòºÅ: ",ans,2,DOECHO,YEA);
   if(ans[0] == 'a' || ans[0] == 'A')
   {
	j=0;
   }
   else if((ans[0] == 'b' || ans[0] == 'B') && count>1)
   {
	j=1;
   }
   else if((ans[0] == 'c' || ans[0] == 'C') && count==3)
   {
	j=2;
   } 
   else
   {
	return NA;
   }
   i=0;
   fp=fopen(buf,"w");
   while(i<count)
   {
	strcat(modulename[i].filename,"_M");
	if(i!=j)
	{
	   fwrite(&modulename[i],sizeof(struct module_index),1,fp);
	}
	i++;
   }
   fclose(fp);
   setmodulefile(buf,currboard,modulename[j].filename);
   if(unlink(buf)==-1)
   {
   }
   return YEA;
}
/*added by chaser to delete all of module*/
int del_all_module(struct module_index *modulename, int count)
{
   char buf[NAMELEN];
   FILE *fp;
   if(askyn("È·ÈÏÈ«²¿É¾³ýÂð?",NA,NA) == NA)
   {
	return NA;
   }
   while(count>0)
   {
	strcat(modulename[count-1].filename,"_M");
	setmodulefile(buf,currboard,modulename[count-1].filename);
	unlink(buf);
	count--;
   }
   setmodulefile(buf,currboard,"modulename");
   fp=fopen(buf,"w");
   fclose(fp);
   return YEA;
}
/*added by chaser to edit module*/
int change_module(struct module_index *modulename, int count)
{
   char ans[3];
   char buf[STRLEN];
   int i;
   setmodulefile(buf,currboard,"modulename");
   getdata(2,0,"ÊäÈëÒª¸Ä±äµÄÐòºÅ: ",ans,2,DOECHO,YEA);
   if(strlen(ans) == 0)
   {
	return NA;
   }
   i=ans[0]-'a';
   if(i<count && i>=0)
   {
	time_t now=time(0);
	int fp;
	FILE *file1;
	FILE *file2;
	char filepath[STRLEN];
	char fname[STRLEN];
	char fpath[STRLEN];
	char buf[256];
	sprintf(fname,"%ld",now);
	setmodulefile(fpath,currboard,fname);
	while((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
	   now++;
	   sprintf(fname,"%ld",now);
	   setmodulefile(fpath,currboard,fname);
	}
	fchmod(fp,0644);
	close(fp);
	file1=fopen(fpath,"w");
//	j=strlen(modulename[i]);
//	modulename[i][j-1]='\0';
	strcpy(module_name,modulename[i].filename);
	strcat(modulename[i].filename,"_M");	
	setmodulefile(filepath,currboard,modulename[i].filename);
	file2=fopen(filepath,"r");
	while(fgets(buf,256,file2)!=NULL)
	{
	   fputs(buf,file1);
	}
	fclose(file1);
	fclose(file2);
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        post_article(currboard,(char *)NULL, NULL,1);
        if(writeinto_modulefile==0)
	{
	   rename(fpath,filepath);
	}
	else
	{
	   unlink(fpath);
	}
   }

   *module_name='\0';
   return YEA;

}
/*added by chaser */
int check_modulename(int num,struct module_index module[3],char *buf)
{
   int i=0;
   while(i<num)
  {
	if(strcmp(module[i].filename,buf)==0)
	{
	    return NA;
	}
	i++;
  }
  return YEA;
}
/* Added by netty to handle post saving into (0)Announce */


int Import_post(int ent, struct fileheader *fileinfo, char *direct)
{
	/*modified by pax ¶à¸öË¿Â·*/
	int opt;
	char msg[2];
	char fname[STRLEN];

	if (!chk_currBM(currBM))
		return DONOTHING;
	
	getdata(t_lines - 1, 0,"ÇëÊäÈë²Ù×÷Ë¿Â·±àºÅ 1-4(Ä¬ÈÏÎªµÚÒ»Ë¿Â·):", msg, 2, DOECHO,  YEA);

	if (msg[0] == '\n' || msg[0] == '\0')
	msg[0] = '1';
	opt = atoi(msg)-1;
	if(!isdigit(msg[0]) || opt <0 || opt > 3 )
	{
		presskeyfor ("ÊäÈë´íÎó, Çë°´<Enter>¼ÌÐø...",t_lines - 1);
		return FULLUPDATE;
	}
	else
	{
		FILE *fn;
		char annpath[512];
		if(opt==0)
		{
			sprintf(fname,".announcepath");	
		}
		else
		{
			sprintf(fname,".announcepath%d",opt);	
		}	/*end*/
		// »ØÊÕÕ¾, nextline add by mudboy
		// if (digestmode==9) return DONOTHING;
		if (!HAS_PERM(PERM_BOARDS/*|PERM_PERSONAL*/))
			return DONOTHING;
		if(fileinfo->accessed[0] & FILE_VISIT)
		{
			if(askyn("ÎÄÕÂÔø¾­·ÅÈëÁË¾«»ªÇø, ÏÖÔÚ»¹ÒªÔÙ·ÅÈëÂð",YEA,YEA)==NA)
				return 1;
		}
		else
		{
			fileinfo->accessed[0] |= FILE_VISIT;  /* ½«±êÖ¾ÖÃÎ» */
			substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
		}
		sethomefile(annpath, currentuser.userid,fname);//pax@sjtubbs
		if((fn = fopen(annpath, "r")) == NULL )
		{
			presskeyfor("¶Ô²»Æð, ÄãÃ»ÓÐÉè¶¨Ë¿Â·. ÇëÏÈÓÃ f Éè¶¨Ë¿Â·.",t_lines-1);
			return 1;
		}
		fscanf(fn,"%s",annpath);
		fclose(fn);
		if (!dashd(annpath))
		{
			presskeyfor("ÄãÉè¶¨µÄË¿Â·ÒÑ¶ªÊ§, ÇëÖØÐÂÓÃ f Éè¶¨.",t_lines-1);
			return 1;
		}
	
		return (a_Import("0Announce", currboard, fileinfo, annpath, NA));
	}
}



int
check_notespasswd(void)
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
        getdata(2, 0, "ÇëÊäÈëÃØÃÜ±¸ÍüÂ¼ÃÜÂë: ", passbuf, 19, NOECHO, YEA);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return NA;
        if (!checkpasswd(prepass, passbuf))
        {
            move(3, 0);
            prints("´íÎóµÄÃØÃÜ±¸ÍüÂ¼ÃÜÂë...");
            pressanykey();
            return NA;
        }
    }
    return YEA;
}

int
show_b_secnote(void)
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
        prints("´ËÌÖÂÛÇøÉÐÎÞ¡¸ÃØÃÜ±¸ÍüÂ¼¡¹¡£");
    }
    pressanykey();
    return FULLUPDATE;
}

int
show_b_note(void)
{
    clear();
    if (show_board_notes(currboard, 2) == -1)
    {
        move(3, 30);
        prints("´ËÌÖÂÛÇøÉÐÎÞ¡¸±¸ÍüÂ¼¡¹¡£");
        pressanykey();
    }
    return FULLUPDATE;
}

int
into_announce(void)
{
    char	found[STRLEN];
    if(currboard[0]=='\0')
	return DONOTHING;
    if (a_menusearch("0Announce", currboard, found))
    {
	strcpy(jhpath,currboard);
	strcat(jhpath,"¾«»ªÇø");
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
        usercomplete( "ÄúÏë¿´Ë­µÄ¸öÈËÎÄ¼¯: " , lookid);
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
forward_post(int ent, struct fileheader *fileinfo, char *direct)
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_forward(ent, fileinfo, direct));
}

int
forward_u_post(int ent, struct fileheader *fileinfo, char *direct)
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_u_forward(ent, fileinfo, direct));
}
#endif

extern int
    mainreadhelp(void);
extern int b_vote(void);
extern int b_results(void);
extern int b_vote_maintain(void);
extern int b_notes_edit(void);
extern int Rangefunc(int ent, struct fileheader *fileinfo, char *direct);
extern int return_del(int ent, struct fileheader *fileinfo, char *direct);/* »ØÊÕÎÄÕÂ »ØÊÕÕ¾, add by mudboy*/
extern int return_range(int ent, struct fileheader *fileinfo, char *direct); /* »ØÊÕÕ¾Çø¶Î»Ö¸´ÎÄÕÂ, add by mudboy*/
extern int return_find(int ent, struct fileheader *fileinfo, char *direct);

struct one_key read_comms[] =
{
	{'_', underline_post},
	/*{'w', makeDELETEDflag,},*/ //canceled by cf
	{'Y', UndeleteArticle},
	{'r', read_post},
	{'K', skip_post},
	{'#', topfile_post},  //add by cf
	/*{'u', skip_post, },*//* Key 'u' use to query user's info */
	{'d', del_post},
	{'D', del_range},
	{'m', mark_post},
	{'+',recommend_post},
	{'E', edit_post},
	{Ctrl('G'), acction_mode,},
	{'`', acction_mode,},
	{'g', digest_post,},
	{'T', edit_title,},
	{'s', do_select,},
	{Ctrl('C'), do_cross,},
	{Ctrl('P'), do_post,},
	{Ctrl('Q'), do_module,},
	{'C', new_flag_clearto,	/* Çå³ýÎ´¶Á±ê¼Çµ½µ±Ç°Î»ÖÃ */},
	{'c', new_flag_clear,  /* ÇåÈ«²¿Î´¶Á±ê¼Ç */},
#ifdef INTERNET_EMAIL
	{'F', forward_post,},
	{'U', forward_u_post,},
	{Ctrl('R'), post_reply,},
#endif
	{'i', full_Save_post,},
	{'J', part_Save_post,},
	{'I', Import_post,},
	{'R', b_results,},
	{'v', b_vote,},
	{'V', b_vote_maintain,},
	{'W', b_notes_edit,},
	{Ctrl('W'), b_notes_passwd,},
	{'h', mainreadhelp,},
	{Ctrl('J'), mainreadhelp,},
	{KEY_TAB, show_b_note,},
	{'z', show_b_secnote,},
	{'x', into_announce,},
	//{	'X', into_myAnnounce,},
	//{	Ctrl('X'), into_PAnnounce,},
	{'a', auth_search_down,},
	{'A', auth_search_up,},
	{'/', t_search_down,},
	{'?', t_search_up,},
	{'\'', post_search_down,},
	{'\"', post_search_up,},
	{']', thread_down,},
	{'[', thread_up,},
	{Ctrl('D'), deny_user,},
#ifdef BOARD_CONTROL
	{Ctrl('K'), control_user,},
#endif
	{Ctrl('A'), show_author,},
	{Ctrl('N'), SR_first_new,},
	{'n', SR_first_new,},
	{'\\', SR_last,},
	{'=', SR_first,},
	{Ctrl('S'), SR_read,},
	{'p', SR_read,},
	//{'-', del_push,},
	{Ctrl('U'), SR_author,},
	{'b', SR_BMfunc,},
	{'B', Rangefunc,},
	{Ctrl('T'), acction_mode,},
	{'t', thesis_mode,	/* youzi 1997.7.8 */},
	{'!', Q_Goodbye,},
	{'S', s_msg,},
	{'f', t_friends,},
	{'o', return_del,   /* »ØÊÕÎÄÕÂ »ØÊÕÕ¾, add by mudboy*/},
	{'X', return_find,	},
	{'O', return_range,  /* ´Ó»ØÊÕÕ¾Çø¶Î»Ö¸´ÎÄÕÂ, add by rwzmm @ sjtubbs */},
	{'*', show_file_info, /* show file info, add by rwzmm @ sjtubbs */},
	{'\0', NULL},
};

int
Read(void)
{
    char    buf[STRLEN];
    char    notename[STRLEN];
    time_t  usetime;
    struct stat st;
    extern struct BCACHE *brdshm;
	/*extern char lastboard[80];*/
	struct shortfile *tmpbp;
    int iPos;
    if (!selboard)
    {
        move(2, 0);
        prints("ÇëÏÈÑ¡ÔñÌÖÂÛÇø\n");
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

    if (vote_flag(currboard, '\0', 1 /* ¼ì²é¶Á¹ýÐÂµÄ±¸ÍüÂ¼Ã» */ ) == 0)
    {
        if (dashf(notename))
        {
            ansimore3(notename, YEA);
            vote_flag(currboard, 'R', 1 /* Ð´Èë¶Á¹ýÐÂµÄ±¸ÍüÂ¼ */ );
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
            prints("ºÜ±§Ç¸, ±¾°åÃæÊôÓÚÍÅÌå°åÃæ£¬¶øÄú²»ÊÇ¸Ã°å³ÉÔ±");
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

    /* ½øÈë°åÃæ */
   
    i_read(READING, buf, readtitle, readdoent, &read_comms[0], sizeof(struct fileheader));
    /* ÍË³ö°åÃæ */
    brdshm->inboard[iPos]--;

    /* Í³¼ÆÊ±¼ä */
    board_usage(currboard, time(0) - usetime);
    brc_update();
    tmpbp=getbcache(currboard);
    if(tmpbp->flag&PUSH_FLAG)
    	brc_putpushrecord();
    return 0;
}
/*Add by SmallPig*/
void
notepad(void)
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
    prints("¿ªÊ¼ÄãµÄÁôÑÔ°É£¡´ó¼ÒÕýÊÃÄ¿ÒÔ´ý....\n");
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
            getdata(5, 0, "ÊÇ·ñ°ÑÄãµÄ´ó×÷·ÅÈëÁôÑÔ°å (Y)ÊÇµÄ (N)²»Òª (E)ÔÙ±à¼­ [Y]: ", note1, 3, DOECHO, YEA);
            if (note1[0] == 'e' || note1[0] == 'E')
                continue;
            else
                break;
        }
        if (note1[0] != 'N' && note1[0] != 'n')
        {
            sprintf(tmp, "\033[1;32m%s\033[37m£¨%.18s£©", currentuser.userid, currentuser.username);

            fprintf(in, "\033[1;34m¨Š\033[44m¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰\033[36mËá\033[32mÌð\033[33m¿à\033[31mÀ±\033[37m°å\033[34m¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰\033[44m¨Ž\033[m\n");
            getdatestring(thetime,NA);
            fprintf(in, "\033[1;34m¨Š\033[32;44m %-44s\033[32mÔÚ \033[36m%23.23s\033[32m Àë¿ªÊ±ÁôÏÂµÄ»°  \033[m\n", tmp, datestring+6);
            for (n = 0; n < i; n++)
            {
                if (note[n][0] == '\0')
                    break;
                fprintf(in, "\033[1;34m¨Š\033[33;44m %-75.75s\033[1;34m\033[m \n", note[n]);
            }
            fprintf(in,  "\033[1;34m¨Š\033[44m ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ \033[m \n");
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
Q_Goodbye(void)
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

    /* edwardc.990423 Ñ¶Ï¢ä¯ÀÀÆ÷ */
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
    			if( askyn("ÊÇ·ñ½«´Ë´ÎÊÕµ½µÄÑ¶Ï¢¼Ä»ØÐÅÏä", YEA, NA) == YEA )
    			{
    #ifdef TIMECOUNTER
    				getnowdatestring();
    #else
    				getdatestring(time(0), NA);
    #endif
    				sprintf( title, "[%14.14s]ËùÓÐÑ¶Ï¢±¸·Ý", datestring );
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
Goodbye(void)
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
                    strcpy(syswork[i],"[Ö°Îñ²»Ã÷]");
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
    prints("Äã¾ÍÒªÀë¿ª %s £¬¿ÉÓÐÊ²÷á½¨ÒéÂð£¿\n", BoardName);
    prints("[\033[1;33m1\033[m] ¼ÄÐÅ¸ø¹ÜÀíÈËÔ±\n");
    prints("[\033[1;33m2\033[m] °´´íÁËÀ²£¬ÎÒ»¹ÒªÍæ\n");
#ifdef USE_NOTEPAD

    if (strcmp(currentuser.userid, "guest") != 0)
    {
        prints("[\033[1;33m3\033[m] Ð´Ð´\033[1;32mÁô\033[33mÑÔ\033[35m°å\033[mÂÞ\n");
    }
#endif
    prints("[\033[1;33m4\033[m]\033[1;32m ¸øÅóÓÑ·¢¸öÏûÏ¢ ;)\033[m\n");
    prints("[\033[1;33m5\033[m] ²»¼ÄÂÞ£¬ÒªÀë¿ªÀ²\n");
    sprintf(buf, "ÄãµÄÑ¡ÔñÊÇ [\033[1;32m5\033[m]£º");
    getdata(8, 0, buf, genbuf, 4, DOECHO, YEA);
    clear();
    choose = genbuf[0] - '0';
    if (choose == 1)
    {
        prints("     Õ¾³¤µÄ ID    ¸º Ôð µÄ Ö° Îñ\n");
        prints("     ============ =====================\n");
        for (i = 1; i <= num_sysop; i++)
        {
            prints("[\033[1;33m%2d\033[m] %-12s %s\n", i, sysoplist[i - 1]
                   ,syswork[i - 1]);
        }
        prints("[\033[1;33m%2d\033[m] »¹ÊÇ×ßÁËÂÞ£¡\n",num_sysop + 1);
        sprintf(buf, "ÄãµÄÑ¡ÔñÊÇ [\033[1;32m%d\033[m]£º", num_sysop + 1);
        getdata(num_sysop + 5, 0, buf, genbuf, 4, DOECHO, YEA);
        choose = atoi(genbuf);
        if (choose >= 1 && choose <= num_sysop)
            do_send(sysoplist[choose - 1], "Ê¹ÓÃÕß¼ÄÀ´µÄ½¨ÒéÐÅ");
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
do_report(char *filename, char *s)
{
    char    buf[1024];
    time_t  dtime;
    time(&dtime);
    getdatestring(dtime,NA);
    sprintf(buf, "%s %16.16s %16.16s %s\n", currentuser.userid,currentuser.lasthost,datestring+6, s);
    file_append(filename, buf);
}
void
report(char *s)
{
    do_report("trace",s);
}

void
gamelog(char *s)
{
    do_report("game/trace",s);
}

void
board_usage(char *mode, time_t usetime)
{
    time_t  now;
    char    buf[256];
    now = time(0);
    getdatestring(now,NA);
    sprintf(buf, "%.22s USE %-20.20s Stay: %5ld (%s)\n", datestring, mode, usetime, currentuser.userid);
    file_append("use_board", buf);
}


int
Info(void)
{
    modify_user_mode(XMENU);
    ansimore("Version.Info", YEA);
    clear();
    return 0;
}

int
Conditions(void)
{
    modify_user_mode(XMENU);
    ansimore("COPYING", YEA);
    clear();
    return 0;
}

int
Welcome(void)
{
    char    ans[3];
    modify_user_mode(XMENU);
    if (!dashf("Welcome2"))
        ansimore("Welcome", YEA);
    else
    {
        clear();
        stand_title("¹Û¿´½øÕ¾»­Ãæ");
        for (;;)
        {
            getdata(1, 0, "(1)ÌØÊâ½øÕ¾¹«²¼À¸  (2)±¾Õ¾½øÕ¾»­Ãæ ? : ",
                    ans, 2, DOECHO, YEA);

            /* skyo.990427 modify  °´ Enter Ìø³ö  */
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
cmpbnames(char *bname, void *vbrec)
{
	struct fileheader *brec = (struct fileheader *)vbrec;
    if (!strncasecmp(bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

// »ØÊÕÕ¾, next func changed by mudboy
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
        sprintf(fname, "M.%ld.A", now);
        setbfile(genbuf, brd, fname);
        count = 0;
        while (dashf(genbuf))
        {
            now++;
            sprintf(fname, "M.%ld.A", now);
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
            else if (strncmp(genbuf, "·¢ÐÅÈË: ", 8) == 0)
            {
                if ((ptr = strrchr(genbuf, ')')) != NULL)
                {
                    *ptr = '\0';
                    if ((ptr = strrchr(genbuf, '(')) != NULL)
                        strcpy(from, ptr + 1);
                }
            }
            else if (strncmp(genbuf, "×ªÐÅÕ¾: ", 8) == 0)
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
thesis_mode(void)
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
/* Add by everlove 07/08/2001 ÖÆ×÷ºÏ¼¯ */
void Add_Combine(char *board, struct fileheader *fileinfo, int has_yinyan)
{
    FILE *fp;
    char buf[STRLEN];
    char temp2[1024];

    sprintf(buf,"tmp/%s.combine",currentuser.userid);
    fp=fopen(buf,"at");
    fprintf(fp,"\033[1;32m¡î©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¡î\033[m\n");
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
            fprintf(fp, "    \033[0;1;32m%s \033[mÓÚ",s_ptr);
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
            fprintf(fp, " \033[1;36m%s\033[m Ìáµ½£º\n",s_ptr);
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
            if (!has_yinyan && ((strstr(temp2, "¡¾")) || (*temp2==':')))
                continue;
            if ((*temp2==':') && ((strstr(temp2, "¡¾"))||(*(temp2+2)==':')))
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

//{ »ØÊÕÕ¾, add by mudboy
int
return_del(int ent, struct fileheader *fileinfo, char *direct)
{
    int i,fail,ed=0;
    char buf[512], buffer[128]; //modified by proto@sjtubbs 2006.11.30
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
    if ((p = strrchr(UTitle, '-')))
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
		//added by proto@sjtubbs 2006.11.30
		sprintf(buffer, "returned the deleted file '%s' on '%s'", fileinfo->title, currboard);
    	report(buffer);
		//added end
    }
    return DIRCHANGED;
}
//}



//add by rwzmm @ sjtubbs, range move files from return box to the board
int
return_range(int ent, struct fileheader *fileinfo, char *direct)
{
	char    num[9];
    int     inum1, inum2;
	
	if (digestmode!=9)
        return DONOTHING;
	if((digestmode == 9) && (returnmode != 0))
		return DONOTHING;

	getdata(t_lines - 1, 0, "Ê×ÆªÎÄÕÂ±àºÅ: ", num, 7, DOECHO, YEA);
    inum1 = atoi(num);
    if (inum1 <= 0)
    {
        move(t_lines - 1, 50);
        prints("´íÎó±àºÅ...");
        egetch();
        return PARTUPDATE;
    }
    getdata(t_lines - 1, 25, "Ä©ÆªÎÄÕÂ±àºÅ: ", num, 7, DOECHO, YEA);
    inum2 = atoi(num);
    if (inum2 < inum1 + 1)
    {
        move(t_lines - 1, 50);
        prints("´íÎóÇø¼ä...");
        egetch();
        return PARTUPDATE;
    }

	move(t_lines - 1, 50);
    if (askyn("È·¶¨»Ö¸´£¿", NA, NA) == YEA)
    {
		char buffer[128]; //added by proto@sjtubbs 2006.11.30
        return_file_range(direct, inum1, inum2);
        fixkeep(direct, inum1, inum2);
		//added by proto@sjtubbs 2006.11.30
		sprintf(buffer, "returned the deleted files %d-%d on '%s'", inum1, inum2, currboard);
    	report(buffer);
		//added end
        return DIRCHANGED;
    }
    move(t_lines - 1, 50);
    clrtoeol();
    prints("·ÅÆú...");
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
			sprintf(fullpath, "%s/%s/%s", buf, getfolderstr(fhdr.filename), fhdr.filename);

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
			if ((p = strrchr(UTitle, '-')))
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
return_find(int ent, struct fileheader *fileinfo, char *direct)
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


    getdata (t_lines - 1, 0, "²éÑ¯Ä£Ê½: 1)Í¬×÷Õß 2)±êÌâ¹Ø¼ü×Ö [1]: ", 
		ch, 3, DOECHO, YEA);
	if (ch[0] == '\0')
		ch[0] = '1';
	type = atoi (ch);
	if (type < 1 || type > 2 )
	{
		saveline (t_lines - 1, 1);
		return PARTUPDATE;
	}
	else if(type == 2)	//±êÌâ¹Ø¼ü×Ö
	{
		getdata (t_lines - 1, 0, "ÄúÏë²éÕÒµÄÎÄÕÂ±êÌâ¹Ø¼ü×Ö: ",
			someoneID, 30, DOECHO, YEA);
		if (someoneID[0] == '\0')
		{
			saveline (t_lines - 1, 1);
			return PARTUPDATE;
		}
		type = 3;	//warning
	}

	else	//×÷Õß¹Ø¼ü×Ö
	{
		getdata (t_lines - 1, 0, "ÄúÏë²éÕÒÄÄÎ»ÍøÓÑµÄÎÄÕÂ? ", someoneID,
			13, DOECHO, YEA);
		if (someoneID[0] == '\0')
		{
			saveline (t_lines - 1, 1);
			return PARTUPDATE;
		}
		getdata (t_lines - 1, 37,
			"¾«È·²éÕÒ°´ Y£¬ Ä£ºý²éÕÒÇë»Ø³µ[Enter]", ch, 2, DOECHO,
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
void rengedel_autoreport (char *title, char *str, char *userid, char *sysboard)
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

//add by clist at sjtubbs
char* getfolderstr(char* filename)
{
	static char str[4] = {'\0','\0','\0','\0'};
	int len = strlen(filename);
	str[0] = filename[len-5]>'4'?filename[len-5]-5:filename[len-5];
	str[1] = filename[len-4];
	str[2] = filename[len-3];
	return str;
}
