# -*- coding: utf8 -*-

from ntbbs.core.storage import BaseStruct

strlen = 80
encpasslen = 14
idlen = 12
namelen = 40
maxfriends = 200
maxrejects = 32
bmlen=60
bdnamelen=16

maxfilterlen=16
maxitem=8

maxmsgsize = 1024


#struct fileheader
#{
#      char filename[STRLEN];      /* the DIR files */
#      char owner[STRLEN-4];
#      unsigned int pnum;//the number of push articles
#      char title[STRLEN];
#      unsigned level;
#      unsigned char accessed[4];
#      unsigned int id;
#      unsigned int reid;              
#};


class Fileheader(BaseStruct):
    __format__ = [
        ('filename', '%ds'%(strlen), None),
        ('owner', '%ds'%(strlen-4), None),
        ('pnum', 'I', 0),
        ('title', '%ds'%(strlen), None),
        ('level', 'I', 0),
        ('accessed', 'I', 0),
        ('id', 'I', None),
        ('reid', 'I', None),
    ]
    size = 256

#struct userec
#{                           /* Structure used to hold information in */
#    char userid[IDLEN + 2]; /* PASSFILE */
#    time_t firstlogin;
#    char lasthost[60];
#    unsigned int numlogins;
#    unsigned int numposts;
#    unsigned int medals;    /*  */
#    unsigned int money;     /*  */
#    unsigned int inbank;    /*  */
#    time_t banktime;        /*  */
#    char flags[2];
##ifdef ENCPASSLEN
#    char passwd[ENCPASSLEN];
##else
#    char passwd[PASSLEN];
##endif
#    char username[NAMELEN];
#    char ident[NAMELEN];
#    char termtype[16];
#    char reginfo[STRLEN - 16];
#    unsigned int userlevel;
#    time_t lastlogin;
#    time_t lastlogout;      /*  */
#    time_t stay;
#    char realname[NAMELEN];
#    char address[STRLEN];
#    char email[STRLEN - 12];
#    unsigned int nummails;
#    time_t lastjustify;
#    char gender;
#    unsigned char birthyear;
#    unsigned char birthmonth;
#    unsigned char birthday;
#    int signature;
#    unsigned int userdefine;
#    time_t notedate;
#    int noteline;
#};

class Userec(BaseStruct):
    __format__ = [
        ('userid', '%ds'%(idlen+2), None),
        ('firstlogin', 'i', None),
        ('lasthost', '60s', None),
        ('numlogins', 'I', 0),
        ('numposts', 'I', 0),
        ('medals', 'I', 0),
        ('money', 'I', 0),
        ('inbank', 'I', 0),
        ('banktime', 'i', 0),
        ('flags', 'H', 0),
        ('passwd', '%ds'%(encpasslen), None),
        ('username', '%ds'%(namelen), ''),
        ('ident', '%ds'%(namelen), ''),
        ('termtype', '16s', 'vt100'),
        ('reginfo', '%ds'%(strlen-16), None),
        ('userlevel', 'I', 0),
        ('lastlogin', 'i', None),
        ('lastlogout', 'i', None),
        ('stay', 'i', None),
        ('realname', '%ds'%(namelen), None),
        ('address', '%ds'%(strlen), None),
        ('email', '%ds'%(strlen-12), None),
        ('nummails', 'I', 0),
        ('lastjustify', 'i', 0),
        ('gender', 'b', None),
        ('birthyear', 'B', None),
        ('birthmonth', 'B', None),
        ('birthday', 'B', None),
        ('signature', 'i', 0),
        ('userdefine', 'I', 0),
        ('notedate', 'i', 0),
        ('noteline', 'i', 0),
    ]
    size = 512

