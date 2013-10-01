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
$Id: main.c,v 1.3 2004/07/17 10:28:30 jawahh Exp $
*/
#include "bbs.h"

#define BADLOGINFILE    "logins.bad"
#define VISITLOG	BBSHOME"/.visitlog"
int ERROR_READ_SYSTEM_FILE = NA;
int RMSG = YEA;
int msg_num = 0;
int count_friends = 0, count_users = 0;
int iscolor = 1;
int mailXX = 0;
char *getenv ();
int friend_login_wall ();
char *sysconf_str ();
struct user_info *t_search ();
void r_msg ();
void count_msg ();
void c_recover ();
void tlog_recover ();
void refreshdate ();
int listmode;
int numofsig = 0;
jmp_buf byebye;
extern struct BCACHE *brdshm;
extern struct UTMPFILE *utmpshm;
FILE *ufp;
int talkrequest = NA;

/* int ntalkrequest = NA ; */
int enter_uflags;
time_t lastnote;

struct user_info uinfo;

#ifndef BBSD
char tty_name[20];
#endif
char fromhost[60];

char BoardName[STRLEN];
char ULIST[STRLEN];
int utmpent = -1;
time_t login_start_time;
int showansi = 1;
int started = 0;

char GoodWish[20][STRLEN - 3];
int WishNum = 0;
int orderWish = 0;
extern int enabledbchar;

#ifdef ALLOWSWITCHCODE
int convcode = 0;
extern void resolve_GbBig5Files ();
#endif

void
log_usies (mode, mesg)
char *mode, *mesg;
{
    time_t now;
    char buf[256], *fmt;

    now = time (0);
    fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
    getdatestring (now, NA);
    sprintf (buf, fmt, datestring, mode, currentuser.userid, mesg);
    file_append ("usies", buf);
}

void
u_enter ()
{
    enter_uflags = currentuser.flags[0];
    memset (&uinfo, 0, sizeof (uinfo));
    uinfo.active = YEA;
    uinfo.pid = getpid ();
    /* Ê¹·Ç SYSOP È¨ÏÞ ID µÇÂ½Ê±×Ô¶¯»Ö¸´µ½·ÇÒþÉí×´Ì¬ */
    if (!HAS_PERM (PERM_CLOAK))
        currentuser.flags[0] &= ~CLOAK_FLAG;
    if (HAS_PERM (PERM_LOGINCLOAK) && (currentuser.flags[0] & CLOAK_FLAG))
        uinfo.invisible = YEA;
    uinfo.mode = LOGIN;
    uinfo.pager = 0;
#ifdef BBSD

    uinfo.idle_time = time (0);
#endif

    if (DEFINE (DEF_DELDBLCHAR))
        enabledbchar = 1;
    else
        enabledbchar = 0;
    if (DEFINE (DEF_FRIENDCALL))
    {
        uinfo.pager |= FRIEND_PAGER;
    }
    if (currentuser.flags[0] & PAGER_FLAG)
    {
        uinfo.pager |= ALL_PAGER;
        uinfo.pager |= FRIEND_PAGER;
    }
    if (DEFINE (DEF_FRIENDMSG))
    {
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    if (DEFINE (DEF_ALLMSG))
    {
        uinfo.pager |= ALLMSG_PAGER;
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    uinfo.uid = usernum;
    strncpy (uinfo.from, fromhost, 60);
    /* deleted by alt@
       if (!DEFINE(DEF_NOTHIDEIP)) {
          uinfo.from[22] = 'H';
       }    */
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strncpy (uinfo.tty, tty_name, 20);
#endif

    iscolor = (DEFINE (DEF_COLOR)) ? 1 : 0;
    strncpy (uinfo.userid, currentuser.userid, 20);
    strncpy (uinfo.realname, currentuser.realname, 20);
    strncpy (uinfo.username, currentuser.username, NAMELEN);
    getfriendstr ();
    getrejectstr ();
    if (HAS_PERM (PERM_EXT_IDLE))
        uinfo.ext_idle = YEA;

    listmode = 0;			/* ½èÓÃÒ»ÏÂ, ÓÃÀ´¼ÍÂ¼µ½µ× utmpent ¿¨Î»Ê§°Ü¼¸´Î */
    while (1)
    {
        utmpent = getnewutmpent (&uinfo);
        if (utmpent >= 0 || utmpent == -1)
            break;
        if (utmpent == -2 && listmode <= 100)
        {
            listmode++;
            usleep (250);		/* ÐÝÏ¢ËÄ·ÖÖ®Ò»ÃëÔÙ½ÓÔÚÀø */
            continue;
        }
        if (listmode > 100)
        {			/* ·ÅÆú°É */
            sprintf (genbuf, "getnewutmpent(): too much times, give up.");
            report (genbuf);
            prints ("getnewutmpent(): Ê§°ÜÌ«¶à´Î, ·ÅÆú. Çë»Ø±¨Õ¾³¤.\n");
            sleep (3);
            exit (0);
        }
    }
    if (utmpent < 0)
    {
        sprintf (genbuf, "Fault: No utmpent slot for %s\n", uinfo.userid);
        report (genbuf);
    }
    listmode = 0;
    digestmode = NA;
}

void
setflags (mask, value)
int mask, value;
{
    if (((currentuser.flags[0] & mask) && 1) != value)
    {
        if (value)
            currentuser.flags[0] |= mask;
        else
            currentuser.flags[0] &= ~mask;
    }
}
/*
user: the user_info[num] member  in utmpshm struct
*/
int
kick_web_user(struct user_info *user)
{
	int stay = 0;
	FILE *fp;
	struct userec u;
	if (!user->active)
		return -1;
	stay =abs(time(0)-*(int *) (user->from + 32));
	if(stay > 3600*24*3)
		stay = 3600*24*3;
	else if(stay < 0)
		stay = 0;
    
	fp = fopen(".PASSWDS","r+");
	fseek(fp,(user->uid-1)*sizeof(struct userec),SEEK_SET);
	fread(&u,sizeof(struct userec),1,fp);
	if( strcasecmp(u.userid,user->userid) )
	{
		fclose(fp);
		memset(user,0,sizeof(struct user_info));
		return -2;
	}
	u.stay += stay;
	u.lastlogout = time(0);
	fseek(fp,-sizeof(struct userec),SEEK_CUR);
	fwrite(&u,sizeof(struct userec),1,fp);
	fclose(fp);
	memset(user,0,sizeof(struct user_info));
	return 0;
}

void
u_exit ()
{
    int fd;

    /*ÕâÐ©ÐÅºÅµÄ´¦ÀíÒª¹Øµô, ·ñÔòÔÚÀëÏßÊ±µÈºò»Ø³µÊ±³öÏÖ  (ylsdd)
          ÐÅºÅ»áµ¼ÖÂÖØÐ´Ãûµ¥, Õâ¸öµ¼ÖÂµÄÃûµ¥»ìÂÒ±Èkick user¸ü¶à*/
    signal (SIGHUP, SIG_DFL);
    signal (SIGALRM, SIG_DFL);
    signal (SIGPIPE, SIG_DFL);
    signal (SIGTERM, SIG_DFL);
    signal (SIGUSR1, SIG_IGN);
    signal (SIGUSR2, SIG_IGN);

    setflags (PAGER_FLAG, (uinfo.pager & ALL_PAGER));
    if (HAS_PERM (PERM_LOGINCLOAK))
        setflags (CLOAK_FLAG, uinfo.invisible);

    if (!ERROR_READ_SYSTEM_FILE)
    {
        time_t stay;

        set_safe_record ();
        stay = time (0) - login_start_time;
        currentuser.stay += stay;
        currentuser.lastlogout = time (0);
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser),
                           usernum);
    }

    uinfo.invisible = YEA;
    uinfo.sockactive = NA;
    uinfo.sockaddr = 0;
    uinfo.destuid = 0;
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strcpy (uinfo.tty, "NoTTY");
#endif

    uinfo.pid = 0;
    uinfo.active = NA;
    //update_utmp();
    if ((fd = open (ULIST, O_RDWR | O_CREAT, 0600)) > 0)
    {
        flock (fd, LOCK_EX);
    }
    update_utmp ();
    if (fd > 0)
    {
        flock (fd, LOCK_UN);
        close (fd);
    }
    utmpent = -1;			// add by quickmouse to prevent supercloak
}

