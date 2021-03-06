/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
	Firebird Bulletin Board System  -- Version 3.0 series
	Copyright (C) 1999, Peng Piaw Foong, Harimau.bbs@MSIA.pine.ncu.edu.tw
	Edward Ping-Da Chuang, edwardc.bbs@bbs.seed.net.tw
						
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: bbs.h,v 1.4 2004/07/15 10:14:56 bbs Exp $
*/


#ifndef  _BBS_H_
#define _BBS_H_

#ifndef BBSIRC

/* Global includes, needed in most every source file... */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#ifndef BSD44
#include <stdlib.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include "config.h"             /* User-configurable stuff */

#include "glossary.h"
#include "functions.h"          /* you can enable functions that you want */
#include "permissions.h"

//#include <mysql/mysql.h>	/* added by cf@yssy */

#define DENY_POST_BOARDNAME "penalty"   //add by alt

#define TRAIN_ANNOUNCE_BOARDNAME	"TrainingAdmin"		//add by rwzmm

#ifndef LOCK_EX
#define LOCK_EX         2       /* exclusive lock */
#define LOCK_UN         8       /* unlock */
#endif


#ifdef XINU
extern int errno;
#endif

#define randomize() srand((unsigned)time(NULL))

#define YEA (1)                 /* Booleans  (Yep, for true and false) */
#define NA  (0)

#define DOECHO (1)              /* Flags to getdata input function */
#define NOECHO (0)

char *bfile();

extern FILE *ufp;               /* External variable declarations */
extern long ti;

#endif                          /* BBSIRC */

#define MAXFRIENDS (200)
#define MAXREJECTS (32)
#define NUMPERMS   (32)
#define REG_EXPIRED         360 /* 重做身份确认期限 */
#define MAXGUEST			 2000     /* 最多 guest 帐号上站个数 */

#define SectNum         	12      /*add by rwzmm , 现有分区数 */
#define FILE_BUFSIZE        255 /* max. length of a file in SHM */
#define FILE_MAXLINE         25 /* max. line of a file in SHM */
#define MAX_WELCOME          15 /* 欢迎画面数 */
#define MAX_GOODBYE          15 /* 离站画面数 */
#define MAX_ISSUE            15 /* 最大进站画面数 */
#define MAX_DIGEST         1000 /* 最大文摘数 */
#define MAX_TOP				5 /* 最大置底文章数 */
#define MAX_POSTRETRY       100
#define MAX_MSG_SIZE 1024	//for multi_msg,by cf
#define MSGDIR ".msgdir"
#define MORE_BUFSIZE       4096

#ifdef BIGGER_MOVIE
#define MAXMOVIE		8
#else
#define MAXMOVIE		6       /* lines of  activity board  */
#endif

#define ACBOARD_BUFSIZE     256 /* max. length of each line for activity board */
//#define ACBOARD_MAXLINE      210 /* max. lines of  activity board  */
#define ACBOARD_MAXLINE      2000       /* max. lines of  activity board  */
#define STRLEN               80 /* Length of most string data */
#define NAMELEN              40 /* Length of username/realname */
#define IDLEN                12 /* Length of userids */

#ifdef MD5                      /* MD5 cryptographic support */
#define ENCPASSLEN         35
#else
#define ENCPASSLEN         14   /* Length of encrypted passwd field */
#endif

#define PASSLEN		 	14      /* User's password length (13 chars) */

#ifdef CODE_VALID
#define RNDPASSLEN             10       /* 暗码认证的暗码长度 (适宜范围 4~10) */
#endif

/*#define MAXGOPHERITEMS     9999*/ /* max of gopher items */
#define PASSFILE     ".PASSWDS" /* Name of file User records stored in */
#define ULIST_BASE   ".UTMP"    /* Names of users currently on line */
#define RETURN_DIR  ".RETURNDIR"        /* 回收站, add by mudboy */
#define TOPFILE_DIR ".TOPFILE"

extern char ULIST[];

#ifndef BBSIRC

#define FLUSH       ".PASSFLUSH"        /* Stores date for user cache flushing */
#define BOARDS      ".BOARDS"   /* File containing list of boards */
#define DOT_DIR     ".DIR"      /* Name of Directory file info */
#define THREAD_DIR  ".THREAD"   /* Name of Thread file info */
#define DIGEST_DIR  ".DIGEST"   /* Name of Digest file info */
#define MARKED_DIR  ".MARKEDDIR"
#define AUTHOR_DIR  ".AUTHORDIR"
#define KEY_DIR	    ".KEYDIR"
/* add by  pax@sjtubbs */
#define BANK_RATE "BANK/RATE"//利率
#define PK_PERSON_FILE	"RPG.PERSON" /* 个人资料 */
/*end*/

#define QUIT 0x666              /* Return value to abort recursive functions */

