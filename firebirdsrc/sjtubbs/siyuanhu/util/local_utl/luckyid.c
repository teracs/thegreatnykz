/****************************************************************/
/* 	this file is created by smartfish @ liumang bbs				*/
/****************************************************************/

#include "../../include/bbs.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>



int safewrite(int fd, char *buf, int size)
{
	int     cc, sz = size, origsz = size;
	char   *bp = buf;
	
	do {
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR)) {
//			report("safewrite err!");
			return -1;
		}
		if (cc > 0) {
			bp += cc;
			sz -= cc;
		}
	} while (sz > 0);
	return origsz;
}


int get_record(char *filename, char *rptr, int size, int id)
{
	int     fd;
	
	if ((fd = open(filename, O_RDONLY, 0)) == -1) return -1;
	if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1) {
		close(fd);
		return -1;
	}
	if (read(fd, rptr, size) != size) {
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


int substitute_record(char *filename, char *rptr, int size, int id)
{
	int     fd;
	
	if(id < 1) {
//		report("substitue_record(...) id <= 0");
		return -1;
	}	
	if(id > 70000){ // if id too big, maybe error!
//		report("substitue_record(...) id > 70000");
		return -1;
	}
	if((fd=open(filename,O_WRONLY|O_CREAT,0644))==-1) return -1;
	flock(fd, LOCK_EX);
	if(lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1){ 
//		report("subrec seek err");
		;
	} else {
		if (safewrite(fd, rptr, size) != size)
			;
//			report("subrec write err");
	} 

	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}




void
main()
{
  int size, sum, num;

  char filename[STRLEN];
  struct stat st;
  struct userec fboy;
  FILE *fp;
  char secu[STRLEN];

  char buf[3*STRLEN];
  char filebuf[3*STRLEN];
  FILE *target;
  FILE *fpPASSWD; 
  struct userec luckyid;
  int luckyid_num = 0;
  int prize;//奖金
  prize=1000;
  size = sizeof (struct userec);
 
  sprintf(filename, "%s/.PASSWDS", BBSHOME);

  if (stat (filename, &st) == -1)
  {

    return;
  }
  sum = st.st_size / size;
	
  if(sum == 0)
  {
	  return;
  }

  fboy.userlevel = 0;
 // while(!(fboy.userlevel & 0400000000))
/*modify by pax@1003*/
	//上站次数 >=50
	//文章数   >=500
	//生命力   >=30
	while((!(fboy.userlevel & 0400000000)) || (compute_user_value(&fboy)<30)||(fboy.numposts<500)||(fboy.numlogins<50))
	{
	  sleep(1);
	  srand (time (NULL));
	  num = rand () % sum;

	  fp = fopen (filename, "r");
	  fseek (fp, num * size, SEEK_SET);
	  fread (&fboy, size, 1, fp);
	  fclose (fp);
	}
	
  //经验值的1/20为奖金
  prize = count_exp(&fboy)/10; 

  sprintf(buf,"%s/0Announce/bbslist/luckyid", BBSHOME);
  if ((target = fopen(buf, "w")) == NULL)
  {
	  printf("Can't open luckyid\n");
	  return;
  }

  fprintf(target, "today's lucky id is %s\n the prize is %d", fboy.userid,prize);
  fclose(target);
  sprintf(filebuf, "%s/%s", BBSHOME, PASSFILE);

  if ((fpPASSWD = open(filebuf, O_RDONLY, 0)) == -1)
  {
	  printf("error\n");
	  return;
  }
  

  while (read(fpPASSWD, &luckyid, sizeof(luckyid)) == sizeof(luckyid))
  {		
	  if(!strncmp(luckyid.userid, fboy.userid, 12))
		  break;
	  else
		  luckyid_num ++;
  }

  close(fpPASSWD);

   printf("%s\n", luckyid.userid);

//  luckyid.money += 1000;
  luckyid.money += prize;
 
  substitute_record(filebuf, &luckyid, sizeof(struct userec), luckyid_num + 1);

  return;

}

//计算经验值(至少10000,保证中奖金额至少1000)
int
count_exp(udata)
struct userec *udata;
{
	int     exp;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	exp = udata->numposts +
		udata->numlogins / 3 +
		(time(0) - udata->firstlogin) / 86400 +
		udata->stay / 3600;
	return exp > 10000 ? exp : 10000;
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