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
$Id: sendmsg.c,v 1.3 2004/07/17 10:28:30 jawahh Exp $
*/

#include "bbs.h"
#ifdef lint
#include <sys/uio.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern int RMSG;
extern int msg_num;
char buf2[STRLEN];
struct user_info *t_search ();
extern struct UTMPFILE *utmpshm;

void count_msg ();
void r_msg ();
extern int is_web_user(struct user_info *user);

int
get_msg (uid, msg, line)
char *msg, *uid;
int line;
{
    char genbuf[3];

    move (line, 0);
    clrtoeol ();
    prints ("ËÍÒôÐÅ¸ø£º%s", uid);
    memset (msg, 0, sizeof (msg));
    while (1)
    {
        getdata (line + 1, 0, "ÒôÐÅ : ", msg, 55, DOECHO, NA);
        if (msg[0] == '\0')
            return NA;
        getdata (line + 2, 0,
                 "È·¶¨ÒªËÍ³öÂð(Y)ÊÇµÄ (N)²»Òª (E)ÔÙ±à¼­? [Y]: ", genbuf, 2,
                 DOECHO, YEA);
        if (genbuf[0] == 'e' || genbuf[0] == 'E')
            continue;
        if (genbuf[0] == 'n' || genbuf[0] == 'N')
            return NA;
        else
            return YEA;
    }
}

char
msgchar (uin)
struct user_info *uin;
{
    if (uin->mode == BBSNET)
        return '@';
    if (isreject (uin))
        return '*';
    if ((uin->pager & ALLMSG_PAGER))
        return ' ';
    if (hisfriend (uin))
    {
        if ((uin->pager & FRIENDMSG_PAGER))
            return 'O';
        else
            return '#';
    }
    return '*';
}

int
canmsg (uin)
struct user_info *uin;
{
    if (isreject (uin))
        return NA;
    if ((uin->pager & ALLMSG_PAGER) || HAS_PERM (PERM_SYSOP | PERM_FORCEPAGE))
        return YEA;
    if ((uin->pager & FRIENDMSG_PAGER) && hisfriend (uin))
        return YEA;
    return NA;
}

s_msg ()
{
    do_sendmsg (NULL, NULL, 0, 0);
}

int
show_allmsgs ()
{
    char fname[STRLEN];

    if (!strcmp (currentuser.userid, "guest"))
        return;
#ifdef LOG_MY_MESG

    setuserfile (fname, "msgfile.me");
#else

    setuserfile (fname, "msgfile");
#endif

    clear ();
    modify_user_mode (LOOKMSGS);
    if (dashf (fname))
    {
        mesgmore (fname, YEA, 0, 9999);
        clear ();
    }
    else
    {
        move (5, 30);
        prints ("Ã»ÓÐÈÎºÎµÄÑ¶Ï¢´æÔÚ£¡£¡");
        pressanykey ();
        clear ();
    }
}

