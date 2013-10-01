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
$Id: comm_lists.c,v 1.3 2004/07/17 04:40:09 bbs Exp $
*/
#include <dlfcn.h>
#include "bbs.h"
#define SC_BUFSIZE              10240
#define SC_KEYSIZE              256
#define SC_CMDSIZE              256
#define sysconf_ptr( offset )   (&sysconf_buf[ offset ]);

struct smenuitem
{
    int line, col, level;
    char *name, *desc, *arg;
    int (*fptr) ();
}
*menuitem;

struct sdefine
{
    char *key, *str;
    int val;
}
*sysvar;

char *sysconf_buf;
int sysconf_menu, sysconf_key, sysconf_len;
int domenu ();
int Announce (), /*Personal (), */Boards (), EGroup (), Info (), Goodbye ();
int New (), Post (), GoodBrds (), Read (), Select (), Users (), Welcome ();
int setcalltime ();
int show_allmsgs (), x_lockscreen (), x_showuser ();
int Conditions (), x_cloak (), t_users (), x_info (), x_fillform (),
x_vote ();

/*抽奖程序 by alt
int		lottery();
*/
int x_results (), ent_bnet (), ent_bnet2 (), a_edits (), x_edits ();
int r_deleteall();
int r_searchall (), r_searchalldel (), x_denypostqz (), ent_sortdir (), x_denytalkta();
int shownotepad (), x_userdefine (), x_csh ();
int m_new (), m_read (), m_send (), g_send ();
int ov_send (), s_msg (), mailall (), offline ();

#ifdef INTERNET_EMAIL
int m_internet ();
#endif

int t_users (), t_friends (), t_list (), t_monitor ();
int t_query (), t_talk (), t_pager (), t_friend (), t_reject (), x_cloak ();
int ent_chat ();
//int AddPCorpus ();		// deardragon 个人文集
int sendgoodwish ();

int friend_main();	//add friendtest, by rwzmm

#ifndef WITHOUT_ADMIN_TOOLS
int kick_user (), m_vote ();
int x_level (), x_denylevel (), m_info ();
int d_user (), m_register ();
int d_board (), m_editbrd (), m_mclean (), m_newbrd ();
int m_ordainBM (), m_retireBM (), m_ordainQZ (), m_retireQZ ();
int setsystempasswd ();
#endif

#ifdef ALLOWSWITCHCODE
int switch_code ();
#endif

int wall ();
int friend_wall ();

/*Add By Excellent */

typedef struct
{
    char *name;
    /*	int     (*fptr) ();*/
    void (*fptr);
    int type;
}
MENU;

MENU currcmd;

