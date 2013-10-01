#include "bbs.h"

#define MAXITEMS        1024
#define PATHLEN         256
#define A_PAGESIZE      (t_lines - 5)

#define ADDITEM         0
#define ADDGROUP        1
#define ADDMAIL         2
//#define ADDGOPHER       3


//add by rwzmm
#define ANN_ADD			0
#define ANN_DELETE		1
#define ANN_MOVE		2
//add end
char jhpath[1024];//added by chaser 

int bmonly = 0;
void a_menu (char *maintitle, char *path, int lastlevel, int lastbmonly);

extern char BoardName[];
void a_prompt (int bot, char *pmt, char *buf, int len);		/* added by netty */

typedef struct
{
    char title[72];
    char fname[80];
    char *host;
    int port;
}
ITEM;

int a_fmode = 0;

typedef struct
{
    ITEM *item[MAXITEMS];
    char mtitle[STRLEN];
    char *path;
    int num, page, now;
    int level;
}
MENU;

void
a_showmenu (MENU *pm)
{
    struct stat st;
    struct tm *pt;
    char title[STRLEN * 2], kind[20];
    char fname[STRLEN];
    char ch;
    char buf[STRLEN], genbuf[STRLEN * 2];
    time_t mtime;
    int n;
    char *ptr;
    int in_esc = 0, count = 0;

    // Add by wujian
    char counter[9];
    FILE *fp;

    sprintf (fname, "%s/counter.person", pm->path);
    fp = fopen (fname, "r");
    if (fp)
    {
        fscanf (fp, "%s", counter);
        fclose (fp);
    }
    else
        strcpy (counter, "< none >");
    ch = strlen (counter);
    for (n = ch; n >= 0; n--)
        counter[n + (9 - ch) / 2] = counter[n];
    for (n = (9 - ch) / 2; n > 0; n--)
        counter[n - 1] = ' ';
    // Add end.
    clear ();
    if (chkmail ())
    {
        prints ("[5m");
        //      sprintf(genbuf, "[ÄúÓĞĞÅ¼ş£¬°´ M ¿´ĞÂĞÅ]");
        sprintf (genbuf, "[ÄúÓĞĞÅ¼ş£¬°´ L ¿´ĞÂĞÅ]");
    }
    else
        strcpy (genbuf, pm->mtitle);
    sprintf (buf, "%*s", (80 - strlen (genbuf)) / 2, "");
    prints ("[1;44m%s%s%s[m\n", buf, genbuf, buf);
    prints
    ("           [1;32m F[37m ¼Ä»Ø×Ô¼ºµÄĞÅÏä  [32m¡ü¡ı[37m ÒÆ¶¯ [32m ¡ú <Enter> [37m¶ÁÈ¡ [32m ¡û,q[37m Àë¿ª[m\n");
    prints
    ("[1;44;37m ±àºÅ  %-20s[32m±¾Ä¿Â¼ÒÑ±»ä¯ÀÀ[33m%-9s[32m´Î[37m ×÷  Õß           %8s [m",
     "[Àà±ğ] ±ê    Ìâ", counter, a_fmode ? "µµ°¸Ãû³Æ" : "±à¼­ÈÕÆÚ");
    prints ("\n");
    if (pm->num == 0)
    {
        prints ("      << Ä¿Ç°Ã»ÓĞÎÄÕÂ >>\n");
    }
    for (n = pm->page; n < pm->page + A_PAGESIZE && n < pm->num; n++)
    {
        strcpy (title, pm->item[n]->title);
        sprintf (fname, "%s", pm->item[n]->fname);
        sprintf (genbuf, "%s/%s", pm->path, fname);
        ch = (dashd (genbuf) ? '/' : ' ');
        if (a_fmode && (pm->level & PERM_BOARDS) != 0)
        {
            fname[10] = '\0';
        }
        else
        {
            if (dashf (genbuf) || ch == '/')
            {
                stat (genbuf, &st);
                mtime = st.st_mtime;
            }
            else
                mtime = time (0);
            pt = localtime (&mtime);
            sprintf (fname, "[[1m%02d[m.[1m%02d[m.[1m%02d[m]",
                     pt->tm_year % 100, pt->tm_mon + 1, pt->tm_mday);
            ch = ' ';
        }
        if (pm->item[n]->host != NULL)
        {
            if (pm->item[n]->fname[0] == '0')
                strcpy (kind, "[[1;32mÁ¬ÎÄ[m]");
            else
                strcpy (kind, "[[1;33mÁ¬Ä¿[m]");
        }
        else if (dashf (genbuf))
        {
            strcpy (kind, "[[1;36mÎÄ¼ş[m]");
        }
        else if (dashd (genbuf))
        {
            strcpy (kind, "[[1;37mÄ¿Â¼[m]");
        }
        else
        {
            strcpy (kind, "[[1;32m´íÎó[m]");
        }
        //add by alt@sjtu ¾«»ªÇø²ÊÉ«Ä¿Â¼
        //{
        ptr = title;
        count = in_esc = 0;
        while (*ptr != '\0')
        {
            if (count == 55)
                break;
            if (*ptr == '')
            {
                in_esc = 1;
            }
            else if (in_esc)
            {
                if (strchr ("[0123456789;,", *ptr) == NULL)
                    in_esc = 0;
            }
            else
                count++;
            ptr++;
        }
        //}
        *ptr = '\0';
        if (!strncmp (title, "[Ä¿Â¼] ", 7) || !strncmp (title, "[ÎÄ¼ş] ", 7)
                || !strncmp (title, "[Á¬Ä¿] ", 7) || !strncmp (title, "[Á¬ÎÄ] ", 7))
            sprintf (genbuf, "%-s %-55s%-s%c", kind, title + 7, fname, ch);
        else
            sprintf (genbuf, "%-s %-55s%-s%c", kind, title, fname, ch);
        strncpy (title, genbuf, STRLEN * 2);
        title[STRLEN * 2 - 1] = '\0';
        prints ("  %3d  %s\n", n + 1, title);
    }
    clrtobot ();
    move (t_lines - 1, 0);
    prints ("%s", (pm->level & PERM_BOARDS) ?
            "[1;31;44m[°å  Ö÷]  [33mËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ĞÂÔöÎÄÕÂ a ©¦ ĞÂÔöÄ¿Â¼ g ©¦ ±à¼­µµ°¸ E        [m"
            :
            "[1;31;44m[¹¦ÄÜ¼ü] [33m ËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ÒÆ¶¯ÓÎ±ê k,¡ü,j,¡ı ©¦ ¶ÁÈ¡×ÊÁÏ Rtn,¡ú         [m");
}

void
a_additem (MENU * pm, char *title, char *fname, char *host, int port)
{
    ITEM *newitem;

    if (pm->num < MAXITEMS)
    {
        newitem = (ITEM *) malloc (sizeof (ITEM));
        strcpy (newitem->title, title);
        if (host != NULL)
        {
            newitem->host =
                (char *) malloc (sizeof (char) * (strlen (host) + 1));
            strcpy (newitem->host, host);
        }
        else
            newitem->host = host;
        newitem->port = port;
        strcpy (newitem->fname, fname);
        pm->item[(pm->num)++] = newitem;
    }
}
void
a_freeitem(ITEM *item)
{
	if(item->host)
		free(item->host);
	free(item);
}