int
cmpuids (uid, up)
char *uid;
struct userec *up;
{
    return !ci_strncmp (uid, up->userid, sizeof (up->userid));
}

int
dosearchuser (userid)
char *userid;
{
    int id;

    if ((id = getuser (userid)) != 0)
    {
        if (cmpuids (userid, &lookupuser))
        {
            memcpy (&currentuser, &lookupuser, sizeof (currentuser));
            return usernum = id;
        }
    }
    memset (&currentuser, 0, sizeof (currentuser));
    return usernum = 0;
}


void
talk_request ()
{
    signal (SIGUSR1, talk_request);
    talkrequest = YEA;
    bell ();
    bell ();
    bell ();
    sleep (1);
    bell ();
    bell ();
    bell ();
    bell ();
    bell ();
    return;
}

void
abort_bbs ()
{
    extern int child_pid;
    int i;

    if (child_pid)
        kill (child_pid, 9);
    if (uinfo.mode == POSTING || uinfo.mode == SMAIL || uinfo.mode == EDIT
            || uinfo.mode == EDITUFILE || uinfo.mode == EDITSFILE
            || uinfo.mode == EDITANN)
        keep_fail_post ();
    /*if (started)
      {
        time_t stay;

        stay = time (0) - login_start_time;
        sprintf (genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);
        log_usies ("AXXED", genbuf);
        u_exit ();
      }*/
    if (started)
    {
        time_t stay;
        stay = time(0) - login_start_time;
        sprintf(genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);

        /* inboard user count  by ZV 2002.12.13 */
        if ((uinfo.mode == READING) || (uinfo.mode == POSTING) ||
                (uinfo.mode == EDIT))
        {
            for (i=0; i<MAXBOARD; i++)
                if (!strcmp(brdshm->bcache[i].filename, currboard))
                {
                    if (brdshm->inboard[i]>0)
                        brdshm->inboard[i]--;
                    break;
                }
        }

        log_usies("AXXED", genbuf);
        u_exit();
    }
    exit (0);
}

int
cmpuids2 (unum, urec)
int unum;
struct user_info *urec;
{
    return (unum == urec->uid);
}

int
count_multi (uentp)
struct user_info *uentp;
{
    static int count;

    if (uentp == NULL)
    {
        int num = count;

        count = 0;
        return num;
    }
    if (!uentp->active || !uentp->pid)
        return 0;
    if (uentp->uid == usernum)
        count++;
    return 1;
}

int
count_user ()
{
    count_multi (NULL);
    apply_ulist (count_multi);
    return count_multi (NULL);
}

void
multi_user_check ()
{
    struct user_info uin;
    int logins, mustkick = 0,pos;

    if (HAS_PERM (PERM_MULTILOG))
        return;			/* don't check sysops */

#ifdef SPECIALID

    if (strcmp (currentuser.userid, SPECIALID1) == 0
            || strcmp (currentuser.userid, SPECIALID2) == 0)
        return;
#endif
    /* allow multiple guest user */
    logins = count_user ();

    if (heavyload () && logins)
    {
        prints ("[1;33m±§Ç¸, Ä¿Ç°ÏµÍ³¸ººÉ¹ýÖØ, ÇëÎðÖØ¸´ Login¡£[m\n");
        oflush ();
        sleep (3);
        exit (1);
    }
    if (!strcasecmp ("guest", currentuser.userid))
    {
        if (logins > MAXGUEST)
        {
            prints
            ("[1;33m±§Ç¸, Ä¿Ç°ÒÑÓÐÌ«¶à [1;36mguest[33m, ÇëÉÔºóÔÙÊÔ¡£[m\n");
            oflush ();
            sleep (3);
            exit (1);
        }
        return;
    }
    else if (logins >= MULTI_LOGINS)
    {
        prints
        ("[1;32mÎªÈ·±£ËûÈËÉÏÕ¾È¨Òæ, ±¾Õ¾½öÔÊÐíÄúÓÃ¸ÃÕÊºÅµÇÂ½ %d ¸ö¡£\n[0m",
         MULTI_LOGINS);
        prints
        ("[1;36mÄúÄ¿Ç°ÒÑ¾­Ê¹ÓÃ¸ÃÕÊºÅµÇÂ½ÁË %d ¸ö£¬Äú±ØÐë¶Ï¿ªÆäËûµÄÁ¬½Ó·½ÄÜ½øÈë±¾Õ¾£¡\n[0m",
         logins);
        mustkick = 1;
    }
    if (!(pos=search_ulist (&uin, cmpuids2, usernum)))
        return;			/* user isn't logged in */

    if (!uin.active || (uin.pid && kill (uin.pid, 0) == -1))
        return;			/* stale entry in utmp file */
    getdata (0, 0, "[1;37mÄúÏëÉ¾³ýÖØ¸´µÄ login Âð (Y/N)? [N][m",
             genbuf, 4, DOECHO, YEA);
    if (genbuf[0] == 'N' || genbuf[0] == 'n' || genbuf[0] == '\0')
    {
        if (!mustkick)
            return;
        prints
        ("[33mºÜ±§Ç¸£¬ÄúÒÑ¾­ÓÃ¸ÃÕÊºÅµÇÂ½ %d ¸ö£¬ËùÒÔ£¬´ËÁ¬Ïß½«±»È¡Ïû¡£[m\n",
         logins);
        oflush ();
        sleep (3);
        exit (1);
    }
    else
    {
        if (!uin.pid)
            return;
		/*webÓÃ»§Ã»ÓÐµ¥¶ÀµÄ½ø³Ì,Òªµ¥¶Àkick */
		if( is_web_user(&uin) )
			kick_web_user(&(utmpshm->uinfo[pos-1]));
		else
	        kill (uin.pid, SIGHUP);
        //ÒÔÇ°²»ÊÇSIGHUP£¬»áµ¼ÖÂ±à¼­×÷Òµ¶ªÊ§ by sunner
        report ("kicked (multi-login)");
        log_usies ("KICK ", currentuser.username);
    }
}

