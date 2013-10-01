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

#define BBS_PAGESIZE    (t_lines - 4)

#define BRC_MAXSIZE     75000
#define BRC_MAXNUM      60
#define BRC_STRLEN      BDNAME_LEN-1
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
#define GOOD_BRC_NUM	20
#define BRC_PUSHMAXNUM 100

#define HIDE_DIR	9

char brc_buf[BRC_MAXSIZE];
int brc_size, brc_changed = 0, currpushnum = 0;
char brc_name[BRC_STRLEN];
int brc_list[BRC_MAXNUM], brc_num, brc_pushnum = 0;
char lastboard[80] = "";

char *sysconf_str(char *key);
extern time_t login_start_time;
extern int numboards;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
struct unread_push
{
    int filename;
    int num;
} brc_pushlist[BRC_PUSHMAXNUM];
struct newpostdata *nbrd;

struct goodboard
{

    int id;
    char name[64];
    int parent;
    int type;
} GoodBrd[100];
int reload_GoodBrd = 0, GoodBrd_count = 0, currdir = 0;
struct colboard
{
    char name[BRC_STRLEN];
};

struct coldir
{
    char name[80];
    char file[80];
};

int *zapbuf;
int brdnum, yank_flag = 0;
char *boardprefix;

int boardparent;
int choosemode;

void EGroup(char *cmd)
{
    char buf[STRLEN];

    sprintf(buf, "EGROUP%c", *cmd);
    GoodBrd_count = 0;
    choosemode = 1;
    boardparent = 0;
    boardprefix = sysconf_str(buf);
    choose_board(DEFINE(DEF_NEWPOST) ? 1 : 0);
}

void Boards(void)
{
    boardprefix = NULL;
    GoodBrd_count = 0;
    boardparent = -1;
    choosemode = 0;
    choose_board(0);
}

void GoodBrds(void)
{
    if (!strcmp(currentuser.userid, "guest"))
        return;

    modify_user_mode(FAVOURITE);
    reload_GoodBrd = 1;
    boardparent = -2;
    choosemode = 0;
    boardprefix = NULL;

    choose_board_good("main.hck", DEFINE(DEF_NEWPOST) ? 1 : 0);
}

int removefile(char *file)
{
    char filename[80];

    snprintf(filename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, file);
    if (remove(filename) == 0)
        return 0;
    return -1;

}

char *getstring_bynum(char *result, int num, int a, int b, int c)
{

    if (num > 100000)
        num = 0;

    if (num <= a)
        snprintf(result, 80, "[1;32m%3d[m", num);
    else if (num <= b)
        snprintf(result, 80, "[1;33m%3d[m", num);
    else if (num <= c)
        snprintf(result, 80, "[1;35m%3d[m", num);
    else
        snprintf(result, 80, "[1;31m%3d[m", num);
    return result;
}

int f_replace_file(struct boardcollection sstruct)
{
    int sd;
    char file[80];

    snprintf(file, 80, "home/%c/%c/%s/%s", toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             sstruct.dirname);

    if ((sd = open(file, O_RDWR | O_CREAT, 0644)) == -1)
        return -1;
    write(sd, &sstruct, sizeof(struct boardcollection));
    close(sd);
    return 0;
}

int getfilename(void)
{
    char buf3[80];
    time_t t;
    int filesd, count = 0;;
    t = time(0);
    snprintf(buf3, 80, "home/%c/%c/%s/%ld", toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, t);
    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        snprintf(buf3, 80, "home/%c/%c/%s/%ld",
                 toupper(currentuser.userid[0]),
                 toupper(currentuser.userid[1]), currentuser.userid, t);

        if (count++ > MAX_POSTRETRY)
        {
            close(filesd);
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);
    return t;
}
int f_get_file(char *file, struct boardcollection *sstruct, int n)
{
    int sd;
    char currentfilename[80];

    snprintf(currentfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, file);
    if ((sd = open(currentfilename, O_RDONLY)) == -1)
        return -1;
    if (lseek(sd, (n - 1) * sizeof(struct boardcollection), SEEK_CUR) ==
        -1)
    {
        close(sd);
        return -3;
    }
    if (read(sd, sstruct, sizeof(struct boardcollection)) !=
        sizeof(struct boardcollection))
    {
        close(sd);
        return -2;
    }
    close(sd);
    return 0;
}
int do_changedir(struct boardcollection dirr, char *olddir, char *newdir)
{
    int i;
    int booldel = 0;

    while (booldel != 1)
    {
        for (i = 0; i < dirr.subnum; i++)
        {
            if (!strcmp(dirr.subdirname[i], olddir))
            {
                snprintf(dirr.subdirname[i], 80, "%s", newdir);
                f_replace_file(dirr);
                booldel = 1;
                break;
            }
        }
        if (booldel == 0)
        {
            if (dirr.extends[0] == 0)
                return -1;
            if (f_get_file(dirr.extends, &dirr, 1) != 0)
                return -1;
        }
    }
    return 0;
}
int getsubdirnum(char *file)
{
    struct boardcollection tmp;
    int i, num = 0;

    if (f_get_file(file, &tmp, 1) == -1)
        return -1;
    for (i = 0; i <= tmp.subnum - 1; i++)
        num += getsubdirnum(tmp.subdirsfile[i]);
    if (tmp.extends[0] != 0)
        num += getsubdirnum(tmp.extends);
    return num + tmp.subnum;
}
int getdirnumber(void)
{
    char bufnum[80];
    int num;
    char cnum[10];
    FILE *fp;

    snprintf(bufnum, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             ".dirnum");
    if ((fp = fopen(bufnum, "r")) == NULL)
    {
        if ((fp = fopen(bufnum, "w")) == NULL)
            return -1;
        num = getsubdirnum("main.hck");
        snprintf(cnum, 10, "%d", num);
        fputs(cnum, fp);
        fclose(fp);
        return num;
    }
    fgets(cnum, 10, fp);

    fclose(fp);
    return atoi(cnum);
}
int freshdirnum(int n)
{
    char bufnum[80];
    FILE *fp;
    char num[10];

    snprintf(bufnum, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             ".dirnum");
    if ((fp = fopen(bufnum, "w")) == NULL)
        return -1;
    snprintf(num, 10, "%d", n);
    fputs(num, fp);
    fclose(fp);
    return 0;
}
int do_adddir(char *boardfile, char *newdir)
{
    char boardfilename[80];
    struct boardcollection currentdir;
    struct boardcollection newstruct;
    time_t now;

    snprintf(boardfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);
    if (f_get_file(boardfile, &currentdir, 1) != 0)
        return -1;
    if (currentdir.subnum < 20)
    {
        now = getfilename();
        currentdir.subnum++;
        snprintf(currentdir.subdirsfile[currentdir.subnum - 1], 80, "%ld",
                 now);
        snprintf(currentdir.subdirname[currentdir.subnum - 1], 80, "%s",
                 newdir);
        f_replace_file(currentdir);
        init_dir(&newstruct, currentdir.subdirsfile[currentdir.subnum - 1],
                 boardfile);
        f_replace_file(newstruct);
    }
    else
    {
        if (currentdir.extends[0] == 0)
        {
            now = getfilename();
            snprintf(currentdir.extends, 80, "%ld", now);
            f_replace_file(currentdir);
            init_dir(&newstruct, currentdir.extends, boardfile);
            newstruct.isextends = 1;
            f_replace_file(newstruct);
            do_adddir(currentdir.extends, newdir);
        }
        else
        {
            do_adddir(currentdir.extends, newdir);
        }
    }
    return 0;

}

int init_dir(struct boardcollection *dirr, char *dirname, char *updir)
{
    int i = 0;

    dirr->subnum = 0;
    dirr->boardnum = 0;
    dirr->isextends = 0;
    snprintf(dirr->dirname, 80, dirname, toupper(currentuser.userid[0]),
             currentuser.userid);
    snprintf(dirr->updir, 80, updir, toupper(currentuser.userid[0]),
             currentuser.userid);
    for (i = 0; i <= 19; i++)
    {
        dirr->subdirname[i][0] = 0;
        dirr->subdirsfile[i][0] = 0;
        dirr->boards[i][0] = 0;
    }
    dirr->extends[0] = 0;
    return 0;
}