MENU sysconf_cmdlist[] = {
                             {"domenu", domenu, 0},
                             {"EGroups", EGroup, 0},
                             {"BoardsAll", Boards, 0},
                             {"BoardsGood", GoodBrds, 0},
                             {"BoardsNew", New, 0},
                             {"LeaveBBS", Goodbye, 0},
                             {"Announce", Announce, 0},
//                             {"Personal", Personal, 0},
                             {"SelectBoard", Select, 0},
                             {"ReadBoard", Read, 0},
                             {"PostArticle", Post, 0},
                             {"SetAlarm", setcalltime, 0},
                             {"MailAll", mailall, 0},
                             {"LockScreen", x_lockscreen, 0},
                             {"ShowUser", x_showuser, 0},
                             {"OffLine", offline, 0},
                             {"ReadNewMail", m_new, 0},
                             {"ReadMail", m_read, 0},
                             {"SendMail", m_send, 0},
                             {"GroupSend", g_send, 0},
                             {"OverrideSend", ov_send, 0},
#ifdef INTERNET_EMAIL
                             {"SendNetMail", m_internet, 0},
#endif
                             {"UserDefine", x_userdefine, 0},
                             {"ShowFriends", t_friends, 0},
                             {"ShowLogins", t_users, 0},
                             {"QueryUser", t_query, 0},
                             {"Talk", t_talk, 0},
                             {"SetPager", t_pager, 0},
                             {"SetCloak", x_cloak, 0},
                             {"SendMsg", s_msg, 0},
                             {"ShowMsg", show_allmsgs, 0},
                             {"SetFriends", t_friend, 0},
                             {"SetRejects", t_reject, 0},
                             {"FriendWall", friend_wall, 0},
                             {"EnterChat", ent_chat, 0},
                             {"ListLogins", t_list, 0},
                             {"Monitor", t_monitor, 0},
                             {"FillForm", x_fillform, 0},
                             {"Information", x_info, 0},
                             {"FriendTest", friend_main, 0},	//add friend test, by rwzmm @ sjtubbs
                             /*抽奖程序
                                {"Lottery", lottery, 0},
                              */
                             {"EditUFiles", x_edits, 0},
                             {"ShowLicense", Conditions, 0},
                             {"ShowVersion", Info, 0},
                             {"Notepad", shownotepad, 0},
                             {"Vote", x_vote, 0},
                             {"VoteResult", x_results, 0},
                             {"ExecBBSNet", ent_bnet, 0},
                             {"ExecBBSNet2", ent_bnet2, 0},
                             {"Sortdir", ent_sortdir, 0},
                             {"DenyPostQZ", x_denypostqz, 0},
                             {"BanTalkTA", x_denytalkta, 0},
                             {"Searchall", r_searchall, 0},
                             {"Deleteall", r_deleteall, 0},
                             {"Searchalldel", r_searchalldel, 0},
                             {"ShowWelcome", Welcome, 0},
                             {"AllUsers", Users, 0},
//                             {"AddPCorpus", AddPCorpus, 0},
                             {"GoodWish", sendgoodwish, 0},
#ifdef ALLOWSWITCHCODE
                             {"SwitchCode", switch_code, 0},
#endif
                             {"Kick", kick_user, 0},
                             {"OpenVote", m_vote, 0},
                             {"Setsyspass", setsystempasswd, 0},
                             {"Register", m_register, 0},
                             {"Info", m_info, 0},
                             {"Level", x_level, 0},
                             {"OrdainBM", m_ordainBM, 0},
                             {"RetireBM", m_retireBM, 0},
                             //mudboy add follow two lines
                             {"OrderQZ", m_ordainQZ, 0},
                             {"RetireQZ", m_retireQZ, 0},
                             {"ChangeLevel", x_denylevel, 0},
                             {"DelUser", d_user, 0},
                             {"NewBoard", m_newbrd, 0},
                             {"ChangeBrd", m_editbrd, 0},
                             {"BoardDel", d_board, 0},
                             {"MailClean", m_mclean, 0},
                             {"SysFiles", a_edits, 0},
                             {"Wall", wall, 0},
                             {0, 0, 0}
                         };

void
encodestr (str)
register char *str;
{
    register char ch, *buf;
    int n;

    buf = str;
    while ((ch = *str++) != '\0')
    {
        if (*str == ch && str[1] == ch && str[2] == ch)
        {
            n = 4;
            str += 3;
            while (*str == ch && n < 100)
            {
                str++;
                n++;
            }
            *buf++ = '\01';
            *buf++ = ch;
            *buf++ = n;
        }
        else
            *buf++ = ch;
    }
    *buf = '\0';
}

void
decodestr (str)
register char *str;
{
    register char ch;
    int n;

    while ((ch = *str++) != '\0')
        if (ch != '\01')
            outc (ch);
        else if (*str != '\0' && str[1] != '\0')
        {
            ch = *str++;
            n = *str++;
            while (--n >= 0)
                outc (ch);
        }
}

void *
sysconf_funcptr (func_name, type)
char *func_name;
int *type;
{
    int n = 0;
    char *str;

    while ((str = sysconf_cmdlist[n].name) != NULL)
    {
        if (strcmp (func_name, str) == 0)
        {
            *type = sysconf_cmdlist[n].type;
            return (sysconf_cmdlist[n].fptr);
        }
        n++;
    }

    *type = -1;
    return NULL;
}

void *
sysconf_addstr (str)
char *str;
{
    int len = sysconf_len;
    char *buf;

    buf = sysconf_buf + len;
    strcpy (buf, str);
    sysconf_len = len + strlen (str) + 1;
    return buf;
}

