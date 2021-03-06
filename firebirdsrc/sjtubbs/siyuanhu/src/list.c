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
$Id: list.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
*/

#include "bbs.h"

//#ifndef DLM
//#undef  ALLOWGAME
//#endif

#define BBS_PAGESIZE    (t_lines - 4)
#define refreshtime     (30)
extern time_t login_start_time;
extern char BoardName[];
int (*func_list_show) ();
time_t update_time = 0;
int freshmode = 0;
int toggle1 = 0, toggle2 = 0;
int mailmode, numf;
int friendmode = 0;
int usercounter, real_user_names = 0;
int range, page, readplan, num;
int pmode;
struct user_info *user_record[MAXACTIVE];
struct userec *user_data;

// add by Flier - 2000.5.12 - Begin
enum sort_type
{ stUserID, stUserName, stIP, stState }
st = stUserID;

// add by Flier - 2000.5.12 - End

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

int
friend_search (uid, uentp, tblsize)
unsigned /*short*/int uid;
struct user_info *uentp;
int tblsize;
{
    int hi, low, mid;
    int cmp;

    if (uid == 0)
    {
        return NA;
    }
    hi = tblsize - 1;
    low = 0;
    while (low <= hi)
    {
        mid = (low + hi) / 2;
        cmp = uentp->friend[mid] - uid;
        if (cmp == 0)
        {
            return YEA;
        }
        if (cmp > 0)
            hi = mid - 1;
        else
            low = mid + 1;
    }
    return NA;
}

int
myfriend (uid)
unsigned /*short*/int uid;
{
    return friend_search (uid, &uinfo, uinfo.fnum);
}

int
hisfriend (uentp)
struct user_info *uentp;
{
    return friend_search (uinfo.uid, uentp, uentp->fnum);
}

int
isreject (struct user_info *uentp)
{
    int i;

    if (HAS_PERM (PERM_SYSOP))
        return NA;
    if (uentp->uid != uinfo.uid)
    {
        for (i = 0;
                i < MAXREJECTS && (uentp->reject[i] || uinfo.reject[i]); i++)
        {
            if (uentp->reject[i] == uinfo.uid || uentp->uid == uinfo.reject[i])
                return YEA;		/* 被设为黑名单 */
        }
    }
    return NA;
}

print_title ()
{

    docmdtitle ((friendmode) ? "[好朋友列表]" : "[使用者列表]",
                " 聊天[[1;32mt[m] 寄信[[1;32mm[m] 送讯息[[1;32ms[m] 加,减朋友[[1;32mo[m,[1;32md[m] 看说明档[[1;32m→[m,[1;32mRtn[m] 切换模式 [[1;32mf[m] 求救[[1;32mh[m]");
    update_endline ();
}

print_title2 ()
{

    docmdtitle ((friendmode) ? "[好朋友列表]" : "[使用者列表]",
                "        寄信[[1;32mm[m] 加,减朋友[[1;32mo[m,[1;32md[m] 看说明档[[1;32m→[m,[1;32mRtn[m] 选择[[1;32m↑[m,[1;32m↓[m] 求救[[1;32mh[m]");
    update_endline ();
}

void
update_data ()
{
    if (readplan == YEA)
        return;
    if (time (0) >= update_time + refreshtime - 1)
    {
        freshmode = 1;
    }
    signal (SIGALRM, update_data);
    alarm (refreshtime);
    return;
}


int
print_user_info_title ()
{
    char title_str[512];
    char *field_2;

    move (2, 0);
    clrtoeol ();
    field_2 = "使用者昵称";
    if (real_user_names)
        field_2 = "真实姓名  ";
    sprintf (title_str,
             "[1;44m编号 %s使用者代号%s %s%s%s%8.8s %s上站的位置%s      P M %c%s目前动态%s %5s[m\n",
             (st == stUserID) ? "[32m{" : " ",
             (st == stUserID) ? "}[37m" : " ",
             (st == stUserName) ? "[32m{" : " ", field_2,
             (st == stUserName) ? "}[37m" : " ", " ",
             (st == stIP) ? "[32m{" : " ", (st == stIP) ? "}[37m" : " ",
             ((HAS_PERM (PERM_SYSOP | PERM_SEECLOAK | PERM_CLOAK)) ? 'C' :
              ' '), (st == stState) ? "[32m{" : " ",
             (st == stState) ? "}[37m" : " ",
#ifdef SHOW_IDLE_TIME
             "时:分");
#else
             ""
            );
#endif
    prints ("%s", title_str);
    return 0;
}

show_message (msg)
char msg[];
{

    move (BBS_PAGESIZE + 2, 0);
    clrtoeol ();
    if (msg != NULL)
        prints ("[1m%s[m", msg);
    refresh ();
}

void
swap_user_record (a, b)
int a, b;
{
    struct user_info *c;

    c = user_record[a];
    user_record[a] = user_record[b];
    user_record[b] = c;
}

// Add by Flier - 2000.5.12 - Begin
int
compare_user_record (left, right)
struct user_info *left, *right;
{
    int retCode;

    switch (st)
    {
    case stUserID:
        retCode = strcasecmp (left->userid, right->userid);
        break;
    case stUserName:
        retCode = strcasecmp (left->username, right->username);
        break;
    case stIP:
        retCode = strncmp (left->from, right->from, 20);
        break;
    case stState:
        retCode = left->mode - right->mode;
        break;
    }
    return retCode;
}

