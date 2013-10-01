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
$Id: boards.c,v 1.3 2004/07/04 07:09:39 bbs Exp $
*/

#include "bbs.h"

#define BBS_PAGESIZE    (t_lines - 4)

// Ö§³Ö290¸ö°å, 290*256=74240, Ôö¼Ó°åÃæºó×öµ÷Õû
#define BRC_MAXSIZE     75000
#define BRC_MAXNUM      60
#define BRC_STRLEN      BDNAME_LEN-1
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
#define GOOD_BRC_NUM	20

char brc_buf[BRC_MAXSIZE];
int brc_size, brc_changed = 0;
char brc_name[BRC_STRLEN];
int brc_list[BRC_MAXNUM], brc_num;

//void anywhere_lockscreen();


char *sysconf_str ();
extern time_t login_start_time;
extern int numboards;
extern struct shortfile *bcache;

struct newpostdata
{
    char *name, *title, *BM;
    char flag;
    int pos, total;
    char unread, zap;
    char status;
}
*nbrd;

struct goodboard
{
    char ID[GOOD_BRC_NUM][BRC_STRLEN];
    int num;
}
GoodBrd;

int *zapbuf;
int brdnum, yank_flag = 0;
char *boardprefix;

void
EGroup (cmd)
char *cmd;
{
    char buf[STRLEN];

    sprintf (buf, "EGROUP%c", *cmd);
    GoodBrd.num = 0;
    boardprefix = sysconf_str (buf);
    choose_board (DEFINE (DEF_NEWPOST) ? 1 : 0);
}

void
Boards ()
{
    boardprefix = NULL;
    GoodBrd.num = 0;
    choose_board (0);
}

void
GoodBrds ()
{
    if (!strcmp (currentuser.userid, "guest"))
        return;
    //modified by rwzmm@sjtubbs
    modify_user_mode(FAVOURITE);
    GoodBrd.num = 9999;
    boardprefix = NULL;
    //  choose_board (1);
    choose_board (2);
}

void
New ()
{
    if (heavyload ())
    {
        clear ();
        prints ("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ýÖØ£¬Çë¸ÄÓÃ Boards Ö¸ÁîÔÄÀÀÌÖÂÛÇø...");
        pressanykey ();
        return;
    }
    boardprefix = NULL;
    GoodBrd.num = 0;
    choose_board (1);
}

int
inGoodBrds (char *bname)
{
    int i;

    for (i = 0; i < GoodBrd.num && i < GOOD_BRC_NUM; i++)
        if (!strcmp (bname, GoodBrd.ID[i]))
            return i + 1;
    return 0;
}

void
load_zapbuf ()
{
    char fname[STRLEN];
    int fd, size, n;

    size = MAXBOARD * sizeof (int);
    zapbuf = (int *) malloc (size);
    for (n = 0; n < MAXBOARD; n++)
        zapbuf[n] = 1;
    setuserfile (fname, ".lastread");
    if ((fd = open (fname, O_RDONLY, 0600)) != -1)
    {
        size = numboards * sizeof (int);
        read (fd, zapbuf, size);
        close (fd);
    }
}

void
load_GoodBrd ()
{
    int i;
    char fname[STRLEN];
    FILE *fp;

    GoodBrd.num = 0;
    setuserfile (fname, ".goodbrd");
    if ((fp = fopen (fname, "r")))
    {
        for (i = 0; i < GOOD_BRC_NUM; i++)
            if (fscanf (fp, "%s\n", GoodBrd.ID[i]) != EOF)
            {
                if (getbnum (GoodBrd.ID[i]))
                    GoodBrd.num++;
                else
                    i--;
            }
            else
                break;
        fclose (fp);
    }
    if (GoodBrd.num == 0)
    {
        GoodBrd.num++;
        if (getbcache (DEFAULTBOARD))
            strcpy (GoodBrd.ID[0], DEFAULTBOARD);
        else
            strcpy (GoodBrd.ID[0], currboard);
    }
}

