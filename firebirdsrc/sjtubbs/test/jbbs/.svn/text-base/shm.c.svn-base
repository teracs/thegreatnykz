#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <glib.h>



#include "bbs.h"
#include "const.h"
#include "log.h"
#include "shm.h"

struct UTMPFILE *shm_utmp = NULL;
struct BCACHE *shm_bcache = NULL;
struct UCACHE *shm_ucache = NULL;
static struct SHM_t *bbsshm = NULL;

static void
attach_err(int shmkey, char *name, int err)
{
	g_warning(__FILE__ ":  %s: %s! key = %x", name, strerror(err),
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

static int
apply_record(char *filename, int (*fptr) (void *,int), int size)
{
	char abuf[8192];
	int fd;
	int count = 0;

	if (size > 8192)
	{
		g_warning(__FILE__ ": too big record size! %d", size);
		return -1;
	}

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;

	while (read(fd, abuf, size) == size)
	{
		if ((*fptr) (abuf,count) != RETOK)
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

int
resolve_bcache(void)
{
	struct stat st;
	time_t now;

	now = time(0);
	if (stat(BOARDS, &st) < 0)
	{
		g_warning(__FILE__ ": can't stat file " BOARDS);
		return 1;
	}
	if (shm_bcache->uptime < st.st_mtime)
	{
		g_debug(__FILE__ ": reload bcache");
		shm_bcache->number = apply_record(BOARDS, fill_bcache, sizeof(struct boardheader));
		shm_bcache->uptime = st.st_mtime;
	}
	return 0;
}
static int
fill_ucache(void *p,int count)
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

int
resolve_ucache(void)
{
	struct stat st;
	time_t now;
	if (stat(FLUSH, &st) < 0)
	{
		g_warning(__FILE__ ": can't stat file " FLUSH);
		return 1;
	}
	now = time(0);
	if (st.st_mtime > shm_ucache->uptime)
	{
		g_debug(__FILE__ ": reload ucache");
		shm_ucache->number = apply_record(PASSFILE, fill_ucache, sizeof(struct userec));
		shm_ucache->uptime = st.st_mtime;
	}
	return 0;
}

int
shm_init(void)
{
	if (attach_shm() ||  resolve_bcache() || resolve_ucache())
		return 1;
	else
		return 0;
}

void
shm_final(void)
{
	if (bbsshm)
	{
		shmdt(bbsshm);
		bbsshm  = NULL;
	}
}