/*int
do_sendmsg (uentp, msgstr, mode, userpid)
     struct user_info *uentp;
     char msgstr[256];
     int mode;
     int userpid;
{
  char uident[STRLEN], ret_str[20];
  time_t now;
  struct user_info *uin;
  char buf[80], *msgbuf, *timestr;
 
#ifdef LOG_MY_MESG
  char *mymsg, buf2[80];
  int ishelo = 0;		
 
  mymsg = (char *) malloc (256);
#endif
  msgbuf = (char *) malloc (256);
 
  if (mode == 0)
    {
      move (2, 0);
      clrtobot ();
 
      modify_user_mode (MSG);
    }
  if (uentp == NULL)
    {
      prints ("<ÊäÈëÊ¹ÓÃÕß´úºÅ>\n");
      move (1, 0);
      clrtoeol ();
      prints ("ËÍÑ¶Ï¢¸ø: ");
      creat_list ();
      namecomplete (NULL, uident);
      if (uident[0] == '\0')
	{
	  clear ();
	  return 0;
	}
      uin = t_search (uident, NA);
      if (uin == NULL)
	{
	  move (2, 0);
	  prints ("¶Ô·½Ä¿Ç°²»ÔÚÏßÉÏ...\n");
	  pressreturn ();
	  move (2, 0);
	  clrtoeol ();
	  return -1;
	}
      if (uin->mode == BBSNET || uin->mode == LOCKSCREEN
	  || uin->mode == PAGE || !canmsg (uin))
	{
	  move (2, 0);
	  prints ("Ä¿Ç°ÎÞ·¨´«ËÍÑ¶Ï¢¸ø¶Ô·½.\n");
	  pressreturn ();
	  move (2, 0);
	  clrtoeol ();
	  return -1;
	}
    }
  else
    {
      if (uentp->uid == usernum)
	return 0;
      uin = uentp;
      if (uin->mode == BBSNET || uin->mode == LOCKSCREEN
	  || uin->mode == PAGE || !canmsg (uin))
	return 0;
      strcpy (uident, uin->userid);
    }
  if (msgstr == NULL)
    {
      if (!get_msg (uident, buf, 1))
	{
	  move (1, 0);
	  clrtoeol ();
	  move (2, 0);
	  clrtoeol ();
	  return 0;
	}
    }
  now = time (0);
  timestr = ctime (&now) + 11;
  *(timestr + 8) = '\0';
  strcpy (ret_str, "^Z»Ø");
  if (msgstr == NULL || mode == 2)
    {
      sprintf (msgbuf,
	       "[0;1;44;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(%s)[m[%05dm\n",
	       currentuser.userid, timestr,
	       (msgstr == NULL) ? buf : msgstr, ret_str, uinfo.pid);
#ifdef LOG_MY_MESG
      sprintf (mymsg,
	       "[1;32;40mTo [1;33;40m%-12.12s[m (%-5.5s):%-55.55s\n",
	       uin->userid, timestr, (msgstr == NULL) ? buf : msgstr);
 
      sprintf (buf2, "ÄãµÄºÃÅóÓÑ %s ÒÑ¾­ÉÏÕ¾ÂÞ£¡", currentuser.userid);
 
      if (msgstr != NULL)
	if (strcmp (msgstr, buf2) == 0)
	  ishelo = 1;
	else if (strcmp (buf, buf2) == 0)
	  ishelo = 1;
#endif
    }
  else if (mode == 0)
    {
      sprintf (msgbuf,
	       "[0;1;5;44;33mÕ¾³¤ ì¶[36m %8.8s [33m¹ã²¥£º[m[1;37;44m%-57.57s[m[%05dm\n",
	       timestr, msgstr, uinfo.pid);
    }
  else if (mode == 1)
    {
      sprintf (msgbuf,
	       "[0;1;44;36m%-12.12s[37m([36m%-5.5s[37m) ÑûÇëÄã[37m%-48.48s[31m(%s)[m[%05dm\n",
	       currentuser.userid, timestr, msgstr, ret_str, uinfo.pid);
    }
  else if (mode == 3)
    {
      sprintf (msgbuf,
	       "[0;1;45;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(%s)[m[%05dm\n",
	       currentuser.userid, timestr,
	       (msgstr == NULL) ? buf : msgstr, ret_str, uinfo.pid);
    }
  if (userpid)
    {
      if (userpid != uin->pid)
	{
	  saveline (0, 0);	
	  move (0, 0);
	  clrtoeol ();
	  prints ("[1m¶Ô·½ÒÑ¾­ÀëÏß...[m\n");
	  sleep (1);
	  saveline (0, 1);	
	  return -1;
	}
    }
  if (!uin->active || kill (uin->pid, 0) == -1)
    {
      if (msgstr == NULL)
	{
	  prints ("\n¶Ô·½ÒÑ¾­ÀëÏß...\n");
	  pressreturn ();
	  clear ();
	}
      return -1;
    }
  sethomefile (buf, uident, "msgfile");
  file_append (buf, msgbuf);
 
#ifdef LOG_MY_MESG
 
  if (mode == 2 || (mode == 0 && msgstr == NULL))
    {
      if (ishelo == 0)
	{
	  sethomefile (buf, currentuser.userid, "msgfile.me");
	  file_append (buf, mymsg);
	}
    }
  sethomefile (buf, uident, "msgfile.me");
  file_append (buf, msgbuf);
  free (mymsg);
 
#endif
  free (msgbuf);
  if (uin->pid)
    {
      kill (uin->pid, SIGUSR2);
    }
  if (msgstr == NULL)
    {
      prints ("\nÒÑËÍ³öÑ¶Ï¢...\n");
      pressreturn ();
      clear ();
    }
  return 1;
}*/

int send_web_msg(touserid)
char *touserid;
{
    int i;
    FILE *fp;
    char buf[1048], file[256], *id = touserid;
	struct stat fsbuf;

    memset(buf, 0x20, 1048);
    buf[1047] = 0x00;

    sprintf(file, "home/%c/%s/.msgdir", toupper(id[0]), id);
	if(stat(file,&fsbuf)==-1)
		return 1;
    i = fsbuf.st_size / sizeof(struct msgdir);
    if (get_record(file,buf, sizeof(struct msgdir), i ) < 0)
        return 1;
    for(i = 0; i<1048; i ++)
    {
        if(buf[i] == 0x00)
            buf[i] = 0x20;
    }
    sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
    fp = fopen(file, "a");
    fwrite(buf, sizeof(struct msgdir), 1, fp);
    fclose(fp);
	return 0;
	
}

int do_sendmsg(uentp, msgstr, mode, userpid)
struct user_info *uentp;
char msgstr[MAX_MSG_SIZE];
int mode;
int userpid;