#ifndef BBSD
void
system_init (argc, argv)
int argc;
char **argv;
#else
void
system_init ()
#endif
{
#ifndef BBSD
    char *rhost;
#endif

    struct sigaction act;

    gethostname (genbuf, 256);
    sprintf (ULIST, "%s.%s", ULIST_BASE, genbuf);

#ifndef BBSD

    if (argc >= 3)
    {
        strncpy (fromhost, argv[2], 60);
    }
    else
    {
        fromhost[0] = '\0';
    }
    if ((rhost = getenv ("REMOTEHOST")) != NULL)
        strncpy (fromhost, rhost, 60);
    fromhost[59] = '\0';
#if defined(SHOW_IDLE_TIME)

    if (argc >= 4)
    {
        strncpy (tty_name, argv[3], 20);
    }
    else
    {
        tty_name[0] = '\0';
    }
#endif
#endif

#ifndef lint
    signal (SIGHUP, abort_bbs);
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGPIPE, SIG_IGN);
#ifdef DOTIMEOUT

    init_alarm ();
    uinfo.mode = LOGIN;
    alarm (LOGIN_TIMEOUT);
#else

    signal (SIGALRM, SIG_SIG);
#endif

    signal (SIGTERM, SIG_IGN);
    signal (SIGURG, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
#endif

    signal (SIGUSR1, talk_request);

    sigemptyset (&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = r_msg;
    sigaction (SIGUSR2, &act, NULL);
}

void
system_abort ()
{
    if (started)
    {
        log_usies ("ABORT", currentuser.username);
        u_exit ();
    }
    clear ();
    refresh ();
    prints ("Ð»Ð»¹âÁÙ, ¼ÇµÃ³£À´à¸ !\n");
    exit (0);
}

void
logattempt (uid, frm)
char *uid, *frm;
{
    char fname[STRLEN];

    getdatestring (time (0), NA);
    sprintf (genbuf, "%-12.12s  %-30s %s\n", uid, datestring, frm);
    file_append (BADLOGINFILE, genbuf);
    sethomefile (fname, uid, BADLOGINFILE);
    file_append (fname, genbuf);

}

int
check_tty_lines ()
{				/* dii.nju.edu.cn  zhch  2000.4.11 */
    static unsigned char buf1[] = { 255, 253, 31 };
    unsigned char buf2[100];
    int n;

    if (ttyname (0))
        return;
    write (0, buf1, 3);
    n = read (0, buf2, 80);
    if (n == 12)
    {
        if (buf2[0] != 255 || buf2[1] != 251 || buf2[2] != 31)
            return;
        if (buf2[3] != 255 || buf2[4] != 250 || buf2[5] != 31
                || buf2[10] != 255 || buf2[11] != 240)
            return;
        t_lines = buf2[9];
    }
    if (n == 9)
    {
        if (buf2[0] != 255 || buf2[1] != 250 || buf2[2] != 31
                || buf2[7] != 255 || buf2[8] != 240)
            return;
        t_lines = buf2[6];
    }
    if (t_lines < 24 || t_lines > 100)
        t_lines = 24;
}

struct max_log_record
{
    int year;
    int month;
    int day;
    int logins;
    unsigned long visit;
}
max_log;
void
visitlog (void)
{
    int vfp;
    time_t now;
    struct tm *tm;
    extern struct UTMPFILE *utmpshm;

    vfp = open (VISITLOG, O_RDWR | O_CREAT, 0644);
    if (vfp == -1)
    {
        report ("Can NOT write visit Log to .visitlog");
        return;
    }
    flock (vfp, LOCK_EX);
    lseek (vfp, (off_t) 0, SEEK_SET);
    read (vfp, &max_log, (size_t) sizeof (max_log));
    if (max_log.year < 1990 || max_log.year > 2020)
    {
        now = time (0);
        tm = localtime (&now);
        max_log.year = tm->tm_year + 1900;
        max_log.month = tm->tm_mon + 1;
        max_log.day = tm->tm_mday;
        max_log.visit = 0;
        max_log.logins = 0;
    }
    max_log.visit++;
    if (max_log.logins > utmpshm->max_login_num)
        utmpshm->max_login_num = max_log.logins;
    else
        max_log.logins = utmpshm->max_login_num;
    lseek (vfp, (off_t) 0, SEEK_SET);
    write (vfp, &max_log, (size_t) sizeof (max_log));
    flock (vfp, LOCK_UN);
    close (vfp);
    sprintf (genbuf,
             "[1;32m´Ó [[36m%4dÄê%2dÔÂ%2dÈÕ[32m] Æð, ×î¸ßÈËÊý¼ÇÂ¼: [[36m%d[32m] ÀÛ¼Æ·ÃÎÊÈË´Î: [[36m%u[32m][m\n",
             max_log.year, max_log.month, max_log.day, max_log.logins,
             max_log.visit);
    prints (genbuf);
}

void
login_query ()
{
    char uid[IDLEN + 2];
    char passbuf[PASSLEN];
    int curr_login_num;
    int attempts;
    char genbuf[STRLEN];
    char *ptr;
    extern struct UTMPFILE *utmpshm;

    curr_login_num = num_active_users ();
    if (curr_login_num >= MAXACTIVE)
    {
        ansimore ("etc/loginfull", NA);
        oflush ();
        sleep (1);
        exit (1);
    }
#ifdef BBSNAME
    strcpy (BoardName, BBSNAME);
#else

    ptr = sysconf_str ("BBSNAME");
    if (ptr == NULL)
        ptr = "ÉÐÎ´ÃüÃû²âÊÔÕ¾";
    strcpy (BoardName, ptr);
#endif

    if (fill_shmfile (1, "etc/issue", "ISSUE_SHMKEY"))
    {
        show_issue ();		/* is anibanner ready, remark this and put * \n\n */
    }
    prints
    ("[1;35m»¶Ó­¹âÁÙ[1;40;33m¡¾ %s ¡¿ [m[[0;1;33;41m Add '.' after YourID to login for BIG5 [m]\n[0;1;32m±¾Õ¾¿É×¢²áÕÊºÅÊý: [[1;36m%d[0;1;32m] [m",
     BoardName, MAXUSERS);
    resolve_utmp ();
    if (utmpshm->usersum == 0)
        utmpshm->usersum = allusers ();
    if (utmpshm->max_login_num < curr_login_num)
        utmpshm->max_login_num = curr_login_num;
    prints
    ("[1;32mÄ¿Ç°ÒÑÓÐÕÊºÅÊý: [[1;36m%d[32m] [1;32mÄ¿Ç°ÉÏÕ¾ÈËÊý: [[1;36m%d[1;32m/[1;36m%d[1;32m] \n",
     utmpshm->usersum, curr_login_num, MAXACTIVE);
    //    utmpshm->usersum, curr_login_num-CountCloakMan(), MAXACTIVE);
    visitlog ();

#ifdef MUDCHECK_BEFORELOGIN

    prints ("[1;33mÎª·ÀÖ¹Ê¹ÓÃ³ÌÊ½ÉÏÕ¾£¬Çë°´ [1;36mCTRL + C[m : ");
    genbuf[0] = igetkey ();
    if (genbuf[0] != Ctrl ('C'))
    {
        prints ("\n¶Ô²»Æð£¬Äã²¢Ã»ÓÐ°´ÏÂ CTRL+C ¼ü£¡\n");
        oflush ();
        exit (1);
    }
    else
    {
        prints ("[CTRL] + [C]\n");
    }
#endif

    attempts = 0;
    while (1)
    {
        if (attempts++ >= LOGINATTEMPTS)
        {
            ansimore ("etc/goodbye", NA);
            oflush ();
            sleep (1);
            exit (1);
        }
        prints("[1;33mÇëÊäÈëÕÊºÅ[m(ÊÔÓÃÇëÊäÈë `[1;36mguest[m', ×¢²áÇëÊäÈë`[1;31mnew[m')");
        getdata (0, 0,": ",uid,IDLEN +1,DOECHO,YEA);
#ifdef ALLOWSWITCHCODE

        ptr = strchr (uid, '.');
        if (ptr)
        {
            convcode = 1;
            *ptr = '\0';
        }
#endif
        if ((strcasecmp (uid, "guest") == 0)
                && (MAXACTIVE - curr_login_num < 10))
        {
            ansimore ("etc/loginfull", NA);
            oflush ();
            sleep (1);
            exit (1);
        }
        if (strcasecmp (uid, "new") == 0)
        {
#ifdef LOGINASNEW
            memset (&currentuser, 0, sizeof (currentuser));
            new_register ();
            ansimore3 ("etc/firstlogin", YEA);
            break;
#else

            prints
            ("[1;37m±¾ÏµÍ³Ä¿Ç°ÎÞ·¨ÒÔ [36mnew[37m ×¢²á, ÇëÓÃ[36m guest[37m ½øÈë...[m\n");
#endif

        }
        else if (*uid == '\0')
            ;
        else if (!dosearchuser (uid))
        {
            prints ("[1;31m¾­²éÖ¤£¬ÎÞ´Ë ID (User ID Error)...[m\n");
        }
        else if (strcasecmp (uid, "guest") == 0)
        {
            currentuser.userlevel = 0;
            break;
#ifdef SYSOPLOGINPROTECT

        }
        else if (!strcasecmp (uid, "SYSOP")
                 && strcmp (fromhost, "localhost")
                 && strcmp (fromhost, "127.0.0.1"))
        {
            prints ("[1;32m ¾¯¸æ: ´Ó %s µÇÂ¼ÊÇ·Ç·¨µÄ! ÇëÎðÔÙÊÔ![m\n",
                    fromhost);
            prints
            ("[×¢Òâ] Îª°²È«Æð¼û£¬±¾Õ¾ÒÑ¾­Éè¶¨ SYSOP Ö»ÄÜ´ÓÖ÷»úµÇÂ½¡£\n       Èç¹ûÄúÈ·ÊµÊÇ±¾Õ¾µÄ SYSOP £¬ÇëµÇÂ½µ½±¾ BBS ·þÎñÆ÷£¬È»ºó: \n              telnet localhost port.\n");
            oflush ();
            sleep (1);
            exit (1);
#endif

        }
        else
        {
#ifdef ALLOWSWITCHCODE
            if (!convcode)
                convcode = !(currentuser.userdefine & DEF_USEGB);
#endif

            getdata (0, 0, "[1;37mÇëÊäÈëÃÜÂë: [m", passbuf, PASSLEN,
                     NOECHO, YEA);
            passbuf[8] = '\0';

            if (!checkpasswd (currentuser.passwd, passbuf))
            {
                logattempt (currentuser.userid, fromhost);
                prints ("[1;31mÃÜÂëÊäÈë´íÎó (Password Error)...[m\n");
            }
            else
            {
#ifdef SPECIALID
                if (strcmp (uid, SPECIALID1) == 0)
                {
                    ansimore ("etc/specialwelcomeboy", NA);
                    prints ("ÇëÊäÈëÄúµÄêÇ³Æ [ÎÒÊÇÐÂÉúboy] £º");
                    getdata (0, 0, "", currentuser.username, NAMELEN,
                             DOECHO, YEA);
                    if (strlen (currentuser.username) == 0)
                        strcpy (currentuser.username, "ÎÒÊÇÐÂÉúboy");
                }
                else if (strcmp (uid, SPECIALID2) == 0)
                {
                    ansimore ("etc/specialwelcomegirl", NA);
                    prints ("ÇëÊäÈëÄúµÄêÇ³Æ [ÎÒÊÇÐÂÉúgirl] £º");
                    getdata (0, 0, "", currentuser.username, NAMELEN,
                             DOECHO, YEA);
                    if (strlen (currentuser.username) == 0)
                        strcpy (currentuser.username, "ÎÒÊÇÐÂÉúgirl");
                }
                else
                {
#endif
				if (!HAS_PERM (PERM_BASIC) && !HAS_PERM (PERM_SYSOP))
                    {
                        prints
                        ("[1;32m±¾ÕÊºÅÒÑÍ£»ú¡£ÇëÏò [36mSYSOP[32m ²éÑ¯Ô­Òò[m\n");
                        oflush ();
                        sleep (1);
                        exit (1);
                    }
#ifdef CHECK_LESS60SEC
                    if (abs (time (0) - currentuser.lastlogin) < 60
                            && !HAS_PERM (PERM_SYSOP)
                            && strcasecmp (currentuser.userid, "guest") != 0)
                    {
                        prints
                        ("ÄúÔÚ 60 ÃëÄÚÖØ¸´ÉÏÕ¾.Çë°´ [1;36mCtrl+C[m ¼üÈ·ÈÏÄúµÄÉí·Ý: ");
                        genbuf[0] = igetkey ();
                        if (genbuf[0] != Ctrl ('C'))
                        {
                            prints
                            ("\n¶Ô²»Æð£¬Äú²¢Ã»ÓÐ°´ÏÂ CTRL+C ¼ü£¡Äú²»ÄÜ½øÈë±¾Õ¾\n");
                            prints ("ÈôÓÐÒÉÎÊÇëÍ¨ÖªÕ¾ÎñÈËÔ±, Ð»Ð».\n");
                            oflush ();
                            sleep (3);
                            exit (1);
                        }
                        prints ("\n");
                    }
#endif
#ifdef SPECIALID

                }
#endif

                /*
                			      if (HAS_PERM (PERM_RECLUSION) && !HAS_PERM (PERM_SYSOP))
                				{		// add byquickmouse
                				  attempts = (time (0) - currentuser.lastlogin) / 86400;
                				  if (attempts < 30)
                				    {
                				      prints
                					("\033[1;32mÓÃ»§ÒÑ¾­¹éÒþÌïÔ°%dÌìÁË£¬30ÌìÄÚ²»»á»ØÀ´\033[m\n",
                					 attempts);
                				      oflush ();
                				      sleep (1);
                				      exit (1);
                				    }
                				  else
                				    {
                				      prints (";32m»¶Ó­Äú½áÊøÒþ¾ÓÉú»î£¬ÔÙ´Î»Øµ½Õâ¸ö´ó¼ÒÍ¥\n");
                				      currentuser.userlevel =
                					currentuser.userlevel & (~PERM_RECLUSION);
                				      substitute_record (PASSFILE, &currentuser,
                							 sizeof (currentuser), usernum);
                				      pressanykey ();
                				    }
                				}*/

#ifdef CHECK_SYSTEM_PASS
                if (HAS_PERM (PERM_SYSOP))
                {
                    if (!check_systempasswd ())
                    {
                        prints ("\n¿ÚÁî´íÎó, ²»µÃÇ©Èë ! !\n");
                        oflush ();
                        sleep (2);
                        exit (1);
                    }
                }
#endif
                bzero (passbuf, PASSLEN - 1);
                break;
            }
        }
    }
    multi_user_check ();
    if (!term_init (currentuser.termtype))
    {
        prints ("Bad terminal type.  Defaulting to 'vt100'\n");
        strcpy (currentuser.termtype, "vt100");
        term_init (currentuser.termtype);
    }
    check_tty_lines ();		/* 2000.03.14 */
    sethomepath (genbuf, currentuser.userid);
    mkdir (genbuf, 0755);
    login_start_time = time (0);
}

int
valid_ident (ident)
char *ident;
{
    static char *invalid[] = { "unknown@", "root@", "gopher@", "bbs@",
                               "guest@", "nobody@", "www@", NULL
                             };
    int i;

    if (ident[0] == '@')
        return 0;
    for (i = 0; invalid[i] != NULL; i++)
        if (strstr (ident, invalid[i]) != NULL)
            return 0;
    return 1;
}

void
write_defnotepad ()
{
    currentuser.notedate = time (0);
    set_safe_record ();
    substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    return;
}

void
notepad_init ()
{
    FILE *check;
    char notetitle[STRLEN];
    char tmp[STRLEN * 2];
    char *fname, *bname, *ntitle;
    long int maxsec;
    time_t now;

    maxsec = 86400;
    lastnote = 0;
    if ((check = fopen ("etc/checknotepad", "r")) != NULL)
    {
        fgets (tmp, sizeof (tmp), check);
        lastnote = atol (tmp);
        fclose (check);
    }
    now = time (0);
    if ((now - lastnote) >= maxsec)
    {
        move (t_lines - 1, 0);
        prints ("¶Ô²»Æð£¬ÏµÍ³×Ô¶¯·¢ÐÅ£¬ÇëÉÔºò.....");
        refresh ();
        check = fopen ("etc/checknotepad", "w");
        lastnote = now - (now % maxsec);
        fprintf (check, "%d", lastnote);
        fclose (check);
        if ((check = fopen ("etc/autopost", "r")) != NULL)
        {
            while (fgets (tmp, STRLEN, check) != NULL)
            {
                fname = strtok (tmp, " \n\t:@");
                bname = strtok (NULL, " \n\t:@");
                ntitle = strtok (NULL, " \n\t:@");
                if (fname == NULL || bname == NULL || ntitle == NULL)
                    continue;
                else
                {
                    getdatestring (now, NA);
                    sprintf (notetitle, "[%8.8s %6.6s] %s", datestring + 6,
                             datestring + 23, ntitle);
                    if (dashf (fname))
                    {
                        Postfile (fname, bname, notetitle, 1);
                        sprintf (tmp, "%s ×Ô¶¯ÕÅÌù", ntitle);
                        report (tmp);
                    }
                }
            }
            fclose (check);
        }
        getdatestring (now, NA);
        sprintf (notetitle, "[%s] ÁôÑÔ°å¼ÇÂ¼", datestring);
        if (dashf ("etc/notepad", "r"))
        {
            Postfile ("etc/notepad", "message", notetitle, 1);
            unlink ("etc/notepad");
        }
        report ("×Ô¶¯·¢ÐÅÊ±¼ä¸ü¸Ä");
    }
    return;
}


void
user_login ()
{
    char fname[STRLEN];
    char *ruser;

    if (strcmp (currentuser.userid, "SYSOP") == 0)
    {
        currentuser.userlevel = ~0;	/* SYSOP gets all permission bits */
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser),
                           usernum);
    }
    ruser = getenv ("REMOTEUSER");
    sprintf (genbuf, "%s@%s", ruser ? ruser : "?", fromhost);
    log_usies ("ENTER", genbuf);
    u_enter ();
    if (ruser != NULL)
    {
        sprintf (genbuf, "%s@%s", ruser, fromhost);
        if (valid_ident (genbuf))
        {
            strncpy (currentuser.ident, genbuf, NAMELEN);
            currentuser.ident[NAMELEN - 1] = '\0';
        }
    }
    report ("Enter");
    started = 1;
    if ((!HAS_PERM (PERM_MULTILOG | PERM_SYSOP))
            && count_user () > MULTI_LOGINS && strcmp (currentuser.userid, "guest"))
    {
        report ("kicked (multi-login)[Â©ÍøÖ®Óã]");
        abort_bbs ();
    }
    initscr ();