// Add by Filer - 2000.5.12 - End

void
sort_user_record (int left, int right)
{
    int i, last;

    if (left >= right)
        return;
    swap_user_record (left, (left + right) / 2);
    last = left;
    for (i = left + 1; i <= right; i++)
    {
        // Modified by Flier - 2000.5.12
        if (compare_user_record (user_record[i], user_record[left]) < 0)
        {
            swap_user_record (++last, i);
        }
    }
    swap_user_record (left, last);
    sort_user_record (left, last - 1);
    sort_user_record (last + 1, right);
}

int
fill_userlist ()
{
    register int i, n, totalusernum;
    int friendno[MAXACTIVE];
    extern struct UTMPFILE *utmpshm;

    resolve_utmp ();
    totalusernum = 0;
    numf = 0;
    for (i = 0; i < USHM_SIZE; i++)
    {
        if (!utmpshm->uinfo[i].active
                || !utmpshm->uinfo[i].pid || isreject (&utmpshm->uinfo[i]))
            continue;
        if ((utmpshm->uinfo[i].invisible)
                && (usernum != utmpshm->uinfo[i].uid)
                && (!HAS_PERM (PERM_SYSOP | PERM_SEECLOAK)))
            continue;
        if (myfriend (utmpshm->uinfo[i].uid))
        {
            friendno[numf++] = totalusernum;
        }
        else if (friendmode)
            continue;
        user_record[totalusernum++] = &utmpshm->uinfo[i];
    }
    if (!friendmode)
    {
        for (i = 0, n = 0; i < totalusernum; i++)
        {
            if (n >= numf)
                break;
            if (friendno[n] == i)
            {
                if (i != n)
                    swap_user_record (n, i);
                n++;
            }
        }
        if (numf >= 2)
        {
            sort_user_record (0, numf - 1);
        }
        else if (numf == 2)
        {
            if (compare_user_record (user_record[0], user_record[1]) < 0)
                swap_user_record (0, 1);
        }
        sort_user_record (numf, totalusernum - 1);
    }
    else
    {
        if (totalusernum >= 2)
        {
            sort_user_record (0, totalusernum - 1);
        }
        else if (totalusernum == 2)
        {
            if (compare_user_record (user_record[0], user_record[1]) < 0)
                swap_user_record (0, 1);
        }
    }
    range = totalusernum;
    return totalusernum == 0 ? -1 : 1;
}

int
do_userlist ()
{
    int i, j, override;
    char user_info_str[STRLEN * 2];
    struct user_info *uentp;

    move (3, 0);
    print_user_info_title ();
    for (i = 0, j = 0; j < BBS_PAGESIZE && i + page < range; i++)
    {
        uentp = user_record[i + page];
        override = (i + page < numf) || friendmode;
        if (readplan == YEA)
            return 0;
        if (uentp == NULL || !uentp->active || !uentp->pid)
        {			//by sunner
            continue;		/* 某人正巧离开 */
        }
        if (uentp != NULL)
        {			// by wujian 增加是否为对方好友的显示
            sprintf (user_info_str,
                     " [m%3d%2s%-12.22s[37m %-20.20s[m %-16.16s %c %c %c %s%-10.10s[37m %5.5s[m\n",
                     i + 1 + page,
#ifdef SHOWMETOFRIEND
                     (override) ? ((hisfriend (uentp)) ? "[32m√" :
                                   "√[32m") : ((hisfriend (uentp)) ?
                                                         "[33m  " : "  "),
#else
                     (override) ? "[32m√" : "  ",
#endif
                             uentp->userid,
                             (real_user_names) ? uentp->realname : uentp->username,
                             /* uentp->from, */
						  	 ((uentp->from[22] != 'H')||hisfriend(uentp)||HAS_PERM(PERM_SYSOP))? uentp->from:BBSIP,
                             (uentp->mode ==
                              BBSNET) ? '@' : pagerchar (hisfriend (uentp),
                                                         uentp->pager),
                             msgchar (uentp), (uentp->invisible == YEA) ? '@' : ' ',
                             (uentp->invisible == YEA) ? "[36m" : (uentp->mode ==
                                                                    POSTING) ?
                             "[32m" : ((uentp->mode == BBSNET) ? "[33m" : ""),
                             ModeType (uentp->mode),
#ifdef SHOW_IDLE_TIME
                             idle_str (uentp));
#else
                             ""
                            );
#endif
            clrtoeol ();
            prints ("%s", user_info_str);
            j++;
        }
    }
    return 0;
}

int
show_userlist ()
{
    if (update_time + refreshtime < time (0))
    {
        fill_userlist ();
        update_time = time (0);
    }
    if (range == 0 /* ||fill_userlist() == 0 */ )
    {
        move (2, 0);
        prints ("没有使用者（朋友）在列表中...\n");
        clrtobot ();
        if (friendmode)
        {
            move (BBS_PAGESIZE + 3, 0);
            if (askyn ("是否转换成使用者模式", YEA, NA) == YEA)
            {
                range = num_visible_users ();
                page = -1;
                friendmode = NA;
                return 1;
            }
        }
        else
            pressanykey ();
        return -1;
    }
    do_userlist ();
    clrtobot ();
    return 1;
}