int boardisexist(struct boardcollection tmp, char *newboard)
{
    int i = 0;

    while (i < tmp.boardnum)
    {
        if (strcmp(tmp.boards[i], newboard) == 0)
            return 1;
        i++;
    }
    return 0;
}
int do_addboard(char *boardfile, char *newboard)
{
    char boardfilename[80];
    struct boardcollection currentdir;
    struct boardcollection newstruct;
    time_t now;

    snprintf(boardfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);
    if (f_get_file(boardfile, &currentdir, 1) != 0)
        return -1;
    if (boardisexist(currentdir, newboard))
    {
        move(2, 0);
        prints("¸Ã°åÒÑ¾­´æÔÚÓÚ¸ÃÄ¿Â¼,ÎÞÐèÔÙ´ÎÌí¼Ó.");
        pressanykey();
        return -1;
    }
    if (currentdir.boardnum < 20)
    {
        currentdir.boardnum++;
        snprintf(currentdir.boards[currentdir.boardnum - 1], 80, "%s",
                 newboard);
        f_replace_file(currentdir);
    }
    else
    {
        if (currentdir.extends[0] == 0)
        {
            now = getfilename();
            snprintf(currentdir.extends, 80, "%ld", now);
            f_replace_file(currentdir);
            init_dir(&newstruct, currentdir.extends, boardfile);
            newstruct.isextends = 1;
            f_replace_file(newstruct);
            do_addboard(currentdir.extends, newboard);
        }
        else
        {
            do_addboard(currentdir.extends, newboard);
        }
    }
    return 0;

}
int struct_del_dir(struct boardcollection *dirr, char *deldir)
{
    int i;
    int booldel = 0;

    while (booldel != 1)
    {
        for (i = 0; i < dirr->subnum; i++)
        {
            if (!strcasecmp(dirr->subdirsfile[i], deldir))
            {
                snprintf(dirr->subdirsfile[i], 80, "%s",
                         dirr->subdirsfile[dirr->subnum - 1]);
                snprintf(dirr->subdirname[i], 80, "%s",
                         dirr->subdirname[dirr->subnum - 1]);
                dirr->subdirsfile[dirr->subnum - 1][0] = 0;

                dirr->subnum--;
                booldel = 1;
                break;
            }
        }
        if (booldel == 0)
        {
            if (dirr->extends[0] == 0)
                return -1;
            if (f_get_file(dirr->extends, dirr, 1) != 0)
                return -1;
        }
    }
    if (booldel == 1)
        return 0;
    return -1;
}
int struct_del_board(struct boardcollection *dirr, char *delboard)
{
    int i;
    int booldel = 0;

    while (booldel != 1)
    {
        for (i = 0; i < dirr->boardnum; i++)
        {
            if (!strcasecmp(dirr->boards[i], delboard))
            {
                snprintf(dirr->boards[i], 80, "%s",
                         dirr->boards[dirr->boardnum - 1]);
                dirr->boards[dirr->boardnum - 1][0] = 0;
                dirr->boardnum--;
                booldel = 1;
                break;
            }
        }
        if (booldel == 0)
        {
            if (dirr->extends[0] == 0)
                return -1;
            if (f_get_file(dirr->extends, dirr, 1) != 0)
                return -1;
        }
    }
    if (booldel == 1)
        return 0;
    return -1;
}
int do_del_nocare(struct boardcollection dirr)
{
    int i;
    struct boardcollection temp;

    for (i = 0; i < dirr.subnum; i++)
    {
        if (f_get_file(dirr.subdirsfile[i], &temp, 1) != 0)
            return -1;
        do_del_nocare(temp);
    }
    if (dirr.extends[0] != 0)
    {
        if (f_get_file(dirr.extends, &temp, 1) != 0)
            return -1;
        do_del_nocare(temp);
    }
    removefile(dirr.dirname);
    return 0;
}
int do_deldir(struct boardcollection updir, char *deldir)
{
    int num, subnum;
    struct boardcollection dirr;
    struct boardcollection upupdir;

    if (f_get_file(deldir, &dirr, 1) != 0)
        return -1;
    num = getdirnumber();
    subnum = getsubdirnum(deldir) + 1;
    do_del_nocare(dirr);
    struct_del_dir(&updir, deldir);
    if (updir.isextends == 1 && updir.boardnum == 0 && updir.subnum == 0)
    {
        if (updir.extends[0] == 0)
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            upupdir.extends[0] = 0;
            f_replace_file(upupdir);
        }
        else
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            snprintf(upupdir.extends, 80, "%s", updir.extends);
            f_replace_file(upupdir);
        }
        removefile(updir.dirname);
    }
    else
    {
        f_replace_file(updir);
    }
    freshdirnum(num - subnum);
    return 0;

}

int do_delboard(struct boardcollection updir, char *delboard)
{
    struct boardcollection upupdir;

    struct_del_board(&updir, delboard);
    if (updir.isextends == 1 && updir.boardnum == 0 && updir.subnum == 0)
    {
        if (updir.extends[0] == 0)
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            upupdir.extends[0] = 0;
            f_replace_file(upupdir);
        }
        else
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            snprintf(upupdir.extends, 80, "%s", updir.extends);
            f_replace_file(upupdir);
        }
        removefile(updir.dirname);
    }
    else
    {
        f_replace_file(updir);
    }
    return 0;
}
int getgood_dirnum(struct boardcollection currentdir)
{
    int i = 0;

    i += currentdir.subnum;
    while (currentdir.extends[0] != 0)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return -1;
        i += currentdir.subnum;
    }
    return i;
}

int getgood_brdnum(struct boardcollection currentdir)
{
    int i = 0;

    i += currentdir.boardnum;
    while (currentdir.extends[0] != 0)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return -1;
        i += currentdir.boardnum;
    }
    return i;
}
int get_pos(char *boardname)
{

    int i;

    for (i = 0; i < MAXBOARD; i++)
    {
        if (!strcasecmp(brdshm->bcache[i].filename, boardname))
        {
            if (!(brdshm->bcache[i].level & PERM_POSTMASK)
                && !HAS_PERM(brdshm->bcache[i].level)
                && !(brdshm->bcache[i].level & PERM_NOZAP))
                return -1;
            return i;

        }

    }
    return -1;
}