#ifdef USE_NOTEPAD

    notepad_init ();
    if (strcmp (currentuser.userid, "guest") != 0)
    {
        if (DEFINE (DEF_NOTEPAD))
        {
            int noteln;

            if (lastnote > currentuser.notedate)
                currentuser.noteline = 0;
            noteln = countln ("etc/notepad");
            if (currentuser.noteline == 0)
            {
                shownotepad ();
            }
            else if ((noteln - currentuser.noteline) > 0)
            {
                move (0, 0);
                ansimore2 ("etc/notepad", NA, 0,
                           noteln - currentuser.noteline + 1);
                igetkey ();
                clear ();
            }
            currentuser.noteline = noteln;
            write_defnotepad ();
        }
    }
#endif
    if (show_statshm ("0Announce/bbslist/countusr", 0) && DEFINE (DEF_GRAPH))
    {
        refresh ();
        pressanykey ();
    }
    if ((vote_flag (NULL, '\0', 2 /* ¼ì²é¶Á¹ýÐÂµÄWelcome Ã» */ ) == 0))
    {
        //here change by alt@sjtu for vote when login
        {
            if (dashf ("Welcome"))
            {
                ansimore ("Welcome", YEA);
                vote_flag (NULL, 'R', 2 /* Ð´Èë¶Á¹ýÐÂµÄWelcome */ );
            }
            //      strcpy(currboard,"sysop");
            //      b_vote();
            //       vote_flag(NULL, 'R', 2 /* Ð´Èë¶Á¹ýÐÂµÄWelcome */ );
        }
    }
    else
    {
        if (fill_shmfile (3, "Welcome2", "WELCOME_SHMKEY"))
            show_welcomeshm ();
    }
    show_statshm ("etc/posts/day", 1);
    refresh ();
    move (t_lines - 2, 0);
    clrtoeol ();

    if (currentuser.numlogins < 1)
    {
        currentuser.numlogins = 0;
        getdatestring (time (0), NA);
        prints ("\033[1;36m¡î ÕâÊÇÄúµÚ \033[33m1\033[36m ´Î°Ý·Ã±¾Õ¾£¬Çë¼Ç×¡½ñÌì°É¡£\n");
        prints ("¡î ÄúµÚÒ»´ÎÁ¬Èë±¾Õ¾µÄÊ±¼äÎª \033[33m%s\033[m ", datestring);
    }
    else
    {
        getdatestring (currentuser.lastlogin, NA);
        prints
        ("\033[1;36m¡î ÕâÊÇÄúµÚ \033[33m%d\033[36m ´Î°Ý·Ã±¾Õ¾£¬ÉÏ´ÎÄúÊÇ´Ó \033[33m%s\033[36m Á¬Íù±¾Õ¾¡£\n",
         currentuser.numlogins + 1, currentuser.lasthost);
        prints ("¡î ÉÏ´ÎÁ¬ÏßÊ±¼äÎª \033[33m%s\033[m ", datestring);
    }
    igetkey ();
    WishNum = 9999;
    setuserfile (fname, BADLOGINFILE);
    if (ansimore (fname, NA) != -1)
    {
        if (askyn ("ÄúÒªÉ¾³ýÒÔÉÏÃÜÂëÊäÈë´íÎóµÄ¼ÇÂ¼Âð", YEA, YEA) == YEA)
            unlink (fname);
    }

    set_safe_record ();
    check_uinfo (&currentuser, 0);
    strncpy (currentuser.lasthost, fromhost, 16);
    currentuser.lasthost[15] = '\0';	/* dumb mistake on my part */
    currentuser.lastlogin = time (0);

    if (HAS_PERM (PERM_SYSOP) || !strcmp (currentuser.userid, "guest"))
        currentuser.lastjustify = time (0);
    //next remove by alt@sjtu
    //È¡ÏûÃ¿Äê1´ÎµÄÖØÐÂ×¢²á
    //{
    /*
       if (    HAS_PERM(PERM_LOGINOK) 
       && (abs(time(0)-currentuser.lastjustify)>=REG_EXPIRED * 86400)) {
       #ifdef MAILCHECK       
       currentuser.email[0]= '\0';
       #endif      
       currentuser.address[0]='\0';
       currentuser.userlevel &= ~(PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);
       mail_file("etc/expired", currentuser.userid, "¸üÐÂ¸öÈË×ÊÁÏËµÃ÷¡£");
       }
     */
    //}
    currentuser.numlogins++;
    if (currentuser.firstlogin == 0)
    {
        currentuser.firstlogin = time (0) - 7 * 86400;
    }
    substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    check_register_info ();
}