char *
sysconf_str (key)
char *key;
{
    int n;

    for (n = 0; n < sysconf_key; n++)
        if (strcmp (key, sysvar[n].key) == 0)
            return (sysvar[n].str);
    return NULL;
}

int
sysconf_eval (key)
char *key;
{
    int n;

    for (n = 0; n < sysconf_key; n++)
        if (strcmp (key, sysvar[n].key) == 0)
            return (sysvar[n].val);
    if (*key < '0' || *key > '9')
    {
        sprintf (genbuf, "sysconf: unknown key: %s.", key);
        report (genbuf);
    }
    return (strtol (key, NULL, 0));
}

void
sysconf_addkey (key, str, val)
char *key, *str;
int val;
{
    int num;

    if (sysconf_key < SC_KEYSIZE)
    {
        if (str == NULL)
            str = sysconf_buf;
        else
            str = sysconf_addstr (str);
        num = sysconf_key++;
        sysvar[num].key = sysconf_addstr (key);
        sysvar[num].str = str;
        sysvar[num].val = val;
    }
}

void
sysconf_addmenu (fp, key)
FILE *fp;
char *key;
{
    struct smenuitem *pm;
    char buf[256];
    char *cmd, *arg[5], *ptr;
    int n;

    sysconf_addkey (key, "menu", sysconf_menu);
    while (fgets (buf, sizeof (buf), fp) != NULL && buf[0] != '%')
    {
        cmd = strtok (buf, " \t\n");
        if (cmd == NULL || *cmd == '#')
        {
            continue;
        }
        arg[0] = arg[1] = arg[2] = arg[3] = arg[4] = "";
        n = 0;
        for (n = 0; n < 5; n++)
        {
            if ((ptr = strtok (NULL, ",\n")) == NULL)
                break;
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            if (*ptr == '"')
            {
                arg[n] = ++ptr;
                while (*ptr != '"' && *ptr != '\0')
                    ptr++;
                *ptr = '\0';
            }
            else
            {
                arg[n] = ptr;
                while (*ptr != ' ' && *ptr != '\t' && *ptr != '\0')
                    ptr++;
                *ptr = '\0';
            }
        }
        pm = &menuitem[sysconf_menu++];
        pm->line = sysconf_eval (arg[0]);
        pm->col = sysconf_eval (arg[1]);
        if (*cmd == '@')
        {
            pm->level = sysconf_eval (arg[2]);
            pm->name = sysconf_addstr (arg[3]);
            pm->desc = sysconf_addstr (arg[4]);
            pm->fptr = sysconf_addstr (cmd + 1);
            pm->arg = pm->name;
        }
        else if (*cmd == '!')
        {
            pm->level = sysconf_eval (arg[2]);
            pm->name = sysconf_addstr (arg[3]);
            pm->desc = sysconf_addstr (arg[4]);
            pm->fptr = sysconf_addstr ("domenu");
            pm->arg = sysconf_addstr (cmd + 1);
        }
        else
        {
            pm->level = -2;
            pm->name = sysconf_addstr (cmd);
            pm->desc = sysconf_addstr (arg[2]);
            pm->fptr = (void *) sysconf_buf;
            pm->arg = sysconf_buf;
        }
    }
    pm = &menuitem[sysconf_menu++];
    pm->name = pm->desc = pm->arg = sysconf_buf;
    pm->fptr = (void *) sysconf_buf;
    pm->level = -1;
}

void
sysconf_addblock (fp, key)
FILE *fp;
char *key;
{
    char buf[256];
    int num;

    if (sysconf_key < SC_KEYSIZE)
    {
        num = sysconf_key++;
        sysvar[num].key = sysconf_addstr (key);
        sysvar[num].str = sysconf_buf + sysconf_len;
        sysvar[num].val = -1;
        while (fgets (buf, sizeof (buf), fp) != NULL && buf[0] != '%')
        {
            encodestr (buf);
            strcpy (sysconf_buf + sysconf_len, buf);
            sysconf_len += strlen (buf);
        }
        sysconf_len++;
    }
    else
    {
        while (fgets (buf, sizeof (buf), fp) != NULL && buf[0] != '%')
        {}
    }
}

