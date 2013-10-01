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

#include "bbs.h"
#include <stdarg.h>
#include <sys/uio.h>
#include <time.h>
#if defined(BSD44)
#include <stdlib.h>

#elif defined(LINUX)

#else

#include <rpcsvc/rstat.h>
#endif

FILE *ftemp;
char filename[64], buffer[64], firstchar;

int mot;
struct postheader header;
extern int numofsig;
extern char lastboard[80];

int continue_flag;
int readpost;
int digestmode;
int returnmode;
int local_article;
struct userec currentuser;
char blog_user_id[32];
char blog_id[32];
char blog_catId[32];

char module_name[NAMELEN];
int writeinto_modulefile;
int write_module = 0;
extern char jhpath[1024];

int usernum = 0;
char currboard[STRLEN - BM_LEN];
char currBM[BM_LEN - 4];
int selboard = 0;
char someoneID[31];
char ReadPost[STRLEN] = "";
char ReplyPost[STRLEN] = "";

int current_article_reid = 0;
unsigned int reply_id = 0;

char anony_id[IDLEN + 2];

extern struct BCACHE *brdshm;

int FFLL = 0;

#ifdef BOARD_CONTROL
int control_user(void);
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

char genbuf[1024];
char quote_title[120], quote_board[120];
char quote_file[120], quote_user[120];

char module_file[NAMELEN];

#ifndef NOREPLY
char replytitle[STRLEN];
#endif

struct shortfile *getbcache(char *bname);
int totalusers, usercounter;

void rengedel_autoreport(char *title, char *str, char *userid,
                         char *sysboard);

int check_stuffmode(void)
{

    if (uinfo.mode == RMAIL || (uinfo.mode == READING
                                && seek_in_file("etc/macroboards",
                                                currboard)))

        return YEA;
    else
        return NA;
}

int topfile_post(int ent, struct fileheader *fhdr, char *direct)
{
    if (!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if (fhdr->accessed[0] & FILE_TOP1)
    {
        char buffer[128];

        fhdr->accessed[0] &= ~FILE_TOP1;
        dele_digest_top(fhdr->filename, direct, TOPFILE_DIR);

        sprintf(buffer, "untopped the file '%s' on '%s'", fhdr->title,
                currboard);
        report(buffer);

        log_bm_ops(currboard, "°åÃæ", "!#", fhdr->title);
    }
    else
    {
        struct fileheader topfile;
        char *ptr, buf[64], buffer[128];

        memcpy(&topfile, fhdr, sizeof(topfile));
        topfile.filename[0] = 'T';
        topfile.accessed[1] |= FILE_NOREPLY2;
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s%s/%s", buf, getfolderstr(topfile.filename),
                topfile.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] |= FILE_TOP1;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        sprintf(&genbuf[512], "%s%s/%s", buf, getfolderstr(fhdr->filename),
                fhdr->filename);

        strcpy(ptr, TOPFILE_DIR);
        if (get_topfile_num(buf, sizeof(topfile)) >= MAX_TOP)
        {

            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("±§Ç¸£¬ÄãµÄÖÃµ×ÎÄÕÂÒÑ¾­³¬¹ý %d Æª£¬ÎÞ·¨ÔÙ¼ÓÈë...\n",
                   MAX_TOP);
            pressanykey();
            return PARTUPDATE;
        }
        link(&genbuf[512], genbuf);
        append_record(buf, &topfile, sizeof(topfile));
        fhdr->accessed[0] |= FILE_TOP1;
        fhdr->accessed[0] &= ~FILE_DELETED;

        sprintf(buffer, "topped the file '%s' on '%s'", topfile.title,
                currboard);
        report(buffer);

        log_bm_ops(currboard, "°åÃæ", "#", topfile.title);
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);
    return PARTUPDATE;
}