#define FILE_READ  0x1          /* Ownership flags used in fileheader structure */
#define FILE_OWND  0x2          /* accessed array */
#define FILE_VISIT 0x4
#define FILE_MARKED 0x8
#define FILE_DIGEST 0x10        /* Digest Mode */
#define FILE_RECOMMENDED 0x8	/*Recommend post*/
#define FILE_FORWARDED 0x20     /* Undelete file */
#define MAIL_REPLY 0x20         /* Mail Reply */
#define FILE_NOREPLY 0x40       /* No Allow Replay */
#define FILE_DELETED 0x80
#define FILE_TOP1 0x2			/*modified from 0x320 to 0x2,by acdsee */

#define VOTE_FLAG    0x1
#define NOZAP_FLAG   0x2
#define OUT_FLAG     0x4
#define ANONY_FLAG   0x8
#define NOREPLY_FLAG 0x10       /* No reply board */
#define POSITIVE_ANONY_FLAG   0x20   /* modified by littlesword@SJTUbbs,positive_anony */

#define ZAPPED  0x1             /* For boards...tells if board is Zapped */

/* reg form flags. add by rwzmm @ sjtubbs tech group */
#define REG_ACCEPT	0x1;
#define	REG_REJECT	0x2;
#define	REG_DELETE	0x4;
/* add end */





/* these are flags in userec.flags[0] */
#define PAGER_FLAG   0x1        /* true if pager was OFF last session */
#define CLOAK_FLAG   0x2        /* true if cloak was ON last session */
#define SIG_FLAG     0x8        /* true if sig was turned OFF last session */
#define BRDSORT_FLAG 0x20       /* true if the boards sorted alphabetical */
#define CURSOR_FLAG  0x80  /* true if the cursor mode open */   /* 现在不用了 */
#define ACTIVE_BOARD 0x200      /* true if user toggled active movie board on */

#define MSG_SEPERATOR   "\
———————————————————————————————————————"
#define MULTI_LOGINS	3       /* 同时可上站 ID 数 */
#define DEF_VALUE		"★★ 欢迎光临饮水思源站! ★★"
#define DEF_FILE		"etc/whatdate"

#define ALL_PAGER       0x1
#define FRIEND_PAGER    0x2
#define ALLMSG_PAGER    0x4
#define FRIENDMSG_PAGER 0x8

#define SHIFTMODE(usernum,mode) ((usernum<MAXUSERS)?mode:mode<<4)

#define SETFILEMODE(array,usernum,mode) \
     (array[usernum%MAXUSERS] |= ((usernum<MAXUSERS)?mode:mode<<4))

#define CLRFILEMODE(array,usernum,mode) \
          (array[usernum%MAXUSERS] &= ((usernum<MAXUSERS)?~mode:~(mode<<4)))

#define CHECKFILEMODE(array,usernum,mode) \
       (array[usernum%MAXUSERS] & ((usernum<MAXUSERS)?mode:mode<<4))
#define USERIDSIZE (16)
#define USERNAMESZ (24)
#define TERMTYPESZ (10)
/* END */

#endif                          /* BBSIRC */

#ifndef NO_STRUCT_H
#include "struct.h"
#endif

#include "modes.h"              /* The list of valid user modes */

#define DONOTHING       0       /* Read menu command return states */
#define FULLUPDATE      1       /* Entire screen was destroyed in this oper */
#define PARTUPDATE      2       /* Only the top three lines were destroyed */
#define DOQUIT          3       /* Exit read menu was executed */
#define NEWDIRECT       4       /* Directory has changed, re-read files */
#define READ_NEXT       5       /* Direct read next file */
#define READ_PREV       6       /* Direct read prev file */
#define GOTO_NEXT       7       /* Move cursor to next */
#define DIRCHANGED      8       /* Index file was changed */
#define MODECHANGED     9       /* ... */

#define I_TIMEOUT   (-2)        /* Used for the getchar routine select call */
#define I_OTHERDATA (-333)      /* interface, (-3) will conflict with chinese */

#define SCREEN_SIZE (23)        /* Used by read menu  */

#define DEGISTFLAG		"\033[0;4m"	/*Degist Flag add by pax*/

extern int scrint;              /* Set when screen has been initialized */

/* Used by namecomplete *only* */

extern int digestmode;          /*To control Digestmode */
extern int returnmode;			/*To control detailed mode in return mode*/

#ifndef NO_STRUCT_H             /* NO_STRUCT_H */

extern struct userec currentuser;       /*  user structure is loaded from passwd */

/*  file at logon, and remains for the   */
/*  entire session */

extern struct user_info uinfo;  /* Ditto above...utmp entry is stored here
                                   and written back to the utmp file when
                                   necessary (pretty darn often). */
extern int usernum;             /* Index into passwds file user record */
extern int utmpent;             /* Index into this users utmp file entry */
extern int count_friends, count_users;  /*Add by SmallPig for count users and friends */

extern int t_lines, t_columns;  /* Screen size / width */
extern struct userec lookupuser;        /* Used when searching for other user info */
//pax
extern struct person currentpker;
extern struct person lookuppker;
//end

