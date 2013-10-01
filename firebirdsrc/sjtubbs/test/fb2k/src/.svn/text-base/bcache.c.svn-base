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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>
#define chartoupper(c)  ((c >= 'a' && c <= 'z') ? c+'A'-'a' : c)

struct BCACHE *brdshm = NULL;
struct UCACHE *uidshm = NULL;
struct UTMPFILE *utmpshm = NULL;
struct userec lookupuser;
struct shortfile *bcache = NULL;
int usernumber = 0;
int numboards = -1;
struct SHM_t *bbsshm = NULL;
void attach_err(int shmkey, char *name, int err)
{
    sprintf(genbuf, "Error! %s error #%d! key = %x.\n", name, err, shmkey);
    write(1, genbuf, strlen(genbuf));
    exit(1);
}

int attach_shm(void)
{

    int shmid;

    if (bbsshm != NULL)
        return 0;
#ifdef USE_SHM_HUGETLB
    shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE, SHM_HUGETLB);
#else
    shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE, 0);
#endif
    if (shmid < 0)
    {
#ifdef USE_SHM_HUGETLB
        shmid =
            shmget(BBS_SHM_KEY, BBS_SHM_SIZE,
                   IPC_CREAT | 0600 | SHM_HUGETLB);
#else
        shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE, IPC_CREAT | 0600);
#endif
        if (shmid < 0)
            attach_err(BBS_SHM_KEY, "shmget", errno);
        bbsshm = (void *) shmat(shmid, NULL, 0);
        if (bbsshm == (void *) -1)
            attach_err(BBS_SHM_KEY, "shmat", errno);
        memset(bbsshm, 0, BBS_SHM_SIZE);
    }
    else
    {
        bbsshm = (void *) shmat(shmid, NULL, 0);
        if (bbsshm == (void *) -1)
            attach_err(BBS_SHM_KEY, "shmat", errno);
    }
    return 0;
}

int fillbcache(void *vfptr)
{
    struct boardheader *fptr = (struct boardheader *) vfptr;
    struct shortfile *bptr;

    if (numboards >= MAXBOARD)
        return 0;
    bptr = &bcache[numboards++];
    memcpy(bptr, fptr, sizeof(struct shortfile));
    return 0;
}

void resolve_boards(void)
{
    struct stat st;
    time_t now;
    FILE *fp_nopost = NULL;
    char buffer[2][16] = { {0}, {0} };
    FILE *fp_filter = NULL;
    char mybuffer[256] = { 0 };
    int mycount = 0;

    if (brdshm == NULL)
    {
        attach_shm();
        brdshm = &(bbsshm->bcacheshm);
    }
    numboards = brdshm->number;
    bcache = brdshm->bcache;
    now = time(0);
    if (stat(BOARDS, &st) < 0)
    {
        st.st_mtime = now - 3600;
    }
    if (brdshm->uptime < st.st_mtime || brdshm->uptime < now - 3600)
    {
        log_usies("CACHE", "reload bcache");
        numboards = 0;
        apply_record(BOARDS, fillbcache, sizeof(struct boardheader));
        brdshm->number = numboards;
        brdshm->uptime = now;
    }

    fp_nopost = fopen("NO_POST_TIME", "r");
    if (fp_nopost == NULL)
    {
        brdshm->no_post_begin = 0;
        brdshm->no_post_end = 0;
    }
    else
    {
        fgets(buffer[1], 15, fp_nopost);
        fgets(buffer[2], 15, fp_nopost);
        brdshm->no_post_begin = atoi(buffer[1]);
        brdshm->no_post_end = atoi(buffer[2]);
        fclose(fp_nopost);
    }
    fp_nopost = NULL;
    fp_nopost = fopen("ALL_NO_POST_TIME", "r");
    if (fp_nopost == NULL)
    {
        brdshm->all_no_post_begin = 0;
        brdshm->all_no_post_end = 0;
    }
    else
    {
        fgets(buffer[1], 15, fp_nopost);
        fgets(buffer[2], 15, fp_nopost);
        brdshm->all_no_post_begin = atoi(buffer[1]);
        brdshm->all_no_post_end = atoi(buffer[2]);
        fclose(fp_nopost);
    }

    fp_filter = fopen("./etc/keywords", "r");
    for (mycount = 0; mycount < MAX_ITEM; mycount++)
    {
        memset(brdshm->wordfilter[mycount], 0x00, MAX_FILTER_LENGTH);
    }
    if (fp_filter != NULL)
    {
        int item_count = 0;

        while (fgets(mybuffer, sizeof(mybuffer), fp_filter))
        {
            if (item_count >= MAX_ITEM)
                break;
            if (strlen(mybuffer) < 4)
            {
                memset(mybuffer, 0x00, 256);
                continue;
            }
            if (strlen(mybuffer) > MAX_FILTER_LENGTH + 1)
            {
                mybuffer[MAX_FILTER_LENGTH] = '\0';
                mybuffer[MAX_FILTER_LENGTH + 1] = '\0';
            }
            if (mybuffer[strlen(mybuffer) - 1] == '\n')
            {
                mybuffer[strlen(mybuffer) - 1] = '\0';
            }
            strncpy(brdshm->wordfilter[item_count], mybuffer,
                    strlen(mybuffer));
            item_count++;
            memset(mybuffer, 0x00, 256);
        }
        fclose(fp_filter);
    }

}

