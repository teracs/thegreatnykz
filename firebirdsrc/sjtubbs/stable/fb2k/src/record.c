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

#define BUFSIZE (8192)

#ifdef SYSV
int flock(fd, op)
int fd, op;
{
    switch (op)
    {
    case LOCK_EX:
        return lockf(fd, F_LOCK, 0);
    case LOCK_UN:
        return lockf(fd, F_ULOCK, 0);
    default:
        return -1;
    }
}
#endif

int safewrite(int fd, void *buf, int size)
{
    int cc, sz = size, origsz = size;
    char *bp = (char *) buf;

    do
    {
        cc = write(fd, bp, sz);
        if ((cc < 0) && (errno != EINTR))
        {
            report("safewrite err!");
            return -1;
        }
        if (cc > 0)
        {
            bp += cc;
            sz -= cc;
        }
    }
    while (sz > 0);
    return origsz;
}


long get_num_records(char *filename, int size)
{
    char *s, buf[64];
    int num;
    struct stat st;

    if (stat(filename, &st) == -1)
        return 0;

    num = st.st_size;
    strcpy(buf, filename);
    s = strrchr(buf, '/') + 1;
    strcpy(s, TOPFILE_DIR);
    if (stat(buf, &st) == -1)
        return num / size;
    else
        return (num + st.st_size) / size;
}

int get_topfile_num(char *filename, int size)
{
    char *s, buf[64];
    struct stat st;

    strcpy(buf, filename);
    s = strrchr(buf, '/') + 1;
    strcpy(s, TOPFILE_DIR);
    if (stat(buf, &st) == -1)
        return 0;
    else
        return st.st_size / size;
}
int get_dir_num(char *filename, int size)
{
    char *s, buf[64];
    struct stat st;

    strcpy(buf, filename);
    s = strrchr(buf, '/') + 1;
    strcpy(s, DOT_DIR);
    if (stat(buf, &st) == -1)
        return 0;
    else
        return st.st_size / size;
}

int get_return_dir_num(char *filename, int size)
{
    char *s, buf[64];
    struct stat st;

    strcpy(buf, filename);
    s = strrchr(buf, '/') + 1;
    strcpy(s, ".RETURNDIR");
    if (stat(buf, &st) == -1)
        return 0;
    else
        return st.st_size / size;
}

