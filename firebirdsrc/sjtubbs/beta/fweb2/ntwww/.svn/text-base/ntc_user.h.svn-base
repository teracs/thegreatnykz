#ifndef _H_NTC_USER
#define _H_NTC_USER

#include <stdio.h>
#include <ctype.h>

G_BEGIN_DECLS

/* passwd file operation */
/**
 * Get the position of user 'userid' in passwd file
 * 
 * @param userid the userid of the user
 * 
 * @return the position, starting from 0
 */
extern int nt_get_passwd_pos(const char *userid);
/**
 * get struct userec of 'userid' stored in passwd file
 * 
 * @param userid the userid
 * @param puser  store result of struct userec to the memory pointed to by 'puser', if sucessfully
 * 
 * @return TRUE if successfully
 * @see nt_get_userec_with_uid
 */
extern gboolean nt_get_userec(const char *userid, struct userec *puser);
/**
 * write the struct userec back to passwd file
 * 
 * @param puser  the struct to be written
 * 
 * @return TRUE if successfully
 * @see nt_set_userec_with_uid
 */
extern gboolean nt_set_userec(struct userec *puser);
/**
 * As nt_get_userec, but more effectively
 * 
 * @param uid    the uid of the user wanted, starting from 1
 * @param puser  the struct userec will written to the memory pointed to by 'puser'
 * 
 * @return TRUE if successfully
 * @see nt_get_userec
 */
extern gboolean nt_get_userec_with_uid(int uid, struct userec * puser);
/**
 * As nt_set_userec, but more effectively
 * 
 * @param puser  the struct userec to be written
 * @param uid    the uid of the user, starting from 1
 * 
 * @return 
 * @see nt_set_userec
 */
extern gboolean nt_set_userec_with_uid(struct userec *puser, int uid);

/* password checking and crypting*/
/**
 * Check if pw_crypted and pw_try are the same
 * 
 * @param pw_crypted the password crypted
 * @param pw_try     the clean text password
 * 
 * @return TRUE if the same
 */
extern gboolean nt_check_passwd(char * pw_crypted, char * pw_try);
/**
 * Crypt a password, only the first 8 byte is in effect if strlen(pw) > 8
 * 
 * @param pw     the clean text password
 * 
 * @return readonly crypted password
 */
extern char *nt_crypt_passwd(char * pw);

/* user login and logout*/
/**
 * logout the user pointed by puinfo
 * 
 * @param puinfo the user
 * 
 * @return TRUE if successfully
 */
extern gboolean nt_logout_user(struct user_info *puinfo);
/**
 * Check if user pointed by puser exceeds login limit. About 100 times per day.
 * 
 * @param puser  the user to be checked
 * 
 * @return FALSE if the user exceeds the limit
 */
extern gboolean nt_check_login_limit(struct userec *puser);

/* check user state*/
/**
 * Check if user has logined from www
 * 
 * @param puinfo the user to be checked
 * 
 * @return TRUE if logined from www
 */

inline static gboolean nt_is_web_user(struct user_info *puinfo)
{
	return (puinfo->mode == 10001);
}

/**
 * setup path to 'file' at user 'userid' home directory. the path is written to 'buf', the length of the 'buf' is 'buflen'. return the length of the path
 */

inline static int nt_sethomefile(char *buf, size_t buflen, const char *userid, const char *filename)
{
	return snprintf(buf, buflen, "home/%c/%c/%s/%s", toupper(userid[0]), toupper(userid[1]), userid, filename);
}



/* user perimission */
#ifdef HAS_PERM
#undef HAS_PERM
#endif

/**
 * Check if the user pointed by struct userec *puser has perimission 'perm'
 */
#define HAS_PERM(puser,perm) ((puser)->userlevel&(perm))
/**
 * give permission 'perm' to user pointed by struct userec *puser. but the result doesn't write to passwd file
 * 
 * @see nt_set_userec
 */
#define SET_PERM(puser,perm) ((puser)->userlevel |= (perm))
#define HAS_PERM_LOGIN(puser) ((puser)->userlevel&PERM_BASIC)
#define HAS_PERM_SYSOP(puser) ((puser)->userlevel&PERM_SYSOP) 
#define HAS_PERM_REALNAME(puser) ((puser)->userlevel&PERM_PERSONAL) 


/* active user */
#define USER_LOGIN_LIMIT_NUM 3
/**
 * Get an unused struct user_info from share memory
 * 
 * @param userid  the user who will use this struct
 * @param ppuinfo OUT: the (struct user_info *) will written to the memory pointed by ppuinfo
 * 
 * @return the index of new user_info in shm, -1 on error
 */
extern int nt_new_uinfo(const char *userid, struct user_info **ppuinfo);
/**
 * get online user_infos of 'userid' from shm
 * 
 * @param userid  the userid
 * @param puinfos to store the returned array of pointer to struct user_info
 * @param max     get up to 'max' number of user_info
 * 
 * @return the actual number of user_info got
 */
extern int nt_get_active_uinfo(const char *userid, struct user_info *puinfos[], int max);
/**
 * Get an array of (struct user_info *) of online users who satisfy 'condition' from shm
 * 
 * @param puinfos   to store the result array
 * @param max       get up to max number of (struct user_info *)
 * @param condition the condition function. return TRUE if the user satisfy condition. 1st arg is (struct user_info *) to the user, 2nd arg is userdata
 * @param userdata  the userdata passed to 'condition' function
 * 
 * @return the actual number user got
 */
extern int nt_get_active_uinfo_condition(struct user_info *puinfos[], int max, 
		gboolean (*condition)(struct user_info *, void *), void *userdata);
/**
 * iterate over the struct user_info of online user
 * 
 * @param walk_func walker function. if return FALSE, the iteration will stop. 1st arg is (struct user_info *) to the user, 2nd arg is userdata
 * @param userdata  the userdata passed to walker function
 */
extern void nt_foreach_active_uinfo(gboolean (*walk_func)(struct user_info *,void *),
		void *userdata);


/**
 * check if user have a www msg
 *
 * @param userid the user
 * 
 * @return TRUE if has msg, FALSE if not
 */
extern gboolean nt_msg_www_has_new(const char *userid);

/**
 * get online user num
 *
 * @return the number
 */
extern unsigned int nt_get_online_num(void);
G_END_DECLS
#endif