int chk_list(board)
char *board;
{
    char b[STRLEN + 17];

    setbfile(b, board, "board.allow");
    if (dashf(b) && !seek_in_file(b, currentuser.userid))
        return 0;
    else
        return 1;
}

int apply_boards(int (*func) (struct shortfile *))
{
    register int i;

    resolve_boards();
    for (i = 0; i < numboards; i++)
        if ((bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level)
             || (bcache[i].level & PERM_NOZAP)) && (HAS_PERM(PERM_SYSOP)
                                                    || chk_list(bcache[i].
                                                                filename)))
            if ((*func) (&bcache[i]) == QUIT)
                return QUIT;
    return 0;
}

struct shortfile *getbcache(char *bname)
{
    register int i;

    resolve_boards();
    for (i = 0; i < numboards; i++)
        if (!strncasecmp(bname, bcache[i].filename, STRLEN))
            return &bcache[i];
    return NULL;
}

int getbnum(char *bname)
{
    register int i;

    resolve_boards();

    for (i = 0; i < numboards; i++)
    {
        if (bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level)
            || (bcache[i].level & PERM_NOZAP))
        {
            if (!strncasecmp(bname, bcache[i].filename, STRLEN))
                return i + 1;
        }
    }
    return 0;
}

void setoboard(char *bname)
{
    register int i;

    resolve_boards();
    for (i = 0; i < numboards; i++)
    {
        if (bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level)
            || (bcache[i].level & PERM_NOZAP))
        {
            if (bcache[i].filename[0] != 0 && bcache[i].filename[0] != ' ')
            {
                strcpy(bname, bcache[i].filename);
                return;
            }
        }
    }
}

int haspostperm(char *bname)
{
    register int i;

    if (deny_me(bname) && !HAS_PERM(PERM_SYSOP))
        return 0;
    if ( strcmp(currentuser.userid, "guest"))
    {
        if (strcmp(bname, DEFAULTBOARD) == 0)
            return 1;
        if (strcmp(bname, "BBSHelp") == 0)
            return 1;
        if (strcmp(bname, "delation") == 0)
            return 1;
        if (strcmp(bname, "Court") == 0)
            return 1;
        if (strcmp(bname, "WelcomeNewer") == 0)
            return 1;
    }
    if ((i = getbnum(bname)) == 0)
        return 0;
    set_safe_record();
    if (!HAS_PERM(PERM_POST))
        return 0;
    return (HAS_PERM
            ((bcache[i - 1].level & ~PERM_NOZAP) & ~PERM_POSTMASK));
}

int normal_board(char *bname)
{
    register int i;

    if (strcmp(bname, DEFAULTBOARD) == 0)
        return 1;
    if ((i = getbnum(bname)) == 0)
        return 0;
    if (bcache[i - 1].level & PERM_NOZAP)
        return 1;
    return (bcache[i - 1].level == 0);
}

int fillucache(void *vuentp)
{
    struct userec *uentp = (struct userec *) vuentp;

    if (usernumber < MAXUSERS)
    {
        strncpy(uidshm->userid[usernumber], uentp->userid, IDLEN + 1);
        uidshm->userid[usernumber++][IDLEN] = '\0';
    }
    return 0;
}

void resolve_ucache(void)
{
    struct stat st;
    int ftime;

    if (uidshm == NULL)
    {
        attach_shm();
        uidshm = &(bbsshm->ucacheshm);
    }
    if (stat(FLUSH, &st) < 0)
    {
        ftime = time(0) - 86400;
    }
    else
        ftime = st.st_mtime;
    if (uidshm->uptime < ftime)
    {
        log_usies("CACHE", "reload ucache");

        usernumber = 0;
        apply_record(PASSFILE, fillucache, sizeof(struct userec));
        uidshm->number = usernumber;
        uidshm->uptime = ftime;
    }
}