#struct user_info
#{                           /* Structure used in UTMP file */
#    int active;             /* When allocated this field is true */
#    int uid;                /* Used to find user name in passwd file */
#    int pid;                /* kill() to notify user of talk request */
#    int invisible;          /* Used by cloaking function in Xyz menu */
#    int sockactive;         /* Used to coordinate talk requests */
#    int sockaddr;           /* ... */
#    int destuid;            /* talk uses this to identify who called */
#    int mode;               /* UL/DL, Talk Mode, Chat Mode, ... */
#    int pager;              /* pager toggle, YEA, or NA */
#    int in_chat;            /* for in_chat commands   */
#    int fnum;               /* number of friends */
#    int ext_idle;           /* has extended idle time, YEA or NA */
#    time_t logintime;       /* login time */
#    char chatid[10];        /* chat id, if in chat mode */
#    char from[60];          /* machine name the user called in from */
##ifndef BBSD
#    char tty[20];           /* tty port */
##else
#    time_t idle_time;       /* to keep idle time */
##endif
#    char userid[20];
#    char realname[20];
#    char username[NAMELEN];
#    int friends[MAXFRIENDS];
#    int reject[MAXREJECTS];
#    int utmpkey;
#    //add by rwzmm@sjtubbs
#    //lock ac-board while msg.
#    int IsMsg;
#};
class Userinfo(BaseStruct):
    __format__ = [
        ('active', 'i', 1),
        ('uid', 'i', None),
        ('pid', 'i', None),
        ('invisible', 'i', None),
        ('sockactive', 'i', 0),
        ('sockaddr', 'i', 0),
        ('destuid', 'i', 0),
        ('mode', 'i', 0),
        ('pager', 'i', 1),
        ('in_chat', 'i', 0),
        ('fnum', 'i', 0),
        ('ext_idle', 'i', 0),
        ('logintime', 'i', None),
        ('chatid', '10s', ''),
        ('from',  '60s', None),
        ('idle_time', 'i', 0),
        ('userid', '20s', None),
        ('realname', '20s', None),
        ('username', '%ds'%(namelen), None),
        ('friends', 'i', [0]*maxfriends, maxfriends),
        ('reject', 'i', [0]*maxrejects, maxrejects),
        ('utmpkey', 'i', 0),
        ('ismsg', 'i', 0),
    ]
    size = 1144

#struct voteip
#{
#    char ip[48];
#    unsigned int ipnum;     //different user with the same ip can only vote no more than MAXVOTEPERIP
#};

class Voteip(BaseStruct):
    __format__ = [
        ('ip', '48s', ''),
        ('ipnum', 'I', 0),
    ]
    size = 52
#struct boardcollection
#{
#    int subnum;             //the number of subdirectories not includes dirs in extends
#    int boardnum;           //the number of boards
#    int isextends;          //0 stands for being not an extends,1 not
#    char subdirsfile[20][32];       //one dir can have 20 sub dirs, this stores the filename of each subs
#    char subdirname[20][32];        //names
#    char boards[20][32];    //can have 20 boards
#    char dirname[32];       //filename of current dir
#    char updir[32];         //the filename of father dir
#    char extends[32];       //extends of subdirectories.
#};
class Boardcollection(BaseStruct):
    __format__ = [
        ('subnum', 'i', 0),
        ('boardnum', 'i', 0),
        ('isextends', 'i', 0),
        ('subdirsfile', '32s', ['']*20, 20),
        ('subdirname', '32s', ['']*20, 20),
        ('boards', '32s', ['']*20, 20),
        ('dirname', '32s', ''),
        ('updir', '32s', ''),
        ('extends', '32s', '')
    ]
    size = 2028


#struct override
#{
#    char id[13];
#    char exp[40];
#};
class Override(BaseStruct):
    __format__ = [
        ('id','13s',''),
        ('exp', '40s', ''),
    ]
    size = 53

#struct downline
#{
#    char id[13];            // 下线 ID
#    time_t time;            // 注册时间
#    long int last;          // 上次奖励 <= 10000
#    long int total;         // 累计奖励 <= 500000
#    long int money;         // 下线钱数
#    char mode[12];          // 状态描述
#    int num;                // 奖励次数 <= 99
#    int status;             // 帐号状态
#    char unuse[80];
#};
class Downline(BaseStruct):
    __format__ = [
        ('id', '13s', ''),
        ('time', 'i', 0),
        ('last', 'l', 0),
        ('total', 'l', 0),
        ('money', 'l', 0),
        ('mode', '12s', ''),
        ('num', 'i', 0),
        ('status', 'i', 0),
        ('unuse', '80x', 0),
    ]
    size =  132