int
deal_key (char ch, int allnum, int pagenum)	//环顾四方处理按键
{
    char buf[STRLEN], desc[5];
    static int msgflag;
    extern int friendflag;

    if (msgflag == YEA)
    {
        show_message (NULL);
        msgflag = NA;
    }
    switch (ch)
    {
    case 'Y':
            if (HAS_PERM (PERM_CLOAK))
                x_cloak ();
        break;
    case 'P':
        t_pager ();
        break;
    case 'C':
    case 'c':
        if (ch == 'C')
        {
            strcpy (genbuf, "变换昵称(不是临时变换)为: ");
        }
        else
        {
            strcpy (genbuf, "暂时变换昵称(最多10个汉字): ");
        }
        strcpy (buf, "");
        getdata (BBS_PAGESIZE + 3, 0, genbuf, buf,
                 (ch == 'C') ? NAMELEN : 21, DOECHO, NA);
        if (buf[0] != '\0')
        {
            strcpy (uinfo.username, buf);
            if (ch == 'C')
            {
                set_safe_record ();
                strcpy (currentuser.username, buf);
                substitute_record (PASSFILE, &currentuser,
                                   sizeof (currentuser), usernum);
            }
        }
        break;
    case 'k':
    case 'K':
        if (!HAS_PERM (PERM_SYSOP) && (usernum != user_record[allnum]->uid))
            return 1;
        if (!strcmp (currentuser.userid, "guest"))
            return 1;
#ifdef SPECIALID

        if (!strcmp (currentuser.userid, SPECIALID1)
                || !strcmp (currentuser.userid, SPECIALID2))
            return 1;
#endif

        if (user_record[allnum]->pid == uinfo.pid)
            strcpy (buf, "你自己要把【自己】踢出去吗");
        else
            sprintf (buf, "你要把 %s 踢出站外吗", user_record[allnum]->userid);
        move (BBS_PAGESIZE + 2, 0);
        if (askyn (buf, NA, NA) == NA)
            break;
        if (kick_user (user_record[allnum]) == 1)
        {
            sprintf (buf, "%s 已被踢出站外", user_record[allnum]->userid);
        }
        else
        {
            sprintf (buf, "%s 无法踢出站外", user_record[allnum]->userid);
        }
        user_record[allnum]->active = NA;
        update_time = 0;
        msgflag = YEA;
        break;
    case 'h':
    case 'H':
        show_help ("help/userlisthelp");
        break;
	case Ctrl('H'):
		sjtuschedule();
		break;
    case 't':
    case 'T':
        if (!HAS_PERM (PERM_PAGE))
            return 1;
        if (user_record[allnum]->uid != usernum)
            ttt_talk (user_record[allnum]);
        else
            return 1;
        break;
    case 'v':
    case 'V':
        if (!HAS_PERM (PERM_SYSOP))
            return 1;
        real_user_names = !real_user_names;
        break;
    case 'm':
    case 'M':
        if (!HAS_PERM (PERM_POST))
            return 1;
        m_send (user_record[allnum]->userid);
        break;
    case 'g':
    case 'G':
        if (!HAS_PERM (PERM_POST))
            return 1;
        sendGoodWish (user_record[allnum]->userid);
        break;
    case 'f':
    case 'F':
        if (friendmode)
            friendmode = NA;
        else
            friendmode = YEA;
        update_time = 0;
        break;
/*
    case 'x':
    case 'X':
        if (!Personal (user_record[allnum]->userid))
        {
            sprintf (buf, "%s 还没有申请个人文集", user_record[allnum]->userid);
            msgflag = YEA;
        }
        break;
*/
    case 's':
    case 'S':
        if (!strcmp ("guest", currentuser.userid))
            return 0;
        if (!HAS_PERM (PERM_MESSAGE))
            return 0;
        if (!canmsg (user_record[allnum]))
        {
            sprintf (buf, "%s 已经关闭讯息呼叫器", user_record[allnum]->userid);
            msgflag = YEA;
            break;
        }
        do_sendmsg (user_record[allnum], NULL, 0, user_record[allnum]->pid);
        break;
    case 'o':
    case 'O':
    case 'r':
    case 'R':
#ifdef SPECIALID

        if (!strcmp (currentuser.userid, SPECIALID1)
                || !strcmp (currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp ("guest", currentuser.userid))
            return 0;
        if (ch == 'o' || ch == 'O')
        {
            friendflag = YEA;
            strcpy (desc, "好友");
        }
        else
        {
            friendflag = NA;
            strcpy (desc, "坏人");
        }
        sprintf (buf, "确定要把 %s 加入%s名单吗",
                 user_record[allnum]->userid, desc);
        move (BBS_PAGESIZE + 2, 0);
        if (askyn (buf, NA, NA) == NA)
            break;
        if (addtooverride (user_record[allnum]->userid) == -1)
        {
            sprintf (buf, "%s 已在%s名单", user_record[allnum]->userid, desc);
        }
        else
        {
            sprintf (buf, "%s 列入%s名单", user_record[allnum]->userid, desc);
        }
        msgflag = YEA;
        break;
    case 'd':
    case 'D':
#ifdef SPECIALID

        if (!strcmp (currentuser.userid, SPECIALID1)
                || !strcmp (currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp ("guest", currentuser.userid))
            return 0;
        sprintf (buf, "确定要把 %s 从好友名单删除吗",
                 user_record[allnum]->userid);
        move (BBS_PAGESIZE + 2, 0);
        if (askyn (buf, NA, NA) == NA)
            break;
        if (deleteoverride (user_record[allnum]->userid, "friends") == -1)
        {
            sprintf (buf, "%s 本来就不在朋友名单中",
                     user_record[allnum]->userid);
        }
        else
        {
            sprintf (buf, "%s 已从朋友名单移除", user_record[allnum]->userid);
        }
        msgflag = YEA;
        break;
    default:
        return 0;
    }
    if (friendmode)
        modify_user_mode (FRIEND);
    else
        modify_user_mode (LUSERS);
    if (readplan == NA)
    {
        print_title ();
        clrtobot ();
        if (show_userlist () == -1)
            return -1;
        if (msgflag)
            show_message (buf);
        update_endline ();
    }
    return 1;
}

int
deal_key2 (char ch, int allnum, int pagenum)	//探视网友处理按键
{
    char buf[STRLEN];
    static int msgflag;

    if (msgflag == YEA)
    {
        show_message (NULL);
        msgflag = NA;
    }
    switch (ch)
    {
    case 'h':
    case 'H':
        show_help ("help/usershelp");
        break;
	case Ctrl('H'):
		sjtuschedule();
		break;
    case 'm':
    case 'M':
        if (!HAS_PERM (PERM_POST))
            return 1;
        m_send (user_data[allnum - pagenum].userid);
        break;
    case 'o':
    case 'O':
#ifdef SPECIALID

        if (!strcmp (currentuser.userid, SPECIALID1)
                || !strcmp (currentuser.userid, SPECIALID2))
            return 0;
#endif
        /*if (user_record[allnum]->pid == uinfo.pid)*/
        if (!strcmp ("guest", currentuser.userid))
            return 0;
        sprintf (buf, "确定要把 %s 加入好友名单吗",
                 user_data[allnum - pagenum].userid);
        move (BBS_PAGESIZE + 2, 0);
        if (askyn (buf, NA, NA) == NA)
            break;
        if (addtooverride (user_data[allnum - pagenum].userid) == -1)
        {
            sprintf (buf, "%s 已在朋友名单",
                     user_data[allnum - pagenum].userid);
            show_message (buf);
        }
        else
        {
            sprintf (buf, "%s 列入朋友名单",
                     user_data[allnum - pagenum].userid);
            show_message (buf);
        }
        msgflag = YEA;
        if (!friendmode)
            return 1;
        break;
    case 'f':
    case 'F':
        toggle1++;
        if (toggle1 >= 3)
            toggle1 = 0;
        break;
    case 't':
    case 'T':
        toggle2++;
//        if (toggle2 >= 2)
//          toggle2 = 0;
#ifdef ALLOWGAME
	 if (toggle2 >= 3) toggle2 = 0;
#else
	 if (toggle2 >= 2) toggle2 = 0;
#endif
        break;
    case 'd':
    case 'D':
#ifdef SPECIALID

        if (!strcmp (currentuser.userid, SPECIALID1)
                || !strcmp (currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp ("guest", currentuser.userid))
            return 0;
        sprintf (buf, "确定要把 %s 从好友名单删除吗",
                 user_data[allnum - pagenum].userid);
        move (BBS_PAGESIZE + 2, 0);
        if (askyn (buf, NA, NA) == NA)
            break;
        if (deleteoverride (user_data[allnum - pagenum].userid, "friends")
                == -1)
        {
            sprintf (buf, "%s 本来就不在朋友名单中",
                     user_data[allnum - pagenum].userid);
            show_message (buf);
        }
        else
        {
            sprintf (buf, "%s 已从朋友名单移除",
                     user_data[allnum - pagenum].userid);
            show_message (buf);
        }
        msgflag = YEA;
        if (!friendmode)
            return 1;
        break;
    default:
        return 0;
    }
    modify_user_mode (LAUSERS);
    if (readplan == NA)
    {
        print_title2 ();
        move (3, 0);
        clrtobot ();
        if (Show_Users () == -1)
            return -1;
        update_endline ();
    }
    redoscr ();
    return 1;
}

int
countusers (uentp)
struct userec *uentp;
{
    static int totalusers;
    char permstr[11];

    if (uentp == NULL)
    {
        int c = totalusers;

        totalusers = 0;
        return c;
    }
    if (uentp->numlogins != 0 && uleveltochar (permstr, uentp->userlevel) != 0)
        totalusers++;
    return 0;
}

printuent (uentp)
struct userec *uentp;
{
    static int i;
    char permstr[11];
    char msgstr[18];
    int override;

    if (uentp == NULL)
    {
        printutitle ();
        i = 0;
        return 0;
    }
    if (uentp->numlogins == 0 || uleveltochar (permstr, uentp->userlevel) == 0)
        return 0;
    if (i < page || i >= page + BBS_PAGESIZE || i >= range)
    {
        i++;
        if (i >= page + BBS_PAGESIZE || i >= range)
            return QUIT;
        else
            return 0;
    }
    uleveltochar (&permstr, uentp->userlevel);
    switch (toggle1)
    {
    case 0:
        getdatestring (uentp->lastlogin, NA);
        sprintf (msgstr, "%-.16s", datestring + 6);
        break;
    case 1:
        sprintf (msgstr, "%-.16s", uentp->lasthost);
        break;
    case 2:
    default:
        getdatestring (uentp->firstlogin, NA);
        sprintf (msgstr, "%-.14s", datestring);
        break;
    }
    user_data[i - page] = *uentp;
    override = myfriend (searchuser (uentp->userid));
    prints (" %5d%2s%s%-12s%s %-17s %6d %4d %10s %-16s\n", i + 1,
            (override) ? "√" : "",
            (override) ? "[1;32m" : "", uentp->userid, (override) ? "[m" : "",
#if defined(ACTS_REALNAMES)
            HAS_PERM (PERM_SYSOP) ? uentp->realname : uentp->username,
#else
            uentp->username,
#endif
//            (toggle2 == 0) ? (uentp->numlogins) : (uentp->numposts),
//            (toggle2 == 0) ? uentp->stay / 3600 : uentp->nummails,
#ifdef ALLOWGAME
		(toggle2 == 0) ? (uentp->numlogins) : (toggle2 == 1) ? (uentp->numposts) : (uentp->money),
		(toggle2 == 0) ? uentp->stay / 3600 : (toggle2 == 1) ? (uentp->medals) : uentp->nummails,
#else
                (toggle2 == 0) ? (uentp->numlogins) : (uentp->numposts),
                (toggle2 == 0) ? uentp->stay / 3600 : uentp->nummails,
#endif
            HAS_PERM (PERM_SEEULEVELS) ? permstr : "", msgstr);
    i++;
    usercounter++;
    return 0;
}

int
allusers ()
{
    countusers (NULL);
    if (apply_record (PASSFILE, countusers, sizeof (struct userec)) == -1)
    {
        return 0;
    }
    return countusers (NULL);
}

int
mailto (uentp)
struct userec *uentp;
{
    char filename[STRLEN];

    sprintf (filename, "tmp/mailall.%s", currentuser.userid);
    if ((uentp->userlevel == PERM_BASIC && mailmode == 1) ||
            (uentp->userlevel & PERM_POST && mailmode == 2) ||
            (uentp->userlevel & PERM_BOARDS && mailmode == 3) ||
            (uentp->userlevel & PERM_CHATCLOAK && mailmode == 4))
    {
        mail_file (filename, uentp->userid, save_title);
    }
    return 1;
}

mailtoall (mode)
int mode;
{

    mailmode = mode;
    if (apply_record (PASSFILE, mailto, sizeof (struct userec)) == -1)
    {
        prints ("No Users Exist");
        pressreturn ();
        return 0;
    }
    return;
}

Show_Users ()
{

    usercounter = 0;
    modify_user_mode (LAUSERS);
    printuent ((struct userec *) NULL);
    if (apply_record (PASSFILE, printuent, sizeof (struct userec)) == -1)
    {
        prints ("No Users Exist");
        pressreturn ();
        return 0;
    }
    clrtobot ();
    return 0;
}

setlistrange (i)
int i;
{
    range = i;
}


do_query (star, curr)
int star, curr;
{
    if (user_record[curr]->userid != NULL)
    {
        clear ();
        t_query (user_record[curr]->userid);
        move (t_lines - 1, 0);
        prints
        ("[0;1;37;44m聊天[[1;32mt[37m] 寄信[[1;32mm[37m] 送讯息[[1;32ms[37m] 加,减朋友[[1;32mo[37m,[1;32md[37m] 选择使用者[[1;32m↑[37m,[1;32m↓[37m] 切换模式 [[1;32mf[37m] 求救[[1;32mh[37m][m");
    }
}

do_query2 (star, curr)
int star, curr;
{
    if (user_data[curr - star].userid != NULL)
    {
        t_query (user_data[curr - star].userid);
        move (t_lines - 1, 0);
        prints
        ("[0;1;37;44m          寄信[[1;32mm[37m] 加,减朋友[[1;32mo[37m,[1;32md[37m] 看说明档[[1;32m→[37m,[1;32mRtn[37m] 选择[[1;32m↑[37m,[1;32m↓[37m] 求救[[1;32mh[37m]          [m");
    }
}

Users ()
{
    range = allusers ();
    modify_user_mode (LAUSERS);
    clear ();
    user_data = (struct userec *) calloc (sizeof (struct userec), BBS_PAGESIZE);
    choose (NA, 0, print_title2, deal_key2, Show_Users, do_query2);
    clear ();
    free (user_data);
    return;
}

int
t_friends ()
{
    char buf[STRLEN];

    modify_user_mode (FRIEND);
    real_user_names = 0;
    friendmode = YEA;
    setuserfile (buf, "friends");
    if (!dashf (buf))
    {
        move (1, 0);
        clrtobot ();
        prints ("你尚未利用 Info -> Override 设定好友名单，所以...\n");
        range = 0;
    }
    else
    {
        num_alcounter ();
        range = count_friends;
    }
    if (range == 0)
    {
        move (2, 0);
        clrtobot ();
        prints ("目前无好友上线\n");
        move (BBS_PAGESIZE + 3, 0);
        if (askyn ("是否转换成使用者模式", YEA, NA) == YEA)
        {

            range = num_visible_users ();
            freshmode = 1;
            page = -1;
            friendmode = NA;
            update_time = 0;
            choose (YEA, 0, print_title, deal_key, show_userlist, do_query);
            clear ();
            return;
        }
    }
    else
    {
        update_time = 0;
        choose (YEA, 0, print_title, deal_key, show_userlist, do_query);
    }
    clear ();
    friendmode = NA;
    return;
}

int
t_users ()
{
    friendmode = NA;
    modify_user_mode (LUSERS);
    real_user_names = 0;
    range = num_visible_users ();
    if (range == 0)
    {
        move (3, 0);
        clrtobot ();
        prints ("目前无使用者上线\n");
    }
    update_time = 0;
    choose (YEA, 0, print_title, deal_key, show_userlist, do_query);
    clear ();
    return 0;
}

int
choose (update, defaultn, title_show, key_deal, list_show, read)
int update;
int defaultn;
int (*title_show) ();
int (*key_deal) ();
int (*list_show) ();
int (*read) ();
{
    int ch, number, deal;

    readplan = NA;
    (*title_show) ();
    func_list_show = list_show;
    signal (SIGALRM, SIG_IGN);
    if (update == 1)
        update_data ();
    page = -1;
    number = 0;
    num = defaultn;
    while (1)
    {
        if (num <= 0)
            num = 0;
        if (num >= range)
            num = range - 1;
        if (page < 0 || freshmode == 1)
        {
            freshmode = 0;
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            move (3, 0);
            clrtobot ();
            if ((*list_show) () == -1)
                return -1;
            update_endline ();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            if ((*list_show) () == -1)
                return -1;
            update_endline ();
            continue;
        }
        if (readplan == YEA)
        {
            if ((*read) (page, num) == -1)
                return num;
        }
        else
        {
            move (3 + num - page, 0);
            prints (">", number);
        }
        ch = egetch ();
        if (readplan == NA)
            move (3 + num - page, 0);
        prints (" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
        {
            if (readplan == YEA)
            {
                readplan = NA;
                move (1, 0);
                clrtobot ();
                if ((*list_show) () == -1)
                    return -1;
                (*title_show) ();
                continue;
            }
            break;
        }
        deal = (*key_deal) (ch, num, page);
        if (range == 0)
            break;
        if (deal == 1)
            continue;
        else if (deal == -1)
            break;
        switch (ch)
        {
        case 'b':
        case Ctrl ('B'):
                    case KEY_PGUP:
                            if (num == 0)
                                num = range - 1;
                    else
                        num -= BBS_PAGESIZE;
            break;
        case ' ':
            if (readplan == YEA)
    {
                if (++num >= range)
                    num = 0;
                break;
            }
        case 'N':
        case Ctrl ('F'):
                    case KEY_PGDN:
                            if (num == range - 1)
                                num = 0;
                    else
                        num += BBS_PAGESIZE;
            break;
        case 'p':
        case 'l':
        case KEY_UP:
            if (num-- <= 0)
                num = range - 1;
            break;
        case 'n':
        case 'j':
        case KEY_DOWN:
            if (++num >= range)
                num = 0;
            break;
        case KEY_TAB:
            if (st != stState)
                st++;
            else
                st = stUserID;
            fill_userlist ();
            freshmode = 1;
            break;
        case '$':
        case KEY_END:
            num = range - 1;
            break;
        case KEY_HOME:
            num = 0;
            break;
        case '\n':
        case '\r':
            if (number > 0)
    {
                num = number - 1;
                break;
            }
            /* fall through */
        case KEY_RIGHT:
            {
                if (readplan == YEA)
                {
                    if (++num >= range)
                        num = 0;
                }
                else
                    readplan = YEA;
                break;
            }
        default:
            ;
        }
        if (ch >= '0' && ch <= '9')
        {
            number = number * 10 + (ch - '0');
            ch = '\0';
        }
        else
        {
            number = 0;
        }
    }
    signal (SIGALRM, SIG_IGN);
    return -1;
}

//next func add by alt
/* 抽奖程序用
void getoluserid(char *buf,int n)
{
		strncpy(buf,user_record[n]->userid,20);
}
*/

void printptitle()
{
  print_title2();
  move(2,0);
  prints("\033[1;44m 编号  使用者代号    使用者昵称 上站次数  发文章数   等  级    上次登录地址    \033[0m\n");
  update_endline();

}

int countpusers(uentp)
struct userec *uentp;
{
  static int allpusers;
  char permstr[10];
  if( uentp == NULL ) {
      int c = allpusers;
      allpusers = 0;
      return c;
  }
  if(uentp->numlogins!=0&&uleveltochar(permstr,uentp->userlevel)!=0)
    if( (uentp->userlevel==PERM_BASIC&&pmode==1)||  /*未注册*/
        (uentp->userlevel&PERM_POST&&pmode==2)||    /*已注册*/
        (uentp->userlevel&PERM_666LIFE&&pmode==3)||   /*修改基本权限*/
        (uentp->userlevel&PERM_CLOAK&&pmode==4)||   /*隐身*/
        (uentp->userlevel&PERM_SEECLOAK&&pmode==5)||  /*看隐身*/
        (uentp->userlevel&PERM_XEMPT&&pmode==6)||/*永久帐号*/
        (uentp->userlevel&PERM_WELCOME&&pmode==7)||/*编辑进站画面*/
        (uentp->userlevel&PERM_BOARDS&&pmode==8)|| /*板主*/
        (uentp->userlevel&PERM_ACCOUNTS&&pmode==9)||/*帐号管理员*/
        (uentp->userlevel&PERM_CHATCLOAK&&pmode==10)||/*智囊团成员*/
		(uentp->userlevel&PERM_OVOTE&&pmode==11)||/*投票管理员*/
		(uentp->userlevel&PERM_SYSOP&&pmode==12)||/*本站站长*/
		(uentp->userlevel&PERM_ANNOUNCE&&pmode==13)||/*精华区总管*/
		(uentp->userlevel&PERM_OBOARDS&&pmode==14)||/*讨论区总管*/
		(uentp->userlevel&PERM_ACBOARD&&pmode==15)||/*活动看板总管*/
		(uentp->userlevel&PERM_FORCEPAGE&&pmode==16)||/*强制呼叫*/
		(uentp->userlevel&PERM_EXT_IDLE&&pmode==17)||/*延长发呆时间*/
		(uentp->userlevel&PERM_MESSAGE&&pmode==18)||/*给他人发消息*/
		(uentp->userlevel&PERM_ACHATROOM&&pmode==19)||/*聊天室总管*/
		(uentp->userlevel&PERM_REALESTATE&&pmode==20)||/*大信箱*/
		(uentp->userlevel&PERM_MAILINFINITY&&pmode==21)||/*信箱无上限*/
		(uentp->userlevel&PERM_REALNAME&&pmode==22)||/*个人文集总管*/
		(uentp->userlevel&PERM_SPECIAL6&&pmode==23)||/*技术智囊*/
		(uentp->userlevel&PERM_SPECIAL7&&pmode==24)||
		(uentp->userlevel&PERM_SPECIAL8&&pmode==25)||
		(uentp->userlevel&PERM_SPECIAL9&&pmode==26)||
		(uentp->userlevel&PERM_SPECIALT&&pmode==27)
		)/*本站区长*/
		
        allpusers++;
  return 0;
}

int apusers()
{
  countpusers((struct userec *)NULL);
  if(apply_record(PASSFILE,countpusers,sizeof(struct userec)) == -1)
      return 0;
  return countpusers((struct userec *)NULL);
}

int FindPerm(uentp)
struct userec *uentp;
{
     /* level:  pmode */
    static int i;
    char permstr[10];
    char addstr[18];
    int override;
    if( uentp == NULL )  { printptitle(); i = 0; return 0; }
    if( uentp->numlogins == 0 ||
        uleveltochar(permstr,uentp->userlevel)==0 )
        return 0;
    if(i<page||i>=page+BBS_PAGESIZE||i>=range)  {
        i++;
        if(i>=page+BBS_PAGESIZE||i>=range)
            return QUIT;
        else
            return 0;
    }

    if( (uentp->userlevel==PERM_BASIC&&pmode==1)||  /*未注册*/
        (uentp->userlevel&PERM_POST&&pmode==2)||    /*已注册*/
        (uentp->userlevel&PERM_666LIFE&&pmode==3)||   /*修改基本权限*/
        (uentp->userlevel&PERM_CLOAK&&pmode==4)||   /*隐身*/
        (uentp->userlevel&PERM_SEECLOAK&&pmode==5)||  /*看隐身*/
        (uentp->userlevel&PERM_XEMPT&&pmode==6)||/*永久帐号*/
        (uentp->userlevel&PERM_WELCOME&&pmode==7)||/*编辑进站画面*/
        (uentp->userlevel&PERM_BOARDS&&pmode==8)|| /*板主*/
        (uentp->userlevel&PERM_ACCOUNTS&&pmode==9)||/*帐号管理员*/
        (uentp->userlevel&PERM_CHATCLOAK&&pmode==10)||/*智囊团成员*/
		(uentp->userlevel&PERM_OVOTE&&pmode==11)||/*投票管理员*/
		(uentp->userlevel&PERM_SYSOP&&pmode==12)||/*本站站长*/
		(uentp->userlevel&PERM_ANNOUNCE&&pmode==13)||/*精华区总管*/
		(uentp->userlevel&PERM_OBOARDS&&pmode==14)||/*讨论区总管*/
		(uentp->userlevel&PERM_ACBOARD&&pmode==15)||/*活动看板总管*/
		(uentp->userlevel&PERM_FORCEPAGE&&pmode==16)||/*强制呼叫*/
		(uentp->userlevel&PERM_EXT_IDLE&&pmode==17)||/*延长发呆时间*/
		(uentp->userlevel&PERM_MESSAGE&&pmode==18)||/*给他人发消息*/
		(uentp->userlevel&PERM_ACHATROOM&&pmode==19)||/*聊天室总管*/
		(uentp->userlevel&PERM_REALESTATE&&pmode==20)||/*大信箱*/
		(uentp->userlevel&PERM_MAILINFINITY&&pmode==21)||/*信箱无上限*/
		(uentp->userlevel&PERM_REALNAME&&pmode==22)||/*个人文集总管*/
		(uentp->userlevel&PERM_SPECIAL6&&pmode==23)||/*技术智囊*/
		(uentp->userlevel&PERM_SPECIAL7&&pmode==24)||/*本站区长*/
		(uentp->userlevel&PERM_SPECIAL8&&pmode==25)||
		(uentp->userlevel&PERM_SPECIAL9&&pmode==26)||
		(uentp->userlevel&PERM_SPECIALT&&pmode==27)
		)/*本站仲裁*/

	{
           uleveltochar(&permstr,uentp->userlevel);
           sprintf(addstr,"%-.16s",uentp->lasthost);
           user_data[i-page]=*uentp;
           override=myfriend(searchuser(uentp->userid));
           move(i-page+3,0);
           prints("%4d %2s%s%-12s%s  %-8s  %5d     %5d     %-10s  %-16s\n",i+1,
             (override)?"√":"",
             (override)?"\033[1;32m":"",uentp->userid,(override)?"\033[0m":"",
             uentp->username,
             uentp->numlogins,
             uentp->numposts,
             permstr,
             addstr);
           i++ ;
     }
     return 0;
}
int getperm()
{
  modify_user_mode( ADMIN );
  FindPerm((struct userec *)NULL);
  if(apply_record(PASSFILE,FindPerm,sizeof(struct userec))==-1)
  {
     prints("没有此权限的用户!");
     pressreturn();
     return 0;
  }
  clrtobot();
  return 0;
}

int dealukey(ch,allnum,pagenum)
char ch;
int allnum,pagenum;
{
	char    buf[STRLEN];
	static  int   msgflag;
	if(msgflag==YEA)
	{
		show_message(NULL);
		msgflag=NA;
	}
	switch(ch)
	{
	case 'h': case 'H':
		show_help( "help/usershelp" );
		break;
	case 'm': case 'M':
		if(!HAS_PERM(PERM_POST))
			return 1;
		m_send(user_data[allnum-pagenum].userid);
		break;
	case 'o': case 'O':
		if(!strcmp("guest",currentuser.userid))
			return 0;
		sprintf(buf,"确定要把 %s 加入好友名单吗",
			user_data[allnum-pagenum].userid);
		move(BBS_PAGESIZE+3,0);
		if(askyn(buf,NA,NA)==NA)
			break;
		if(addtooverride(user_data[allnum-pagenum].userid)
			==-1)
		{
			sprintf(buf,"%s 已在朋友名单",
				user_data[allnum-pagenum].userid);
			show_message(buf);
		}
		else
		{
			sprintf(buf,"%s 列入朋友名单",
				user_data[allnum-pagenum].userid);
			show_message(buf);
		}
		msgflag=YEA;
		return 1;
		break;
	case 'd': case 'D':
		sprintf(buf,"确定要把 %s 从好友名单删除吗",
			user_data[allnum-pagenum].userid);
		move(BBS_PAGESIZE+3,0);
		if(askyn(buf,NA,NA)==NA)
			break;
		if(deleteoverride(user_data[allnum-pagenum].userid,"frinds")
			==-1)
		{
			sprintf(buf,"%s 本来就不在好友名单中",
				user_data[allnum-pagenum].userid);
			show_message(buf);
		}
		else
		{
			sprintf(buf,"%s 已从好友名单移除",
				user_data[allnum-pagenum].userid);
			show_message(buf);
		}
		msgflag=YEA;
		return 1;
		break;
	default:
		return 0;
	}
	modify_user_mode( LAUSERS );
	if(readplan==NA)
	{
		print_title2();
		move(3,0);
		clrtobot();
		if(getperm()==-1)
			return -1;
		update_endline();
	}
	redoscr();
	return 1;
}



void t_findperm()
{
	char msg[4];
	int ch;
	clear();
	move(0,0);
	friendmode = NA;
	if(!HAS_PERM(PERM_SYSOP))
		return;
	modify_user_mode(ADMIN);
	prints("思源湖BBS特殊权限统计系统\n\n");
	prints("查询具有下列权限的用户: \n\n");
	prints("(0)  放弃\n");
	prints("(1)  未注册帐号                   (15) 活动看板总管\n");
	prints("(2)  已注册帐号                   (16) 强制呼叫\n");
	prints("(3)  666生命力                    (17) 延长发呆时间\n");
	prints("(4)  隐身权限                     (18) 给他人发消息\n");
	prints("(5)  看隐身权限                   (19) 聊天室总管\n");
	prints("(6)  永久帐号                     (20) 本站仲裁\n");
	prints("(7)  编辑进站画面                 (21) 信箱无上限\n");
	prints("(8)  板主                         (22) 真实电子邮箱\n");
	prints("(9)  帐号管理员                   (23) 技术智囊\n");
	prints("(10) 智囊团成员                   (24) 本站区长\n");
	prints("(11) 投票管理员                   (25) 银行行长\n");
	prints("(12) 本站站长                     (26) 隐藏板面阅读权限\n");
	prints("(13) 精华区总管                   (27) T板面阅读权限\n");
	prints("(14) 讨论区总管\n");
	
	getdata( 23, 0,"请输入您要查询的权限 (0~27) [0]: ", msg, 3, DOECHO, /*NULL,*/ YEA);
	//if( msg[0]-'0' < 1 || msg[0]-'0' > 9 )
	ch = atoi(msg);
	if (!isdigit(msg[0]) || ch <= 0 || ch > 27 || msg[0] == '\n' || msg[0] == '\0')
		return;
	pmode = ch;
	range = apusers();
	if( range == 0 ) {
		move(15,0);
		prints("没有此权限的用户!");
		pressanykey();
		return;
	}
	clear();
	user_data=(struct userec *)calloc(sizeof(struct userec),BBS_PAGESIZE);
	choose(NA,0,print_title2,dealukey,getperm,do_query2);
	clear();
	free(user_data);
	return;
}


