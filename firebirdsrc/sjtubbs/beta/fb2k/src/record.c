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
$Id: record.c,v 1.4 2004/07/08 15:58:44 rwzmm Exp $
*/

#include "bbs.h"

#define BUFSIZE (8192)

#ifdef SYSV
int
flock(fd, op)
int     fd, op;
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
    int     cc, sz = size, origsz = size;
    char   *bp = (char*)buf;

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


long
get_num_records(char *filename, int size)
{
    char *s,buf[64];
    int num;
    struct stat st;
    if (stat(filename, &st) == -1)
        return 0;

    /*changed by cf*/
    num=st.st_size;
    strcpy(buf,filename);
    s=strrchr(buf,'/')+1;
    strcpy(s,TOPFILE_DIR);
    //  if(digestmode>1)
    //      return num/size;
    if(stat(buf,&st)==-1)
        return num/size;
    else
        return (num+st.st_size)/size;
    /*return (st.st_size / size);*/

    /*end*/
}
/*added by jawahh*/
/*return the number of topfiles in a board*/
int
get_topfile_num(char *filename, int size)
{
    char *s,buf[64];
    struct stat st;
    strcpy(buf,filename);
    s=strrchr(buf,'/')+1;
    strcpy(s,TOPFILE_DIR);
    if(stat(buf,&st)==-1)
        return 0;
    else
        return st.st_size/size;
}
int
get_dir_num(char *filename, int size)
{
    char *s,buf[64];
    struct stat st;
    strcpy(buf,filename);
    s=strrchr(buf,'/')+1;
	//modified by rwzmm @ sjtubbs
    strcpy(s, DOT_DIR);
    if(stat(buf,&st)==-1)
        return 0;
    else
        return st.st_size/size;
}

int
get_return_dir_num(char *filename, int size)
{
    char *s,buf[64];
    struct stat st;
    strcpy(buf,filename);
    s=strrchr(buf,'/')+1;
	//modified by rwzmm @ sjtubbs
    strcpy(s, ".RETURNDIR");
    if(stat(buf,&st)==-1)
        return 0;
    else
        return st.st_size/size;
}