void
parse_sysconf (fname)
char *fname;
{
    FILE *fp;
    char buf[256];
    char tmp[256], *ptr;
    char *key, *str;
    int val;

    if ((fp = fopen (fname, "r")) == NULL)
    {
        return;
    }
    sysconf_addstr ("(null ptr)");
    while (fgets (buf, sizeof (buf), fp) != NULL)
    {
        ptr = buf;
        while (*ptr == ' ' || *ptr == '\t')
            ptr++;

        if (*ptr == '%')
        {
            strtok (ptr, " \t\n");
            if (strcmp (ptr, "%menu") == 0)
            {
                str = strtok (NULL, " \t\n");
                if (str != NULL)
                    sysconf_addmenu (fp, str);
            }
            else
            {
                sysconf_addblock (fp, ptr + 1);
            }
        }
        else if (*ptr == '#')
        {
            key = strtok (ptr, " \t\"\n");
            str = strtok (NULL, " \t\"\n");
            if (key != NULL && str != NULL && strcmp (key, "#include") == 0)
            {
                parse_sysconf (str);
            }
        }
        else if (*ptr != '\n')
        {
            key = strtok (ptr, "=#\n");
            str = strtok (NULL, "#\n");
            if (key != NULL & str != NULL)
            {
                strtok (key, " \t");
                while (*str == ' ' || *str == '\t')
                    str++;
                if (*str == '"')
                {
                    str++;
                    strtok (str, "\"");
                    val = atoi (str);
                    sysconf_addkey (key, str, val);
                }
                else
                {
                    val = 0;
                    strcpy (tmp, str);
                    ptr = strtok (tmp, ", \t");
                    while (ptr != NULL)
                    {
                        val |= sysconf_eval (ptr);
                        ptr = strtok (NULL, ", \t");
                    }
                    sysconf_addkey (key, NULL, val);
                }
            }
            else
            {
                report (ptr);
            }
        }
    }
    fclose (fp);
}

void
build_sysconf (configfile, imgfile)
char *configfile, *imgfile;
{
    struct smenuitem *old_menuitem;
    struct sdefine *old_sysvar;
    char *old_buf;
    int old_menu, old_key, old_len;
    struct sysheader
    {
        char *buf;
        int menu, key, len;
    }
    shead;
    int fh;

    old_menuitem = menuitem;
    old_menu = sysconf_menu;
    old_sysvar = sysvar;
    old_key = sysconf_key;
    old_buf = sysconf_buf;
    old_len = sysconf_len;
    menuitem = (void *) malloc (SC_CMDSIZE * sizeof (struct smenuitem));
    sysvar = (void *) malloc (SC_KEYSIZE * sizeof (struct sdefine));
    sysconf_buf = (void *) malloc (SC_BUFSIZE);
    sysconf_menu = 0;
    sysconf_key = 0;
    sysconf_len = 0;
    parse_sysconf (configfile);
    if ((fh = open (imgfile, O_WRONLY | O_CREAT, 0644)) > 0)
    {
        ftruncate (fh, 0);
        shead.buf = sysconf_buf;
        shead.menu = sysconf_menu;
        shead.key = sysconf_key;
        shead.len = sysconf_len;
        write (fh, &shead, sizeof (shead));
        write (fh, menuitem, sysconf_menu * sizeof (struct smenuitem));
        write (fh, sysvar, sysconf_key * sizeof (struct sdefine));
        write (fh, sysconf_buf, sysconf_len);
        close (fh);
    }
    free (menuitem);
    free (sysvar);
    free (sysconf_buf);
    menuitem = old_menuitem;
    sysconf_menu = old_menu;
    sysvar = old_sysvar;
    sysconf_key = old_key;
    sysconf_buf = old_buf;
    sysconf_len = old_len;
}

