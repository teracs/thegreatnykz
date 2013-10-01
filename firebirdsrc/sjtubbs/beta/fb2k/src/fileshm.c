/*
$Id: fileshm.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
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

int fill_shmfile (int mode, char *fname, char *shmkey)
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
    tmp = mmap(NULL, sizeof(struct FILESHM) * maxnum, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if(tmp==MAP_FAILED)
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

    if ((fffd = fopen (fname, "r")) == NULL)
    {
        return 0;
    }
    while ((fgets (buf, FILE_BUFSIZE, fffd) != NULL) && nowfn < maxnum)
    {
        if (lines > FILE_MAXLINE)
            continue;
        if (strstr (buf, "@logout@") || strstr (buf, "@login@"))
        {
            tmp[nowfn].fileline = lines;
            nowfn++;
            lines = 0;
            continue;
        }
        ptr = tmp[nowfn].line[lines];
        memcpy (ptr, buf, sizeof (buf));
        lines++;
    }
    fclose (fffd);
    tmp[nowfn].fileline = lines;
    nowfn++;
    tmp[0].max = nowfn;
    return 1;
}


void show_shmfile (struct FILESHM *fh)
{
    int i;
    char buf[FILE_BUFSIZE];

    for (i = 0; i < fh->fileline; i++)
    {
        strcpy (buf, fh->line[i]);
        showstuff (buf /*, 0 */ );
    }
}


void show_goodbyeshm (void)
{
    int logouts;

    logouts = goodbyeshm[0].max;
    clear ();
    show_shmfile (&goodbyeshm
                  [(currentuser.numlogins % ((logouts <= 1) ? 1 : logouts))]);
    munmap (goodbyeshm, sizeof(struct FILESHM)*MAX_GOODBYE);
	goodbyeshm = NULL;
}

void show_welcomeshm (void)
{
    int welcomes;

    welcomes = welcomeshm[0].max;
    clear ();
    show_shmfile (&welcomeshm
                  [(currentuser.numlogins % ((welcomes <= 1) ? 1 : welcomes))]);
//    if (DEFINE (DEF_TOP10))
        pressanykey ();
    munmap(welcomeshm, sizeof(struct FILESHM)*MAX_WELCOME);
	welcomeshm = NULL;
}


void show_issue (void)
{
    int issues = issueshm[0].max;

    show_shmfile (&issueshm[(issues <= 1) ? 0 :
                            ((time (0) / 86400) % (issues))]);
    munmap(issueshm, sizeof(struct FILESHM)*MAX_ISSUE);
	issueshm = NULL;
}
