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
$Id: delete.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
*/

#include "bbs.h"

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

void mail_info (char *lastword);

int
offline ()
{
    int i;
    char buf[STRLEN], lastword[640];

    modify_user_mode (OFFLINE);
    clear ();
    if (HAS_PERM (PERM_SYSOP) || HAS_PERM (PERM_BOARDS) || HAS_PERM (PERM_BLEVELS)
            || HAS_PERM (PERM_ADMINMENU) || HAS_PERM (PERM_SEEULEVELS) 
			|| HAS_PERM(PERM_XEMPT)  || HAS_PERM(PERM_SPECIAL6) 
			|| HAS_PERM(PERM_SPECIAL7) || HAS_PERM(PERM_SPECIAL8))
    {
        move (1, 0);
        prints ("\n\n您有重任在身, 不能随便自杀啦!!\n");
        pressreturn ();
        clear ();
        return;
    }
    if (currentuser.stay < 86400)
    {
        move (1, 0);
        prints ("\n\n对不起, 您还未够资格执行此命令!!\n");
        prints ("请 mail 给 SYSOP 说明自杀原因, 谢谢。\n");
        pressreturn ();
        clear ();
        return;
    }
    getdata (1, 0, "请输入你的密码: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
        prints ("\n\n很抱歉, 您输入的密码不正确。\n");
        pressreturn ();
        clear ();
        return;
    }
    getdata (3, 0, "请问你叫什麽名字? ", buf, NAMELEN, DOECHO, YEA);
    if (*buf == '\0' || strcmp (buf, currentuser.realname))
    {
        prints ("\n\n很抱歉, 我并不认识你。\n");
        pressreturn ();
        clear ();
        return;
    }
    clear ();
    move (1, 0);
    prints ("[1;5;31m警告[0;1;31m： 自杀後, 您将无法再用此帐号进入本站！！");
    move (3, 0);
    prints ("[1;32m但帐号要在 30 天後才会删除。好难过喔 :( .....[m");
    move (5, 0);
    i = 0;
    if (askyn ("真是舍不得你，你走之前有什么话想说么", YEA, NA) == YEA)
    {
        strcpy (lastword, ">\n>");
        buf[0] = '\0';
        for (i = 0; i < 8; i++)
        {
            getdata (i + 6, 0, ": ", buf, 77, DOECHO, YEA);
            if (buf[0] == '\0')
                break;
            strcat (lastword, buf);
            strcat (lastword, "\n>");
        }
        if (i == 0)
            lastword[0] = '\0';
        else
            strcat (lastword, "\n\n");
        move (i + 8, 0);
        if (i == 0)
            prints ("哎，你还是什么都不愿意说，是不是还有心思未了？");
        else if (i <= 4)
            prints ("看着你憔悴的脸，我心都碎了 ... ");
        else
            prints
            ("我会记得你的，朋友，我也知道你的离开也是没有办法的事，好走了");
    }
    move (i + 10, 0);
    if (askyn ("你确定要离开这个大家庭", NA, NA) == 1)
    {
        clear ();
        currentuser.userlevel = 0;
        substitute_record (PASSFILE, &currentuser, sizeof (struct userec),
                           usernum);
        mail_info (lastword);
        modify_user_mode (OFFLINE);
        kick_user (&uinfo);
        exit (0);
    }
}

getuinfo (fn)
FILE *fn;
{
    int num;
    char buf[40];

    fprintf (fn, "\n\n他的代号     : %s\n", currentuser.userid);
    fprintf (fn, "他的昵称     : %s\n", currentuser.username);
    fprintf (fn, "真实姓名     : %s\n", currentuser.realname);
    fprintf (fn, "居住住址     : %s\n", currentuser.address);
    fprintf (fn, "电子邮件信箱 : %s\n", currentuser.email);
    fprintf (fn, "真实 E-mail  : %s\n", currentuser.reginfo);
    fprintf (fn, "Ident 资料   : %s\n", currentuser.ident);
    getdatestring (currentuser.firstlogin, NA);
    fprintf (fn, "帐号建立日期 : %s\n", datestring);
    getdatestring (currentuser.lastlogin, NA);
    fprintf (fn, "最近光临日期 : %s\n", datestring);
    fprintf (fn, "最近光临机器 : %s\n", currentuser.lasthost);
    fprintf (fn, "上站次数     : %d 次\n", currentuser.numlogins);
    fprintf (fn, "文章数目     : %d\n", currentuser.numposts);
    fprintf (fn, "上站总时数   : %d 小时 %d 分钟\n",
             currentuser.stay / 3600, (currentuser.stay / 60) % 60);
    strcpy (buf, "bTCPRD#@XWBA#VS-DOM-F0123456789t");
    for (num = 0; num < 32; num++)
        if (!(currentuser.userlevel & (1 << num)))
            buf[num] = '-';
    buf[num] = '\0';
    fprintf (fn, "使用者权限   : %s\n\n", buf);
}