void
save_GoodBrd ()
{
    int i;
    FILE *fp;
    char fname[STRLEN];

    if (GoodBrd.num <= 0)
    {
        GoodBrd.num = 1;
        if (getbcache (DEFAULTBOARD))
            strcpy (GoodBrd.ID[0], DEFAULTBOARD);
        else
            strcpy (GoodBrd.ID[0], currboard);
    }
    setuserfile (fname, ".goodbrd");
    if ((fp = fopen (fname, "wb+")) != NULL)
    {
        for (i = 0; i < GoodBrd.num; i++)
            fprintf (fp, "%s\n", GoodBrd.ID[i]);
        fclose (fp);
    }
}

void
save_zapbuf ()
{
    char fname[STRLEN];
    int fd, size;

    setuserfile (fname, ".lastread");
    if ((fd = open (fname, O_WRONLY | O_CREAT, 0600)) != -1)
    {
        size = numboards * sizeof (int);
        write (fd, zapbuf, size);
        close (fd);
    }
}

int
load_boards ()
{
    struct shortfile *bptr;
    struct newpostdata *ptr;
    int n, addto = 0, goodbrd = 0;

    resolve_boards ();
    if (zapbuf == NULL)
    {
        load_zapbuf ();
    }
    brdnum = 0;
    if (GoodBrd.num == 9999)
    {
        load_GoodBrd ();
        goodbrd = 1;
    }
    for (n = 0; n < numboards; n++)
    {
        bptr = &bcache[n];
        if (!(bptr->filename[0]))
            continue;		/* Òþ²Ø±»É¾³ýµÄ°åÃæ */
        if (goodbrd == 0)
        {
            if (!(bptr->level & PERM_POSTMASK) && !HAS_PERM (bptr->level)
                    && !(bptr->level & PERM_NOZAP))
                continue;
            if (boardprefix != NULL
                    && strchr (boardprefix, bptr->title[0]) == NULL
                    && boardprefix[0] != '*')
                continue;
            if (boardprefix != NULL && boardprefix[0] == '*')
            {
                if (!strstr (bptr->title, "¡ñ")
                        && !strstr (bptr->title, "¡Ñ") && bptr->title[0] != '*')
                    continue;
            }
            if (boardprefix == NULL && bptr->title[0] == '*')
                continue;
            addto = yank_flag || zapbuf[n] != 0 || (bptr->level & PERM_NOZAP);
        }
        else
            addto = inGoodBrds (bptr->filename);
        if (addto)
        {
            ptr = &nbrd[brdnum++];
            ptr->name = bptr->filename;
            ptr->title = bptr->title;
            ptr->BM = bptr->BM;
            ptr->flag =
                bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
            ptr->pos = n;
            ptr->total = -1;
            ptr->zap = (zapbuf[n] == 0);
            if (bptr->level & PERM_POSTMASK)
                ptr->status = 'p';
            else if (bptr->level & PERM_NOZAP)
                ptr->status = 'z';
            else if (ptr->flag & NOREPLY_FLAG)
                ptr->status = 'x';
            else if ((bptr->level & ~PERM_POSTMASK) != 0)
                ptr->status = 'r';
            else
                ptr->status = ' ';
        }
    }
    if (brdnum == 0 && !yank_flag && boardprefix == NULL)
    {
        if (goodbrd)
        {
            GoodBrd.num = 0;
            save_GoodBrd ();
            GoodBrd.num = 9999;
        }
        brdnum = -1;
        yank_flag = 1;
        return -1;
    }
    return 0;
}

int
search_board (num)
int *num;
{
    static int i = 0, find = YEA;
    static char bname[STRLEN];
    int n, ch, tmpn = NA;

    if (find == YEA)
    {
        bzero (bname, sizeof (bname));
        find = NA;
        i = 0;
    }
    while (1)
    {
        move (t_lines - 1, 0);
        clrtoeol ();
        prints ("ÇëÊäÈëÒªÕÒÑ°µÄ board Ãû³Æ£º%s", bname);
        ch = egetch ();

        if (isprint2 (ch))
        {
            bname[i++] = ch;
            for (n = 0; n < brdnum; n++)
            {
                if (!ci_strncmp (nbrd[n].name, bname, i))
                {
                    tmpn = YEA;
                    *num = n;
                    if (!strcmp (nbrd[n].name, bname))
                        return 1	/* ÕÒµ½ÀàËÆµÄ°å£¬»­ÃæÖØ»­
                               				 */ ;
                }
            }
            if (tmpn)
                return 1;
            if (find == NA)
            {
                bname[--i] = '\0';
            }
            continue;
        }
        else if (ch == Ctrl ('H') || ch == KEY_LEFT || ch == KEY_DEL ||
                 ch == '\177')
        {
            i--;
            if (i < 0)
            {
                find = YEA;
                break;
            }
            else
            {
                bname[i] = '\0';
                continue;
            }
        }
        else if (ch == '\t')
        {
            find = YEA;
            break;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_RIGHT)
        {
            find = YEA;
            break;
        }
        bell (1);
    }
    if (find)
    {
        move (t_lines - 1, 0);
        clrtoeol ();
        return 2 /* ½áÊøÁË */ ;
    }
    return 1;
}