int
a_loadnames (MENU * pm)
{
    FILE *fn;
    ITEM litem;
    char buf[PATHLEN], *ptr;
    char hostname[STRLEN];

    pm->num = 0;
    sprintf (buf, "%s/.Names", pm->path);
    if ((fn = fopen (buf, "r")) == NULL)
        return 0;
    hostname[0] = '\0';
    while (fgets (buf, sizeof (buf), fn) != NULL)
    {
        if ((ptr = strchr (buf, '\n')) != NULL)
            *ptr = '\0';
        if (strncmp (buf, "Name=", 5) == 0)
        {
            strncpy (litem.title, buf + 5, 72);
            litem.title[71] = '\0';
        }
        else if (strncmp (buf, "Path=", 5) == 0)
        {
            if (strncmp (buf, "Path=~/", 7) == 0)
                strncpy (litem.fname, buf + 7, 80);
            else
                strncpy (litem.fname, buf + 5, 80);
            litem.fname[79] = '\0';
            if ((!strstr (litem.title + 38, "(BM: BMS)")
                    || HAS_PERM (PERM_BOARDS))
                    && (!strstr (litem.title + 38, "(BM: SYSOPS)")
                        || HAS_PERM (PERM_SYSOP)))
            {
                a_additem (pm, litem.title, litem.fname,
                           (strlen (hostname) == 0) ? NULL : hostname,
                           litem.port);
            }
            hostname[0] = '\0';
        }
        else if (strncmp (buf, "# Title=", 8) == 0)
        {
            strncpy (pm->mtitle, buf + 8, STRLEN);
        }
        else if (strncmp (buf, "Host=", 5) == 0)
        {
            strncpy (hostname, buf + 5, STRLEN);
        }
        else if (strncmp (buf, "Port=", 5) == 0)
        {
            litem.port = atoi (buf + 5);
        }
    }
    fclose (fn);
    return 1;
}

void
a_savenames (MENU * pm)
{
    FILE *fn;
    ITEM *item;
    char fpath[PATHLEN];
    int n;

    sprintf (fpath, "%s/.Names", pm->path);
    if ((fn = fopen (fpath, "w")) == NULL)
        return;
    fprintf (fn, "#\n");
    if (!strncmp (pm->mtitle, "[Ä¿Â¼] ", 7)
            || !strncmp (pm->mtitle, "[ÎÄ¼ş] ", 7)
            || !strncmp (pm->mtitle, "[Á¬Ä¿] ", 7)
            || !strncmp (pm->mtitle, "[Á¬ÎÄ] ", 7))
    {
        fprintf (fn, "# Title=%s\n", pm->mtitle + 7);
    }
    else
    {
        fprintf (fn, "# Title=%s\n", pm->mtitle);
    }
    fprintf (fn, "#\n");
    for (n = 0; n < pm->num; n++)
    {
        item = pm->item[n];
        if (!strncmp (item->title, "[Ä¿Â¼] ", 7)
                || !strncmp (item->title, "[ÎÄ¼ş] ", 7)
                || !strncmp (item->title, "[Á¬Ä¿] ", 7)
                || !strncmp (item->title, "[Á¬ÎÄ] ", 7))
        {
            fprintf (fn, "Name=%s\n", item->title + 7);
        }
        else
            fprintf (fn, "Name=%s\n", item->title);
        if (item->host != NULL)
        {
            fprintf (fn, "Host=%s\n", item->host);
            fprintf (fn, "Port=%d\n", item->port);
            fprintf (fn, "Type=1\n");
            fprintf (fn, "Path=%s\n", item->fname);
        }
        else
            fprintf (fn, "Path=~/%s\n", item->fname);
        fprintf (fn, "Numb=%d\n", n + 1);
        fprintf (fn, "#\n");
    }
    fclose (fn);
    chmod (fpath, 0644);
}

void
a_prompt (int bot, char *pmt, char *buf, int len)
{
    saveline (t_lines + bot, 0);
    move (t_lines + bot, 0);
    clrtoeol ();
    getdata (t_lines + bot, 0, pmt, buf, len, DOECHO, YEA);
    saveline (t_lines + bot, 1);
}

/* added by netty to handle post saving into (0)Announce */
int
a_Save (char *path, char *key, struct fileheader *fileinfo, int nomsg, int full)
{

    char board[40];
    int ans = NA;

    if (!nomsg)
    {
        sprintf (genbuf, "È·¶¨½« [%-.40s] ´æÈëÔİ´æµµÂğ", fileinfo->title);
        if (askyn (genbuf, NA, YEA) == NA)
            return FULLUPDATE;
    }
    sprintf (board, "bm/%s", currentuser.userid);
    if (dashf (board))
    {
        if (nomsg)
            ans = YEA;
        else
            ans = askyn ("Òª¸½¼ÓÔÚ¾ÉÔİ´æµµÖ®ááÂğ", NA, YEA);
    }
    if (in_mail)
        sprintf (genbuf, "mail/%c/%c/%s/%s", toupper (currentuser.userid[0]), toupper (currentuser.userid[1]),
                 currentuser.userid, fileinfo->filename);
    else
        sprintf (genbuf, "boards/%s/%s/%s", key, getfolderstr(fileinfo->filename), fileinfo->filename);
    if (full)
        f_cp (genbuf, board, (ans) ? O_APPEND : O_CREAT);
    else
        part_cp (genbuf, board, (ans) ? "a+" : "w+");
    if (!nomsg)
        presskeyfor ("ÒÑ½«¸ÃÎÄÕÂ´æÈëÔİ´æµµ, Çë°´<Enter>¼ÌĞø...", t_lines - 1);
    return FULLUPDATE;
}

/* added by netty to handle post saving into (0)Announce */
int
a_Import (char *path, char *key, struct fileheader *fileinfo, char *direct, int nomsg)
{
    char fname[STRLEN], *ip, bname[PATHLEN];
    int ch;
    MENU pm;

    //FILE       *fn;

    modify_user_mode (DIGEST);
    pm.path = direct;
    strcpy (pm.mtitle, "");
    a_loadnames (&pm);
    strcpy (fname, fileinfo->filename);
    sprintf (bname, "%s/%s", pm.path, fname);
    ip = &fname[strlen (fname) - 1];
    while (dashf (bname))
    {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        sprintf (bname, "%s/%s", pm.path, fname);
    }
    sprintf (genbuf, "%-38.38s %s ", fileinfo->title, fileinfo->owner);
    a_additem (&pm, genbuf, fname, NULL, 0);
    a_savenames (&pm);
    if (in_mail)
        sprintf (genbuf, "mail/%c/%c/%s/%s", toupper (currentuser.userid[0]), toupper (currentuser.userid[1]),
                 currentuser.userid, fileinfo->filename);
    else
        sprintf (genbuf, "boards/%s/%s/%s", key, getfolderstr(fileinfo->filename), fileinfo->filename );
    f_cp (genbuf, bname, O_CREAT);
    if (!nomsg)
    {
        presskeyfor ("ÒÑ½«¸ÃÎÄÕÂ·Å½ø¾«»ªÇø, Çë°´<Enter>¼ÌĞø...", t_lines - 1);
    }
    for (ch = 0; ch < pm.num; ch++)
        a_freeitem(pm.item[ch]);
	if(!in_mail)
	{
		write_ann_trace(key, currentuser.userid, ANN_ADD, fileinfo->title, direct);
		//here for ¾­ÑéÖµ to add
	}
    return 1;
}

// deardragon 0921