#endif                          /* NO_STRUCT_H */
extern char someoneDIR[];
extern int nettyNN;
extern char netty_path[];
extern char netty_board[];
extern char currboard[];        /* name of currently selected board */
extern char currBM[];           /* BM of currently selected board */

extern int selboard;            /* THis flag is true if above is active */

extern char genbuf[1024];       /* generally used global buffer */

extern struct commands cmdlist[];       /* main menu command list */

extern jmp_buf byebye;          /* Used for exception condition like I/O error */

extern struct commands xyzlist[];       /* These are command lists for all the */
extern struct commands talklist[];      /* sub-menus */
extern struct commands maillist[];
extern struct commands dellist[];
extern struct commands maintlist[];

extern char save_title[];       /* These are used by the editor when inserting */
extern char save_filename[];    /* header information */
extern int in_mail;
extern int dumb_term;
extern int showansi;
extern time_t uptime;           /* save user last key-in time, up every 1min */

#define Min(a,b) ((a<b)?a:b)
#define Max(a,b) ((a>b)?a:b)

/*SREAD Define*/
#define SR_BMBASE       (10)
#define SR_BMDEL	(11)
#define SR_BMMARK       (12)
#define SR_BMDIGEST     (13)
#define SR_BMIMPORT     (14)
#define SR_BMTMP        (15)
#define SR_BMUNDERLINE (16)
/*SREAD Define*/

#ifndef EXTEND_KEY
#define EXTEND_KEY
#define KEY_TAB         9
#define KEY_ESC         27
#define KEY_UP          0x0101
#define KEY_DOWN        0x0102
#define KEY_RIGHT       0x0103
#define KEY_LEFT        0x0104
#define KEY_HOME        0x0201
#define KEY_INS         0x0202
#define KEY_DEL         0x0203
#define KEY_END         0x0204
#define KEY_PGUP        0x0205
#define KEY_PGDN        0x0206
#endif

/* edwardc.990706 move shmkeys from sysconf.ini */


struct _shmkey
{
    char *key;
    int value;
};
const static struct _shmkey shmkeys[] =
    {
        {"BCACHE_SHMKEY", 30002
        },
        {"UCACHE_SHMKEY", 30012},
        {"UTMP_SHMKEY", 30022},
        {"ACBOARD_SHMKEY", 30032},
        {"ISSUE_SHMKEY", 30042},
        {"GOODBYE_SHMKEY", 30052},
        {"WELCOME_SHMKEY", 30062},
        {"STAT_SHMKEY", 30072},
        {"CONV_SHMKEY", 30082},
        {"MCACHE_SHMKEY", 30092},
        {NULL, 0}
    };

#define Ctrl(c)         ( c & 037 )
#define isprint2(c)     ( (c & 0x80) || isprint(c) )

#ifdef  SYSV
#define bzero(tgt, len)         memset( tgt, 0, len )
#define bcopy(src, tgt, len)    memcpy( tgt, src, len)

#define usleep(usec)            {               \
    struct timeval t;                           \
    t.tv_sec = usec / 1000000;                  \
    t.tv_usec = usec % 1000000;                 \
    select( 0, NULL, NULL, NULL, &t);           \
}

#endif                          /* SYSV */

/* =============== ANSI EDIT ================== */
#define   ANSI_RESET    "\033[0m"
#define   ANSI_REVERSE  "\033[7m\033[4m"
extern int editansi;
extern int KEY_ESC_arg;

/* ============================================ */

/* Added by deardragon 1999.11.2 */
extern char datestring[];

/* Added End. */

//add by rwzmm @ sjtubbs, the max length in multi-line message
#define		EACH_LINE_LENGTH	61
//add end

//add by rwzmm @ sjtubbs, 新帐号注册一天以后方可以发文章
//如果要关闭此功能，将下面的宏定义注释掉即可。
/*#define		POST_AFTER_ONE_DAY*/

/*启用实名制*/
#define 	REAL_NAME_EMAIL
#define 	REAL_NAME_EMAIL_LEN 40
#define		REAL_NAME_FILE "real_email"
#define		REAL_NAME_CHK_CODE "real_email_chk_code"
#endif                          /* of _BBS_H_ */

/* 
p:put index into mysql db 
a:cf@yssy
t:2005-04-16
*/

#define DB_HOST "localhost"
#define DB_USER "siyuanhu"
#define DB_PASSWD "SYH"
#define DB_SOCK "/tmp/mysql.sock"
#define DB_PORT 0
#define DB_NAME "siyuanhu"
/*
=======
#define DB_HOST "202.120.3.4"
#define DB_USER "bbs"
#define DB_PASSWD "p0o9i8u7"
#define DB_SOCK NULL
#define DB_PORT 3306
#define DB_NAME "bbs"
>>>>>>> .r510
*/


//marco define
#define BM_LOG

/* add by pax@2007.1.1*/
#define LOG_M        0
#define LOG_G        1
#define LOG_I        2
/* end add by pax*/

#define MAIL_COST