{
    char uident[STRLEN];
    time_t now;
    struct user_info *uin;
    char buf[80], *msgbuf, *timestr;

    /*
    	//add by rwzmm @ sjtubbs, long and multi message auto new line
    	char autonewline[MAX_MSG_SIZE];
    	char* anchor = NULL;
    	char* newlinepoint = NULL;
    	int countchar = 0;
    	//add end*/



#ifdef LOG_MY_MESG

    char *mymsg, buf2[80];
    int ishelo = 0;		/* ÊÇ²»ÊÇºÃÓÑÉÏÕ¾Í¨ÖªÑ¶Ï¢ */

    mymsg = (char *) malloc(MAX_MSG_SIZE);
#endif

    msgbuf = (char *) malloc(MAX_MSG_SIZE);

    if (mode == 0)
    {
        move(2, 0);
        clrtobot();
        /*
        	if (uinfo.invisible && !HAS_PERM(PERM_SYSOP)) {
        			move(2, 0);
        			prints("±§Ç¸, ´Ë¹¦ÄÜÔÚÒþÉí×´Ì¬ÏÂ²»ÄÜÖ´ÐÐ...\n");
        			pressreturn();
        			return 0;
        		}
        */
        modify_user_mode(MSG);
        //add by rwzmm @ sjtubbs

    }
    if (uentp == NULL)
    {
        prints("<ÊäÈëÊ¹ÓÃÕß´úºÅ>\n");
        move(1, 0);
        clrtoeol();
        prints("ËÍÑ¶Ï¢¸ø: ");
        creat_list();
        namecomplete(NULL, uident);
        if (uident[0] == '\0')
        {
            clear();

            return 0;
        }
        uin = t_search(uident, NA);
        if (uin == NULL)
        {
            move(2, 0);
            prints("¶Ô·½Ä¿Ç°²»ÔÚÏßÉÏ...\n");
            pressreturn();
            move(2, 0);
            clrtoeol();

            return -1;
        }
        if (uin->mode == IRCCHAT || uin->mode == BBSNET || uin->mode == LOCKSCREEN ||  uin->mode == PAGE ||  !canmsg(uin))
        {
            move(2, 0);
            prints("Ä¿Ç°ÎÞ·¨´«ËÍÑ¶Ï¢¸ø¶Ô·½.\n");
            pressreturn();
            move(2, 0);
            clrtoeol();

            return -1;
        }
    }
    else
    {
        if (uentp->uid == usernum)

            return 0;
        uin = uentp;
        if (uin->mode == IRCCHAT || uin->mode == BBSNET ||  uin->mode == PAGE || uin->mode == LOCKSCREEN || !canmsg(uin))

            return 0;
        strcpy(uident, uin->userid);
    }
    if (msgstr == NULL)
    {
        if (!get_msg(uident, buf, 1))
        {
            move(1, 0);
            clrtoeol();
            move(2, 0);
            clrtoeol();
            return 0;
        }
    }
    now = time(0);
    /*
    timestr = ctime(&now) + 11;
    *(timestr + 8) = '\0';
    */
    timestr = ctime(&now);
    *(timestr+24) = '\0';
    if (msgstr == NULL || mode == 2)
    {


        //add by rwzmm@sjtubbs, debug for multi message

        sprintf(msgbuf, "[0;1;44;36m%-12.12s[33m([36m%-25s[33m):%50s\n%s\n", currentuser.userid, timestr, "[1;31m^Z»Ø¸´ »Ø³µÈ¡Ïû[0m",(msgstr == NULL) ? buf : msgstr);
#ifdef LOG_MY_MESG
        //	sprintf(mymsg, "[1;32;40mTo [1;33;40m%-12.12s[m (%-5.5s):%-58.58s\n", uin->userid, timestr, (msgstr == NULL) ? buf : msgstr);
        sprintf(mymsg, "[1;32;40mTo [1;33;40m%-12.12s (%-25s):[0m[0m[0m\n%s\n", uin->userid, timestr, (msgstr == NULL) ? buf : msgstr);

        sprintf(buf2, "ÄãµÄºÃÅóÓÑ %s ÒÑ¾­ÉÏÕ¾ÂÞ£¡", currentuser.userid);

        if (msgstr != NULL)
            if (strcmp(msgstr, buf2) == 0)
                ishelo = 1;
            else if (strcmp(buf, buf2) == 0)
                ishelo = 1;
#endif

    }
    else if (mode == 0)
    {
        sprintf(msgbuf, "[0;1;5;44;33mÕ¾³¤ ÓÚ[36m %-25.25s [33m¹ã²¥£º[m\n[1;37;44m%s[m\n", timestr, msgstr);
    }
    else if (mode == 1)
    {
        sprintf(msgbuf, "[0;1;44;36m%-12.12s[37m([36m%s[37m)"
                "[m\n[0;1;44;37mÑûÇëÄã[37m%-48.48s[31m(^Z)[m[\n",
                currentuser.userid, timestr, msgstr);
    }
    else if (mode == 3)
    {
        sprintf(msgbuf, "[0;1;45;36m%-12.12s[33m([36m%s[33m):[m\n%s\n", currentuser.userid, timestr, (msgstr == NULL) ? buf : msgstr);
    }
    if (userpid)
    {
        if (userpid != uin->pid)
        {
            saveline(0, 0);	/* Save line */
            move(0, 0);
            clrtoeol();
            prints("[1m¶Ô·½ÒÑ¾­ÀëÏß...[m\n");
            sleep(1);
            saveline(0, 1);	/* restore line */
            return -1;
        }
    }
    if (!uin->active || kill(uin->pid, 0) == -1)
    {
        if (msgstr == NULL)
        {
            prints("\n¶Ô·½ÒÑ¾­ÀëÏß....\n");
            pressreturn();
            clear();
        }
        return -1;
    }

    sethomefile(buf, uident, "msgfile");
    file_append(buf, msgbuf);
    sethomefile(buf, uident, MSGDIR);
    if(msgstr == NULL || mode==2 || mode == 1 || mode ==3)
        append_msg(buf, currentuser.userid, msgbuf);
    else if(mode == 0)
        append_msg(buf, "" , msgbuf);

    //add by hace

#ifdef LOG_MY_MESG
    /*
     * 990610.edwardc ³ýÁËÎÒÖ±½ÓËÍÑ¶Ï¢¸øÄ³ÈËÍâ, ÆäËûÈç¹ã²¦¸øÕ¾ÉÏ
     * ²¦ºÃÓÑÒ»ÂÉ²»¼ÍÂ¼ .. :)
     */

    if (mode == 2 || (mode == 0 && msgstr == NULL))
    {
        if (ishelo == 0)
        {
            sethomefile(buf, currentuser.userid, "msgfile.me");
            file_append(buf, mymsg);
        }
    }
    sethomefile(buf, uident, "msgfile.me");
    file_append(buf, msgbuf);
    free(mymsg);

#endif

    free(msgbuf);
    if (uin->pid)
    {
		if(is_web_user(uin))
			/* ·¢ËÍ¸øwebÓÃ»§ */
			send_web_msg(uin->userid);
		else
			/* ·¢ËÍ¸øtelnetÓÃ»§ */
			kill(uin->pid, SIGUSR2);
    }
    if (msgstr == NULL)
    {
        prints("\nÒÑËÍ³öÑ¶Ï¢...\n");
        pressreturn();
        clear();
    }
    return 1;
}

