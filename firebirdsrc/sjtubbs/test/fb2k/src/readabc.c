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

#include <string.h>
#include "bbs.h"

extern char currboard[STRLEN];
extern char someoneID[31];

int marked_all(int type)
{
    struct fileheader post;
    int fd;
    int key, nkey, i, flag;
    char tname[128], fname[128], dname[128], buf[256];
    char tempname1[STRLEN], tempname2[STRLEN];
    char keywords[5][STRLEN];
    char nkeywords[5][STRLEN];
    char *cppos, *cpaux, *temp, *tail, next;
    struct stat st1, st2;

    sprintf(dname, "boards/%s/%s", currboard, DOT_DIR);
    sprintf(fname, "boards/%s/%s2", currboard, DOT_DIR);
    switch (type)
    {
    case 0:
        sprintf(tname, "boards/%s/%s", currboard, MARKED_DIR);
        break;
    case 1:
        sprintf(tname, "boards/%s/%s", currboard, AUTHOR_DIR);
        break;
    case 2:
    case 3:
        sprintf(tname, "boards/%s/SOMEONE.%s.DIR.%d", currboard, someoneID,
                type - 2);
        break;
    case 4:
        sprintf(tname, "boards/%s/KEY.%s.DIR", currboard,
                currentuser.userid);
        break;
    }

    if (stat(dname, &st1) == -1)
        return 1;
    if ((type != 4) && (stat(tname, &st2) != -1) && st2.st_mtime >= st1.st_mtime)
        return 1;
    unlink(tname);
    sprintf(buf, "cp %s %s", dname, fname);
    system(buf);

    if ((fd = open(fname, O_RDONLY, 0)) == -1)
        return -1;

    if (type == 4 || type == 2)
    {
        strtolower(tempname2, someoneID);
        tempname2[sizeof(tempname2) - 1] = '\0';
        key = nkey = 0;
        if (tempname2[0] == '+' || tempname2[0] == '-')
        {
            temp = &tempname2[0];
            while (*temp)
            {
                if (*temp == '+')
                {
                    tail = temp + 1;
                    while (*tail && *tail != '+' && *tail != '-')
                        tail++;
                    next = *tail;
                    *tail = 0;
                    if (key < 5)
                    {
                        sprintf(keywords[key], "%s", temp + 1);
                        if (strlen(keywords[key]))
                           key++;
                    }    
                    *tail = next;
                    temp = tail;
                }    
                else if (*temp == '-')
                {
                    tail = temp + 1;
                    while (*tail && *tail != '+' && *tail != '-')
                        tail++;
                    next = *tail;
                    *tail = 0;
                    if (nkey < 5)
                    {
                        sprintf(nkeywords[nkey], "%s", temp + 1);
                        if (strlen(nkeywords[nkey]))
                           nkey++;
                    }    
                    *tail = next;
                    temp = tail;
                }    
                else
                    temp++;
            }    
        }    
        else
        {
            sprintf(keywords[key], "%s", tempname2);
            key++;
        }    
    }    
    while (read(fd, &post, sizeof(struct fileheader)) ==
           sizeof(struct fileheader))
    {
        switch (type)
        {
        case 0:
            if (post.accessed[0] & FILE_MARKED)
                append_record(tname, &post, sizeof(post));
            break;
        case 1:
            if (strncmp(post.title, "Re: ", 4))
            {
                append_record(tname, &post, sizeof(post));
            }
            break;
        case 2:
        case 4:
            if (type == 2)
                strtolower(tempname1, post.owner);
            else
                strtolower(tempname1, post.title);
            tempname1[sizeof(tempname1) - 1] = '\0';
            for (i = 0; i < key + nkey; i++)
            {
                if (i < key)
                {
                    cppos = strstr(tempname1, keywords[i]);
                    flag = 0;
                }    
                else
                {
                    cppos = strstr(tempname1, nkeywords[i - key]);
                    flag = 1;
                }    
                cpaux = tempname1;
                while (cppos != NULL)
                {
                    while (cpaux < cppos)
                        cpaux += ((*cpaux & 0x80) ? 2 : 1);
                    if (cpaux == cppos)
                    {
                        flag = 1 - flag;
                        cppos = NULL;
                        break;
                    }
                    else
                        cppos = strstr(cpaux, tempname2);
                }
                if (!flag)
                   break;
            }    
            if (i >= key + nkey)
                append_record(tname, &post, sizeof(post));
            break;
        case 3:
            if (!strcasecmp(post.owner, someoneID))
                append_record(tname, &post, sizeof(post));
            break;
        }
    }
    close(fd);
    unlink(fname);
    return 0;
}

int marked_mail()
{
    struct fileheader post;
    int fd;
    char tname[128], fname[128], dname[128], buf[256];
    struct stat st1, st2;

    sprintf(dname, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,DOT_DIR);
    sprintf(fname, "mail/%c/%c/%s/%s2", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,DOT_DIR);
    sprintf(tname, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, MARKED_DIR);

    if (stat(dname, &st1) == -1)
        return 1;
    if (stat(tname, &st2) != -1)
    {
        if (st2.st_mtime >= st1.st_mtime)
            return 1;
    }
    unlink(tname);
    sprintf(buf, "cp %s %s", dname, fname);
    system(buf);

    if ((fd = open(fname, O_RDONLY, 0)) == -1)
        return -1;

    while (read(fd, &post, sizeof(struct fileheader)) ==
           sizeof(struct fileheader))
            if (post.accessed[0] & FILE_MARKED)
                append_record(tname, &post, sizeof(post));
            
    close(fd);
    unlink(fname);
    return 0;
}
int return_search_all(int type)
{
    struct fileheader post;
    int fd;
    char tname[128], fname[128], dname[128], buf[256];
    char tempname1[STRLEN], tempname2[STRLEN];
    struct stat st1, st2;

    sprintf(dname, "boards/%s/%s", currboard, RETURN_DIR);
    sprintf(fname, "boards/%s/%s2", currboard, RETURN_DIR);
    switch (type)
    {
    case 1:
        sprintf(tname, "boards/%s/SOMEONE.%s.RETURN_DIR.1", currboard,
                someoneID);
        break;
    case 2:
        sprintf(tname, "boards/%s/SOMEONE.%s.RETURN_DIR.2", currboard,
                someoneID);
        break;
    case 3:
        sprintf(tname, "boards/%s/KEY.%s.RETURN_DIR", currboard,
                currentuser.userid);
        break;
    }

    if (dashf(tname))
        unlink(tname);

    if (stat(dname, &st1) == -1)
        return 1;
    if (stat(tname, &st2) != -1)
    {
        if (st2.st_mtime >= st1.st_mtime)
            return 1;
    }
    unlink(tname);
    sprintf(buf, "cp %s %s", dname, fname);
    system(buf);

    if ((fd = open(fname, O_RDONLY, 0)) == -1)
        return -1;

    while (read(fd, &post, sizeof(struct fileheader)) ==
           sizeof(struct fileheader))
    {
        switch (type)
        {
        case 1:
        case 3:
            if (type == 1)
                strtolower(tempname1, post.owner);
            else
                strtolower(tempname1, post.title);
            strtolower(tempname2, someoneID);
            tempname1[sizeof(tempname1) - 1] = '\0';
            tempname2[sizeof(tempname2) - 1] = '\0';
            if (strstr(tempname1, tempname2))
                append_record(tname, &post, sizeof(post));
            break;
        case 2:
            if (!strcasecmp(post.owner, someoneID))
                append_record(tname, &post, sizeof(post));
            break;
        }
    }
    close(fd);
    unlink(fname);
    return 0;
}
