#ifndef _H_NTC_MAIL
#define _H_NTC_MAIL

#include <stdio.h>
#include <ctype.h>

G_BEGIN_DECLS
    inline static int nt_setmailfile(char *buf, size_t buflen,
                                     const char *userid,
                                     const char *filename)
{
    return snprintf(buf, buflen, "mail/%c/%c/%s/%s", toupper(userid[0]),
                    toupper(userid[1]), userid, filename);
}

/**
 * get the number of mails of user userid
 *
 * @param userid the userid
 * 
 * @return the number on success, 0 on failed
 */
extern unsigned int nt_mail_get_num(const char *userid);

/**
 * get the number of unread mails of user userid
 *
 * @param userid the userid
 * 
 * @return the number on success, 0 on failed
 */
extern unsigned int nt_mail_get_unread_num(const char *userid);

extern int
nt_post_mail(char *userid, char *title, char *file, char *id,
          char *nickname, int autocr, char *ip, int sig);

extern int
nt_post_imail(char *userid, char *title, char *file, char *id,
           char *nickname, int autocr, char *ip, int sig);

G_END_DECLS
#endif /*_H_NTC_MAIL*/