int
dowall (uin)
struct user_info *uin;
{
    if (!uin->active || !uin->pid)
        return -1;
    move (1, 0);
    clrtoeol ();
    prints ("[1;32mÕý¶Ô %s ¹ã²¥.... Ctrl-D Í£Ö¹¶Ô´ËÎ» User ¹ã²¥¡£[m",
            uin->userid);
    refresh ();
    do_sendmsg (uin, buf2, 0, uin->pid);
}

int
myfriend_wall (uin)
struct user_info *uin;
{
    if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid
            || isreject (uin))
        return -1;
    if (myfriend (uin->uid))
    {
        move (1, 0);
        clrtoeol ();
        prints ("[1;32mÕýÔÚËÍÑ¶Ï¢¸ø %s...  [m", uin->userid);
        refresh ();
        do_sendmsg (uin, buf2, 3, uin->pid);
    }
}

int
hisfriend_wall (uin)
struct user_info *uin;
{
    if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid
            || isreject (uin))
        return -1;
    if (hisfriend (uin))
    {
        move (1, 0);
        clrtoeol ();
        prints ("[1;32mÕýÔÚËÍÑ¶Ï¢¸ø %s...  [m", uin->userid);
        refresh ();
        do_sendmsg (uin, buf2, 3, uin->pid);
    }
}