int cmpboard(const void *vbrd, const void *vtmp)
{
    struct newpostdata *brd = (struct newpostdata *) vbrd;
    struct newpostdata *tmp = (struct newpostdata *) vtmp;
    register int type = 0;

    if (brd->status == 'd' && tmp->status != 'd')
        return -1;
    if (brd->status != 'd' && tmp->status == 'd')
        return 1;

    if ((brd->flag2[0] & BOARD_DIR_FLAG)
        && (!(tmp->flag2[0] & BOARD_DIR_FLAG)))
        return -1;
    if (!(brd->flag2[0] & BOARD_DIR_FLAG)
        && ((tmp->flag2[0] & BOARD_DIR_FLAG)))
        return 1;

    if (!(currentuser.flags[0] & BRDSORT_FLAG))
    {
        type = brd->title[0] - tmp->title[0];
        if (type == 0)
            type = strncasecmp(brd->title + 1, tmp->title + 1, 6);

    }
    if (type == 0)
        type = strcasecmp(brd->name, tmp->name);
    return type;
}
void save_zapbuf(void)
{
    char fname[STRLEN];
    int fd, size;

    setuserfile(fname, ".lastread");
    if ((fd = open(fname, O_WRONLY | O_CREAT, 0600)) != -1)
    {
        size = numboards * sizeof(int);
        write(fd, zapbuf, size);
        close(fd);
    }
}
int choose_board_good(char *filename, int newflag)
{
    struct newpostdata *ptr;
    struct newpostdata *ntbrd = NULL;
    struct boardcollection currentstruct;
    struct newgood tmpgood;
    int pos, boolread = 1;
    char bname[STRLEN], buf[STRLEN], tch[4];
    char askynbuf[STRLEN];
    struct boardheader fh;
    char dname[80];
    int brdnum, dirnum = -1, totalnum = -1, num = 0;
    int page, ch, tmp, number = 0, ndir = 0;
    int loop_mode = 0;

    if (!strcmp(currentuser.userid, "guest"))
        yank_flag = 1;
    modify_user_mode(FAVOURITE);

    clear();
    page = -1;
    brdnum = -1;
    while (1)
    {
        if (brdnum == -1)
        {
            if (f_get_file(filename, &currentstruct, 1) == -1)
            {
                if (!strcmp(filename, "main.hck"))
                {
                    init_dir(&currentstruct, "main.hck", "main.hck");
                    f_replace_file(currentstruct);
                }
                else
                {
                    prints("read file error");
                    exit(0);
                }
            }
            brdnum = getgood_brdnum(currentstruct);
            dirnum = getgood_dirnum(currentstruct);

            ntbrd =
                (struct newpostdata *) malloc(sizeof(struct newpostdata) *
                                              brdnum);
            if (load_goodboards(currentstruct, ntbrd, dirnum, brdnum) != 0)
            {
                brdnum = -1;
                continue;
            }
            qsort(ntbrd, brdnum, sizeof(ntbrd[0]), cmpboard);
            totalnum = brdnum + dirnum;
        }
        if (num < 0)
            num = 0;
        if (num >= totalnum)
            num = totalnum - 1;
        if (page < 0)
        {
            tmp = num;
            while (num < totalnum)
            {
                if (num < dirnum)
                {
                    if (boolread == 0)
                    {
                        boolread = 1;
                        break;
                    }
                    num++;
                    continue;
                }
                ptr = &ntbrd[num - dirnum];
                if (ptr->total == -1)
                    check_newpost(ptr);
                if (ptr->unread)
                {
                    break;
                }
                num++;
            }
            if (num >= totalnum)
                num = tmp;

            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;

            showgood_brdlist(page, 1, newflag, currentstruct, dirnum,
                             brdnum, ntbrd);
            update_endline();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            showgood_brdlist(page, 1, newflag, currentstruct, dirnum,
                             brdnum, ntbrd);
            update_endline();
        }
        if (dirnum + brdnum <= 0)
            move(3, 0);
        else
            move(3 + num - page, 0);

        prints(">", number);
        if (loop_mode == 0)
        {
            ch = egetch();
        }
        move(3 + num - page, 0);
        prints(" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
        {
            free(ntbrd);
            return 0;
        }
        switch (ch)
        {
        case 'P':
        case 'b':
        case Ctrl('B'):
        case KEY_PGUP:
            if (num == 0)
                num = totalnum - 1;
            else
                num -= BBS_PAGESIZE;
            break;
        case 'L':
            m_read();
            page = -1;
            break;
        case Ctrl('v'):
            anywhere_lockscreen();
            update_endline();
            break;
        case 'M':
            m_new();
            page = -1;
            break;
        case 'u':
            modify_user_mode(QUERY);
            t_query(NULL);
            page = -1;
            break;
        case 'H':
            {
                show_help("0Announce/bbslist/day");
                move(t_lines - 1, 0);
                tch[0] = '\0';
                getdata(t_lines - 1, 0, "²é¿´·ÖÇøÊ®´ó:(1~b)Çø 0)È¡Ïû [0]:",
                        tch, 3, DOECHO, YEA);
                char dList[] = "123456789ab";
                char top10file[64];

                while (tch[0] != '\0' && strchr(dList, tch[0]) != NULL)
                {
                    sprintf(top10file, "0Announce/bbslist/new%ctop10",
                            tch[0]);
                    show_help(top10file);
                    move(t_lines - 1, 0);
                    tch[0] = '\0';
                    getdata(t_lines - 1, 0,
                            "²é¿´·ÖÇøÊ®´ó:(1~b)Çø 0)È¡Ïû [0]:", tch, 3,
                            DOECHO, YEA);

                }
                page = -1;
                break;
            }
        case 'C':
        case 'c':
        	{
			   if (dirnum + brdnum <= 0)
                    break;
                getname_byn(currentstruct, num, &tmpgood, dirnum, brdnum);
                if (!tmpgood.isdir)
                {
            		ptr = &ntbrd[num - dirnum];
                    brc_initial(ptr->name);
                    setbdir(buf, currboard);
                    flag_clearto(NA,buf,YEA);
                }
        	}
        case 'x':
            if (load_goodboards(currentstruct, ntbrd, dirnum, brdnum) != 0)
            {
                brdnum = -1;
                break;
            }
            qsort(ntbrd, brdnum, sizeof(ntbrd[0]), cmpboard);
            page = -1;
            break;
        case 'l':
            show_allmsgs();
            page = -1;
            break;
        case 'N':
        case ' ':
        case Ctrl('F'):
        case KEY_PGDN:
            if (num == totalnum - 1)
                num = 0;
            else
                num += BBS_PAGESIZE;
            break;
        case 'p':
        case 'k':
        case KEY_UP:
            if (num-- <= 0)
                num = totalnum - 1;
            break;
        case 'n':
        case 'j':
        case KEY_DOWN:
            if (++num >= totalnum)
                num = 0;
            break;
        case '$':
            num = totalnum - 1;
            break;
        case '!':
            free(ntbrd);
            return Goodbye();
            break;
        case 'h':
            show_help("help/collectionhelp");
            page = -1;
            break;

        case 's':
            currentuser.flags[0] ^= BRDSORT_FLAG;
            qsort(ntbrd, brdnum, sizeof(ntbrd[0]), cmpboard);
            page = 999;
            break;
        case 't':
        case 'T':
            {
                if (num >= dirnum)
                    break;
                if (getdata
                    (0, 0, "ÇëÊäÈëÐÂÄ¿Â¼Ãû³Æ: ", dname, IDLEN + 1, DOECHO,
                     YEA))
                {
                    getname_byn(currentstruct, num, &tmpgood, dirnum,
                                brdnum);
                    do_changedir(currentstruct, tmpgood.subdirname, dname);
                    if (f_get_file
                        (currentstruct.dirname, &currentstruct, 1) != 0)
                        break;
                }
                page = -1;
                break;
            }
        case 'A':

            if ((ndir = getdirnumber()) == -1)
                break;
            if (ndir >= MAX_DIRNUM)
            {
                snprintf(askynbuf, 80, "ÄúÒÑ³¬¹ý%d¸öÄ¿Â¼µÄ×î´óÏÞÖÆ.",
                         MAX_DIRNUM);
                presskeyfor(askynbuf, t_lines - 1);
                break;
            }
            if (getdata
                (0, 0, "ÇëÊäÈëÄ¿Â¼Ãû³Æ: ", dname, 20 + 1, DOECHO, YEA))
            {
                if (do_adddir(filename, dname) == 0)
                {
                    ndir++;
                    freshdirnum(ndir);
                }

            }
            page = -1;
            brdnum = -1;
            break;
        case 'a':
            {
                if (gettheboardname
                    (1, "ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ", &pos, &fh,
                     bname))
                {
                    if (fh.flag2[0] & BOARD_DIR_FLAG)
                    {
                        move(2, 0);
                        clrtoeol();
                        prints
                            ("¶Ô²»Æð£¬Äã²»ÄÜÖ±½ÓÌí¼Ó·ÖÇø×ÓÄ¿Â¼µ½ÊÕ²Ø¼Ð£¡");
                        pressanykey();
                        page = -1;
                        brdnum = -1;
                        break;
                    }
                    if (!inGoodBrds(bname))
                    {
                        do_addboard(filename, bname);
                    }
                }
                page = -1;
                brdnum = -1;
                break;
            }
        case 'd':

            if (num < dirnum)
            {
                getname_byn(currentstruct, num, &tmpgood, dirnum, brdnum);
                sprintf(askynbuf, "È·¶¨ÒªÉ¾³ýÄ¿Â¼%s", tmpgood.subdirname);
                move(t_lines - 1, 0);
                saveline(t_lines - 1, 0);
                clrtoeol();
                if (askyn(askynbuf, NA, YEA) == YEA)
                {
                    do_deldir(currentstruct, tmpgood.subdirsfile);

                    page = -1;
                    brdnum = -1;
                    break;
                }
                saveline(t_lines - 1, 1);

            }
            else if (num < totalnum)
            {
                ptr = &ntbrd[num - dirnum];
                sprintf(askynbuf, "È·¶¨ÒªÉ¾³ýÌÖÂÛÇø%s", ptr->name);
                move(t_lines - 1, 0);
                saveline(t_lines - 1, 0);
                clrtoeol();
                if (askyn(askynbuf, NA, YEA) == YEA)
                {
                    do_delboard(currentstruct, ptr->name);

                    page = -1;
                    brdnum = -1;
                    break;
                }
                saveline(t_lines - 1, 1);
            }
            break;

        case KEY_HOME:
            num = 0;
            break;
        case KEY_TAB:
            tmp = num;
            if (num < dirnum)
                num = dirnum - 1;
            num++;
            while (num <= totalnum - 1 && num >= dirnum)
            {
                ptr = &ntbrd[num - dirnum];
                if (ptr->unread)
                    break;
                num++;
            }
            if (num == totalnum)
            {
                if (tmp < dirnum)
                {
                    num = tmp;
                    break;
                }
                num = dirnum;
                while (num < tmp && num >= dirnum)
                {
                    ptr = &ntbrd[num - dirnum];
                    if (ptr->unread)
                        break;
                    num++;
                }
            }
            break;

        case KEY_END:
            num = totalnum - 1;
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
                if (dirnum + brdnum <= 0)
                    break;
                getname_byn(currentstruct, num, &tmpgood, dirnum, brdnum);
                if (tmpgood.isdir)
                {
                    choose_board_good(tmpgood.subdirsfile, newflag);
                    boolread = 0;
                }
                else
                {
                    ptr = &ntbrd[num - dirnum];
                    brc_initial(ptr->name);
                    if (0 == strcasecmp("master", tmpgood.boards))
                    {
                        report("enter board master");
                    }

                    memcpy(currBM, ptr->BM, BM_LEN - 4);
                    if (DEFINE(DEF_FIRSTNEW))
                    {
                        setbdir(buf, currboard);
                        tmp = unread_position(buf, ptr);
                        page = tmp - t_lines / 2;
                        getkeep(buf, page > 1 ? page : 1, tmp + 1);
                    }
                    Read();

                    if (zapbuf[ptr->pos] > 0 && brc_num > 0)
                    {
                        zapbuf[ptr->pos] = brc_list[0];
                    }
                    ptr->total = -1;

                }
                page = -1;
                break;
            }
        case 'S':
            if (HAS_PERM(PERM_MESSAGE))
            {
                s_msg();
                page = -1;
            }
            break;
        case 'f':
            if (!HAS_PERM(PERM_BASIC))
                break;
            t_friends();
            page = -1;
            break;
        case 'w':
            visit_wiki();
            page = -1;
            break;
        default:
            ;
        }
        modify_user_mode(FAVOURITE);
    }
    clear();
    save_zapbuf();
    free(ntbrd);
}

