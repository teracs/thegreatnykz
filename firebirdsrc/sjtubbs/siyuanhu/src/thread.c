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
$Id: thread.c,v 1.4 2004/07/24 18:03:01 rwzmm Exp $
*/

#include "bbs.h"

#include "record.c"

char tname[STRLEN];
char fname[STRLEN];

struct postnode
{
    int num;
    struct postnode *next;
};

//struct titlenode
//{
//    char *title;
//    struct titlenode *next;
//    struct postnode *post;
//};

struct reidnode
{
	unsigned int reid;
	struct reidnode *next;
	struct postnode *post;
};

struct reidnode topreid;

int
report ()
{}

int
thread (post, num)
struct fileheader *post;
int num;
{
    struct reidnode *tmp;
//    char *ntitle;
	unsigned int nreid;

    tmp = &topreid;

	nreid = post->reid;
//    if (post->title[0] == 'R' && post->title[1] == 'e' && post->title[2] == ':')
//        ntitle = post->title + 4;
//    else
//        ntitle = post->title;
	
    while (1)
    {
        if (tmp->next == NULL)
        {
            struct reidnode *reidtmp;
            reidtmp = (struct reidnode *) malloc (sizeof (struct reidnode));
            reidtmp->reid = malloc (sizeof (unsigned int));
 //           strcpy (titletmp->title, ntitle);
			reidtmp->reid = nreid;
            reidtmp->post = NULL;
            reidtmp->next = NULL;
            tmp->next = reidtmp;
        }
        if (/*!strcmp (tmp->next->title, ntitle)*/tmp->next->reid == nreid)
        {
            struct postnode *tmppost, *first = tmp->next->post;

            if (first == NULL)
            {
                tmppost = (struct postnode *) malloc (sizeof (struct postnode));
                tmppost->num = num;
                tmppost->next = NULL;
                tmp->next->post = tmppost;
                return;
            }
            while (1)
            {
                if (first->next == NULL)
                {
                    tmppost = (struct postnode *)
                              malloc (sizeof (struct postnode));
                    tmppost->num = num;
                    tmppost->next = NULL;
                    first->next = tmppost;
                    return;
                }
                first = first->next;
            }
        }
        else
        {
            tmp = tmp->next;
        }
    }
}

int
visit_all ()
{
    struct reidnode *tmp;
    struct fileheader post;
    int i = 0;

    tmp = topreid.next;

    while (tmp)
    {
        struct postnode *tmppost;

        i++;
        tmppost = tmp->post;
        while (tmppost)
        {
            get_record (fname, &post, sizeof (post), tmppost->num);
            append_record (tname, &post, sizeof (post));
            tmppost = tmppost->next;
        }
        tmp = tmp->next;
    }

}

int
main (argc, argv)
char *argv[];
int argc;
{
    FILE *tf;
    int i = 0;
    struct fileheader post;
    char dname[STRLEN];
    char buf[256];
    struct stat st1, st2;

    sprintf (dname, "boards/%s/%s", argv[1], DOT_DIR);
    sprintf (fname, "boards/%s/%s2", argv[1], DOT_DIR);
    sprintf (tname, "boards/%s/%s", argv[1], THREAD_DIR);

    if (stat (dname, &st1) == -1)
        return;
    if (stat (tname, &st2) != -1)
    {
        if (st2.st_mtime >= st1.st_mtime)
            return;
    }
    unlink (tname);
    sprintf (buf, "cp %s %s", dname, fname);
    system (buf);

    if ((tf = fopen (fname, "rb")) == NULL)
    {
        printf (".DIR cant open...");
        return;
    }
    topreid.next = NULL;
    topreid.post = NULL;
    while (1)
    {
        i++;
        if (fread (&post, sizeof (post), 1, tf) <= 0)
            break;
        thread (&post, i);
    }
    visit_all ();
    fclose (tf);
    unlink (fname);
}
