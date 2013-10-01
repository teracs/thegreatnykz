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
int watchinboardmode = 0;
int usercounter, real_user_names = 0;
int range, page, readplan, num;
struct user_info *user_record[MAXACTIVE];
struct userec *user_data;

enum sort_type
{ stUserID, stUserName, stIP, stState } st = stUserID;

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

static int friend_search(int uid, struct user_info *uentp, int tblsize)
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
        cmp = uentp->friends[mid] - uid;
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

int myfriend(int uid)
{
    return friend_search(uid, &uinfo, uinfo.fnum);
}

int hisfriend(struct user_info *uentp)
{
    return friend_search(uinfo.uid, uentp, uentp->fnum);
}

int isreject(struct user_info *uentp)
{
    int i;

    if (HAS_PERM(PERM_SYSOP))
        return NA;
    if (uentp->uid != uinfo.uid)
    {
        for (i = 0;
             i < MAXREJECTS && (uentp->reject[i] || uinfo.reject[i]); i++)
        {
            if (uentp->reject[i] == uinfo.uid
                || uentp->uid == uinfo.reject[i])
                return YEA;
        }
    }
    return NA;
}

int print_title(void)
{
    char buf[32];

    if (uinfo.mode == WATCHINBOARD)
        sprintf(buf, "[%s°åÔÚÏßÁÐ±í]", currboard);
    else if (friendmode)
        sprintf(buf, "%s", "[ºÃÅóÓÑÁÐ±í]");
    else
        sprintf(buf, "%s", "[Ê¹ÓÃÕßÁÐ±í]");
    docmdtitle(buf,
               " ÁÄÌì[[1;32mt[m] ¼ÄÐÅ[[1;32mm[m] ËÍÑ¶Ï¢[[1;32ms[m] ¼Ó,¼õÅóÓÑ[[1;32mo[m,[1;32md[m] ¿´ËµÃ÷µµ[[1;32m¡ú[m,[1;32mRtn[m] ÇÐ»»Ä£Ê½ [[1;32mf[m] Çó¾È[[1;32mh[m]");
    update_endline();
    return 0;
}

int print_title2(void)
{
    docmdtitle((friendmode) ? "[ºÃÅóÓÑÁÐ±í]" : "[Ê¹ÓÃÕßÁÐ±í]",
               "        ¼ÄÐÅ[[1;32mm[m] ¼Ó,¼õÅóÓÑ[[1;32mo[m,[1;32md[m] ¿´ËµÃ÷µµ[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ý[m] Çó¾È[[1;32mh[m]");
    update_endline();
    return 0;
}

void update_data(int num)
{
    if (readplan == YEA)
        return;
    if (time(0) >= update_time + refreshtime - 1)
    {
        freshmode = 1;
    }
    signal(SIGALRM, update_data);
    alarm(refreshtime);
    return;
}

int print_user_info_title(void)
{
    char title_str[512];
    char *field_2;

    move(2, 0);
    clrtoeol();
    field_2 = "Ê¹ÓÃÕßêÇ³Æ";
    if (real_user_names)
        field_2 = "ÕæÊµÐÕÃû  ";
    sprintf(title_str,
            "[1;44m±àºÅ %sÊ¹ÓÃÕß´úºÅ%s %s%s%s%8.8s %sÉÏÕ¾µÄÎ»ÖÃ%s      P M %c%sÄ¿Ç°¶¯Ì¬%s %5s[m\n",
            (st == stUserID) ? "[32m{" : " ",
            (st == stUserID) ? "}[37m" : " ",
            (st == stUserName) ? "[32m{" : " ", field_2,
            (st == stUserName) ? "}[37m" : " ", " ",
            (st == stIP) ? "[32m{" : " ", (st == stIP) ? "}[37m" : " ",
            ((HAS_PERM(PERM_SYSOP | PERM_SEECLOAK | PERM_CLOAK)) ? 'C' :
             ' '), (st == stState) ? "[32m{" : " ",
            (st == stState) ? "}[37m" : " ",
#ifdef SHOW_IDLE_TIME
            "Ê±:·Ö");
#else
            "");
#endif
    prints("%s", title_str);
    return 0;
}