void
set_numofsig ()
{
    int sigln;
    char signame[STRLEN];

    setuserfile (signame, "signatures");
    sigln = countln (signame);
    numofsig = sigln / MAXSIGLINES;
    if ((sigln % MAXSIGLINES) != 0)
        numofsig += 1;
}

int
check_maxmail ()
{
    extern char currmaildir[STRLEN];
    int maxmail, maxsize, mailsize, oldnum;
	int bmns = 0;	//µ£ÈÎ°åÖ÷ÊýÁ¿

//    maxmail = (HAS_PERM (PERM_SYSOP)) ?
//              MAX_SYSOPMAIL_HOLD : (HAS_PERM (PERM_BOARDS)) ?
//              MAX_BMMAIL_HOLD : MAX_MAIL_HOLD;
	//modified by rwzmm @ sjtubbs, change the max mail hold of bms
	//depending on how many bms he gets.
	if(HAS_PERM (PERM_SYSOP))
	{
		maxmail = MAX_SYSOPMAIL_HOLD;
	}
	else if(HAS_PERM (PERM_BOARDS))
	{
		bmns = getbmns(currentuser.userid);
		if(bmns > 0)
		maxmail = MAX_BMMAIL_HOLD + MAX_MAIL_HOLD * (bmns-1);
	}
	else
	{
		maxmail = MAX_MAIL_HOLD;
	}

    set_safe_record ();
    oldnum = currentuser.nummails;
    currentuser.nummails =
        get_num_records (currmaildir, sizeof (struct fileheader));
    if (oldnum != currentuser.nummails)
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    if (HAS_PERM (PERM_MAILINFINITY))
        return 0;
    maxsize = getmailboxsize (currentuser.userid, currentuser.userlevel);
    mailsize = getmailsize (currentuser.userid);
    if (currentuser.nummails > maxmail || mailsize > maxsize)
    {
        mailXX = 1;
        clear ();
        move (4, 0);
        if (currentuser.nummails > maxmail)
            prints ("ÄúµÄË½ÈËÐÅ¼þ¸ß´ï %d ·â, ÄúµÄÓÊ¼þÉÏÏÞ: %d ·â\n",
                    currentuser.nummails, maxmail);
        if (mailsize > maxsize)
            prints ("ÄúµÄÐÅ¼þÈÝÁ¿¸ß´ï %d K£¬ÄúµÄÈÝÁ¿ÉÏÏÞ: %d K\n",
                    mailsize, maxsize);
        prints
        ("ÄúµÄË½ÈËÐÅ¼þÒÑ¾­³¬ÏÞ, ÇëÕûÀíÐÅÏä£¬·ñÔòÎÞ·¨Ê¹ÓÃ±¾Õ¾µÄËÍÐÅ¹¦ÄÜ¡£\n");
        if (currentuser.nummails > maxmail + 100)
        {
            sprintf (genbuf, "Ë½ÈËÐÅ¼þ¹ýÁ¿: %d ·â", currentuser.nummails);
            securityreport (genbuf);
        }
        if (mailsize > maxsize + 1000)
        {
            sprintf (genbuf, "Ë½ÈËÐÅ¼þ¹ýÁ¿: %d K", mailsize);
            securityreport (genbuf);
        }
    }
    else
    {
        mailXX = 0;
    }
    return mailXX;
}

