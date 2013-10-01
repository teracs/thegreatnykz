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

#include "sys/mman.h"
#include "bbs.h"

struct FILESHM
{
    char line[FILE_MAXLINE][FILE_BUFSIZE];
    int fileline;
    int max;
};

struct FILESHM *welcomeshm = NULL;
struct FILESHM *goodbyeshm = NULL;
struct FILESHM *issueshm = NULL;

int fill_shmfile(int mode, char *fname, char *shmkey)
{
    FILE *fffd;
    char *ptr;
    char buf[FILE_BUFSIZE];
    int lines = 0, nowfn = 0, maxnum;
    struct FILESHM *tmp;

    switch (mode)
    {
    case 1:
        maxnum = MAX_ISSUE;
        break;
    case 2:
        maxnum = MAX_GOODBYE;
        break;
    case 3:
        maxnum = MAX_WELCOME;
        break;
    }
    tmp =
        mmap(NULL, sizeof(struct FILESHM) * maxnum, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (tmp == MAP_FAILED)
    {
        return 0;
    }

    switch (mode)
    {
    case 1:
        issueshm = tmp;
        break;
    case 2:
        goodbyeshm = tmp;
        break;
    case 3:
        welcomeshm = tmp;
        break;
    }

    if ((fffd = fopen(fname, "r")) == NULL)
    {
        return 0;
    }
    while ((fgets(buf, FILE_BUFSIZE, fffd) != NULL) && nowfn < maxnum)
    {
        if (lines > FILE_MAXLINE)
            continue;
        if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
        {
            tmp[nowfn].fileline = lines;
            nowfn++;
            lines = 0;
            continue;
        }
        ptr = tmp[nowfn].line[lines];
        memcpy(ptr, buf, sizeof(buf));
        lines++;
    }
    fclose(fffd);
    tmp[nowfn].fileline = lines;
    nowfn++;
    tmp[0].max = nowfn;
    return 1;
}

void show_shmfile(struct FILESHM *fh)
{
    int i;
    char buf[FILE_BUFSIZE];

    for (i = 0; i < fh->fileline; i++)
    {
        strcpy(buf, fh->line[i]);
        showstuff(buf);
    }
}

void show_goodbyeshm(void)
{
    int logouts;

    logouts = goodbyeshm[0].max;
    clear();
    show_shmfile(&goodbyeshm
                 [(currentuser.numlogins %
                   ((logouts <= 1) ? 1 : logouts))]);
    munmap(goodbyeshm, sizeof(struct FILESHM) * MAX_GOODBYE);
    goodbyeshm = NULL;
}

void show_welcomeshm(void)
{
    int welcomes;

    welcomes = welcomeshm[0].max;
    clear();
    show_shmfile(&welcomeshm
                 [(currentuser.numlogins %
                   ((welcomes <= 1) ? 1 : welcomes))]);

    pressanykey();
    munmap(welcomeshm, sizeof(struct FILESHM) * MAX_WELCOME);
    welcomeshm = NULL;
}

void show_issue(void)
{
    int issues = issueshm[0].max;

    show_shmfile(&issueshm[(issues <= 1) ? 0 :
                           ((time(0) / 86400) % (issues))]);
    munmap(issueshm, sizeof(struct FILESHM) * MAX_ISSUE);
    issueshm = NULL;
}