void setuserid(int num, char *userid)
{
    if (num > 0 && num <= MAXUSERS)
    {
        if (num > uidshm->number)
            uidshm->number = num;
        strncpy(uidshm->userid[num - 1], userid, IDLEN + 1);
    }
}

int searchnewuser(void)
{
    register int num, i;

    resolve_ucache();
    num = uidshm->number;
    for (i = 0; i < num; i++)
        if (uidshm->userid[i][0] == '\0')
            return i + 1;
    if (num < MAXUSERS)
        return (num + 1);
    return 0;
}

void getuserid(char *userid, unsigned int uid)
{
    resolve_ucache();
    strcpy(userid, uidshm->userid[uid - 1]);
}

int searchuser(char *userid)
{
    register int i;

    resolve_ucache();
    for (i = 0; i < uidshm->number; i++)
        if (!strncasecmp(userid, uidshm->userid[i], IDLEN + 1))
            return i + 1;
    return 0;
}

int getuser(char *userid)
{
    static char olduserid[IDLEN + 1] = "";
    static int olduid = 0;
    static struct userec olduser;

    if (!strncmp(olduserid, userid, IDLEN + 1))
    {
        memcpy(&lookupuser, &olduser, sizeof(struct userec));
        return olduid;
    }
    int uid = searchuser(userid);

    if (uid == 0)
        return 0;
    get_record(PASSFILE, &lookupuser, sizeof(lookupuser), uid);
    memcpy(&olduser, &lookupuser, sizeof(struct userec));
    olduid = uid;
    strncpy(olduserid, userid, IDLEN);
    olduserid[IDLEN] = '\0';
    return uid;
}

char *u_namearray(char (*buf)[13], int *pnum, char *tag)
{
    register struct UCACHE *reg_ushm = uidshm;
    register char *ptr, tmp;
    register int n, total;
    char tagbuf[STRLEN];
    int ch, num = 0;

    resolve_ucache();
    if (*tag == '\0')
    {
        *pnum = reg_ushm->number;
        return reg_ushm->userid[0];
    }
    for (n = 0; tag[n] != '\0'; n++)
    {
        tagbuf[n] = chartoupper(tag[n]);
    }
    tagbuf[n] = '\0';
    ch = tagbuf[0];
    total = reg_ushm->number;
    for (n = 0; n < total; n++)
    {
        ptr = reg_ushm->userid[n];
        tmp = *ptr;
        if (tmp == ch || tmp == ch - 'A' + 'a')
            if (chkstr(tag, tagbuf, ptr))
                strcpy(buf[num++], ptr);
    }
    *pnum = num;
    return buf[0];
}

void resolve_utmp(void)
{
    if (utmpshm == NULL)
    {
        attach_shm();
        utmpshm = &(bbsshm->utmpshm);
    }
}

int getnewutmpent(struct user_info *up)
{
    int utmpfd;
    struct user_info *uentp;
    time_t now;
    int i, n, num[2];

    utmpfd = open(ULIST, O_RDWR | O_CREAT, 0600);
    if (utmpfd < 0)
        return -1;

    resolve_utmp();

    if (flock(utmpfd, LOCK_EX) < 0)
    {
        close(utmpfd);
        report("Warning on flock ULIST failed!");
        return -1;
    }
    if (utmpshm->max_login_num < count_users)
        utmpshm->max_login_num = count_users;
    for (i = 0; i < USHM_SIZE; i++)
    {
        uentp = &(utmpshm->uinfo[i]);
        if (!uentp->active || !uentp->pid)
            break;
    }
    if (i >= USHM_SIZE)
    {
        flock(utmpfd, LOCK_UN);
        close(utmpfd);
        return -2;
    }
    utmpshm->uinfo[i] = *up;

    now = time(0);
    if (now > utmpshm->uptime + 60)
    {
        num[0] = num[1] = 0;
        utmpshm->uptime = now;
        for (n = 0; n < USHM_SIZE; n++)
        {
            uentp = &(utmpshm->uinfo[n]);
            if (uentp->active && uentp->pid)
            {

                if (!is_web_user(uentp) && kill(uentp->pid, 0) == -1)
                {
                    memset(uentp, 0, sizeof(struct user_info));
                    continue;
                }
                else
                {
                    num[(uentp->invisible == YEA) ? 1 : 0]++;
                }
            }
        }
        utmpshm->usersum = allusers();
        n = USHM_SIZE - 1;
        while (n > 0 && utmpshm->uinfo[n].active == 0)
            n--;
        ftruncate(utmpfd, 0);
        write(utmpfd, utmpshm->uinfo, (n + 1) * sizeof(struct user_info));
    }
    flock(utmpfd, LOCK_UN);
    close(utmpfd);
    return i + 1;
}

