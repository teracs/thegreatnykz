/* for shm antispam of rexchen */
/* $Id: mail2bbs.h,v 1.2 2004/06/29 14:59:23 rwzmm Exp $ */

#ifndef _MAIL2BBS_H_
#define _MAIL2BBS_H_

//#define MAIL_LIST_MODULE
//#define MAILLIST_RECEIVER     "_mail-list_"

#define DONT_RETURN_ANYTHING    /* 不产生任何退信 (not done yet) */
#define RULE  3                 /* rule 2 .... 10 (不建议超过 10 ) */
#define SPAM_SHMKEY		(31000L)
#define SPAMTABLE		2048

struct SPAM
{
    int spam_a;
    int spam_b;
    int spam_c;
    int spam_times;
};

struct SPAM_MAIL
{
    struct SPAM mail_flag[SPAMTABLE];
    time_t update_time;
};

#endif/*_MAIL2BBS_H_*/