int
a_menusearch (char *path, char *key, char *found)
{
    FILE *fn;
    char flag = '0';
    char buf[PATHLEN], *ptr;
    int searchmode = 0;

    if ('\0' == key[0])
    {
//        key = bname;//modified by chaser 06.05.23
        a_prompt (-1, "ÊäÈëÓûËÑÑ°Ö®ÌÖÂÛÇøÃû³Æ: ", key, 18);
        searchmode = 1;
    }
    sprintf (buf, "0Announce/.Search");
    if (key[0] != '\0' && (fn = fopen (buf, "r")) != NULL)
    {
        while (fgets (buf, sizeof (buf), fn) != NULL)
        {
            if (searchmode && !strstr (buf, "groups/"))
                continue;
            ptr = strchr (buf, ':');
            if (!ptr)
                return 0;
            else
            {
                *ptr = '\0';
                ptr = strtok (ptr + 1, " \t\n");
            }
            if (!strcasecmp (buf, key))
            {
                sprintf (found, "0Announce/%s", ptr);
                flag = '1';
                break;
            }
        }
        fclose (fn);
        if (flag == '0')
        {
            //a_prompt(-1, "ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌĞø...", buf, 2);
            presskeyfor ("ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌĞø...",
                         t_lines - 1);
            return 0;
        }
        return 1;
    }
    return 0;
}

#ifdef INTERNET_EMAIL
void
a_forward (char *path, ITEM *pitem, int mode)
{
    struct shortfile fhdr;
    char fname[PATHLEN], *mesg;

    sprintf (fname, "%s/%s", path, pitem->fname);
    if (dashf (fname))
    {
        strncpy (fhdr.title, pitem->title, STRLEN);
        strncpy (fhdr.filename, pitem->fname, STRLEN);
        fhdr.title[STRLEN - 1] = '\0';
        fhdr.filename[STRLEN - 1] = '\0';
        switch (doforward (path, &fhdr, mode))
        {
        case 0:
            mesg = "ÎÄÕÂ×ª¼ÄÍê³É!\n";
            break;
        case -1:
            mesg = "System error!!.\n";
            break;
        case -2:
            mesg = "Invalid address.\n";
            break;
        default:
            mesg = "È¡Ïû×ª¼Ä¶¯×÷.\n";
        }
        prints (mesg);
    }
    else
    {
        move (t_lines - 1, 0);
        prints ("ÎŞ·¨×ª¼Ä´ËÏîÄ¿.\n");
    }
    pressanykey ();
}
#endif

void
a_newitem (MENU *pm, int mode)
{
    char uident[STRLEN];
    char board[STRLEN], title[STRLEN];
    char fname[STRLEN], fpath[PATHLEN], fpath2[PATHLEN];
    char *mesg;
    FILE *pn;
    char /*ans[10], */head;


    pm->page = 9999;
    head = 'X';
    switch (mode)
    {
    case ADDITEM:
        head = 'A';		/* article */
        break;
    case ADDGROUP:
        head = 'D';		/* directory */
        break;
    case ADDMAIL:
        sprintf (board, "bm/%s", currentuser.userid);
        if (!dashf (board))
        {
            presskeyfor ("ÇëÏÈÖÁ¸ÃÌÖÂÛÇø½«ÎÄÕÂ´æÈëÔİ´æµµ, °´<Enter>¼ÌĞø...",
                         t_lines - 1);
            return;
        }
        mesg = "ÇëÊäÈëµµ°¸Ö®Ó¢ÎÄÃû³Æ(¿Éº¬Êı×Ö)£º";
        break;
//    case ADDGOPHER:
        //        {
        //            int gport;
        //            char ghost[STRLEN], gtitle[STRLEN], gfname[STRLEN];
        //
        //            a_prompt (-2, "Á¬ÏßµÄÎ»Ö·£º", ghost, STRLEN - 14);
        //            if (ghost[0] == '\0')
        //                return;
        //
        //            /* edwardc.990427 fix up .. @_@ terrible code */
        //
        //            a_prompt (-2, "Á¬ÏßµÄÄ¿Â¼£º(¿É°´ Enter Ô¤Éè£©", gfname, STRLEN - 14);
        //            /* ÈÃÁ¬ÏßÄ¿Â¼Ô¤ÉèÎª¿Õ×Ö´®£¬´ó¶à¿ÉÁ¬½Ó */
        //            if (gfname[0] == '\0')
        //                gfname[0] = '\0';
        //
        //            a_prompt (-2, "Á¬ÏßµÄPort£º(Ô¤ÉèÎª 70£©", ans, 6);
        //            if (ans[0] == '\0')
        //                strcpy (ans, "70");
        //
        //            gport = atoi (ans);
        //
        //            a_prompt (-2, "±êÌâ£º£¨Ô¤ÉèÎªÁ¬ÏßÎ»Ö·)", gtitle, 70);
        //            if (gtitle[0] == '\0')
        //                strncpy (gtitle, ghost, STRLEN);
        //
        //            a_additem (pm, gtitle, gfname, ghost, gport);
        //            a_savenames (pm);
        //            return;
        //        }
        
    }
    /* edwardc.990320 system will assign a filename for you .. */
    sprintf (fname, "%c%lX", head, time (0) + getpid () + getppid () + rand ());
    sprintf (fpath, "%s/%s", pm->path, fname);
    mesg = "ÇëÊäÈëÎÄ¼ş»òÄ¿Â¼Ö®ÖĞÎÄÃû³Æ£º ";
    a_prompt (-1, mesg, title, 35);
    if (*title == '\0')
        return;
    switch (mode)
    {
    case ADDITEM:
        if (vedit (fpath, 0, YEA) == -1)
            return;
        chmod (fpath, 0644);
        break;
    case ADDGROUP:
        if (strlen (fpath) >= 230)
        {
            //a_prompt(-1,"¶Ô²»Æğ, Ä¿Â¼²ã´ÎÌ«Éî, È¡Ïû²Ù×ö!",ans,2);
            presskeyfor ("¶Ô²»Æğ, Ä¿Â¼²ã´ÎÌ«Éî, È¡Ïû²Ù×ö!", t_lines - 1);
            return;
        }
        mkdir (fpath, 0755);
        chmod (fpath, 0755);
        break;
    case ADDMAIL:
        rename (board, fpath);
        break;
    }
    if (mode != ADDGROUP)
        sprintf (genbuf, "%-38.38s %s ", title, currentuser.userid);
    else
    {
        /*Add by SmallPig*/
        if (HAS_PERM (PERM_SYSOP) || HAS_PERM (PERM_ANNOUNCE))
        {
            move (1, 0);
            clrtoeol ();
            getdata (1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
            if (uident[0] != '\0')
                sprintf (genbuf, "%-38.38s(BM: %s)", title, uident);
            else
                sprintf (genbuf, "%-38.38s", title);
        }
        else
            sprintf (genbuf, "%-38.38s", title);
    }
    a_additem (pm, genbuf, fname, NULL, 0);
    a_savenames (pm);
    if (mode == ADDGROUP)
    {
        sprintf (fpath2, "%s/%s/.Names", pm->path, fname);
        if ((pn = fopen (fpath2, "w")) != NULL)
        {
            fprintf (pn, "#\n");
            fprintf (pn, "# Title=%s\n", genbuf);
            fprintf (pn, "#\n");
            fclose (pn);
        }
    }
}

void
a_moveitem (MENU *pm)
{
    ITEM *tmp;
    char newnum[STRLEN];
    int num, n;

    sprintf (genbuf, "ÇëÊäÈëµÚ %d ÏîµÄĞÂ´ÎĞò: ", pm->now + 1);
    a_prompt (-2, genbuf, newnum, 6);
    num = (newnum[0] == '$') ? 9999 : atoi (newnum) - 1;
    if (num >= pm->num)
        num = pm->num - 1;
    else if (num < 0)
        return;
    tmp = pm->item[pm->now];
    if (num > pm->now)
    {
        for (n = pm->now; n < num; n++)
            pm->item[n] = pm->item[n + 1];
    }
    else
    {
        for (n = pm->now; n > num; n--)
            pm->item[n] = pm->item[n - 1];
    }
    pm->item[num] = tmp;
    pm->now = num;
    a_savenames (pm);
}

void
a_copypaste (MENU *pm, int paste)
{
    static char title[STRLEN], filename[STRLEN], fpath[PATHLEN];
    ITEM *item;
    char newpath[PATHLEN] /*,ans[5] */ ;
    FILE *fn;

    move (t_lines - 1, 0);
    sethomefile (fpath, currentuser.userid, ".copypaste");

    if (!paste)
    {
        fn = fopen (fpath, "w+");
        item = pm->item[pm->now];
        fwrite (item->title, sizeof (item->title), 1, fn);
        fwrite (item->fname, sizeof (item->fname), 1, fn);
        sprintf (genbuf, "%s/%s", pm->path, item->fname);
        strncpy (fpath, genbuf, PATHLEN);
        fpath[PATHLEN - 1] = '\0';
        fwrite (fpath, sizeof (fpath), 1, fn);
        fclose (fn);
        //a_prompt(-1,"µµ°¸±êÊ¶Íê³É. (×¢Òâ! Õ³ÌùÎÄÕÂáá²ÅÄÜ½«ÎÄÕÂ delete!)",ans,2);
        presskeyfor ("µµ°¸±êÊ¶Íê³É. (×¢Òâ! Õ³ÌùÎÄÕÂáá²ÅÄÜ½«ÎÄÕÂ delete!)",
                     t_lines - 1);
    }
    else
    {
        if ((fn = fopen (fpath /*genbuf */ , "r")) == NULL)
        {
            //a_prompt(-1,"ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",ans,2);
            presskeyfor ("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.", t_lines - 1);
            return;
        }
        fread (title, sizeof (item->title), 1, fn);
        fread (filename, sizeof (item->fname), 1, fn);
        fread (fpath, sizeof (fpath), 1, fn);
        fclose (fn);
        sprintf (newpath, "%s/%s", pm->path, filename);
        if (*title == '\0' || *filename == '\0')
        {
            //a_prompt(-1,"ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî. ",ans,2);
            presskeyfor ("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.", t_lines - 1);
        }
        else if (!(dashf (fpath) || dashd (fpath)))
        {
            sprintf (genbuf,
                     "Äã¿½±´µÄ%sµµ°¸/Ä¿Â¼²»´æÔÚ,¿ÉÄÜ±»É¾³ı,È¡ÏûÕ³Ìù.",
                     filename);
            //a_prompt(-1,genbuf,ans,2);
            presskeyfor (genbuf, t_lines - 1);
        }
        else if (dashf (newpath) || dashd (newpath))
        {
            sprintf (genbuf, "%s µµ°¸/Ä¿Â¼ÒÑ¾­´æÔÚ. ", filename);
            //a_prompt(-1,genbuf,ans,2);
            presskeyfor (genbuf, t_lines - 1);
        }
        else if (strstr (newpath, fpath) != NULL)
        {
            //a_prompt(-1,"ÎŞ·¨½«Ä¿Â¼°á½ø×Ô¼ºµÄ×ÓÄ¿Â¼ÖĞ, »áÔì³ÉËÀ»ØÈ¦.",ans,2);
            presskeyfor ("ÎŞ·¨½«Ä¿Â¼°á½ø×Ô¼ºµÄ×ÓÄ¿Â¼ÖĞ, »áÔì³ÉËÀ»ØÈ¦.",
                         t_lines - 1);
        }
        else
        {			/*
            				   sprintf(genbuf, "ÄúÈ·¶¨ÒªÕ³Ìù %s µµ°¸/Ä¿Â¼Âğ", filename);
            				   if (askyn(genbuf, NA, YEA) == YEA) { */
            sprintf (genbuf, "/bin/cp -r %s %s", fpath, newpath);
            system (genbuf);
            a_additem (pm, title, filename, NULL, 0);
            a_savenames (pm);
            //}
        }
    }
    pm->page = 9999;
}

void
a_delete (MENU * pm)
{
    ITEM *item;
    char fpath[PATHLEN], ans[5];
    int n;
    FILE *fn;

    item = pm->item[pm->now];
    move (t_lines - 2, 0);
    prints ("%5d  %-50s\n", pm->now + 1, item->title);
    if (item->host == NULL)
    {
        sethomefile (fpath, currentuser.userid, ".copypaste");
        if ((fn = fopen (fpath /*genbuf */ , "r")) != NULL)
        {
            fread (genbuf, sizeof (item->title), 1, fn);
            fread (genbuf, sizeof (item->fname), 1, fn);
            fread (genbuf, sizeof (fpath), 1, fn);
            fclose (fn);
        }
        sprintf (fpath, "%s/%s", pm->path, item->fname);
        if (!strncmp (genbuf, fpath, sizeof (fpath)))
        {
            a_prompt (-1, "[1;5;31m¾¯¸æ[m: ¸Ãµµ°¸/Ä¿Â¼ÊÇ±»°åÖ÷×öÁË"
                      "±ê¼Ç, [1;33mÈç¹ûÉ¾³ı, Ôò²»ÄÜÔÙÕ³Ìù¸ÃÎÄÕÂ!![m", ans, 2);
        }
        if (dashf (fpath))
        {
            if (askyn ("É¾³ı´Ëµµ°¸, È·¶¨Âğ", NA, YEA) == NA)
                return;
            unlink (fpath);
        }
        else if (dashd (fpath))
        {
            if (askyn ("É¾³ıÕû¸ö×ÓÄ¿Â¼, ±ğ¿ªÍæĞ¦Å¶, È·¶¨Âğ", NA, YEA) == NA)
                return;
            deltree (fpath);
        }
    }
    else
    {
        if (askyn ("É¾³ıÁ¬ÏßÑ¡Ïî, È·¶¨Âğ", NA, YEA) == NA)
            return;
    }
    a_freeitem(item);
    (pm->num)--;
    for (n = pm->now; n < pm->num; n++)
        pm->item[n] = pm->item[n + 1];
    a_savenames (pm);
}

void
a_newname (MENU *pm)
{
    ITEM *item;
    char fname[STRLEN];
    char fpath[PATHLEN];
    char *mesg;

    item = pm->item[pm->now];
    a_prompt (-2, "ĞÂµµÃû: ", fname, 12);
    if (*fname == '\0')
        return;
    sprintf (fpath, "%s/%s", pm->path, fname);
    if (!valid_fname (fname))
    {
        mesg = "²»ºÏ·¨µµ°¸Ãû³Æ.";
    }
    else if (dashf (fpath) || dashd (fpath))
    {
        mesg = "ÏµÍ³ÖĞÒÑÓĞ´Ëµµ°¸´æÔÚÁË.";
    }
    else
    {
        sprintf (genbuf, "%s/%s", pm->path, item->fname);
        if (rename (genbuf, fpath) == 0)
        {
            strcpy (item->fname, fname);
            a_savenames (pm);
            return;
        }
        mesg = "µµÃû¸ü¸ÄÊ§°Ü!!";
    }
    prints (mesg);
    egetch ();
}

void
a_manager (MENU * pm, int ch)
{
	int opt;
	char msg[2];
	char fname[STRLEN];
    char uident[STRLEN];
    ITEM *item = NULL;
    MENU xpm;
    char fpath[PATHLEN], changed_T[STRLEN];	//, ans[5];

    if (pm->num > 0)
    {
        item = pm->item[pm->now];
        sprintf (fpath, "%s/%s", pm->path, item->fname);
    }
    switch (ch)
    {
    case 'a':
        a_newitem (pm, ADDITEM);
        break;
    case 'g':
        a_newitem (pm, ADDGROUP);
        break;
    case 'i':
        a_newitem (pm, ADDMAIL);
        break;

    case 'p':
        a_copypaste (pm, 1);
        break;
    case 'S'://added by chaser to show all Ë¿Â·s
	{
		FILE *fn;
		int i=0;
		move(0,0);
		clear();
		prints("Ë¿Â·ÉèÖÃÇé¿öÈçÏÂËùÊ¾: ");
		while(i<4)
		{
			pm->page=9999;
			if(0 == i)
			{
				sprintf(fname,".fpp");
				sethomefile (genbuf,currentuser.userid,fname);
			}
			else
			{
				sprintf(fname,".fpp%d",i);
				sethomefile (genbuf,currentuser.userid,fname);
			}
			fn=fopen(genbuf,"r");
			move(i+1,0);
			if(fn)
			{
					char tmpath[1000];
					if(fgets(tmpath,1000,fn)==NULL)
					{
						 sprintf(genbuf,"µÚ%dË¿Â·: ",i+1);
					} 
					else
                               	        {
						sprintf (genbuf, "µÚ%dË¿Â·: %s",i+1,tmpath);
					}
					fclose(fn);
			}
			else
			{
				sprintf(genbuf,"µÚ%dË¿Â·: %s",i+1," ÉĞÎ´ÉèÖÃ");	
			}
			prints(genbuf);
			i++;
		}
		pressanykey();
		break;
	}
/*    case 'S':
        {
			getdata(  t_lines - 1, 0,"ÇëÊäÈë²Ù×÷Ë¿Â·±àºÅ 1-4(Ä¬ÈÏÎªµÚÒ»Ë¿Â·):", msg, 2, DOECHO,  YEA);
			
			if (msg[0] == '\n' || msg[0] == '\0')
				msg[0] = '1';
			opt = atoi(msg) -1;
			if(!isdigit(msg[0]) || opt <0 || opt > 3 )
			{
				presskeyfor ("ÊäÈë´íÎó, Çë°´<Enter>¼ÌĞø...",t_lines - 1);
			}
			else
			{
				FILE *fn;
				pm->page = 9999;
				if(opt==0)
				{
					sethomefile (genbuf, currentuser.userid, ".announcepath");
				}
				else
				{
					sprintf(fname,".announcepath%d",opt);	
					sethomefile (genbuf, currentuser.userid, fname);
				}
				
				fn = fopen (genbuf, "r");
				if (fn)
				{
					char tmpath[1000];
					
					fgets (tmpath, 1000, fn);
					fclose (fn);
					sprintf (genbuf, "µ±Ç°Ë¿Â·: %s", tmpath);
				}
				else
					strcpy (genbuf, "Äú»¹Ã»ÓĞÓÃ f ÉèÖÃË¿Â·.");
				presskeyfor (genbuf, t_lines - 1);
			}
            break;
        }*/
    case 'f':
        {
			getdata(t_lines - 1, 0,"ÇëÊäÈë²Ù×÷Ë¿Â· 1-4(Ä¬ÈÏÎªµÚÒ»Ë¿Â·):", msg, 2, DOECHO,  YEA);
			opt = atoi(msg);
			if (msg[0] == '\n' || msg[0] == '\0')
				msg[0] = '1';
			opt = atoi(msg)-1;
			if(!isdigit(msg[0]) || opt <0 || opt > 3 )
			{
				presskeyfor ("ÊäÈë´íÎó, Çë°´<Enter>¼ÌĞø...",t_lines - 1);
			}

			else
			{
				FILE *fn;
				FILE *fm;
				char jh[256];
				pm->page = 9999;
				if(opt == 0)
				{
					sprintf(fname,".announcepath");
					sprintf(jh,".fpp");	
				}
				else
				{
					sprintf(fname,".announcepath%d",opt);
					sprintf(jh,".fpp%d",opt);	
				}
				sethomefile (genbuf, currentuser.userid, fname);
				fn = fopen (genbuf, "w+");
				sethomefile(genbuf,currentuser.userid,jh);
				fm = fopen(genbuf,"w+");
				if (fn&&fm)
				{
					fprintf (fn, "%s", pm->path);
					fclose (fn);
					fprintf (fm, "%s",jhpath);
					fclose (fm);
					presskeyfor ("ÒÑ½«¸ÃÂ·¾¶ÉèÎªË¿Â·, Çë°´<Enter>¼ÌĞø...",						t_lines - 1);
				}
				
/*				fn = fopen (genbuf, "w+");
				if (fn)
				{
					fprintf (fn, "%s", pm->path);
					fclose (fn);
				}*/
			}
			break;
		}
	}
    if (pm->num <= 0)
        return;
    switch (ch)
    {
    case 's':
        a_fmode = !a_fmode;
        pm->page = 9999;
        break;
    case 'm':
        a_moveitem (pm);
        pm->page = 9999;
        break;
    case 'd':
    case 'D':
        a_delete (pm);
        pm->page = 9999;
        break;
    case 'V':
    case 'v':
        if (HAS_PERM (PERM_SYSOP))
        {
            if (ch == 'v')
                sprintf (fpath, "%s/.Names", pm->path);
            else
                sprintf (fpath, "0Announce/.Search");
            if (dashf (fpath))
            {
                modify_user_mode (EDITANN);
                vedit (fpath, 0, YEA);
                modify_user_mode (DIGEST);
            }
            pm->page = 9999;
        }
        break;
    case 't':
        strncpy (changed_T, item->title, 72);
        getdata (t_lines - 2, 0, "ĞÂ±êÌâ: ", changed_T, 72, DOECHO, NA);
        if (*changed_T)
        {
            strncpy (item->title, changed_T, 72);
            item->title[71] = '\0';
            if (dashd (fpath))
            {
                xpm.path = fpath;
                a_loadnames (&xpm);
                strcpy (xpm.mtitle, changed_T);
                a_savenames (&xpm);
            }
            a_savenames (pm);
        }
        pm->page = 9999;
        break;
    case 'T':
        a_prompt (-2, "ĞÂ±êÌâ: ", changed_T, 35);
        if (*changed_T)
        {
            if (dashf (fpath))
            {
                sprintf (genbuf, "%-38.38s %s ", changed_T, currentuser.userid);
                strncpy (item->title, genbuf, 72);
                item->title[71] = '\0';
            }
            else if (dashd (fpath))
            {
                if (HAS_PERM (PERM_SYSOP) || HAS_PERM (PERM_ANNOUNCE))
                {
                    move (1, 0);
                    clrtoeol ();
                    getdata (1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
                    if (uident[0] != '\0')
                        sprintf (genbuf, "%-38.38s(BM: %s)", changed_T, uident);
                    else
                        sprintf (genbuf, "%-38.38s", changed_T);
                }
                else
                    sprintf (genbuf, "%-38.38s", changed_T);
                xpm.path = fpath;
                a_loadnames (&xpm);
                strcpy (xpm.mtitle, genbuf);
                a_savenames (&xpm);
                strncpy (item->title, genbuf, 72);
            }
            else if (pm->item[pm->now]->host != NULL)
                strncpy (item->title, changed_T, 72);
            item->title[71] = '\0';
            a_savenames (pm);
        }
        pm->page = 9999;
        break;
    case 'E':
        if (dashf (fpath))
        {
            modify_user_mode (EDITANN);
            vedit (fpath, 0, YEA);
            modify_user_mode (DIGEST);
        }
        pm->page = 9999;
        break;
    case 'n':
        a_newname (pm);
        pm->page = 9999;
        break;
    case 'c':
        a_copypaste (pm, 0);
        break;
    }
}

void
a_menu (char *maintitle, char *path, int lastlevel, int lastbmonly)
{
    MENU me;
    FILE *fp;
    char fname[PATHLEN], tmp[STRLEN], *bmstr, buf[STRLEN];
    int ch, bmonly, number = 0, savemode, counter = 0;
    time_t now = time (0), timetotal = 0;
    char something[PATHLEN + 20];	//¾«»ªÇø¼ÆÊıÆ÷¡¢±¸ÍüÂ¼ add by wujian

    modify_user_mode (DIGEST);
    strcpy (something, path);
    strcat (something, "/welcome");
    me.path = path;
    show_help (something);
    strcpy (me.mtitle, maintitle);
    me.level = lastlevel;
    bmonly = lastbmonly;
    a_loadnames (&me);

    memset (buf, 0, STRLEN);
    strncpy (buf, me.mtitle, STRLEN);
    bmstr = strstr (buf + 38, "(BM:");
    if (bmstr != NULL)
    {
        if (chk_currBM (bmstr + 4))
            me.level |= PERM_BOARDS;
        else if (bmonly == 1 && !HAS_PERM (PERM_BOARDS))
            return;
    }
    //nextline added by alt@02.04.18
    else if (chk_currBM (""))
        me.level |= PERM_BOARDS;

    if (strstr (me.mtitle, "(BM: BMS)") || strstr (me.mtitle, "(BM: SECRET)")
            || strstr (me.mtitle, "(BM: SYSOPS)"))
        bmonly = 1;

    strcpy (buf, me.mtitle);
    bmstr = strstr (buf, "(BM:");

    me.page = 9999;
    me.now = 0;
    while (1)
    {
        if (me.now >= me.num && me.num > 0)
        {
            me.now = me.num - 1;
        }
        else if (me.now < 0)
        {
            me.now = 0;
        }
        if (me.now < me.page || me.now >= me.page + A_PAGESIZE)
        {
            me.page = me.now - (me.now % A_PAGESIZE);
            a_showmenu (&me);
        }
        move (3 + me.now - me.page, 0);
        prints ("->");
        ch = egetch ();
        move (3 + me.now - me.page, 0);
        prints ("  ");
        if (ch == 'Q' || ch == 'q' || ch == KEY_LEFT || ch == EOF)
	{
	    char *tmp;
	    if((tmp=strrchr(jhpath,'/'))!=NULL)
		*tmp='\0';
            break;
	}
EXPRESS:			/* Leeward 98.09.13 */
        switch (ch)
        {
        case KEY_UP:
        case 'K':
        case 'k':
            if (--me.now < 0)
                me.now = me.num - 1;
            break;
        case KEY_DOWN:
        case 'J':
        case 'j':
            if (++me.now >= me.num)
                me.now = 0;
            break;
        case KEY_PGUP:
        case Ctrl ('B'):
                        if (me.now >= A_PAGESIZE)
                            me.now -= A_PAGESIZE;
                else if (me.now > 0)
                    me.now = 0;
                else
                    me.now = me.num - 1;
            break;
        case KEY_PGDN:
        case Ctrl ('F'):
                    case ' ':
                            if (me.now < me.num - A_PAGESIZE)
                                me.now += A_PAGESIZE;
                    else if (me.now < me.num - 1)
                        me.now = me.num - 1;
                    else
                        me.now = 0;
            break;
        case Ctrl ('C'):
                        if (me.num == 0)
                            break;
            set_safe_record ();
            if (!HAS_PERM (PERM_POST))
                break;
            sprintf (fname, "%s/%s", path, me.item[me.now]->fname);
            if (!dashf (fname))
                break;
            if (me.now < me.num)
    {
                char bname[30];

                clear ();
                if (get_a_boardname (bname, "ÇëÊäÈëÒª×ªÌùµÄÌÖÂÛÇøÃû³Æ: "))
                {
                    move (1, 0);
                    if (deny_me (bname))
                    {
                        prints ("\n\nÄúÉĞÎŞÈ¨ÏŞÔÚ %s ·¢±íÎÄÕÂ.", bname);
                        pressreturn ();
                        me.page = 9999;
                        break;
                    }
                    sprintf (tmp, "ÄãÈ·¶¨Òª×ªÌùµ½ %s °åÂğ", bname);
                    if (askyn (tmp, NA, NA) == 1)
                    {
                        Postfile (fname, bname, me.item[me.now]->title, 0);
                        move (3, 0);
                        sprintf (tmp, "[1mÒÑ¾­°ïÄã×ªÌùÖÁ %s °åÁË[m", bname);
                        prints (tmp);
                        refresh ();
                        sleep (1);
                    }
                }
                me.page = 9999;
            }
            show_message (NULL);
            break;
        case 'M':
            savemode = uinfo.mode;
            m_new ();
            modify_user_mode (savemode);
            me.page = 9999;
            break;
        case 'L':
            savemode = uinfo.mode;
            m_read ();
            modify_user_mode (savemode);
            me.page = 9999;
            break;
        case 'h':
            show_help ("help/announcereadhelp");
            me.page = 9999;
            break;
        case '\n':
        case '\r':
            if (number > 0)
            {
                me.now = number - 1;
                number = 0;
                continue;
            }
        case 'R':
        case 'r':
        case KEY_RIGHT:
            if (me.now < me.num)
            {
                if (me.item[me.now]->host != NULL)
                {
                    //if (me.item[me.now]->fname[0] == '0')
                    //                    {
                    //                        if (get_con
                    //                                (me.item[me.now]->host,
                    //                                 me.item[me.now]->port) != -1)
                    //                        {
                    //                            char tmpfile[30];
                    //                            GOPHER tmp;
                    //                            extern GOPHER *tmpitem;
                    //
                    //                            tmpitem = &tmp;
                    //                            strcpy (tmp.server, me.item[me.now]->host);
                    //                            strcpy (tmp.file, me.item[me.now]->fname);
                    //                            sprintf (tmp.title, "0%s", me.item[me.now]->title);
                    //                            tmp.port = me.item[me.now]->port;
                    //                            enterdir (me.item[me.now]->fname);
                    //                            setuserfile (tmpfile, "gopher.tmp");
                    //                            savetmpfile (tmpfile);
                    //                            ansimore (tmpfile, YEA);
                    //                            unlink (tmpfile);
                    //                        }
                    //                    }
                    //                    else
                    //                    {
                    //                        gopher (me.item[me.now]->host,
                    //                                me.item[me.now]->fname,
                    //                                me.item[me.now]->port, me.item[me.now]->title);
                    //                    }
                    
                    me.page = 9999;
                    break;
                }
                else
                    sprintf (fname, "%s/%s", path, me.item[me.now]->fname);
                if (dashf (fname))
                {
                    /* Leeward 98.09:ÔÚ¾«»ªÇøÔÄ¶ÁÎÄÕÂµ½Ä©Î²Ê±£¬ÓÃÉÏ£¯ÏÂ¼ıÍ·Ö±½ÓÌø×ªµ½Ç°£¯ºóÒ»Ïî */
                    if (ansimore (fname, NA) == KEY_UP)
                    {
                        if (--me.now < 0)
                            me.now = me.num - 1;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    }
                    prints
                    ("[1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ı [m");
                    switch (ch = egetch ())
                    {
                    case KEY_DOWN:
                    case ' ':
                    case '\n':
                        if (++me.now >= me.num)
                            me.now = 0;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    case KEY_UP:
                    case 'u':
                    case 'U':
                        if (--me.now < 0)
                            me.now = me.num - 1;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    case 'h':
                        goto EXPRESS;
                    default:
                        break;
                    }
                }
                else if (dashd (fname))
                {
		    char buff[39];//modify by chaser
		    int i=37;
                    strncpy(buff,me.item[me.now]->title,38);
                    while(i>=0)
                    {
                        if(buff[i]==' ')
                                i--;
                        else
                                break;
                    }
                    buff[i+1]='\0';
                    strcat(jhpath,"/");
                    strcat(jhpath,buff);//modify end

                    timetotal += time (0) - now;
                    now = time (0);
                    a_menu (me.item[me.now]->title, fname, me.level, bmonly);
                    timetotal += time (0) - now;
                    now = time (0);
                }
                me.page = 9999;
            }
            break;
        case '/':
            {
                char found[256];//modified by chaser 2006.05.23
		char tmp[1000];
//		char tmpboard[19];
		char key[20]={0};
//		strcpy(tmpboard,currboard);
		strcpy(tmp,jhpath);
                if (a_menusearch (path, key, found))
                {
		    strcpy(jhpath,key);
		    strcat(jhpath,"¾«»ªÇø");
                    timetotal += time (0) - now;
                    now = time (0);
//		    strcpy(currboard,key);
                    a_menu ("", found, ((HAS_PERM (PERM_ANNOUNCE)
                                         || HAS_PERM (PERM_SYSOP)
                                         || HAS_PERM (PERM_OBOARDS)) ?
                                        PERM_BOARDS : 0), 0);
                    timetotal += time (0) - now;
                    now = time (0);
                    me.page = 9999;
		    strcpy(jhpath,tmp);
//		    strcpy(currboard,tmpboard);
                }
            }
            break;
#ifdef INTERNET_EMAIL

        case 'F':
        case 'U':
            if (me.now < me.num && HAS_PERM (PERM_BASIC))
            {
                a_forward (path, me.item[me.now], ch == 'U');
                me.page = 9999;
            }
            break;
#endif

        case '!':
            if (!Q_Goodbye ())
                break;		/* youzi leave */
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
        if (me.level & PERM_BOARDS)
            a_manager (&me, ch);
    }
    for (ch = 0; ch < me.num; ch++)
        a_freeitem(me.item[ch]);
    if (me.level & PERM_BOARDS)
        timetotal = 0;
    else
        timetotal += time (0) - now;
    if (timetotal > 3)
    {
        sprintf (something, "%s/counter.person", path);
        fp = fopen (something, "r");
        if (fp)
        {
            fscanf (fp, "%d", &counter);
            fclose (fp);
        }
        fp = fopen (something, "w+");
        if (fp)
        {
            flock (fileno (fp), LOCK_EX);
            fprintf (fp, "%d", counter + 1);
            flock (fileno (fp), LOCK_UN);
            fclose (fp);
        }
    }
}

int
linkto (char *path, char *fname, char *title)
{
    MENU pm;

    pm.path = path;

    //strcpy(pm.mtitle, f_title);
    a_loadnames (&pm);
    if (pm.mtitle[0] == '\0')
        strcpy (pm.mtitle, title);
    a_additem (&pm, title, fname, NULL, 0);
    a_savenames (&pm);
	return 1;
}

int
add_grp (char *group, char *gname, char *bname, char *title)
{
    FILE *fn;
    char buf[PATHLEN];
    char searchname[STRLEN];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];

    sprintf (buf, "0Announce/.Search");
    sprintf (searchname, "%s: groups/%s/%s", bname, group, bname);
    sprintf (gpath, "0Announce/groups/%s", group);
    sprintf (bpath, "%s/%s", gpath, bname);
    if (!dashd ("0Announce"))
    {
        mkdir ("0Announce", 0755);
        chmod ("0Announce", 0755);
        if ((fn = fopen ("0Announce/.Names", "w")) == NULL)
            return -1;
        fprintf (fn, "#\n");
        fprintf (fn, "# Title=%s ¾«»ªÇø¹«²¼À¸\n", BoardName);
        fprintf (fn, "#\n");
        fclose (fn);
    }
    if (!seek_in_file (buf, bname))
        add_to_file (buf, searchname);
    if (!dashd ("0Announce/groups"))
    {
        mkdir ("0Announce/groups", 0755);
        chmod ("0Announce/groups", 0755);

        if ((fn = fopen ("0Announce/groups/.Names", "w")) == NULL)
            return -1;
        fprintf (fn, "#\n");
        fprintf (fn, "# Title=ÌÖÂÛÇø¾«»ª\n");
        fprintf (fn, "#\n");
        fclose (fn);

        //linkto("0Announce", "ÌÖÂÛÇø¾«»ª","groups", "ÌÖÂÛÇø¾«»ª");
        linkto ("0Announce", "groups", "ÌÖÂÛÇø¾«»ª");
    }
    if (!dashd (gpath))
    {
        mkdir (gpath, 0755);
        chmod (gpath, 0755);
        sprintf (buf, "%s/.Names", gpath);
        if ((fn = fopen (buf, "w")) == NULL)
            return -1;
        fprintf (fn, "#\n");
        fprintf (fn, "# Title=%s\n", gname);
        fprintf (fn, "#\n");
        fclose (fn);
        //linkto("0Announce/groups", gname, group, gname);
        linkto ("0Announce/groups", group, gname);
    }
    if (!dashd (bpath))
    {
        mkdir (bpath, 0755);
        chmod (bpath, 0755);
        //linkto(gpath, title, bname, title);
        linkto (gpath, bname, title);
        sprintf (buf, "%s/.Names", bpath);
        if ((fn = fopen (buf, "w")) == NULL)
        {
            return -1;
        }
        fprintf (fn, "#\n");
        fprintf (fn, "# Title=%s\n", title);
        fprintf (fn, "#\n");
        fclose (fn);
    }
    return 1;

}

int
del_grp (char *grp, char *bname, char *title)
{
    char buf[STRLEN], buf2[STRLEN], buf3[30];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];
    char check[30];
    int i, n;
    MENU pm;

    strncpy (buf3, grp, 29);
    buf3[29] = '\0';
    sprintf (buf, "0Announce/.Search");
    sprintf (gpath, "0Announce/groups/%s", buf3);
    sprintf (bpath, "%s/%s", gpath, bname);
    deltree (bpath);

    pm.path = gpath;
    a_loadnames (&pm);
    for (i = 0; i < pm.num; i++)
    {
        strcpy (buf2, pm.item[i]->title);
        strcpy (check, strtok (pm.item[i]->fname, "/~\n\b"));
        if (strstr (buf2, title) && !strcmp (check, bname))
        {
            a_freeitem(pm.item[i]);
            (pm.num)--;
            for (n = i; n < pm.num; n++)
                pm.item[n] = pm.item[n + 1];
            a_savenames (&pm);
            break;
        }
    }
	return 1;
}

int
edit_grp (char *bname, char *grp, char *title, char *newtitle)
{
    char buf[STRLEN], buf2[STRLEN], buf3[30];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];
    int i;
    MENU pm;

    strncpy (buf3, grp, 29);
    buf3[29] = '\0';
    sprintf (buf, "0Announce/.Search");
    sprintf (gpath, "0Announce/groups/%s", buf3);
    sprintf (bpath, "%s/%s", gpath, bname);
    if (!seek_in_file (buf, bname))
        return 0;

    pm.path = gpath;
    a_loadnames (&pm);
    for (i = 0; i < pm.num; i++)
    {
        strncpy (buf2, pm.item[i]->title, STRLEN);
        buf2[STRLEN - 1] = '\0';
        if (strstr (buf2, title) && strstr (pm.item[i]->fname, bname))
        {
            strcpy (pm.item[i]->title, newtitle);
            break;
        }
    }
    a_savenames (&pm);
    pm.path = bpath;
    a_loadnames (&pm);
    strcpy (pm.mtitle, newtitle);
    a_savenames (&pm);
	return 1;
}

/*
int
AddPCorpus ()
{
    FILE *fn;
    char digestpath[80] = "0Announce/groups/GROUP_0/PersonalCorpus";
    char personalpath[80], Log[200], title[200], ftitle[80];
    time_t now;
    int id;

    modify_user_mode (DIGEST);
    sprintf (Log, "%s/Log", digestpath);
    if (!check_systempasswd ())
    {
        return 1;
    }
    clear ();
    stand_title ("´´½¨¸öÈËÎÄ¼¯");

    move (4, 0);
    if (!gettheuserid (1, "ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ", &id))
        return 1;

    sprintf (personalpath, "%s/%c/%s", digestpath,
             toupper (lookupuser.userid[0]), lookupuser.userid);
    if (dashd (personalpath))
    {
        presskeyfor ("¸ÃÓÃ»§µÄ¸öÈËÎÄ¼¯Ä¿Â¼ÒÑ´æÔÚ, °´ÈÎÒâ¼üÈ¡Ïû..", 4);
        return 1;
    }

    move (4, 0);
    if (askyn ("È·¶¨ÒªÎª¸ÃÓÃ»§´´½¨Ò»¸ö¸öÈËÎÄ¼¯Âğ?", YEA, NA) == NA)
    {
        presskeyfor ("ÄãÑ¡ÔñÈ¡Ïû´´½¨. °´ÈÎÒâ¼üÈ¡Ïû...", 6);
        return 1;
    }
    mkdir (personalpath, 0755);
    chmod (personalpath, 0755);

    move (7, 0);
    prints ("[Ö±½Ó°´ ENTER ¼ü, Ôò±êÌâÈ±Ê¡Îª: .[32m%s µÄ¸öÈËÎÄ¼¯.[m]",
            lookupuser.userid);
    getdata (6, 0, "ÇëÊäÈë¸öÈËÎÄ¼¯Ö®±êÌâ: ", title, 39, DOECHO, YEA);
    if (title[0] == '\0')
        sprintf (title, "%s µÄ¸öÈËÎÄ¼¯", lookupuser.userid);
    sprintf (title, "%-38.38s(BM: %s)", title, lookupuser.userid);
    sprintf (digestpath, "%s/%c", digestpath, toupper (lookupuser.userid[0]));
    sprintf (ftitle, "¸öÈËÎÄ¼¯ £­£­ (Ê××ÖÄ¸ %c)",
             toupper (lookupuser.userid[0]));
    //linkto(digestpath, ftitle, lookupuser.userid, title);
    linkto (digestpath, lookupuser.userid, title);
    sprintf (personalpath, "%s/.Names", personalpath);
    if ((fn = fopen (personalpath, "w")) == NULL)
    {
        return -1;
    }
    fprintf (fn, "#\n");
    fprintf (fn, "# Title=%s\n", title);
    fprintf (fn, "#\n");
    fclose (fn);
    if (!(lookupuser.userlevel & PERM_PERSONAL))
    {
        char secu[STRLEN];

        lookupuser.userlevel |= PERM_PERSONAL;
        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
        sprintf (secu, "´´½¨¸öÈËÎÄ¼¯, ¸øÓè %s ÎÄ¼¯¹ÜÀíÈ¨ÏŞ", lookupuser.userid);
        securityreport (secu);
        move (9, 0);
        prints (secu);
        sethomefile (genbuf, lookupuser.userid, ".announcepath");
        report (genbuf);
        if (fn = fopen (genbuf, "w+"))
        {
            fprintf (fn, "%s/%s", digestpath, lookupuser.userid);
            fclose (fn);
        }

    }
    now = time (NULL);
    getdatestring (now, NA);
    sprintf (genbuf, ".[36m%-12.12s.[m %14.14s .[32m %.38s.[m",
             lookupuser.userid, datestring, title);
    add_to_file (Log, genbuf);
    presskeyfor ("ÒÑ¾­´´½¨¸öÈËÎÄ¼¯, Çë°´ÈÎÒâ¼ü¼ÌĞø...", 12);
    return 0;
}*/


