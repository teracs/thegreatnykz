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

#define BUFSIZE (8192)
char genbuf[1024];
struct UCACHE *uidshm = NULL;
struct SHM_t *bbsshm = NULL;

int usernumber;

char datestring[30];

int getdatestring(time_t now, int Chang)
{
    struct tm *tm;
    char weeknum[7][3] = { "Ìì", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù" };

    tm = localtime(&now);
    sprintf(datestring, "%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
    return (tm->tm_sec % 10);
}
extern void file_append(char *fpath, char *msg);
void log_usies(char *mode, char *mesg)
{
    time_t now;
    char buf[256];

    now = time(0);
    getdatestring(now, NA);
    sprintf(buf, "%s %s %s\n", datestring, mode, mesg);
    file_append("usies", buf);
}

int compute_user_value(struct userec *urec)
{
    int value;

    if ((urec->userlevel & PERM_XEMPT)
        || strcmp(urec->userid, "SYSOP") == 0
        || strcmp(urec->userid, "guest") == 0)
        return 999;

    if (urec->userlevel & PERM_666LIFE)
    {
        return 666;
    }

    value = (time(0) - urec->lastlogin) / 60;

    if (strcmp(urec->userid, "new") == 0)
    {
        return (30 - value) * 60;
    }
    if (urec->numlogins <= 3)
        return (15 * 1440 - value) / 1440;
    if (!(urec->userlevel & PERM_LOGINOK))
        return (30 * 1440 - value) / 1440;
    if (urec->stay > 1000000)
        return (365 * 1440 - value) / 1440;
    return (120 * 1440 - value) / 1440;
}

int danger_user_id(char *userid)
{
    char *ptr, ch;

    ptr = userid;
    while ((ch = *ptr++) != '\0')
    {
        if (!isalnum(ch) && ch != '_')
            return 1;
    }
    return 0;
}

char *horoscope(char month, char day)
{
    char *name[12] = {
        "Ä¦ôÉ", "Ë®Æ¿", "Ë«Óã", "ÄµÑò", "½ðÅ£", "Ë«×Ó",
        "¾ÞÐ·", "Ê¨×Ó", "´¦Å®", "Ìì³Ó", "ÌìÐ«", "ÉäÊÖ"
    };

    switch (month)
    {
    case 1:
        if (day < 21)
            return (name[0]);
        else
            return (name[1]);
    case 2:
        if (day < 19)
            return (name[1]);
        else
            return (name[2]);
    case 3:
        if (day < 21)
            return (name[2]);
        else
            return (name[3]);
    case 4:
        if (day < 21)
            return (name[3]);
        else
            return (name[4]);
    case 5:
        if (day < 21)
            return (name[4]);
        else
            return (name[5]);
    case 6:
        if (day < 22)
            return (name[5]);
        else
            return (name[6]);
    case 7:
        if (day < 23)
            return (name[6]);
        else
            return (name[7]);
    case 8:
        if (day < 23)
            return (name[7]);
        else
            return (name[8]);
    case 9:
        if (day < 23)
            return (name[8]);
        else
            return (name[9]);
    case 10:
        if (day < 24)
            return (name[9]);
        else
            return (name[10]);
    case 11:
        if (day < 23)
            return (name[10]);
        else
            return (name[11]);
    case 12:
        if (day < 22)
            return (name[11]);
        else
            return (name[0]);
    }
    return ("²»Ïê");
}

static int resolve_ucache(void);
extern int f_rm(char *fpath);

int main(int argc, char **argv)
{
    struct userec utmp, utmp2;
    int fd, val, i;
    struct userec zerorec;
    int size, j, re;
    FILE *fdtmp;
    char nname[50], buf[50], buf2[50], genbuf[256];

    (void) chdir(BBSHOME);
    resolve_ucache();
    strcpy(nname, "tmp/bbs.killid");
    fdtmp = fopen(nname, "w+");
    log_usies("CLEAN", "dated users.");
    memset(&zerorec, 0, sizeof(zerorec));
    if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
        return -1;
    flock(fd, LOCK_EX);
    size = sizeof(utmp);
    for (i = 0; i < MAXUSERS; i++)
    {
        if (read(fd, &utmp, size) != size)
            break;
        val = compute_user_value(&utmp);
        if (utmp.userid[0] != '\0' && val < 0)
        {
            if (lseek(fd, (off_t) (-size), SEEK_CUR) == -1)
            {
                flock(fd, LOCK_UN);
                close(fd);
                return -1;
            }
            write(fd, &zerorec, sizeof(utmp));
            memset(uidshm->userid[i], 0, IDLEN + 1);
            if (strcmp(utmp.userid, "new") == 0)
                continue;
            re = 0;
            lseek(fd, 0, SEEK_SET);
            for (j = 0; j < i; j++)
            {
                if (read(fd, &utmp2, size) != size)
                    break;
                if (strcmp(utmp.userid, utmp2.userid) == 0)
                {
                    re = 1;
                    break;
                }
            }
            lseek(fd, (off_t) (i + 1) * size, SEEK_SET);
            for (j = i + 1; j < MAXUSERS; j++)
            {
                if (read(fd, &utmp2, size) != size)
                    break;
                if (strcmp(utmp.userid, utmp2.userid) == 0)
                {
                    if (lseek(fd, (off_t) (-size), SEEK_CUR) == -1)
                    {
                        flock(fd, LOCK_UN);
                        close(fd);
                        return -1;
                    }
                    write(fd, &zerorec, sizeof(utmp));
                    memset(uidshm->userid[j], 0, IDLEN + 1);
                }
            }
            lseek(fd, (off_t) (i + 1) * size, SEEK_SET);
            if (re)
                continue;
            getdatestring(utmp.lastlogin, NA);
            sprintf(genbuf, "#%d %-12s %14.14s %d %d %d", i + 1,
                    utmp.userid, datestring, utmp.numlogins, utmp.numposts,
                    val);
            log_usies("KILL ", genbuf);
            if (!danger_user_id(utmp.userid))
            {
                if (snprintf(genbuf, sizeof(genbuf), "mail/%c/%c/%s",
                             toupper(utmp.userid[0]),
                             toupper(utmp.userid[1]),
                             utmp.userid) < (int) sizeof(genbuf))
                {
                    f_rm(genbuf);
                }
                if (snprintf(genbuf, sizeof(genbuf), "home/%c/%c/%s",
                             toupper(utmp.userid[0]),
                             toupper(utmp.userid[1]),
                             utmp.userid) < (int) sizeof(genbuf))
                {
                    f_rm(genbuf);
                }
                sprintf(buf, "[[1;3%dm%s×ù[m] ",
                        (utmp.gender == 'F') ? 5 : 6,
                        horoscope(utmp.birthmonth, utmp.birthday));
                sprintf(buf2, "ÍøÁä[[1;32m%ld[0;1m]Ìì",
                        (time(0) - utmp.firstlogin) / 86400);
                fprintf(fdtmp,
                        "[1;37m%s [m([1;33m%s[m) ¹²ÉÏÕ¾ [1;32m%d[m ´Î  %s  %s\n",
                        utmp.userid, utmp.username, utmp.numlogins, buf2,
                        buf);
                getdatestring(utmp.lastlogin, NA);
                fprintf(fdtmp,
                        "ÉÏ ´Î ÔÚ: [[1;32m%s[m] ´Ó [[1;32m%s[m] µ½±¾Õ¾Ò»ÓÎ¡£\n",
                        datestring, utmp.lasthost);
                getdatestring(utmp.lastlogout, NA);
                fprintf(fdtmp,
                        "ÀëÕ¾Ê±¼ä: [[1;32m%s[m] ÉúÃüÁ¦:[[1;32m%d[m] ÎÄÕÂ:[[1;32m%d[m] \n\n",
                        datestring, val, utmp.numposts);
            }
        }
    }
    flock(fd, LOCK_UN);
    close(fd);
    fclose(fdtmp);
    return 0;
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

int apply_record(char *filename, int (*fptr) (void *), int size)
{
    char abuf[BUFSIZE];
    int fd;

    if (size > BUFSIZE)
    {
        log_usies("ERROR", "too big record size!!");
        return -1;
    }

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return -1;
    while (read(fd, abuf, size) == size)
        if ((*fptr) (abuf) == QUIT)
        {
            close(fd);
            return QUIT;
        }
    close(fd);
    return 0;
}
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

int resolve_ucache(void)
{

    int ftime;

    if (uidshm == NULL)
    {
        attach_shm();
        uidshm = &(bbsshm->ucacheshm);
    }
    ftime = time(0) - 86400;
    usernumber = 0;
    apply_record(PASSFILE, fillucache, sizeof(struct userec));
    uidshm->number = usernumber;
    uidshm->uptime = ftime;
    return 0;
}
