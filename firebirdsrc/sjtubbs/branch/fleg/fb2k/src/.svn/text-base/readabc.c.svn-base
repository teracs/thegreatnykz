#include <string.h>
#include "bbs.h"

char tname[STRLEN];
char fname[STRLEN];
extern char currboard[STRLEN];
extern char someoneID[31];

int
marked_all (int type)
{
    struct fileheader post;
    int fd;
    char dname[STRLEN], buf[STRLEN];
    char tempname1[50], tempname2[50];
    struct stat st1, st2;

    sprintf (dname, "boards/%s/%s", currboard, DOT_DIR);
    sprintf (fname, "boards/%s/%s2", currboard, DOT_DIR);
    switch (type)
    {
    case 0:
        sprintf (tname, "boards/%s/%s", currboard, MARKED_DIR);
        break;
    case 1:
        sprintf (tname, "boards/%s/%s", currboard, AUTHOR_DIR);
        break;
    case 2:
    case 3:
        sprintf (tname, "boards/%s/SOMEONE.%s.DIR.%d", currboard, someoneID,
                 type - 2);
        break;
    case 4:
        sprintf (tname, "boards/%s/KEY.%s.DIR", currboard, currentuser.userid);
        break;
    }

    if (stat (dname, &st1) == -1)
        return 1;
    if (stat (tname, &st2) != -1)
    {
        if (st2.st_mtime >= st1.st_mtime)
            return 1;
    }
    unlink (tname);
    sprintf (buf, "cp %s %s", dname, fname);
    system (buf);

    if ((fd = open (fname, O_RDONLY, 0)) == -1)
        return -1;

    while (read (fd, &post, sizeof (struct fileheader)) ==
            sizeof (struct fileheader))
    {
        switch (type)
        {
        case 0:
            if (post.accessed[0] & FILE_MARKED)
                append_record (tname, &post, sizeof (post));
            break;
        case 1:
            if (strncmp (post.title, "Re: ", 4))
            {
                append_record (tname, &post, sizeof (post));
            }
            break;
        case 2:
        case 4:
            if (type == 2)
                strtolower (tempname1, post.owner);
            else
                strtolower (tempname1, post.title);
            strtolower (tempname2, someoneID);
			tempname1[44] = '\0';
			tempname2[44] = '\0';
            if (strstr (tempname1, tempname2))
                append_record (tname, &post, sizeof (post));
            break;
        case 3:
            if (!strcasecmp (post.owner, someoneID))
                append_record (tname, &post, sizeof (post));
            break;
        }
    }
    close (fd);
    unlink (fname);
    return 0;
}


int
return_search_all (int type)
{
    struct fileheader post;
    int fd;
    char dname[STRLEN], buf[STRLEN];
    char tempname1[50], tempname2[50];
    struct stat st1, st2;
	
    sprintf (dname, "boards/%s/%s", currboard, RETURN_DIR);
    sprintf (fname, "boards/%s/%s2", currboard, RETURN_DIR);
    switch (type)
    {
    case 1:  //作者模糊
		sprintf (tname, "boards/%s/SOMEONE.%s.RETURN_DIR.1", currboard, someoneID);
		break;
    case 2:  //作者精确
        sprintf (tname, "boards/%s/SOMEONE.%s.RETURN_DIR.2", currboard, someoneID);
        break;
    case 3: //关键字
        sprintf (tname, "boards/%s/KEY.%s.RETURN_DIR", currboard, currentuser.userid);
        break;
    }
	
	//查询之前先删除上次的查询索引
	//debug by rwzmm @ sjtubbs, 2004.07.03.
	if(dashf(tname))
		unlink(tname);

    if (stat (dname, &st1) == -1)
        return 1;
    if (stat (tname, &st2) != -1)
    {
        if (st2.st_mtime >= st1.st_mtime)
            return 1;
    }
    unlink(tname);
    sprintf (buf, "cp %s %s", dname, fname);
    system (buf);
	
    if ((fd = open (fname, O_RDONLY, 0)) == -1)
        return -1;
	
    while (read (fd, &post, sizeof (struct fileheader)) ==
		sizeof (struct fileheader))
    {
        switch (type)
        {
        case 1:
        case 3:
            if (type == 1)
                strtolower (tempname1, post.owner);
            else
                strtolower (tempname1, post.title);
            strtolower (tempname2, someoneID);
			tempname1[44] = '\0';
			tempname2[44] = '\0';
            if (strstr (tempname1, tempname2))
                append_record (tname, &post, sizeof (post));
            break;
        case 2:
            if (!strcasecmp (post.owner, someoneID))
                append_record (tname, &post, sizeof (post));
            break;
        }
    }
    close (fd);
    unlink (fname);
    return 0;
}