/*end*/
int
append_record(char *filename, void *record, int size)
{

    int     fd;
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

//added by kongyang 2005-5-23
int
append_ip(char *filename, char *ip)
{
	int fd;
	struct voteip tmp;
	int size=sizeof(struct voteip);
	if((fd=open(filename,O_RDWR|O_CREAT,0644))==-1)
	{
		report("open file error in append_ip()");
		return -1;
	}
	while(read(fd,&tmp,size)==size)
	{
		if(!strcmp(ip,tmp.ip))
		{
			flock(fd, LOCK_EX);
			lseek(fd,-sizeof(unsigned int),SEEK_CUR);
			tmp.ipnum++;
			write(fd,&tmp.ipnum,sizeof(unsigned int));
			flock(fd, LOCK_UN);
			close(fd);
			return 0;
		}
	}
	flock(fd, LOCK_EX);
	lseek(fd,0,SEEK_END);
	sprintf(tmp.ip,"%s",ip);
	tmp.ipnum=1;
	write(fd,&tmp,size);
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

void
toobigmesg(void)
{
    report("record size too big!!\n");
}


int
apply_record(char *filename, int (*fptr) (void *), int size)
{
    char    abuf[BUFSIZE];
    int     fd;

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

//added by kongyang 2005-5-24 uses to search a file for how many times an ip has voted
int 
search_ip(char *filename, char *userip)
{
	int fd;
	/*int times=0;*/
	int size=sizeof(struct voteip);
	struct voteip tmp;
	if((fd=open(filename,O_RDONLY,0))==-1)
		return 0;
	while(read(fd,&tmp,size)==size)
	{
		if(!strcmp(userip,tmp.ip))
		{
			close(fd);
			return tmp.ipnum;
		}
	}
	close(fd);
	return 0;
		
	
}

int
search_record(char *filename, void *rptr, int size, int (*fptr) (char *, void*), char *farg)
{
    int     fd;
    int     id = 1;
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

int
get_record(char *filename, void *rptr, int size, int id)
{
    int     fd;
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

int
get_records(char *filename, void *vrptr, int size, int id, int number)
{
    int     fd;
    int     n;
	char *rptr = (char*)vrptr;
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
    //  if(digestmode>1)
    //	return (n / size);
    //added by cf
/* FIXME: what the hell ..., read .TOPDIR? */
    if(n<number*size)
    {
        char *s,buf[64];
        struct stat st;
        strcpy(buf,filename);
        s=strrchr(buf,'/')+1;
        strcpy(s,TOPFILE_DIR);
        if((stat(buf,&st)!=-1) && st.st_size>0)
        {
            fd=open(buf,O_RDONLY,0); //ûÓ×´í¼?            
			while(read(fd,&rptr[n],size))
            {
                n+=size;
                if(n>=number*size)
                    break;
            }
            close(fd);
        }
    }
    //end
    return (n / size);
}

int substitute_record(char *filename, void *rptr, int size, int id)
{
    int     fd;
    //add by cf
    struct stat st;
    if(stat(filename,&st)==-1 || st.st_size/size <id )
        return -1;
    //end

    if(id < 1)
    {
        report("substitue_record(...) id <= 0");
        return -1;
    }
    if(id > 120000)
    { // if id too big, maybe error!
        report("substitue_record(...) id > 120000");
        return -1;
    }
    if((fd=open(filename,O_WRONLY|O_CREAT,0644))==-1)
        return -1;
    flock(fd, LOCK_EX);
    if(lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
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
    char   *ptr, *delfname, *tmpfname;

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

int
delete_record(char *filename, int size, int id)
{
    char    tmpfile[STRLEN], deleted[STRLEN];
    char    abuf[BUFSIZE];
    int     fdr, fdw;
    int     count;

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
    if (rename(filename, deleted) == -1 ||
            rename(tmpfile, filename) == -1)
    {
        report("delrec rename err");
        return -1;
    }
    	return 0;
}

int delete_range(char *filename, int id1, int id2, int *prealdel)
{
    struct fileheader fhdr;
    char    tmpfile[STRLEN], deleted[STRLEN];
    int     fdr, fdw, fd, count;
    //char    genbuf[STRLEN];
	
	(*prealdel) = 0;
    tmpfilename(filename, tmpfile, deleted);
    if((fd=open(".dellock",O_RDWR|O_CREAT|O_APPEND, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);

    if ((fdr = open(filename, O_RDONLY, 0)) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    if((fdw=open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        close(fdr);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    count = 1;
    while(read(fdr, &fhdr, sizeof(fhdr))==sizeof(fhdr))
    {
        /*changed by cf*/
        /*     if(count<id1||count>id2||fhdr.accessed[0] & (FILE_MARKED|FILE_DIGEST)){*/
        if (count < id1 || count > id2 || fhdr.accessed[0] & (FILE_MARKED | FILE_DIGEST | FILE_TOP1))
        {
            /*end*/
            if ((safewrite(fdw,(char *)&fhdr,sizeof(fhdr))==-1))
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
            char   *t;
            char    buf[STRLEN], fullpath[STRLEN];
			//add by rwzmm @ sjtubbs
			(*prealdel) += 1;
            strcpy(buf, filename);
            if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
            sprintf(fullpath, "%s/%s", buf, fhdr.filename);
#ifdef BACKUP_RANGE_DELETE
            //{ »ØÊÕÕ¾, add by mudboy
            if (digestmode==0 && uinfo.mode==READING)
            {
                fhdr.accessed[0] = 0;
                setbfile(buf,currboard,RETURN_DIR);
                sprintf(genbuf, "%-32.32s - %s", fhdr.title, currentuser.userid);
                strncpy(fhdr.title, genbuf, STRLEN);
                append_record(buf, &fhdr, sizeof(fhdr));
            }
            else
            {
#ifdef BACK_D_TO_RANGEDEL     //added by aegis @011028
                if(!NotBackupBoard())
                {
                    cancelpost(currboard, currentuser.userid,
                               &fhdr, BACK_D_TO_RANGEDEL);
                }
#endif
                unlink(fullpath);
            }
            //}
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

int update_file(char *dirname, int size, int ent, int (*filecheck) (void*), void (*fileupdate) (char*))
{
    char    abuf[BUFSIZE];
    int     fd;

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

int delete_file(char *dirname, int size, int ent, int (*filecheck) (void*))
{
    char    abuf[BUFSIZE];
    int     fd;
    struct stat st;
    long    numents;

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
                int     i;
                for (i = ent; i < numents; i++)
                {
                    if (lseek(fd, (off_t) (i * size), SEEK_SET) == -1)
                        break;
                    if (read(fd, abuf, size) != size)
                        break;
                    if (lseek(fd, (off_t) ((i - 1) * size), SEEK_SET) == -1)
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
            int     i;
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