void show_message(char *msg)
{

    move(BBS_PAGESIZE + 2, 0);
    clrtoeol();
    if (msg != NULL)
        prints("[1m%s[m", msg);
    refresh();
}

void swap_user_record(int a, int b)
{
    struct user_info *c;

    c = user_record[a];
    user_record[a] = user_record[b];
    user_record[b] = c;
}

int compare_user_record(struct user_info *left, struct user_info *right)
{
    int retCode;

    switch (st)
    {
    case stUserID:
        retCode = strcasecmp(left->userid, right->userid);
        break;
    case stUserName:
        retCode = strcasecmp(left->username, right->username);
        break;
    case stIP:
        retCode = strncmp(left->from, right->from, 20);
        break;
    case stState:
        retCode = left->mode - right->mode;
        break;
    default:
        retCode = 0;
        break;
    }
    return retCode;
}

void sort_user_record(int left, int right)
{
    int i, last;

    if (left >= right)
        return;
    swap_user_record(left, (left + right) / 2);
    last = left;
    for (i = left + 1; i <= right; i++)
    {

        if (compare_user_record(user_record[i], user_record[left]) < 0)
        {
            swap_user_record(++last, i);
        }
    }
    swap_user_record(left, last);
    sort_user_record(left, last - 1);
    sort_user_record(last + 1, right);
}

int fill_userlist(void)
{
    FILE *fp;
    char inboard_file[STRLEN];
    char buf[STRLEN];
    char *userid;

    register int i, n, totalusernum;
    int friendno[MAXACTIVE];
    extern struct UTMPFILE *utmpshm;

    resolve_utmp();
    totalusernum = 0;
    numf = 0;

    if (uinfo.mode == WATCHINBOARD)
    {
        setbfile(inboard_file, currboard, INBOARD_FILE);
        if ((fp = fopen(inboard_file, "r")) == NULL)
            return -1;
        while (fgets(buf, STRLEN, fp) != NULL)
        {
            userid = (char *) strtok(buf, "\n");
            if (userid != NULL)
            {
                for (i = 0; i < USHM_SIZE; i++)
                {
                    if (strcasecmp(utmpshm->uinfo[i].userid, userid) == 0)
                    {
                        if (!utmpshm->uinfo[i].active
                            || !utmpshm->uinfo[i].pid
                            || isreject(&utmpshm->uinfo[i]))
                            break;
                        if ((utmpshm->uinfo[i].invisible)
                            && (usernum != utmpshm->uinfo[i].uid)
                            && (!HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)))
                            break;
                        user_record[totalusernum++] = &utmpshm->uinfo[i];
                        break;
                    }
                }
            }
        }
        range = totalusernum;
        return totalusernum == 0 ? -1 : 1;
    }
    else
    {
        for (i = 0; i < USHM_SIZE; i++)
        {
            if (!utmpshm->uinfo[i].active
                || !utmpshm->uinfo[i].pid || isreject(&utmpshm->uinfo[i]))
                continue;
            if ((utmpshm->uinfo[i].invisible)
                && (usernum != utmpshm->uinfo[i].uid)
                && (!HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)))
                continue;
            if (myfriend(utmpshm->uinfo[i].uid))
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
                        swap_user_record(n, i);
                    n++;
                }
            }
            if (numf >= 2)
            {
                sort_user_record(0, numf - 1);
            }
            else if (numf == 2)
            {
                if (compare_user_record(user_record[0], user_record[1]) <
                    0)
                    swap_user_record(0, 1);
            }
            sort_user_record(numf, totalusernum - 1);
        }
        else
        {
            if (totalusernum >= 2)
            {
                sort_user_record(0, totalusernum - 1);
            }
            else if (totalusernum == 2)
            {
                if (compare_user_record(user_record[0], user_record[1]) <
                    0)
                    swap_user_record(0, 1);
            }
        }
        range = totalusernum;
        return totalusernum == 0 ? -1 : 1;
    }
}