int
check_newpost (ptr)
struct newpostdata *ptr;
{
    struct fileheader fh;
    struct stat st;
    char filename[STRLEN];
    int fd, offset, total;

    ptr->total = ptr->unread = 0;
    setbdir (genbuf, ptr->name);
    if ((fd = open (genbuf, O_RDWR)) < 0)
        return 0;
    fstat (fd, &st);
    total = st.st_size / sizeof (fh);
    if (total <= 0)
    {
        close (fd);
        return 0;
    }
    ptr->total = total;
    if (!brc_initial (ptr->name))
    {
        ptr->unread = 1;
    }
    else
    {
        offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
        lseek (fd, (off_t) (offset + (total - 1) * sizeof (fh)), SEEK_SET);
        if (read (fd, filename, STRLEN) > 0 && brc_unread (filename))
        {
            ptr->unread = 1;
        }
    }
    close (fd);
    return 1;
}

int
unread_position (dirfile, ptr)
char *dirfile;
struct newpostdata *ptr;
{
    struct fileheader fh;
    char filename[STRLEN];
    int fd, offset, step, num;

    num = ptr->total + 1;
    if (ptr->unread && (fd = open (dirfile, O_RDWR)) > 0)
    {
        if (!brc_initial (ptr->name))
        {
            num = 1;
        }
        else
        {
            offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
            num = ptr->total - 1;
            step = 4;
            while (num > 0)
            {
                lseek (fd, (off_t) (offset + num * sizeof (fh)), SEEK_SET);
                if (read (fd, filename, STRLEN) <= 0 || !brc_unread (filename))
                    break;
                num -= step;
                if (step < 32)
                    step += step / 2;
            }
            if (num < 0)
                num = 0;
            while (num < ptr->total)
            {
                lseek (fd, (off_t) (offset + num * sizeof (fh)), SEEK_SET);
                if (read (fd, filename, STRLEN) <= 0 || brc_unread (filename))
                    break;
                num++;
            }
        }
        close (fd);
    }
    if (num < 0)
        num = 0;
    return num;
}