void New(void)
{
    if (heavyload())
    {
        clear();
        prints("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ýÖØ£¬Çë¸ÄÓÃ Boards Ö¸ÁîÔÄÀÀÌÖÂÛÇø...");
        pressanykey();
        return;
    }
    boardprefix = NULL;
    GoodBrd_count = 0;
    boardparent = -1;
    choosemode = HIDE_DIR;
    choose_board(1);
}

int inGoodBrds(char *bname)
{
    int i;

    for (i = 0; i < 100; i++)
    {
        if (GoodBrd[i].type == 1)
            continue;
        if ((!strcasecmp(bname, GoodBrd[i].name))
            && GoodBrd[i].parent == currdir)
            return i + 1;
    }
    return 0;
}
int inGoodBrds_d(char *bname)
{
    int i;

    for (i = 0; i < 100; i++)
    {
        if ((!strcasecmp(bname, GoodBrd[i].name))
            && GoodBrd[i].parent == currdir)
            return i + 1;
    }
    return 0;

}
int getposbyid(int id)
{
    int i;

    for (i = 0; i < 100; i++)
    {
        if (id == GoodBrd[i].id)
            return i + 1;
    }
    return 0;
}
void load_zapbuf(void)
{
    char fname[STRLEN];
    int fd, size;

    size = MAXBOARD * sizeof(int);
    zapbuf = (int *) malloc(size);

    memset(zapbuf, 1, size);
    setuserfile(fname, ".lastread");
    if ((fd = open(fname, O_RDONLY, 0600)) != -1)
    {
        size = numboards * sizeof(int);
        read(fd, zapbuf, size);
        close(fd);
    }
}

void load_GoodBrd(void)
{
    int i, fp;
    char fname[STRLEN];

    GoodBrd_count = 0;
    setuserfile(fname, ".goodbrd.fixme");
    if ((fp = open(fname, O_RDONLY)))
    {
        memset(&GoodBrd, 0, sizeof(GoodBrd));
        read(fp, &GoodBrd, sizeof(GoodBrd));
        for (i = 0; i < GOOD_BRC_NUM && GoodBrd[i].id != 0; i++)
        {
            if (GoodBrd[i].type == 1)
            {
                GoodBrd_count++;
                continue;
            }

            if (getbnum(GoodBrd[i].name))
                GoodBrd_count++;
        }
        close(fp);
    }
    if (GoodBrd_count == 0)
    {
        GoodBrd_count++;
        if (getbcache(DEFAULTBOARD))
            strcpy(GoodBrd[0].name, DEFAULTBOARD);
        else
            strcpy(GoodBrd[0].name, currboard);
    }
    reload_GoodBrd = 0;
}

void save_GoodBrd(void)
{

    int fp;
    char fname[STRLEN];

    if (GoodBrd[0].id == 0)
    {
        GoodBrd[0].id = 1;
        if (getbcache(DEFAULTBOARD))
            strcpy(GoodBrd[0].name, DEFAULTBOARD);
        else
            strcpy(GoodBrd[0].name, currboard);
    }
    setuserfile(fname, ".goodbrd.fixme");
    if ((fp = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644)) != -1)
    {
        write(fp, GoodBrd, sizeof(GoodBrd));
        close(fp);
    }
}

int load_goodboards(struct boardcollection currentstruct,
                    struct newpostdata *nbrd, int dirnum, int brdnum)
{
    struct shortfile *bptr;
    struct newpostdata *ptr;
    struct newgood ndir;
    int n, pos;

