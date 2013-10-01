//created by clist

#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

struct UTMPFILE *utmpshm = NULL;
struct BCACHE *brdshm = NULL;
struct SHM_t *bbsshm = NULL;

static int attach_shm(void);

int main()
{
    FILE *fp, *nfp;
    int boardnum, user_count, deleted_count, found, i;
    char inboard_file[STRLEN], tmpbuf[1024], fnnew[STRLEN];
    char *boardname, *userid;

    chdir(BBSHOME);
    attach_shm();
    utmpshm = &(bbsshm->utmpshm);
    brdshm = &(bbsshm->bcacheshm);

    for (boardnum=0; boardnum<MAXBOARD; boardnum++)
    {
        boardname = brdshm->bcache[boardnum].filename;
        if (boardname == NULL || strlen(boardname) == 0)
            continue;

        sprintf(inboard_file, "boards/%s/%s", boardname, INBOARD_FILE);
        if ((fp = fopen (inboard_file, "r")) == NULL)
            continue;

        sprintf (fnnew, "%s.%d", inboard_file, getuid ());
        if ((nfp = fopen (fnnew, "w")) == NULL)
            continue;

        user_count = 0;
        deleted_count = 0;
        while (fgets (tmpbuf, 1024, fp) != NULL)
        {
            found = 0;
            userid = (char *) strtok(tmpbuf, "\n");
            for (i = 0; i < USHM_SIZE; i++) {
                if (strcasecmp (utmpshm->uinfo[i].userid, userid) == 0)
                {
                    found = 1;
                    fprintf(nfp, "%s\n", userid);
                    user_count ++;
                    break;
                }
            }
            if (!found)
                deleted_count ++;
        }
        fclose (fp);
        fclose (nfp);
        rename (fnnew, inboard_file);

        brdshm->inboard[boardnum] = user_count;

//      printf("board %s has %d users, delete %d inactive users.\n", boardname, user_count, deleted_count);
    }

}

void attach_err (int shmkey, char *name, int err)
{
    char genbuf[63];
    sprintf (genbuf, "Error! %s error #%d! key = %x.\n", name, err, shmkey);
    write (1, genbuf, strlen (genbuf));
    exit (1);
}

int attach_shm (void)
{
    int  shmid;
	if(bbsshm != NULL)
		return 0;
#ifdef USE_SHM_HUGETLB
	shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE, SHM_HUGETLB);
#else
	shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE, 0);
#endif
    if (shmid < 0)
    {
#ifdef USE_SHM_HUGETLB
		shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE,IPC_CREAT | 0600 | SHM_HUGETLB);
#else
		shmid = shmget(BBS_SHM_KEY, BBS_SHM_SIZE,IPC_CREAT | 0600);
#endif      
        if (shmid < 0)
            attach_err (BBS_SHM_KEY, "shmget", errno);
        bbsshm = (void *) shmat (shmid, NULL, 0);
        if (bbsshm == (void *) -1)
            attach_err (BBS_SHM_KEY, "shmat", errno);
        memset (bbsshm, 0, BBS_SHM_SIZE);
    }
    else
    {
		bbsshm = (void *) shmat (shmid, NULL, 0);
        if (bbsshm == (void *) -1)
            attach_err (BBS_SHM_KEY, "shmat", errno);
    }
    return 0;
}

