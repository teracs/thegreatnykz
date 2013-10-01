/***************** IMPORTANT NOTES ***********************
 
 This php extension CAN'T be used in mutil-threads environment,
 e.g. you can't use it with apache 2.0 with worker mode.
 This extension use swig util(www.swig.org). Before make, you
 should:
 1. swig -php jbbs.i to generate wrapper c source file
 2. edit jbbs_wrapper.c, change as below:
    A) in PHP_MSHUTDOWN_FUNCTION(jbbs), add "jbbs_module_shutdown();".
    like
    PHP_MSHUTDOWN_FUNCTION(jbbs)
    {
            jbbs_module_shutdown();
	    return SUCCESS;
    }
    B) in PHP_MINIT_FUNCTION(jbbs), add 2 lines
    if (!jbbs_module_init())
             return FAILURE;
    like
    PHP_MINIT_FUNCTION(jbbs)
    {
    	int i;
        for (i = 0; swig_types_initial[i]; i++) {
        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
        }
	CG(active_class_entry) = NULL;

        if (!jbbs_module_init())
                return FAILURE;

	return SUCCESS;
    }
    C) in PHP_RINIT_FUNCTION(jbbs), add 2 lines
    if(!jbbs_request_init())
        return FAILURE;
    like
    PHP_RINIT_FUNCTION(jbbs)
    {
        if(!jbbs_request_init())
            return FAILURE;
        return SUCCESS;
    }
    D) in PHP_RSHUTDOWN_FUNCTION(jbbs), add "jbbs_request_shutdown();"
    like
    PHP_RSHUTDOWN_FUNCTION(jbbs)
    {
        jbbs_request_shutdown();
        return SUCCESS;
    }
 3. make and enjoy it:)

 Author: jawahh (jawahh@gmail.com)

**********************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

#include "bbs.h"
#include "const.h"
#include "log.h"
#include "shm.h"

/*************** START UTIL FUNCTION ********************/
/* ALL UTIL FUNCTION SHOULD BE STATIC */

/*justify if user is a web user*/
static int 
is_web_user(struct user_info *user)
{
	return (10001==user->mode);
}

/* get user position in PASSWD files */
static int
getusernum(char *id)
{
	int i;

	if (id[0] == 0)
		return -1;
	for (i = 0; i < MAXUSERS; i++)
	{
		if (!strcasecmp(shm_ucache->userid[i], id))
			return i;
	}
	return -1;
}


/* get struct userec from PASSWD files for user whose userid is id,
and write it to urec. return 0 on failure */
static int
getuser(char *id, struct userec *urec)
{
	int uid, fd;

	if(id==NULL || id[0]==0)
		return 0;
	uid = getusernum(id);
	if (uid < 0)
		return 0;
	fd = open(PASSFILE, O_RDONLY);
	lseek(fd, uid * sizeof(struct userec), SEEK_SET);
	read(fd, urec,sizeof(struct userec));
	close(fd);
	return 1;
}

/*************** END UTIL FUNCTION *********************/


static struct userec currentuser;
static struct user_info *u_info;

static char pre_working_dir[PATH_MAX];
/* this function is called before every requests start
return 0 for failure*/
int jbbs_request_init(void)
{
	getcwd(pre_working_dir,PATH_MAX);
	chdir(BBSHOME);

	if((uid_t)BBSUID!=geteuid())
		return 0;
	if(shm_init())
		return 0;
	
	bzero(&currentuser,sizeof(currentuser));
	u_info = NULL;

	chdir(pre_working_dir);
	return 1;
}
void jbbs_request_shutdown(void)
{
	/*
	getcwd(pre_working_dir,PATH_MAX);
	chdir(BBSHOME);

	do sth here, plz

	chdir(pre_working_dir);
	*/
}
/* this function is module initial function
return 0 for failure*/
int jbbs_module_init(void)
{
	getcwd(pre_working_dir,PATH_MAX);
	chdir(BBSHOME);

	if(log_init())
		return 0;


	chdir(pre_working_dir);
	return 1;
}
void jbbs_module_shutdown(void)
{
	getcwd(pre_working_dir,PATH_MAX);
	chdir(BBSHOME);

	shm_final();
	log_final();

	chdir(pre_working_dir);
}
int jbbs_check_session(char * id, char *fromhost, int num, int key)
{
	if(num <0 || num >= MAXACTIVE)
		return 0;
	u_info = &(shm_utmp->uinfo[num]);
	if (strcmp(u_info->from, fromhost))
		return 0;
	if (u_info->utmpkey != key)
		return 0;
	if (u_info->active == 0)
		return 0;
	if (u_info->userid[0] == 0)
		return 0;
	if (!is_web_user(u_info))
		return 0;
	if (!strcasecmp(u_info->userid, "new"))
		return 0;	
	if (!strcasecmp(u_info->userid, "guest"))
	{
		u_info->idle_time = time(0);
		return 0;
	}
	if ( 0 == getuser(u_info->userid,&currentuser) ) 
		return 0;
	if (strcmp(currentuser.userid, id))
		return 0;
	u_info->idle_time = time(0);
	return 1;
}
/*if currentuser doesn't have such permission, return 0*/
unsigned int jbbs_has_perm(unsigned int perm)
{
	return  currentuser.userlevel & perm ;
}