int do_userlist(void)
{
    int i, j, override;
    char user_info_str[STRLEN * 2];
    struct user_info *uentp;

    move(3, 0);
    print_user_info_title();
    for (i = 0, j = 0; j < BBS_PAGESIZE && i + page < range; i++)
    {
        uentp = user_record[i + page];
        override = (i + page < numf) || friendmode;
        if (readplan == YEA)
            return 0;
        if (uentp == NULL || !uentp->active || !uentp->pid)
        {
            continue;
        }
        if (uentp != NULL)
        {
            sprintf(user_info_str,
                    " [m%3d%2s%-12.22s[37m %-20.20s[m %-16.16s %c %c %c %s%-10.10s[37m %5.5s[m\n",
                    i + 1 + page,
#ifdef SHOWMETOFRIEND
                    (override) ? ((hisfriend(uentp)) ? "[32m¡Ì" :
                                  "¡Ì[32m") : ((hisfriend(uentp)) ?
                                                "[33m  " : "  "),
#else
                    (override) ? "[32m¡Ì" : "  ",
#endif
                    uentp->userid,
                    (real_user_names) ? uentp->realname : uentp->username,
                    uentp->from,
                    (uentp->mode ==
                     BBSNET) ? '@' : pagerchar(hisfriend(uentp),
                                               uentp->pager),
                    msgchar(uentp), (uentp->invisible == YEA) ? '@' : ' ',
                    (uentp->invisible == YEA) ? "[36m" : (uentp->mode ==
                                                           POSTING) ?
                    "[32m" : ((uentp->mode == BBSNET) ? "[33m" : ""),
                    ModeType(uentp->mode),
#ifdef SHOW_IDLE_TIME
                    idle_str(uentp));
#else
                    "");
#endif
            clrtoeol();
            prints("%s", user_info_str);
            j++;
        }
    }
    return 0;
}

int show_userlist(void)
{
    if (update_time + refreshtime < time(0))
    {
        fill_userlist();
        update_time = time(0);
    }
    if (range == 0)
    {
        move(2, 0);
        prints("Ã»ÓÐÊ¹ÓÃÕß£¨ÅóÓÑ£©ÔÚÁÐ±íÖÐ...\n");
        clrtobot();
        if (friendmode)
        {
            move(BBS_PAGESIZE + 3, 0);
            if (askyn("ÊÇ·ñ×ª»»³ÉÊ¹ÓÃÕßÄ£Ê½", YEA, NA) == YEA)
            {
                range = num_visible_users();
                page = -1;
                friendmode = NA;
                return 1;
            }
        }
        else
            pressanykey();
        return -1;
    }
    do_userlist();
    clrtobot();
    return 1;
}

