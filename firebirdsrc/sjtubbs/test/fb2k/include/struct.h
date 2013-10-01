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
$Id: struct.h,v 1.7 2004/07/24 17:57:16 rwzmm Exp $
*/

/* Note the protocol field is not inside an #ifdef FILES...
   this is a waste but allows you to add/remove UL/DL support without
   rebuilding the PASSWDS file (and it's only a lil ole int anyway).
*/
#ifndef _H_BBS_STRUCT
#define _H_BBS_STRUCT

#if defined (c_plusplus) || defined (__cplusplus)
extern "C"
{
#endif

    typedef unsigned char uschar;
    struct userec
    {                           /* Structure used to hold information in */
        char userid[IDLEN + 2]; /* PASSFILE */
        time_t firstlogin;
        char lasthost[60];
        unsigned int numlogins;
        unsigned int numposts;
        unsigned int medals;    /* 奖章数 */
        unsigned int money;     /* 金钱 */
        unsigned int inbank;    /* 存款 */
        time_t banktime;        /* 存入时间 */
        char flags[2];
#ifdef ENCPASSLEN
        char passwd[ENCPASSLEN];
#else
        char passwd[PASSLEN];
#endif
        char username[NAMELEN];
        char ident[NAMELEN];
        char termtype[16];
        char reginfo[STRLEN - 16];
        unsigned int userlevel;
        time_t lastlogin;
        time_t lastlogout;      /* 最近离线时间 */
        time_t stay;
        char realname[NAMELEN];
        char address[STRLEN];
        char email[STRLEN - 12];
        unsigned int nummails;
        time_t lastjustify;
        char gender;
        unsigned char birthyear;
        unsigned char birthmonth;
        unsigned char birthday;
        int signature;
        unsigned int userdefine;
        time_t notedate;
        int noteline;
    };

    struct user_info
    {                           /* Structure used in UTMP file */
        int active;             /* When allocated this field is true */
        int uid;                /* Used to find user name in passwd file */
        int pid;                /* kill() to notify user of talk request */
        int invisible;          /* Used by cloaking function in Xyz menu */
        int sockactive;         /* Used to coordinate talk requests */
        int sockaddr;           /* ... */
        int destuid;            /* talk uses this to identify who called */
        int mode;               /* UL/DL, Talk Mode, Chat Mode, ... */
        int pager;              /* pager toggle, YEA, or NA */
        int in_chat;            /* for in_chat commands   */
        int fnum;               /* number of friends */
        int ext_idle;           /* has extended idle time, YEA or NA */
        time_t logintime;       /* login time */
        char chatid[10];        /* chat id, if in chat mode */
        char from[60];          /* machine name the user called in from */
#ifndef BBSD
        char tty[20];           /* tty port */
#else
        time_t idle_time;       /* to keep idle time */
#endif
        char userid[20];
        char realname[20];
        char username[NAMELEN];
        int friends[MAXFRIENDS];
        int reject[MAXREJECTS];
        int utmpkey;
        //add by rwzmm@sjtubbs
        //lock ac-board while msg.
        int IsMsg;
    };

//added by kongyang 2005
//the following used in b_vote to store ip information
    struct voteip
    {
        char ip[48];
        unsigned int ipnum;     //different user with the same ip can only vote no more than MAXVOTEPERIP
    };
//added by kongyang 2006-06-28
    struct boardcollection
    {
        int subnum;             //the number of subdirectories not includes dirs in extends
        int boardnum;           //the number of boards
        int isextends;          //0 stands for being not an extends,1 not
        char subdirsfile[20][32];       //one dir can have 20 sub dirs, this stores the filename of each subs
        char subdirname[20][32];        //names
        char boards[20][32];    //can have 20 boards
        char dirname[32];       //filename of current dir
        char updir[32];         //the filename of father dir
        char extends[32];       //extends of subdirectories.
    };

    struct override
    {
        char id[13];
        char exp[40];
    };

    struct downline
    {
        char id[13];            // 下线 ID
        time_t time;            // 注册时间
        long int last;          // 上次奖励 <= 10000
        long int total;         // 累计奖励 <= 500000
        long int money;         // 下线钱数
        char mode[12];          // 状态描述
        int num;                // 奖励次数 <= 99
        int status;             // 帐号状态
        char unuse[80];
    };

#define BM_LEN 60
#define BDNAME_LEN 16

// add by zsong, for sensitive word filter
#define MAX_FILTER_LENGTH 16
#define MAX_ITEM 8
// add end

    struct boardheader
    {                           /* This structure is used to hold data in */
        //modified by zsong, for subboard
        char filename[STRLEN - 4];      /* the BOARDS files */
        int group;
        //modified end
        char owner[STRLEN - BM_LEN];
        //here modified by zsong @ sjtubbs, 20051214
        //char BM[BM_LEN - 1];
        char BM[BM_LEN - 4];
        char flag2[3];
        //modified end
        char flag;
        char title[STRLEN];
        unsigned level;
        unsigned char accessed[12];
        //modified by rwzmm @ sjtubbs
//      unsigned char accessed[4];
//      unsigned int id;
//      unsigned int reid;
        //re id. add by rwzmm @ sjtubbs.
    };

    struct fileheader
    {                           /* This structure is used to hold data in */
        char filename[STRLEN];  /* the DIR files */
        char owner[STRLEN - 4];
        unsigned int pnum;      //the number of push articles
        char title[STRLEN];
        unsigned level;
//    unsigned char accessed[12]; /* struct size = 256 bytes */
//modified by rwzmm @ sjtubbs
        unsigned char accessed[4];
        unsigned int id;
        unsigned int reid;      //re id. add by rwzmm @ sjtubbs.
    };

    struct shortfile
    {                           /* used for caching files and boards */
        char filename[STRLEN - 4];      /* also will do for mail directories */
        int group;              //for sub-board
        char owner[STRLEN - BM_LEN];
        //modified by zsong, 20051214
        char BM[BM_LEN - 4];
        char flag2[3];
        //modified end
        char flag;
        char title[STRLEN];
        unsigned level;
        unsigned char accessed;
        //add by zsong, sub-board
//      int     total;
//      int     lastpost;
        //add end
    };

    struct one_key
    {                           /* Used to pass commands to the readmenu */
        int key;
        int (*fptr) ();
    };

#define USHM_SIZE       (MAXACTIVE + 10)
#define SESS_MAGIC_NUMBER 5555

    struct UTMPFILE
    {
        struct user_info uinfo[USHM_SIZE];
        time_t uptime;
        unsigned int usersum;
        int max_login_num;
        //long int          visit_total;
    };

    struct BCACHE
    {
        struct shortfile bcache[MAXBOARD];
        int number;
        time_t uptime;
        time_t pollvote;
        time_t fresh_date;
        char date[60];
        time_t friendbook;      /* friendbook mtime */
        int inboard[MAXBOARD];
        unsigned int no_post_begin;     /* add by zsong, fk zhouji */
        unsigned int no_post_end;       /* add by zsong, fk zhouji */
        unsigned int all_no_post_begin;
        unsigned int all_no_post_end;
        char wordfilter[MAX_ITEM][MAX_FILTER_LENGTH];
    };

    struct UCACHE
    {
        char userid[MAXUSERS][IDLEN + 1];
        int number;
        time_t uptime;
    };

    struct ACSHM
    {
        char data[ACBOARD_MAXLINE][ACBOARD_BUFSIZE];
        int movielines;
        int movieitems;
        time_t update;
    };

    struct SHM_t
    {
        struct UTMPFILE utmpshm;
        int gap0;
        struct BCACHE bcacheshm;
        int gap1;
        struct UCACHE ucacheshm;
        int gap2;
        struct ACSHM movieshm;
    };
    struct postheader
    {
        char title[STRLEN];
        char ds[40];
        int reply_mode;
        char include_mode;
        int chk_anony;
        int postboard;
    };

/*for muti_msg,by cf*/

    struct msgdir
    {
        char userid[IDLEN + 2];
        time_t tt;
        pid_t pid;
        char msg[MAX_MSG_SIZE];
    };

/* add by rwzmm @ sjtubbs tech group, for new register form.*/
    struct regform
    {
        int usernum;            //idx in PASSWDS
        char date[30];          //for date string
        char userid[IDLEN + 2];
        char realname[NAMELEN]; //realname
        char dept[STRLEN];      //department
        char addr[STRLEN];      //address
        char phone[STRLEN];     //phone number
//      char assoc[STRLEN];             //association, cancel this?
        unsigned char status;
    };
/*added by chaser, for post module*/
    struct module_index
    {
        char filename[NAMELEN];
        char reserved[STRLEN];
    };

    enum login_proto_t
    {
        LOGIN_PROTO_TCP4,
        LOGIN_PROTO_TCP6,
        LOGIN_PROTO_SHELL
    };

    struct newpostdata
    {
        char *name, *title, *BM;
        char flag2[3];          //for the consistent with boardheader
        char flag;
        int pos, total;
        char unread, zap;
        char status;
    };
    struct newgood
    {
        char subdirsfile[80];
        char subdirname[80];
        char boards[80];
        int isdir;              // 1 dir 0 boards
    };

struct anony_item
{
    char userid[IDLEN + 2];
    char ranstr[ANONY_RANSTR_SIZE + 2];
    time_t lastpost_time;
};

struct registdetail
{
    char userid[IDLEN + 1];
    char email[40];
    char gender;
    int bus;
};

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif

#endif