void
load_sysconf_image (imgfile)
char *imgfile;
{
    struct sysheader
    {
        char *buf;
        int menu, key, len;
    }
    shead;
    struct stat st;
    char *ptr, *func;
    int fh, n, diff, x;

    if ((fh = open (imgfile, O_RDONLY)) > 0)
    {
        fstat (fh, &st);
        ptr = malloc (st.st_size);
        read (fh, &shead, sizeof (shead));
        read (fh, ptr, st.st_size);
        close (fh);

        menuitem = (void *) ptr;
        ptr += shead.menu * sizeof (struct smenuitem);
        sysvar = (void *) ptr;
        ptr += shead.key * sizeof (struct sdefine);
        sysconf_buf = (void *) ptr;
        ptr += shead.len;
        sysconf_menu = shead.menu;
        sysconf_key = shead.key;
        sysconf_len = shead.len;
        /*
                sprintf( genbuf, "buf = %d, %d, %d", menuitem, sysvar, sysconf_buf );
                report( genbuf );
                sprintf( genbuf, "%d, %d, %d, %d, %s", shead.buf, shead.len,
                        shead.menu, shead.key, sysconf_buf );
                report( genbuf );
        */
        diff = sysconf_buf - shead.buf;
        for (n = 0; n < sysconf_menu; n++)
        {
            menuitem[n].name += diff;
            menuitem[n].desc += diff;
            menuitem[n].arg += diff;
            func = (char *) menuitem[n].fptr;
            menuitem[n].fptr = sysconf_funcptr (func + diff, &x);
        }
        for (n = 0; n < sysconf_key; n++)
        {
            sysvar[n].key += diff;
            sysvar[n].str += diff;
        }
    }
}

void
load_sysconf ()
{
    if (!dashf ("sysconf.img"))
    {
        report ("build sysconf.img");
        build_sysconf ("etc/sysconf.ini", "sysconf.img");
    }
    load_sysconf_image ("sysconf.img");
}

int
domenu_screen (pm)
struct smenuitem *pm;
{
    char *str;
    int line, col, num;

    /*    if(!DEFINE(DEF_NORMALSCR))  */
    clear ();
    line = 3;
    col = 0;
    num = 0;
    while (1)
    {
        switch (pm->level)
        {
        case -1:
            return (num);
        case -2:
            if (strcmp (pm->name, "title") == 0)
            {
                firsttitle (pm->desc);
            }
            else if (strcmp (pm->name, "screen") == 0)
            {
                if ((str = sysconf_str (pm->desc)) != NULL)
                {
                    move (pm->line, pm->col);
                    decodestr (str);
                }
            }
            break;
        default:
            if (pm->line >= 0 && HAS_PERM (pm->level))
            {
                if (pm->line == 0)
                {
                    pm->line = line;
                    pm->col = col;
                }
                else
                {
                    line = pm->line;
                    col = pm->col;
                }
                move (line, col);
                prints ("  %s", pm->desc);
                line++;
            }
            else
            {
                if (pm->line > 0)
                {
                    line = pm->line;
                    col = pm->col;
                }
                pm->line = -1;
            }
        }
        num++;
        pm++;
    }
}