int deal_key(int ch, int allnum, int pagenum)
{
    char buf[STRLEN], desc[5];
    static int msgflag;
    extern int friendflag;

    if (msgflag == YEA)
    {
        show_message(NULL);
        msgflag = NA;
    }
    switch (ch)
    {
    case 'Y':
        if (HAS_PERM(PERM_CLOAK))
            x_cloak();
        break;
    case 'P':
        t_pager();
        break;
    case 'C':
    case 'c':
        if (ch == 'C')
        {
            strcpy(genbuf, "±ä»»êÇ³Æ(²»ÊÇÁÙÊ±±ä»»)Îª: ");
        }
        else
        {
            strcpy(genbuf, "ÔÝÊ±±ä»»êÇ³Æ(×î¶à10¸öºº×Ö): ");
        }
        strcpy(buf, "");
        getdata(BBS_PAGESIZE + 3, 0, genbuf, buf,
                (ch == 'C') ? NAMELEN : 21, DOECHO, NA);
        if (buf[0] != '\0')
        {
            strcpy(uinfo.username, buf);
            if (ch == 'C')
            {
                set_safe_record();
                strcpy(currentuser.username, buf);
                substitute_record(PASSFILE, &currentuser,
                                  sizeof(currentuser), usernum);
            }
        }
        break;
    case 'k':
    case 'K':
        if (!HAS_PERM(PERM_SYSOP) && (usernum != user_record[allnum]->uid))
            return 1;
        if (!strcmp(currentuser.userid, "guest"))
            return 1;
#ifdef SPECIALID

        if (!strcmp(currentuser.userid, SPECIALID1)
            || !strcmp(currentuser.userid, SPECIALID2))
            return 1;
#endif

        if (user_record[allnum]->pid == uinfo.pid)
            strcpy(buf, "Äã×Ô¼ºÒª°Ñ¡¾×Ô¼º¡¿Ìß³öÈ¥Âð");
        else
            sprintf(buf, "ÄãÒª°Ñ %s Ìß³öÕ¾ÍâÂð",
                    user_record[allnum]->userid);
        move(BBS_PAGESIZE + 2, 0);
        if (askyn(buf, NA, NA) == NA)
            break;
        if (kick_user(user_record[allnum]) == 1)
        {
            sprintf(buf, "%s ÒÑ±»Ìß³öÕ¾Íâ", user_record[allnum]->userid);
        }
        else
        {
            sprintf(buf, "%s ÎÞ·¨Ìß³öÕ¾Íâ", user_record[allnum]->userid);
        }
        user_record[allnum]->active = NA;
        update_time = 0;
        msgflag = YEA;
        break;
    case 'h':
    case 'H':
        show_help("help/userlisthelp");
        break;
    case 't':
    case 'T':
        if (!HAS_PERM(PERM_PAGE))
            return 1;
        if (user_record[allnum]->uid != usernum)
            ttt_talk(user_record[allnum]);
        else
            return 1;
        break;
    case 'v':
    case 'V':
        if (!HAS_PERM(PERM_SYSOP))
            return 1;
        real_user_names = !real_user_names;
        break;
    case 'm':
    case 'M':
        if (!HAS_PERM(PERM_POST))
            return 1;
        m_send(user_record[allnum]->userid);
        break;
    case 'g':
    case 'G':
        if (!HAS_PERM(PERM_POST))
            return 1;
        sendGoodWish(user_record[allnum]->userid);
        break;
    case 'f':
    case 'F':
        if (uinfo.mode == WATCHINBOARD)
            return 0;
        if (friendmode)
            friendmode = NA;
        else
            friendmode = YEA;
        update_time = 0;
        break;

    case 's':
    case 'S':
        if (!strcmp("guest", currentuser.userid))
            return 0;
        if (!HAS_PERM(PERM_MESSAGE))
            return 0;
        if (!canmsg(user_record[allnum]))
        {
            sprintf(buf, "%s ÒÑ¾­¹Ø±ÕÑ¶Ï¢ºô½ÐÆ÷",
                    user_record[allnum]->userid);
            msgflag = YEA;
            break;
        }
        do_sendmsg(user_record[allnum], NULL, 0, user_record[allnum]->pid);
        break;
    case 'o':
    case 'O':
    case 'r':
    case 'R':
#ifdef SPECIALID

        if (!strcmp(currentuser.userid, SPECIALID1)
            || !strcmp(currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp("guest", currentuser.userid))
            return 0;
        if (ch == 'o' || ch == 'O')
        {
            friendflag = YEA;
            strcpy(desc, "ºÃÓÑ");
        }
        else
        {
            friendflag = NA;
            strcpy(desc, "»µÈË");
        }
        sprintf(buf, "È·¶¨Òª°Ñ %s ¼ÓÈë%sÃûµ¥Âð",
                user_record[allnum]->userid, desc);
        move(BBS_PAGESIZE + 2, 0);
        if (askyn(buf, NA, NA) == NA)
            break;
        if (addtooverride(user_record[allnum]->userid) == -1)
        {
            sprintf(buf, "%s ÒÑÔÚ%sÃûµ¥", user_record[allnum]->userid,
                    desc);
        }
        else
        {
            sprintf(buf, "%s ÁÐÈë%sÃûµ¥", user_record[allnum]->userid,
                    desc);
        }
        msgflag = YEA;
        break;
    case 'd':
    case 'D':
#ifdef SPECIALID

        if (!strcmp(currentuser.userid, SPECIALID1)
            || !strcmp(currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp("guest", currentuser.userid))
            return 0;
        sprintf(buf, "È·¶¨Òª°Ñ %s ´ÓºÃÓÑÃûµ¥É¾³ýÂð",
                user_record[allnum]->userid);
        move(BBS_PAGESIZE + 2, 0);
        if (askyn(buf, NA, NA) == NA)
            break;
        if (deleteoverride(user_record[allnum]->userid, "friends") == -1)
        {
            sprintf(buf, "%s ±¾À´¾Í²»ÔÚÅóÓÑÃûµ¥ÖÐ",
                    user_record[allnum]->userid);
        }
        else
        {
            sprintf(buf, "%s ÒÑ´ÓÅóÓÑÃûµ¥ÒÆ³ý",
                    user_record[allnum]->userid);
        }
        msgflag = YEA;
        break;
    default:
        return 0;
    }
    if (uinfo.mode != WATCHINBOARD)
    {
        if (friendmode)
            modify_user_mode(FRIEND);
        else
            modify_user_mode(LUSERS);
    }
    if (readplan == NA)
    {
        print_title();
        clrtobot();
        if (show_userlist() == -1)
            return -1;
        if (msgflag)
            show_message(buf);
        update_endline();
    }
    return 1;
}

int deal_key2(int ch, int allnum, int pagenum)
{
    char buf[STRLEN];
    static int msgflag;

    if (msgflag == YEA)
    {
        show_message(NULL);
        msgflag = NA;
    }
    switch (ch)
    {
    case 'h':
    case 'H':
        show_help("help/usershelp");
        break;
    case 'm':
    case 'M':
        if (!HAS_PERM(PERM_POST))
            return 1;
        m_send(user_data[allnum - pagenum].userid);
        break;
    case 'o':
    case 'O':
#ifdef SPECIALID

        if (!strcmp(currentuser.userid, SPECIALID1)
            || !strcmp(currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp("guest", currentuser.userid))
            return 0;
        sprintf(buf, "È·¶¨Òª°Ñ %s ¼ÓÈëºÃÓÑÃûµ¥Âð",
                user_data[allnum - pagenum].userid);
        move(BBS_PAGESIZE + 2, 0);
        if (askyn(buf, NA, NA) == NA)
            break;
        if (addtooverride(user_data[allnum - pagenum].userid) == -1)
        {
            sprintf(buf, "%s ÒÑÔÚÅóÓÑÃûµ¥",
                    user_data[allnum - pagenum].userid);
            show_message(buf);
        }
        else
        {
            sprintf(buf, "%s ÁÐÈëÅóÓÑÃûµ¥",
                    user_data[allnum - pagenum].userid);
            show_message(buf);
        }
        msgflag = YEA;
        if (!friendmode)
            return 1;
        break;
    case 'f':
    case 'F':
        if (uinfo.mode == WATCHINBOARD)
            return 0;
        toggle1++;
        if (toggle1 >= 3)
            toggle1 = 0;
        break;
    case 't':
    case 'T':
        toggle2++;
        if (toggle2 >= 2)
            toggle2 = 0;
        break;
    case 'd':
    case 'D':
#ifdef SPECIALID

        if (!strcmp(currentuser.userid, SPECIALID1)
            || !strcmp(currentuser.userid, SPECIALID2))
            return 0;
#endif

        if (!strcmp("guest", currentuser.userid))
            return 0;
        sprintf(buf, "È·¶¨Òª°Ñ %s ´ÓºÃÓÑÃûµ¥É¾³ýÂð",
                user_data[allnum - pagenum].userid);
        move(BBS_PAGESIZE + 2, 0);
        if (askyn(buf, NA, NA) == NA)
            break;
        if (deleteoverride(user_data[allnum - pagenum].userid, "friends")
            == -1)
        {
            sprintf(buf, "%s ±¾À´¾Í²»ÔÚÅóÓÑÃûµ¥ÖÐ",
                    user_data[allnum - pagenum].userid);
            show_message(buf);
        }
        else
        {
            sprintf(buf, "%s ÒÑ´ÓÅóÓÑÃûµ¥ÒÆ³ý",
                    user_data[allnum - pagenum].userid);
            show_message(buf);
        }
        msgflag = YEA;
        if (!friendmode)
            return 1;
        break;
    default:
        return 0;
    }
    modify_user_mode(LAUSERS);
    if (readplan == NA)
    {
        print_title2();
        move(3, 0);
        clrtobot();
        if (Show_Users() == -1)
            return -1;
        update_endline();
    }
    redoscr();
    return 1;
}

int countusers(void *vuentp)
{
    struct userec *uentp = (struct userec *) vuentp;
    static int totalusers;
    char permstr[11];

    if (uentp == NULL)
    {
        int c = totalusers;

        totalusers = 0;
        return c;
    }
    if (uentp->numlogins != 0
        && uleveltochar(permstr, uentp->userlevel) != 0)
        totalusers++;
    return 0;
}
int printuent(void *vuentp)
{
    struct userec *uentp = (struct userec *) vuentp;
    static int i;
    char permstr[11];
    char msgstr[18];
    int override;

    if (uentp == NULL)
    {
        printutitle();
        i = 0;
        return 0;
    }
    if (uentp->numlogins == 0
        || uleveltochar(permstr, uentp->userlevel) == 0)
        return 0;
    if (i < page || i >= page + BBS_PAGESIZE || i >= range)
    {
        i++;
        if (i >= page + BBS_PAGESIZE || i >= range)
            return QUIT;
        else
            return 0;
    }
    uleveltochar(permstr, uentp->userlevel);
    switch (toggle1)
    {
    case 0:
        getdatestring(uentp->lastlogin, NA);
        sprintf(msgstr, "%-.16s", datestring + 6);
        break;
    case 1:
        sprintf(msgstr, "%-.16s", uentp->lasthost);
        break;
    case 2:
    default:
        getdatestring(uentp->firstlogin, NA);
        sprintf(msgstr, "%-.14s", datestring);
        break;
    }
    user_data[i - page] = *uentp;
    override = myfriend(searchuser(uentp->userid));
    prints(" %5d%2s%s%-12s%s %-17s %6d %4d %10s %-16s\n", i + 1,
           (override) ? "¡Ì" : "",
           (override) ? "[1;32m" : "", uentp->userid,
           (override) ? "[m" : "",
#if defined(ACTS_REALNAMES)
           HAS_PERM(PERM_SYSOP) ? uentp->realname : uentp->username,
#else
           uentp->username,
#endif
           (toggle2 == 0) ? (uentp->numlogins) : (uentp->numposts),
           (toggle2 ==
            0) ? (unsigned int) (uentp->stay / 3600) : uentp->nummails,
           HAS_PERM(PERM_SEEULEVELS) ? permstr : "", msgstr);
    i++;
    usercounter++;
    return 0;
}

int allusers(void)
{
    countusers(NULL);
    if (apply_record(PASSFILE, countusers, sizeof(struct userec)) == -1)
    {
        return 0;
    }
    return countusers(NULL);
}

int mailto(void *vuentp)
{
    struct userec *uentp = (struct userec *) vuentp;
    char filename[STRLEN];

    sprintf(filename, "tmp/mailall.%s", currentuser.userid);
    if ((uentp->userlevel == PERM_BASIC && mailmode == 1) ||
        (uentp->userlevel & PERM_POST && mailmode == 2) ||
        (uentp->userlevel & PERM_BOARDS && mailmode == 3) ||
        (uentp->userlevel & PERM_CHATCLOAK && mailmode == 4))
    {
        mail_file(filename, uentp->userid, save_title);
    }
    return 1;
}

void mailtoall(int mode)
{

    mailmode = mode;
    if (apply_record(PASSFILE, mailto, sizeof(struct userec)) == -1)
    {
        prints("No Users Exist");
        pressreturn();
    }
}

int Show_Users(void)
{

    usercounter = 0;
    modify_user_mode(LAUSERS);
    printuent((struct userec *) NULL);
    if (apply_record(PASSFILE, printuent, sizeof(struct userec)) == -1)
    {
        prints("No Users Exist");
        pressreturn();
        return 0;
    }
    clrtobot();
    return 0;
}

void setlistrange(int i)
{
    range = i;
}

int do_query(int star, int curr)
{
    if (user_record[curr]->userid != NULL)
    {
        clear();
        t_query(user_record[curr]->userid);
        move(t_lines - 1, 0);
        prints
            ("[0;1;37;44mÁÄÌì[[1;32mt[37m] ¼ÄÐÅ[[1;32mm[37m] ËÍÑ¶Ï¢[[1;32ms[37m] ¼Ó,¼õÅóÓÑ[[1;32mo[37m,[1;32md[37m] Ñ¡ÔñÊ¹ÓÃÕß[[1;32m¡ü[37m,[1;32m¡ý[37m] ÇÐ»»Ä£Ê½ [[1;32mf[37m] Çó¾È[[1;32mh[37m][m");
    }
    return 0;
}

int do_query2(int star, int curr)
{
    if (user_data[curr - star].userid != NULL)
    {
        t_query(user_data[curr - star].userid);
        move(t_lines - 1, 0);
        prints
            ("[0;1;37;44m          ¼ÄÐÅ[[1;32mm[37m] ¼Ó,¼õÅóÓÑ[[1;32mo[37m,[1;32md[37m] ¿´ËµÃ÷µµ[[1;32m¡ú[37m,[1;32mRtn[37m] Ñ¡Ôñ[[1;32m¡ü[37m,[1;32m¡ý[37m] Çó¾È[[1;32mh[37m]          [m");
    }
    return 0;
}

void Users(void)
{
    range = allusers();
    modify_user_mode(LAUSERS);
    clear();
    user_data =
        (struct userec *) calloc(sizeof(struct userec), BBS_PAGESIZE);
    choose(NA, 0, print_title2, deal_key2, Show_Users, do_query2);
    clear();
    free(user_data);
}

int t_friends(void)
{
    if (watchinboardmode)
    {
        t_watchinboard();
        return 0;
    }

    char buf[STRLEN];

    modify_user_mode(FRIEND);
    real_user_names = 0;
    friendmode = YEA;
    setuserfile(buf, "friends");
    if (!dashf(buf))
    {
        move(1, 0);
        clrtobot();
        prints("ÄãÉÐÎ´ÀûÓÃ Info -> Override Éè¶¨ºÃÓÑÃûµ¥£¬ËùÒÔ...\n");
        range = 0;
    }
    else
    {
        num_alcounter();
        range = count_friends;
    }
    if (range == 0)
    {
        move(2, 0);
        clrtobot();
        prints("Ä¿Ç°ÎÞºÃÓÑÉÏÏß\n");
        move(BBS_PAGESIZE + 3, 0);
        if (askyn("ÊÇ·ñ×ª»»³ÉÊ¹ÓÃÕßÄ£Ê½", YEA, NA) == YEA)
        {

            range = num_visible_users();
            freshmode = 1;
            page = -1;
            friendmode = NA;
            update_time = 0;
            choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
            clear();
            return 0;
        }
    }
    else
    {
        update_time = 0;
        choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
    }
    clear();
    friendmode = NA;
    return 0;
}

int t_users(void)
{
    if (watchinboardmode)
    {
        t_watchinboard();
        return 0;
    }
    friendmode = NA;
    modify_user_mode(LUSERS);
    real_user_names = 0;
    range = num_visible_users();
    if (range == 0)
    {
        move(3, 0);
        clrtobot();
        prints("Ä¿Ç°ÎÞÊ¹ÓÃÕßÉÏÏß\n");
    }
    update_time = 0;
    choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
    clear();
    return 0;
}

int t_watchinboard()
{
    struct shortfile *bp = getbcache(currboard);
    if ((bp->flag & ANONY_FLAG) || (bp->flag & POSITIVE_ANONY_FLAG))
        return DONOTHING;

    friendmode = NA;
    modify_user_mode(WATCHINBOARD);
    watchinboardmode = 1;
    real_user_names = 0;
    update_time = 0;
    choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
    clear();
    watchinboardmode = 0;
    return FULLUPDATE;
}

int
choose(int update, int defaultn, int (*title_show) (void),
       int (*key_deal) (int, int, int), int (*list_show) (void),
       int (*read) (int, int))
{
    int ch, number, deal;

    readplan = NA;
    (*title_show) ();
    func_list_show = list_show;
    signal(SIGALRM, SIG_IGN);
    if (update == 1)
        update_data(0);
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
            move(3, 0);
            clrtobot();
            if ((*list_show) () == -1)
                return -1;
            update_endline();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            if ((*list_show) () == -1)
                return -1;
            update_endline();
            continue;
        }
        if (readplan == YEA)
        {
            if ((*read) (page, num) == -1)
                return num;
        }
        else
        {
            move(3 + num - page, 0);
            prints(">", number);
        }
        ch = egetch();
        if (readplan == NA)
            move(3 + num - page, 0);
        prints(" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
        {
            if (readplan == YEA)
            {
                readplan = NA;
                move(1, 0);
                clrtobot();
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
        case Ctrl('B'):
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
        case Ctrl('F'):
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
            fill_userlist();
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
    signal(SIGALRM, SIG_IGN);
    return -1;
}