#ifndef BBSD
int
main (argc, argv)
int argc;
char **argv;
#else
void
start_client ()
#endif
{
    load_sysconf ();

#ifdef ALLOWSWITCHCODE

    resolve_GbBig5Files ();
#endif

#ifndef BBSD

    if (argc < 2 || *argv[1] != 'h')
    {
        printf ("You cannot execute this program directly.\n");
        exit (-1);
    }
    system_init (argc, argv);
#else

    system_init ();
#endif

    if (setjmp (byebye))
    {
        system_abort ();
    }
#ifndef BBSD
    get_tty ();
    init_tty ();
#endif

    login_query ();
    user_login ();
    m_init ();
    RMSG = NA;
    clear ();
    c_recover ();
#ifdef TALK_LOG

    tlog_recover ();		/* 990713.edwardc for talk_log recover */
#endif

    if (strcmp (currentuser.userid, "guest"))
    {
        if (HAS_PERM (PERM_ACCOUNTS) && dashf("register_sjtubbs"))
        {
			struct stat st = {0};
			stat("register_sjtubbs", &st);
			if(st.st_size >0)
            {
				prints ("[0;1m\n\n");
				prints ("[33mÄ¿Ç°ÓÐÐÂÊ¹ÓÃÕßµÈºòÄúµÄÉóÅú¡£[37m\n\n");
				pressanykey ();
			}
        }
        if (check_maxmail ())
            pressanykey ();
        move (9, 0);
        clrtobot ();
        if (!DEFINE (DEF_NOLOGINSEND))
            if (!uinfo.invisible)
                apply_ulist (friend_login_wall);
        clear ();
        set_numofsig ();
    }

    ActiveBoard_Init ();

    fill_date ();			/* ¼ì²é¼ÍÄîÈÕ */
    b_closepolls ();		/* ¹Ø±ÕÍ¶Æ± */

    num_alcounter ();

	//add by rwzmm @ sjtubbs
	//show global vote.
	show_global_vote();

    if (count_friends > 0 && DEFINE (DEF_LOGFRIEND))
        t_friends ();
    while (1)
    {
        if (DEFINE (DEF_NORMALSCR))
            domenu ("TOPMENU");
        else
            domenu ("TOPMENU2");
        Goodbye ();
    }
}