int
friend_wall ()
{
    char buf[3];
    char msgbuf[256], filename[80];
    time_t now;
    char *timestr;

    now = time (0);
    timestr = ctime (&now) + 11;
    *(timestr + 8) = '\0';

    /*
    	if (uinfo.invisible) {
    		move(2, 0);
    		prints("±§Ç¸, ´Ë¹¦ÄÜÔÚÒþÉí×´Ì¬ÏÂ²»ÄÜÖ´ÐÐ...\n");
    		pressreturn();
    		return 0;
    	}
    */
    modify_user_mode (MSG);
    move (2, 0);
    clrtobot ();
    getdata (1, 0, "ËÍÑ¶Ï¢¸ø [1] ÎÒµÄºÃÅóÓÑ£¬[2] ÓëÎÒÎªÓÑÕß: ",
             buf, 2, DOECHO, YEA);
    switch (buf[0])
    {
    case '1':
        if (!get_msg ("ÎÒµÄºÃÅóÓÑ", buf2, 1))
            return 0;
        if (apply_ulist (myfriend_wall) == -1)
        {
            move (2, 0);
            prints ("ÏßÉÏ¿ÕÎÞÒ»ÈË\n");
            pressanykey ();
        }
        else
        {
            /* ¼ÇÂ¼ËÍÑ¶Ï¢¸øºÃÓÑ */
            sprintf (msgbuf,
                     "[0;1;45;36mËÍÑ¶Ï¢¸øºÃÓÑ[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z»Ø)[m[%05dm\n",
                     timestr, buf2, uinfo.pid);
            setuserfile (filename, "msgfile.me");
            file_append (filename, msgbuf);
        }
        break;
    case '2':
        if (!get_msg ("ÓëÎÒÎªÓÑÕß", buf2, 1))
            return 0;
        if (apply_ulist (hisfriend_wall) == -1)
        {
            move (2, 0);
            prints ("ÏßÉÏ¿ÕÎÞÒ»ÈË\n");
            pressanykey ();
        }
        else
        {
            /* ¼ÇÂ¼ËÍÑ¶Ï¢¸øÓëÎÒÎªÓÑÕß */
            sprintf (msgbuf,
                     "[0;1;45;36mËÍ¸øÓëÎÒÎªÓÑ[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z»Ø)[m[%05dm\n",
                     timestr, buf2, uinfo.pid);
            setuserfile (filename, "msgfile.me");
            file_append (filename, msgbuf);

        }
        break;
    default:
        return 0;
    }
    move (6, 0);
    prints ("Ñ¶Ï¢´«ËÍÍê±Ï...");
    pressanykey ();
    return 1;
}
/*
void
r_msg2 ()
{
  FILE *fp;
  char buf[256];
  char msg[256];
  char fname[STRLEN];
  int line, tmpansi;
  int y, x, ch, i, j;
  int MsgNum;
  struct sigaction act;
 
  if (!strcmp (currentuser.userid, "guest"))
    return;
  getyx (&y, &x);
  if (uinfo.mode == TALK)
    line = t_lines / 2 - 1;
  else
    line = 0;
  setuserfile (fname, "msgfile");
  i = get_num_records (fname, 129);
  if (i == 0)
    return;
  signal (SIGUSR2, count_msg);
  tmpansi = showansi;
  showansi = 1;
  oflush ();
  if (RMSG == NA)
    {
      saveline (line, 0);
      saveline (line + 1, 2);
    }
  MsgNum = 0;
  RMSG = YEA;
  while (1)
    {
      MsgNum = (MsgNum % i);
      if ((fp = fopen (fname, "r")) == NULL)
	{
	  RMSG = NA;
	  if (msg_num)
	    r_msg ();
	  else
	    {
	      sigemptyset (&act.sa_mask);
	      act.sa_flags = SA_NODEFER;
	      act.sa_handler = r_msg;
	      sigaction (SIGUSR2, &act, NULL);
	    }
	  return;
	}
      for (j = 0; j < (i - MsgNum); j++)
	{
	  if (fgets (buf, 256, fp) == NULL)
	    break;
	  else
	    strcpy (msg, buf);
	}
      fclose (fp);
      move (line, 0);
      clrtoeol ();
      prints ("%s", msg);
      refresh ();
      {
	struct user_info *uin;
	char msgbuf[STRLEN];
	int good_id, send_pid;
	char *ptr, usid[STRLEN];
 
	ptr = strrchr (msg, '[');
	send_pid = atoi (ptr + 1);
	ptr = strtok (msg + 12, " [");
	if (ptr == NULL)
	  good_id = NA;
	else if (!strcmp (ptr, currentuser.userid))
	  good_id = NA;
	else
	  {
	    strcpy (usid, ptr);
	    uin = t_search (usid, send_pid);
	    if (uin == NULL)
	      good_id = NA;
	    else
	      good_id = YEA;
	  }
	if (good_id == YEA && canmsg (uin))
	  {
	    int userpid;
 
	    userpid = uin->pid;
	    move (line + 1, 0);
	    clrtoeol ();
	    sprintf (msgbuf, "»ØÑ¶Ï¢¸ø %s: ", usid);
	    getdata (line + 1, 0, msgbuf, buf, 55, DOECHO, YEA);
	    if (buf[0] == Ctrl ('Z'))
	      {
		MsgNum++;
		continue;
	      }
	    else if (buf[0] == Ctrl ('A'))
	      {
		MsgNum--;
		if (MsgNum < 0)
		  MsgNum = i - 1;
		continue;
	      }
	    if (buf[0] != '\0')
	      {
		if (do_sendmsg (uin, buf, 2, userpid) == 1)
		  sprintf (msgbuf, "[1;32m°ïÄãËÍ³öÑ¶Ï¢¸ø %s ÁË![m", usid);
		else
		  sprintf (msgbuf, "[1;32mÑ¶Ï¢ÎÞ·¨ËÍ³ö.[m");
	      }
	    else
	      sprintf (msgbuf, "[1;33m¿ÕÑ¶Ï¢, ËùÒÔ²»ËÍ³ö.[m");
	    move (line + 1, 0);
	    clrtoeol ();
	    refresh ();
	    prints ("%s", msgbuf);
	    refresh ();
	    if (!strstr (msgbuf, "°ïÄã"))
	      sleep (1);
	  }
	else
	  {
	    sprintf (msgbuf, "[1;32mÕÒ²»µ½·¢Ñ¶Ï¢µÄ %s! Çë°´ÉÏ:[^Z ¡ü] "
		     "»òÏÂ:[^A ¡ý] »òÆäËû¼üÀë¿ª.[m", usid);
	    move (line + 1, 0);
	    clrtoeol ();
	    refresh ();
	    prints ("%s", msgbuf);
	    refresh ();
	    if ((ch = igetkey ()) == Ctrl ('Z') || ch == KEY_UP)
	      {
		MsgNum++;
		continue;
	      }
	    if (ch == Ctrl ('A') || ch == KEY_DOWN)
	      {
		MsgNum--;
		if (MsgNum < 0)
		  MsgNum = i - 1;
		continue;
	      }
	  }
      }
      break;
    }
  saveline (line, 1);
  saveline (line + 1, 3);
  showansi = tmpansi;
  move (y, x);
  refresh ();
  RMSG = NA;
  if (msg_num)
    {
      msg_num--;
      r_msg ();
    }
  else
    {
      sigemptyset (&act.sa_mask);
      act.sa_flags = SA_NODEFER;
      act.sa_handler = r_msg;
      sigaction (SIGUSR2, &act, NULL);
    }
  return;
}
*/
void r_msg2()
{
    FILE *fp;
    char buf[MAX_MSG_SIZE];
    struct msgdir md;
    char msg[MAX_MSG_SIZE];
    char fname[STRLEN];
    int line, tmpansi;
    int y, x, ch, i, j;
    int MsgNum;
    struct sigaction act;
    int msgline;

    if (!strcmp(currentuser.userid, "guest"))
        return;
    if(!HAS_PERM(PERM_MESSAGE))
        return;
    memset(msg,0,MAX_MSG_SIZE);
    getyx(&y, &x);
    if (uinfo.mode == TALK)
        line = t_lines / 2 - 1;
    else
        line = 0;
    //add by hace
    setuserfile(fname, MSGDIR);
    if (RMSG == NA)
    {
        for(j=0;j<48;j+=2)
            //			saveline(line + j/2, j);
            saveline(j/2, j);
    }
    i = get_num_records(fname, sizeof(struct msgdir));
    if (i == 0)
        return;
    signal(SIGUSR2, count_msg);

    tmpansi = showansi;
    showansi = 1;
    oflush();
    //	saveline(line, 0);
    //	saveline(line + 1, 2);

    MsgNum = 0;
    RMSG = YEA;
    while (1)
    {
        MsgNum = (MsgNum % i);
        if ((fp = fopen(fname, "r")) == NULL)
        {
            RMSG = NA;
            if (msg_num)
                r_msg();
            else
            {
                sigemptyset(&act.sa_mask);
                act.sa_flags = SA_NODEFER;
                act.sa_handler = r_msg;
                sigaction(SIGUSR2, &act, NULL);
            }
            return;
        }
        for (j = 0; j < (i - MsgNum); j++)
        {
            /*
            			if (fgets(buf, 256, fp) == NULL)
            				break;
            			else
            				strcpy(msg, buf);
            */
            if (fread(&md,sizeof(md),1,fp) == 0 )
                break;
            else
                strcpy(msg, md.msg);
        }
        fclose(fp);
        msgline=count_msgline(msg);
        move(line, 0);
        clrtoeol();

        //lock acboard while msging
        uinfo.IsMsg = 1;
        prints("%s\n", msg);
        refresh();
        {
            struct user_info *uin;
            char msgbuf[STRLEN];
            int good_id, send_pid;
            char usid[IDLEN+2];

            send_pid = md.pid;

            if (!strcmp(md.userid,""))
                good_id = NA;
            else if (!strcmp(md.userid, currentuser.userid))
                good_id = NA;
            else
            {
                strcpy(usid, md.userid);
                uin = t_search(usid, send_pid);
                if (uin == NULL)
                    good_id = NA;
                else
                    good_id = YEA;
            }
            if (good_id == YEA && canmsg(uin))
            {
                int userpid;

                userpid = uin->pid;
                move(line + msgline, 0);
                clrtoeol();
                refresh();
                prints("[1;36m»ØÑ¶Ï¢¸ø [1;33m%s  ([1;44;31mCtrl Q »»ÐÐ »Ø³µ½áÊø[m[1;33m)[m\n", usid);
                ch = multi_getdata(line + msgline+1, 0, EACH_LINE_LENGTH, NULL, buf, 1024, 5, 1);
                if (buf[0] == Ctrl('Z') || ch == Ctrl('Z'))
                {
                    MsgNum++;
                    // 					for(j=msgline; j <=24; j++)
                    for(j=0; j <=24; j++)
                        //						saveline(line+j,2*j+1);
                        saveline(j,2*j+1);
                    continue;
                }
                else if (buf[0] == Ctrl('A'))
                {
                    MsgNum--;
                    if (MsgNum < 0)
                        MsgNum = i - 1;
                    continue;
                }
                if (buf[0] != '\0')
                {
                    if (do_sendmsg(uin, buf, 2, userpid) == 1)
                        sprintf(msgbuf, "[1;32m°ïÄãËÍ³öÑ¶Ï¢¸ø %s ÁË! ", usid);
                    else
                        sprintf(msgbuf, "[1;32mÑ¶Ï¢ÎÞ·¨ËÍ³ö. ");

                }
                else
                    sprintf(msgbuf, "[1;33m¿ÕÑ¶Ï¢, ËùÒÔ²»ËÍ³ö.");
                move(line + msgline +count_msgline(buf)+2,0);
                clrtoeol();
                refresh();
                //				prints("%s", msgbuf);
                prints("%s°´ÈÎÒâ¼ü¼ÌÐø...[m",msgbuf);
                if (!strstr(msgbuf, "°ïÄã"))
                    igetkey();
                refresh();
                /*
                if (!strstr(msgbuf, "°ïÄã"))
                	sleep(1);
                */
            }
            else
            {
                move(line + msgline, 0);
                sprintf(msgbuf, "[1;32mÕÒ²»µ½·¢Ñ¶Ï¢µÄ %s! Çë°´ÉÏ:[^Z ¡ü] »òÏÂ:[^A ¡ý] »òÆäËü¼üÀë¿ª[m", usid);
                clrtoeol();
                refresh();
                prints("%s", msgbuf);
                refresh();
                if ((ch = igetkey()) == Ctrl('Z') || ch == KEY_UP)
                {
                    MsgNum++;
                    continue;
                }
                if (ch == Ctrl('A') || ch == KEY_DOWN)
                {
                    MsgNum--;
                    if (MsgNum < 0)
                        MsgNum = i - 1;
                    continue;
                }
            }
        }
        break;
    }
    //saveline(line, 1);
    //saveline(line + 1, 3);
    for(j=1;j<=48;j+=2)
        //		saveline(line + (j-1)/2, j);
        saveline((j-1)/2, j);
    showansi = tmpansi;
    move(y, x);
    refresh();

    RMSG = NA;
    if (msg_num)
        r_msg();
    else
    {
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_NODEFER;
        act.sa_handler = r_msg;
        sigaction(SIGUSR2, &act, NULL);
    }
    //unlock acboard while msging
    uinfo.IsMsg = 0;
    return;
}
void
count_msg ()
{
    signal (SIGUSR2, count_msg);
    msg_num++;
}
/*
void
r_msg ()
{
  FILE *fp;
  char buf[256];
  char msg[256];
  char fname[STRLEN];
  int line, tmpansi;
  int y, x, i, j, premsg;
  char ch;
  struct sigaction act;
 
  signal (SIGUSR2, count_msg);
  msg_num++;
  getyx (&y, &x);
  tmpansi = showansi;
  showansi = 1;
  if (uinfo.mode == TALK)
    line = t_lines / 2 - 1;
  else
    line = 0;
  if (DEFINE (DEF_MSGGETKEY))
    {
      oflush ();
      saveline (line, 0);
      premsg = RMSG;
    }
  while (msg_num)
    {
      if (DEFINE (DEF_SOUNDMSG))
	{
	  bell ();
	}
      setuserfile (fname, "msgfile");
      i = get_num_records (fname, 129);
      if ((fp = fopen (fname, "r")) == NULL)
	{
	  sigemptyset (&act.sa_mask);
	  act.sa_flags = SA_NODEFER;
	  act.sa_handler = r_msg;
	  sigaction (SIGUSR2, &act, NULL);
	  return;
	}
      for (j = 0; j <= (i - msg_num); j++)
	{
	  if (fgets (buf, 256, fp) == NULL)
	    break;
	  else
	    strcpy (msg, buf);
	}
      fclose (fp);
      move (line, 0);
      clrtoeol ();
      prints ("%s", msg);
      refresh ();
      msg_num--;
      if (DEFINE (DEF_MSGGETKEY))
	{
	  RMSG = YEA;
	  ch = 0;
#ifdef MSG_CANCLE_BY_CTRL_C
	  while (ch != Ctrl ('C'))
#else
	  while (ch != '\r' && ch != '\n')
#endif
	    {
	      ch = igetkey ();
#ifdef MSG_CANCLE_BY_CTRL_C
	      if (ch == Ctrl ('C'))
		break;
#else
	      if (ch == '\r' || ch == '\n')
		break;
#endif
	      else if (ch == Ctrl ('R') || ch == 'R' || ch == 'r'
		       || ch == Ctrl ('Z'))
		{
		  struct user_info *uin;
		  char msgbuf[STRLEN];
		  int good_id, send_pid;
		  char *ptr, usid[STRLEN];
 
		  ptr = strrchr (msg, '[');
		  send_pid = atoi (ptr + 1);
		  ptr = strtok (msg + 12, " [");
		  if (ptr == NULL)
		    good_id = NA;
		  else if (!strcmp (ptr, currentuser.userid))
		    good_id = NA;
		  else
		    {
		      strcpy (usid, ptr);
		      uin = t_search (usid, send_pid);
		      if (uin == NULL)
			good_id = NA;
		      else
			good_id = YEA;
		    }
		  oflush ();
		  saveline (line + 1, 2);
		  if (good_id == YEA)
		    {
		      int userpid;
 
		      userpid = uin->pid;
		      move (line + 1, 0);
		      clrtoeol ();
		      sprintf (msgbuf, "Á¢¼´»ØÑ¶Ï¢¸ø %s: ", usid);
		      getdata (line + 1, 0, msgbuf, buf, 55, DOECHO, YEA);
		      if (buf[0] != '\0' && buf[0] != Ctrl ('Z')
			  && buf[0] != Ctrl ('A'))
			{
			  if (do_sendmsg (uin, buf, 2, userpid))
			    sprintf (msgbuf,
				     "[1;32m°ïÄãËÍ³öÑ¶Ï¢¸ø %s ÁË![m", usid);
			  else
			    sprintf (msgbuf, "[1;32mÑ¶Ï¢ÎÞ·¨ËÍ³ö.[m");
			}
		      else
			sprintf (msgbuf, "[1;33m¿ÕÑ¶Ï¢, ËùÒÔ²»ËÍ³ö. [m");
		    }
		  else
		    {
		      sprintf (msgbuf, "[1;32mÕÒ²»µ½·¢Ñ¶Ï¢µÄ %s.[m", usid);
		    }
		  move (line + 1, 0);
		  clrtoeol ();
		  refresh ();
		  prints ("%s", msgbuf);
		  refresh ();
		  if (!strstr (msgbuf, "°ïÄã"))
		    sleep (1);
		  saveline (line + 1, 3);
		  refresh ();
		  break;
		}		
	    }			
	}			
    }				
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_NODEFER;
  act.sa_handler = r_msg;
  sigaction (SIGUSR2, &act, NULL);
  if (DEFINE (DEF_MSGGETKEY))
    {
      RMSG = premsg;
      saveline (line, 1);
    }
  showansi = tmpansi;
  move (y, x);
  refresh ();
  return;
}
*/
void r_msg()
{
    FILE *fp;
    char buf[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    char fname[STRLEN];
    int line, tmpansi;
    int y, x, i, j, premsg;
    char ch;
    struct sigaction act;
    struct msgdir md;
    int msgline;

    signal(SIGUSR2, count_msg);
    msg_num++;
    getyx(&y, &x);
    tmpansi = showansi;
    showansi = 1;
    if (uinfo.mode == TALK)
        line = t_lines / 2 - 1;
    else
        line = 0;
    if (DEFINE(DEF_MSGGETKEY))
    {
        oflush();
        //saveline(line, 0);
        for(i=0;i<48;i+=2)
            //			saveline(line + i/2, i);
            saveline(i/2, i);
        premsg = RMSG;
    }

    if(msg_num)
        uinfo.IsMsg = 1;

    while (msg_num)
    {
        if (DEFINE(DEF_SOUNDMSG))
        {
            bell();
        }
        setuserfile(fname, MSGDIR);
        i = get_num_records(fname, sizeof(struct msgdir));
        if ((fp = fopen(fname, "r")) == NULL)
        {
            sigemptyset(&act.sa_mask);
            act.sa_flags = SA_NODEFER;
            act.sa_handler = r_msg;
            sigaction(SIGUSR2, &act, NULL);
            return;
        }
        for (j = 0; j <= (i - msg_num); j++)
        {
            /*
            if (fgets(buf, 256, fp) == NULL)
            	break;
            else
            	strcpy(msg, buf);
            */
            if (fread(&md,sizeof(md),1,fp) == 0)
                break;
            else
                strncpy(msg,md.msg,MAX_MSG_SIZE);
        }
        fclose(fp);
        msgline=count_msgline(msg);
        move(line, 0);
        clrtoeol();
        prints("%s", msg);
        refresh();
        msg_num--;
        if (DEFINE(DEF_MSGGETKEY))
        {
            RMSG = YEA;
            ch = 0;
#ifdef MSG_CANCLE_BY_CTRL_C

            while (ch != Ctrl('C'))
#else

            while (ch != '\r' && ch != '\n')
#endif

            {
                ch = igetkey();
#ifdef MSG_CANCLE_BY_CTRL_C

                if (ch == Ctrl('C'))
                    break;
#else

                if (ch == '\r' || ch == '\n')
                {
                    for(j=0; j <=24; j++)
                        //						saveline(line+j,2*j+1);
                        saveline(j,2*j+1);
                    break;
                }
#endif
                else if (ch == Ctrl('R') || ch == 'R' || ch == 'r' || ch == Ctrl('Z'))
                {
                    struct user_info *uin;
                    char msgbuf[STRLEN];
                    int good_id, send_pid;
                    char usid[IDLEN+2];

                    //ptr = strrchr(msg, '[');
                    //send_pid = atoi(ptr + 1);
                    //ptr = strtok(msg + 12, " [");
                    send_pid = md.pid;
                    if (md.userid[0]==0)
                        good_id = NA;
                    else if (!strcmp(md.userid, currentuser.userid))
                        good_id = NA;
                    else
                    {
                        strcpy(usid, md.userid);
                        uin = t_search(usid, send_pid);
                        if (uin == NULL)
                            good_id = NA;
                        else
                            good_id = YEA;
                    }
                    oflush();
                    //saveline(line + 1, 2);
                    if (good_id == YEA)
                    {
                        int userpid;

                        userpid = uin->pid;
                        move(line + msgline, 0);
                        clrtoeol();
                        refresh();
                        //modified by rwzmm@sjtubbs
                        //lock acboard while msging
                        uinfo.IsMsg = 1;
                        prints("[1;36mÁ¢¼´»ØÑ¶Ï¢¸ø [1;33m%s  ([1;44;31mCtrl Q »»ÐÐ£¬»Ø³µ½áÊø[m[1;33m)[m\n", usid);
                        //getdata(line + msgline , 0, msgbuf, buf, 55, DOECHO, YEA);
                        multi_getdata(line + msgline+ 1, 0, EACH_LINE_LENGTH, NULL, buf, 1024, 5, 1);
                        if (buf[0] != '\0' && buf[0] != Ctrl('Z') && buf[0] != Ctrl('A'))
                        {
                            if (do_sendmsg(uin, buf, 2, userpid))
                                sprintf(msgbuf, "[1;32m°ïÄãËÍ³öÑ¶Ï¢¸ø %s ÁË!", usid);
                            else
                                sprintf(msgbuf, "[1;32mÑ¶Ï¢ÎÞ·¨ËÍ³ö.");
                        }
                        else
                            sprintf(msgbuf, "[1;33m¿ÕÑ¶Ï¢, ËùÒÔ²»ËÍ³ö.");
                    }
                    else
                    {
                        sprintf(msgbuf, "[1;32mÕÒ²»µ½·¢Ñ¶Ï¢µÄ %d.", usid);
                    }
                    move(line + msgline + count_msgline(buf)+2, 0);
                    //move(0,0);
                    clrtoeol();
                    refresh();
                    prints("%s°´ÈÎÒâ¼ü¼ÌÐø...[m", msgbuf);

                    for(j=0; j <=24; j++)
                        //						saveline(line+j,2*j+1);
                        saveline(j,2*j+1);

                    refresh();
                    /*
                    if (!strstr(msgbuf, "°ïÄã"))
                    	sleep(1);
                    saveline(line + 1, 3);
                    */
                    if (!strstr(msgbuf, "°ïÄã"))
                        igetkey();
                    refresh();
                    break;
                }	/* if */
            }	/* while */
        }		/* if */
    }			/* while */

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = r_msg;
    sigaction(SIGUSR2, &act, NULL);

    if (DEFINE(DEF_MSGGETKEY))
    {
        RMSG = premsg;
        //	saveline(line, 1);
        for(i=1;i<48;i+=2)
            //			saveline(line + (i-1)/2, i);
            saveline((i-1)/2, i);
    }
    showansi = tmpansi;

    //unlock acboard while end msging
    uinfo.IsMsg = 0;

    move(y, x);
    refresh();

    return;
}
int
friend_login_wall (pageinfo)
struct user_info *pageinfo;
{
    char msg[STRLEN];
    int x, y;

    if (!pageinfo->active || !pageinfo->pid || isreject (pageinfo))
        return 0;
    if (hisfriend (pageinfo))
    {
        if (getuser (pageinfo->userid) <= 0)
            return 0;
        if (!(lookupuser.userdefine & DEF_LOGINFROM))
            return 0;
        if (pageinfo->uid == usernum)
            return 0;
        /* edwardc.990427 ºÃÓÑÒþÉí¾Í²»ÏÔÊ¾ËÍ³öÉÏÕ¾Í¨Öª */
        if (pageinfo->invisible)
            return 0;
        getyx (&y, &x);
        if (y > 22)
        {
            pressanykey ();
            move (7, 0);
            clrtobot ();
        }
        prints ("ËÍ³öºÃÓÑÉÏÕ¾Í¨Öª¸ø %s\n", pageinfo->userid);
        sprintf (msg, "ÄãµÄºÃÅóÓÑ %s ÒÑ¾­ÉÏÕ¾ÂÞ£¡", currentuser.userid);
        do_sendmsg (pageinfo, msg, 2, pageinfo->pid);
    }
    return 0;
}

int append_msg(char *path, char *uid, char *msg)
{
    struct msgdir m;
    int fd;
    if ((fd=open(path, O_WRONLY | O_CREAT, 0644)) == -1)
    {
        report("open file error in append_msg()");
        return -1;
    }
    memset(&m,0,sizeof(m));
    strncpy(m.userid,uid,IDLEN+2);
    m.pid=getpid();
    m.tt=time(NULL);
    strncpy(m.msg,msg,MAX_MSG_SIZE);
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    if (safewrite(fd, &m, sizeof(m)) == -1)
        report("apprec write err!");
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int count_msgline(char *msg)
{
    char *p=msg;
    int i=0;
    while(*p++)
    {
        if(*p=='\n')
            i++;
    }
    return i;
}