    resolve_boards();
    if (zapbuf == NULL)
    {
        load_zapbuf();
    }
    for (n = 0; n < brdnum; n++)
    {
        getname_byn(currentstruct, n + dirnum, &ndir, dirnum, brdnum);
        if (ndir.isdir)
            continue;
        pos = get_pos(ndir.boards);
        if (-1 == pos)
        {
            free(nbrd);
            do_delboard(currentstruct, ndir.boards);
            return -1;
        }

        bptr = &bcache[pos];
        ptr = &nbrd[n];
        ptr->name = bptr->filename;
        ptr->title = bptr->title;
        ptr->BM = bptr->BM;
        ptr->flag =
            bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
        strncpy(ptr->flag2, bptr->flag2, 3);
        ptr->pos = pos;
        ptr->total = -1;
        ptr->zap = (zapbuf[pos] == 0);
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

    return 0;
}
int load_boards(void)
{
    struct shortfile *bptr;
    struct newpostdata *ptr;
    int n, addto = 0, goodbrd = 0;
    char notename[STRLEN];

    resolve_boards();
    if (zapbuf == NULL)
    {
        load_zapbuf();
    }
    brdnum = 0;

    if (uinfo.mode == FAVOURITE)
    {
        if (reload_GoodBrd == 1)
        {
            load_GoodBrd();
        }

        goodbrd = 1;
        for (n = 0; GoodBrd[n].id != 0; n++)
        {
            if (GoodBrd[n].type == 1 && GoodBrd[n].parent == currdir)
            {
                int i, j = 0;

                for (i = 0; i <= GoodBrd_count - 1; i++)
                {
                    if (GoodBrd[i].parent == GoodBrd[n].id)
                        j++;
                }
                ptr = &nbrd[brdnum++];
                ptr->name = (&GoodBrd[n])->name;
                ptr->title = (&GoodBrd[n])->name;
                ptr->status = 'd';
                ptr->total = GoodBrd[n].id;
                ptr->pos = j;
            }
        }
    }

    for (n = 0; n < numboards; n++)
    {
        bptr = &bcache[n];
        if (!(bptr->filename[0]))
            continue;
        if (goodbrd == 0)
        {
            if (!(bptr->level & PERM_POSTMASK) && !HAS_PERM(bptr->level)
                && !(bptr->level & PERM_NOZAP))
                continue;

            setbfile(notename, bptr->filename, "board.allow");
            if (dashf(notename)
                && !seek_in_file(notename, currentuser.userid))
                continue;

            if (choosemode == 0)
            {
                if (boardparent > 0 && boardparent != bptr->group - 1)
                    continue;
                if (boardparent == 0 && bptr->group != 0)
                    continue;
                if (boardparent > 0 && bptr->title[0] == '*')
                    continue;
            }
            else if (choosemode == HIDE_DIR)
            {
                if (bptr->flag2[0] & BOARD_DIR_FLAG)
                    continue;
                if (boardprefix != NULL
                    && strchr(boardprefix, bptr->title[0]) == NULL
                    && boardprefix[0] != '*')
                    continue;
                if (boardprefix != NULL && boardprefix[0] == '*')
                {
                    if (!strstr(bptr->title, "¡ñ")
                        && !strstr(bptr->title, "¡Ñ")
                        && bptr->title[0] != '*')
                        continue;
                }
                if (boardprefix == NULL && bptr->title[0] == '*')
                    continue;
            }

            else
            {
                if (boardparent > 0 && boardparent != bptr->group - 1)
                    continue;
                if (boardparent == 0 && bptr->group != 0)
                    continue;
                if (boardparent > 0 && bptr->title[0] == '*')
                    continue;

                if (boardprefix != NULL
                    && strchr(boardprefix, bptr->title[0]) == NULL
                    && boardprefix[0] != '*')
                    continue;
                if (boardprefix != NULL && boardprefix[0] == '*')
                {
                    if (!strstr(bptr->title, "¡ñ")
                        && !strstr(bptr->title, "¡Ñ")
                        && bptr->title[0] != '*')
                        continue;
                }
                if (boardprefix == NULL && bptr->title[0] == '*')
                    continue;
            }
            addto = yank_flag || zapbuf[n] != 0
                || (bptr->level & PERM_NOZAP);
        }
        else
            addto = inGoodBrds(bptr->filename);
        if (addto)
        {
            ptr = &nbrd[brdnum++];
            ptr->name = bptr->filename;
            ptr->title = bptr->title;
            ptr->BM = bptr->BM;
            ptr->flag =
                bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
            strncpy(ptr->flag2, bptr->flag2, 3);

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
    if (brdnum == 0 && !yank_flag && boardprefix == NULL
        && (currdir == 0 || boardparent == -1))
    {
        if (goodbrd)
        {
            GoodBrd_count = 0;
            save_GoodBrd();
            reload_GoodBrd = 1;
        }
        brdnum = -1;
        yank_flag = 1;
        return -1;
    }
    return 0;
}

int search_board(int *num)
{
    static int i = 0, find = YEA;
    static char bname[STRLEN];
    int n, ch, tmpn = NA;

    if (find == YEA)
    {
        bzero(bname, sizeof(bname));
        find = NA;
        i = 0;
    }
    while (1)
    {
        move(t_lines - 1, 0);
        clrtoeol();
        prints("ÇëÊäÈëÒªÕÒÑ°µÄ board Ãû³Æ£º%s", bname);
        ch = egetch();

        if (isprint2(ch))
        {
            bname[i++] = ch;
            for (n = 0; n < brdnum; n++)
            {
                if (!strncasecmp(nbrd[n].name, bname, i))
                {
                    tmpn = YEA;
                    *num = n;
                    if (!strcasecmp(nbrd[n].name, bname))
                        return 1;
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
        else if (ch == Ctrl('H') || ch == KEY_LEFT || ch == KEY_DEL ||
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
        bell();
    }
    if (find)
    {
        move(t_lines - 1, 0);
        clrtoeol();
        return 2;
    }
    return 1;
}

int check_newpost(struct newpostdata *ptr)
{
    struct fileheader fh;
    struct stat st;
    char filename[STRLEN];
    int fd, offset, total;

    ptr->total = ptr->unread = 0;
    setbdir(genbuf, ptr->name);
    if ((fd = open(genbuf, O_RDWR)) < 0)
        return 0;
    fstat(fd, &st);
    total = st.st_size / sizeof(fh);
    if (total <= 0)
    {
        close(fd);
        return 0;
    }
    ptr->total = total;
    if (!brc_initial(ptr->name))
    {
        ptr->unread = 1;
    }
    else
    {
        offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
        lseek(fd, (off_t) (offset + (total - 1) * sizeof(fh)), SEEK_SET);
        if (read(fd, filename, STRLEN) > 0 && brc_unread(filename))
        {
            ptr->unread = 1;
        }
    }
    close(fd);
    return 1;
}

int unread_position(char *dirfile, struct newpostdata *ptr)
{
    struct fileheader fh;
    char filename[STRLEN];
    int fd, offset, step, num;

    num = ptr->total + 1;
    if (ptr->unread && (fd = open(dirfile, O_RDWR)) > 0)
    {
        if (!brc_initial(ptr->name))
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
                lseek(fd, (off_t) (offset + num * sizeof(fh)), SEEK_SET);
                if (read(fd, filename, STRLEN) <= 0
                    || !brc_unread(filename))
                    break;
                num -= step;
                if (step < 32)
                    step += step / 2;
            }
            if (num < 0)
                num = 0;
            while (num < ptr->total)
            {
                lseek(fd, (off_t) (offset + num * sizeof(fh)), SEEK_SET);
                if (read(fd, filename, STRLEN) <= 0
                    || brc_unread(filename))
                    break;
                num++;
            }
        }
        close(fd);
    }
    if (num < 0)
        num = 0;
    return num;
}

void show_brdlist(int page, int clsflag, int newflag)
{
    struct newpostdata *ptr;
    int n, i, inboard = -1;
    char tmpBM[BM_LEN - 4], inboardstring[80];

    if (clsflag)
    {
        clear();
        docmdtitle("[ÌÖÂÛÇøÁÐ±í]",
                   " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ý[m] ÁÐ³ö[[1;32my[m] ÅÅÐò[[1;32ms[m] ËÑÑ°[[1;32m/[m] ÇÐ»»[[1;32mc[m] ÇóÖú[[1;32mh[m]\n");
        prints
            ("[1;44;37m %s ÌÖÂÛÇøÃû³Æ     V  Àà±ð  ×ª %-25s   S °å  Ö÷       [m\n",
             newflag ? "È«²¿  Î´" : "  ±àºÅ  ",
             "ÖÐ  ÎÄ  Ðð  Êö      ÔÚÏß");
    }
    move(3, 0);
    if (brdnum == 0)
    {
        prints("    ¿ÕÄ¿Â¼");
        refresh();
        return;
    }

    for (n = page; n < page + BBS_PAGESIZE; n++)
    {

        if (n >= brdnum)
        {
            prints("\n");
            continue;
        }
        ptr = &nbrd[n];

        if ((ptr->flag2[0] & BOARD_DIR_FLAG))
        {
            strcpy(tmpBM, ptr->BM);
            ptr->BM[0] <= ' ' ? "SYSOP" : strtok(tmpBM, " ");
            if (!newflag)
            {
                prints("  %4d %s %-14s %s %-30s %s  %c%c%c %-12s\n",
                       n + 1, (ptr->zap
                               && !(ptr->flag & NOZAP_FLAG)) ? "£­" : "£«",
                       ptr->name, " ", ptr->title + 1,
                       "  \033[1;32m-\033[m", ' ', ' ', ptr->status,
                       "SYSOP");
            }
            else
            {
                prints("   [Ä¿Â¼]%c%-14s %s %-30s %s  %c%c%c %-12s\n",
                       (ptr->zap && !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                       ptr->name, " ", ptr->title + 1,
                       "  \033[1;32m-\033[m", ' ', ' ', ptr->status,
                       ptr->BM[0] <= ' ' ? "SYSOP" : strtok(tmpBM, " "));
            }
        }

        else
        {
            if (ptr->status != 'd')
            {
                if (!newflag)
                {
                    prints("  %4d %c ", n + 1, ptr->zap
                           && !(ptr->flag & NOZAP_FLAG) ? '-' : ' ');
                }
                else
                {
                    if (ptr->total == -1)
                    {
                        refresh();
                        check_newpost(ptr);
                    }
                    for (i = 0; i < MAXBOARD; i++)
                        if (!strcasecmp
                            (brdshm->bcache[i].filename, ptr->name))
                        {
                            inboard = brdshm->inboard[i];

                            if (inboard < 0)
                            {
                                inboard = 0;

                            }
                            break;
                        }
                    prints(" %5d %s", ptr->total,
                           ptr->unread ? "¡ô" : "¡ó");
                }
                strcpy(tmpBM, ptr->BM);

                getstring_bynum(inboardstring, inboard, 20, 40, 60);
                prints("%c%-14s %s %-30s %s  %c%c%c %-12s\n",
                       (ptr->zap && !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                       ptr->name,
                       (ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
                       ptr->title + 1,
                       inboardstring,
                       (ptr->flag & PUSH_FLAG) ? 'P' : ' ',
                       (ptr->flag & NOPOST_FLAG) ? 'w' : ' ',
                       HAS_PERM(PERM_POST) ? ptr->status : ' ',
                       ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖÐ" : strtok(tmpBM,
                                                                 " "));
            }
            else
            {
                prints(" %5d £« <Ä¿Â¼> %s\n", ptr->pos, ptr->name);
            }
        }
    }
    refresh();
}

int getptr_byn(struct boardcollection tmpstruct, int n,
               struct newpostdata *ptr, int dirnum, int brdnum)
{
    struct newgood ndir;
    struct shortfile *bptr;
    int pos;

    if (n >= dirnum)
        return 0;
    getname_byn(tmpstruct, n, &ndir, dirnum, brdnum);
    pos = get_pos(ndir.boards);
    bptr = &brdshm->bcache[pos];
    if (zapbuf == NULL)
    {
        load_zapbuf();
    }
    ptr->name = bptr->filename;
    ptr->title = bptr->title;
    ptr->BM = bptr->BM;
    ptr->flag = bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
    strncpy(ptr->flag2, bptr->flag2, 3);
    ptr->pos = pos;
    ptr->total = -1;
    ptr->zap = (zapbuf[pos] == 0);
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
    return 1;
}
int getname_byn(struct boardcollection tmpstruct, int n,
                struct newgood *ndir, int dirnum, int brdnum)
{
    if (n <= dirnum - 1 && n >= 0)
    {
        ndir->isdir = 1;
        while (n + 1 - tmpstruct.subnum > 0)
        {
            n -= tmpstruct.subnum;
            if (tmpstruct.extends[0] != 0)
            {
                if (f_get_file(tmpstruct.extends, &tmpstruct, 1) != 0)
                    return -1;
            }

        }
        snprintf(ndir->subdirname, 80, "%s", tmpstruct.subdirname[n]);
        snprintf(ndir->subdirsfile, 80, "%s", tmpstruct.subdirsfile[n]);

    }
    else if (n <= dirnum + brdnum - 1 && n >= dirnum)
    {
        ndir->isdir = 0;
        n -= dirnum;
        while (n + 1 - tmpstruct.boardnum > 0)
        {
            n -= tmpstruct.boardnum;
            if (tmpstruct.extends[0] != 0)
            {
                if (f_get_file(tmpstruct.extends, &tmpstruct, 1) != 0)
                    return -1;
            }
        }
        snprintf(ndir->boards, 80, "%s", tmpstruct.boards[n]);
    }
    else
        ndir->isdir = -1;
    return 0;
}
int
showgood_brdlist(int page, int clsflag, int newflag,
                 struct boardcollection currentstruct, int dirnum,
                 int brdnum, struct newpostdata *nbrd)
{
    struct newpostdata *ptr;
    struct newgood tmpgood;
    int n, i, inboard = -1;
    char tmpBM[BM_LEN - 4], inboardstring[80];

    if (clsflag)
    {
        clear();
        docmdtitle("[ÌÖÂÛÇøÁÐ±í]",
                   " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] ¸ÄÃû[[1;32mT[m,[1;32mt[m] Ë¢ÐÂ[[1;32mx[m] Ìí¼Ó[[1;32mA,a[m] É¾³ý[[1;32md[m] Ìø×ª[[1;32mTAB[m] ÇóÖú[[1;32mh[m]\n");
        prints
            ("[1;44;37m %s ÌÖÂÛÇøÃû³Æ     V  Àà±ð  ×ª %-25s   S °å  Ö÷       [m\n",
             newflag ? "È«²¿  Î´" : "  ±àºÅ  ",
             "ÖÐ  ÎÄ  Ðð  Êö      ÔÚÏß");
    }
    move(3, 0);
    if (dirnum + brdnum <= 0)
    {
        prints("       £­  <¿ÕÄ¿Â¼>\n");
    }
    else
    {
        for (n = page; n < page + BBS_PAGESIZE; n++)
        {
            getname_byn(currentstruct, n, &tmpgood, dirnum, brdnum);
            if (tmpgood.isdir == 1)
            {
                prints("       £« %-17s<Ä¿Â¼>\n", tmpgood.subdirname);
            }
            else if (tmpgood.isdir == 0)
            {
                ptr = &nbrd[n - dirnum];
                if (ptr->total == -1)
                {
                    refresh();
                    check_newpost(ptr);
                }
                for (i = 0; i < MAXBOARD; i++)
                    if (!strcasecmp(brdshm->bcache[i].filename, ptr->name))
                    {
                        inboard = brdshm->inboard[i];
                        if (inboard < 0)
                        {
                            inboard = 0;
                        }
                        break;
                    }
                if (inboard == -1)
                {
                    prints("       !! %-17s<´íÎó>\n", ptr->name);
                    continue;
                }
                prints(" %5d %s", ptr->total, ptr->unread ? "¡ô" : "¡ó");

                strcpy(tmpBM, ptr->BM);
                getstring_bynum(inboardstring, inboard, 20, 40, 60);
                prints("%c%-14s %s %-30s %s  %c%c%c %-12s\n",
                       (ptr->zap && !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                       brdshm->bcache[i].filename,
                       (ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
                       ptr->title + 1,
                       inboardstring,
                       (ptr->flag & PUSH_FLAG) ? 'P' : ' ',
                       (ptr->flag & NOPOST_FLAG) ? 'w' : ' ',
                       HAS_PERM(PERM_POST) ? ptr->status : ' ',
                       ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖÐ" : strtok(tmpBM,
                                                                 " "));
            }
            else if (n >= brdnum + dirnum)
            {
                prints("\n");
                continue;
            }
        }
    }
    refresh();
    return 0;
}

int read_dir(char *name, int newflag)
{

    int tmpgrp, tmpmode;

    tmpgrp = boardparent;
    tmpmode = choosemode;
    choosemode = 0;
    boardparent = getbnum(name) - 1;
    choose_board(newflag);
    boardparent = tmpgrp;
    choosemode = tmpmode;
    brdnum = -1;
    return 0;
}

int read_board(struct newpostdata *ptr, int newflag)
{
    char buf[STRLEN];

    int tmp;
    int page;

    brc_initial(ptr->name);
    memcpy(currBM, ptr->BM, BM_LEN - 1);
    if (DEFINE(DEF_FIRSTNEW))
    {
        setbdir(buf, currboard);
        tmp = unread_position(buf, ptr);
        page = tmp - t_lines / 2;
        getkeep(buf, page > 1 ? page : 1, tmp + 1);
    }
    Read();
    if (zapbuf[ptr->pos] > 0 && brc_num > 0)
    {
        zapbuf[ptr->pos] = brc_list[0];
    }
    ptr->total = -1;
    return 0;
}

int choose_board(int newflag)
{
    static int num;
    struct newpostdata newpost_buffer[MAXBOARD];
    struct newpostdata *ptr;
    int page = -1, ch = 0, tmp, number, tmpnum;
    char tch[4];
    int loop_mode = 0;

    if (!strcmp(currentuser.userid, "guest"))
        yank_flag = 1;
    nbrd = newpost_buffer;
    if (newflag == 2)
        modify_user_mode(FAVOURITE);
    else
        modify_user_mode(newflag ? READNEW : READBRD);
    brdnum = number = 0;
    clear();

    while (1)
    {
        if (brdnum <= 0)
        {
            if (load_boards() == -1)
                continue;
            qsort(nbrd, brdnum, sizeof(nbrd[0]), cmpboard);
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
                    if (ptr->flag2[0] & BOARD_DIR_FLAG)
                    {

                        break;
                    }
                    if (ptr->total == -1)
                        check_newpost(ptr);
                    if (ptr->unread)
                        break;
                    num++;
                }
                if (num >= brdnum)
                    num = tmp;
            }
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist(page, 1, newflag);
            update_endline();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist(page, 0, newflag);
            update_endline();
        }
        move(3 + num - page, 0);
        prints(">", number);
        if (loop_mode == 0)
        {
            ch = egetch();
        }
        move(3 + num - page, 0);
        prints(" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
            break;
        switch (ch)
        {
        case 'P':
        case 'b':
        case Ctrl('B'):
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
            show_brdlist(page, 1, newflag);
            break;
        case 'L':
            m_read();
            page = -1;
            break;
        case Ctrl('v'):
            anywhere_lockscreen();
            update_endline();
            break;
        case 'M':
            m_new();
            page = -1;
            break;
        case 'u':
            modify_user_mode(QUERY);
            t_query(NULL);
            page = -1;
            break;
        case 'H':
            {
                show_help("0Announce/bbslist/day");
                move(t_lines - 1, 0);
                tch[0] = '\0';
                getdata(t_lines - 1, 0, "²é¿´·ÖÇøÊ®´ó:(1~b)Çø 0)È¡Ïû [0]:",
                        tch, 3, DOECHO, YEA);
                char dList[] = "123456789ab";
                char top10file[64];

                while (tch[0] != '\0' && strchr(dList, tch[0]) != NULL)
                {
                    sprintf(top10file, "0Announce/bbslist/new%ctop10",
                            tch[0]);
                    show_help(top10file);
                    move(t_lines - 1, 0);
                    tch[0] = '\0';
                    getdata(t_lines - 1, 0,
                            "²é¿´·ÖÇøÊ®´ó:(1~b)Çø 0)È¡Ïû [0]:", tch, 3,
                            DOECHO, YEA);

                }
                page = -1;
                break;
            }

        case 'l':
            show_allmsgs();
            page = -1;
            break;
        case 'N':
        case ' ':
        case Ctrl('F'):
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
        case '!':
            return Goodbye();
            break;
        case 'h':
            show_help("help/boardreadhelp");
            page = -1;
            break;
        case '/':
            move(3 + num - page, 0);
            prints(">", number);
            tmpnum = num;
            tmp = search_board(&num);
            move(3 + tmpnum - page, 0);
            prints(" ", number);
            if (tmp == 1)
                loop_mode = 1;
            else
            {
                loop_mode = 0;
                update_endline();
            }
            break;
        case 's':
            currentuser.flags[0] ^= BRDSORT_FLAG;
            qsort(nbrd, brdnum, sizeof(nbrd[0]), cmpboard);
            page = 999;
            break;
        case 'y':
            if (GoodBrd_count)
                break;
            yank_flag = !yank_flag;
            brdnum = -1;
            break;
        case 'z':
            if (GoodBrd_count)
                break;

            if (HAS_PERM(PERM_BASIC) && !(nbrd[num].flag & NOZAP_FLAG))
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

#ifdef SPECIALID

            if (strcmp(currentuser.userid, SPECIALID1) == 0
                || strcmp(currentuser.userid, SPECIALID2) == 0)
                break;
#endif
            if (nbrd[num].flag2[0] & BOARD_DIR_FLAG)
                break;
            if (uinfo.mode == READNEW || uinfo.mode == READBRD)
            {

                char bname[STRLEN];
                char askynbuf[STRLEN];

                strcpy(bname, nbrd[num].name);
                sprintf(askynbuf, "½« %s °åÌí¼Óµ½¸öÈËÊÕ²Ø¼Ð", bname);
                move(t_lines - 1, 0);
                saveline(t_lines - 1, 0);
                clrtoeol();
                if (askyn(askynbuf, YEA, YEA) == YEA)
                {
                    do_addboard("main.hck", bname);
                }
                saveline(t_lines - 1, 1);
                break;

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

        case KEY_RIGHT:
            {
                tmp = num;
                num = 0;

                if (nbrd[tmp].flag2[0] & BOARD_DIR_FLAG)
                {
                    char name[16] = { 0 };
                    strcpy(name, nbrd[tmp].name);
                    read_dir(name, newflag);
                    nbrd = newpost_buffer;
                }
                else
                {
                    struct newpostdata data;
                    memcpy(&data, &nbrd[tmp], sizeof(struct newpostdata));
                    read_board(&data, newflag);
                    nbrd = newpost_buffer;
                    memcpy(&nbrd[tmp], &data, sizeof(struct newpostdata));
                }

                num = tmp;
                page = -1;
                break;
            }

        case 'S':
            if (HAS_PERM(PERM_MESSAGE))
            {
                s_msg();
                page = -1;
            }

            break;
        case 'f':
            if (!HAS_PERM(PERM_BASIC))
                break;
            t_friends();
            page = -1;
            break;
        default:
            ;
        }
        if (newflag == 2)
            modify_user_mode(FAVOURITE);
        else
            modify_user_mode(newflag ? READNEW : READBRD);
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
    clear();
    save_zapbuf();
    return -1;
}

char *brc_getrecord(char *ptr, char *name, int *pnum, int *list)
{
    int num;
    char *tmp;

    strncpy(name, ptr, BRC_STRLEN);
    ptr += BRC_STRLEN;
    num = (*ptr++) & 0xff;
    tmp = ptr + num * sizeof(int);
    if (num > BRC_MAXNUM)
    {
        num = BRC_MAXNUM;
    }
    *pnum = num;
    memcpy(list, ptr, num * sizeof(int));
    return tmp;
}

char *brc_putrecord(char *ptr, char *name, int num, int *list)
{
    if (num > 0)
    {
        if (num > BRC_MAXNUM)
        {
            num = BRC_MAXNUM;
        }
        strncpy(ptr, name, BRC_STRLEN);
        ptr += BRC_STRLEN;
        *ptr++ = num;
        memcpy(ptr, list, num * sizeof(int));
        ptr += num * sizeof(int);
    }
    return ptr;
}

int brc_putpushrecord(void)
{
    int fd, i;
    char filepath[80];

    snprintf(filepath, 80, "home/%c/%c/%s/.push.%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             currboard);
    if ((fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;
    for (i = 0; i <= brc_pushnum - 1; i++)
    {
        write(fd, &brc_pushlist[i], sizeof(struct unread_push));
    }
    close(fd);
    return 0;
}

int brc_getpushrecord(void)
{
    int fd;
    char filepath[80];

    snprintf(filepath, 80, "home/%c/%c/%s/.push.%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             currboard);
    if (!strcmp(lastboard, currboard))
        return -1;
    snprintf(lastboard, 80, "%s", currboard);
    brc_pushnum = 0;
    currpushnum = 0;
    if ((fd = open(filepath, O_RDONLY | O_CREAT, 0644)) == -1)
        return -1;
    while (read(fd, &brc_pushlist[brc_pushnum], sizeof(struct unread_push))
           == sizeof(struct unread_push))
    {
        brc_pushnum++;
    }
    currpushnum = brc_pushnum;
    close(fd);
    return 0;
}

void brc_update(void)
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
        ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
    }
    if (1)
    {
        setuserfile(dirfile, ".boardrc");
        if ((fd = open(dirfile, O_RDONLY)) != -1)
        {
            tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
            close(fd);
        }
        else
        {
            tmp_size = 0;
        }
    }
    tmp = tmp_buf;
    while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
    {
        tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
        if (strncmp(tmp_name, currboard, BRC_STRLEN) != 0)
        {
            ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
        }
    }
    brc_size = (int) (ptr - brc_buf);

    if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
    {
        ftruncate(fd, 0);
        write(fd, brc_buf, brc_size);
        close(fd);
    }
    brc_changed = 0;
}

int brc_initial(char *boardname)
{
    char dirfile[STRLEN], *ptr;
    int fd;

    if (strcmp(currboard, boardname) == 0)
    {
        return brc_num;
    }
    brc_update();
    strcpy(currboard, boardname);
    brc_changed = 0;
    if (brc_buf[0] == '\0')
    {
        setuserfile(dirfile, ".boardrc");
        if ((fd = open(dirfile, O_RDONLY)) != -1)
        {
            brc_size = read(fd, brc_buf, sizeof(brc_buf));
            close(fd);
        }
        else
        {
            brc_size = 0;
        }
    }
    ptr = brc_buf;
    while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
    {
        ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
        if (strncmp(brc_name, currboard, BRC_STRLEN) == 0)
        {
            return brc_num;
        }
    }
    strncpy(brc_name, boardname, BRC_STRLEN);
    brc_list[0] = 1;
    brc_num = 1;
    return 0;
}

void brc_addlist(char *filename)
{
    int ftime, n, i;

    if (!strcmp(currentuser.userid, "guest"))
        return;
    ftime = atoi(&filename[2]);
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
int brc_updatepush(struct fileheader *fileinfo)
{
    int ftime, n;

    ftime = atoi(&(fileinfo->filename[2]));
    if ((fileinfo->filename[0] != 'M' && fileinfo->filename[0] != 'G')
        || fileinfo->filename[1] != '.')
    {
        return 0;
    }
    if (fileinfo->pnum <= 0 || fileinfo->pnum > 10000)
        return 0;
    if (brc_pushnum < 0)
        brc_pushnum = 0;
    if (currpushnum < 0)
        currpushnum = 0;
    for (n = 0; n < brc_pushnum; n++)
    {
        if (ftime == brc_pushlist[n].filename)
        {
            brc_pushlist[n].num = fileinfo->pnum;
            return 1;
        }
    }

    brc_pushnum++;
    if (brc_pushnum >= BRC_PUSHMAXNUM)
        brc_pushnum = BRC_PUSHMAXNUM;
    currpushnum++;
    if (currpushnum >= BRC_PUSHMAXNUM)
        currpushnum = 1;
    brc_pushlist[currpushnum - 1].filename = ftime;
    brc_pushlist[currpushnum - 1].num = fileinfo->pnum;
    return 1;
}
int brc_pushunread(struct fileheader *fileinfo)
{
    int ftime, n;

    ftime = atoi(&(fileinfo->filename[2]));
    if ((fileinfo->filename[0] != 'M' && fileinfo->filename[0] != 'G')
        || fileinfo->filename[1] != '.')
    {
        return 0;
    }
    if (fileinfo->pnum <= 0 || fileinfo->pnum > 10000)
        return 0;
    if (brc_pushnum < 0)
        brc_pushnum = 0;
    if (currpushnum < 0)
        currpushnum = 0;
    for (n = 0; n < brc_pushnum; n++)
    {
        if (ftime == brc_pushlist[n].filename)
        {
            if (fileinfo->pnum > (unsigned int) brc_pushlist[n].num)
            {
                return 1;
            }
            else if (fileinfo->pnum == (unsigned int) brc_pushlist[n].num)
                return 0;
        }
    }

    brc_pushnum++;
    if (brc_pushnum >= BRC_PUSHMAXNUM)
        brc_pushnum = BRC_PUSHMAXNUM;
    currpushnum++;
    if (currpushnum >= BRC_PUSHMAXNUM)
        currpushnum = 1;
    brc_pushlist[currpushnum - 1].filename = ftime;
    brc_pushlist[currpushnum - 1].num = 0;
    return 1;
}
int brc_unread(char *filename)
{
    int ftime, n;

    ftime = atoi(&filename[2]);
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

int addtodenytalk(char *uident, char *msg, int ischange)
{
    char buf[50], strtosave[256], buf2[50];
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;

    seek = SeekInNameList(uident);
    if (seek && !ischange)
    {
        move(3, 0);
        prints(" %s ÒÑ¾­ÔÚÃûµ¥ÖÐ¡£", uident);
        pressanykey();
        return -1;
    }
    if (ischange && !seek)
    {
        move(3, 0);
        prints(" %s ²»ÔÚÃûµ¥ÖÐ¡£", uident);
        pressreturn();
        return -1;
    }
    while (1)
    {
        getdata(3, 0, "ÊäÈëËµÃ÷: ", buf, 40, DOECHO, YEA);
        if (killwordsp(buf) == 0)
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
    nowtime = time(0);
    getdatestring(nowtime, NA);
    getdata(4, 0,
            "ÊäÈë·â½ûÊ±¼ä[È±Ê¡Îª 1 Ìì, 0 Îª·ÅÆú, ÊÖ¶¯½â·âÔÚÊ±¼äÇ°¼Ó'.']: ",
            buf2, 4, DOECHO, YEA);
    if (buf2[0] == '\0')
        day = 1;
    else if (buf2[0] == '.')
    {
        fAutoUndeny = 0;
        day = atoi(buf2 + 1);
    }
    else
    {
        fAutoUndeny = 1;
        day = atoi(buf2);
    }
    if (day <= 0)
        return -1;
    if (day >= 30)
        day = 30;
    nowtime += day * 86400;
    getdatestring(nowtime, NA);
    sprintf(strtosave, "%-12s %-40s %s%16.16sµã½â", uident, buf,
            fAutoUndeny ? "(a) " : "(n) ", datestring);
    if (!ischange)
    {
        sprintf(msg,
                "\n  %s ÍøÓÑ: \n\n"
                "\tÎÒºÜ±§Ç¸µØ¸æËßÄú£¬ÄúÒÑ¾­±»È¡Ïû ½øÈëÁÄÌìÊÒ µÄÈ¨Àû\n\n"
                "\tÄú±»·â½ûµÄÔ­ÒòÊÇ£º [%s]\n\n"
                "\tÒòÎªÄúµÄÐÐÎª£¬¾ö¶¨Í£Ö¹Äã½øÈëÁÄÌìÊÒµÄÈ¨Àû [%d] Ìì\n\n"
                "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ÐÅÉêÇë½â·â¡£\n\n",
                uident, buf, day, datestring, currentuser.userid);
    }
    else
    {
        sprintf(msg,
                "\n  %s ÍøÓÑ: \n\n"
                "\t¹ØÓÚÄã½øÈëÁÄÌìÊÒÈ¨ÏÞ±»È¡ÏûµÄÎÊÌâ£¬ÏÖ±ä¸üÈçÏÂ:\n\n"
                "\t·â½ûµÄÔ­Òò£º [%s]\n\n"
                "\t´ÓÏÖÔÚ¿ªÊ¼£¬Í£Ö¹¸ÃÈ¨ÀûÊ±¼ä£º [%d] Ìì\n\n"
                "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ÐÅÉêÇë½â·â¡£\n\n",
                uident, buf, day, datestring, currentuser.userid);
    }
    sprintf(genbuf, "boards/deny_talk");

    if (ischange)
        del_from_file(genbuf, uident);
    return add_to_file(genbuf, strtosave);
}

int deldenytalk(char *uident)
{
    char fn[STRLEN];

    sprintf(fn, "boards/deny_talk");
    return del_from_file(fn, uident);
}

int x_CleanMainHck()
{
    struct boardcollection currentstruct;

    clear();
    if (askyn("ÄãÈ·¶¨ÒªÇå¿ÕÊÕ²Ø¼ÐÂð?", NA, NA) == NA)
    {
        prints("È¡ÏûÇå¿ÕÊÕ²Ø¼Ð");
        pressanykey();
        clear();
        return 0;
    }

    init_dir(&currentstruct, "main.hck", "main.hck");
    unlink(currentstruct.dirname);
    f_replace_file(currentstruct);
    prints("ÊÕ²Ø¼ÐÒÑÇå¿Õ");
    pressanykey();
    clear();

    return 1;
}