#ifndef BBSD
void get_load(load)
double load[];
{
#if defined(LINUX)
    FILE *fp;

    fp = fopen("/proc/loadavg", "r");
    if (!fp)
        load[0] = load[1] = load[2] = 0;
    else
    {
        float av[3];

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

int set_safe_record(void)
{
    struct userec tmp;
    extern int ERROR_READ_SYSTEM_FILE;

    if (get_record(PASSFILE, &tmp, sizeof(currentuser), usernum) == -1)
    {
        char buf[STRLEN];

        sprintf(buf, "Error:Read Passfile %4d %12.12s", usernum,
                currentuser.userid);
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

char *sethomepath(char *buf, char *userid)
{
    sprintf(buf, "home/%c/%c/%s", toupper(userid[0]), toupper(userid[1]),
            userid);
    return buf;
}

void setqtitle(char *stitle)
{
    FFLL = 1;
    if (strncmp(stitle, "Re: ", 4) != 0)
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

void setqreid(unsigned int reid)
{
    FFLL = 1;
    reply_id = reid;
}

int WHICHDIST(char *userid)
{
    char datafile[80] = BBSHOME "/etc/DISTmaster.ini";
    FILE *fp;
    int counts = 1;
    char t[256], *t1, *t2, *t3;

    fp = fopen(datafile, "r");
    if (fp == NULL)
        return SectNum;
    while (fgets(t, 255, fp) && counts <= 11)
    {
        t1 = strtok(t, " \t\n");

        if (t1 == NULL || t1[0] == '#')
            continue;
        if (!isalpha(t1[0]))
        {
            counts++;
            continue;
        }
        t2 = strtok(NULL, " \t\n");
        if (t2 != NULL)
            t3 = strtok(NULL, " \t\n");
        else
            t3 = NULL;

        if ((t1 != NULL && !strcasecmp(t1, userid))
            || (t2 != NULL && !strcasecmp(t2, userid)) || (t3 != NULL
                                                           &&
                                                           !strcasecmp(t3,
                                                                       userid)))
        {

            fclose(fp);
            return (counts);
        }
        counts++;
    }
    fclose(fp);
    return (counts);
}

int chk_currDM(void)
{
    char BoardsFile[256], category;
    int fd, total, index, Dindex, DMLevel;
    struct boardheader *buffer;
    struct stat st;

    char CateInSect[SectNum][5] =
        { "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw",
"x" };

    sprintf(BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open(BoardsFile, O_RDONLY);
    if (fd == -1)
        return NA;
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer =
        (struct boardheader *) calloc((size_t) total,
                                      sizeof(struct boardheader));
    if (buffer == NULL)
    {
        close(fd);
        return NA;
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close(fd);
        return NA;
    }
    close(fd);

    for (index = 0; index < total; index++)
    {
        if (!strcmp(buffer[index].filename, currboard))
            break;
    }
    if (index == total)
        return NA;

    category = buffer[index].title[0];
    free(buffer);

    for (Dindex = 0; Dindex < SectNum; Dindex++)
    {
        if (strchr(CateInSect[Dindex], category) != NULL)
            break;
    }

    if (Dindex == SectNum)
        return NA;

    DMLevel = WHICHDIST(currentuser.userid);
    if (Dindex == DMLevel)
        return YEA;
    else
        return NA;
}

int chk_parentBM(void)
{
    int mypos = 0;
    int parent_pos = 0;
    char *ptr;
    char BMstrbuf[STRLEN - 1];

    for (mypos = 0; mypos < MAXBOARD; mypos++)
    {
        if (!strcmp(brdshm->bcache[mypos].filename, currboard))
            break;
    }
    if (mypos == MAXBOARD)
        return NA;
    parent_pos = brdshm->bcache[mypos].group - 1;
    if (parent_pos < 0)
        return NA;
    strncpy(BMstrbuf, brdshm->bcache[parent_pos].BM, STRLEN - 4);
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

int chk_currBM(char *BMstr)
{
    char *ptr;
    char BMstrbuf[STRLEN - 1];

//  if (digestmode == 9)
//      return NA;

    if (HAS_PERM(PERM_BLEVELS))
        return YEA;
    if (HAS_PERM(PERM_SPECIAL7) && chk_currDM() == YEA)
        return YEA;
    if (!HAS_PERM(PERM_BOARDS))
        return NA;

    if (chk_parentBM() == YEA)
        return YEA;

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

int chk_ap(int i)
{
    FILE* fn;
    struct shortfile *bp;
    char fname[STRLEN];
    char ap_fname[STRLEN];  
    char ap_BM[BM_LEN-4];
    int j = 0;
    if ( i < 0 || i > 3 )
        return NA;
   
    if ( 0 == i ){
        sprintf(fname, ".fpp");
        sethomefile(ap_fname, currentuser.userid, fname);
    }
    else{
        sprintf(fname, ".fpp%d", i);
        sethomefile(ap_fname, currentuser.userid, fname);       
    }
    fn = fopen(ap_fname, "r");
    
    if (fn){
        char tmpath[1000];

       if ( fgets(tmpath, 1000, fn) == NULL )
            return YEA;
        else{
            while( tmpath[j] != '\0' ){
                unsigned int ch = tmpath[j];
                if ( (ch >= '0' && ch <= '9')
                     || (ch >= 'a' && ch <= 'z')
                     || (ch >= 'A' && ch <= 'Z')
                     || (ch == '_'))
                    j++ ;
                else{
                    tmpath[j] = '\0';
                    break;
                }

            }
	    bp = getbcache(tmpath);
	    memcpy(ap_BM, bp->BM, BM_LEN - 4);
            return chk_currBM(ap_BM);
        }
    }
    else
        return YEA;
}

void setquotefile(char *filepath)
{
    strcpy(quote_file, filepath);
}

char *sethomefile(char *buf, char *userid, char *filename)
{
    sprintf(buf, "home/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, filename);
    return buf;
}

char *setuserfile(char *buf, char *filename)
{
    sprintf(buf, "home/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, filename);
    return buf;
}

char *setbpath(char *buf, char *boardname)
{
    strcpy(buf, "boards/");
    strcat(buf, boardname);
    return buf;
}

char *setbdir(char *buf, char *boardname)
{
    char dir[STRLEN];

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
    case 5:
    case 6:
    case 7:
        break;
    }
    dir[STRLEN - 1] = '\0';
    if (digestmode == 5 || digestmode == 6)
        sprintf(buf, "boards/%s/SOMEONE.%s.DIR.%d", boardname, someoneID,
                digestmode - 5);
    else if (digestmode == 7)
        sprintf(buf, "boards/%s/KEY.%s.DIR", boardname,
                currentuser.userid);
    else if (digestmode == 9)
    {
        if (returnmode == 1 || returnmode == 2)
            sprintf(buf, "boards/%s/SOMEONE.%s.RETURN_DIR.%d", boardname,
                    someoneID, returnmode);
        else if (returnmode == 3)
            sprintf(buf, "boards/%s/KEY.%s.RETURN_DIR", boardname,
                    currentuser.userid);
        else
            sprintf(buf, "boards/%s/%s", boardname, RETURN_DIR);
    }
    else
        sprintf(buf, "boards/%s/%s", boardname, dir);
    return buf;
}

char *setbfile(char *buf, char *boardname, char *filename)
{
    int len = strlen(filename);

    if (len > 10 && filename[1] == '.' && filename[len - 2] == '.'
        && isdigit(filename[len - 5]) && isdigit(filename[len - 4])
        && isdigit(filename[len - 3]))
        sprintf(buf, "boards/%s/%s/%s", boardname, getfolderstr(filename),
                filename);
    else
        sprintf(buf, "boards/%s/%s", boardname, filename);
    return buf;
}

char *setmodulefile(char *buf, char *boardname, char *filename)
{
    sprintf(buf, "vote/%s/%s", boardname, filename);
    return buf;
}

void setmfile(char *buf, char *userid, char *filename)
{
    sprintf(buf, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, filename);
}

char *setmdir(char *buf, char *userid)
{
    sprintf(buf, "mail/%c/%c/%s/.DIR", toupper(userid[0]),
            toupper(userid[1]), userid);
    return buf;
}

int 
is_denied_anony(char* userid)
{
    struct shortfile *bp = getbcache (currboard);
    char buf[STRLEN];
    char anonyid[IDLEN + 2];

    if (!((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG)))
        return 0;

    strcpy(anonyid, get_anonyid(userid));

    setbfile(buf, currboard, "deny_users");
    if (seek_in_denyfile(buf, anonyid))
        return 1;

    return 0;
}

int
deny_me( char * bname)
{
    char buf[STRLEN];
    setbfile(buf, bname, "deny_users");
    return seek_in_denyfile(buf, currentuser.userid) || is_denied_anony(currentuser.userid);
}

//add by alt@02.03.09
int
seek_in_denyfile(char *filename, char *seekstr)
{
    FILE* fp;
    char buf[STRLEN];
    char *namep, *date, *userid;
    char flag;
    time_t nowtime;
    struct anony_item item;

    if ((fp = fopen(filename, "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, ": \n\r\t");
        if (namep != NULL && strcasecmp(namep, seekstr) == 0)
        {
            fclose(fp);
            nowtime = time(0);
            getdatestring(nowtime, NA);
            flag = buf[55];
            date = (char *) (buf + 58);
            if (flag == 'a' && strncmp(datestring, date, 16) >= 0)
            {
                if (del_from_file(filename, seekstr))
                {
                    char msgbuf[STRLEN];
                    char repbuf[STRLEN];

                    sprintf(repbuf, "»Ö¸´%sÔÚ%s°åµÄ·¢ÎÄÈ¨ÏÞ",
                            seekstr, currboard);
                    sprintf(msgbuf, "\n  %s ÍøÓÑ£º\n\n"
                            "\tÒò·â½ûÊ±¼äÒÑ¹ý£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º·¢±íÎÄÕÂ¡»È¨Á¦¡£\n\n"
                            "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                            seekstr, currboard, BBSID);

                    if (strchr(seekstr, '.'))
                    {
                        if (search_in_anonyindex(NULL, seekstr+6, &item) == -1)
                            return 0;
                        userid = item.userid;
                    }
                    else
                        userid = seekstr;

#ifdef DENY_POST_BOARDNAME
					if (strncmp(currboard, "BMTraining", 10) == 0)
					{
						autoreport(repbuf,msgbuf,userid,TRAIN_ANNOUNCE_BOARDNAME);
					}
					else
					{
						autoreport(repbuf,msgbuf,userid,DENY_POST_BOARDNAME);
					}

#else

                    autoreport(repbuf, msgbuf, seekstr, NULL);
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

void shownotepad(void)
{
    modify_user_mode(NOTEPAD);
    ansimore("etc/notepad", YEA);
    return;
}

int uleveltochar(char *buf, unsigned int lvl)
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
    buf[10] = '\0';
    buf[9] = (lvl & (PERM_BOARDS)) ? 'B' : ' ';
    buf[8] = (lvl & (PERM_CLOAK)) ? 'C' : ' ';
    buf[7] = (lvl & (PERM_SEECLOAK)) ? '#' : ' ';
    buf[6] = (lvl & (PERM_XEMPT)) ? 'X' : ' ';
    buf[5] = (lvl & (PERM_CHATCLOAK)) ? 'M' : ' ';
    buf[4] = (lvl & (PERM_ACCOUNTS)) ? 'A' : ' ';
    buf[3] = (lvl & (PERM_HONOR)) ? 'N' : ' ';
    buf[2] = (lvl & (PERM_OBOARDS)) ? 'O' : ' ';
    buf[1] = (lvl & (PERM_666LIFE)) ? 'D' : ' ';
    buf[0] = (lvl & (PERM_SYSOP)) ? 'S' : ' ';
    return 1;
}

void printutitle(void)
{
    move(2, 0);
    prints
        ("\033[1;44m ±à ºÅ  Ê¹ÓÃÕß´úºÅ   %-19s #%-4s #%-4s %8s    %-12s  \033[m\n",
#if defined(ACTS_REALNAMES)
         HAS_PERM(PERM_SYSOP) ? "ÕæÊµÐÕÃû" : "Ê¹ÓÃÕßêÇ³Æ",
#else
         "Ê¹ÓÃÕßêÇ³Æ",
#endif
         (toggle2 == 0) ? "ÉÏÕ¾" : "ÎÄÕÂ",
         (toggle2 == 0) ? "Ê±Êý" : "ÐÅ¼þ",
         HAS_PERM(PERM_SEEULEVELS) ? " µÈ  ¼¶ " : "",
         (toggle1 == 0) ? "×î½ü¹âÁÙÈÕÆÚ" :
         (toggle1 == 1) ? "×î½ü¹âÁÙµØµã" : "ÕÊºÅ½¨Á¢ÈÕÆÚ");
}

int g_board_names(struct shortfile *fhdrp)
{
    if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(fhdrp->level)
        || (fhdrp->level & PERM_NOZAP))
    {
        AddToNameList(fhdrp->filename);
    }
    return 0;
}

void make_blist(void)
{
    CreateNameList();
    apply_boards(g_board_names);
}

int Select(void)
{
    modify_user_mode(SELECT);
    do_select(0, NULL, genbuf);
    return 0;
}

int junkboard(void)
{
    struct shortfile *bp;

    bp = getbcache(currboard);
    if (bp->flag & NOPOST_FLAG)
        return 1;
    if (strstr(JUNKBOARDS, currboard))
        return 1;
    else
        return seek_in_file("etc/junkboards", currboard);
}

int Post(void)
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

int Postfile(char *filename, char *nboard, char *posttitle, int mode)
{
    char bname[STRLEN];
    char dbname[STRLEN];
    struct boardheader fh;

    if (nboard != NULL)
        if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, nboard) <= 0)
        {
            sprintf(bname, "%s ÌÖÂÛÇøÕÒ²»µ½", nboard);
            report(bname);
            return -1;
        }
    in_mail = NA;
    if(!mode)
    	strcpy(quote_board, currboard);
    else
    	strcpy(quote_board, nboard);
    strcpy(dbname, currboard);
    strcpy(currboard, nboard);
    strcpy(quote_file, filename);
    strcpy(quote_title, posttitle);
    post_cross('l', mode);
    strcpy(currboard, dbname);
    return 0;
}

int get_a_boardname(char *bname, char *prompt)
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

int do_cross(int ent, struct fileheader *fileinfo, char *direct)
{
    char bname[STRLEN];
    char dbname[STRLEN];
    char ispost[10];
    time_t now = time(0);
    int digbackup;
    struct tm *tm;

    if (digestmode && digestmode != 9)
        return DONOTHING;
    digbackup = digestmode;
    set_safe_record();
    if (!HAS_PERM(PERM_POST))
        return DONOTHING;
    if (uinfo.mode != RMAIL)
        sprintf(genbuf, "boards/%s/%s/%s", currboard,
                getfolderstr(fileinfo->filename), fileinfo->filename);
    else
        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid,
                fileinfo->filename);
    strncpy(quote_file, genbuf, sizeof(quote_file));
    quote_file[sizeof(quote_file) - 1] = '\0';
    strcpy(quote_title, fileinfo->title);

    clear();

    tm = localtime(&now);
    if (brdshm->all_no_post_end >= brdshm->all_no_post_begin)
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {

            if ((unsigned int) tm->tm_hour >= brdshm->all_no_post_begin
                && (unsigned int) tm->tm_hour < brdshm->all_no_post_end)
            {
                move(5, 10);
                clrtobot();
                prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±È«Õ¾Ö»¶Á¡£",
                       brdshm->all_no_post_begin, brdshm->all_no_post_end);
                pressreturn();
                clear();
                return FULLUPDATE;
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {
            if ((unsigned int) tm->tm_hour >= brdshm->no_post_begin
                && (unsigned int) tm->tm_hour < brdshm->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {
                    move(5, 10);
                    clrtobot();
                    prints
                        ("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
                         brdshm->no_post_begin, brdshm->no_post_end);
                    pressreturn();
                    clear();
                    return FULLUPDATE;
                }
            }
        }
    }
    else
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {

            if ((unsigned int) tm->tm_hour >= brdshm->all_no_post_begin
                || (unsigned int) tm->tm_hour < brdshm->all_no_post_end)
            {
                move(5, 10);
                clrtobot();
                prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±È«Õ¾Ö»¶Á¡£",
                       brdshm->all_no_post_begin, brdshm->all_no_post_end);
                pressreturn();
                clear();
                return FULLUPDATE;
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {
            if ((unsigned int) tm->tm_hour >= brdshm->no_post_begin
                || (unsigned int) tm->tm_hour < brdshm->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {
                    move(5, 10);
                    clrtobot();
                    prints
                        ("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
                         brdshm->no_post_begin, brdshm->no_post_end);
                    pressreturn();
                    clear();
                    return FULLUPDATE;
                }
            }
        }

    }
    prints
        ("\033[1;33mÇëÕäÏ§±¾Õ¾×ÊÔ´£¬ÇëÔÚ×ªÔØºóÉ¾³ý²»±ØÒªµÄÎÄÕÂ£¬Ð»Ð»£¡ \n\033[1;32m²¢ÇÒ£¬Äã²»ÄÜ½«±¾ÎÄ×ªÔØµ½±¾°å£¬Èç¹ûÄã¾õµÃ²»·½±ãµÄ»°£¬ÇëÓëÕ¾³¤ÁªÏµ¡£\033[m\n\n");
    prints("ÄúÑ¡Ôñ×ªÔØµÄÎÄÕÂÊÇ: [\033[1;33m%s\033[m]\n", quote_title);
    if (!get_a_boardname
        (bname,
         "ÄúÈç¹ûÈ·¶¨Òª×ªÔØµÄ»°£¬ÇëÊäÈëÒª×ªÌùµÄÌÖÂÛÇøÃû³Æ(È¡Ïû×ªÔØÇë°´»Ø³µ): "))
    {
        return FULLUPDATE;
    }
    if (!strcmp(bname, currboard) && uinfo.mode != RMAIL)
    {
        move(8, 8);
        prints("¶Ô²»Æð£¬±¾ÎÄ¾ÍÔÚÄúÒª×ªÔØµÄ°åÃæÉÏ£¬ËùÒÔÎÞÐè×ªÔØ¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    move(3, 0);
    clrtoeol();
    prints("×ªÔØ ' %s ' µ½ %s °å ", quote_title, bname);
    move(4, 0);
    getdata(4, 0, "(S)×ªÐÅ (L)±¾Õ¾ (A)È¡Ïû? [L]: ", ispost, 9, DOECHO,
            YEA);
    if (ispost[0] == 'a' || ispost[0] == 'A')
        prints("È¡Ïû");
    else
    {
        strcpy(quote_board, currboard);
        strcpy(dbname, currboard);
        strcpy(currboard, bname);
        digbackup = digestmode;
        digestmode = 0;
        if (ispost[0] != 's' && ispost[0] != 'S')
            ispost[0] = 'L';
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

void readtitle(void)
{
    struct shortfile *bp;
    int i, j, bnum, tuid;
    struct user_info uin;
    char tmp[40], bmlists[3][16];
    extern int t_cmpuids(int uid, struct user_info *up);
    FILE *fp;
    char buf[256], buf2[256] = "";
    int has_note;
    char *t = NULL;

    char header[STRLEN], title[STRLEN];
    char readmode[11];

    bp = getbcache(currboard);
    memcpy(currBM, bp->BM, BM_LEN - 4);
    bnum = 0;
    for (i = 0, j = 0; currBM[i] != '\0'; i++)
    {
        if (currBM[i] == ' ')
        {
            bmlists[bnum][j] = '\0';
            bnum++;
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
        for (i = 0; i < bnum; i++)
        {
            tuid = getuser(bmlists[i]);
            search_ulist(&uin, t_cmpuids, tuid);
            if (uin.active && uin.pid && !uin.invisible)
            {
                sprintf(tmp, "\033[32m%s\033[33m ", bmlists[i]);
            }
            else if (uin.active && uin.pid && uin.invisible
                     && (HAS_PERM(PERM_SEECLOAK) || usernum == uin.uid))
            {
                sprintf(tmp, "\033[36m%s\033[33m ", bmlists[i]);
            }
            else
                sprintf(tmp, "%s ", bmlists[i]);
            strcat(header, tmp);
        }
    }
    if (chkmail())

        strcpy(title, "[ÄúÓÐÐÅ¼þ£¬°´ L ¿´ÐÂÐÅ]");
    else if ((bp->flag & VOTE_FLAG))
        sprintf(title, "¡ùÍ¶Æ±ÖÐ,°´ v ½øÈëÍ¶Æ±¡ù");
    else
        strcpy(title, bp->title + 8);

    showtitle(header, title);

    if (!DEFINE(DEF_BOARDNOTE))
        has_note = 0;
    else
    {
        sprintf(buf, "vote/%s/note2", currboard);
        fp = fopen(buf, "r");
        if (fp)
        {
            fgets(buf2, 254, fp);
            buf2[254] = 0;
            fclose(fp);
            t = strchr(buf2, '\n');
            if (t)
                *t = '\0';
            if (strlen(buf2) != 0)
            {
                prints("%s\n", buf2);
                has_note = 1;
            }
            else
                has_note = 0;
        }
        else
            has_note = 0;
    }
    if (!has_note)
        prints
            ("Àë¿ª[\033[1;32m¡û\033[m,\033[1;32me\033[m] Ñ¡Ôñ[\033[1;32m¡ü\033[m,\033[1;32m¡ý\033[m] ÔÄ¶Á[\033[1;32m¡ú\033[m,\033[1;32mRtn\033[m] ·¢±íÎÄÕÂ[\033[1;32mCtrl-P\033[m] ¿³ÐÅ[\033[1;32md\033[m] ±¸ÍüÂ¼[\033[1;32mTAB\033[m] ÇóÖú[\033[1;32mh\033[m]\n");

    if (digestmode == 0)
    {
        if (DEFINE(DEF_THESIS))
            strcpy(readmode, "Ö÷Ìâ");
        else
            strcpy(readmode, "Ò»°ã");
    }
    else if (digestmode == 1)
        strcpy(readmode, "ÎÄÕª");
    else if (digestmode == 2)
        strcpy(readmode, "Ö÷Ìâ");
    else if (digestmode == 3)
        strcpy(readmode, "MARK");
    else if (digestmode == 4)
        strcpy(readmode, "Ô­×÷");
    else if (digestmode == 7)
        strcpy(readmode, "±êÌâ¹Ø¼ü×Ö");

    else if (digestmode == 9)
        strcpy(readmode, "»ØÊÕ");

    if (DEFINE(DEF_THESIS) && digestmode == 0)
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-38s[%4sÊ½¿´°å] \033[m\n",
               "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    else if (digestmode == 5 || digestmode == 6)
        prints
            ("\033[1;37;44m ±àºÅ    %-12s %6s %-10s (¹Ø¼ü×Ö: \033[32m%-12s\033[37m) [\033[33m%s\033[37mÍ¬×÷ÕßÔÄ¶Á] \033[m\n",
             "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", someoneID,
             (digestmode == 5) ? "Ä£ºý" : "¾«È·");
    else if (digestmode == 7)
        prints
            ("\033[1;37;44m ±àºÅ    %-12s %6s %-32s[%10sÊ½¿´°å] \033[m\n",
             "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    else
        prints("\033[1;37;44m ±àºÅ    %-12s %6s %-40s[%4sÄ£Ê½] \033[m\n",
               "¿¯ µÇ Õß", "ÈÕ  ÆÚ", " ±ê  Ìâ", readmode);
    clrtobot();
}

char *getstring_byaccess(char *result, int num, int a, int b, int c)
{
    int i;

    i = num;
    if (i <= 0)
        i = -i;
    if (i <= a)
        snprintf(result, 80, "[1;32m%3d[m", i);
    else if (i <= b)
        snprintf(result, 80, "[1;33m%3d[m", i);
    else if (i <= c)
        snprintf(result, 80, "[1;35m%3d[m", i);
    else
        snprintf(result, 80, "[1;31m%3d[m", i);
    return result;
}

char *readdoent(int num, void *vent)
{
    struct fileheader *ent = (struct fileheader *) vent;
    char path[64];
    struct stat st;
    char msg[17];
    static char buf[128];
    struct shortfile *bp;
    time_t filetime;
    char *date;
    char TITLE[STRLEN], color[10];
    int type;
    int inAnnounce = 0;
    unsigned int maxlength;
    int pos;
    char owner_display[20];

#ifdef COLOR_POST_DATE
    struct tm *mytm;
#endif
    bp = getbcache(currboard);
    type = brc_unread(ent->filename) ?
        (!DEFINE(DEF_NOT_N_MASK) ? 'N' : '+') : ' ';
    if ((bp->flag & PUSH_FLAG) && ent->pnum > 0 && ent->pnum < 10000)
    {
        if (strcmp(lastboard, currboard))
        {
            brc_getpushrecord();
        }
        if (brc_pushunread(ent))
        {
            if (type == '+')
                type = '*';
            else
                type = 'p';
        }
    }

    if ((ent->accessed[0] & FILE_DIGEST))
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
/*
    if (ent->accessed[0] & FILE_DELETED && chk_currBM(currBM))
    {
        if (brc_unread(ent->filename))
            type = 'X';
        else
            type = 'x';
    }
*/
    if (!(ent->accessed[0] & FILE_DELETED) && (digestmode == 9)
        && (HAS_PERM(PERM_SPECIAL7)))
    {
        if (brc_unread(ent->filename))
            type = 'D';
        else
            type = 'd';
    }

    if (ent->accessed[0] & FILE_TOP1)
    {
        type = '#';
    }
    if ((ent->accessed[1] & FILE_RECOMMENDED))
    {
        type = '*';
    }

    if (ent->accessed[1] & FILE_NOREPLY2 || bp->flag & NOREPLY_FLAG)
        noreply = 2;
    else if (ent->accessed[0] & FILE_NOREPLY)
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

    if (!strncmp("Re: ", ent->title, 4))
        maxlength = 50;
    else
        maxlength = 47;
    if (ent->title[maxlength])
    {
        strnzhcpy(TITLE, ent->title, maxlength - 3);
        pos = strlen(TITLE) - 1;
        while (TITLE[pos] == ' ' || TITLE[pos] == '\t')
        {
            TITLE[pos] = '\0';
            pos--;
        }
        if (strlen(TITLE) == maxlength - 3)
            strcat(TITLE, "..");
        else if (strlen(TITLE) == maxlength - 4)
            strcat(TITLE, "...");
    }
    else
        strncpy(TITLE, ent->title, maxlength);

    if (strncmp(ent->owner, "Anony.", 6) == 0)
        sprintf(owner_display, currboard);
    else
        sprintf(owner_display, "%s", ent->owner);

    if (FFLL == 0)
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
            sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ",
                    num, inAnnounce == 1 ? "\033[4m" : "", type,
                    inAnnounce == 1 ? "\033[m" : "", owner_display, color,
                    date, (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
		    (noreply ? "\033[0;1;4;33mx\033[m" : " "), TITLE);
        }
        else
        {
            sprintf(buf, " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",
                    num, inAnnounce == 1 ? "\033[4m" : "", type,
                    inAnnounce == 1 ? "\033[m" : "", owner_display, color,
                    date, (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
		    (noreply ? "\033[0;1;4;33mx\033[m" : " "), TITLE);
        }
    }
    else
    {
        if (!strncmp("Re: ", ent->title, 4))
        {
            if (reply_id != 0)
            {
                if ((ent->reid == reply_id) && (ent->reid == ent->id))
                {
                    sprintf(buf,
                            " \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m%-.48s\033[m ",
                            num,
                            inAnnounce == 1 ? "\033[4m" : "",
                            type,
                            inAnnounce == 1 ? "\033[m" : "",
                            owner_display, color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
                else if ((ent->reid == reply_id) && (ent->reid != ent->id))
                {
                    sprintf(buf,
                            " \033[1;36m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
                            num,
                            inAnnounce == 1 ? "\033[4m" : "",
                            type,
                            inAnnounce == 1 ? "\033[m" : "",
                            owner_display, color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
                else
                {
                    sprintf(buf,
                            " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
            }
            else
            {
                if ((strncmp(ReplyPost, ent->title, 40) == 0)
                    && (ent->reid == reply_id))
                {
                    sprintf(buf,
                            " \033[1;36m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;36m%-.48s\033[m ",
                            num,
                            inAnnounce == 1 ? "\033[4m" : "",
                            type,
                            inAnnounce == 1 ? "\033[m" : "",
                            owner_display, color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
                else
                {
                    sprintf(buf,
                            " %5d %s%c%s %-12.12s %s%6.6s\033[m %s%-.48s ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
            }
        }
        else
        {
            if (reply_id != 0)
            {
                if (ent->reid == reply_id)
                {
                    sprintf(buf,
                            " \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m¡ô %-.45s\033[m ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
                else
                {
                    sprintf(buf,
                            " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
            }
            else
            {
                if ((strncmp(ReadPost, ent->title, 40) == 0)
                    && (ent->reid == reply_id))
                {
                    sprintf(buf,
                            " \033[1;32m%5d\033[m %s%c%s %-12.12s %s%6.6s.%s\033[1;32m¡ô %-.45s\033[m ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
                else
                {
                    sprintf(buf,
                            " %5d %s%c%s %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",
                            num, inAnnounce == 1 ? "\033[4m" : "", type,
                            inAnnounce == 1 ? "\033[m" : "", owner_display,
                            color, date,
                            (noreply > 1) ? "\033[0;1;4;31mx\033[m" :
			    (noreply ? "\033[0;1;4;33mx\033[m" : " "),
                            TITLE);
                }
            }
        }

    }

    setbdir(path, currboard);
    if ((stat(path, &st) != -1)
        && ((int) (st.st_size / sizeof(struct fileheader)) < num))
    {
        ent->accessed[0] |= FILE_TOP1;
        if ((ent->accessed[0] & FILE_MARKED)
            && (ent->accessed[0] & FILE_DIGEST))
            strcpy(msg, "[°å¹æ]");
        else if (ent->accessed[0] & FILE_DIGEST)
            strcpy(msg, "[Í¨Öª]");
        else if (ent->accessed[0] & FILE_MARKED)
            strcpy(msg, "[ÌáÊ¾]");
        else
            strcpy(msg, "[ÖÃµ×]");
        msg[16] = 0;

        int weekday = ((filetime + 28800) / 86400 + 4) % 7;

        sprintf(color, "\033[1;%dm", 31 + weekday);

        sprintf(buf,
                " \033[1;31m%6s\033[m  %-12.12s %s%6.6s\033[m %s¡ô %-.45s ",
                msg, owner_display, color, date, " ", TITLE);
    }

    noreply = 0;
    return buf;
}

char currfile[STRLEN];
int cmpfilename(void *vfhdr)
{
    struct fileheader *fhdr = (struct fileheader *) vfhdr;

    if (!strncmp(fhdr->filename, currfile, STRLEN))
        return 1;
    return 0;
}

int cmpdigestfilename(char *digest_name, void *vfhdr)
{
    struct fileheader *fhdr = (struct fileheader *) vfhdr;

    if (!strcmp(fhdr->filename, digest_name))
        return 1;
    return 0;
}

int cpyfilename(struct fileheader *fhdr)
{
    char buf[STRLEN];
    time_t tnow;
    struct tm *now;

    tnow = time(0);
    now = localtime(&tnow);
    sprintf(buf, "-%s", fhdr->owner);
    strncpy(fhdr->owner, buf, IDLEN + 2);
    sprintf(buf, "<< ±¾ÎÄ±» %s ÓÚ %d/%d %d:%02d:%02d É¾³ý >>",
            currentuser.userid, now->tm_mon + 1, now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec);
    mystrlcpy(fhdr->title, buf, STRLEN);
    fhdr->filename[STRLEN - 1] = 'L';
    fhdr->filename[STRLEN - 2] = 'L';
    return 0;
}

int do_reply_push(struct fileheader *ptitle, int ent, char *direct)
{

    char filepath[STRLEN], content[80], pushbuf[2048], pfilename[80];
    FILE *fp;

    snprintf(pfilename, 80, "%s", ptitle->filename);
    pfilename[0] = 'P';
    snprintf(filepath, 80, "boards/%s/%s/%s", currboard,
             getfolderstr(pfilename), pfilename);
    getdata(t_lines - 2, 0, "ÍÆÎÄ: ", content, 72, DOECHO, YEA);
    if (!strcmp(content, ""))
        return -1;
    format_pushbuf(content, pushbuf);
    if ((fp = fopen(filepath, "a")) == NULL)
    {
        return -1;
    }
    fprintf(fp, "%s\n", pushbuf);
    fclose(fp);
    if (ptitle->pnum > 10000)
        ptitle->pnum = 0;
    ptitle->pnum++;
    substitute_record(direct, ptitle, sizeof(struct fileheader), ent);
    brc_updatepush(ptitle);
    return 0;

}

int read_pushpost(int ent, struct fileheader *fileinfo, char *direct)
{
    char *t;
    char buf[512], pushfilename[80];
    int ch;
    int cou;

    brc_addlist(fileinfo->filename);
    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    snprintf(pushfilename, 80, "%s", fileinfo->filename);
    pushfilename[0] = 'P';
    sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(pushfilename),
            pushfilename);
    brc_updatepush(fileinfo);
    if (!dashf(genbuf))
    {
        clear();
        move(10, 30);
        prints("¶Ô²»Æð£¬±¾ÎÄÄÚÈÝ¶ªÊ§£¡(%s)", genbuf);
        pressanykey();
        return FULLUPDATE;
    }
    strncpy(quote_file, genbuf, sizeof(quote_file));
    strcpy(quote_board, currboard);

    current_article_reid = fileinfo->reid;

    strcpy(quote_title, fileinfo->title);
    quote_file[sizeof(quote_file)-1] = fileinfo->filename[STRLEN - 2];
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
        prints
            ("\033[0;1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m»ØÐÅ R ©¦ ½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X»òp \033[m");
    }
    else
    {
        prints
            ("\033[0;1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,<Enter>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X »ò p \033[m");
    }

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
        {

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
            extern struct shortfile *getbcache(char *bname);

            bp = getbcache(currboard);
            noreply = fileinfo->accessed[0] & FILE_NOREPLY || fileinfo->accessed[1] & FILE_NOREPLY2
                || bp->flag & NOREPLY_FLAG;
            if (!noreply || HAS_PERM(PERM_SYSOP) || chk_currBM(currBM))
            {
                local_article = !(fileinfo->filename[STRLEN - 1] == 'S');
                if (bp->flag & PUSH_FLAG)
                    do_reply_push(fileinfo, ent, direct);
                else
                    do_reply(fileinfo->title, fileinfo->owner,
                             fileinfo->reid);
            }
            else
            {
                clear();
                move(5, 6);
                prints
                    ("¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.");
                pressreturn();
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
        locate_the_post(fileinfo, fileinfo->title, 5, 0, 1);
        sread(1, 0, fileinfo);
        break;
    case Ctrl('S'):
    case Ctrl('X'):
    case 'p':
        sread(0, 0, fileinfo);
        break;
    case Ctrl('A'):

        clear();
        show_author(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case 'S':
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

int read_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char *t;
    char buf[512];
    int ch;
    int cou;

    brc_addlist(fileinfo->filename);
    if (fileinfo->owner[0] == '-')
        return FULLUPDATE;
    clear();
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(fileinfo->filename),
            fileinfo->filename);
    if (!dashf(genbuf))
    {
        clear();
        move(10, 30);
        prints("¶Ô²»Æð£¬±¾ÎÄÄÚÈÝ¶ªÊ§£¡(%s)", genbuf);
        pressanykey();
        return FULLUPDATE;
    }
    strncpy(quote_file, genbuf, sizeof(quote_file));
    strcpy(quote_board, currboard);

    current_article_reid = fileinfo->reid;

    strcpy(quote_title, fileinfo->title);
    quote_file[sizeof(quote_file)-1] = fileinfo->filename[STRLEN - 2];
    strcpy(quote_user, fileinfo->owner);
#ifndef NOREPLY

    ch = ansimore_withmode(genbuf, NA, MORE_MODE_POST);
#else

    ch = ansimore_withmode(genbuf, YEA, MORE_MODE_POST);
#endif
#ifndef NOREPLY

    move(t_lines - 1, 0);
    clrtoeol();
    if (haspostperm(currboard))
    {
        prints
            ("\033[0;1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m»ØÐÅ R ©¦ ½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X»òp \033[m");
    }
    else
    {
        prints
            ("\033[0;1;44;31m[ÔÄ¶ÁÎÄÕÂ]  \033[33m½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,<Enter>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X »ò p \033[m");
    }

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

    // refresh();
#ifndef BBSD

    sleep(1);
#endif

    if (ch == 0)
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
        {
            sread(0, 0, fileinfo);
            break;
        }
        else
            return READ_NEXT;
    case KEY_DOWN:
    case KEY_PGDN:
    case ']':
        return READ_NEXT;
    case KEY_UP:
    case KEY_PGUP:
    case '[':
        return READ_PREV;
    case 'a':
    case 'A':
        {
            if (fileinfo->pnum > 10000)
                fileinfo->pnum = 0;
            if (fileinfo->pnum > 0)
                read_pushpost(ent, fileinfo, direct);
            break;
        }
    case 'Y':
    case 'R':
    case 'x':
    case 'X':
        {
            struct shortfile *bp;
            extern struct shortfile *getbcache(char *bname);

            bp = getbcache(currboard);
            noreply = fileinfo->accessed[0] & FILE_NOREPLY || fileinfo->accessed[1] & FILE_NOREPLY2
                || bp->flag & NOREPLY_FLAG;
            if (!noreply || HAS_PERM(PERM_SYSOP) || chk_currBM(currBM))
            {
                local_article = !(fileinfo->filename[STRLEN - 1] == 'S');
                if (bp->flag & PUSH_FLAG)
                    do_reply_push(fileinfo, ent, direct);
                else
                    do_reply(fileinfo->title, fileinfo->owner,
                             fileinfo->reid);
            }
            else
            {
                clear();
                move(5, 6);
                prints
                    ("¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.");
                pressreturn();
            }
            break;
        }
    case 'y':
    case 'r':
        {
            struct shortfile *bp;

            bp = getbcache(currboard);
            noreply = fileinfo->accessed[0] & FILE_NOREPLY || fileinfo->accessed[1] & FILE_NOREPLY2
                || bp->flag & NOREPLY_FLAG;
            local_article = (fileinfo->filename[STRLEN - 2] == 'L');
            if (!noreply || HAS_PERM(PERM_SYSOP) || chk_currBM(currBM))
            {
                do_reply(fileinfo->title, fileinfo->owner, fileinfo->reid);

            }
            else
            {
                clear();
                prints
                    ("\n\n    ¶Ô²»Æð, ¸ÃÎÄÕÂÓÐ²»¿É RE ÊôÐÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.    ");
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
        locate_the_post(fileinfo, fileinfo->title, 5, 0, 1);
        sread(1, 0, fileinfo);
        break;
    case Ctrl('S'):
    case Ctrl('X'):
    case 'p':
    case 'P':
        sread(0, 0, fileinfo);
        break;
    case Ctrl('A'):

        clear();
        show_author(0, fileinfo, '\0');
        return READ_NEXT;
        break;
    case 'S':
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

int skip_post(int ent, struct fileheader *fileinfo, char *direct)
{
    brc_addlist(fileinfo->filename);
    return GOTO_NEXT;
}

int do_select(int ent, struct fileheader *fileinfo, char *direct)
{
    char bname[STRLEN], bpath[STRLEN];
    struct stat st;

    if (digestmode == 9)
        return DONOTHING;
    move(0, 0);
    clrtoeol();
    prints("Ñ¡ÔñÒ»¸öÌÖÂÛÇø (Ó¢ÎÄ×ÖÄ¸´óÐ¡Ð´½Ô¿É)\n");
    clrtoeol();
    prints("ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ");
    clrtoeol();

    make_blist();
    namecomplete((char *) NULL, bname);
    if (*bname == '\0')
        return FULLUPDATE;
    setbpath(bpath, bname);
    if ((stat(bpath, &st) == -1))
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

    if ((!HAS_PERM(PERM_SYSOP)) || (!strncmp(bname, "Arbitrator", 10))
        || (!strncmp(currboard, "OscarAward", 10)))
    {
        setbfile(genbuf, bname, "board.allow");
        if (dashf(genbuf) && !seek_in_file(genbuf, currentuser.userid))
        {
            clear();
            move(5, 10);
            prints("ºÜ±§Ç¸, ±¾°åÃæÊôÓÚÍÅÌå°åÃæ£¬¶øÄú²»ÊÇ¸Ã°å³ÉÔ±");
            pressanykey();
            return FULLUPDATE;
        }
    }
#endif

    userexitboard(currboard);

    selboard = 1;
    brc_initial(bname);

    if (0 == strcmp("master", currboard))
    {
        report("enter board master");
    }

    userenterboard(currboard);

    move(0, 0);
    clrtoeol();
    move(1, 0);
    clrtoeol();
    setbdir(direct, currboard);
    return NEWDIRECT;
}

void do_acction(int type)
{
    char buf[STRLEN];

    move(t_lines - 1, 0);
    clrtoeol();
    prints("\033[1;5mÏµÍ³´¦Àí±êÌâÖÐ, ÇëÉÔºò...\033[m\n");
    refresh();
    switch (type)
    {
    case 2:
        sprintf(buf, "bin/thread %s", currboard);
        system(buf);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        marked_all(type - 3);
        break;
    }
}

int acction_mode(int ent, struct fileheader *fileinfo, char *direct)
{
    int type;
    extern char currdirect[STRLEN];
    char ch[4];

    if (digestmode != NA)
    {
        if (digestmode == 5 || digestmode == 6)
        {
            sprintf(genbuf, "boards/%s/SOMEONE.%s.DIR.%d", currboard,
                    someoneID, digestmode - 5);
            unlink(genbuf);
        }
        else if (digestmode == 7)
        {
            sprintf(genbuf, "boards/%s/KEY.%s.DIR", currboard,
                    currentuser.userid);
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

        if (!chk_currBM(currBM))
            getdata(t_lines - 1, 0,
                    "ÇÐ»»Ä£Ê½µ½: 1)ÎÄÕª 2)Í¬Ö÷Ìâ 3)±» m ÎÄÕÂ 4)Ô­×÷ 5)Í¬×÷Õß 6)±êÌâ¹Ø¼ü×Ö [1]: ",
                    ch, 3, DOECHO, YEA);
        else
            getdata(t_lines - 1, 0,
                    "ÇÐ»»Ä£Ê½µ½:1)ÎÄÕª2)Í¬Ö÷Ìâ3)±»mÎÄÕÂ4)Ô­×÷5)Í¬×÷Õß6)±êÌâ¹Ø¼ü×Ö9)»ØÊÕÕ¾ [1]: ",
                    ch, 3, DOECHO, YEA);

        if (ch[0] == '\0')
            ch[0] = '1';
        type = atoi(ch);

        if (type < 1 || (type > 6 && (type != 9))
            || ((type == 9) && (!chk_currBM(currBM))))
        {

            saveline(t_lines - 1, 1);
            return PARTUPDATE;
        }
        else if (type == 6)
        {
            getdata(t_lines - 1, 0, "ÄúÏë²éÕÒµÄÎÄÕÂ±êÌâ¹Ø¼ü×Ö: ",
                    someoneID, 30, DOECHO, YEA);
            if (someoneID[0] == '\0')
            {
                saveline(t_lines - 1, 1);
                return PARTUPDATE;
            }
            type = 7;
        }
        else if (type == 5)
        {
            if (fileinfo != NULL)
                strcpy(someoneID, fileinfo->owner);
            else
                someoneID[0] = '\0';
            getdata(t_lines - 1, 0, "ÄúÏë²éÕÒÄÄÎ»ÍøÓÑµÄÎÄÕÂ? ", someoneID,
                    13, DOECHO, YEA_FOR_ONCE);
            if (someoneID[0] == '\0')
            {
                saveline(t_lines - 1, 1);
                return PARTUPDATE;
            }
            getdata(t_lines - 1, 37,
                    "¾«È·²éÕÒ°´ Y£¬ Ä£ºý²éÕÒÇë»Ø³µ[Enter]", ch, 2, DOECHO,
                    YEA);
            if (ch[0] == 'y' || ch[0] == 'Y')
                type = 6;
        }

        digestmode = type;
        setbdir(currdirect, currboard);
        if (digestmode != 1)
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

int dele_digest_top(char *dname, char *direc, char *flag)
{
    char digest_name[STRLEN];
    char new_dir[STRLEN];
    char buf[STRLEN];
    char *ptr;
    struct fileheader fh;
    int pos;

    strncpy(digest_name, dname, STRLEN);
    strcpy(new_dir, direc);
    if (!strcmp(flag, DIGEST_DIR))
        digest_name[0] = 'G';
    else if (!strcmp(flag, TOPFILE_DIR))
        digest_name[0] = 'T';
    else if (!strcmp(flag, DOT_DIR))
        digest_name[0] = 'M';

    ptr = strrchr(new_dir, '/') + 1;

    strcpy(ptr, flag);
    strncpy(buf, currfile, STRLEN);
    strncpy(currfile, digest_name, STRLEN);
    pos =
        search_record(new_dir, &fh, sizeof(fh), cmpdigestfilename,
                      digest_name);
    if (pos > 0)
        delete_file(new_dir, sizeof(struct fileheader), pos, cmpfilename);
    strncpy(currfile, buf, STRLEN);
    *ptr = '\0';
    sprintf(buf, "%s%s/%s", new_dir, getfolderstr(digest_name),
            digest_name);
    unlink(buf);
    if (pos <= 0)
        return -1;
    else
        return 0;
}

int move_digest_post(int ent, struct fileheader *fhdr, char *direct)
{
    char buf[STRLEN];
    char num[8];
    int dst;

    sprintf(buf, "ÇëÊäÈëµÚ %d ÆªÎÄÕªµÄÐÂ´ÎÐò: ", ent);
    getdata(t_lines - 1, 0, buf, num, 6, DOECHO, YEA);
    dst = atoi(num);
    if (dst <= 0)
    {
        move(t_lines - 1, 50);
        prints("´íÎó±àºÅ...");
        egetch();
        return PARTUPDATE;
    }
    strncpy(buf, currfile, STRLEN);
    strncpy(currfile, fhdr->filename, STRLEN);
    move_file(direct, sizeof(struct fileheader), ent, dst, cmpfilename);
    strncpy(currfile, buf, STRLEN);
    log_bm_ops(currboard, "°åÃæ", "gg", fhdr->title);
    return DIRCHANGED;
}

int digest_post(int ent, struct fileheader *fhdr, char *direct)
{
#ifdef LOGHANDLER
    char query_string[4096], title_ok[256];
    int sqllen = 0;
#endif

    if (!chk_currBM(currBM))
        return DONOTHING;
    if (digestmode == 1)
        return move_digest_post(ent, fhdr, direct);

    if (fhdr->accessed[0] & FILE_DIGEST)
    {
        fhdr->accessed[0] = (fhdr->accessed[0] & ~FILE_DIGEST);

        dele_digest_top(fhdr->filename, direct, DIGEST_DIR);

#ifdef LOGHANDLER
        if (!strcmp(currboard, "Search"))
        {
            sqllen = snprintf(query_string, sizeof(query_string),
                              LOGHANDLER_SQL
                              "DELETE FROM `search_found` WHERE filename = '%s'\n",
                              fhdr->filename);
            loghander_write(query_string, sqllen);
        }
        else if (!strcmp(currboard, "Notice"))
        {
            sqllen = snprintf(query_string, sizeof(query_string),
                              LOGHANDLER_SQL
                              "DELETE FROM `notice` WHERE filename = '%s'\n",
                              fhdr->filename);
            loghander_write(query_string, sqllen);
        }
#endif

        log_bm_ops(currboard, "°åÃæ", "!g", fhdr->title);
    }
    else
    {
        struct fileheader digest;
        char *ptr, buf[64];

        memcpy(&digest, fhdr, sizeof(digest));
        digest.filename[0] = 'G';
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(digest.filename),
                digest.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        digest.accessed[0] = 0;
        sprintf(&genbuf[512], "%s/%s/%s", buf,
                getfolderstr(fhdr->filename), fhdr->filename);
        strcpy(ptr, DIGEST_DIR);
        if (get_num_records(buf, sizeof(digest)) > MAX_DIGEST)
        {
            move(3, 0);
            clrtobot();
            move(4, 10);
            prints("±§Ç¸£¬ÄãµÄÎÄÕªÎÄÕÂÒÑ¾­³¬¹ý %d Æª£¬ÎÞ·¨ÔÙ¼ÓÈë...\n",
                   MAX_DIGEST);
            pressanykey();
            return PARTUPDATE;
        }
        link(&genbuf[512], genbuf);
        append_record(buf, &digest, sizeof(digest));
        fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
        fhdr->accessed[0] &= ~FILE_DELETED;

#ifdef LOGHANDLER
        if (!strcmp(currboard, "Search"))
        {
            mysql_escape_string(title_ok, fhdr->title,
                                strlen(fhdr->title));
            if (!strncmp(title_ok, "[ÕÐÁì]", 6))
            {
                sqllen = snprintf(query_string, sizeof(query_string),
                                  LOGHANDLER_SQL
                                  "INSERT INTO `search_found` ( `id`, `reid`, `title`, `filename`) VALUES ('', '%d', '%s', '%s')\n",
                                  fhdr->reid, title_ok + 6,
                                  fhdr->filename);
                loghander_write(query_string, sqllen);
            }
        }
        else if (!strcmp(currboard, "Notice"))
        {
            mysql_escape_string(title_ok, fhdr->title,
                                strlen(fhdr->title));
            if (fhdr->title[0] == '[' && fhdr->title[9] == ']')
            {
                sqllen = snprintf(query_string, sizeof(query_string),
                                  LOGHANDLER_SQL
                                  "INSERT INTO `notice` ( `id`, `reid`, `title`, `filename`) VALUES ('', '%d', '%s', '%s')\n",
                                  fhdr->reid, title_ok + 10,
                                  fhdr->filename);
                loghander_write(query_string, sqllen);
            }
        }
#endif

        log_bm_ops(currboard, "°åÃæ", "g", fhdr->title);
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);

    return PARTUPDATE;
}

#ifndef NOREPLY
int do_reply(char *title, char *id, unsigned int reid)
{
    strncpy(replytitle, title,49);
    post_article(currboard, id, &reid, NA);
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int garbage_line(char *str)
{
    int qlevel = 0;

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
        if (strstr(str, "Ìáµ½:\n") || strstr(str, ": ¡¿\n")
            || strncmp(str, "==>", 3) == 0 || strstr(str, "µÄÎÄÕÂ ¡õ"))
            return 1;
    return (*str == '\n');
}

int Origin2(char *text)
{
    char tmp[STRLEN];

    sprintf(tmp, ":¡¤%s %s¡¤[FROM:", BoardName, BBSHOST);
    if (strstr(text, tmp))
        return 1;
    else
        return 0;
}

void do_quote(char *filepath, char quote_mode)
{
    FILE *inf, *outf;
    char *qfile, *quser;
    char buf[256], *ptr;
    char op;
    int bflag, i;
    FILE *module;

    qfile = quote_file;
    quser = quote_user;
    bflag = strncmp(qfile, "mail", 4);
    outf = fopen(filepath, "a");

    if (*module_file != '\0')
    {
        strcat(module_file, "_M");
        setmodulefile(buf, currboard, module_file);
        if ((module = fopen(buf, "r")) != NULL)
        {
            while (fgets(buf, 256, module) != NULL)
            {
                fprintf(outf, buf);
            }
            fclose(module);
            *module_file = '\0';
        }
    }

    if (quote_mode != '\0' && *qfile != '\0'
        && (inf = fopen(qfile, "r")) != NULL)
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
                        if (op == 'S' && i >= 10)
                        {
                            fprintf(outf,
                                    ": .................£¨ÒÔÏÂÊ¡ÂÔ£©");
                            break;
                        }
                        i++;
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

void getcross(char *filepath, int mode)
{
    FILE *inf, *of;
    char buf[256];
    char owner[248];
    char datetime[32];
    char datestr[STRLEN] = "";
    char *filename;
    char *left, *right;
    int count, owner_found = 0;
    time_t now;

    now = time(0);
    inf = fopen(quote_file, "r");
    of = fopen(filepath, "w");
    filename = &quote_file[strlen(quote_file) - 1];
    datetime[0] = '\0';
    while (*filename != '/')
        filename--;
    filename++;
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
            write_header(of, 1, 1);
            in_mail = YEA;
        }
        else
            write_header(of, 1, 1);
        if (fgets(buf, 256, inf) != NULL)
        {
            if (strncmp(&buf[2], "ÐÅÈË: ", 6))
            {
                owner_found = 0;
                strcpy(owner, "Unkown User");
            }
            else
            {
                for (count = 8;
                     buf[count] != ' ' && buf[count] != '\n'
                     && buf[count] != '\0'; count++)
                    owner[count - 8] = buf[count];
                owner[count - 8] = '\0';
                owner_found = 1;
            }
            fgets(buf, 256, inf);
            if (fgets(buf, 256, inf) != NULL){
                left = strstr(buf, "·¢ÐÅÕ¾:");
                if (left){
                   while (*left && *left != '(')
                       left++;
                   right = left;
                   while (*right && *right != ')')
                       right++;
                   if (right > left){
                        *right = 0;
                        sprintf(datetime, "%s", left + 1);
                        *right = ')';
                   }    
               }    
           }    
        }
        if (!datetime[0])
           sprintf(datetime, "Î´ÖªÊ±¼ä");
        if (in_mail == YEA)
            fprintf(of,
                    "\033[1;37m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô \033[32m%s \033[37mµÄÐÅÏä ¡¿\n",
                    currentuser.userid);
        else
            fprintf(of,
                    "\033[1;37m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô \033[32m%s \033[37mÌÖÂÛÇø ¡¿\n",
                    quote_board);
        if (owner_found)
        {
            if (fgets(buf, 256, inf) != NULL)
                if (buf[0] != '\n')
                    fgets(buf, 256, inf);
            fprintf(of, "¡¾ Ô­ÎÄÓÉ\033[32m %s\033[37m ÓÚ \033[33m%s\033[37m ·¢±í ¡¿\033[m\n",
                    owner, datetime);
        }
        else
            fseek(inf, (long) 0, SEEK_SET);
        if (in_mail != YEA)
           fprintf(of, "[[Post:%s,%s|¡¾ Ô­ÎÄÁ´½Ó ¡¿]]\n\n", quote_board, filename);
    }
    else if (mode == 1)
    {
        fprintf(of,
                "\033[1;41;33m·¢ÐÅÈË: %s (×Ô¶¯·¢ÐÅÏµÍ³), ÐÅÇø: %s%*s\033[m\n",
                BBSID, quote_board,
                48 - strlen(BBSID) - strlen(quote_board), " ");
        fprintf(of, "±ê  Ìâ: %s\n", quote_title);
        getdatestring(now, NA);
        fprintf(of, "·¢ÐÅÕ¾: %s\033[33m×Ô¶¯·¢ÐÅÏµÍ³\033[m(%s)\n\n",
                BoardName, datestring);
    }
    else if (mode == 2)
    {
        write_header(of, 0, 1);
    }
    else if (mode == 3)
    {
        char *cppos, *cpaux;
        time_t now;

        now = time(0);
        if (fgets(buf, 256, inf) != NULL)
        {
            for (cppos = strstr(buf, "ÐÅÇø: "); (cppos != NULL) && ((cpaux = strstr(&cppos[1], "ÐÅÇø: ")) != NULL); cppos = cpaux)
                ;
            if (cppos != NULL)
                sprintf(cppos, "ÐÅÇø: %s\n", currboard);
            fputs(buf, of);
        }
        for (; (fgets(buf, 256, inf) != NULL) && (buf[0] != '\n'); )
        {
            if (!strncmp(buf, "·¢ÐÅÕ¾: ", 8) && ((cppos = strchr(buf, '(')) != NULL) && ((cpaux = strrchr(buf, ')')) != NULL))
            {
                if ((cpaux - cppos == 30) && (cppos[17] == ':') && (cppos[20] == ':'))
                {
                    cppos[15] = '\0';
                    sprintf(datestr, " %s ", &cppos[1]);
                }
                else if ((cpaux - cppos == 25) && (cppos[14] == ':') && (cppos[17] == ':'))
                {
                    cppos[11] = cppos[25] = '\0';
                    sprintf(datestr, " %s, %s ", &cppos[5], &cppos[21]);
                }
            }
        }
        fprintf(of, "±ê  Ìâ: %s\n", save_title);
        getdatestring(now, NA);
        fprintf(of, "·¢ÐÅÕ¾: %s (%s), %s\n", BoardName, datestring, (local_article) ? "Õ¾ÄÚÐÅ¼þ" : "×ªÐÅ");
        fprintf(of, "\n");
        fprintf(of, "\033[1;37m¡¾ ±¾ÎÄ\033[32m%s\033[37mÔ­ÔØÓÚ \033[32m%s \033[37mÌÖÂÛÇø ¡¿\033[m\n\n", datestr, quote_board);
    }
    while (fgets(buf, 256, inf) != NULL)
    {
        fprintf(of, "%s", buf);
    }
    fclose(inf);
    fclose(of);
    *quote_file = '\0';
}

int do_post(void)
{
    *quote_file = '\0';
    *quote_user = '\0';
    local_article = YEA;
    return post_article(currboard, (char *) NULL, NULL, 0);
}

int post_reply(int ent, struct fileheader *fileinfo, char *direct)
{
    char uid[STRLEN];
    char title[STRLEN];
    char *t;
    FILE *fp;

    if (!strcmp(currentuser.userid, "guest"))
        return DONOTHING;
    clear();
    if (check_maxmail())
    {
        pressreturn();
        return FULLUPDATE;
    }
    modify_user_mode(SMAIL);

    setbfile(quote_file, currboard, fileinfo->filename);
    strcpy(quote_user, fileinfo->owner);

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

    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' ||
        fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, STRLEN - 5);

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
        prints("Ê¹ÓÃÕß '%s' ÎÞ·¨ÊÕÐÅ£¬ÐÅÏäÒÑÂú\n", uid);
        break;
    default:
        prints("ÐÅ¼þÒÑ³É¹¦µØ¼Ä¸øÔ­×÷Õß %s\n", uid);
    }
    pressreturn();
    in_mail = NA;
    return FULLUPDATE;
}

int post_cross(char islocal, int mode)
{
    struct fileheader postfile;
    struct shortfile *bp;
    char filepath[STRLEN], fname[STRLEN];
    char buf[256], buf4[STRLEN], whopost[IDLEN + 2];
    int fp, count;
    time_t now;
    char tempbuffer[STRLEN];

    if (!haspostperm(currboard) && !mode)
    {
        prints("\n\nÄúÉÐÎÞÈ¨ÏÞÔÚ %s °å·¢±íÎÄÕÂ£¬È¡Ïû×ªÔØ\n", currboard);
        return -1;
    }
    setbfile(tempbuffer, currboard, "board.allow");
    if ((dashf(tempbuffer)
         && !seek_in_file(tempbuffer, currentuser.userid)) && !mode)
    {
        prints("\n\nÄú²»ÊÇ %s °å³ÉÔ±£¬È¡Ïû×ªÔØ\n", currboard);
        return -1;
    }

    memset(&postfile, 0, sizeof(postfile));
    now = time(0);
    sprintf(fname, "M.%ld.A", now);
    if (!mode)
    {
        if (!strstr(quote_title, "(×ªÔØ)"))
            sprintf(buf4, "%.70s(×ªÔØ)", quote_title);
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
    else if (mode == 3)
        strcpy(whopost, quote_user);
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
    mystrlcpy(postfile.title, save_title, STRLEN);

    postfile.id = now;
    postfile.reid = now;

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
            sprintf(buf,
                    "cross_posting '%s' on '%s': append_record failed!",
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
        sprintf(buf, "cross_posted '%s' on '%s'", postfile.title,
                currboard);
        report(buf);
    }
    if (mode == 3)
    {
        FILE *fp;
        if ((fp = fopen(filepath, "a")) != NULL)
        {
            fprintf(fp,
                    "--\n\033[1;36m¡ù µ¼¶Á:¡¤%s ÓÚ %16.16s ÍÆ¼ö±¾ÎÄ¡¤[FROM: %-.46s][m\n",
                    currentuser.userid, datestring + 6, currentuser.lasthost); //fromhost
            fclose(fp);
        }
    }
    return 1;
}

void add_crossinfo(char *filepath, int mode)
{
    FILE *fp;
    int color;

    color = (currentuser.numlogins % 7) + 31;
    if ((fp = fopen(filepath, "a")) == NULL)
        return;
    fprintf(fp,
            "--\n\033[m\033[1;%2dm¡ù ×ª%s:¡¤%s %s¡¤[FROM: %-.46s]\033[m\n",
            color, (mode == 1) ? "ÔØ" : "¼Ä", BoardName, BBSHOST,
            fromhost);
    fclose(fp);
    return;
}

int show_board_notes(char *bname, int command)
{
    char buf[256];

    sprintf(buf, "vote/%s/notes", bname);
    if (dashf(buf))
    {
        if (command == 1)
        {
            ansimore_partial(buf, NA, 0, 19);
        }
        else
        {
            ansimore(buf, YEA);
        }
        return 1;
    }
    else if (dashf("vote/notes"))
    {
        if (command == 1)
        {
            ansimore_partial("vote/notes", NA, 0, 19);
        }
        else
        {
            ansimore("vote/notes", YEA);
        }
        return 1;
    }
    return -1;
}

int show_user_notes(void)
{
    char buf[256];

    setuserfile(buf, "notes");
    if (dashf(buf))
    {
        ansimore(buf, YEA);
        return FULLUPDATE;
    }
    clear();
    move(10, 15);
    prints("ÄúÉÐÎ´ÔÚ InfoEdit->WriteFile ±à¼­¸öÈË±¸ÍüÂ¼¡£\n");
    pressanykey();
    return FULLUPDATE;
}

void outgo_post(struct fileheader *fh, char *board)
{
    char buf[256];

    sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board,
            fh->filename, header.chk_anony ? board : currentuser.userid,
            header.chk_anony ? "ÎÒÊÇÄäÃûÌìÊ¹" : currentuser.username,
            save_title);
    file_append("innd/out.bntp", buf);
    return;
}

int too_young(void)
{
    if (strcmp(currboard, "BBSHelp") == 0)
        return 0;

    if (strcmp(currboard, "sysop") == 0)
        return 0;

    if ((time(0) - currentuser.firstlogin) > 86400)
        return 0;

    return 1;
}

#ifdef KEYWORD_FILTER
void keywordfilter(char *filepath, struct BCACHE *pCache)
{
    FILE *fp = NULL;
    struct stat st;
    int item = 0;
    char *filecontent = NULL;
    char *noheadcontent = NULL;
    char *ptr;
    int i = 0;
    int k = 0;

    if (pCache == NULL)
        return;
    if (filepath == NULL)
        return;
    if (stat(filepath, &st) < 0)
        return;
    filecontent = malloc(st.st_size + 1);
    memset(filecontent, 0x00, st.st_size + 1);
    if (filecontent == NULL)
        return;
    fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        free(filecontent);
        return;
    }
    fread(filecontent, st.st_size, 1, fp);
    fclose(fp);

    noheadcontent = filecontent;
    k = 0;
    while (k < 3)
    {
        if ((*noheadcontent) == '\n')
        {
            k++;
        }
        noheadcontent++;
        if (noheadcontent - filecontent >= st.st_size)
        {
            noheadcontent = filecontent;
            break;
        }
    }

    for (item = 0; item < MAX_ITEM; item++)
    {
        if (strlen(pCache->wordfilter[item]) < 4)
            break;
        ptr = NULL;
        while ((ptr = strstr(noheadcontent, pCache->wordfilter[item])))
        {
            for (i = 0; i < (int) strlen(pCache->wordfilter[item]); i++)
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

int
post_article(char *postboard, char *mailid, unsigned int *p_reid,
             int is_module)
{
    struct fileheader postfile;
    struct shortfile *bp;
    extern struct BCACHE *brdshm;
    char filepath[STRLEN], fname[STRLEN], buf[STRLEN];
    int fp, aborted, count;
    time_t now = time(0);
    static time_t lastposttime = 0;
    static int failure = 0;
    struct tm *tm;

    tm = localtime(&now);
    if (brdshm->all_no_post_end >= brdshm->all_no_post_begin)
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {

            if ((unsigned int) tm->tm_hour >= brdshm->all_no_post_begin
                && (unsigned int) tm->tm_hour < brdshm->all_no_post_end)
            {
                move(5, 10);
                clrtobot();
                prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±È«Õ¾Ö»¶Á¡£",
                       brdshm->all_no_post_begin, brdshm->all_no_post_end);
                pressreturn();
                clear();
                return FULLUPDATE;
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {
            if ((unsigned int) tm->tm_hour >= brdshm->no_post_begin
                && (unsigned int) tm->tm_hour < brdshm->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {
                    move(5, 10);
                    clrtobot();
                    prints
                        ("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
                         brdshm->no_post_begin, brdshm->no_post_end);
                    pressreturn();
                    clear();
                    return FULLUPDATE;
                }
            }
        }
    }
    else
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {

            if ((unsigned int) tm->tm_hour >= brdshm->all_no_post_begin
                || (unsigned int) tm->tm_hour < brdshm->all_no_post_end)
            {
                move(5, 10);
                clrtobot();
                prints("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±È«Õ¾Ö»¶Á¡£",
                       brdshm->all_no_post_begin, brdshm->all_no_post_end);
                pressreturn();
                clear();
                return FULLUPDATE;
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {
            if ((unsigned int) tm->tm_hour >= brdshm->no_post_begin
                || (unsigned int) tm->tm_hour < brdshm->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {
                    move(5, 10);
                    clrtobot();
                    prints
                        ("±¾Õ¾ÓÚÃ¿ÈÕ %d Ê±ÖÁ´ÎÈÕ %d Ê±ÔÝÍ£Ð£ÍâÓÃ»§µÄ·¢ÎÄÈ¨ÏÞ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å²éÔÄÏà¹ØÍ¨ÖªÓëËµÃ÷¡£",
                         brdshm->no_post_begin, brdshm->no_post_end);
                    pressreturn();
                    clear();
                    return FULLUPDATE;
                }
            }
        }

    }
    modify_user_mode(POSTING);

    if ((abs(now - lastposttime) < 3 || failure >= 9999) &&
        (!HAS_PERM(PERM_SYSOP)))
    {
        clear();
        move(5, 10);
        failure++;
        if (failure > 9999)
        {
            if (failure >= 10020)
                abort_bbs();
            prints("¶Ô²»Æð£¬ÄúÔÚ±»È°×è¶à´ÎµÄÇé¿öÏÂ£¬ÈÔ²»¶ÏÊÔÍ¼·¢ÎÄ¡£");
            move(6, 10);
            prints("ÄúÄ¿Ç°±»ÏµÍ³ÈÏ¶¨Îª¹àË®»ú£¬ÇëÍË³öºóÖØÐÂµÇÂ½£¡[%d/20]",
                   failure - 9999);
        }
        else
        {
            prints("ÄúÌ«ÐÁ¿àÁË£¬ÏÈºÈ±­¿§·ÈÐª»á¶ù£¬3 ÃëÖÓºóÔÙ·¢±íÎÄÕÂ¡£\n");
            if (failure > 5)
            {
                move(6, 10);
                prints("ÄúÔÚ±»È°×èµÄÇé¿öÏÂÈÔ¾ÉÊÔÍ¼·¢±íÎÄÕÂ¡£[%d/20]",
                       failure - 5);
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
    if (strcmp(currentuser.userid, "guest")==0)
    {
        move(5, 10);
        clrtobot();
        prints("´ËÌÖÂÛÇøÊÇÎ¨¶ÁµÄ, »òÊÇÄúÉÐÎÞÈ¨ÏÞÔÚ´Ë·¢±íÎÄÕÂ¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    if (!haspostperm(postboard))
    {
        move(5, 10);
        clrtobot();
        prints("´ËÌÖÂÛÇøÊÇÎ¨¶ÁµÄ, »òÊÇÄúÉÐÎÞÈ¨ÏÞÔÚ´Ë·¢±íÎÄÕÂ¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
#ifdef  REAL_NAME_EMAIL
    if (!HAS_PERM(PERM_REALNAME)
        && (strcmp(currboard, "WelcomeNewer") != 0 && strcmp(currboard, "sysop") != 0))
    {

        firstchar = toupper(currentuser.userid[0]);
        sprintf(filename, "home/%c/%c/%s/real_email", firstchar,
                toupper(currentuser.userid[1]), currentuser.userid);

        if ((ftemp = fopen(filename, "r")) == NULL)
        {
            move(5, 10);
            clrtobot();
            prints
                ("ÄúÉÐÎ´°ó¶¨Email£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ¡£\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
        }
        else
        {
            move(5, 10);
            clrtobot();
            prints
                ("ÄúÉÐÎ´¼¤»îEmail£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ¡£\n\n          Çëµ½ÒÔÏÂÐÅÏäÀïÊÕÐÅ²¢µã»÷¼¤»îÁ´½Ó:\n\n          ");
            while (fgets(buffer, 32, ftemp))
            {
                prints("%s\n", buffer);
            }
            prints
                ("\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
            fclose(ftemp);
        }

        pressreturn();
        clear();
        return FULLUPDATE;
    }
#endif
#ifdef	POST_AFTER_ONE_DAY
    if (too_young()
        && (strcmp(currboard, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
    {
        move(5, 10);
        clrtobot();
        prints
            ("Äú×¢²áÊ±¼ä»¹²»Âú24Ð¡Ê±£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ.\n\n          Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
#endif
    memset(&postfile, 0, sizeof(postfile));
    clear();
    show_board_notes(postboard, 1);
    bp = getbcache(postboard);
    if (bp->flag & OUT_FLAG && replytitle[0] == '\0')
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
    if (is_module)
    {
        strcpy(header.title, module_name);
    }
    header.postboard = YEA;
    if (post_header(&header, is_module) == YEA)
    {
        strcpy(postfile.title, header.title);
        strncpy(save_title, postfile.title, STRLEN);
    }
    else
        return FULLUPDATE;
    now = time(0);
    lastposttime = now;
    if (is_module)
    {
        sprintf(fname, "%s%s", module_name, "_M");
    }
    else
    {
        sprintf(fname, "M.%ld.A", now);
    }
    if (!is_module)
    {
        setbfile(filepath, postboard, fname);
    }
    else
    {
        setmodulefile(filepath, postboard, fname);
    }
    count = 0;
    if (!is_module)
    {
        while ((fp =
                open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
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
        fp = open(filepath, O_CREAT | O_WRONLY, 0644);
    }
    fchmod(fp, 0644);
    close(fp);
    strcpy(postfile.filename, fname);
    in_mail = NA;
    strncpy(postfile.owner, (header.chk_anony) ?
            get_anonyid(currentuser.userid) : currentuser.userid, STRLEN);
    if (!is_module)
    {
        setbfile(filepath, postboard, postfile.filename);
    }
    else
    {
        setmodulefile(filepath, postboard, postfile.filename);
    }
    modify_user_mode(POSTING);
    do_quote(filepath, header.include_mode);
    if (p_reid == NULL)
    {
        if (!is_module)
            aborted = vedit_reid(filepath, YEA, YEA, now);
        else
            aborted = vedit_reid(filepath, NA, YEA, now);
    }
    else
    {
        aborted = vedit_reid(filepath, YEA, YEA, *p_reid);
    }
    mystrlcpy(postfile.title, save_title, STRLEN);

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

    if (aborted == -1)
    {
        unlink(filepath);
        if (is_module)
        {
            writeinto_modulefile = 0;
        }
        clear();
        return FULLUPDATE;
    }
    writeinto_modulefile = 1;
#ifdef KEYWORD_FILTER
    keywordfilter(filepath, brdshm);
#endif
    setbdir(buf, postboard);
    if (digestmode > 0)
        sprintf(buf, "boards/%s/%s", currboard, DOT_DIR);
    if (noreply)
    {
        postfile.accessed[0] |= FILE_NOREPLY;
        noreply = 0;
    }

    postfile.id = now;
    if (mailid == NULL)
    {
        if (p_reid == NULL)
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
        if (p_reid == NULL)
        {
            postfile.reid = 0;
            postfile.id = 0;
        }
        else
        {
            postfile.reid = *p_reid;
        }

    }

#ifdef MARK_X_FLAG
    if (markXflag)
    {
        postfile.accessed[0] |= FILE_DELETED;
        markXflag = 0;
    }
    else
        postfile.accessed[0] &= ~FILE_DELETED;
#endif
    postfile.accessed[2] = 0;
    if (mailtoauther)
    {
        if (header.chk_anony)
            prints("¶Ô²»Æð£¬Äú²»ÄÜÔÚÄäÃû°åÊ¹ÓÃ¼ÄÐÅ¸øÔ­×÷Õß¹¦ÄÜ¡£");
        else if (!mail_file(filepath, mailid, postfile.title))
            prints("ÐÅ¼þÒÑ³É¹¦µØ¼Ä¸øÔ­×÷Õß %s", mailid);
        else
            prints("ÐÅ¼þÓÊ¼ÄÊ§°Ü£¬%s ÎÞ·¨ÊÕÐÅ¡£", mailid);
        mailtoauther = 0;
        pressanykey();
    }
    if (!is_module)
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
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                          usernum);
    }
    return FULLUPDATE;
}

int IsTheFileOwner(struct fileheader *fileinfo)
{
    char buf[512];
    int posttime;

    if (fileinfo->owner[0] == '-' || strstr(fileinfo->owner, "."))
        return 0;
    if (strcmp(currentuser.userid, fileinfo->owner))
        return 0;
    strcpy(buf, &(fileinfo->filename[2]));
    buf[strlen(buf) - 2] = '\0';
    posttime = atoi(buf);
    if (posttime < currentuser.firstlogin)
        return 0;
    return 1;
}

int change_title(char *fname, char *title)
{
    FILE *fp, *out;
    char buf[256], outname[STRLEN];
    int newtitle = 0;

    if ((fp = fopen(fname, "r")) == NULL)
        return 0;

    sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid,
            uinfo.pid);
    if ((out = fopen(outname, "w")) == NULL)
        return 0;
    while ((fgets(buf, 256, fp)) != NULL)
    {
        if (!strncmp(buf, "±ê  Ìâ: ", 8) && newtitle == 0)
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
    char buf[512];
    char *t;
    extern char currmaildir[STRLEN];
    int isBM = 0;

    if (!in_mail)
    {
        isBM = chk_currBM(currBM) ? YEA : NA;

        if (digestmode != NA && isBM == NA)
            return DONOTHING;

        if (isBM == NA)
        {
            struct shortfile *bp;

            if (!IsTheFileOwner(fileinfo))
                return DONOTHING;
            bp = getbcache(currboard);

            if (((bp->flag & ANONY_FLAG)
                 || (bp->flag & POSITIVE_ANONY_FLAG))
                && !strcmp(fileinfo->owner, currboard))
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
        sprintf(genbuf, "%s/%s/%s", buf, getfolderstr(fileinfo->filename),
                fileinfo->filename);

    if (vedit(genbuf, NA, NA) == -1)
        return FULLUPDATE;
#ifdef MARK_X_FLAG

    if (markXflag)
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
        sprintf(genbuf, "edited post '%s' on %s", fileinfo->title,
                currboard);
        report(genbuf);
    }
    return FULLUPDATE;
}

void getnam(char *direct, int num, char *id)
{
    FILE *fp;
    int size;
    struct fileheader ff;
    size = sizeof(struct fileheader);
    fp = fopen(direct, "r");
    if (fp == NULL)
        return;
    fseek(fp, (num - 1) * size, SEEK_SET);
    strcpy(id, "none.");
    if (fread(&ff, size, 1, fp))
        strcpy(id, ff.filename);
    fclose(fp);
}

int edit_title(int ent, struct fileheader *fileinfo, char *direct)
{

    FILE *fp;
    char *temp;

    struct stat st;
    char buf[STRLEN];
    char buftitle[STRLEN];
    int top_ent = 0;
    char dirpath[STRLEN];
    char toppath[STRLEN];
    struct stat dirst;
    int real_file_num = 0;
    int top_mode = 0;

    char id1[20], id2[20];

    getnam(direct, ent, id1);

    if (!in_mail && !chk_currBM(currBM))
    {
        struct shortfile *bp;

        if (digestmode != NA)
            return DONOTHING;
        if (!IsTheFileOwner(fileinfo))
            return DONOTHING;
        bp = getbcache(currboard);
        if (((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG))
            && !strcmp(fileinfo->owner, currboard))
            return DONOTHING;
    }

    if (stat(direct, &st) == -1)
        return DONOTHING;

    if ((int) (st.st_size / sizeof(struct fileheader)) < ent)
    {

        if (!in_mail && !chk_currBM(currBM))
            return DONOTHING;

        top_mode = 1;
        setbfile(dirpath, currboard, DOT_DIR);
        stat(dirpath, &dirst);
        real_file_num = dirst.st_size / sizeof(struct fileheader);
        top_ent = ent - real_file_num;
        move(t_lines - 1, 0);
        clrtoeol();
        if (askyn("ÄãÈ·ÈÏÒªÐÞ¸ÄÖÃµ×ÎÄÕÂ±êÌâÂð", NA, NA) == NA)
            return PARTUPDATE;
    }

    strcpy(buf, fileinfo->title);
    getdata(t_lines - 1, 0, "ÐÂÎÄÕÂ±êÌâ: ", buf, 50, DOECHO, NA);
    if (!strcmp(buf, fileinfo->title))
        return PARTUPDATE;
    check_title(buf);
    if (buf[0] != '\0')
    {
        char tmp[STRLEN * 2], *t;

        getnam(direct, ent, id2);
        if (strcmp(id1, id2))
            return PARTUPDATE;
        strcpy(fileinfo->title, buf);
        strcpy(tmp, direct);
        if ((t = strrchr(tmp, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);

        if (top_mode == 0)
        {
            substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        }
        else
        {
            setbfile(toppath, currboard, TOPFILE_DIR);
            substitute_record(toppath, fileinfo, sizeof(*fileinfo),
                              top_ent);
        }

        sprintf(buftitle, "boards/%s/%s/%s", currboard,
                getfolderstr(fileinfo->filename), fileinfo->filename);
        change_title(buftitle, buf);
        if ((fp = fopen(buftitle, "a+")) == NULL)
            return 0;
        getdatestring(time(0), NA);
        
        if (strstr(datestring, "ÐÇÆÚ"))
        {
            temp = &datestring[strlen(datestring) - 1];
            while (*temp && *temp != ' ')
                temp--;
            *temp = '\0';
        }    
        fprintf(fp,
                "\n[1;36m¡ù ÐÞ¸Ä:¡¤%s ÓÚ %s ÐÞ¸Ä±êÌâ¡¤[FROM: %-.46s][m\n",
                currentuser.userid, datestring, currentuser.lasthost);
        fclose(fp);

    }
    return PARTUPDATE;
}

int underline_post(int ent, struct fileheader *fileinfo, char *direct)
{

    if (!chk_currBM(currBM) && !IsTheFileOwner(fileinfo))
    {
        return DONOTHING;
    }
    if (chk_currBM(currBM))
    {
	if (fileinfo->accessed[1] & FILE_NOREPLY2 || fileinfo->accessed[0] & FILE_NOREPLY)
	{
	    fileinfo->accessed[1] &= ~FILE_NOREPLY2;
	    fileinfo->accessed[0] &= ~FILE_NOREPLY;
	    if (!IsTheFileOwner(fileinfo))
	    {
	    	log_bm_ops(currboard, "°åÃæ", "!x", fileinfo->title);
	    }
	}
	else if (IsTheFileOwner(fileinfo))
	{
	    fileinfo->accessed[0] |= FILE_NOREPLY;
	}
	else
	{
	    fileinfo->accessed[1] |= FILE_NOREPLY2;
	    log_bm_ops(currboard, "°åÃæ", "x", fileinfo->title);
	}
    }
    else if (!(fileinfo->accessed[1] & FILE_NOREPLY2))
    {
	if (fileinfo->accessed[0] & FILE_NOREPLY)
	{
	    fileinfo->accessed[0] &= ~FILE_NOREPLY;
	}
	else
	{
	    fileinfo->accessed[0] |= FILE_NOREPLY;
	}
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);

    return PARTUPDATE;
}

int show_file_info(int ent, struct fileheader *fileinfo, char *direct)
{
    char url[512] = { 0 };
    char filepath[STRLEN] = { 0 };
    char filename[STRLEN];

    struct stat file_data;

//  if (digestmode != 0)
//      return DONOTHING;
    sprintf(filepath, BBSHOME "/boards/%s/%s/%s", currboard,
            getfolderstr(fileinfo->filename), fileinfo->filename);
    if (fileinfo->filename[0] == 'T')
    {
        sprintf(url, "http://" BBSHOST "/bbstopcon?board=%s&file=%s",
                currboard, fileinfo->filename);
    }
    else
    {
        sprintf(url, "http://" BBSHOST "/bbscon?board=%s&file=%s",
                currboard, fileinfo->filename);
    }
    if (stat(filepath, &file_data) < 0)
    {
        clear();
        move(10, 30);
        prints("¶Ô²»Æð£¬µ±Ç°ÎÄÕÂ²»´æÔÚ£¡\n", filepath);
        pressanykey();
        clear();
        return FULLUPDATE;
    }

    sprintf(filename, "%s", fileinfo->filename);
    filename[0] = 'M';

    clear();
    move(0, 0);
    prints("ÒûË®Ë¼Ô´BBSÎÄÕÂÐÅÏ¢\n\n");
    prints("ÎÄÕÂ±êÌâ: %s\n\n", fileinfo->title);
    prints("ÎÄÕÂ×÷Õß: %s\n\n", fileinfo->owner);
    prints("ÎÄÕÂ´óÐ¡: %d ×Ö½Ú\n\n", file_data.st_size);
    prints("ÎÄÕÂÁ´½Ó: \n%s\n\n", url);
    prints("×Ô¶¨Òå³¬Á´½Ó£º\n[[Post:%s,%s|%s]]\n\n", currboard, filename, fileinfo->title);
    pressanykey();
    clear();
    return FULLUPDATE;
}

int mark_post(int ent, struct fileheader *fileinfo, char *direct)
{

    if (!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[0] & FILE_MARKED)
    {
        fileinfo->accessed[0] &= ~FILE_MARKED;
        log_bm_ops(currboard, "°åÃæ", "!m", fileinfo->title);
    }
    else
    {
        fileinfo->accessed[0] |= FILE_MARKED;
        fileinfo->accessed[0] &= ~FILE_DELETED;
        log_bm_ops(currboard, "°åÃæ", "m", fileinfo->title);
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);

    return PARTUPDATE;
}

int recommend_post(int ent, struct fileheader *fileinfo, char *direct)
{
#ifdef LOGHANDLER
    char query_string[4096], title_ok[256];
    int sqllen = 0;
#endif

    if (!HAS_PERM(PERM_SPECIAL7))
    {
        return DONOTHING;
    }
    if (fileinfo->accessed[1] & FILE_RECOMMENDED)
    {
        clear();

        prints("ÄúÑ¡ÔñÈ¡Ïûµ¼¶ÁµÄÎÄÕÂÊÇ: [\033[1;33m%s\033[m]\n", fileinfo->title);
        if (askyn("ÒÑÔÚ"RECOMMEND_DIGEST_BOARDNAME"°å×Ô¶¯·¢²¼µÄ¸±±¾£¬²»»áÒòÈ¡Ïûµ¼¶Á¶ø×Ô¶¯É¾³ý£¬¼ÌÐøÂð", NA, NA) == NA)
            return FULLUPDATE;

        fileinfo->accessed[1] &= ~FILE_RECOMMENDED;
#ifdef LOGHANDLER

        sqllen =
            snprintf(query_string, sizeof(query_string),
                     LOGHANDLER_SQL
                     "DELETE FROM `recommend` WHERE `board`='%s' AND `reid`='%d' AND filename = '%s'\n",
                     currboard, fileinfo->reid, fileinfo->filename);
        loghander_write(query_string, sqllen);
#endif

        log_bm_ops(currboard, "°åÃæ", "!+", fileinfo->title);
    }
    else
    {
        char dbname[STRLEN];

        if (digestmode || !strcmp(currboard, RECOMMEND_DIGEST_BOARDNAME) || !strcmp(fileinfo->owner, BBSID))
            return DONOTHING;
#ifndef ANONYM_RECOMMEND
        if (strncmp(fileinfo->owner, "Anony.", 6) == 0)
            return DONOTHING;
        else
            strcpy(quote_user, fileinfo->owner);
#else
        strcpy(quote_user, (strncmp(fileinfo->owner, "Anony.", 6)) ? fileinfo->owner : ANONYRECID);
#endif

        sprintf(genbuf, "boards/%s/%s/%s", currboard, getfolderstr(fileinfo->filename), fileinfo->filename);
        strncpy(quote_file, genbuf, sizeof(quote_file));
        quote_file[sizeof(quote_file) - 1] = '\0';
        strcpy(quote_title, fileinfo->title);

        clear();

        prints("ÄúÑ¡ÔñÍÆ¼öµ¼¶ÁµÄÎÄÕÂÊÇ: [\033[1;33m%s\033[m]\n", quote_title);
        if (askyn("±¾ÎÄ½«»áÔÚ"RECOMMEND_DIGEST_BOARDNAME"°å×Ô¶¯·¢²¼¸±±¾£¬È¡Ïûµ¼¶Á²¢²»×Ô¶¯É¾³ý¸±±¾£¬¼ÌÐøÂð", NA, NA) == NA)
            return FULLUPDATE;

        strcpy(quote_board, currboard);
        strcpy(dbname, currboard);
        strcpy(currboard, RECOMMEND_DIGEST_BOARDNAME);

        fileinfo->accessed[1] |= FILE_RECOMMENDED;
        fileinfo->accessed[0] &= ~FILE_DELETED;

        if (post_cross('L', 3) == -1)
            prints("\nÎÄÕÂ: \'%s\' ÍÆ¼öµ¼¶Á³ö´í£º\nÎÄÕÂ¸±±¾ÎÞ·¨·¢²¼µ½ %s °å\n", quote_title, RECOMMEND_DIGEST_BOARDNAME);
        else
            prints("\nÎÄÕÂ: \'%s\' ÒÑ¾­ÍÆ¼öµ¼¶Á£¡\nÎÄÕÂ¸±±¾ÒÑ¾­·¢²¼µ½ %s °å\n", quote_title, RECOMMEND_DIGEST_BOARDNAME);
        strcpy(currboard, dbname);

#ifdef LOGHANDLER

        mysql_escape_string(title_ok, fileinfo->title,
                            strlen(fileinfo->title));
        sqllen =
            snprintf(query_string, sizeof(query_string),
                     LOGHANDLER_SQL
                     "INSERT INTO `recommend` ( `id` , `board` , `user_id` , `reid` , `title` ,`filename`,  `height` ) VALUES ('', '%s', '%s/%s', '%d', '%s', '%s', '0')\n",
                     currboard, fileinfo->owner, currentuser.userid,
                     fileinfo->reid, title_ok, fileinfo->filename);
        loghander_write(query_string, sqllen);
#endif

        log_bm_ops(currboard, "°åÃæ", "+", fileinfo->title);
        pressreturn();
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    sleep(1);

    return FULLUPDATE;
}

int del_range(int ent, struct fileheader *fileinfo, char *direct)
{
    char num[8];
    int inum1, inum2;
    int realdel;

    realdel = 0;

    if (uinfo.mode == READING)
    {
        if (!chk_currBM(currBM))
        {
            return DONOTHING;
        }
    }

    if (digestmode > 1)
        return DONOTHING;

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
            char content[1024];
            char returnpath[STRLEN];
            struct stat dirst;
            int return_file_num = 0;

            sprintf(genbuf, "Range delete %d-%d on %s", inum1, inum2,
                    currboard);
            securityreport(genbuf);
            setbfile(returnpath, currboard, RETURN_DIR);
            if ((stat(returnpath, &dirst)) != -1)
            {
                return_file_num =
                    dirst.st_size / (sizeof(struct fileheader));
            }

            sprintf(content, "¹²%dÆª, »ØÊÕÕ¾:%d-%d", realdel,
                    return_file_num - realdel + 1, return_file_num);
            log_bm_ops(currboard, "°åÃæ", "D", content);
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
int UndeleteArticle(int ent, struct fileheader *fileinfo, char *direct)
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

    if (strcmp(currboard, "deleted") && strcmp(currboard, "junk"))
    {
#ifdef KEEP_DELETED_HEADER
        if (fileinfo->owner[0] != '-')
            return DONOTHING;
        kind = 1;
#else

        return DONOTHING;
#endif

    }
    sprintf(buf, "boards/%s/%s/%s", currboard,
            getfolderstr(fileinfo->filename), fileinfo->filename);
    fp = fopen(buf, "r");
    if (!fp)
        return DONOTHING;

    strcpy(UTitle, fileinfo->title);
    if ((p = strrchr(UTitle, '-')))
    {
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i--)
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
            i++;
            strcpy(UBoard, p + 9);
            if ((p = strchr(UBoard, ' ')))
                *p = 0;
            else if ((p = strchr(UBoard, '\n')))
                *p = 0;
        }
        else if (strstr(buf, "±ê  Ìâ: "))
        {
            i++;
            strcpy(UTitle, buf + 8);
            if ((p = strchr(UTitle, '\n')))
                *p = 0;
        }
    }
    fclose(fp);

#ifdef KEEP_DELETED_HEADER

    if (kind)
    {
        strncpy(fileinfo->owner, &(fileinfo->owner[1]), IDLEN + 2);
        mystrlcpy(fileinfo->title, UTitle, STRLEN);
        fileinfo->filename[STRLEN - 1] = 'L';
        fileinfo->filename[STRLEN - 2] = 'L';
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        if (!strcmp(fileinfo->owner, currentuser.userid))
        {
            set_safe_record();
            if (!junkboard())
                currentuser.numposts++;
            substitute_record(PASSFILE, &currentuser,
                              sizeof(currentuser), usernum);
        }
        else if (!strstr(fileinfo->owner, ".") && BMDEL_DECREASE)
        {
            int id;

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
        if (SR_BMDELFLAG != YEA)
        {
            clear();
            move(2, 0);
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
            currboard, getfolderstr(fileinfo->filename),
            fileinfo->filename, UBoard, getfolderstr(UFile.filename),
            UFile.filename);
    system(buf);

    sprintf(buf, "boards/%s/.DIR", UBoard);
    append_record(buf, &UFile, sizeof(UFile));

    fileinfo->accessed[0] |= FILE_FORWARDED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    sprintf(buf, "undeleted %s's ¡°%s¡± on %s", UFile.owner, UFile.title,
            UBoard);
    report(buf);
    if (SR_BMDELFLAG != YEA)
    {
        clear();
        move(2, 0);
        prints("'%s' ÒÑ»Ö¸´µ½ %s °å \n", UFile.title, UBoard);
        pressreturn();
    }
    return FULLUPDATE;
}

#ifdef LOGHANDLER

static int loghandler_fd = -1;

static int loghandler_timeindex = -1;
static int loghandler_open_new(void)
{
    int randomindex, timeindex;
    char path[1024];

    randomindex = getpid() % LOGHANDLER_RANDOMINDEX_NUM;
    timeindex =
        time(NULL) / LOGHANDLER_ROTATING_TIME % LOGHANDLER_TIMEINDEX_NUM;
    snprintf(path, 1024, LOGHANDLER_PATH_FORMT, timeindex, randomindex);
    if (loghandler_fd > -1)
    {
        close(loghandler_fd);
    }
    loghandler_fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (loghandler_fd > -1)
    {
        loghandler_timeindex = timeindex;
    }
    return loghandler_fd;
}
int loghandler_get_fd(void)
{
    int timeindex;

    if (loghandler_fd < 0)
    {
        return loghandler_open_new();
    }
    timeindex =
        time(NULL) / LOGHANDLER_ROTATING_TIME % LOGHANDLER_TIMEINDEX_NUM;
    if (timeindex != loghandler_timeindex)
    {
        return loghandler_open_new();
    }
    return loghandler_fd;
}
int loghander_write(char *line, int len)
{
    int fd;

    if ('\n' != line[len - 1])
        return -2;
    fd = loghandler_get_fd();
    if (fd < 0)
        return -1;
    else
    {
        return write(fd, line, len);
    }
}
int loghandler_writev(int linenum, ...)
{
    va_list ap;
    struct iovec *pvec;
    int i, ret = 0, fd, len;
    char *line;

    fd = loghandler_get_fd();
    if (fd < 0)
        return -1;
    pvec = (struct iovec *) malloc(linenum * sizeof(struct iovec));
    if (!pvec)
        return -1;
    va_start(ap, linenum);
    for (i = 0; i < linenum; ++i)
    {
        line = va_arg(ap, char *);
        len = va_arg(ap, int);

        if (line[len - 1] != '\n')
        {
            ret = -1;
            break;
        }
        pvec[i].iov_base = line;
        pvec[i].iov_len = len;
    }
    va_end(ap);
    if (ret < 0)
    {
        free(pvec);
        return ret;
    }
    ret = writev(fd, pvec, linenum);
    free(pvec);
    return ret;
}

int mysql_escape_string(char *dst, char *src, int len)
{
    int i;
    char *tmpdst = dst;

    for (i = 0; i < len; i++)
    {
        switch (src[i])
        {
        case '\n':
            *tmpdst++ = '\\';
            *tmpdst++ = 'n';
            break;
        case '\\':
            *tmpdst++ = '\\';
            *tmpdst++ = '\\';
            break;
        case '\r':
            *tmpdst++ = '\\';
            *tmpdst++ = 'r';
            break;
        case '\0':
            *tmpdst++ = '\\';
            *tmpdst++ = '0';
            break;
        case '\'':
            *tmpdst++ = '\\';
            *tmpdst++ = '\'';
            break;
        case '\"':
            *tmpdst++ = '\\';
            *tmpdst++ = '\"';
            break;
        case '\b':
            *tmpdst++ = '\\';
            *tmpdst++ = 'b';
            break;
        case '\t':
            *tmpdst++ = '\\';
            *tmpdst++ = 't';
            break;
        case 0x1A:
            *tmpdst++ = '\\';
            *tmpdst++ = 'Z';
            break;
        case '%':
            *tmpdst++ = '\\';
            *tmpdst++ = '%';
            break;
        case '_':
            *tmpdst++ = '\\';
            *tmpdst++ = '_';
            break;
        default:
            *tmpdst++ = src[i];
            break;
        }
    }
    *tmpdst++ = '\0';
    return tmpdst - dst - 1;
}
int del_log2db(char *user_id, char *filename, char *board_name)
{
    char query_string[4096];
    int len;

    len =
        snprintf(query_string, sizeof(query_string),
                 LOGHANDLER_SQL
                 "insert into junk (del_user,time,filename,board) values ('%s',FROM_UNIXTIME(%ld),'%s','%s')\n",
                 user_id, time(NULL), filename, board_name);
    return loghander_write(query_string, len);
}
#endif

int del_pushsure(int ent, struct fileheader *fileinfo, char *direct)
{
    char pfilename[80], filepath[80];

    if (!chk_currBM(currBM))
    {
        return DONOTHING;
    }
    snprintf(pfilename, 80, "%s", fileinfo->filename);
    pfilename[0] = 'P';
    snprintf(filepath, 80, "boards/%s/%s/%s", currboard,
             getfolderstr(pfilename), pfilename);
    if (remove(filepath) != 0)
        return -1;
    fileinfo->pnum = 0;
    substitute_record(direct, fileinfo, sizeof(struct fileheader), ent);
    return 0;
}

int del_post(int ent, struct fileheader *fileinfo, char *direct)
{
    char buf[512];
    char usrid[STRLEN];
    char *t;
    int owned = 0, fail, IScurrent = 0;
    int posttime;
    int i;

    if (fileinfo->accessed[0] & (FILE_MARKED | FILE_DIGEST | FILE_TOP1))
        return DONOTHING;

    if (digestmode > 1
        || ((!strcmp(currboard, "deleted") || !strcmp(currboard, "junk"))
            && digestmode != 9))
        return DONOTHING;
#ifdef SPECIALID

    if (!strcmp(currentuser.userid, SPECIALID1)
        || !strcmp(currentuser.userid, SPECIALID2))
        return DONOTHING;
#endif

    if (fileinfo->owner[0] == '-')
        return DONOTHING;
    strcpy(usrid, fileinfo->owner);
    if (!strstr(usrid, "."))
        IScurrent = !strcmp(usrid, currentuser.userid);

    strcpy(buf, &(fileinfo->filename[2]));
    buf[strlen(buf) - 2] = '\0';
    posttime = atoi(buf);
    if (!IScurrent)
    {
        owned = getuser(usrid);
        if (owned != 0)
        {
            if (posttime < lookupuser.firstlogin)
                owned = 0;
        }
    }
    else
        owned = posttime > currentuser.firstlogin;

    if (!chk_currBM(currBM))
    {
        struct shortfile *bp;

        if (!(owned && IScurrent))
            return DONOTHING;
        bp = getbcache(currboard);
        if (((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG))
            && !strcmp(fileinfo->owner, currboard))
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
    if (fileinfo->pnum > 0 && fileinfo->pnum < 10000)
        del_pushsure(ent, fileinfo, direct);
    strncpy(currfile, fileinfo->filename, STRLEN);
#ifndef KEEP_DELETED_HEADER

    fail =
        delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
#else

    fail = update_file(direct, sizeof(struct fileheader), ent, cmpfilename,
                       cpyfilename);
#endif

    if (!fail)
    {

        if (digestmode == 0)
        {
            fileinfo->accessed[0] = FILE_DELETED;
            setbfile(buf, currboard, RETURN_DIR);
            sprintf(genbuf, "%-32.32s - %s", fileinfo->title,
                    currentuser.userid);
            if (strlen(fileinfo->title) > 32)
            {
                for (i = strlen(genbuf); i < 60; i++)
                    genbuf[i] = ' ';
                i = 0;
                while (fileinfo->title[32 + i] != '\0')
                {
                    genbuf[60 + i] = fileinfo->title[32 + i];
                    i++;
                }

                genbuf[60 + i] = '\0';
            }
            mystrlcpy(fileinfo->title, genbuf, STRLEN);

            append_record(buf, fileinfo, sizeof(struct fileheader));
#ifdef LOGHANDLER
            del_log2db(currentuser.userid, fileinfo->filename, currboard);
#endif
        }
        else
        {
            strcpy(buf, direct);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(genbuf, "Del '%s' on '%s'", fileinfo->title,
                    currboard);
            report(genbuf);
            cancelpost(currboard, currentuser.userid, fileinfo, owned
                       && IScurrent);
            sprintf(genbuf, "%s/%s/%s", buf,
                    getfolderstr(fileinfo->filename), fileinfo->filename);
            unlink(genbuf);
        }

        strcpy(buf, direct);
        if ((t = strrchr(buf, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "Del '%s' on '%s'", fileinfo->title, currboard);
        report(genbuf);

        if (!junkboard() && !digestmode)
        {
            if (owned && IScurrent)
            {
                set_safe_record();
                if (currentuser.numposts > 0)
                    currentuser.numposts--;
                substitute_record(PASSFILE, &currentuser,
                                  sizeof(currentuser), usernum);
            }
            else if (owned && BMDEL_DECREASE)
            {
                if (lookupuser.numposts > 0)
                    lookupuser.numposts--;
                substitute_record(PASSFILE, &lookupuser,
                                  sizeof(struct userec), owned);
            }
        }

        return DIRCHANGED;

    }
    else if (SR_BMDELFLAG)
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

    if (uinfo.mode != READING&&uinfo.mode != FAVOURITE)
        return DONOTHING;
    if ((fd = open(direct, O_RDONLY, 0)) == -1)
        return DONOTHING;
    for (i = 0; clearall || i < ent; i++)
    {
        if (read(fd, &f_info, sizeof(f_info)) != sizeof(f_info))
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

int full_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return Save_post(ent, fileinfo, direct, 1);
}

int part_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
    return Save_post(ent, fileinfo, direct, 0);
}

int Save_post(int ent, struct fileheader *fileinfo, char *direct, int full)
{
    if (!chk_currBM(currBM))
        return DONOTHING;
    return (a_Save("0Announce", currboard, fileinfo, NA, full));
}

int do_module()
{
    char ans[3];
    struct shortfile *pb = getbcache(currboard);

    if (!chk_currBM(currBM))
        return DONOTHING;
    if (!(pb->flag2[0] & MODULE_FLAG))
    {
        move(t_lines - 1, 0);
        prints("±¾°åÏÖÔÚ²»Ö§³ÖÄ£°å");
        pressanykey();
    }
    else
    {
        struct module_index module[3];

        while (1)
        {
            int count = 0;

            clear();
            count = showmodulename(module);
            move(0, 0);
            prints("±à¼­·¢ÎÄÄ£°å");
            write_module = 1;
            if (0 < count && count < 3)
            {
                getdata(1, 0,
                        "(A)Ôö¼Ó (D)É¾³ý (C)¸Ä±ä (R)È«²¿É¾³ý (E)Àë¿ª[E]: ",
                        ans, 2, DOECHO, YEA);
            }
            else if (count == 3)
            {
                getdata(1, 0, "(D)É¾³ý (C)¸Ä±ä (R)È«²¿É¾³ý (E)Àë¿ª[E]: ",
                        ans, 2, DOECHO, YEA);
            }
            else
            {
                getdata(1, 0, "(A)Ôö¼Ó (E)Àë¿ª [E]: ",
                        ans, 2, DOECHO, YEA);
            }
            if ((ans[0] == 'A' || ans[0] == 'a') && count != 3)
            {
                add_module(module, count);
            }
            else if ((ans[0] == 'D' || ans[0] == 'd') && count != 0)
            {
                del_module(module, count);
            }
            else if ((ans[0] == 'R' || ans[0] == 'r') && count != 0)
            {
                del_all_module(module, count);
            }
            else if ((ans[0] == 'C' || ans[0] == 'c') && count != 0)
            {
                change_module(module, count);
            }
            else
                break;
        }
    }
    write_module = 0;
    clear();
    return FULLUPDATE;

}

int add_module(struct module_index *modulename, int count)
{
    char name[NAMELEN];

    getdata(2, 0, "ÊäÈë±êÌâ: ", name, NAMELEN - 4, DOECHO, YEA);
    if (!check_modulename(count, modulename, name) || strlen(name) == 0)
    {
        move(2, 0);
        clrtoeol();
        prints("±êÌâÖØ¸´»òÕßÃ»ÓÐÊäÈë");
        pressanykey();
        return NA;
    }
    else
    {
        FILE *fp;
        char buf[STRLEN];

        setmodulefile(buf, currboard, "modulename");
        clear();
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        strcpy(module_name, name);
        if (post_article(currboard, (char *) NULL, NULL, 1) != -1)
        {
            if (writeinto_modulefile == 1)
            {
                struct module_index moduleadded;

                strcat(name, "_M");
                strcpy(moduleadded.filename, name);
                setmodulefile(buf, currboard, "modulename");
                fp = fopen(buf, "a");
                fwrite(&moduleadded, sizeof(struct module_index), 1, fp);
                fclose(fp);
                move(1, 0);
                if (count == 2)
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
    *module_name = '\0';
    return YEA;
}

int del_module(struct module_index *modulename, int count)
{
    char ans[3];
    char buf[STRLEN];
    FILE *fp;
    int i;
    int j;

    setmodulefile(buf, currboard, "modulename");
    getdata(2, 0, "ÊäÈëÒªÉ¾³ýµÄÐòºÅ: ", ans, 2, DOECHO, YEA);
    if (ans[0] == 'a' || ans[0] == 'A')
    {
        j = 0;
    }
    else if ((ans[0] == 'b' || ans[0] == 'B') && count > 1)
    {
        j = 1;
    }
    else if ((ans[0] == 'c' || ans[0] == 'C') && count == 3)
    {
        j = 2;
    }
    else
    {
        return NA;
    }
    i = 0;
    fp = fopen(buf, "w");
    while (i < count)
    {
        strcat(modulename[i].filename, "_M");
        if (i != j)
        {
            fwrite(&modulename[i], sizeof(struct module_index), 1, fp);
        }
        i++;
    }
    fclose(fp);
    setmodulefile(buf, currboard, modulename[j].filename);
    if (unlink(buf) == -1)
    {
    }
    return YEA;
}

int del_all_module(struct module_index *modulename, int count)
{
    char buf[NAMELEN];
    FILE *fp;

    if (askyn("È·ÈÏÈ«²¿É¾³ýÂð?", NA, NA) == NA)
    {
        return NA;
    }
    while (count > 0)
    {
        strcat(modulename[count - 1].filename, "_M");
        setmodulefile(buf, currboard, modulename[count - 1].filename);
        unlink(buf);
        count--;
    }
    setmodulefile(buf, currboard, "modulename");
    fp = fopen(buf, "w");
    fclose(fp);
    return YEA;
}

int change_module(struct module_index *modulename, int count)
{
    char ans[3];
    char buf[STRLEN];
    int i;

    setmodulefile(buf, currboard, "modulename");
    getdata(2, 0, "ÊäÈëÒª¸Ä±äµÄÐòºÅ: ", ans, 2, DOECHO, YEA);
    if (strlen(ans) == 0)
    {
        return NA;
    }
    i = ans[0] - 'a';
    if (i < count && i >= 0)
    {
        time_t now = time(0);
        int fp;
        FILE *file1;
        FILE *file2;
        char filepath[STRLEN];
        char fname[STRLEN];
        char fpath[STRLEN];
        char buf[256];

        sprintf(fname, "%ld", now);
        setmodulefile(fpath, currboard, fname);
        while ((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
        {
            now++;
            sprintf(fname, "%ld", now);
            setmodulefile(fpath, currboard, fname);
        }
        fchmod(fp, 0644);
        close(fp);
        file1 = fopen(fpath, "w");

        strcpy(module_name, modulename[i].filename);
        strcat(modulename[i].filename, "_M");
        setmodulefile(filepath, currboard, modulename[i].filename);
        file2 = fopen(filepath, "r");
        while (fgets(buf, 256, file2) != NULL)
        {
            fputs(buf, file1);
        }
        fclose(file1);
        fclose(file2);
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        post_article(currboard, (char *) NULL, NULL, 1);
        if (writeinto_modulefile == 0)
        {
            rename(fpath, filepath);
        }
        else
        {
            unlink(fpath);
        }
    }

    *module_name = '\0';
    return YEA;

}

int check_modulename(int num, struct module_index module[3], char *buf)
{
    int i = 0;

    while (i < num)
    {
        if (strcmp(module[i].filename, buf) == 0)
        {
            return NA;
        }
        i++;
    }
    return YEA;
}

// calculate the mutiline length
int mutilinelength(char *src, int linenum){
    int i, j, chinese;
    int len = strlen(src);
    int line = 0;
    char tmp;
    
    i = j = 0;
    if (src[len] == '\n'){
        src[len] = '\0';
        len--;
    }    
    while (j < len){
        j = i + linenum;
        if (j > len) 
           j = len;
        chinese = j - 1;
        while ((chinese >= i) && (src[chinese] < 0))
              chinese--;
        if ((j - chinese - 1)& 1)
            j--;
        tmp = src[j];
        src[j] = '\0';
        line++;
        src[j] = tmp;
        i = j;
    }    
    return line;
}    
void show_announce_paths()
{
    FILE *fn;
    int i = 0;
    int y, x;
    int line;
    int height = 0;
    char ap[4][1024];
    char fname[STRLEN];


    while (i < 4)
    {
        if (0 == i)
        {
            sprintf(fname, ".fpp");
            sethomefile(genbuf, currentuser.userid, fname);
        }
        else
        {
            sprintf(fname, ".fpp%d", i);
            sethomefile(genbuf, currentuser.userid, fname);
        }
        fn = fopen(genbuf, "r");
        
        if (fn)
        {
            char tmpath[1000];

            if (fgets(tmpath, 1000, fn) == NULL)
            {
                sprintf(ap[i], "µÚ%dË¿Â·: ", i + 1);
            }
            else
            {
                sprintf(ap[i], "µÚ%dË¿Â·: %s", i + 1, tmpath);
            }
            fclose(fn);
        }
        else
        {
            sprintf(ap[i], "µÚ%dË¿Â·: %s", i + 1, " ÉÐÎ´ÉèÖÃ");
        }
        i++;
    }

    i = 0;
    while ( i < 4 ){
        height += mutilinelength(ap[i],80);     
        i++;
    }
    
    getyx(&y,&x);

    if ( y < t_lines - height - 3 )
            line = t_lines - height - 3;
    else
            line = 3;

    move(line++, 0);

    prints("\033[1;33;44m%-80s\033[m","Ë¿Â·ÉèÖÃÇé¿öÈçÏÂËùÊ¾:");

    move(line,0);
    
    i = 0;
    while( i < 4 ){
        mutilineprints(ap[i], 80, &line);
        i++;
    }
    prints("\033[1;33;44m%-80s\033[m","");

}

int Import_post(int ent, struct fileheader *fileinfo, char *direct)
{

    int opt;
    char msg[2];
    char fname[STRLEN];

    if (!chk_currBM(currBM))
        return DONOTHING;

    show_announce_paths();

    getdata(t_lines - 1, 0, "ÇëÊäÈë²Ù×÷Ë¿Â·±àºÅ 1-4(Ä¬ÈÏÎªµÚÒ»Ë¿Â·):", msg,
            2, DOECHO, YEA);

    if (msg[0] == '\n' || msg[0] == '\0')
        msg[0] = '1';
    opt = atoi(msg) - 1;
    if (!isdigit(msg[0]) || opt < 0 || opt > 3)
    {
        presskeyfor("ÊäÈë´íÎó, Çë°´<Enter>¼ÌÐø...", t_lines - 1);
        return FULLUPDATE;
    }
    else
    {
        FILE *fn;
        char annpath[512];

        if (opt == 0)
        {
            sprintf(fname, ".announcepath");
        }
        else
        {
            sprintf(fname, ".announcepath%d", opt);
        }

        if (!HAS_PERM(PERM_BOARDS))
            return DONOTHING;

	if (!chk_ap(opt)){
	    presskeyfor("Äú²»ÔÙÓµÓÐ¸ÃË¿Â·µÄÈ¨ÏÞ£¬ÇëÖØÐÂÉè¶¨Ë¿Â·", t_lines - 1);
	    return 1;
	}
        if (fileinfo->accessed[0] & FILE_VISIT)
        {
            if (askyn("ÎÄÕÂÔø¾­·ÅÈëÁË¾«»ªÇø, ÏÖÔÚ»¹ÒªÔÙ·ÅÈëÂð", YEA, YEA)
                == NA)
                return 1;
        }
        else
        {
            fileinfo->accessed[0] |= FILE_VISIT;
            substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        }
        sethomefile(annpath, currentuser.userid, fname);
        if ((fn = fopen(annpath, "r")) == NULL)
        {
            presskeyfor("¶Ô²»Æð, ÄãÃ»ÓÐÉè¶¨Ë¿Â·. ÇëÏÈÓÃ f Éè¶¨Ë¿Â·.",
                        t_lines - 1);
            return 1;
        }
        fscanf(fn, "%s", annpath);
        fclose(fn);
        if (!dashd(annpath))
        {
            presskeyfor("ÄãÉè¶¨µÄË¿Â·ÒÑ¶ªÊ§, ÇëÖØÐÂÓÃ f Éè¶¨.",
                        t_lines - 1);
            return 1;
        }

        return (a_Import("0Announce", currboard, fileinfo, annpath, NA));
    }
}

int check_notespasswd(void)
{
    FILE *pass;
    char passbuf[20], prepass[STRLEN];
    char buf[STRLEN];

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

int show_b_secnote(void)
{
    char buf[256];

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

int show_b_note(void)
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

int into_announce(void)
{
    char found[STRLEN];

    if (currboard[0] == '\0')
        return DONOTHING;
    if (a_menusearch("0Announce", currboard, found))
    {
        strcpy(jhpath, currboard);
        strcat(jhpath, "¾«»ªÇø");
        a_menu("", found, "", 
               (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_OBOARDS) ? PERM_BOARDS : 0), 0, 0);
        return MODECHANGED;
    }
    return DONOTHING;
}
int upload_manage(void)
{
    char buf[STRLEN],genbuf[STRLEN];

    setbfile(genbuf, "FocusAdmin", "board.allow");
    if (dashf(genbuf) && !seek_in_file(genbuf, currentuser.userid) && !chk_currBM(currBM))
    {
        return DONOTHING;
    }
    else
    {
        sprintf(buf,"/home/bbs/script/uploadmanager.py %s %s",currboard,currentuser.userid);
        system(buf);
    }
    return MODECHANGED;
}
#ifdef INTERNET_EMAIL
int forward_post(int ent, struct fileheader *fileinfo, char *direct)
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_forward(ent, fileinfo, direct));
}

int forward_u_post(int ent, struct fileheader *fileinfo, char *direct)
{
    if (strcmp("guest", currentuser.userid) == 0)
        return DONOTHING;
    return (mail_u_forward(ent, fileinfo, direct));
}
#endif

extern int mainreadhelp(void);
extern int b_vote(void);
extern int b_results(void);
extern int b_vote_maintain(void);
extern int b_notes_edit(void);
extern int Rangefunc(int ent, struct fileheader *fileinfo, char *direct);
extern int return_del(int ent, struct fileheader *fileinfo, char *direct);
extern int return_range(int ent, struct fileheader *fileinfo,
                        char *direct);
extern int return_find(int ent, struct fileheader *fileinfo, char *direct);

struct one_key read_comms[] = {
    {'_', underline_post},
    {'Y', UndeleteArticle},
    {'r', read_post},
    {'K', skip_post},
    {'#', topfile_post},
    {'d', del_post},
    {'D', del_range},
    {'m', mark_post},
    {'+', recommend_post},
    {'E', edit_post},
    {'|', upload_manage,}, 
    {Ctrl('G'), acction_mode,},
    {'`', acction_mode,},
    {'g', digest_post,},
    {'T', edit_title,},
    {'s', do_select,},
    {Ctrl('C'), do_cross,},
    {Ctrl('P'), do_post,},
    {Ctrl('Q'), do_module,},
    {'C', new_flag_clearto,},
    {'c', new_flag_clear,},
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
    {Ctrl('U'), SR_author,},
    {'b', SR_BMfunc,},
    {'B', Rangefunc,},
    {Ctrl('T'), acction_mode,},
    {'t', thesis_mode,},
    {'!', Q_Goodbye,},
    {'S', s_msg,},
    {'f', t_friends,},
    {'o', return_del,},
    {'X', return_find,},
    {'O', return_range,},
    {'*', show_file_info,},
    {Ctrl('E'), t_watchinboard},
    {'w', visit_wiki},
    {'\0', NULL},
};

int Read(void)
{
    char buf[STRLEN];
    char notename[STRLEN];
    time_t usetime;
    struct stat st;
    extern struct BCACHE *brdshm;

    struct shortfile *tmpbp;

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
        ansimore_withmode(notename, YEA, MORE_MODE_STUFF);
#else

    if (vote_flag(currboard, '\0', 1) == 0)
    {
        if (dashf(notename))
        {
            ansimore_withmode(notename, YEA, MORE_MODE_STUFF);
            vote_flag(currboard, 'R', 1);
        }
    }
#endif

#ifdef BOARD_CONTROL

    if ((!HAS_PERM(PERM_SYSOP)) || (!strncmp(currboard, "Arbitrator", 10))
        || (!strncmp(currboard, "OscarAward", 10)))
    {
        setbfile(notename, currboard, "board.allow");
        if (dashf(notename) && !seek_in_file(notename, currentuser.userid))
        {
            clear();
            move(5, 10);
            prints("ºÜ±§Ç¸, ±¾°åÃæÊôÓÚÍÅÌå°åÃæ£¬¶øÄú²»ÊÇ¸Ã°å³ÉÔ±");
            pressanykey();
            return 0;
        }
    }
#endif
    usetime = time(0);

    userenterboard(currboard);
    i_read(READING, buf, readtitle, readdoent, &read_comms[0],
           sizeof(struct fileheader));

    userexitboard(currboard);

    board_usage(currboard, time(0) - usetime);
    brc_update();
    tmpbp = getbcache(currboard);
    if (tmpbp->flag & PUSH_FLAG)
        brc_putpushrecord();
    return 0;
}

void notepad(void)
{
    char tmpname[STRLEN], note1[4];
    char note[3][STRLEN - 4];
    char tmp[STRLEN];
    FILE *in;
    int i, n;
    time_t thetime = time(0);
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
            getdata(5, 0,
                    "ÊÇ·ñ°ÑÄãµÄ´ó×÷·ÅÈëÁôÑÔ°å (Y)ÊÇµÄ (N)²»Òª (E)ÔÙ±à¼­ [Y]: ",
                    note1, 3, DOECHO, YEA);
            if (note1[0] == 'e' || note1[0] == 'E')
                continue;
            else
                break;
        }
        if (note1[0] != 'N' && note1[0] != 'n')
        {
            sprintf(tmp, "\033[1;32m%s\033[37m£¨%.18s£©",
                    currentuser.userid, currentuser.username);

            fprintf(in,
                    "\033[1;34m¨Š\033[44m¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰\033[36mËá\033[32mÌð\033[33m¿à\033[31mÀ±\033[37m°å\033[34m¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰¨‰\033[44m¨Ž\033[m\n");
            getdatestring(thetime, NA);
            fprintf(in,
                    "\033[1;34m¨Š\033[32;44m %-44s\033[32mÔÚ \033[36m%23.23s\033[32m Àë¿ªÊ±ÁôÏÂµÄ»°  \033[m\n",
                    tmp, datestring + 6);
            for (n = 0; n < i; n++)
            {
                if (note[n][0] == '\0')
                    break;
                fprintf(in,
                        "\033[1;34m¨Š\033[33;44m %-75.75s\033[1;34m\033[m \n",
                        note[n]);
            }
            fprintf(in,
                    "\033[1;34m¨Š\033[44m ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ \033[m \n");
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

int Q_Goodbye(void)
{
    extern int started;
    extern int *zapbuf;
    char fname[STRLEN];
    int logouts;

    free(zapbuf);
    setuserfile(fname, "msgfile");

#ifdef LOG_MY_MESG
    unlink(fname);
    setuserfile(fname, "msgfile.me");
#endif

    if (dashf(fname))
    {
        clear();
        mesgmore(fname, NA);
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
                         (logouts ==
                          1) ? 1 : (currentuser.numlogins % (logouts)) + 1,
                         YEA);
        }
    }
    else
    {
        if (fill_shmfile(2, "etc/logout", "GOODBYE_SHMKEY"))
            show_goodbyeshm();
    }
    pressanykey();
    report("exit");

    if (started)
    {
        time_t stay;

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

int Goodbye(void)
{
    char sysoplist[20][15], syswork[20][21], buf[STRLEN];
    int i, num_sysop, choose;
    FILE *sysops;
    char *ptr;

    *quote_file = '\0';
    i = 0;
    if ((sysops = fopen("etc/sysops", "r")) != NULL)
    {
        while (fgets(buf, STRLEN, sysops) != NULL && i <= 19)
        {
            if (buf[0] == '#')
                continue;
            ptr = strtok(buf, " \n\r\t");
            if (ptr)
            {
                strncpy(sysoplist[i], ptr, 14);
                ptr = strtok(NULL, " \n\r\t");
                if (ptr)
                {
                    strncpy(syswork[i], ptr, 20);
                }
                else
                    strcpy(syswork[i], "[Ö°Îñ²»Ã÷]");
                i++;
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
    if (strcmp(currentuser.userid, "guest") != 0 && !deny_me("message"))
    {
        prints
            ("[\033[1;33m3\033[m] Ð´Ð´\033[1;32mÁô\033[33mÑÔ\033[35m°å\033[mÂÞ\n");
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
            prints("[\033[1;33m%2d\033[m] %-12s %s\n", i, sysoplist[i - 1],
                   syswork[i - 1]);
        }
        prints("[\033[1;33m%2d\033[m] »¹ÊÇ×ßÁËÂÞ£¡\n", num_sysop + 1);
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
        if (HAS_PERM(PERM_POST) && choose == 3 && !deny_me("message"))
            notepad();
#endif

        if (choose == 4)
            friend_wall();
    }

    userexitboard(currboard);
    return Q_Goodbye();
}

void do_report(char *filename, char *s)
{
    char buf[1024];
    time_t dtime;

    time(&dtime);
    getdatestring(dtime, NA);
    sprintf(buf, "%s %16.16s %16.16s %s\n", currentuser.userid,
            currentuser.lasthost, datestring + 6, s);
    file_append(filename, buf);
}

void report(char *s)
{
    do_report("trace", s);
}

void gamelog(char *s)
{
    do_report("game/trace", s);
}

void board_usage(char *mode, time_t usetime)
{
    time_t now;
    char buf[256];

    now = time(0);
    getdatestring(now, NA);
    sprintf(buf, "%.22s USE %-20.20s Stay: %5ld (%s)\n", datestring, mode,
            usetime, currentuser.userid);
    file_append("use_board", buf);
}

int Info(void)
{
    modify_user_mode(XMENU);
    ansimore("Version.Info", YEA);
    clear();
    return 0;
}

int Conditions(void)
{
    modify_user_mode(XMENU);
    ansimore("COPYING", YEA);
    clear();
    return 0;
}

int Welcome(void)
{
    char ans[3];

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

int cmpbnames(char *bname, void *vbrec)
{
    struct fileheader *brec = (struct fileheader *) vbrec;

    if (!strncasecmp(bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

void cancelpost(char *board, char *userid, struct fileheader *fh,
                int owned)
{
    struct fileheader postfile;
    FILE *fin, *fout;
    char from[STRLEN], path[STRLEN];
    char fname[STRLEN], *ptr, *brd;
    int len, count;
    time_t now;

    setbfile(genbuf, board, fh->filename);
    if ((fin = fopen(genbuf, "r")) != NULL)
    {
#ifdef BACK_D_TO_RANGEDEL
        if (owned == BACK_D_TO_RANGEDEL)

            brd = (digestmode == 9
                   && (fh->accessed[0] != 0)) ? "deleted" : "rangedel";
        else
            brd = (owned) ? "junk" : "deleted";
        if (digestmode == 9 && (fh->accessed[0] == 0))
            brd = "rangedel";
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
            mystrlcpy(postfile.title, genbuf, STRLEN);
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

            setbfile(genbuf, brd, DOT_DIR);
            append_record(genbuf, &postfile, sizeof(postfile));
        }
        if (strrchr(fh->owner, '.'))
            return;
        if ((fh->filename[STRLEN - 1] == 'S')
            && (fh->filename[STRLEN - 2] == 'S')
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

int thesis_mode(void)
{
    int id;
    unsigned int pbits;

    id = getuser(currentuser.userid);
    pbits = lookupuser.userdefine;

    pbits ^= DEF_THESIS;
    lookupuser.userdefine = pbits;
    currentuser.userdefine = pbits;
    substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
    update_utmp();
    return FULLUPDATE;
}

#ifdef BACKUP_RANGE_DELETE

int NotBackupBoard(void)
{
    if (!strcmp(currboard, "junk") || !strcmp(currboard, "deleted"))
        return 1;
    return seek_in_file("etc/notbackupboards", currboard);
}

#endif

void Add_Combine(char *board, struct fileheader *fileinfo, int has_yinyan)
{
    FILE *fp;
    char buf[STRLEN];
    char temp2[1024];

    sprintf(buf, "tmp/%s.combine", currentuser.userid);
    fp = fopen(buf, "at");
    fprintf(fp,
            "\033[1;32m¡î©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¡î\033[m\n");
    {
        FILE *fp1;
        char buf[80];
        char *s_ptr, *e_ptr;
        int blankline = 0;

        setbfile(buf, board, fileinfo->filename);
        fp1 = fopen(buf, "rt");
        if (fgets(temp2, 256, fp1) != NULL)
        {
            e_ptr = strchr(temp2, ',');
            if (e_ptr != NULL)
                *e_ptr = '\0';
            s_ptr = &temp2[7];
            fprintf(fp, "    \033[0;1;32m%s \033[mÓÚ", s_ptr);
        }
        fgets(temp2, 256, fp1);
        if (fgets(temp2, 256, fp1) != NULL)
        {
            e_ptr = strchr(temp2, ',');
            if (e_ptr != NULL)
                *(--e_ptr) = '\0';
            s_ptr = strchr(temp2, '(');
            if (s_ptr == NULL)
                s_ptr = temp2;
            else
                s_ptr++;
            fprintf(fp, " \033[1;36m%s\033[m Ìáµ½£º\n", s_ptr);
        }
        while (!feof(fp1))
        {
            fgets(temp2, 256, fp1);
            if ((unsigned) *temp2 < '\x1b')
            {
                if (blankline)
                    continue;
                else
                    blankline = 1;
            }
            else
                blankline = 0;
            if (!has_yinyan && ((strstr(temp2, "¡¾")) || (*temp2 == ':')))
                continue;
            if ((*temp2 == ':')
                && ((strstr(temp2, "¡¾")) || (*(temp2 + 2) == ':')))
                continue;
            if (strncmp(temp2, "--", 2) == 0)
                break;
            fputs(temp2, fp);
        }
        fclose(fp1);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

int return_del(int ent, struct fileheader *fileinfo, char *direct)
{
    int i, fail, ed = 0;
    char buf[512], buffer[128];
    char *p;
    struct fileheader d;

    if (digestmode != 9)
        return DONOTHING;
    if ((digestmode == 9) && (returnmode != 0))
        return DONOTHING;
    if (fileinfo->accessed[0] & FILE_TOP1)
        return DONOTHING;
    memcpy(&d, fileinfo, sizeof(d));
    strncpy(currfile, fileinfo->filename, STRLEN);

    if (strlen(fileinfo->title) > 32)
    {
        p = fileinfo->title + 60;
        i = 32;
        while (*p != '\0')
        {
            *(fileinfo->title + i) = *p;
            i++;
            p++;
        }
        *(fileinfo->title + i) = '\0';
    }
    fail =
        delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
    sprintf(buf, "boards/%s/%s", currboard, DOT_DIR);
    fileinfo->accessed[0] = 0;
    append_record(buf, fileinfo, sizeof(struct fileheader));
    if ((d.accessed[0] & FILE_DELETED) && (!junkboard()))
    {
        if (strcmp(d.owner, currentuser.userid))
        {
            if ((ed = getuser(d.owner)) > 0)
            {
                lookupuser.numposts++;
                substitute_record(PASSFILE, &lookupuser,
                                  sizeof(struct userec), ed);
            }
        }
        else
        {
            set_safe_record();
            currentuser.numposts++;
            substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                              usernum);
        }

        sprintf(buffer, "returned the deleted file '%s' on '%s'",
                fileinfo->title, currboard);
        report(buffer);

    }

    log_bm_ops(currboard, "°åÃæ", "o", fileinfo->title);

    return DIRCHANGED;
}

int return_range(int ent, struct fileheader *fileinfo, char *direct)
{
    char num[9];
    int inum1, inum2;

    if (digestmode != 9)
        return DONOTHING;
    if ((digestmode == 9) && (returnmode != 0))
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
        char buffer[128];

        return_file_range(direct, inum1, inum2);
        fixkeep(direct, inum1, inum2);

        sprintf(buffer, "returned the deleted files %d-%d on '%s'", inum1,
                inum2, currboard);
        report(buffer);

        sprintf(buffer, "¹²%dÆª", inum2 - inum1);
        log_bm_ops(currboard, "°åÃæ", "O", buffer);
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
    char tmpfile[STRLEN], deleted[STRLEN];
    int fdr, fdw, fd, count;

    char *p;
    int i = 0;
    int ed = 0;

    tmpfilename(filename, tmpfile, deleted);
    if ((fd = open(".dellock", O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);

    if ((fdr = open(filename, O_RDONLY, 0)) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        close(fdr);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    count = 1;
    while (read(fdr, &fhdr, sizeof(fhdr)) == sizeof(fhdr))
    {

        if (count < id1 || count > id2)
        {
            if ((safewrite(fdw, (char *) &fhdr, sizeof(fhdr)) == -1))
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
            char *t;
            char buf[STRLEN], fullpath[STRLEN];

            strcpy(buf, filename);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(fullpath, "%s/%s/%s", buf, getfolderstr(fhdr.filename),
                    fhdr.filename);

            if ((fhdr.accessed[0] & FILE_DELETED) && (!junkboard()))
            {
                if (strcmp(fhdr.owner, currentuser.userid))
                {
                    if ((ed = getuser(fhdr.owner)) > 0)
                    {
                        lookupuser.numposts++;
                        substitute_record(PASSFILE, &lookupuser,
                                          sizeof(struct userec), ed);
                    }
                }
                else
                {
                    set_safe_record();
                    currentuser.numposts++;
                    substitute_record(PASSFILE, &currentuser,
                                      sizeof(currentuser), usernum);
                }
            }

            fhdr.accessed[0] = 0;
            setbfile(buf, currboard, DOT_DIR);

	    if (strlen(fhdr.title) > 32)
	    {
	    	p = fhdr.title + 60;
		i = 32;
		while (*p != '\0' && i < 32 + STRLEN - 60 && i < 60)
		{
		    *(fhdr.title + i) = *p;
		    i++;
		    p++;
		}
		*(fhdr.title + i) = '\0';
	    }
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

int return_find(int ent, struct fileheader *fileinfo, char *direct)
{
    int type;
    extern char currdirect[STRLEN];
    char ch[4];

    if (digestmode != 9)
        return DONOTHING;

    saveline(t_lines - 1, 0);
    move(t_lines - 1, 0);
    clrtoeol();
    ch[0] = '\0';

    getdata(t_lines - 1, 0, "²éÑ¯Ä£Ê½: 1)Í¬×÷Õß 2)±êÌâ¹Ø¼ü×Ö [1]: ",
            ch, 3, DOECHO, YEA);
    if (ch[0] == '\0')
        ch[0] = '1';
    type = atoi(ch);
    if (type < 1 || type > 2)
    {
        saveline(t_lines - 1, 1);
        return PARTUPDATE;
    }
    else if (type == 2)
    {
        getdata(t_lines - 1, 0, "ÄúÏë²éÕÒµÄÎÄÕÂ±êÌâ¹Ø¼ü×Ö: ",
                someoneID, 30, DOECHO, YEA);
        if (someoneID[0] == '\0')
        {
            saveline(t_lines - 1, 1);
            return PARTUPDATE;
        }
        type = 3;
    }
    else
    {
        getdata(t_lines - 1, 0, "ÄúÏë²éÕÒÄÄÎ»ÍøÓÑµÄÎÄÕÂ? ", someoneID,
                13, DOECHO, YEA);
        if (someoneID[0] == '\0')
        {
            saveline(t_lines - 1, 1);
            return PARTUPDATE;
        }
        getdata(t_lines - 1, 37,
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

void rengedel_autoreport(char *title, char *str, char *userid,
                         char *sysboard)
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report(title);
    sprintf(fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
    if ((se = fopen(fname, "w")) != NULL)
    {
        fprintf(se, "%s", str);
        fclose(se);
        if (userid != NULL)
            mail_file(fname, userid, title);
        if (sysboard != NULL)
            Postfile(fname, sysboard, title, 1);

        unlink(fname);
        modify_user_mode(savemode);
    }

    return;
}

int is_web_user(struct user_info *user)
{
    return (10001 == user->mode);
}

char *getfolderstr(char *filename)
{
    static char str[4] = { '\0', '\0', '\0', '\0' };
    int len = strlen(filename);

    str[0] =
        filename[len - 5] >
        '4' ? filename[len - 5] - 5 : filename[len - 5];
    str[1] = filename[len - 4];
    str[2] = filename[len - 3];
    return str;
}

int exist_in_inboardfile(char *inboard_file, int *usercount)
{
    FILE *fp;
    char buf[STRLEN];
    char *namep;
    int exist, tempcount;

    if ((fp = fopen(inboard_file, "r")) == NULL)
        return 0;

    tempcount = 0;
    exist = 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, "\n");
        if (namep != NULL)
        {
            tempcount++;
            if (strcasecmp(namep, currentuser.userid) == 0)
                exist = 1;
        }
    }
    *usercount = tempcount;
    fclose(fp);
    return exist;
}

int userenterboard(char *boardname)
{
    if (!strcmp(currentuser.userid, "guest"))
        return 0;

    char inboard_file[STRLEN];
    int usercount = 0, iPos;

    setbfile(inboard_file, currboard, INBOARD_FILE);
    if (!exist_in_inboardfile(inboard_file, &usercount))
    {
        usercount++;
        add_to_file(inboard_file, currentuser.userid);
    }

    for (iPos = 0; iPos < MAXBOARD; iPos++)
        if (!strcmp(brdshm->bcache[iPos].filename, currboard))
        {
            brdshm->inboard[iPos] = usercount;
            break;
        }

    return 1;
}

int userexitboard(char *boardname)
{

    char inboard_file[STRLEN];
    int usercount = 0, iPos;

    if (!strcmp(currentuser.userid, "guest"))
        return 0;
    if (currboard[0]=='\0')
    	return 0;

    setbfile(inboard_file, currboard, INBOARD_FILE);
    if (exist_in_inboardfile(inboard_file, &usercount))
    {
        usercount--;
        del_from_file(inboard_file, currentuser.userid);
    }

    for (iPos = 0; iPos < MAXBOARD; iPos++)
        if (!strcmp(brdshm->bcache[iPos].filename, currboard))
        {
            brdshm->inboard[iPos] = usercount;
            break;
        }

    return 1;
}

int log_bm_ops(char *boardname, char *posname, char *operation,
               char *content)
{
    char logfile[STRLEN];
    struct stat statbuf;
    FILE *fp;
    time_t now;
    struct tm *tm;
    char snippet[STRLEN];

    now = time(0);
    tm = localtime(&now);

    sprintf(logfile, "reclog/bmlog%c", get_board_dist_char(boardname));
    if (stat(logfile, &statbuf) == -1)
    {
        fp = fopen(logfile, "a");
        fprintf(fp, "%4dÄê%02dÔÂ%02dÈÕ°åÃæ²Ù×÷¼ÇÂ¼\n", tm->tm_year + 1900,
                tm->tm_mon + 1, tm->tm_mday);
        fprintf(fp, "%-6s%-16s%-13s%-4s%-7s%-5s%-29s\n", "Ê±¼ä", "°åÃæ",
                "²Ù×÷Õß", "cli", "Î»ÖÃ", "²Ù×÷", "ÄÚÈÝ");
    }
    else
        fp = fopen(logfile, "a");

    strcpy(snippet, content);

    if (strlen(snippet) > 29)
    {
        snippet[26] = '.';
        snippet[27] = '.';
        snippet[28] = '.';
        snippet[29] = '\0';
    }

    fprintf(fp, "%02d:%02d %-16s%-13s%-4s%-7s%-5s%-29s\n",
            tm->tm_hour, tm->tm_min, boardname, currentuser.userid, "tel",
            posname, operation, snippet);

    fclose(fp);
    return 1;
}

char *strnzhcpy(char *dest, const char *src, int n)
{
    register int c = 0;
    register char *dst = dest;

    if (n == 0)
        return dest;
    n--;
    while (n > 0 && *src != '\0')
    {
        c = (((*src) & 0x80) & (c ^ 0x80));
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *(dest - (c >> 7)) = '\0';
    return dst;
}

int board_in_which_dist(char *boardname)
{
    char BoardsFile[256] = { 0 };
    char category;
    struct boardheader *buffer = NULL;
    int Dindex, fd, index, total;
    struct stat st;
    char CateInSect[SectNum][5] =
        { "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw",
        "x"
    };

    sprintf(BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open(BoardsFile, O_RDONLY);
    if (fd == -1)
        return -1;
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer =
        (struct boardheader *) calloc((size_t) total,
                                      sizeof(struct boardheader));
    if (buffer == NULL)
    {
        close(fd);
        return -1;
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close(fd);
        return -1;
    }
    close(fd);

    for (index = 0; index < total; index++)
    {
        if (!strcmp(buffer[index].filename, boardname))
            break;
    }
    if (index == total)
        return -1;

    category = buffer[index].title[0];
    free(buffer);

    for (Dindex = 0; Dindex < SectNum; Dindex++)
    {
        if (strchr(CateInSect[Dindex], category) != NULL)
            break;
    }

    if (Dindex == SectNum)
        return -1;

    return Dindex;
}

char get_board_dist_char(char *boardname)
{
    int district = board_in_which_dist(boardname);

    if (district <= 0)
        return '0';
    if (district <= 9)
        return district + '0';
    else
        return district - 10 + 'A';
}

int search_in_anonyindex(char* userid, char* ranstr, struct anony_item *item_p)
{
#ifndef ANONYMOUS_MODULE
    return -1;
#endif

    int fd;
    int aisize = sizeof(struct anony_item);
    int pos;

    if ((fd = open(ANONYFILE, O_RDONLY, 0)) != -1)
    {
        pos = 0;
        if (userid != NULL)
        {
            while (read(fd, item_p, aisize) == aisize)
            {
                if (strcmp(item_p->userid, userid) == 0)
                {
                    close(fd);
                    return pos;
                }
                pos ++;
            }
        }
        else if (ranstr != NULL)
        {
            while (read(fd, item_p, aisize) == aisize)
            {
                if (strcmp(item_p->ranstr, ranstr) == 0)
                {
                    close(fd);
                    return pos;
                }
                pos ++;
            }
        }
        close(fd);
    }
    return -1;
}

void set_random_string(char* ranstr)
{
    struct anony_item item2;
    int i;

    while (1)
    {
        for (i=0; i<ANONY_RANSTR_SIZE; i++)
            ranstr[i] = rand()%26 + 'A';
        ranstr[ANONY_RANSTR_SIZE] = '\0';
        if (search_in_anonyindex(NULL, ranstr, &item2) == -1)
            break;
    }
}

char* get_anonyid(char* userid)
{
#ifndef ANONYMOUS_MODULE
    return currboard;
#endif

    if (strlen(anony_id) > 0)
        return anony_id;

    int pos, fd, fdd;
    struct anony_item item;
    char buf[STRLEN];
    time_t now;

    pos = search_in_anonyindex(userid, NULL, &item);

    if (pos == -1)
    {
        set_random_string(item.ranstr);
        strcpy(item.userid, userid);
        item.lastpost_time = time(0);
               
        if ((fd = open(ANONYFILE, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
        {
            flock(fd, LOCK_EX);
            write(fd, &item, sizeof(struct anony_item));
            flock(fd, LOCK_UN);
            close(fd);
        }
        
        strncpy(anony_id, "Anony.", 6);
        strncpy(anony_id + 6, item.ranstr, 7);
    }
    else
    {
        strncpy(anony_id, "Anony.", 6);
        strncpy(anony_id + 6, item.ranstr, 7);
        setbfile(buf, currboard, "deny_users");
        if (!seek_in_denyfile(buf, anony_id))
        {
            now = time(0);
            if (now - item.lastpost_time > 86400)
            {
                set_random_string(item.ranstr);
            }
            item.lastpost_time = now;

            if ((fdd = open(ANONYFILE, O_WRONLY)) == -1)
                return currboard;
            lseek(fdd, pos * sizeof(struct anony_item), SEEK_SET);
            flock(fdd, LOCK_EX);
            write(fdd, &item, sizeof(struct anony_item));
            flock(fdd, LOCK_UN);
            close(fdd);

            strncpy(anony_id, "Anony.", 6);
            strncpy(anony_id + 6, item.ranstr, 7);
        }
    }
    
    return anony_id;
}

int visit_wiki()
{
    FILE *fp;
    char wiki_item[STRLEN];

    if ((fp = fopen("wikitext/.INDEX", "r")) == NULL)
        return DONOTHING;
    CreateNameList();
    while (fgets(genbuf, STRLEN, fp) != NULL)
    {
        strtok(genbuf, "\n\r\t");
        if (!AddToNameList(genbuf))
            continue;
    }

    while (1)
    {
        clear();
        move(0, 0);
        prints("Ë®Ô´°Ù¿ÆTelnet²éÑ¯Ò³\n");
        namecomplete_zh("ÊäÈëÒª²éÑ¯µÄ´ÊÌõ£¨¿Õ¸ñ×Ô¶¯ËÑÑ°£¬»Ø³µÍË³ö£©£º", wiki_item);
        
        if (strlen(wiki_item) <= 1)
            return FULLUPDATE;

        sprintf(genbuf, "wikitext/%s", wiki_item);
        if (dashf(genbuf))
            ansimore_withmode(genbuf, 0, MORE_MODE_WIKI);
        else
        {
            prints("´íÎó£º¸Ã´ÊÌõ²»´æÔÚ");
            pressanykey();
        }
    }

    return FULLUPDATE;
}
