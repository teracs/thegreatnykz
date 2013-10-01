#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <glib.h>



#include "bbs.h"
#include "shm.h"
#include "dispatcher.h"
#include "const.h"



struct UTMPFILE *shm_utmp = NULL;
struct BCACHE *shm_bcache = NULL;
struct UCACHE *shm_ucache = NULL;
static struct SHM_t *bbsshm = NULL;

static void attach_err(int shmkey, char *name, int err)
{
	g_warning("%s: %s! key = %x", name, strerror(err),
		  shmkey);
}

static int attach_shm(void)
{
	void *shmptr;
	int  shmid;

	if(NULL!=bbsshm)
	{
		return 0;
	}
	g_debug( "attach shm, size %d", BBS_SHM_SIZE);
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
		{
			attach_err(BBS_SHM_KEY, "shmget", errno);
			return 1;
		}
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
		{
			attach_err(BBS_SHM_KEY, "shmat", errno);
			return 1;
		}
		memset(shmptr, 0, BBS_SHM_SIZE);
	}
	else
	{
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
		{
			attach_err(BBS_SHM_KEY, "shmat", errno);
			return 1;
		}
	}
	bbsshm = shmptr;
	shm_utmp = &(bbsshm->utmpshm);
	shm_bcache = &(bbsshm->bcacheshm);
	shm_ucache = &(bbsshm->ucacheshm);
	return 0;
}

static int apply_record(char *filename, int (*fptr) (void *,int), int size)
{
	char abuf[8192];
	int fd;
	int count = 0;

	if (size > 8192)
	{
		g_warning( "too big record size! %d", size);
		return -1;
	}

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;

	while (read(fd, abuf, size) == size)
	{
		if ((*fptr) (abuf,count) != 0)
			break;
		count++;
	}

	close(fd);
	return count+1;
}


static int
fill_bcache(void *p,int count)
{
	struct shortfile *ptr = p, *dptr;
	if (count >= MAXBOARD)
		return 1;
	dptr = &shm_bcache->bcache[count];
	memcpy(dptr, ptr, sizeof(struct shortfile));
	return 0;
}

static int
resolve_bcache(void)
{
	struct stat st;
	time_t now;
	FILE* fp_nopost = NULL;
	char buffer[2][16];

	bzero(buffer,sizeof(buffer));


	now = time(0);
	if (stat(BOARDS, &st) < 0)
	{
		g_warning( "can't stat file " BOARDS);
		return 1;
	}
	if (shm_bcache->uptime < st.st_mtime)
	{
		g_debug( "reload bcache");
		shm_bcache->number = apply_record(BOARDS, fill_bcache, sizeof(struct boardheader));
		shm_bcache->uptime = st.st_mtime;
	}

	/* load no post time to shm, by zsong */
	fp_nopost = fopen("NO_POST_TIME", "r");
	if(fp_nopost == NULL) /* no restriction on boards*/ 
	{
		shm_bcache->no_post_begin = 0;
		shm_bcache->no_post_end = 0;
	}
	else
	{
		fgets(buffer[1], 15, fp_nopost);
		fgets(buffer[2], 15, fp_nopost);
		shm_bcache->no_post_begin = atoi(buffer[1]);
		shm_bcache->no_post_end = atoi(buffer[2]);
		fclose(fp_nopost);
	}
	fp_nopost = NULL;
	fp_nopost = fopen("ALL_NO_POST_TIME", "r");
	if(fp_nopost == NULL) /* no restriction on boards*/ 
	{
		shm_bcache->all_no_post_begin = 0;
		shm_bcache->all_no_post_end = 0;
	}
	else
	{
		fgets(buffer[1], 15, fp_nopost);
		fgets(buffer[2], 15, fp_nopost);
		shm_bcache->all_no_post_begin = atoi(buffer[1]);
		shm_bcache->all_no_post_end = atoi(buffer[2]);
		fclose(fp_nopost);
	}
	
	return 0;
}
static int fill_ucache(void *p,int count)
{
	struct userec *ptr = p;
	if (count < MAXUSERS)
	{
		strncpy(shm_ucache->userid[count], ptr->userid,
			IDLEN + 1);
		shm_ucache->userid[count][IDLEN] = '\0';
		return 0;
	}
	return 1;
}

static int resolve_ucache(void)
{
	struct stat st;
	time_t now;

	if (stat(FLUSH, &st) < 0)
	{
		g_warning( "can't stat file " FLUSH);
		return 1;
	}
	now = time(0);
	if (st.st_mtime > shm_ucache->uptime)
	{
		g_debug( "reload ucache");
		shm_ucache->number = apply_record(PASSFILE, fill_ucache, sizeof(struct userec));
		shm_ucache->uptime = st.st_mtime;
	}
	return 0;
}

CALLBACK_RETURN bbsshm_final(CALLBACK_ID callback_id, void *data, void *userdata)
{
	
	if (bbsshm)
		shmdt(bbsshm);
	return CALLBACK_CONTINUE;
}

gboolean bbsshm_init(EVENT_ID unload_event, EVENT_ID enter_event, EVENT_ID exit_event)
{
	if (attach_shm() ||  resolve_bcache() || resolve_ucache())
		return FALSE;
	register_callback(unload_event, bbsshm_final, NULL);
	return TRUE;
}