int
domenu (menu_name)
char *menu_name;
{
    extern int refscreen;
    struct smenuitem *pm;
    int size, now;
    int cmd, i;

    if (sysconf_menu <= 0)
    {
        return -1;
    }
    pm = &menuitem[sysconf_eval (menu_name)];
    size = domenu_screen (pm);
    now = 0;
    if (strcmp (menu_name, "TOPMENU") == 0 && chkmail ())
    {
        for (i = 0; i < size; i++)
            if (pm[i].line > 0 && pm[i].name[0] == 'M')
                now = i;

    }
    modify_user_mode (MMENU);
    R_monitor ();
    while (1)
    {
        printacbar ();
        while (pm[now].level < 0 || !HAS_PERM (pm[now].level))
        {
            now++;
            if (now >= size)
                now = 0;
        }
        move (pm[now].line, pm[now].col);
        prints ("> ");
        move (pm[now].line, pm[now].col + 1);
        cmd = egetch ();
        move (pm[now].line, pm[now].col);
        prints ("  ");
        switch (cmd)
        {
        case EOF:
            if (!refscreen)
            {
                abort_bbs ();
            }
            domenu_screen (pm);
            modify_user_mode (MMENU);
            R_monitor ();
            break;
        case KEY_RIGHT:
            for (i = 0; i < size; i++)
            {
                if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
                        pm[i].col > pm[now].col && HAS_PERM (pm[i].level))
                    break;
            }
            if (i < size)
            {
                now = i;
                break;
            }
        case '\n':
        case '\r':
            if (strcmp (pm[now].arg, "..") == 0)
            {
                return 0;
            }
            if (pm[now].fptr != NULL)
            {
                int type;

                (void *) sysconf_funcptr (pm[now].name, &type);
                (*pm[now].fptr) (pm[now].arg);
                if (pm[now].fptr == Select)
                {
                    now++;
                }
                domenu_screen (pm);
                modify_user_mode (MMENU);
                R_monitor ();
            }
            break;
        case KEY_LEFT:
            for (i = 0; i < size; i++)
            {
                if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
                        pm[i].col < pm[now].col && HAS_PERM (pm[i].level))
                    break;
                if (pm[i].fptr == Goodbye)
                    break;
            }
            if (i < size)
            {
                now = i;
                break;
            }
            return 0;
        case KEY_DOWN:
            now++;
            break;
        case KEY_UP:
            now--;
            while (pm[now].level < 0 || !HAS_PERM (pm[now].level))
            {
                if (now > 0)
                    now--;
                else
                    now = size - 1;
            }
            break;
            // Modified by Flier - 2000.5.12 - Begin
        case KEY_PGUP:
            now = 0;
            break;
        case KEY_PGDN:
            now = size - 1;
            while (pm[now].level < 0 || !HAS_PERM (pm[now].level))
                now--;
            break;
            // Modified by Flier - 2000.5.12 - End
        case '~':
            if (!HAS_PERM (PERM_SYSOP))
            {
                break;
            }
            free (menuitem);
            report ("rebuild sysconf.img");
            build_sysconf ("etc/sysconf.ini", "sysconf.img");
            report ("reload sysconf.img");
            load_sysconf_image ("sysconf.img");
            pm = &menuitem[sysconf_eval (menu_name)];
            ActiveBoard_Init ();
            size = domenu_screen (pm);
            now = 0;
            break;
        case '!':		/* youzi leave */
            if (strcmp ("TOPMENU", menu_name) == 0)
                break;
            else
                return Goodbye ();
        case Ctrl('v'):
                        anywhere_lockscreen();
            update_endline();
            break;
        default:
            if (cmd >= 'a' && cmd <= 'z')
                cmd = cmd - 'a' + 'A';
            for (i = 0; i < size; i++)
    {
                if (pm[i].line > 0 && cmd == pm[i].name[0] &&
                        HAS_PERM (pm[i].level))
                {
                    now = i;
                    break;
                }
            }
        }
    }
}

