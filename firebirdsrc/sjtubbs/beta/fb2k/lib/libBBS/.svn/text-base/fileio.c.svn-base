/*
 * fileio.c		-- some stuff that file/io related
 *
 * of SEEDNetBBS generation 1 (libtool implement)
 *
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: fileio.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
 */

#ifdef BBS
#include "bbs.h"
#else
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#endif

char fileio_c[] = "$Id: fileio.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $";

#define        BLK_SIZ         4096

static int rm_dir(char *fpath);

void file_append(char *fpath, char *msg)
{
    int fd;

    if ((fd = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
    {
        write(fd, msg, strlen(msg));
        close(fd);
    }
}

int dashf(char *fname)
{
    struct stat st;

    return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}

int dashd(char *fname)
{
    struct stat st;

    return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}

int part_cp(char *src, char *dst, char *mode)
{
    int flag = 0;
    char buf[256];
    FILE *fsrc, *fdst;

    fsrc = fopen(src, "r");
    if (fsrc == NULL)
        return 0;
    fdst = fopen(dst, mode);
    if (fdst == NULL)
        return 0;
    while (fgets(buf, 256, fsrc) != NULL)
    {
        if (flag == 1 && !strcmp(buf, "--\n"))
        {
            fputs(buf, fdst);
            break;
        }
        if (flag == 0 && (!strncmp(buf + 2, "ÐÅÈË: ", 6)
                          || !strncmp(buf, "[1;41;33m·¢ÐÅÈË: ", 18)))
        {
            fputs(buf, fdst);
            continue;
        }
        if (flag == 0
                && (buf[0] == '\0' || buf[0] == '\n'
                    || !strncmp(buf + 2, "ÐÅÈË: ", 6)
                    || !strncmp(buf, "±ê  Ìâ: ", 8)
                    || !strncmp(buf, "·¢ÐÅÕ¾: ", 8)
                    || !strncmp(buf, "[1;41;33m·¢ÐÅÈË: ", 18)))
            continue;
        flag = 1;
        fputs(buf, fdst);
    }
    fclose(fdst);
    fclose(fsrc);
    return 1;
}

/* mode == O_EXCL / O_APPEND / O_TRUNC */
int f_cp(char *src, char *dst, int mode)
{
    int fsrc, fdst, ret;

    ret = 0;
    if ((fsrc = open(src, O_RDONLY)) >= 0)
    {
        ret = -1;
        if ((fdst = open(dst, O_WRONLY | O_CREAT | mode, 0600)) >= 0)
        {
            char pool[BLK_SIZ];

            src = pool;
            do
            {
                ret = read(fsrc, src, BLK_SIZ);
                if (ret <= 0)
                    break;
            }
            while (write(fdst, src, ret) > 0);
            close(fdst);
        }
        close(fsrc);
    }
    return ret;
}

int valid_fname(char *str)
{
    char ch;

    while ((ch = *str++) != '\0')
    {
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                strchr("0123456789-_", ch) != 0)
        {
            ;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

int touchfile(char *filename)
{
    int fd;

    if ((fd = open(filename, O_RDWR | O_CREAT, 0600)) > 0)
        close(fd);

    return fd;
}

int f_rm(char *fpath)
{
    struct stat st;

    if (stat(fpath, &st))
        return -1;

    if (!S_ISDIR(st.st_mode))
        return unlink(fpath);

    return rm_dir(fpath);
}


static int rm_dir(char *fpath)
{
    struct stat st;
    DIR *dirp;
    struct dirent *de;
    char buf[1024];

    if (!(dirp = opendir(fpath)))
        return -1;


    while ((de = readdir(dirp)))
    {
		if( 0==strcmp(de->d_name,".") || 0==strcmp(de->d_name,".."))
			continue;
        if (de->d_name)
        {
			/* if buf is truncated, don't remove it */
			if(snprintf(buf,sizeof(buf),"%s/%s",fpath,de->d_name)>=(int)sizeof(buf))
				continue;
            if (!stat(buf, &st))
            {
                if (S_ISDIR(st.st_mode))
                    rm_dir(buf);
                else
                    unlink(buf);
            }
        }
    }
    closedir(dirp);

    return rmdir(fpath);
}
