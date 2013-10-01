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

#include <unistd.h>
#include <sys/mman.h>

#ifdef _POSIX_MAPPED_FILES

void *mmap(void *start, size_t length, int prot, int
           flags, int fd, off_t offset);

int munmap(void *start, size_t length);

#endif

#include "bbs.h"

int compare(a, b)
struct fileheader *a, *b;
{
    return (atoi(a->filename + 2) - atoi(b->filename + 2));
}

int main(int argc, char **argv)
{
    int fd, size;
    char file[256];
    struct stat buf;
    char *mem;

    if (argc < 2)
    {
        printf("Usage: sortdir board.\n");
        exit(0);
    }
    size = sizeof(struct fileheader);
    sprintf(file, "%s/boards/%s/%s", BBSHOME, argv[1], DOT_DIR);
    fd = open(file, O_RDWR, 0644);
    if (fd == -1)
    {
        printf("Cannot open %s.\n", file);
        exit(0);
    }
    flock(fd, LOCK_EX);
    fstat(fd, &buf);
    mem = mmap(0, buf.st_size, PROT_READ | PROT_WRITE,
               MAP_FILE | MAP_SHARED, fd, 0);
    qsort(mem, buf.st_size / size, size, compare);
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}