int refscreen = NA;

int
egetch ()
{
    int rval;

    check_calltime ();
    if (talkrequest)
    {
        talkreply ();
        refscreen = YEA;
        return -1;
    }
    while (1)
    {
        rval = igetkey ();
        if (talkrequest)
        {
            talkreply ();
            refscreen = YEA;
            return -1;
        }
        if (rval != Ctrl ('L'))
            break;
        redoscr ();
    }
    refscreen = NA;
    return rval;
}

char *
boardmargin ()
{
    static char buf[STRLEN];

    if (selboard)
        sprintf (buf, "ÌÖÂÛÇø [%s]", currboard);
    else
    {
        brc_initial (DEFAULTBOARD);
        if (!getbnum (currboard))
            setoboard (currboard);
        selboard = 1;
        sprintf (buf, "ÌÖÂÛÇø [%s]", currboard);
    }
    return buf;
}

void
update_endline ()
{
    char buf[255], fname[STRLEN], *ptr;
    time_t now;
    FILE *fp;
    int i, allstay, foo, foo2;

    move (t_lines - 1, 0);
    clrtoeol ();

    if (!DEFINE (DEF_ENDLINE))
        return;

    now = time (0);
    allstay = getdatestring (now, YEA);	// allstay Îªµ±Ç°ÃëÊý
    if (allstay == 0)
    {
nowishfile:
        resolve_boards ();
        strcpy (datestring, brdshm->date);
        allstay = 1;
    }
    if (allstay < 5)
    {
        allstay = (now - login_start_time) / 60;
        sprintf (buf, "[[36m%.12s[33m]", currentuser.userid);
        num_alcounter ();
        prints
        ("[1;44;33m[[36m%29s[33m][[36m%4d[33mÈË/[1;36m%3d[33mÓÑ][[36m%1s%1s%1s%1s%1s%1s[33m]ÕÊºÅ%-24s[[36m%3d[33m:[36m%2d[33m][m",
         datestring, count_users, count_friends,
         (uinfo.pager & ALL_PAGER) ? "P" : "p",
         (uinfo.pager & FRIEND_PAGER) ? "O" : "o",
         (uinfo.pager & ALLMSG_PAGER) ? "M" : "m",
         (uinfo.pager & FRIENDMSG_PAGER) ? "F" : "f",
         (DEFINE (DEF_MSGGETKEY)) ? "X" : "x",
         (uinfo.invisible == 1) ? "C" : "c", buf,
         (allstay / 60) % 1000, allstay % 60);
        return;
    }
    setuserfile (fname, "HaveNewWish");
    if (WishNum == 9999 || dashf (fname))
    {
        if (WishNum != 9999)
            unlink (fname);
        WishNum = 0;
        orderWish = 0;

        if (is_birth (currentuser))
        {
            strcpy (GoodWish[WishNum],
                    "                     À²À²¡«¡«£¬ÉúÈÕ¿ìÀÖ!   ¼ÇµÃÒªÇë¿ÍÓ´ :P                   ");
            WishNum++;
        }

        setuserfile (fname, "GoodWish");
        if ((fp = fopen (fname, "r")) != NULL)
        {
            for (; WishNum < 20;)
            {
                if (fgets (buf, 255, fp) == NULL)
                    break;
                buf[STRLEN - 4] = '\0';
                ptr = strtok (buf, "\n\r");
                if (ptr == NULL || ptr[0] == '#')
                    continue;
                strcpy (buf, ptr);
                for (ptr = buf; *ptr == ' ' && *ptr != 0; ptr++)
                    ;
                if (*ptr == 0 || ptr[0] == '#')
                    continue;
                for (i = strlen (ptr) - 1; i < 0; i--)
                    if (ptr[i] != ' ')
                        break;
                if (i < 0)
                    continue;
                foo = strlen (ptr);
                foo2 = (STRLEN - 3 - foo) / 2;
                strcpy (GoodWish[WishNum], "");
                for (i = 0; i < foo2; i++)
                    strcat (GoodWish[WishNum], " ");
                strcat (GoodWish[WishNum], ptr);
                for (i = 0; i < STRLEN - 3 - (foo + foo2); i++)
                    strcat (GoodWish[WishNum], " ");
                GoodWish[WishNum][STRLEN - 4] = '\0';
                WishNum++;
            }
            fclose (fp);
        }
    }
    if (WishNum == 0)
        goto nowishfile;
    if (orderWish >= WishNum * 2)
        orderWish = 0;
    prints ("[0;1;44;33m[[36m%s[33m][m", GoodWish[orderWish / 2]);
    orderWish++;
}

