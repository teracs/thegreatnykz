#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFSIZE (8192)
char genbuf[1024];
struct UCACHE *uidshm = NULL;
/* write by alt@sjtu 2003.03.12
 * Çå³ýËÀÍöID,²¢ÁÐ±íÔÚ tmp/bbs.killid ÎÄ¼þÖÐ */
int usernumber;

char datestring[30];

int
getdatestring (time_t now, int Chang)
{
    struct tm *tm;
    char weeknum[7][3] = { "Ìì", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù" };

    tm = localtime (&now);
    sprintf (datestring, "%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
    return (tm->tm_sec % 10);
}

void
log_usies (mode, mesg)
char *mode, *mesg;
{
    time_t now;
    char buf[256];

    now = time (0);
    getdatestring (now, NA);
    sprintf (buf, "%s %s %s\n", datestring, mode, mesg);
    file_append ("usies", buf);
}

int
compute_user_value (struct userec *urec)
{
    int value;

    if ((urec->userlevel & PERM_XEMPT)
            || strcmp (urec->userid, "SYSOP") == 0
            || strcmp (urec->userid, "guest") == 0)
        return 999;
	/* modified by rwzmm @ sjtubbs, long life */
	if(urec->userlevel & PERM_666LIFE)/* never clean 666 */
	{
		return 666;
	}
    value = (time (0) - urec->lastlogin) / 60;
    /* new user should register in 30 mins */
    if (strcmp (urec->userid, "new") == 0)
    {
        return (30 - value) * 60;
    }
    if (urec->numlogins <= 3)
        return (15 * 1440 - value) / 1440;
    if (!(urec->userlevel & PERM_LOGINOK))
        return (30 * 1440 - value) / 1440;
    if (urec->stay > 1000000)
        return (365 * 1440 - value) / 1440;
    return (120 * 1440 - value) / 1440;
}

int
danger_user_id (char *userid)
{
    char *ptr, ch;

    ptr = userid;
    while ((ch = *ptr++) != '\0')
    {
        if (!isalnum (ch) && ch != '_')
            return 1;
    }
    return 0;
}

char *
horoscope (month, day)
char month, day;
{
    char *name[12] = {
                         "Ä¦ôÉ", "Ë®Æ¿", "Ë«Óã", "ÄµÑò", "½ðÅ£", "Ë«×Ó",
                         "¾ÞÐ·", "Ê¨×Ó", "´¦Å®", "Ìì³Ó", "ÌìÐ«", "ÉäÊÖ"
                     };

    switch (month)
    {
    case 1:
        if (day < 21)
            return (name[0]);
        else
            return (name[1]);
    case 2:
        if (day < 19)
            return (name[1]);
        else
            return (name[2]);
    case 3:
        if (day < 21)
            return (name[2]);
        else
            return (name[3]);
    case 4:
        if (day < 21)
            return (name[3]);
        else
            return (name[4]);
    case 5:
        if (day < 21)
            return (name[4]);
        else
            return (name[5]);
    case 6:
        if (day < 22)
            return (name[5]);
        else
            return (name[6]);
    case 7:
        if (day < 23)
            return (name[6]);
        else
            return (name[7]);
    case 8:
        if (day < 23)
            return (name[7]);
        else
            return (name[8]);
    case 9:
        if (day < 23)
            return (name[8]);
        else
            return (name[9]);
    case 10:
        if (day < 24)
            return (name[9]);
        else
            return (name[10]);
    case 11:
        if (day < 23)
            return (name[10]);
        else
            return (name[11]);
    case 12:
        if (day < 22)
            return (name[11]);
        else
            return (name[0]);
    }
    return ("²»Ïê");
}

int
main (argc, argv)
int argc;
char *argv[];
{
    struct userec utmp, utmp2;
    int fd, val, i;
    struct userec zerorec;
    int size, j, re;
    FILE *fdtmp;
    char nname[50], buf[50], buf2[50], genbuf[80];

    (void) chdir (BBSHOME);
    strcpy (nname, "tmp/bbs.killid");
    fdtmp = fopen (nname, "w+");
    log_usies ("CLEAN", "dated users.");
    memset (&zerorec, 0, sizeof (zerorec));
    if ((fd = open (PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
        return -1;
    flock (fd, LOCK_EX);
    size = sizeof (utmp);
    for (i = 0; i < MAXUSERS; i++)
    {
        if (read (fd, &utmp, size) != size)
            break;
        val = compute_user_value (&utmp);
        if (utmp.userid[0] != '\0' && val < 0)
        {
            if (lseek (fd, (off_t) (-size), SEEK_CUR) == -1)
            {
                flock (fd, LOCK_UN);
                close (fd);
                return -1;
            }
            write (fd, &zerorec, sizeof (utmp));
            if (strcmp (utmp.userid, "new") == 0)
                continue;
            re = 0;
            lseek (fd, 0, SEEK_SET);
            for (j = 0; j < i; j++)
            {
                if (read (fd, &utmp2, size) != size)
                    break;
                if (strcmp (utmp.userid, utmp2.userid) == 0)
                {
                    re = 1;
                    break;
                }
            }
            lseek (fd, (off_t) (i + 1) * size, SEEK_SET);
            if (re)
                continue;		//¶ÔÖØ¸´ID,·ÀÖ¹´íÎóÉ¾³ýÄ¿Â¼
            getdatestring (utmp.lastlogin, NA);
            sprintf (genbuf, "#%d %-12s %14.14s %d %d %d", i + 1,
                     utmp.userid, datestring, utmp.numlogins, utmp.numposts,
                     val);
            log_usies ("KILL ", genbuf);
            if (!danger_user_id (utmp.userid))
            {
                sprintf (genbuf, "mail/%c/%s",
                         toupper (utmp.userid[0]), utmp.userid);
                if (!strncmp (genbuf + 7, utmp.userid, strlen (utmp.userid)))
                    f_rm (genbuf);
                sprintf (genbuf, "home/%c/%s",
                         toupper (utmp.userid[0]), utmp.userid);
                if (!strncmp (genbuf + 7, utmp.userid, strlen (utmp.userid)))
                    f_rm (genbuf);

                sprintf (buf, "[[1;3%dm%s×ù[m] ",
                         (utmp.gender == 'F') ? 5 : 6,
                         horoscope (utmp.birthmonth, utmp.birthday));
                sprintf (buf2, "ÍøÁä[[1;32m%d[0;1m]Ìì",
                         (time (0) - utmp.firstlogin) / 86400);
                fprintf (fdtmp,
                         "[1;37m%s [m([1;33m%s[m) ¹²ÉÏÕ¾ [1;32m%d[m ´Î  %s  %s\n",
                         utmp.userid, utmp.username, utmp.numlogins, buf2, buf);
                getdatestring (utmp.lastlogin, NA);
                fprintf (fdtmp,
                         "ÉÏ ´Î ÔÚ: [[1;32m%s[m] ´Ó [[1;32m%s[m] µ½±¾Õ¾Ò»ÓÎ¡£\n",
                         datestring, utmp.lasthost);
                getdatestring (utmp.lastlogout, NA);
                fprintf (fdtmp,
                         "ÀëÕ¾Ê±¼ä: [[1;32m%s[m] ÉúÃüÁ¦:[[1;32m%d[m] ÎÄÕÂ:[[1;32m%d[m] \n\n",
                         datestring, val, utmp.numposts);
            }
        }
    }
    flock (fd, LOCK_UN);
    close (fd);
    fclose (fdtmp);
    return 1;
}

int
fillucache (uentp)
struct userec *uentp;
{
    if (usernumber < MAXUSERS)
    {
        strncpy (uidshm->userid[usernumber], uentp->userid, IDLEN + 1);
        uidshm->userid[usernumber++][IDLEN] = '\0';
    }
    return 0;
}


int
apply_record (filename, fptr, size)
char *filename;
int (*fptr) ();
int size;
{
    char abuf[BUFSIZE];
    int fd;

    if (size > BUFSIZE)
    {
	log_usies("ERROR","too big record size!!");
        return -1;
    }

    if ((fd = open (filename, O_RDONLY, 0)) == -1)
        return -1;
    while (read (fd, abuf, size) == size)
        if ((*fptr) (abuf) == QUIT)
        {
            close (fd);
            return QUIT;
        }
    close (fd);
    return 0;
}
void
attach_err (shmkey, name, err)
int shmkey;
char *name;
int err;
{
    sprintf (genbuf, "Error! %s error #%d! key = %x.\n", name, err, shmkey);
    write (1, genbuf, strlen (genbuf));
    exit (1);
}

int
search_shmkey (char *keyname)
{
    int i = 0, found = 0;

    while (shmkeys[i].key != NULL)
    {
        if (strcmp (shmkeys[i].key, keyname) == 0)
        {
            found = shmkeys[i].value;
            break;
        }
        i++;
    }

    if (found == 0)
    {
        sprintf (genbuf, "search_shmkey(): cannot found %s SHMKEY entry!",
                 keyname);
        log_usies("ERROR",genbuf);
    }
    return found;
}

void *
attach_shm (shmstr, defaultkey, shmsize)
char *shmstr;
int defaultkey, shmsize;
{
    void *shmptr;
    int shmkey, shmid;

    shmkey = search_shmkey (shmstr);
    if (shmkey < 1024)
        shmkey = defaultkey;
    shmid = shmget (shmkey, shmsize, 0);
    if (shmid < 0)
    {
        shmid = shmget (shmkey, shmsize, IPC_CREAT | 0644);
        if (shmid < 0)
            attach_err (shmkey, "shmget", errno);
        shmptr = (void *) shmat (shmid, NULL, 0);
        if (shmptr == (void *) -1)
            attach_err (shmkey, "shmat", errno);
        memset (shmptr, 0, shmsize);
    }
    else
    {
        shmptr = (void *) shmat (shmid, NULL, 0);
        if (shmptr == (void *) -1)
            attach_err (shmkey, "shmat", errno);
    }
    return shmptr;
}
void
resolve_ucache ()
{
    //struct stat st;
    int ftime;

    if (uidshm == NULL)
    {
        uidshm = attach_shm ("UCACHE_SHMKEY", 3696, sizeof (*uidshm));
    }
    ftime = time (0) - 86400;
    usernumber = 0;
    apply_record (PASSFILE, fillucache, sizeof (struct userec));
    uidshm->number = usernumber;
    uidshm->uptime = ftime;
}
