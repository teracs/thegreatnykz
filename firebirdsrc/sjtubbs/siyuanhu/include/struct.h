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

typedef unsigned char uschar;
struct userec
{                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    time_t firstlogin;
    char lasthost[16];
    unsigned int numlogins;
    unsigned int numposts;
    unsigned int medals;        /* 奖章数 */
    unsigned int money;         /* 金钱 */
    unsigned int inbank;        /* 存款 */
    time_t banktime;            /* 存入时间 */
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
    time_t lastlogout;          /* 最近离线时间 */
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
{              /* Structure used in UTMP file */
    int active;                 /* When allocated this field is true */
    int uid;                    /* Used to find user name in passwd file */
    int pid;                    /* kill() to notify user of talk request */
    int invisible;              /* Used by cloaking function in Xyz menu */
    int sockactive;             /* Used to coordinate talk requests */
    int sockaddr;               /* ... */
    int destuid;                /* talk uses this to identify who called */
    int mode;                   /* UL/DL, Talk Mode, Chat Mode, ... */
    int pager;                  /* pager toggle, YEA, or NA */
    int in_chat;                /* for in_chat commands   */
    int fnum;                   /* number of friends */
    int ext_idle;               /* has extended idle time, YEA or NA */
    char chatid[10];            /* chat id, if in chat mode */
    char from[60];              /* machine name the user called in from */
#ifndef BBSD
    char tty[20];               /* tty port */
#else
    time_t idle_time;           /* to keep idle time */
#endif
    char userid[20];
    char realname[20];
    char username[NAMELEN];
    unsigned /*short*/int friend[MAXFRIENDS];
    unsigned /*short*/int reject[MAXREJECTS];
    int utmpkey;
    //add by rwzmm@sjtubbs
    //lock ac-board while msg.
    int IsMsg;
};

//added by kongyang 2005
//the following used in b_vote to store ip information
struct voteip
{
	char ip[16];
	unsigned int ipnum;    //different user with the same ip can only vote no more than MAXVOTEPERIP
};
//added by kongyang 2006-06-28
struct boardcollection
{
	int subnum;//the number of subdirectories not includes dirs in extends
	int boardnum;//the number of boards
	int isextends;//0 stands for being not an extends,1 not
	char subdirsfile[20][32];//one dir can have 20 sub dirs, this stores the filename of each subs
	char subdirname[20][32];//names
	char boards[20][32];//can have 20 boards
	char dirname[32];//filename of current dir
	char updir[32];//the filename of father dir
	char extends[32];//extends of subdirectories.
};


struct override
{
    char id[13];
    char exp[40];
};

struct downline
{
    char id[13];                // 下线 ID
    time_t time;                // 注册时间
    long int last;              // 上次奖励 <= 10000
    long int total;             // 累计奖励 <= 500000
    long int money;             // 下线钱数
    char mode[12];              // 状态描述
    int num;                    // 奖励次数 <= 99
    int status;                 // 帐号状态
    char unuse[80];
};


#define BM_LEN 60
#define BDNAME_LEN 16



struct boardheader
{            /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char owner[STRLEN - BM_LEN];
    char BM[BM_LEN - 1];
    char flag;
    char title[STRLEN];
    unsigned level;
	unsigned char accessed[12];
	//modified by rwzmm @ sjtubbs
//	unsigned char accessed[4];
//	unsigned int id;
//	unsigned int reid;
		//re id. add by rwzmm @ sjtubbs.
};



struct fileheader
{             /* This structure is used to hold data in */
    char filename[STRLEN];      /* the DIR files */
    char owner[STRLEN];
    char title[STRLEN];
    unsigned level;
//    unsigned char accessed[12]; /* struct size = 256 bytes */
//modified by rwzmm @ sjtubbs
	unsigned char accessed[4];
	unsigned int id;
	unsigned int reid;		//re id. add by rwzmm @ sjtubbs.
};



struct shortfile

{              /* used for caching files and boards */
    char filename[STRLEN];      /* also will do for mail directories */
    char owner[STRLEN - BM_LEN];
    char BM[BM_LEN - 1];
    char flag;
    char title[STRLEN];
    unsigned level;
    unsigned char accessed;
};



struct one_key
{                /* Used to pass commands to the readmenu */
    int key;
    int (*fptr) ();
};


#define USHM_SIZE       (MAXACTIVE + 10)
#define SESS_MAGIC_NUMBER 5555 

struct http_session  //add by jawahh, sjtubbs tech master
{
	int magic; /*magic number, if httpsession watcher start, it will be set to 5555; when session watcher stop, it will be set to 0 */
	pid_t pid;/*http session watcher's pid*/
};

struct UTMPFILE
{
    struct user_info uinfo[USHM_SIZE];
    time_t uptime;
	struct http_session sess; 
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
    time_t friendbook;          /* friendbook mtime */
    int         inboard[MAXBOARD];
};

struct UCACHE
{
    char userid[MAXUSERS][IDLEN + 1];
    int number;
    time_t uptime;
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
	long  usernum;			//idx in PASSWDS
	char date[30];			//for date string
	char userid[IDLEN + 2];
	char realname[NAMELEN];		//realname
	char dept[STRLEN];		//department
	char addr[STRLEN];		//address
	char phone[STRLEN];			//phone number
//	char assoc[STRLEN];		//association, cancel this?
	unsigned char status;
};


/* add by pax @ sjtubbs for the time deposit*/
struct bankbill
{
	long  usernum;			//idx 
	char userid[IDLEN + 2];
	long int  datein;			//for date in
	int  money;		//money
	long int  dateout;			//for date out
	int  length;//存款时间
	char reserved[16];
};

struct callbill
{
	long int  date;			//最后更新的日期
	long int  principal;	//本金
	double interest;		//利息
};
struct rate
{
double timedepo_01;//利息
double timedepo_02;//利息
double timedepo_03;//利息
double timedepo_04;//利息
double timedepo_05;//利息
double timedepo_06;//利息
double timedepo_07;//利息
double timedepo_08;//利息
double timedepo_09;//利息
double timedepo_10;//利息
double timedepo_11;//利息
double timedepo_12;//利息
double currdepo;//活期
long int time;//制定利息的时间
}; 

struct rpg
{	
	int STR;
	int DEF;
	int DEX;
	int STA;
	int WIL;
	int WIS;
	int zz;
	int zy;
	int per;
	int att;//功
	int def;//防守
	int damage;//伤害 
	int block;//隔档
	int power;//魔法强度
	int life;//生命力 HP
	int magic;//魔法 MP
	int lifemax;//max HP
	int magicmax;//max 
	int arti;
	int magi;
	int antifire;
	int antiwater;
	int antiwind;
	int antiearth;
	int antilight;
	int antidark;
	int level;
	long int exp;
	char memo[24];//备用
};

struct article
{
	int type;//宝物种类 弓箭 刀等
	char name[20];//名称
	char creator[IDLEN + 2];
	int att;//功
	int def;//防守
	int damage;//伤害 
	int block;//隔档
	int power;//魔法强度
	int lifemax;//max HP
	int magicmax;//max  //各项指数的增加值 超过256的就是 (x-256)/100按照比例增加
	int antifire;
	int antiwater;
	int antiwind;
	int antiearth;
	int antilight;
	int antidark;
	int level;
	long int exp;
	char memo[80];//说明文字
};

struct magi
{
	char name[20];//名称
	char creator[IDLEN + 2];
	int type;//攻击 治疗 辅助
	int basehpdamage;//基本伤害/治疗
	int plushpdamage;//每升一级的附加伤害/治疗
	int basempdamage;//基本伤害/治疗
	int plusmpdamage;//每升一级的附加伤害/治疗
	int element;//哪种属性的魔法
	int requestMP;//对魔法值的消耗
	int requestHP;//对魔法值的消耗
};

struct pkrecord
{
	char id_a[IDLEN + 2];//对方ID
	char id_b[IDLEN + 2];//自己的ID
	int damage;
	long int date;
	int mode;//0 攻击 1 被攻击 2 格挡 3 击败 4 被击败
};



struct relative
{
char userid[IDLEN + 2]; //对象ID
char relation[6];//关系
int regonizedate;//认证日期
char memo[8];//备用
};


struct house
{
	char ida[IDLEN + 2]; 
	char idb[IDLEN + 2]; 
	char num[4];
	long int price;//房价
	long int payed;//已经支付
	double interest;//利息
	double rate;//利率
	long int datebuy;//购买日期
	long int lastpay;//最后一次付款
};

/*pax@syhbbs 05-09026*/
/*礼物显示*/
struct gift
{
	char name[30];//礼物名称
	char id[IDLEN+2];//赠送者
	char memo[256];// 备注
};



struct person
{	
	int LiT;		//体力
	int LiN;		//内力
	int LiS;		//生命力
	int LTong;		//统率
	int LWu;		//武力
	int LZhi;		//智力
	int LMei;		//魅力
	int LiTMax;		//体力最大
	int LiNMax;		//内力最大
	int LiSMax;		//生命力最大
	int LTongMax;	//统率最大
	int LWuMax;		//武力最大
	int LZhiMax;	//智力最大
	int LMeiMax;	//魅力最大
	int spe;		//特技
	int block;
	int pro;//职业
	int J;//Jin
	int M;
	int S;
	int H;
	int T;//Tu
	int antiJ;
	int antiM;
	int antiS;
	int antiH;
	int antiT;
	int antiMagi;
	int level;
	long int exp;
	char memo[120];//备用
};


struct city
{	
	int food;		//粮食
	int foodin;		//粮食
	int foodout;	//粮食
	int money;		//钱
	int moneyin;	//钱
	int moneyout;	//钱
	int population;	//人口
	int escuage;	//兵役人口
	int peace;		//治安
	int wall;		//城墙
	int wallmax;
	int level;		//城市等级
	int moral;		//士气
	int numSpear;		//矛
	int numSpearMax;	//矛
	int numHalberd;		//戟
	int numHalberdMax;	//戟
	int numArrow;		//箭
	int numArrowMax;	//箭
	int numHorse;		//马
	int numHorseMax;	//马	
	char memo[48];//备用
};

struct arm
{	
	int att;		//攻击
	int def;		//防御
	int inte;		//智力
	int food;		//粮食
	int num;		//人数
	int nummax;		//最多人数
	int numhurt;	//伤病人数
	int energy;		//气力
	int energymax;	//最多气力
	int type;		//兵种
	int array;		//阵型
	int moral;		//士气
	int level;		//城市等级
	char memo[48];//备用
};



struct mud_monster
{	
	int id;//编号
	char name_chn[20];
	char name_eng[40];
	char shortName[10];
	int health;
	int att;
	int def;
	int type;
	int d_base;	
	int d_num;
	int d_face;	
	int spe;		//特技
	int block;
	int J;//Jin
	int M;
	int S;
	int H;
	int T;//Tu
	int antiJ;
	int antiM;
	int antiS;
	int antiH;
	int antiT;
	int antiMagi;
	int level;
	long int exp;
	char memo[120];//备用
};

struct mud_place
{	
	int id;//编号
	char name_chn[32];
	char name_eng[32];
	char shortName[8];
	int E;
	int S;
	int W;
	int N;
	int ES;
	int WS;
	int WN;
	int EN;	
	int type;//0-place 1-building
	char memo[128];//备用
};


struct mud_place_monster
{
	int id;
	int id_place;
	int id_monster;
	int rate;
	int time_start;
	int time_end;
	int block;
	char memo[64];
};

/*struct itimerval { 
struct timeval it_interval; 
struct timeval it_value; 
} ;*/