// copy from admintool.c by hfzjz
//%%%%%%%added by aegis@011128
//mudboy modify, remove original addduserqzpost() and x_denypostqz
int
x_denypostqz ()
{

    char uident[STRLEN], ans[8], repbuf[STRLEN];
    char msgbuf[256], num[2];

    int count, whichdist, id;
    char tmpbname[STRLEN];
    static int currentpage;		//added by hongliang
    clear ();
    modify_user_mode (ADMIN);
    while (1)
    {
        clear ();
        if (currentuser.userlevel & PERM_BLEVELS)
        {
            num[0] = 0;
            num[1] = 0;
            getdata (1, 0, "你要编辑在哪一区被封全站的名单.", num, 2,
                     DOECHO, NA);
            if((num[0] == 'a') || (num[0] =='A'))
            {
                whichdist = 10;
                clear();
                prints ("设定 A 区的 全站的 禁止发文 名单\n");
            }
            else if((num[0] == 'b') || (num[0] =='B'))
            {
                whichdist = 11;
                clear();
                prints ("设定 B 区的 全站的 禁止发文 名单\n");
            }
			//debug by rwzmm @ sjtubbs
			//when ONOARDS denypost in district 0, autopost contents is wrong.
            else if (num[0] == '\0' || num[0] < '1' || num[0] > '9')
            {
                whichdist = 12;
                clear ();
                prints ("设定全站的 禁止发文 名单\n");
            }
            else
            {
                whichdist = num[0] - '0';
                clear ();
                prints ("设定 %d 区的 全站的 禁止发文 名单\n", whichdist);
            }
        }
        else
        {
            whichdist = WHICHDIST (currentuser.userid);
            if (whichdist > 12)
                return 0;
            prints ("设定全站的 禁止发文 名单\n");
        }
        sprintf (genbuf, "boards/deny_users_%d", whichdist);
        count = listfilecontent2 (genbuf, 3,currentpage);
        if (count)
        {
            getdata (1, 0,
                     "(A)增加 (D)删除 (C)改变 (J)下一页 (K)上一页 (E)离开[E]: ", ans, 7, DOECHO,
                     YEA);
        }
        else
        {
            getdata (1, 0, "(A)增加 (E)离开 (J)下一页 (K)上一页 [E]: ", ans, 7, DOECHO, YEA);
        }
        if (*ans == 'A' || *ans == 'a')
        {
            move (2, 0);
            usercomplete ("输入准备加入名单的使用者ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }
            if ((!(id = getuser (uident))) || (!strcasecmp (uident, "SYSOP")))
            {
                move (3, 0);
                prints ("错误的使用者代号,或者你无权封此ID全站发文权");
                pressanykey ();
                clear ();
                return 0;
            }
            if (!(lookupuser.userlevel & PERM_POST))
            {
                move (3, 0);
                prints ("%s 并没有全站发文权限\n", uident);
                pressanykey ();
                return 0;
            }
            if (addtodeny (uident, msgbuf, 0, whichdist * (-1)) == 1)
            {
                sprintf (repbuf,
                         "%s 被 %s 取消全站发文权限", uident,
                         currentuser.userid);
                securityreport (repbuf);
                lookupuser.userlevel &= ~PERM_POST;
                substitute_record (PASSFILE, &lookupuser,
                                   sizeof (lookupuser), id);
                setuserid (id, lookupuser.userid);
                if (msgbuf[0] != '\0')
                {
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
        }
        else if ((*ans == 'C' || *ans == 'c'))
        {
            move (2, 0);
            usercomplete ("请输入被修改者 ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }
            if (!(id = getuser (uident)))
            {
                move (3, 0);
                prints ("错误的使用者代号...");
                pressanykey ();
                clear ();
                return 0;
            }
            if (addtodeny (uident, msgbuf, 1, whichdist * (-1)) == 1)
            {
                sprintf (repbuf,
                         "%s 修改对 %s 被取消全站发文权限的处理",
                         currentuser.userid, uident);
                securityreport (repbuf);
                if (msgbuf[0] != '\0')
                {
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
        }
        else if ((*ans == 'D' || *ans == 'd') && count)
        {
            move (2, 0);
            usercomplete ("请输入要恢复全站发文权者 ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }
            // next remove by alt@sjtu
            /*
               if (!(id = getuser(uident))) {
               move(3, 0);
               prints("错误的使用者代号...");
               pressanykey();
               clear();
               return 0;
               }
             */
            if (SeekInNameList (uident))
            {
                if (deldeny (uident, whichdist * (-1))
                        && (id = getuser (uident)))
                {
                    sprintf (repbuf, "%s 恢复 %s 全站发文权限",
                             currentuser.userid, uident);
                    securityreport (repbuf);
                    lookupuser.userlevel |= PERM_POST;
                    substitute_record (PASSFILE, &lookupuser,
                                       sizeof (lookupuser), id);
                    setuserid (id, lookupuser.userid);
                    sprintf (msgbuf, "\n  %s 网友：\n\n"
                             "\t因封禁时间已过，现恢复你在全站的发文权力\n\n"
                             "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                             uident, currentuser.userid);
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
            else
            {
                prints (" %s 不在名单中。", uident);
                pressanykey ();
                return 0;
            }
        }
        else if ( *ans == 'J' || *ans == 'j' )
        {
            currentpage += 20;
        }
        else if ( *ans == 'K' || *ans == 'k' )
        {
            currentpage -= 20;
        }
        else
            break;
    }
    clear ();
    return FULLUPDATE;
}


//add by rwzmm @ sjtubbs, talk admin (ta) deny the talk right



int
x_denytalkta ()
{

    char uident[STRLEN], ans[8], repbuf[STRLEN];
    char msgbuf[256];
    int count, id;
    char tmpbname[STRLEN];
    static int currentpage;		//turn pages
    clear ();
    modify_user_mode (ADMIN);
    while (1)
    {
        clear ();


        if (!HAS_PERM(PERM_ACHATROOM))
            return DONOTHING;

        prints ("设定 禁止进入聊天室 的名单\n");

        sprintf (genbuf, "boards/deny_talk");
        count = listfilecontent2 (genbuf, 3,currentpage);
        if (count)
        {
            getdata (1, 0,
                     "(A)增加 (D)删除 (C)改变 (J)下一页 (K)上一页 (E)离开[E]: ", ans, 7, DOECHO,
                     YEA);
        }
        else
        {
            getdata (1, 0, "(A)增加 (E)离开 (J)下一页 (K)上一页 [E]: ", ans, 7, DOECHO, YEA);
        }
        if (*ans == 'A' || *ans == 'a')
        {
            move (2, 0);
            usercomplete ("输入准备加入名单的使用者ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }
            if ((!(id = getuser (uident))) || (!strcasecmp (uident, "SYSOP")))
            {
                move (3, 0);
                prints ("错误的使用者代号,或者你无权封此ID进入聊天室权限\n");
                pressanykey ();
                clear ();
                return 0;
            }
            if (!(lookupuser.userlevel & PERM_CHAT))
            {
                move (3, 0);
                prints ("%s 并没有进入聊天室权限\n", uident);
                pressanykey ();
                return 0;
            }
            if (addtodenytalk(uident, msgbuf, 0))
            {
                sprintf (repbuf,
                         "%s 被 %s 取消进入聊天室权限", uident,
                         currentuser.userid);
                securityreport (repbuf);
                lookupuser.userlevel &= ~PERM_CHAT;
                substitute_record (PASSFILE, &lookupuser,
                                   sizeof (lookupuser), id);
                setuserid (id, lookupuser.userid);
                if (msgbuf[0] != '\0')
                {
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
        }
        else if ((*ans == 'C' || *ans == 'c'))
        {
            move (2, 0);
            usercomplete ("请输入被修改者 ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }
            if (!(id = getuser (uident)))
            {
                move (3, 0);
                prints ("错误的使用者代号...");
                pressanykey ();
                clear ();
                return 0;
            }
            if (addtodenytalk(uident, msgbuf, 1))
            {
                sprintf (repbuf,
                         "%s 修改对 %s 被取消进入聊天室权限的处理",
                         currentuser.userid, uident);
                securityreport (repbuf);
                if (msgbuf[0] != '\0')
                {
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
        }
        else if ((*ans == 'D' || *ans == 'd') && count)
        {
            move (2, 0);
            usercomplete ("请输入要恢复进入聊天室权限者 ID: ", uident);
            if (*uident == '\0')
            {
                clear ();
                return 0;
            }

            if (SeekInNameList (uident))
            {
                if (deldenytalk(uident)
                        && (id = getuser (uident)))
                {
                    sprintf (repbuf, "%s 恢复 %s 进入聊天室权限",
                             currentuser.userid, uident);
                    securityreport (repbuf);
                    lookupuser.userlevel |= PERM_CHAT;
                    substitute_record (PASSFILE, &lookupuser,
                                       sizeof (lookupuser), id);
                    setuserid (id, lookupuser.userid);
                    sprintf (msgbuf, "\n  %s 网友：\n\n"
                             "\t因封禁时间已过，现恢复你进入聊天室的权力\n\n"
                             "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                             uident, currentuser.userid);
                    strcpy (tmpbname, currboard);
                    strcpy (currboard, DENY_POST_BOARDNAME);
                    autoreport (repbuf, msgbuf, uident, NULL);
                    strcpy (currboard, tmpbname);
                }
            }
            else
            {
                prints (" %s 不在名单中。", uident);
                pressanykey ();
                return 0;
            }
        }
        else if ( *ans == 'J' || *ans == 'j' )
        {
            currentpage += 20;
        }
        else if ( *ans == 'K' || *ans == 'k' )
        {
            currentpage -= 20;
        }
        else
            break;
    }
    clear ();
    return FULLUPDATE;
}