/*ReWrite by SmallPig*/
void
showtitle (title, mid)
char *title, *mid;
{
    char buf[STRLEN], *note;
    int spc1, spc2;

    note = boardmargin ();
    spc1 = 39 + num_ans_chr (title) - strlen (title) - strlen (mid) / 2;
    if (spc1 < 2)
        spc1 = 2;
    spc2 =
        79 - (strlen (title) - num_ans_chr (title) + spc1 + strlen (note) +
              strlen (mid));
    //if (spc1 < 1) spc1 = 1;
    if (spc2 < 1)
        spc2 = 1;
    move (0, 0);
    clrtoeol ();
    sprintf (buf, "%*s", spc1, "");
    if (!strcmp (mid, BoardName))
        prints ("[1;44;33m%s%s[37m%s[1;44m", title, buf, mid);
    else if (mid[0] == '[')
        prints ("[1;44;33m%s%s[5;36m%s[m[1;44m", title, buf, mid);
    else
        prints ("[1;44;33m%s%s[36m%s", title, buf, mid);
    sprintf (buf, "%*s", spc2, "");
    prints ("%s[33m%s[m\n", buf, note);
    update_endline ();
    move (1, 0);
}

void
firsttitle (title)
char *title;
{
    char middoc[30];

    if (chkmail ())
        //              strcpy(middoc, strstr(title,"ÌÖÂÛÇøÁÐ±í")?"[ÄúÓÐÐÅ¼þ£¬°´ M ¿´ÐÂÐÅ]":"[ÄúÓÐÐÅ¼þ]");
        strcpy (middoc,
                strstr (title,
                        "ÌÖÂÛÇøÁÐ±í") ? "[ÄúÓÐÐÅ¼þ£¬°´ L ¿´ÐÂÐÅ]" : "[ÄúÓÐÐÅ¼þ]");
    else if (mailXX == 1)
        strcpy (middoc, "[ÐÅ¼þ¹ýÁ¿£¬ÇëÕûÀíÐÅ¼þ!]");
    else
        strcpy (middoc, BoardName);

    showtitle (title, middoc);
}

void
docmdtitle (title, prompt)
char *title, *prompt;
{
    firsttitle (title);
    move (1, 0);
    clrtoeol ();
    prints ("%s", prompt);
    clrtoeol ();
}

void
c_recover ()
{
    char fname[STRLEN], buf[STRLEN];
    int a;

    sprintf (fname, "home/%c/%s/%s.deadve", toupper (currentuser.userid[0]),
             currentuser.userid, currentuser.userid);
    if (!dashf (fname) || strcmp (currentuser.userid, "guest") == 0)
        return;
    clear ();
    strcpy (genbuf, "");
    getdata (0, 0,
             "[1;32mÄúÓÐÒ»¸ö±à¼­×÷Òµ²»Õý³£ÖÐ¶Ï£¬(S) Ð´ÈëÔÝ´æµµ (M) ¼Ä»ØÐÅÏä (Q) ËãÁË£¿[M]£º[m",
             genbuf, 2, DOECHO, YEA);
    switch (genbuf[0])
    {
    case 'Q':
    case 'q':
        unlink (fname);
        break;
    case 'S':
    case 's':
        while (1)
        {
            strcpy (genbuf, "");
            getdata (2, 0, "[1;33mÇëÑ¡ÔñÔÝ´æµµ [0-7] [0]£º[m", genbuf, 2,
                     DOECHO, YEA);
            if (genbuf[0] == '\0')
                a = 0;
            else
                a = atoi (genbuf);
            if (a >= 0 && a <= 7)
            {
                sprintf (buf, "home/%c/%s/clip_%d",
                         toupper (currentuser.userid[0]), currentuser.userid,
                         a);
                if (dashf (buf))
                {
                    getdata (3, 0,
                             "[1;31mÔÝ´æµµÒÑ´æÔÚ£¬¸²¸Ç»ò¸½¼Ó? (O)¸²¸Ç (A)¸½¼Ó [O]£º[m",
                             genbuf, 2, DOECHO, YEA);
                    switch (genbuf[0])
                    {
                    case 'A':
                    case 'a':
                        f_cp (fname, buf, O_APPEND);
                        unlink (fname);
                        break;
                    default:
                        unlink (buf);
                        rename (fname, buf);
                        break;
                    }
                }
                else
                    rename (fname, buf);
                break;
            }
        }
        break;
    default:
        mail_file (fname, currentuser.userid, "²»Õý³£¶ÏÏßËù±£ÁôµÄ²¿·Ý...");
        unlink (fname);
        break;
    }
}

#ifdef TALK_LOG
void
tlog_recover ()
{
    char buf[256];

    sprintf (buf, "home/%c/%s/talk_log", toupper (currentuser.userid[0]),
             currentuser.userid);

    if (strcasecmp (currentuser.userid, "guest") == 0 || !dashf (buf))
        return;

    clear ();
    strcpy (genbuf, "");
    getdata (0, 0,
             "[1;32mÄúÓÐÒ»¸ö²»Õý³£¶ÏÏßËùÁôÏÂÀ´µÄÁÄÌì¼ÇÂ¼, ÄúÒª .. (M) ¼Ä»ØÐÅÏä (Q) ËãÁË£¿[Q]£º[m",
             genbuf, 2, DOECHO, YEA);

    if (genbuf[0] == 'M' || genbuf[0] == 'm')
    {
        mail_file (buf, currentuser.userid, "ÁÄÌì¼ÇÂ¼");
    }
    unlink (buf);
    return;
}
#endif