int apply_ulist(int (*fptr) (struct user_info *))
{
    struct user_info *uentp, utmp;
    int i, max;

    resolve_utmp();
    max = USHM_SIZE - 1;
    while (max > 0 && utmpshm->uinfo[max].active == 0)
        max--;
    for (i = 0; i <= max; i++)
    {
        uentp = &(utmpshm->uinfo[i]);
        utmp = *uentp;
        if ((*fptr) (&utmp) == QUIT)
            return QUIT;
    }
    return 0;
}

int apply_ulist_address(int (*fptr) (struct user_info *))
{
    int i, max;

    resolve_utmp();
    max = USHM_SIZE - 1;
    while (max > 0 && utmpshm->uinfo[max].active == 0)
        max--;
    for (i = 0; i <= max; i++)
    {
        if ((*fptr) (&(utmpshm->uinfo[i])) == QUIT)
            return QUIT;
    }
    return 0;
}

int
search_ulist(struct user_info *uentp,
             int (*fptr) (int, struct user_info *), int farg)
{
    int i;

    resolve_utmp();
    for (i = 0; i < USHM_SIZE; i++)
    {
        *uentp = utmpshm->uinfo[i];
        if ((*fptr) (farg, uentp))
            return i + 1;
    }
    return 0;
}

int
search_ulistn(struct user_info *uentp,
              int (*fptr) (int, struct user_info *), int farg, int unum)
{
    int i, j;

    j = 1;
    resolve_utmp();
    for (i = 0; i < USHM_SIZE; i++)
    {
        *uentp = utmpshm->uinfo[i];
        if ((*fptr) (farg, uentp))
        {
            if (j == unum)
                return i + 1;
            else
                j++;
        }
    }
    return 0;
}

int
t_search_ulist(struct user_info *uentp,
               int (*fptr) (int, struct user_info *), int farg, int show,
               int doTalk)
{
    int i, num;
    char col[14];

    resolve_utmp();
    num = 0;
    for (i = 0; i < USHM_SIZE; i++)
    {
        *uentp = utmpshm->uinfo[i];
        if ((*fptr) (farg, uentp))
        {
            if (!uentp->active || !uentp->pid || isreject(uentp))
                continue;
            if ((uentp->invisible == 0) || (uentp->uid == usernum)
                || ((uentp->invisible == 1)
                    && HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)))
            {
                num++;
            }
            else
            {
                continue;
            }
            if (!show)
                continue;
            if (num == 1)
                prints("Ä¿Ç° %s ×´Ì¬ÈçÏÂ£º\n", uentp->userid);
            if (uentp->invisible)
                strcpy(col, "[Òþ][1;36m");
            else if (uentp->mode == POSTING)
                strcpy(col, "[1;32m");
            else if (uentp->mode == BBSNET)
                strcpy(col, "[1;33m");
            else
                strcpy(col, "[1m");
            if (doTalk)
            {
                prints("(%d) ×´Ì¬£º%s%-10s[m£¬À´×Ô£º%.20s\n",
                       num, col, ModeType(uentp->mode), uentp->from);
            }
            else
            {
                prints("%s%-10s[m ", col, ModeType(uentp->mode));
                if ((num) % 5 == 0)
                    outc('\n');
            }
        }
    }
    if (show)
        outc('\n');
    return num;
}

void update_ulist(struct user_info *uentp, int uent)
{
    resolve_utmp();
    if (uent > 0 && uent <= USHM_SIZE)
    {
        utmpshm->uinfo[uent - 1] = *uentp;
    }
}

void update_utmp(void)
{
    update_ulist(&uinfo, utmpent);
}

int
who_callme(struct user_info *uentp, int (*fptr) (int, struct user_info *),
           int farg, int me)
{
    int i;

    resolve_utmp();
    for (i = 0; i < USHM_SIZE; i++)
    {
        *uentp = utmpshm->uinfo[i];
        if ((*fptr) (farg, uentp) && uentp->destuid == me)
            return i + 1;
    }
    return 0;
}