#struct boardheader
#{                           /* This structure is used to hold data in */
#    //modified by zsong, for subboard
#    char filename[STRLEN - 4];      /* the BOARDS files */
#    int group;
#    //modified end
#    char owner[STRLEN - BM_LEN];
#    //here modified by zsong @ sjtubbs, 20051214
#    //char BM[BM_LEN - 1];
#    char BM[BM_LEN - 4];
#    char flag2[3];
#    //modified end
#    char flag;
#    char title[STRLEN];
#    unsigned level;
#    unsigned char accessed[12];
#};
class Boardheader(BaseStruct):
    __format__ = [
        ('filename', '%ds'%(strlen-4)),
        ('group', 'i', 0),
        ('owner', '%ds'%(strlen-bmlen)),
        ('bm', '%ds'%(bmlen-4)),
        ('flag2', 'b', [0]*3, 3),
        ('flag', 'b', 0),
        ('title', '%ds'%(strlen), ''),
        ('level', 'I', 0),
        ('accessed', 'B', [0]*12, 12),
    ]
    size = 256


#struct shortfile
#{                           /* used for caching files and boards */
#    char filename[STRLEN - 4];      /* also will do for mail directories */
#    int group;              //for sub-board
#    char owner[STRLEN - BM_LEN];
#    //modified by zsong, 20051214
#    char BM[BM_LEN - 4];
#    char flag2[3];
#    //modified end
#    char flag;
#    char title[STRLEN];
#    unsigned level;
#    unsigned char accessed;
#};
class Shortfile(BaseStruct):
    __format__ = [
        ('filename', '%ds'%(strlen-4)),
        ('group', 'i', 0),
        ('owner', '%ds'%(strlen-bmlen)),
        ('bm', '%ds'%(bmlen-4)),
        ('flag2', 'b', [0]*3, 3),
        ('flag', 'b', 0),
        ('title', '%ds'%(strlen), ''),
        ('level', 'I', 0),
        ('accessed', 'B', 0),
        ('pad', '3x'),
    ]
    size = 248

#struct postheader
#{
#    char title[STRLEN];
#    char ds[40];
#    int reply_mode;
#    char include_mode;
#    int chk_anony;
#    int postboard;
#};
class Postheader(BaseStruct):
    __format__ = [
        ('title', '%ds'%(strlen)),
        ('ds', '40s'),
        ('reply_mode', 'i', 0),
        ('include_mode', 'b', 0),
        ('chk_anony', 'i', 0),
        ('postboard', 'i', 0),
    ]
    size = 136


#struct msgdir
#{
#    char userid[IDLEN + 2];
#    time_t tt;
#    pid_t pid;
#    char msg[MAX_MSG_SIZE];
#};
class Msgdir(BaseStruct):
    __format__ = [
        ('userid', '%ds'%(idlen+2)),
        ('tt', 'i'),
        ('pid', 'i'),
        ('msg', '%ds'%(maxmsgsize), ''),
    ]
    size = 1048

#struct regform
#{
#    int usernum;            //idx in PASSWDS
#    char date[30];          //for date string
#    char userid[IDLEN + 2];
#    char realname[NAMELEN]; //realname
#    char dept[STRLEN];      //department
#    char addr[STRLEN];      //address
#    char phone[STRLEN];     //phone number
#    unsigned char status;
#};
class Regform(BaseStruct):
    __format__ = [
        ('usernum', 'i'),
        ('date', '30s'),
        ('userid', '%ds'%(idlen+2)),
        ('realname', '%ds'%(namelen)),
        ('dept', '%ds'%(strlen)),
        ('addr', '%ds'%(strlen)),
        ('phone', '%ds'%(strlen)),
        ('status', 'B', 0),
        ('pad', '3x' ),
    ]
    size = 332

#struct module_index
#{
#    char filename[NAMELEN];
#    char reserved[STRLEN];
#};
class Moduleindex(BaseStruct):
    __format__ = [
        ('filename', '%ds'%(namelen)),
        ('reserved', '%ds'%(strlen)),
    ]
    size = 120

#struct newgood
#{
#    char subdirsfile[80];
#    char subdirname[80];
#    char boards[80];
#    int isdir;              // 1 dir 0 boards
#};
class Newgood(BaseStruct):
    __format__ = [
        ('subdirsfile', '80s'),
        ('subdirname', '80s'),
        ('boards', '80s'),
        ('isdir', 'i'),
    ]
    size = 244

# vim: ts=4 sw=4 sts=4 et:
