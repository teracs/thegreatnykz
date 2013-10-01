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

#define MAXITEMS        1024
#define PATHLEN         256
#define A_PAGESIZE      (t_lines - 5)

#define ADDITEM         0
#define ADDGROUP        1
#define ADDMAIL         2

#define ANN_ADD			0
#define ANN_DELETE		1
#define ANN_MOVE		2

#define ANNOUNCE_RECOMMEND_FILE "announce_recommend"

char jhpath[1024];

int bmonly = 0;
void a_menu(char *maintitle, char *path, int lastlevel, int lastbmonly, int depth);

extern char BoardName[];
void a_prompt(int bot, char *pmt, char *buf, int len);

typedef struct
{
    char title[72];
    char fname[80];
    char *host;
    int port;
} ITEM;

int a_fmode = 0;

typedef struct
{
    ITEM *item[MAXITEMS];
    char mtitle[STRLEN];
    char *path;
    int num, page, now;
    int level;
    int depth;
} MENU;

MENU root_menu;

void a_showmenu(MENU * pm)
{
    struct stat st;
    struct tm *pt;
    char title[STRLEN * 2], kind[20];
    char fname[PATHLEN];
    char ch;
    char buf[STRLEN], genbuf[STRLEN * 2];
    time_t mtime;
    int n;
    char *ptr;
    int in_esc = 0, count = 0;

    char counter[9];
    FILE *fp;

    sprintf(fname, "%s/counter.person", pm->path);
    fp = fopen(fname, "r");
    if (fp)
    {
        fscanf(fp, "%s", counter);
        fclose(fp);
    }
    else
        strcpy(counter, "< none >");
    ch = strlen(counter);
    for (n = ch; n >= 0; n--)
        counter[n + (9 - ch) / 2] = counter[n];
    for (n = (9 - ch) / 2; n > 0; n--)
        counter[n - 1] = ' ';

    clear();
    if (chkmail())
    {
        prints("[5m");

        sprintf(genbuf, "[ÄúÓÐÐÅ¼þ£¬°´ L ¿´ÐÂÐÅ]");
    }
    else
        strcpy(genbuf, pm->mtitle);
    sprintf(buf, "%*s", (80 - strlen(genbuf)) / 2, "");
    prints("[1;44m%s%s%s[m\n", buf, genbuf, buf);
    prints
        ("           [1;32m F[37m ¼Ä»Ø×Ô¼ºµÄÐÅÏä  [32m¡ü¡ý[37m ÒÆ¶¯ [32m ¡ú <Enter> [37m¶ÁÈ¡ [32m ¡û,q[37m Àë¿ª[m\n");
    prints
        ("[1;44;37m ±àºÅ  %-20s[32m±¾Ä¿Â¼ÒÑ±»ä¯ÀÀ[33m%-9s[32m´Î[37m ×÷  Õß           %8s [m",
         "[Àà±ð] ±ê    Ìâ", counter, a_fmode ? "µµ°¸Ãû³Æ" : "±à¼­ÈÕÆÚ");
    prints("\n");
    if (pm->num == 0)
    {
        prints("      << Ä¿Ç°Ã»ÓÐÎÄÕÂ >>\n");
    }
    for (n = pm->page; n < pm->page + A_PAGESIZE && n < pm->num; n++)
    {
        strcpy(title, pm->item[n]->title);
        sprintf(fname, "%s", pm->item[n]->fname);
        sprintf(genbuf, "%s/%s", pm->path, fname);
        ch = (dashd(genbuf) ? '/' : ' ');
        if (a_fmode && (pm->level & PERM_BOARDS) != 0)
        {
            fname[10] = '\0';
        }
        else
        {
            if (dashf(genbuf) || ch == '/')
            {
                stat(genbuf, &st);
                mtime = st.st_mtime;
            }
            else
                mtime = time(0);
            pt = localtime(&mtime);
            sprintf(fname, "[[1m%02d[m.[1m%02d[m.[1m%02d[m]",
                    pt->tm_year % 100, pt->tm_mon + 1, pt->tm_mday);
            ch = ' ';
        }
        if (pm->item[n]->host != NULL)
        {
            if (pm->item[n]->fname[0] == '0')
                strcpy(kind, "[[1;32mÁ¬ÎÄ[m]");
            else
                strcpy(kind, "[[1;33mÁ¬Ä¿[m]");
        }
        else if (dashf(genbuf))
        {
            strcpy(kind, "[[1;36mÎÄ¼þ[m]");
        }
        else if (dashd(genbuf))
        {
            strcpy(kind, "[[1;37mÄ¿Â¼[m]");
        }
        else
        {
            strcpy(kind, "[[1;32m´íÎó[m]");
        }

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
                if (strchr("[0123456789;,", *ptr) == NULL)
                    in_esc = 0;
            }
            else
                count++;
            ptr++;
        }

        *ptr = '\0';
        if (!strncmp(title, "[Ä¿Â¼] ", 7) || !strncmp(title, "[ÎÄ¼þ] ", 7)
            || !strncmp(title, "[Á¬Ä¿] ", 7)
            || !strncmp(title, "[Á¬ÎÄ] ", 7))
            sprintf(genbuf, "%-s %-55s%-s%c", kind, title + 7, fname, ch);
        else
            sprintf(genbuf, "%-s %-55s%-s%c", kind, title, fname, ch);
        strncpy(title, genbuf, STRLEN * 2);
        title[STRLEN * 2 - 1] = '\0';
        prints("  %3d  %s\n", n + 1, title);
    }
    clrtobot();
    move(t_lines - 1, 0);
    prints("%s", (pm->level & PERM_BOARDS) ?
           "[1;31;44m[°å  Ö÷]  [33mËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ÐÂÔöÎÄÕÂ a ©¦ ÐÂÔöÄ¿Â¼ g ©¦ ±à¼­µµ°¸ E        [m"
           :
           "[1;31;44m[¹¦ÄÜ¼ü] [33m ËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ÒÆ¶¯ÓÎ±ê k,¡ü,j,¡ý ©¦ ¶ÁÈ¡×ÊÁÏ Rtn,¡ú         [m");
}

void a_additem(MENU * pm, char *title, char *fname, char *host, int port)
{
    ITEM *newitem;

    if (pm->num < MAXITEMS)
    {
        newitem = (ITEM *) malloc(sizeof(ITEM));
        strcpy(newitem->title, title);
        if (host != NULL)
        {
            newitem->host =
                (char *) malloc(sizeof(char) * (strlen(host) + 1));
            strcpy(newitem->host, host);
        }
        else
            newitem->host = host;
        newitem->port = port;
        strcpy(newitem->fname, fname);
        pm->item[(pm->num)++] = newitem;
    }
}

void a_freeitem(ITEM * item)
{
    if (item->host)
        free(item->host);
    free(item);
}

int a_loadnames(MENU * pm)
{
    FILE *fn;
    ITEM litem;
    char buf[PATHLEN], *ptr;
    char hostname[STRLEN];

    pm->num = 0;
    sprintf(buf, "%s/.Names", pm->path);
    if ((fn = fopen(buf, "r")) == NULL)
        return 0;
    hostname[0] = '\0';
    while (fgets(buf, sizeof(buf), fn) != NULL)
    {
        if ((ptr = strchr(buf, '\n')) != NULL)
            *ptr = '\0';
        if (strncmp(buf, "Name=", 5) == 0)
        {
            strncpy(litem.title, buf + 5, 72);
            litem.title[71] = '\0';
        }
        else if (strncmp(buf, "Path=", 5) == 0)
        {
            if (strncmp(buf, "Path=~/", 7) == 0)
                strncpy(litem.fname, buf + 7, 80);
            else
                strncpy(litem.fname, buf + 5, 80);
            litem.fname[79] = '\0';
            if ((!strstr(litem.title + 38, "(BM: BMS)")
                 || HAS_PERM(PERM_BOARDS))
                && (!strstr(litem.title + 38, "(BM: SYSOPS)")
                    || HAS_PERM(PERM_SYSOP)))
            {
                a_additem(pm, litem.title, litem.fname,
                          (strlen(hostname) == 0) ? NULL : hostname,
                          litem.port);
            }
            hostname[0] = '\0';
        }
        else if (strncmp(buf, "# Title=", 8) == 0)
            strncpy(pm->mtitle, buf + 8, STRLEN);
        else if (strncmp(buf, "Host=", 5) == 0)
            strncpy(hostname, buf + 5, STRLEN);
        else if (strncmp(buf, "Port=", 5) == 0)
            litem.port = atoi(buf + 5);
    }
    fclose(fn);
    return 1;
}

void a_savenames(MENU * pm)
{
    FILE *fn;
    ITEM *item;
    char fpath[PATHLEN];
    int n;

    sprintf(fpath, "%s/.Names", pm->path);
    if ((fn = fopen(fpath, "w")) == NULL)
        return;
    fprintf(fn, "#\n");
    if (!strncmp(pm->mtitle, "[Ä¿Â¼] ", 7)
        || !strncmp(pm->mtitle, "[ÎÄ¼þ] ", 7)
        || !strncmp(pm->mtitle, "[Á¬Ä¿] ", 7)
        || !strncmp(pm->mtitle, "[Á¬ÎÄ] ", 7))
        fprintf(fn, "# Title=%s\n", pm->mtitle + 7);
    else
        fprintf(fn, "# Title=%s\n", pm->mtitle);
    fprintf(fn, "#\n");
    for (n = 0; n < pm->num; n++)
    {
        item = pm->item[n];
        if (!strncmp(item->title, "[Ä¿Â¼] ", 7)
            || !strncmp(item->title, "[ÎÄ¼þ] ", 7)
            || !strncmp(item->title, "[Á¬Ä¿] ", 7)
            || !strncmp(item->title, "[Á¬ÎÄ] ", 7))
            fprintf(fn, "Name=%s\n", item->title + 7);
        else
            fprintf(fn, "Name=%s\n", item->title);
        if (item->host != NULL)
        {
            fprintf(fn, "Host=%s\n", item->host);
            fprintf(fn, "Port=%d\n", item->port);
            fprintf(fn, "Type=1\n");
            fprintf(fn, "Path=%s\n", item->fname);
        }
        else
            fprintf(fn, "Path=~/%s\n", item->fname);
        fprintf(fn, "Numb=%d\n", n + 1);
        fprintf(fn, "#\n");
    }
    fclose(fn);
    chmod(fpath, 0644);
}

void a_prompt(int bot, char *pmt, char *buf, int len)
{
    saveline(t_lines + bot, 0);
    move(t_lines + bot, 0);
    clrtoeol();
    getdata(t_lines + bot, 0, pmt, buf, len, DOECHO, YEA);
    saveline(t_lines + bot, 1);
}

int a_Save(char *path, char *key, struct fileheader *fileinfo, int nomsg, int full)
{

    char board[40];
    int ans = NA;

    if (!nomsg)
    {
        sprintf(genbuf, "È·¶¨½« [%-.40s] ´æÈëÔÝ´æµµÂð", fileinfo->title);
        if (askyn(genbuf, NA, YEA) == NA)
            return FULLUPDATE;
    }
    sprintf(board, "bm/%s", currentuser.userid);
    if (dashf(board))
    {
        if (nomsg)
            ans = YEA;
        else
            ans = askyn("Òª¸½¼ÓÔÚ¾ÉÔÝ´æµµÖ®ááÂð", NA, YEA);
    }
    if (in_mail)
        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid,
                fileinfo->filename);
    else
        sprintf(genbuf, "boards/%s/%s/%s", key,
                getfolderstr(fileinfo->filename), fileinfo->filename);
    if (full)
        f_cp(genbuf, board, (ans) ? O_APPEND : O_CREAT);
    else
        part_cp(genbuf, board, (ans) ? "a+" : "w+");
    if (!nomsg)
        presskeyfor("ÒÑ½«¸ÃÎÄÕÂ´æÈëÔÝ´æµµ, Çë°´<Enter>¼ÌÐø...",
                    t_lines - 1);
    return FULLUPDATE;
}

int a_Import(char *path, char *key, struct fileheader *fileinfo, char *direct, int nomsg)
{
#ifdef LOGHANDLER
    char query_string[4096], title_ok[256];
    int sqllen = 0;
#endif

    char fname[STRLEN], *ip, bname[PATHLEN];
    int ch;
    MENU pm;

    modify_user_mode(DIGEST);
    pm.path = direct;
    strcpy(pm.mtitle, "");
    a_loadnames(&pm);
    strcpy(fname, fileinfo->filename);
    sprintf(bname, "%s/%s", pm.path, fname);
    ip = &fname[strlen(fname) - 1];
    while (dashf(bname))
    {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        sprintf(bname, "%s/%s", pm.path, fname);
    }
    sprintf(genbuf, "%-38.38s %s ", fileinfo->title, fileinfo->owner);
    a_additem(&pm, genbuf, fname, NULL, 0);
    a_savenames(&pm);
    if (in_mail)
        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid,
                fileinfo->filename);
    else
        sprintf(genbuf, "boards/%s/%s/%s", key,
                getfolderstr(fileinfo->filename), fileinfo->filename);
    f_cp(genbuf, bname, O_CREAT);
    if (!nomsg)
    {
        presskeyfor("ÒÑ½«¸ÃÎÄÕÂ·Å½ø¾«»ªÇø, Çë°´<Enter>¼ÌÐø...",
                    t_lines - 1);
    }
    for (ch = 0; ch < pm.num; ch++)
        a_freeitem(pm.item[ch]);
    if (!in_mail)
    {
        write_ann_trace(key, currentuser.userid, ANN_ADD, fileinfo->title,
                        direct);
#ifdef LOGHANDLER
        if (!strcmp(currboard, "Search"))
        {
            mysql_escape_string(title_ok, fileinfo->title,
                                strlen(fileinfo->title));
            if (!strncmp(title_ok, "[Ñ°Îï]", 6))
            {
                sqllen = snprintf(query_string, sizeof(query_string),
                                  LOGHANDLER_SQL
                                  "INSERT INTO `search_lost` ( `id`, `reid`, `title`, `filename`) VALUES ('', '%d', '%s', '%s')\n",
                                  fileinfo->reid, title_ok + 6,
                                  fileinfo->filename);
                loghander_write(query_string, sqllen);
            }
        }
#endif

        log_bm_ops(currboard, "¾«»ªÇø", "ÊÕ¾«", fileinfo->title);
    }
    return 1;
}

int a_menusearch(char *path, char *key, char *found)
{
    FILE *fn;
    char flag = '0';
    char buf[PATHLEN], *ptr;
    int searchmode = 0;

    if ('\0' == key[0])
    {
        a_prompt(-1, "ÊäÈëÓûËÑÑ°Ö®ÌÖÂÛÇøÃû³Æ: ", key, 18);
        searchmode = 1;
    }
    sprintf(buf, "0Announce/.Search");
    if (key[0] != '\0' && (fn = fopen(buf, "r")) != NULL)
    {
        while (fgets(buf, sizeof(buf), fn) != NULL)
        {
            if (searchmode && !strstr(buf, "groups/"))
                continue;
            ptr = strchr(buf, ':');
            if (!ptr)
                return 0;
            else
            {
                *ptr = '\0';
                ptr = strtok(ptr + 1, " \t\n");
            }
            if (!strcasecmp(buf, key))
            {
                sprintf(found, "0Announce/%s", ptr);
                flag = '1';
                break;
            }
        }
        fclose(fn);
        if (flag == '0')
        {

            presskeyfor("ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌÐø...",
                        t_lines - 1);
            return 0;
        }
        
        if ((!HAS_PERM(PERM_SYSOP))) 
    	{
        setbfile(genbuf, key, "board.allow");
        if (dashf(genbuf) && !seek_in_file(genbuf, currentuser.userid))
        {
	    presskeyfor("ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌÐø...",
                        t_lines - 1);
            return 0;
        }}
        return 1;
    }
    return 0;
}

#ifdef INTERNET_EMAIL
void a_forward(char *path, ITEM * pitem, int mode)
{
    struct shortfile fhdr;
    char fname[PATHLEN], *mesg;

    sprintf(fname, "%s/%s", path, pitem->fname);
    if (dashf(fname))
    {
        strncpy(fhdr.title, pitem->title, STRLEN);
        strncpy(fhdr.filename, pitem->fname, STRLEN);
        fhdr.title[STRLEN - 1] = '\0';
        fhdr.filename[STRLEN - 5] = '\0';
        switch (doforward(path, &fhdr, mode))
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
        prints(mesg);
    }
    else
    {
        move(t_lines - 1, 0);
        prints("ÎÞ·¨×ª¼Ä´ËÏîÄ¿.\n");
    }
    pressanykey();
}
#endif

void a_newitem(MENU * pm, int mode)
{
    char uident[STRLEN];
    char board[STRLEN], title[STRLEN];
    char fname[STRLEN], fpath[PATHLEN], fpath2[PATHLEN];
    char *mesg;
    FILE *pn;
    char head;

    pm->page = 9999;
    head = 'X';
    switch (mode)
    {
    case ADDITEM:
        head = 'A';
        mesg = "ÇëÊäÈëÎÄ¼þÖ®ÖÐÎÄÃû³Æ£º ";
        break;
    case ADDGROUP:
        head = 'D';
        mesg = "ÇëÊäÈëÄ¿Â¼Ö®ÖÐÎÄÃû³Æ£º ";
        break;
    case ADDMAIL:
        sprintf(board, "bm/%s", currentuser.userid);
        if (!dashf(board))
        {
            presskeyfor("ÇëÏÈÖÁ¸ÃÌÖÂÛÇø½«ÎÄÕÂ´æÈëÔÝ´æµµ, °´<Enter>¼ÌÐø...",
                        t_lines - 1);
            return;
        }
        mesg = "ÇëÊäÈëµµ°¸Ö®ÖÐÎÄÃû³Æ£º ";
        break;
    default:
        mesg = "ÇëÊäÈëÎÄ¼þ»òÄ¿Â¼Ö®ÖÐÎÄÃû³Æ£º ";
    }

    sprintf(fname, "%c%lX", head, time(0) + getpid() + getppid() + rand());
    sprintf(fpath, "%s/%s", pm->path, fname);
    a_prompt(-1, mesg, title, 35);
    if (*title == '\0')
        return;
    switch (mode)
    {
    case ADDITEM:
        if (vedit(fpath, 0, YEA) == -1)
            return;
        chmod(fpath, 0644);
        log_bm_ops(currboard, "¾«»ªÇø", "a", title);
        break;
    case ADDGROUP:
        if (strlen(fpath) >= 230)
        {
            presskeyfor("¶Ô²»Æð, Ä¿Â¼²ã´ÎÌ«Éî, È¡Ïû²Ù×÷!", t_lines - 1);
            return;
        }
        mkdir(fpath, 0755);
        chmod(fpath, 0755);
        log_bm_ops(currboard, "¾«»ªÇø", "g", title);
        break;
    case ADDMAIL:
        rename(board, fpath);
        break;
    }
    if (mode != ADDGROUP)
        sprintf(genbuf, "%-38.38s %s ", title, currentuser.userid);
    else
    {
        if (HAS_PERM(PERM_SYSOP))
        {
            move(1, 0);
            clrtoeol();
            getdata(1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
            if (uident[0] != '\0')
                sprintf(genbuf, "%-38.38s(BM: %s)", title, uident);
            else
                sprintf(genbuf, "%-38.38s", title);
        }
        else
            sprintf(genbuf, "%-38.38s", title);
    }
    a_additem(pm, genbuf, fname, NULL, 0);
    a_savenames(pm);
    if (mode == ADDGROUP)
    {
        sprintf(fpath2, "%s/%s/.Names", pm->path, fname);
        if ((pn = fopen(fpath2, "w")) != NULL)
        {
            fprintf(pn, "#\n");
            fprintf(pn, "# Title=%s\n", genbuf);
            fprintf(pn, "#\n");
            fclose(pn);
        }
    }
}

void a_iteminfo(MENU *pm)
{
    ITEM *item = NULL;
    char path[PATHLEN];
    char tmpline[150];
    char tmpline2[150];
    char title[STRLEN];
    char board[STRLEN];
    char found[256];
    int pos;
    char *linkpath,*boardpath,*digpath,*isinmain;
    if (pm->num <= 0)
        return;

    item = pm->item[pm->now];

    strnzhcpy(title, item->title, 39);
    pos = strlen(title) - 1;
    while (title[pos] == ' ' || title[pos] == '\t')
    {
        title[pos] = '\0';
        pos --;
    }
    
    sprintf(path, "%s/%s", pm->path, item->fname);
    
    linkpath = strchr(path, '/');
    if(linkpath == NULL)return;
    linkpath = strchr(linkpath + 1, '/');
    if(linkpath == NULL)return;
    linkpath = strchr(linkpath + 1, '/');
    if(linkpath == NULL)return;
    else linkpath++;
    boardpath = strchr(linkpath+1, '/');
    if(boardpath == NULL)return;
    strncpy(board,linkpath,boardpath-linkpath);
    board[boardpath-linkpath] = '\0';


    clear();
    move(0, 0);
    prints("ÒûË®Ë¼Ô´BBS¾«»ªÇøÎÄÕÂÐÅÏ¢\n\n");
    prints("ÎÄÕÂ±êÌâ: %s\n\n", title);
    prints("ÎÄÕÂÂ·¾¶: \n%s\n\n", path + strlen(root_menu.path));
    if (dashf(path))
    {
        if (a_menusearch("0Announce", board, found))
        {
            digpath = strchr(found,'/');
            isinmain = strstr(path + strlen(root_menu.path),digpath);
            if(isinmain == NULL)
            {
                sprintf(tmpline2, "%s/%s",digpath,path + strlen(root_menu.path)+1);
                sprintf(tmpline, "http://" BBSHOST "/bbsanc?path=%s", tmpline2);
            }
            else
            {
                sprintf(tmpline2, "%s/%s",digpath,path + strlen(root_menu.path)+strlen(digpath)+1);
                sprintf(tmpline, "http://" BBSHOST "/bbsanc?path=%s", tmpline2);
            }
            if(strlen(tmpline) > 78)
            {
                strncpy(tmpline2,&tmpline[78],78);
                tmpline[78] = '\0';
                prints("¾«»ªÇøÎÄÕÂÁ´½Ó:\n%s\n%s\n\n",tmpline,tmpline2);       
            }
            else
            {
                prints("¾«»ªÇøÎÄÕÂÁ´½Ó:\n%s\n\n",tmpline);       
            }
        
            sprintf(tmpline, "×Ô¶¨Òå³¬Á´½Ó£º\n[[Digest:%s", linkpath);
            if(strlen(tmpline) > 92)
            {
                strncpy(tmpline2,&tmpline[92],78);
                tmpline[92] = '\0';
                prints("%s\n%s|%s]]\n\n",tmpline,tmpline2,title);
            }
            else if(strlen(tmpline)+strlen(title) > 89)
            {
                prints("%s\n|%s]]\n\n",tmpline,title);
            }
            else
                prints("%s|%s]]\n\n",tmpline,title);
        }
    }
    
    if (dashd(path))
    {
    
        if (a_menusearch("0Announce", board, found))
        {
            digpath = strchr(found,'/');
            isinmain = strstr(path + strlen(root_menu.path),digpath);
            if(isinmain == NULL)
            {
                sprintf(tmpline2, "%s/%s",digpath,path + strlen(root_menu.path)+1);
                sprintf(tmpline, "http://" BBSHOST "/bbs0an?path=%s", tmpline2);
            }
            else
            {
                sprintf(tmpline2, "%s/%s",digpath,path + strlen(root_menu.path)+strlen(digpath)+1);
                sprintf(tmpline, "http://" BBSHOST "/bbs0an?path=%s", tmpline2);
            }
            if(strlen(tmpline) > 78)
            {
                strncpy(tmpline2,&tmpline[78],78);
                tmpline[78] = '\0';
                prints("¾«»ªÇøÄ¿Â¼Á´½Ó:\n%s\n%s\n\n",tmpline,tmpline2);       
            }
            else
            {
                prints("¾«»ªÇøÄ¿Â¼Á´½Ó:\n%s\n\n",tmpline);       
            }
            
            sprintf(tmpline, "×Ô¶¨Òå³¬Á´½Ó£º\n[[Digdir:%s", linkpath);
            if(strlen(tmpline) > 92)
            {
                strncpy(tmpline2,&tmpline[92],78);
                tmpline[92] = '\0';
                prints("%s\n%s/|%s]]\n\n",tmpline,tmpline2,title);
            }
            else if(strlen(tmpline)+strlen(title) > 89)
            {
                prints("%s/\n|%s]]\n\n",tmpline,title);
            }
            else
                prints("%s/|%s]]\n\n",tmpline,title);
        }
    }
    
    pressanykey();
    clear();

    pressanykey();
}

void a_moveitem(MENU * pm)
{
    ITEM *tmp;
    char newnum[STRLEN];
    int num, n;

    sprintf(genbuf, "ÇëÊäÈëµÚ %d ÏîµÄÐÂ´ÎÐò: ", pm->now + 1);
    a_prompt(-2, genbuf, newnum, 6);
    num = (newnum[0] == '$') ? 9999 : atoi(newnum) - 1;
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
    a_savenames(pm);
    log_bm_ops(currboard, "¾«»ªÇø", "m", tmp->title);
}

void a_copypastedir(MENU * pm)
{
    static char title[STRLEN], filename[STRLEN], fpath[PATHLEN];
    ITEM *item;
    char newpath[PATHLEN];
    FILE *fn;
    char *newpathpos,newfullpath[PATHLEN];
    MENU cppm;
    int i;
    move(t_lines - 1, 0);
    sethomefile(fpath, currentuser.userid, ".copypaste");

	if ((fn = fopen(fpath, "r")) == NULL)
	{

	    presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ Paste ÃüÁî.",
			t_lines - 1);
	    return;
	}
	fread(title, sizeof(item->title), 1, fn);
	fread(filename, sizeof(item->fname), 1, fn);
	fread(fpath, sizeof(fpath), 1, fn);
	fclose(fn);
	sprintf(newpath, "%s/%s", pm->path, filename);
	if (*title == '\0' || *filename == '\0')
	{

	    presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",
			t_lines - 1);
	}
	else if (!(dashd(fpath)))
	{
	    sprintf(genbuf,
		    "Äã±êÊ¶µÄ%sÄ¿Â¼²»´æÔÚ,¿ÉÄÜ±»É¾³ý,»òÕß±êÊ¶µÄ²»ÊÇÄ¿Â¼,È¡ÏûÕ³Ìù.",
		    filename);

	    presskeyfor(genbuf, t_lines - 1);
	}
	else if (strstr(newpath, fpath) != NULL)
	{

	    presskeyfor("ÎÞ·¨½«Ä¿Â¼°á½ø×Ô¼ºµÄ×ÓÄ¿Â¼ÖÐ, »áÔì³ÉËÀ»ØÈ¦.",
			t_lines - 1);
	}
	else
	{
	    newpathpos = strrchr(newpath,'/');
	    *newpathpos = '\0';
	    
	    cppm.path = fpath;
	    a_loadnames(&cppm);
	    for(i = 0; i < cppm.num; i++)
	    {
	    	 strcpy(newfullpath,newpath);
	    	 strcat(newfullpath,"/");
	    	 strcat(newfullpath,cppm.item[i]->fname);
			 if (dashf(newfullpath) || dashd(newfullpath))
			 {
				    sprintf(genbuf, "%s µµ°¸/Ä¿Â¼ÒÑ¾­´æÔÚ. ¸´ÖÆÊ§°Ü", cppm.item[i]->fname);
				presskeyfor(genbuf, t_lines - 1);
				break;
			 }
			 a_additem(pm, cppm.item[i]->title, cppm.item[i]->fname, NULL, 0);
			 a_savenames(pm);
	    }
	    sprintf(genbuf, "/bin/cp -r %s/* %s/", fpath, newpath);
	    system(genbuf);
	    log_bm_ops(currboard, "¾«»ªÇø", "P", title);

	}
    
    pm->page = 9999;
}
void a_copypaste(MENU * pm, int paste)
{
    static char title[STRLEN], filename[STRLEN], fpath[PATHLEN];
    ITEM *item;
    char newpath[PATHLEN];
    FILE *fn;

    move(t_lines - 1, 0);
    sethomefile(fpath, currentuser.userid, ".copypaste");

    if (!paste)
    {
        fn = fopen(fpath, "w+");
        item = pm->item[pm->now];
        fwrite(item->title, sizeof(item->title), 1, fn);
        fwrite(item->fname, sizeof(item->fname), 1, fn);
        sprintf(genbuf, "%s/%s", pm->path, item->fname);
        strncpy(fpath, genbuf, PATHLEN);
        fpath[PATHLEN - 1] = '\0';
        fwrite(fpath, sizeof(fpath), 1, fn);
        fclose(fn);

        presskeyfor("µµ°¸±êÊ¶Íê³É. (×¢Òâ! Õ³ÌùÎÄÕÂáá²ÅÄÜ½«ÎÄÕÂ delete!)",
                    t_lines - 1);
    }
    else
    {
        if ((fn = fopen(fpath, "r")) == NULL)
        {

            presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",
                        t_lines - 1);
            return;
        }
        fread(title, sizeof(item->title), 1, fn);
        fread(filename, sizeof(item->fname), 1, fn);
        fread(fpath, sizeof(fpath), 1, fn);
        fclose(fn);
        sprintf(newpath, "%s/%s", pm->path, filename);
        if (*title == '\0' || *filename == '\0')
        {

            presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",
                        t_lines - 1);
        }
        else if (!(dashf(fpath) || dashd(fpath)))
        {
            sprintf(genbuf,
                    "Äã¿½±´µÄ%sµµ°¸/Ä¿Â¼²»´æÔÚ,¿ÉÄÜ±»É¾³ý,È¡ÏûÕ³Ìù.",
                    filename);

            presskeyfor(genbuf, t_lines - 1);
        }
        else if (dashf(newpath) || dashd(newpath))
        {
            sprintf(genbuf, "%s µµ°¸/Ä¿Â¼ÒÑ¾­´æÔÚ. ", filename);

            presskeyfor(genbuf, t_lines - 1);
        }
        else if (strstr(newpath, fpath) != NULL)
        {

            presskeyfor("ÎÞ·¨½«Ä¿Â¼°á½ø×Ô¼ºµÄ×ÓÄ¿Â¼ÖÐ, »áÔì³ÉËÀ»ØÈ¦.",
                        t_lines - 1);
        }
        else
        {
            sprintf(genbuf, "/bin/cp -r %s %s", fpath, newpath);
            system(genbuf);
            a_additem(pm, title, filename, NULL, 0);
            a_savenames(pm);
            log_bm_ops(currboard, "¾«»ªÇø", "p", title);

        }
    }
    pm->page = 9999;
}

void a_delete(MENU * pm)
{
    ITEM *item;
    char fpath[PATHLEN], ans[5];
    int n;
    FILE *fn;

    item = pm->item[pm->now];
    move(t_lines - 2, 0);
    prints("%5d  %-50s\n", pm->now + 1, item->title);
    if (item->host == NULL)
    {
        sethomefile(fpath, currentuser.userid, ".copypaste");
        if ((fn = fopen(fpath, "r")) != NULL)
        {
            fread(genbuf, sizeof(item->title), 1, fn);
            fread(genbuf, sizeof(item->fname), 1, fn);
            fread(genbuf, sizeof(fpath), 1, fn);
            fclose(fn);
        }
        sprintf(fpath, "%s/%s", pm->path, item->fname);
        if (!strncmp(genbuf, fpath, sizeof(fpath)))
        {
            a_prompt(-1, "[1;5;31m¾¯¸æ[m: ¸Ãµµ°¸/Ä¿Â¼ÊÇ±»°åÖ÷×öÁË"
                     "±ê¼Ç, [1;33mÈç¹ûÉ¾³ý, Ôò²»ÄÜÔÙÕ³Ìù¸ÃÎÄÕÂ!![m", ans,
                     2);
        }
        if (dashf(fpath))
        {
            if (askyn("É¾³ý´Ëµµ°¸, È·¶¨Âð", NA, YEA) == NA)
                return;
            unlink(fpath);
        }
        else if (dashd(fpath))
        {
            if (askyn("É¾³ýÕû¸ö×ÓÄ¿Â¼, ±ð¿ªÍæÐ¦Å¶, È·¶¨Âð", NA, YEA) == NA)
                return;
            deltree(fpath);
        }
        log_bm_ops(currboard, "¾«»ªÇø", "d", item->title);
    }
    else
    {
        if (askyn("É¾³ýÁ¬ÏßÑ¡Ïî, È·¶¨Âð", NA, YEA) == NA)
            return;
    }
    a_freeitem(item);
    (pm->num)--;
    for (n = pm->now; n < pm->num; n++)
        pm->item[n] = pm->item[n + 1];
    a_savenames(pm);
}

void a_newname(MENU * pm)
{
    ITEM *item;
    char fname[STRLEN];
    char fpath[PATHLEN];
    char *mesg;

    item = pm->item[pm->now];
    a_prompt(-2, "ÐÂµµÃû: ", fname, 12);
    if (*fname == '\0')
        return;
    sprintf(fpath, "%s/%s", pm->path, fname);
    if (!valid_fname(fname))
    {
        mesg = "²»ºÏ·¨µµ°¸Ãû³Æ.";
    }
    else if (dashf(fpath) || dashd(fpath))
    {
        mesg = "ÏµÍ³ÖÐÒÑÓÐ´Ëµµ°¸´æÔÚÁË.";
    }
    else
    {
        sprintf(genbuf, "%s/%s", pm->path, item->fname);
        if (rename(genbuf, fpath) == 0)
        {
            strcpy(item->fname, fname);
            a_savenames(pm);
            log_bm_ops(currboard, "¾«»ªÇø", "T", item->title);
            return;
        }
        mesg = "µµÃû¸ü¸ÄÊ§°Ü!!";
    }
    prints(mesg);
    egetch();
}

void a_recommend(MENU * pm)
{
    ITEM *item = NULL;
    char buf[PATHLEN];
    char recommend_index[PATHLEN];
    char title[STRLEN];
    int pos;

    item = pm->item[pm->now];

    strnzhcpy(title, item->title, 39);
    pos = strlen(title) - 1;
    while (title[pos] == ' ' || title[pos] == '\t')
    {
        title[pos] = '\0';
        pos --;
    }

    clear();
    move(1, 0);
    sprintf(buf, "ÄãÒª½«\"%s\"¼ÓÈë¾«»ªÇøÍÆ¼öÂð", title);
    if (askyn(buf, YEA, NA) == NA)
        return;

    sprintf(recommend_index, "%s/%s", root_menu.path, ANNOUNCE_RECOMMEND_FILE);
    if (!dashf(recommend_index))
    {
        a_additem(&root_menu, "¾«»ªÇøÍÆ¼öÎÄÕÂ                        (BM: BMS)", ANNOUNCE_RECOMMEND_FILE, NULL, 0);
        a_savenames(&root_menu);
    }

    sprintf(buf, "%s/%s %s", pm->path + strlen(root_menu.path), item->fname, title);
    add_to_file(recommend_index, buf);

    prints("ÒÑ½«¸ÃÎÄ¼ÓÈë¾«»ªÇøÍÆ¼ö");
    pressanykey();
}

void a_manager(MENU * pm, int ch)
{
    int opt;
    char msg[2];
    char fname[STRLEN];
    char uident[STRLEN];
    ITEM *item = NULL;
    MENU xpm;
    char fpath[PATHLEN], changed_T[STRLEN];

    if (pm->num > 0)
    {
        item = pm->item[pm->now];
        sprintf(fpath, "%s/%s", pm->path, item->fname);
    }
    switch (ch)
    {
    case 'a':
        a_newitem(pm, ADDITEM);
        break;
    case 'g':
        a_newitem(pm, ADDGROUP);
        break;
    case 'i':
        a_newitem(pm, ADDMAIL);
        break;
    case 'p':
        a_copypaste(pm, 1);
        break;
    case 'P':
        a_copypastedir(pm);
        break;
    case 'S':
        {
            FILE *fn;
            int i = 0;
            int line = 1;
            char mutiline[1024];

            move(0, 0);
            clear();
            prints("Ë¿Â·ÉèÖÃÇé¿öÈçÏÂËùÊ¾: ");
            while (i < 4)
            {
                pm->page = 9999;
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
                move(line, 0);
                
                if (fn)
                {
                    char tmpath[1000];

                    if (fgets(tmpath, 1000, fn) == NULL)
                    {
                        sprintf(mutiline, "µÚ%dË¿Â·: ", i + 1);
                    }
                    else
                    {
                        sprintf(mutiline, "µÚ%dË¿Â·: %s", i + 1, tmpath);
                    }
                    fclose(fn);
                }
                else
                {
                    sprintf(mutiline, "µÚ%dË¿Â·: %s", i + 1, " ÉÐÎ´ÉèÖÃ");
                }
                mutilineprints(mutiline, 80, &line);
                i++;
            }
            pressanykey();
            break;
        }
    case 'f':
        {
            getdata(t_lines - 1, 0, "ÇëÊäÈë²Ù×÷Ë¿Â· 1-4(Ä¬ÈÏÎªµÚÒ»Ë¿Â·):",
                    msg, 2, DOECHO, YEA);
            opt = atoi(msg);
            if (msg[0] == '\n' || msg[0] == '\0')
                msg[0] = '1';
            opt = atoi(msg) - 1;
            if (!isdigit(msg[0]) || opt < 0 || opt > 3)
            {
                presskeyfor("ÊäÈë´íÎó, Çë°´<Enter>¼ÌÐø...", t_lines - 1);
            }

            else
            {
                FILE *fn;
                FILE *fm;
                char jh[256];

                pm->page = 9999;
                if (opt == 0)
                {
                    sprintf(fname, ".announcepath");
                    sprintf(jh, ".fpp");
                }
                else
                {
                    sprintf(fname, ".announcepath%d", opt);
                    sprintf(jh, ".fpp%d", opt);
                }
                sethomefile(genbuf, currentuser.userid, fname);
                fn = fopen(genbuf, "w+");
                sethomefile(genbuf, currentuser.userid, jh);
                fm = fopen(genbuf, "w+");
                if (fn && fm)
                {
                    fprintf(fn, "%s", pm->path);
                    fclose(fn);
                    fprintf(fm, "%s", jhpath);
                    fclose(fm);
                    presskeyfor("ÒÑ½«¸ÃÂ·¾¶ÉèÎªË¿Â·, Çë°´<Enter>¼ÌÐø...",
                                t_lines - 1);
                }

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
        a_moveitem(pm);
        pm->page = 9999;
        break;
    case 'd':
    case 'D':
        a_delete(pm);
        pm->page = 9999;
        break;
    case 'V':
    case 'v':
        if (HAS_PERM(PERM_SYSOP))
        {
            if (ch == 'v')
                sprintf(fpath, "%s/.Names", pm->path);
            else
                sprintf(fpath, "0Announce/.Search");
            if (dashf(fpath))
            {
                modify_user_mode(EDITANN);
                if (vedit(fpath, 0, YEA) != -1)
                    log_bm_ops(currboard, "¾«»ªÇø", "v", pm->mtitle);
                modify_user_mode(DIGEST);
            }
            pm->page = 9999;
        }
        break;
    case 't':
        strncpy(changed_T, item->title, 72);
        getdata(t_lines - 2, 0, "ÐÂ±êÌâ: ", changed_T, 72, DOECHO, NA);
        if (*changed_T)
        {
            strncpy(item->title, changed_T, 72);
            item->title[71] = '\0';
            if (dashd(fpath))
            {
                xpm.path = fpath;
                a_loadnames(&xpm);
                strcpy(xpm.mtitle, changed_T);
                a_savenames(&xpm);
            }
            a_savenames(pm);
            log_bm_ops(currboard, "¾«»ªÇø", "t", changed_T);
        }
        pm->page = 9999;
        break;
    case 'T':
        a_prompt(-2, "ÐÂ±êÌâ: ", changed_T, 35);
        if (*changed_T)
        {
            if (dashf(fpath))
            {
                sprintf(genbuf, "%-38.38s %s ", changed_T,
                        currentuser.userid);
                strncpy(item->title, genbuf, 72);
                item->title[71] = '\0';
            }
            else if (dashd(fpath))
            {
                if (HAS_PERM(PERM_SYSOP))
                {
                    move(1, 0);
                    clrtoeol();
                    getdata(1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
                    if (uident[0] != '\0')
                        sprintf(genbuf, "%-38.38s(BM: %s)", changed_T,
                                uident);
                    else
                        sprintf(genbuf, "%-38.38s", changed_T);
                }
                else
                    sprintf(genbuf, "%-38.38s", changed_T);
                xpm.path = fpath;
                a_loadnames(&xpm);
                strcpy(xpm.mtitle, genbuf);
                a_savenames(&xpm);
                strncpy(item->title, genbuf, 72);
            }
            else if (pm->item[pm->now]->host != NULL)
                strncpy(item->title, changed_T, 72);
            item->title[71] = '\0';
            a_savenames(pm);
            log_bm_ops(currboard, "¾«»ªÇø", "T", changed_T);
        }
        pm->page = 9999;
        break;
    case 'E':
        if (dashf(fpath))
        {
            modify_user_mode(EDITANN);
            if (vedit(fpath, 0, YEA) != -1)
                log_bm_ops(currboard, "¾«»ªÇø", "E", item->title);
            modify_user_mode(DIGEST);
        }
        pm->page = 9999;
        break;
    case 'n':
        a_newname(pm);
        pm->page = 9999;
        break;
    case 'c':
        a_copypaste(pm, 0);
        break;
    case '+':
        a_recommend(pm);
        pm->page = 9999;
        break;
    }
}

void a_menu(char *maintitle, char *path, int lastlevel, int lastbmonly, int depth)
{
    MENU me;
    FILE *fp;
    char fname[PATHLEN], tmp[STRLEN], *bmstr, buf[STRLEN];
    int ch, bmonly, number = 0, savemode, counter = 0;
    time_t now = time(0), timetotal = 0;
    char something[PATHLEN + 20];

    modify_user_mode(DIGEST);
    strcpy(something, path);
    strcat(something, "/welcome");
    me.path = path;
    show_help(something);
    strcpy(me.mtitle, maintitle);
    me.level = lastlevel;
    me.depth = depth;
    bmonly = lastbmonly;
    a_loadnames(&me);

    if (me.depth == 0)
        root_menu = me;

    memset(buf, 0, STRLEN);
    strncpy(buf, me.mtitle, STRLEN);
    bmstr = strstr(buf + 38, "(BM:");
    if (bmstr != NULL)
    {
        if (chk_currBM(bmstr + 4))
            me.level |= PERM_BOARDS;
        else if (bmonly == 1 && !HAS_PERM(PERM_BOARDS))
            return;
    }

    else if (chk_currBM(""))
        me.level |= PERM_BOARDS;

    if (strstr(me.mtitle, "(BM: BMS)") || strstr(me.mtitle, "(BM: SECRET)")
        || strstr(me.mtitle, "(BM: SYSOPS)"))
        bmonly = 1;

    strcpy(buf, me.mtitle);
    bmstr = strstr(buf, "(BM:");

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
            a_showmenu(&me);
        }
        move(3 + me.now - me.page, 0);
        prints("->");
        ch = egetch();
        move(3 + me.now - me.page, 0);
        prints("  ");
        if (ch == 'Q' || ch == 'q' || ch == KEY_LEFT || ch == EOF)
        {
            char *tmp;

            if ((tmp = strrchr(jhpath, '/')) != NULL)
                *tmp = '\0';
            break;
        }
      EXPRESS:
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
        case Ctrl('B'):
            if (me.now >= A_PAGESIZE)
                me.now -= A_PAGESIZE;
            else if (me.now > 0)
                me.now = 0;
            else
                me.now = me.num - 1;
            break;
        case KEY_PGDN:
        case Ctrl('F'):
        case ' ':
            if (me.now < me.num - A_PAGESIZE)
                me.now += A_PAGESIZE;
            else if (me.now < me.num - 1)
                me.now = me.num - 1;
            else
                me.now = 0;
            break;
        case Ctrl('C'):
            if (me.num == 0)
                break;
            set_safe_record();
            if (!HAS_PERM(PERM_POST))
                break;
            sprintf(fname, "%s/%s", path, me.item[me.now]->fname);
            if (!dashf(fname))
                break;
            if (me.now < me.num)
            {
                char bname[30];

                clear();
                if (get_a_boardname(bname, "ÇëÊäÈëÒª×ªÌùµÄÌÖÂÛÇøÃû³Æ: "))
                {
                    move(1, 0);
                    if (deny_me(bname))
                    {
                        prints("\n\nÄúÉÐÎÞÈ¨ÏÞÔÚ %s ·¢±íÎÄÕÂ.", bname);
                        pressreturn();
                        me.page = 9999;
                        break;
                    }
                    sprintf(tmp, "ÄãÈ·¶¨Òª×ªÌùµ½ %s °åÂð", bname);
                    if (askyn(tmp, NA, NA) == 1)
                    {
                        Postfile(fname, bname, me.item[me.now]->title, 0);
                        move(3, 0);
                        sprintf(tmp, "[1mÒÑ¾­°ïÄã×ªÌùÖÁ %s °åÁË[m",
                                bname);
                        prints(tmp);
                        refresh();
                        sleep(1);
                    }
                }
                me.page = 9999;
            }
            show_message(NULL);
            break;
        case 'M':
            savemode = uinfo.mode;
            m_new();
            modify_user_mode(savemode);
            me.page = 9999;
            break;
        case 'L':
            savemode = uinfo.mode;
            m_read();
            modify_user_mode(savemode);
            me.page = 9999;
            break;
        case 'h':
            show_help("help/announcereadhelp");
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
                    me.page = 9999;
                    break;
                }
                else
                    sprintf(fname, "%s/%s", path, me.item[me.now]->fname);
                if (dashf(fname))
                {
                    ch = ansimore_withmode(fname, NA, MORE_MODE_DIGEST);
                    if (ch == KEY_UP)
                    {
                        if (--me.now < 0)
                            me.now = me.num - 1;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    }
                    prints
                        ("[0;1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ý [m");
                    switch (ch)
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
                else if (dashd(fname))
                {
		    if(me.depth > 9)
		    {
			presskeyfor("Ä¿Â¼Ì«Éî£¬½øÈ¥¾Í³ö²»À´ÁË¡£Çë°´ÈÎÒâ¼ü¼ÌÐø...",0);
			me.page=9999;
			break;
		    }

                    char buff[39];
                    int i = 37;

                    strncpy(buff, me.item[me.now]->title, 38);
                    while (i >= 0)
                    {
                        if (buff[i] == ' ')
                            i--;
                        else
                            break;
                    }
                    buff[i + 1] = '\0';
                    strcat(jhpath, "/");
                    strcat(jhpath, buff);

                    timetotal += time(0) - now;
                    now = time(0);
                    a_menu(me.item[me.now]->title, fname, me.level,
                           bmonly, depth + 1);
                    timetotal += time(0) - now;
                    now = time(0);
                }
                me.page = 9999;
            }
            break;
        case '/':
            {
                char found[256];
                char tmp[1000];
                char key[20] = { 0 };
                strcpy(tmp, jhpath);
                if (a_menusearch(path, key, found))
                {
                    strcpy(jhpath, key);
                    strcat(jhpath, "¾«»ªÇø");
                    timetotal += time(0) - now;
                    now = time(0);
                    a_menu("", found, (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_OBOARDS) ?
                                       PERM_BOARDS : 0), 0, 0);
                    timetotal += time(0) - now;
                    now = time(0);
                    me.page = 9999;
                    strcpy(jhpath, tmp);
                }
            }
            break;
        case '*':
            a_iteminfo(&me);
            me.page = 9999;
            break;
#ifdef INTERNET_EMAIL
        case 'F':
        case 'U':
            if (me.now < me.num && HAS_PERM(PERM_BASIC))
            {
                a_forward(path, me.item[me.now], ch == 'U');
                me.page = 9999;
            }
            break;
#endif
        case '!':
            Q_Goodbye();
            break;
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
            a_manager(&me, ch);
    }
    for (ch = 0; ch < me.num; ch++)
        a_freeitem(me.item[ch]);
    if (me.level & PERM_BOARDS)
        timetotal = 0;
    else
        timetotal += time(0) - now;
    if (timetotal > 3)
    {
        sprintf(something, "%s/counter.person", path);
        fp = fopen(something, "r");
        if (fp)
        {
            fscanf(fp, "%d", &counter);
            fclose(fp);
        }
        fp = fopen(something, "w+");
        if (fp)
        {
            flock(fileno(fp), LOCK_EX);
            fprintf(fp, "%d", counter + 1);
            flock(fileno(fp), LOCK_UN);
            fclose(fp);
        }
    }
}

int linkto(char *path, char *fname, char *title)
{
    MENU pm;

    pm.path = path;

    a_loadnames(&pm);
    if (pm.mtitle[0] == '\0')
        strcpy(pm.mtitle, title);
    a_additem(&pm, title, fname, NULL, 0);
    a_savenames(&pm);
    return 1;
}

int add_grp(char *group, char *gname, char *bname, char *title)
{
    FILE *fn;
    char buf[PATHLEN];
    char searchname[STRLEN];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];

    sprintf(buf, "0Announce/.Search");
    sprintf(searchname, "%s: groups/%s/%s", bname, group, bname);
    sprintf(gpath, "0Announce/groups/%s", group);
    sprintf(bpath, "%s/%s", gpath, bname);
    if (!dashd("0Announce"))
    {
        mkdir("0Announce", 0755);
        chmod("0Announce", 0755);
        if ((fn = fopen("0Announce/.Names", "w")) == NULL)
            return -1;
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s ¾«»ªÇø¹«²¼À¸\n", BoardName);
        fprintf(fn, "#\n");
        fclose(fn);
    }
    if (!seek_in_file(buf, bname))
        add_to_file(buf, searchname);
    if (!dashd("0Announce/groups"))
    {
        mkdir("0Announce/groups", 0755);
        chmod("0Announce/groups", 0755);

        if ((fn = fopen("0Announce/groups/.Names", "w")) == NULL)
            return -1;
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=ÌÖÂÛÇø¾«»ª\n");
        fprintf(fn, "#\n");
        fclose(fn);

        linkto("0Announce", "groups", "ÌÖÂÛÇø¾«»ª");
    }
    if (!dashd(gpath))
    {
        mkdir(gpath, 0755);
        chmod(gpath, 0755);
        sprintf(buf, "%s/.Names", gpath);
        if ((fn = fopen(buf, "w")) == NULL)
            return -1;
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s\n", gname);
        fprintf(fn, "#\n");
        fclose(fn);

        linkto("0Announce/groups", group, gname);
    }
    if (!dashd(bpath))
    {
        mkdir(bpath, 0755);
        chmod(bpath, 0755);

        linkto(gpath, bname, title);
        sprintf(buf, "%s/.Names", bpath);
        if ((fn = fopen(buf, "w")) == NULL)
        {
            return -1;
        }
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s\n", title);
        fprintf(fn, "#\n");
        fclose(fn);
    }
    return 1;

}

int del_grp(char *grp, char *bname, char *title)
{
    char buf[STRLEN], buf2[STRLEN], buf3[30];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];
    char check[30];
    int i, n;
    MENU pm;

    strncpy(buf3, grp, 29);
    buf3[29] = '\0';
    sprintf(buf, "0Announce/.Search");
    sprintf(gpath, "0Announce/groups/%s", buf3);
    sprintf(bpath, "%s/%s", gpath, bname);
    deltree(bpath);

    pm.path = gpath;
    a_loadnames(&pm);
    for (i = 0; i < pm.num; i++)
    {
        strcpy(buf2, pm.item[i]->title);
        strcpy(check, strtok(pm.item[i]->fname, "/~\n\b"));
        if (strstr(buf2, title) && !strcmp(check, bname))
        {
            a_freeitem(pm.item[i]);
            (pm.num)--;
            for (n = i; n < pm.num; n++)
                pm.item[n] = pm.item[n + 1];
            a_savenames(&pm);
            break;
        }
    }
    return 1;
}

int edit_grp(char *bname, char *grp, char *title, char *newtitle)
{
    char buf[STRLEN], buf2[STRLEN], buf3[30];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];
    int i;
    MENU pm;

    strncpy(buf3, grp, 29);
    buf3[29] = '\0';
    sprintf(buf, "0Announce/.Search");
    sprintf(gpath, "0Announce/groups/%s", buf3);
    sprintf(bpath, "%s/%s", gpath, bname);
    if (!seek_in_file(buf, bname))
        return 0;

    pm.path = gpath;
    a_loadnames(&pm);
    for (i = 0; i < pm.num; i++)
    {
        strncpy(buf2, pm.item[i]->title, STRLEN);
        buf2[STRLEN - 1] = '\0';
        if (strstr(buf2, title) && strstr(pm.item[i]->fname, bname))
        {
            strcpy(pm.item[i]->title, newtitle);
            break;
        }
    }
    a_savenames(&pm);
    pm.path = bpath;
    a_loadnames(&pm);
    strcpy(pm.mtitle, newtitle);
    a_savenames(&pm);
    return 1;
}

void Announce(void)
{
    char tmp[256];

    strcpy(tmp, currboard);
    strcpy(jhpath, "±¾Õ¾¾«»ªÇø");
    strcpy(currboard, "SJTUBBS");
    sprintf(genbuf, "%s ¾«»ªÇø¹«²¼À¸", BoardName);
    a_menu(genbuf, "0Announce", (HAS_PERM(PERM_SYSOP)) ? PERM_BOARDS :
           0, 0, 0);
    clear();
    jhpath[0] = '\0';
    strcpy(currboard, tmp);
}

int write_ann_trace(char *boardname, char *userid, int mode, char *title,
                    char *location)
{
    int district = -1;
    char filepath[260];

    int ch;
    char content[4096] = { 0 };
    FILE *fp = NULL;
    time_t now;
    struct tm *tm;

    district = board_in_which_dist(boardname);

    if (district <= 0)
        return 0;
    if (district <= 9)
        ch = district + '0';
    else
        ch = district - 10 + 'A';

    sprintf(filepath, "0Announce/groups/GROUP_%c/District%c/ann_rec/%s",
            ch, ch, boardname);
    fp = fopen(filepath, "a");
    if (fp == NULL)
        return 0;

    now = time(0);
    tm = localtime(&now);

    switch (mode)
    {
    case ANN_ADD:
        sprintf(content,
                "%02dÔÂ%02dÈÕ%02dÊ±%02d·Ö%02dÃë %s: Add new file \nfile name: %s\nlocation:  %s\n\n",
                tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,
                tm->tm_sec, userid, title, location);
        break;
    case ANN_DELETE:
        break;
    case ANN_MOVE:
        break;
    default:
        return 0;
    }

    fwrite(content, strlen(content), 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
}