void
show_brdlist (page, clsflag, newflag)
int page, clsflag, newflag;
{
    struct newpostdata *ptr;
    int n,i,inboard;
    char tmpBM[BM_LEN - 1];
    extern struct BCACHE *brdshm;
    if (clsflag)
    {
        clear ();
        docmdtitle ("[ÌÖÂÛÇøÁÐ±í]",
                    " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ý[m] ÁÐ³ö[[1;32my[m] ÅÅÐò[[1;32ms[m] ËÑÑ°[[1;32m/[m] ÇÐ»»[[1;32mc[m] ÇóÖú[[1;32mh[m]\n");
        prints
        ("[1;44;37m %s ÌÖÂÛÇøÃû³Æ     V  Àà±ð  ×ª %-25s   S °å  Ö÷      [m\n",
         newflag ? "È«²¿  Î´" : "  ±àºÅ  ", "ÖÐ  ÎÄ  Ðð  Êö      ÔÚÏß"/*,
         	 newflag ? "" : "   "*/);
    }
    move (3, 0);
    for (n = page; n < page + BBS_PAGESIZE; n++)
    {
        if (n >= brdnum)
        {
            prints ("\n");
            continue;
        }
        ptr = &nbrd[n];
        if (!newflag)
            prints ("  %4d %c ", n + 1, ptr->zap
                    && !(ptr->flag & NOZAP_FLAG) ? '-' : ' ');
        else
        {
            if (ptr->total == -1)
            {
                refresh ();
                check_newpost (ptr);
            }
            for (i=0; i<MAXBOARD; i++)
                if (!strcmp(brdshm->bcache[i].filename, ptr->name))
                {
                    inboard =brdshm->inboard[i];
                    //ÐÞÕý°åÃæÔÚÏßÈËÊý-1µÄbug
                    if(inboard < 0)
                    {
                        inboard = 0;
                        //brdshm->inboard[i] = 0;
                    }
                    break;
                }
            prints (" %5d %s", ptr->total, ptr->unread ? "¡ô" : "¡ó");
        }
        strcpy (tmpBM, ptr->BM);
        //ÐÞÕýÔÚÏßÈËÊý²»ÄÜ¶ÔÆëµÄbug, modified by rwzmm @ sjtubbs

        prints ("%c%-14s %s %-30s %3d    %c %-12s\n",
                (ptr->zap&& !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                ptr->name,
                (ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
                ptr->title + 1,
                inboard,
                HAS_PERM (PERM_POST) ? ptr->status : ' ',
                ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖÐ" : strtok (tmpBM, " "));
    }
    refresh ();
}

int
cmpboard (brd, tmp)
struct newpostdata *brd, *tmp;
{
    register int type = 0;

    if (!(currentuser.flags[0] & BRDSORT_FLAG))
    {
        type = brd->title[0] - tmp->title[0];
        if (type == 0)
            type = ci_strncmp (brd->title + 1, tmp->title + 1, 6);

    }
    if (type == 0)
        type = ci_strcmp (brd->name, tmp->name);
    return type;
}

int
choose_board (newflag)
int newflag;
{
    static int num;
    struct newpostdata newpost_buffer[MAXBOARD];
    struct newpostdata *ptr;
    int page, ch, tmp, number, tmpnum;
    int loop_mode = 0;

    if (!strcmp (currentuser.userid, "guest"))
        yank_flag = 1;
    nbrd = newpost_buffer;
    if(newflag == 2)
        modify_user_mode(FAVOURITE);
    else
        modify_user_mode (newflag ? READNEW : READBRD);
    brdnum = number = 0;
    clear ();
    //show_brdlist(0, 1, newflag);
    while (1)
    {
        if (brdnum <= 0)
        {
            if (load_boards () == -1)
                continue;
            qsort (nbrd, brdnum, sizeof (nbrd[0]), cmpboard);
            page = -1;
            if (brdnum <= 0)
                break;
        }
        if (num < 0)
            num = 0;
        if (num >= brdnum)
            num = brdnum - 1;
        if (page < 0)
        {
            if (newflag)
            {
                tmp = num;
                while (num < brdnum)
                {
                    ptr = &nbrd[num];
                    if (ptr->total == -1)
                        check_newpost (ptr);
                    if (ptr->unread)
                        break;
                    num++;
                }
                if (num >= brdnum)
                    num = tmp;
            }
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist (page, 1, newflag);
            update_endline ();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist (page, 0, newflag);
            update_endline ();
        }
        move (3 + num - page, 0);
        prints (">", number);
        if (loop_mode == 0)
        {
            ch = egetch ();
        }
        move (3 + num - page, 0);
        prints (" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
            break;
        switch (ch)
        {
        case 'P':
        case 'b':
        case Ctrl ('B'):
                    case KEY_PGUP:
                            if (num == 0)
                                num = brdnum - 1;
                    else
                        num -= BBS_PAGESIZE;
            break;
        case 'C':
        case 'c':
            if (newflag == 1)
                newflag = 0;
            else
                newflag = 1;
            show_brdlist (page, 1, newflag);
            break;
        case 'L':
            m_read ();
            page = -1;
            break;
        case Ctrl('v'):
                        anywhere_lockscreen();
            update_endline();
            break;
        case 'M':
            m_new ();
            page = -1;
            break;
        case 'u':
            modify_user_mode (QUERY);
            t_query ();
            page = -1;
            break;
        case 'H':
    {
                show_help ("0Announce/bbslist/day");
                page = -1;
                break;
            }
            /*		case 'R': {
                    		char buf[200],path[80],ans[4],*t;
            	        	sprintf(buf, "[1;5;31mÁ¢¼´¶ÏÏß[m: [1;33mÒÔ±ã»Ö¸´ÉÏ´ÎÕý³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç (Y/N) ? [[1;32mN[m[1;33m]£º[m");
            		        getdata(22, 0, buf, ans, 3, DOECHO, YEA);
            			if (ans[0] == 'Y' || ans[0] == 'y' ) {
                    			setuserfile(path, ".lastread");
            			        t = strstr(path,".");
            				*t = '\0';
            			        sprintf(buf,"cp %s/.lastread.backup %s/.lastread",path,path);
            			        system(buf);
            			        sprintf(buf,"cp %s/.boardrc.backup %s/.boardrc",path,path);
            			        system(buf);
            				move(23,0);clrtoeol();
            				move(22,0);clrtoeol();
            				prints("[1;33mÒÑ¾­»Ö¸´ÉÏ´ÎÕý³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç[m\n[1;32mÇë°´ Enter ¼ü[1;31mÁ¢¼´¶ÏÏß[m[1;32m, È»ºóÖØÐÂµÇÂ½±¾Õ¾ [m");
            				egetch();
            				exit(0);
            			}
            			page = -1;
            			break;	
            	        }
            */
        case 'l':
            show_allmsgs ();
            page = -1;
            break;
        case 'N':
        case ' ':
        case Ctrl ('F'):
                    case KEY_PGDN:
                            if (num == brdnum - 1)
                                num = 0;
                    else
                        num += BBS_PAGESIZE;
            break;
        case 'p':
        case 'k':
        case KEY_UP:
            if (num-- <= 0)
                num = brdnum - 1;
            break;
        case 'n':
        case 'j':
        case KEY_DOWN:
            if (++num >= brdnum)
                num = 0;
            break;
        case '$':
            num = brdnum - 1;
            break;
        case '!':		/* youzi leave */
            return Goodbye ();
            break;
        case 'h':
            show_help ("help/boardreadhelp");
            page = -1;
            break;
        case '/':
            move (3 + num - page, 0);
            prints (">", number);
            tmpnum = num;
            tmp = search_board (&num);
            move (3 + tmpnum - page, 0);
            prints (" ", number);
            if (tmp == 1)
                loop_mode = 1;
            else
    {
                loop_mode = 0;
                update_endline ();
            }
            break;
        case 's':		/* sort/unsort -mfchen */
            currentuser.flags[0] ^= BRDSORT_FLAG;
            qsort (nbrd, brdnum, sizeof (nbrd[0]), cmpboard);
            page = 999;
            break;
        case 'y':
            if (GoodBrd.num)
                break;
            yank_flag = !yank_flag;
            brdnum = -1;
            break;
        case 'z':
            if (GoodBrd.num)
                break;
            if (HAS_PERM (PERM_BASIC) && !(nbrd[num].flag & NOZAP_FLAG))
            {
                ptr = &nbrd[num];
                ptr->zap = !ptr->zap;
                ptr->total = -1;
                zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
                page = 999;
            }
            break;
        case 'a':
        case 'A':
            /*add boards to my favourate. modified by rwzmm@sjtubbs*/
#ifdef SPECIALID

            if (strcmp (currentuser.userid, SPECIALID1) == 0
                    || strcmp (currentuser.userid, SPECIALID2) == 0)
                break;
#endif

            if (GoodBrd.num >= GOOD_BRC_NUM)
            {
                presskeyfor ("¸öÈËÈÈÃÅ°åÊýÒÑ¾­´ïÉÏÏÞ(20)", t_lines - 1);
            }
            else if (GoodBrd.num && uinfo.mode == FAVOURITE)
            {
                int pos;
                char bname[STRLEN];
                struct boardheader fh;

                if (gettheboardname(1, "ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ", &pos, &fh, bname))
                {
                    if (!inGoodBrds (bname))
                    {
                        strcpy (GoodBrd.ID[GoodBrd.num++], bname);
                        save_GoodBrd ();
                        GoodBrd.num = 9999;
                        brdnum = -1;
                        break;
                    }
                }
                page = -1;
            }

            else if (uinfo.mode == READNEW || uinfo.mode == READBRD)
            {
                int i = 0;
                //		  int GoodBoardNumber = 0;
                char bname[STRLEN];
                char askynbuf[STRLEN];

                char fname[STRLEN];
                FILE *fp;

                strcpy(bname, nbrd[num].name);

                GoodBrd.num = 0;
                setuserfile (fname, ".goodbrd");
                if ((fp = fopen (fname, "r")))
                {
                    for (i = 0; i < GOOD_BRC_NUM; i++)
                        if (fscanf (fp, "%s\n", GoodBrd.ID[i]) != EOF)
                        {
                            if (getbnum (GoodBrd.ID[i]))
                                GoodBrd.num++;
                            else
                                i--;
                        }
                        else
                            break;
                    fclose (fp);
                }

                if (GoodBrd.num >= GOOD_BRC_NUM)
                {
                    presskeyfor ("¸öÈËÈÈÃÅ°åÊýÒÑ¾­´ïÉÏÏÞ(20)", t_lines - 1);
                }

                else if (!inGoodBrds (bname))
                {
                    sprintf(askynbuf, "½« %s °åÌí¼Óµ½¸öÈËÊÕ²Ø¼Ð", bname);
                    move(t_lines - 1, 0);
                    saveline(t_lines -1, 0);
                    clrtoeol();
                    if(askyn(askynbuf, YEA, YEA) == YEA)
                    {
                        strcpy (GoodBrd.ID[GoodBrd.num++], bname);
                        save_GoodBrd ();
                    }
                    saveline(t_lines-1, 1);
                    break;
                }
                //here may cause a bug
                GoodBrd.num = 0;
                brdnum = -1;
            }

            break;

        case 'd':
#ifdef SPECIALID

            if (strcmp (currentuser.userid, SPECIALID1) == 0
                    || strcmp (currentuser.userid, SPECIALID2) == 0)
                break;
#endif

            if (GoodBrd.num)
            {
                int i, pos;
                char ans[5];

                sprintf (genbuf, "Òª°Ñ %s ´ÓÊÕ²Ø¼ÐÖÐÈ¥µô£¿[Y/n]",
                         nbrd[num].name);
                getdata (t_lines - 1, 0, genbuf, ans, 2, DOECHO, YEA);
                if (ans[0] == 'n' || ans[0] == 'N')
                {
                    page = -1;
                    break;
                }
                pos = inGoodBrds (nbrd[num].name);
                for (i = pos - 1; i < GoodBrd.num - 1; i++)
                    strcpy (GoodBrd.ID[i], GoodBrd.ID[i + 1]);
                GoodBrd.num--;
                save_GoodBrd ();
                GoodBrd.num = 9999;
                brdnum = -1;
            }
            break;
        case KEY_HOME:
            num = 0;
            break;
        case KEY_END:
            num = brdnum - 1;
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
                char buf[STRLEN];

                ptr = &nbrd[num];
                brc_initial (ptr->name);
				/*add addtional security to board master,log all enter*/
				if(0==strcmp("master",currboard)) 
				{
					report("enter board master");
				}
	
                memcpy (currBM, ptr->BM, BM_LEN - 1);
                if (DEFINE (DEF_FIRSTNEW))
                {
                    setbdir (buf, currboard);
                    tmp = unread_position (buf, ptr);
                    page = tmp - t_lines / 2;
                    getkeep (buf, page > 1 ? page : 1, tmp + 1);
                }
                Read ();

                if (zapbuf[ptr->pos] > 0 && brc_num > 0)
                {
                    zapbuf[ptr->pos] = brc_list[0];
                }
                ptr->total = page = -1;
                break;
            }
        case 'S':		/* sendmsg ... youzi */
            if (HAS_PERM (PERM_MESSAGE))
            {
                s_msg ();
                page = -1;
            }

            break;
        case 'f':		/* show friends ... youzi */
            if (!HAS_PERM (PERM_BASIC))
                break;
            t_friends ();
            page = -1;
            break;
        default:
            ;
        }
        if (newflag ==2)
            modify_user_mode(FAVOURITE);
        else
            modify_user_mode (newflag ? READNEW : READBRD);
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
    clear ();
    save_zapbuf ();
    return -1;
}

char *
brc_getrecord (ptr, name, pnum, list)
char *ptr, *name;
int *pnum, *list;
{
    int num;
    char *tmp;

    strncpy (name, ptr, BRC_STRLEN);
    ptr += BRC_STRLEN;
    num = (*ptr++) & 0xff;
    tmp = ptr + num * sizeof (int);
    if (num > BRC_MAXNUM)
    {
        num = BRC_MAXNUM;
    }
    *pnum = num;
    memcpy (list, ptr, num * sizeof (int));
    return tmp;
}

char *
brc_putrecord (ptr, name, num, list)
char *ptr, *name;
int num, *list;
{
    if (num > 0)
    {
        if (num > BRC_MAXNUM)
        {
            num = BRC_MAXNUM;
        }
        strncpy (ptr, name, BRC_STRLEN);
        ptr += BRC_STRLEN;
        *ptr++ = num;
        memcpy (ptr, list, num * sizeof (int));
        ptr += num * sizeof (int);
    }
    return ptr;
}

void
brc_update ()
{
    char dirfile[STRLEN], *ptr;
    char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
    char tmp_name[BRC_STRLEN];
    int tmp_list[BRC_MAXNUM], tmp_num;
    int fd, tmp_size;

    if (brc_changed == 0)
    {
        return;
    }
    ptr = brc_buf;
    if (brc_num > 0)
    {
        ptr = brc_putrecord (ptr, brc_name, brc_num, brc_list);
    }
    if (1)
    {
        setuserfile (dirfile, ".boardrc");
        if ((fd = open (dirfile, O_RDONLY)) != -1)
        {
            tmp_size = read (fd, tmp_buf, sizeof (tmp_buf));
            close (fd);
        }
        else
        {
            tmp_size = 0;
        }
    }
    tmp = tmp_buf;
    while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
    {
        tmp = brc_getrecord (tmp, tmp_name, &tmp_num, tmp_list);
        if (strncmp (tmp_name, currboard, BRC_STRLEN) != 0)
        {
            ptr = brc_putrecord (ptr, tmp_name, tmp_num, tmp_list);
        }
    }
    brc_size = (int) (ptr - brc_buf);

    if ((fd = open (dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
    {
        ftruncate (fd, 0);
        write (fd, brc_buf, brc_size);
        close (fd);
    }
    brc_changed = 0;
}

int
brc_initial (boardname)		//deardragon0912
char *boardname;
{
    char dirfile[STRLEN], *ptr;
    int fd;

    if (strcmp (currboard, boardname) == 0)
    {
        return brc_num;
    }
    brc_update ();
    strcpy (currboard, boardname);
    brc_changed = 0;
    if (brc_buf[0] == '\0')
    {
        setuserfile (dirfile, ".boardrc");
        if ((fd = open (dirfile, O_RDONLY)) != -1)
        {
            brc_size = read (fd, brc_buf, sizeof (brc_buf));
            close (fd);
        }
        else
        {
            brc_size = 0;
        }
    }
    ptr = brc_buf;
    while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
    {
        ptr = brc_getrecord (ptr, brc_name, &brc_num, brc_list);
        if (strncmp (brc_name, currboard, BRC_STRLEN) == 0)
        {
            return brc_num;
        }
    }
    strncpy (brc_name, boardname, BRC_STRLEN);
    brc_list[0] = 1;
    brc_num = 1;
    return 0;
}

void
brc_addlist (filename)
char *filename;
{
    int ftime, n, i;

    if (!strcmp (currentuser.userid, "guest"))
        return;
    ftime = atoi (&filename[2]);
    if ((filename[0] != 'M' && filename[0] != 'G') || filename[1] != '.')
    {
        return;
    }
    if (brc_num <= 0)
    {
        brc_list[brc_num++] = ftime;
        brc_changed = 1;
        return;
    }
    for (n = 0; n < brc_num; n++)
    {
        if (ftime == brc_list[n])
        {
            return;
        }
        else if (ftime > brc_list[n])
        {
            if (brc_num < BRC_MAXNUM)
                brc_num++;
            for (i = brc_num - 1; i > n; i--)
            {
                brc_list[i] = brc_list[i - 1];
            }
            brc_list[n] = ftime;
            brc_changed = 1;
            return;
        }
    }
    if (brc_num < BRC_MAXNUM)
    {
        brc_list[brc_num++] = ftime;
        brc_changed = 1;
    }
}

int
brc_unread (filename)
char *filename;
{
    int ftime, n;

    ftime = atoi (&filename[2]);
    if ((filename[0] != 'M' && filename[0] != 'G') || filename[1] != '.')
    {
        return 0;
    }
    if (brc_num <= 0)
        return 1;
    for (n = 0; n < brc_num; n++)
    {
        if (ftime > brc_list[n])
        {
            return 1;
        }
        else if (ftime == brc_list[n])
        {
            return 0;
        }
    }
    return 0;
}
/*
<<<<<<< boards.c
 
 
 
 
 
=======
*/



//add chat admin's deny list. by rwzmm @ sjtubbs

int
addtodenytalk (char *uident, char *msg, int ischange)
{
    char buf[50], strtosave[256], buf2[50];
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;

    seek = SeekInNameList (uident);
    if (seek && !ischange)
    {
        move (3, 0);
        prints (" %s ÒÑ¾­ÔÚÃûµ¥ÖÐ¡£", uident);
        pressanykey ();
        return -1;
    }
    if (ischange && !seek)
    {
        move (3, 0);
        prints (" %s ²»ÔÚÃûµ¥ÖÐ¡£", uident);
        pressreturn ();
        return -1;
    }
    while (1)
    {
        getdata (3, 0, "ÊäÈëËµÃ÷: ", buf, 40, DOECHO, YEA);
        if (killwordsp (buf) == 0)
            continue;
        tmp = buf;
        while (*tmp != '\0')
        {
            if (*tmp == ' ')
                *tmp = '.';
            tmp++;
        }
        break;
    }
    nowtime = time (0);
    getdatestring (nowtime, NA);
    getdata (4, 0,
             "ÊäÈë·â½ûÊ±¼ä[È±Ê¡Îª 1 Ìì, 0 Îª·ÅÆú, ÊÖ¶¯½â·âÔÚÊ±¼äÇ°¼Ó'.']: ",
             buf2, 4, DOECHO, YEA);
    if (buf2[0] == '\0')
        day = 1;
    else if (buf2[0] == '.')
    {
        fAutoUndeny = 0;
        day = atoi (buf2 + 1);
    }
    else
    {
        fAutoUndeny = 1;
        day = atoi (buf2);
    }
    if (day <= 0)
        return -1;
    if (day >= 30)
        day = 30;
    nowtime += day * 86400;
    getdatestring (nowtime, NA);
    sprintf (strtosave, "%-12s %-40s %s%16.16sµã½â", uident, buf,
             fAutoUndeny ? "(a) " : "(n) ", datestring);
    if (!ischange)
    {
        sprintf (msg,
                 "\n  %s ÍøÓÑ: \n\n"
                 "\tÎÒºÜ±§Ç¸µØ¸æËßÄú£¬ÄúÒÑ¾­±»È¡Ïû ½øÈëÁÄÌìÊÒ µÄÈ¨Àû\n\n"
                 "\tÄú±»·â½ûµÄÔ­ÒòÊÇ£º [%s]\n\n"
                 "\tÒòÎªÄúµÄÐÐÎª£¬¾ö¶¨Í£Ö¹Äã½øÈëÁÄÌìÊÒµÄÈ¨Àû [%d] Ìì\n\n"
                 "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ÐÅÉêÇë½â·â¡£\n\n",
                 uident, buf, day, datestring, currentuser.userid);
    }
    else
    {
        sprintf (msg,
                 "\n  %s ÍøÓÑ: \n\n"
                 "\t¹ØÓÚÄã½øÈëÁÄÌìÊÒÈ¨ÏÞ±»È¡ÏûµÄÎÊÌâ£¬ÏÖ±ä¸üÈçÏÂ:\n\n"
                 "\t·â½ûµÄÔ­Òò£º [%s]\n\n"
                 "\t´ÓÏÖÔÚ¿ªÊ¼£¬Í£Ö¹¸ÃÈ¨ÀûÊ±¼ä£º [%d] Ìì\n\n"
                 "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ÐÅÉêÇë½â·â¡£\n\n",
                 uident, buf, day, datestring, currentuser.userid);
    }
    sprintf (genbuf, "boards/deny_talk");

    if (ischange)
        del_from_file (genbuf, uident);
    return add_to_file (genbuf, strtosave);
}

/* Modify End. */

int
deldenytalk (char *uident)
{
    char fn[STRLEN];

    sprintf (fn, "boards/deny_talk");
    return del_from_file (fn, uident);
}




