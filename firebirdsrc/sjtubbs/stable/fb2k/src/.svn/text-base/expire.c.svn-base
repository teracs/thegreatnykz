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

#include <stdlib.h>
#include <strings.h>
#include "bbs.h"

#define DEF_DAYS        60

#define EXPIRE_CONF     BBSHOME"/etc/expire.ctl"

char *bpath = BBSHOME "/boards";

void report(char *s)
{
}

#include "record.c"

struct life
{
    char bname[BDNAME_LEN];
    int days;
    int maxp;
    int minp;
};
typedef struct life life;

static char *getfolderstr(char *filename)
{
    static char str[4] = { '\0', '\0', '\0', '\0' };
    int len = strlen(filename);

    str[0] =
        filename[len - 5] >
        '4' ? filename[len - 5] - 5 : filename[len - 5];
    str[1] = filename[len - 4];
    str[2] = filename[len - 3];
    return str;
}

void expire(life * brd)
{
    struct fileheader head;
    struct stat state;
    char lockfile[1024], tmpfile[1024], bakfile[1024];
    char fpath[1024], index[1024], *fname;
    int total;
    int fd, fdr, fdw, done, keep;
    int duetime, ftime;

    printf("board %s; days %d\n", brd->bname, brd->days);

    sprintf(index, "%s/%s/.RETURNDIR", bpath, brd->bname);
    sprintf(lockfile, "%s.lock", index);
    if ((fd = open(lockfile, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
        return;
    flock(fd, LOCK_EX);

    strcpy(fpath, index);
    fname = (char *) strrchr(fpath, '.');

    duetime = time(NULL) - brd->days * 24 * 60 * 60;
    done = 0;
    if ((fdr = open(index, O_RDONLY, 0)) > 0)
    {
        fstat(fdr, &state);
        total = state.st_size / sizeof(head);
        sprintf(tmpfile, "%s.new", index);
        unlink(tmpfile);
        if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) > 0)
        {
            while (read(fdr, &head, sizeof head) == sizeof head)
            {
                done = 1;
                ftime = atoi(head.filename + 2);

                if (ftime < duetime)
                    keep = 0;
                else
                    keep = 1;

                if (keep)
                {
                    if (safewrite(fdw, (char *) &head, sizeof head) == -1)
                    {
                        done = 0;
                        break;
                    }
                }
                else
                {
                    sprintf(fname, "%s/%s", getfolderstr(head.filename),
                            head.filename);
                    unlink(fpath);
                    printf("\t%s\n", fname);
                    total--;
                }
            }
            close(fdw);
        }
        close(fdr);
    }
    if (done)
    {
        sprintf(bakfile, "%s.old", index);
        if (rename(index, bakfile) != -1)
            rename(tmpfile, index);
    }
    flock(fd, LOCK_UN);
    close(fd);
}

int main(int argc, char **argv)
{
    FILE *fin;
    int number;
    size_t count;
    life db, table[MAXBOARD], *key;
    struct dirent *de;
    DIR *dirp;
    char *ptr, *bname, buf[256];

    db.days = DEF_DAYS;

    count = 0;
    if ((fin = fopen(EXPIRE_CONF, "r")))
    {
        while (fgets(buf, 256, fin))
        {
            if (buf[0] == '#')
                continue;

            bname = (char *) strtok(buf, " \t\r\n");
            if (bname && *bname)
            {
                ptr = (char *) strtok(NULL, " \t\r\n");
                if (ptr && (number = atoi(ptr)) > 0)
                {
                    key = &(table[count++]);
                    strcpy(key->bname, bname);
                    key->days = number;
                    key->maxp = db.maxp;
                    key->minp = db.minp;

                    ptr = (char *) strtok(NULL, " \t\r\n");
                    if (ptr && (number = atoi(ptr)) > 0)
                    {
                        key->maxp = number;

                        ptr = (char *) strtok(NULL, " \t\r\n");
                        if (ptr && (number = atoi(ptr)) > 0)
                        {
                            key->minp = number;
                        }
                    }
                }
            }
        }
        fclose(fin);
    }
    if (count > 1)
    {
        qsort(table, count, sizeof(life), (const void *) strcasecmp);
    }

    if (!(dirp = opendir(bpath)))
    {
        printf(":Err: unable to open %s\n", bpath);
        return -1;
    }
    while ((de = readdir(dirp)))
    {
        ptr = de->d_name;
        if (ptr[0] > ' ' && ptr[0] != '.')
        {
            if (count)
                key =
                    (life *) bsearch(ptr, table, count, sizeof(life),
                                     (const void *) strcasecmp);
            else
                key = NULL;
            if (!key)
                key = &db;
            strncpy(key->bname, ptr, BDNAME_LEN - 1);
            key->bname[BDNAME_LEN - 1] = '\0';
            expire(key);
        }
    }
    closedir(dirp);
    return 0;
}