int append_record(char *filename, void *record, int size)
{

    int fd;

    if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
    {
        report("open file error in append_record()");
        return -1;
    }
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    if (safewrite(fd, record, size) == -1)
        report("apprec write err!");
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int append_ip(char *filename, char *ip)
{
    int fd;
    struct voteip tmp;
    int size = sizeof(struct voteip);

    if ((fd = open(filename, O_RDWR | O_CREAT, 0644)) == -1)
    {
        report("open file error in append_ip()");
        return -1;
    }
    while (read(fd, &tmp, size) == size)
    {
        if (!strcmp(ip, tmp.ip))
        {
            flock(fd, LOCK_EX);
            lseek(fd, -sizeof(unsigned int), SEEK_CUR);
            tmp.ipnum++;
            write(fd, &tmp.ipnum, sizeof(unsigned int));
            flock(fd, LOCK_UN);
            close(fd);
            return 0;
        }
    }
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    sprintf(tmp.ip, "%s", ip);
    tmp.ipnum = 1;
    write(fd, &tmp, size);
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int append_mail(char *filename, char *email)
{
    return append_ip(filename, email);
}
void toobigmesg(void)
{
    report("record size too big!!\n");
}

int apply_record(char *filename, int (*fptr) (void *), int size)
{
    char abuf[BUFSIZE];
    int fd;

    if (size > BUFSIZE)
    {
        toobigmesg();
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

int search_ip(char *filename, char *userip)
{
    int fd;

    int size = sizeof(struct voteip);
    struct voteip tmp;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return 0;
    while (read(fd, &tmp, size) == size)
    {
        if (!strcmp(userip, tmp.ip))
        {
            close(fd);
            return tmp.ipnum;
        }
    }
    close(fd);
    return 0;
}

int search_mail(char *filename, char *email)
{
    return search_ip(filename, email);
}

int get_real_mail( char firstchar, char *userid, char *real_mail)
{

    FILE *stream;
    char bm_file_path[36], buffer[51];

    sprintf(bm_file_path, "home/%c/%c/%s/real_email", firstchar,
            toupper(userid[1]), userid);

    if ((stream = fopen(bm_file_path, "r")) == NULL){
        return -1;
    }
    else
    {
        while (fgets(buffer, 32, stream))
        {
            sprintf(real_mail, "%s", buffer);
        }
        fclose(stream);
    }
    return 0;
}

int
search_record(char *filename, void *rptr, int size,
              int (*fptr) (char *, void *), char *farg)
{
    int fd;
    int id = 1;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return 0;
    while (read(fd, rptr, size) == size)
    {
        if ((*fptr) (farg, rptr))
        {
            close(fd);
            return id;
        }
        id++;
    }
    close(fd);
    return 0;
}

int get_record(char *filename, void *rptr, int size, int id)
{
    int fd;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return -1;
    if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        close(fd);
        return -1;
    }
    if (read(fd, rptr, size) != size)
    {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int get_records(char *filename, void *vrptr, int size, int id, int number)
{
    int fd;
    int n;
    char *rptr = (char *) vrptr;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return -1;
    if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        close(fd);
        return 0;
    }
    if ((n = read(fd, rptr, size * number)) == -1)
    {
        close(fd);
        return -1;
    }
    close(fd);

    if (n < number * size)
    {
        char *s, buf[64];
        struct stat st;

        strcpy(buf, filename);
        s = strrchr(buf, '/') + 1;
        strcpy(s, TOPFILE_DIR);
        if ((stat(buf, &st) != -1) && st.st_size > 0)
        {
            fd = open(buf, O_RDONLY, 0);        //ûÓ×´í¼?            
            while (read(fd, &rptr[n], size))
            {
                n += size;
                if (n >= number * size)
                    break;
            }
            close(fd);
        }
    }
    return (n / size);
}

int substitute_record(char *filename, void *rptr, int size, int id)
{
    int fd;

    struct stat st;

    if (stat(filename, &st) == -1 || st.st_size / size < id)
        return -1;

    if (id < 1)
    {
        report("substitue_record(...) id <= 0");
        return -1;
    }
    if (id > 120000)
    {                           // if id too big, maybe error!
        report("substitue_record(...) id > 120000");
        return -1;
    }
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);
    if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        report("subrec seek err");
    }
    else
    {
        if (safewrite(fd, rptr, size) != size)
            report("subrec write err");
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

void tmpfilename(char *filename, char *tmpfile, char *deleted)
{
    char *ptr, *delfname, *tmpfname;

    strcpy(tmpfile, filename);
    delfname = ".deleted";
    tmpfname = ".tmpfile";
    if ((ptr = strrchr(tmpfile, '/')) != NULL)
    {
        strcpy(ptr + 1, delfname);
        strcpy(deleted, tmpfile);
        strcpy(ptr + 1, tmpfname);
    }
    else
    {
        strcpy(deleted, delfname);
        strcpy(tmpfile, tmpfname);
    }
}

int delete_record(char *filename, int size, int id)
{
    char tmpfile[STRLEN], deleted[STRLEN];
    char abuf[BUFSIZE];
    int fdr, fdw;
    int count;

    if (size > BUFSIZE)
    {
        toobigmesg();
        return -1;
    }

    tmpfilename(filename, tmpfile, deleted);
    if ((fdr = open(filename, O_RDONLY, 0)) == -1)
    {
        report("delrec open err");
        return -1;
    }
    if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        report("delrec tmp err");
        close(fdr);
        return -1;
    }
    count = 1;
    while (read(fdr, abuf, size) == size)
        if (id != count++ && (safewrite(fdw, abuf, size) == -1))
        {
            unlink(tmpfile);
            close(fdr);
            close(fdw);
            report("delrec write err");
            return -1;
        }
    close(fdr);
    close(fdw);
    if (rename(filename, deleted) == -1 || rename(tmpfile, filename) == -1)
    {
        report("delrec rename err");
        return -1;
    }
    return 0;
}

int delete_range(char *filename, int id1, int id2, int *prealdel)
{
    struct fileheader fhdr;
    char tmpfile[STRLEN], deleted[STRLEN];
    int fdr, fdw, fd, count;

    (*prealdel) = 0;
    tmpfilename(filename, tmpfile, deleted);
    if ((fd = open(".dellock", O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);

    if ((fdr = open(filename, O_RDONLY, 0)) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        close(fdr);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    count = 1;
    while (read(fdr, &fhdr, sizeof(fhdr)) == sizeof(fhdr))
    {
        if (count < id1 || count > id2
            || fhdr.accessed[0] & (FILE_MARKED | FILE_DIGEST | FILE_TOP1))
        {
            if ((safewrite(fdw, (char *) &fhdr, sizeof(fhdr)) == -1))
            {
                close(fdw);
                unlink(tmpfile);
                close(fdr);
                flock(fd, LOCK_UN);
                close(fd);
                return -1;
            }
        }
        else
        {
            char *t;
            char buf[STRLEN], fullpath[STRLEN];

            //add by rwzmm @ sjtubbs
            (*prealdel) += 1;
            strcpy(buf, filename);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(fullpath, "%s/%s", buf, fhdr.filename);
#ifdef BACKUP_RANGE_DELETE
            if (digestmode == 0 && uinfo.mode == READING)
            {
		int i;
                fhdr.accessed[0] = 0;
                setbfile(buf, currboard, RETURN_DIR);
                sprintf(genbuf, "%-32.32s - %s", fhdr.title,
                        currentuser.userid);
		for (i = strlen(genbuf); i < 60; ++i)
		    genbuf[i] = ' ';
		genbuf[i] = '\0';
		if (strlen(fhdr.title) > 32)
		    strcat(genbuf, &fhdr.title[32]);
                mystrlcpy(fhdr.title, genbuf, STRLEN);
                append_record(buf, &fhdr, sizeof(fhdr));
            }
            else
            {
#ifdef BACK_D_TO_RANGEDEL
                if (!NotBackupBoard())
                {
                    cancelpost(currboard, currentuser.userid,
                               &fhdr, BACK_D_TO_RANGEDEL);
                }
#endif
                unlink(fullpath);
            }
#else
            unlink(fullpath);
#endif

        }
        count++;
    }
    close(fdw);
    close(fdr);
    if (rename(filename, deleted) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if (rename(tmpfile, filename) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int update_file(char *dirname, int size, int ent,
                int (*filecheck) (void *), void (*fileupdate) (char *))
{
    char abuf[BUFSIZE];
    int fd;

    if (size > BUFSIZE)
    {
        toobigmesg();
        return -1;
    }

    if ((fd = open(dirname, O_RDWR)) == -1)
        return -1;

    flock(fd, LOCK_EX);
    if (lseek(fd, (off_t) (size * (ent - 1)), SEEK_SET) != -1)
    {
        if (read(fd, abuf, size) == size)
        {
            if ((*filecheck) (abuf))
            {
                lseek(fd, (off_t) (-size), SEEK_CUR);
                (*fileupdate) (abuf);
                if (safewrite(fd, abuf, size) != size)
                {
                    report("update err");
                    flock(fd, LOCK_UN);
                    close(fd);
                    return -1;
                }
                flock(fd, LOCK_UN);
                close(fd);
                return 0;
            }
        }
    }
    lseek(fd, 0, SEEK_SET);
    while (read(fd, abuf, size) == size)
    {
        if ((*filecheck) (abuf))
        {
            lseek(fd, (off_t) (-size), SEEK_CUR);
            (*fileupdate) (abuf);
            if (safewrite(fd, abuf, size) != size)
            {
                report("update err");
                flock(fd, LOCK_UN);
                close(fd);
                return -1;
            }
            flock(fd, LOCK_UN);
            close(fd);
            return 0;
        }
    }
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
}

int delete_file(char *dirname, int size, int ent,
                int (*filecheck) (void *))
{
    char abuf[BUFSIZE];
    int fd;
    struct stat st;
    long numents;

    if (size > BUFSIZE)
    {
        toobigmesg();
        return -1;
    }

    if ((fd = open(dirname, O_RDWR)) == -1)
        return -1;

    flock(fd, LOCK_EX);
    fstat(fd, &st);
    numents = ((long) st.st_size) / size;

    if (((long) st.st_size) % size != 0)
        report("align err\n");

    if (lseek(fd, (off_t) (size * (ent - 1)), SEEK_SET) != -1)
    {
        if (read(fd, abuf, size) == size)
        {
            if ((*filecheck) (abuf))
            {
                int i;

                for (i = ent; i < numents; i++)
                {
                    if (lseek(fd, (off_t) (i * size), SEEK_SET) == -1)
                        break;
                    if (read(fd, abuf, size) != size)
                        break;
                    if (lseek(fd, (off_t) ((i - 1) * size), SEEK_SET) ==
                        -1)
                        break;
                    if (safewrite(fd, abuf, size) != size)
                        break;
                }
                ftruncate(fd, (off_t) size * (numents - 1));
                flock(fd, LOCK_UN);
                close(fd);
                return 0;
            }
        }
    }
    lseek(fd, 0, SEEK_SET);
    ent = 0;
    while (read(fd, abuf, size) == size)
    {
        if ((*filecheck) (abuf))
        {
            int i;

            for (i = ent; i < numents; i++)
            {
                if (lseek(fd, (off_t) ((i + 1) * size), SEEK_SET) == -1)
                    break;
                if (read(fd, abuf, size) != size)
                    break;
                if (lseek(fd, (off_t) (i * size), SEEK_SET) == -1)
                    break;
                if (safewrite(fd, abuf, size) != size)
                    break;
            }
            ftruncate(fd, (off_t) size * (numents - 1));
            flock(fd, LOCK_UN);
            close(fd);
            return 0;
        }
        ent++;
    }
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
}

int move_file(char *dirname, int size, int src, int dst, int (*filecheck) (void *))
{
    char sbuf[BUFSIZE];
    int fd;
    struct stat st;
    long numents;
    int found = NA, error = NA;

    if (src == dst)
        return 0;
    if (size > BUFSIZE)
    {
        toobigmesg();
        return -1;
    }
    if ((fd = open(dirname, O_RDWR)) == -1)
        return -1;

    flock(fd, LOCK_EX);
    fstat(fd, &st);
    numents = (long)st.st_size / size;
    if ((long)st.st_size % size != 0)
        report("align err\n");
    if (dst > numents)
        dst = numents;

    if (lseek(fd, (off_t)(size * (src - 1)), SEEK_SET) != -1
            && read(fd, sbuf, size) == size && (*filecheck)(sbuf))
        found = YEA;
    else
    {
        error = lseek(fd, 0, SEEK_SET) == -1;
        for (src = 1; !error && !(error = read(fd, sbuf, size) != size); src++)
            if ((*filecheck)(sbuf))
            {
                found = YEA;
                break;
            }
    }

    if (!error && found && src != dst)
    {
        int i, d;
        char tbuf[BUFSIZE];

        d = src < dst ? 1 : -1;
        for (i = src; i != dst; i += d)
        {
            if (error = lseek(fd, (off_t)((i - 1 + d) * size), SEEK_SET) == -1)
                break;
            if (error = read(fd, tbuf, size) != size)
                break;
            if (error = lseek(fd, (off_t)((i - 1) * size), SEEK_SET) == -1)
                break;
            if (error = safewrite(fd, tbuf, size) != size)
                break;
        }
        if (!error)
            error = lseek(fd, (off_t)((dst - 1) * size), SEEK_SET) == -1;
        if (!error)
            error = safewrite(fd, sbuf, size) != size;
    }
    flock(fd, LOCK_UN);
    close(fd);
    return error ? -1 : 0;
}