void
mail_info (char *lastword)
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now = time (0);
    getdatestring (now, NA);
    sprintf (filename, "tmp/suicide.%s", currentuser.userid);
    if ((fn = fopen (filename, "w")) != NULL)
    {
        fprintf (fn,
                 "[1m%s[m 已经在 [1m%s[m 登记自杀了，以下是他的资料，请保留...",
                 currentuser.userid, datestring);
        getuinfo (fn);
        fclose (fn);
        Postfile (filename, "syssecurity", "登记自杀通知(30天後生效)...", 2);
        unlink (filename);
    }
    if ((fn = fopen (filename, "w")) != NULL)
    {
        fprintf (fn, "大家好,\n\n");
        fprintf (fn, "我是 %s (%s)。我己经决定在 30 天后离开这里了。\n\n",
                 currentuser.userid, currentuser.username);
        getdatestring (currentuser.firstlogin, NA);
        fprintf (fn,
                 "自 %14.14s 至今，我已经来此 %d 次了，在这总计 %d 分钟的网络生命中，\n",
                 datestring, currentuser.numlogins, currentuser.stay / 60);
        fprintf (fn,
                 "我又如何会轻易舍弃呢？但是我得走了...  点点滴滴－－尽在我心中！\n\n");
        fprintf (fn, "%s", lastword);
        fprintf (fn,
                 "朋友们，请把 %s 从你们的好友名单中拿掉吧。因为我己经决定离开这里了!\n\n",
                 currentuser.userid);
        fprintf (fn, "或许有朝一日我会回来的。 珍重!! 再见!!\n\n\n");
        getdatestring (now, NA);
        fprintf (fn, "%s 于 %s 留.\n\n", currentuser.userid, datestring);
        fclose (fn);
        Postfile (filename, "message", "轻轻的我走了...", 2);
        unlink (filename);
    }
}


/*
 
int
reclusion ()			//add by quickmouse
{
  int i;
  char buf[STRLEN], lastword[640];
  FILE *fn;
  char filename[STRLEN];
  time_t now;
 
  modify_user_mode (RECLUSION);
  clear ();
  if (HAS_PERM (PERM_SYSOP) || HAS_PERM (PERM_BOARDS)
      || HAS_PERM (PERM_ADMINMENU) || HAS_PERM (PERM_SEEULEVELS))
    {
      move (1, 0);
      prints ("\n\n您有重任在身, 不能归隐田园啦!!\n");
      pressreturn ();
      clear ();
      return;
    }
  if (!HAS_PERM (PERM_LOGINOK)
      || (time (0) - currentuser.firstlogin) / 86400 < 180)
    {
      move (1, 0);
      prints ("\n\n对不起, 您还未够资格执行此命令!!\n");
      pressreturn ();
 
      clear ();
      return;
    }
  getdata (1, 0, "请输入你的密码: ", buf, PASSLEN, NOECHO, YEA);
  if (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
      prints ("\n\n很抱歉, 您输入的密码不正确。\n");
      pressreturn ();
      clear ();
      return;
    }
  clear ();
  move (1, 0);
  prints
    ("归隐田园提供给使用者一个暂时离开BBS的方式，使用者的帐号将被系统保留一年。\n");
  prints ("但是，在设置归隐田园之后的30天内，用户无法登陆上站。\n");
  prints
    (";5;31m警告0;1;31m： 从现在起30天内，您将无法再用此帐号进入本站！！\n");
  prints (";32m并且站长也无法给您恢复m");
  move (5, 0);
  i = 0;
  if (askyn ("真是舍不得你，你归隐之前有什么话想说么", NA, NA) == YEA)
    {
      strcpy (lastword, ">\n>");
      buf[0] = '\0';
      for (i = 0; i < 8; i++)
	{
	  getdata (i + 6, 0, ": ", buf, 77, DOECHO, YEA);
	  if (buf[0] == '\0')
	    break;
	  strcat (lastword, buf);
	  strcat (lastword, "\n>");
	}
      if (i == 0)
	lastword[0] = '\0';
      else
	strcat (lastword, "\n\n");
      move (i + 8, 0);
      if (i == 0)
	prints ("哎，你还是什么都不愿意说，看来你真的累了");
      else if (i <= 4)
	prints ("看着你憔悴的脸，好好休息一下吧");
      else
	prints ("我会记得你的，朋友，记得早点儿回来");
    }
  move (i + 10, 0);
  if (askyn ("你确定要暂时离开这个大家庭", NA, NA) == 1)
    {
      clear ();
      currentuser.userlevel = currentuser.userlevel | PERM_RECLUSION;
      substitute_record (PASSFILE, &currentuser, sizeof (struct userec),
			 usernum);
      sprintf (filename, "tmp/reclusion.%s", currentuser.userid);
      if ((fn = fopen (filename, "w")) != NULL)
	{
	  fprintf (fn, "大家好,\n\n");
	  fprintf (fn,
		   "我是 %s (%s)。我决定为了自己的理想而归隐田园。\n\n",
		   currentuser.userid, currentuser.username);
	  fprintf (fn, "%s", lastword);
	  fprintf (fn, "等我修成正果，我就会回来的。 珍重!! \n\n\n");
#ifndef TIMECOUNTER
	  //time_t now;
	  getdatestring (now, NA);
#else
	  getnowdatestring ();
#endif
	  fprintf (fn, "%s 于 %s 留.\n\n", currentuser.userid, datestring);
	  fclose (fn);
	  Postfile (filename, "message", "登记归隐田园...", 2);
	  unlink (filename);
	}
      modify_user_mode (OFFLINE);	// offline 指出是kick自己
      kick_user (&uinfo);
      exit (0);
    }
}
*/