void
Announce (void)
{
    char tmp[256];
    strcpy(tmp,currboard);
    strcpy(jhpath,"±¾Õ¾¾«»ªÇø");
    strcpy(currboard,"SJTUBBS");
    sprintf (genbuf, "%s ¾«»ªÇø¹«²¼À¸", BoardName);
    a_menu (genbuf, "0Announce", (HAS_PERM (PERM_ANNOUNCE)
                                  || HAS_PERM (PERM_SYSOP)) ? PERM_BOARDS :
            0, 0);
    clear ();
    jhpath[0]='\0';
    strcpy(currboard,tmp);
}


//add by rwzmm, announce trace to distrct

int board_in_which_dist(char* boardname)
{
	char BoardsFile[256] = {0};
	char category;
	struct boardheader *buffer = NULL;
	int Dindex, fd, index, total;
	struct stat st;
	char CateInSect[SectNum][5] =
	{"a", "bc", "d", "efg", "hij", "kl", "o", "p","qrs","tu","vw", "x"};

	sprintf (BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open (BoardsFile, O_RDONLY);
    if (fd == -1)
        return -1;
    (void) fstat (fd, &st);
    total = st.st_size / sizeof (struct boardheader);
    buffer = (struct boardheader *) calloc ((size_t) total, sizeof (struct boardheader));
    if (buffer == NULL)
    {
        close (fd);
        return -1;
    }
    if (read (fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close (fd);
        return -1;
    }
    close (fd);
	
    for (index = 0; index < total; index++)
    {
        if(!strcmp(buffer[index].filename,currboard))
            break;
    }
    if(index == total)
        return -1;
	
    category = buffer[index].title[0];
    free(buffer);
	
    for ( Dindex = 0; Dindex < SectNum; Dindex++ )
    {
        if (strchr (CateInSect[Dindex], category) != NULL)
            break;
    }
	
    if (Dindex == SectNum)
        return -1;

	return Dindex;
}

int write_ann_trace(char* boardname, char* userid, int mode, char* title, char* location)
{
	int district = -1;
	char filepath[260];
	
	int ch;
	char content[4096] = {0};
	FILE* fp = NULL;
	time_t now;
	struct tm *tm;
	
	district = board_in_which_dist(boardname);
	//here, no record for district 0, the system district
	if(district<=0)
		return 0;
	if(district <=9)
		ch = district + '0';
	else
		ch = district - 10 + 'A';
	
	sprintf(filepath, "0Announce/groups/GROUP_%c/District%c/ann_rec/%s", ch, ch, boardname);
	fp = fopen(filepath, "a");
	if (fp == NULL)
		return 0;

	now = time(0);
	tm = localtime(&now);
	
	switch(mode)
	{
		case ANN_ADD:
			sprintf(content, "%02dÔÂ%02dÈÕ%02dÊ±%02d·Ö%02dÃë %s: Add new file \nfile name: %s\nlocation:  %s\n\n", tm->tm_mon+1, tm->tm_mday, tm->tm_hour,tm->tm_min,tm->tm_sec,userid, title, location);
			break;
		case ANN_DELETE:
			break;
		case ANN_MOVE:
			break;
		default:
			return 0;
	}

	fwrite(content, strlen(content), 1, fp) ;
	fclose(fp);
	fp = NULL;
	return 0;
}


//add end

