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
$Id: xyz.c,v 1.3 2004/07/04 07:09:39 bbs Exp $
*/

#define EXTERN
#include "bbs.h"

#ifdef ALLOWGAME
#include "money.h"
#endif

int use_define = 0;
int child_pid = 0;
extern int iscolor;
extern int enabledbchar;

#ifdef ALLOWSWITCHCODE
extern int switch_code ();
extern int convcode;
#endif

char datestring[30];

extern struct BCACHE *brdshm;

#define TH_LOW	10
#define TH_HIGH	15

/*
万年历 lunar add by pax
*/
#define	BMRow	7	/* number of rows for each bitmap */
#define	BMCol	11	/* number of columns for each bitmap */
#define	NBM	26	/* number of bitmaps */
#define NianBM  0
#define YueBM   1
#define RiBM    2
#define ShiBM   3
#define	GanBM	4
#define	ZhiBM	14
char lunarstr[4096];

char version[] = "Lunar V3.0 (last modified by Siyuanhu ,July 15, 2005)";//pax@sjtubbs 05-07

typedef struct {
int year, month, day, hour, weekday;
int leap;	/* the lunar month is a leap month */
} Date;

typedef char byte;

#include "tables.h"

static int daysInSolarMonth[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int moon[2] = {29,30}; /* a short (long) lunar month has 29 (30) days */

static	char	*GanGB[] = {
    "甲", "乙", "丙", "丁", "戊",
    "己", "庚", "辛", "壬", "癸"
};

static	char	*ZhiGB[] = {
    "子", "丑", "寅", "卯", "辰", "巳",
    "午", "未", "申", "酉", "戌", "亥"
};

static	char   *ShengXiaoGB[] = {
    "鼠", "牛", "虎", "兔", "龙", "蛇",
    "马", "羊", "猴", "鸡", "狗", "猪"
};

static char *weekdayGB[] = {
    "日", "一", "二", "三",
    "四", "五", "六"
};


//国历节日 *表示放假日
static	char	*Festival[12][31] ={
	{"元旦节","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	""
	},//1
	{"","世界湿地日","","","","","","","","国际气象节",
	"","","","情人节","","","","","","",
	"","","","","","","","","","",
	""
	},//2
	
	{
		"国际海豹日","","全国爱耳日","","学雷锋纪念日","","","妇女节","","",
	"","植树节 孙中山逝世纪念日","","国际警察日","消费者权益日","","中国国医节 国际航海日","","","",
	"世界森林日 消除种族歧视国际日 世界儿歌日","世界水日","世界气象日","世界防治结核病日","全国中小学生安全教育日","","","","","巴勒斯坦国土日",
	""},//3


	{
			"愚人节 全国爱国卫生运动月(四月) 税收宣传月(四月)","","","","","","世界卫生日","","","",
	"","","","","","","","","","",
	"","世界地球日","世界图书和版权日","亚非新闻工作者日","","","","","","",
	""
		},//4
	

	{
					"劳动节","劳动节假日","劳动节假日","青年节","碘缺乏病防治日","","","世界红十字日","","",
	"","国际护士节","","","国际家庭日","","国际电信日","国际博物馆日","","全国学生营养日",
	"","","国际牛奶日","","","","","","","",
	"世界无烟日"},//5
	 

	{
				"国际儿童节","","","","世界环境保护日","全国爱眼日","","","","",
	"","","","","","","防治荒漠化和干旱日","","","",
	"","","国际奥林匹克日","","全国土地日","国际禁毒日","","","","",
	""},//6

	{	"香港回归纪念日 中共诞辰 世界建筑日","国际体育记者日","","","","","抗日战争纪念日","","","",
	"世界人口日","","","","","","","","","",
	"","","","","","","","","","非洲妇女日",
	""},//7


	{	"建军节","","","","","","","中国男子节(爸爸节)","","",
	"","","","","抗日战争胜利纪念","","","","","",
	"","","","","","","","","","",
	""},//8
	

	{	"","","","","","","","国际扫盲日 国际新闻工作者日","毛泽东逝世纪念","中国教师节", 
	"","","","世界清洁地球日","","国际臭氧层保护日","","九·一八事变纪念日","","国际爱牙日",
	"","","","","","","世界旅游日","孔子诞辰","","",
	""},//9

	{	"国庆节 世界音乐日 国际老人节","国庆节假日 国际和平与民主自由斗争日","国庆节假日","世界动物日","","老人节","","全国高血压日 世界视觉日","世界邮政日 万国邮联日","辛亥革命纪念日 世界精神卫生日",
	"","","世界保健日 国际教师节","世界标准日","国际盲人节(白手杖节)","世界粮食日","世界消除贫困日","","","",
	"","世界传统医药日","","联合国日","","","","","","",
	"世界勤俭日"},//10

	{	"","","","","","","十月社会主义革命纪念日","中国记者日","全国消防安全宣传教育日","世界青年节",
	"国际科学与和平周(本日所属的一周)","孙中山诞辰纪念日","","世界糖尿病日","","","国际大学生节 世界学生节","","","彝族年",
	"彝族年 世界问候日 世界电视日","彝族年","","感恩节","","","","","国际声援巴勒斯坦人民国际日","",
	""},//11
	{	"世界艾滋病日","","世界残疾人日","","国际经济和社会发展志愿人员日","","","国际儿童电视日","世界足球日","世界人权日",
	"","西安事变纪念日","南京大屠杀(1937年)纪念日","","","","","","","澳门回归纪念",
	"国际篮球日","","","平安夜","圣诞节","毛泽东诞辰纪念","","","","",
	""}//12
};


//农历节日 *表示放假日
static	char	*LunarFestival[12][30] ={
		{"春节","春节","春节","","","","","","","",
	"","","","","元宵节","","","","","",
	"","","","","","","","","",""
	},//1
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//2
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//3
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//4
			{"","","","","端午节","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//5
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","火把节","火把节","火把节","","","",""
	},//6
			{"","","","","","","七夕情人节","","","",
	"","","","","中元节","","","","","",
	"","","","","","","","","",""
	},//7
			{"","","","","","","","","","",
	"","","","","中秋节","","","","","",
	"","","","","","","","","",""
	},//8
			{"","","","","","","","","重阳节","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//9
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//10
			{"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","",""
	},//11

			{"","","","","","","","腊八节","","",
	"","","","","","","","","","",
	"","","小年","","","","","","",""
	}//12
};

/*
//某月的第几个星期几
var wFtv = new Array(
"0150 世界麻风日", //一月的最后一个星期日（月倒数第一个星期日）
"0520 国际母亲节",
"0530 全国助残日",
"0630 父亲节",
"0730 被奴役国家周",
"0932 国际和平日",
"0940 国际聋人节 世界儿童日",
"0950 世界海事日",
"1011 国际住房日",
"1013 国际减轻自然灾害日(减灾日)",
"1144 感恩节")

*/
Date solar, lunar, gan, zhi, gan2, zhi2, lunar2;

int ymonth[Nyear];	/* number of lunar months in the years */
int yday[Nyear];	/* number of lunar days in the years */
int mday[Nmonth+1];	/* number of days in the months of the lunar year */
int jieAlert;		/* if there is uncertainty in JieQi calculation */

int	showHZ = 0;			/* output in hanzi */
int	showBM = 0;			/* output in bitmap */
char	BMfile[] = "lunar.bitmap";	/* bit map file */
char	GZBM[NBM][BMRow][BMCol];	/* the bitmap array */
//char	*progname;

void	Solar2Lunar(), Lunar2Solar();
long    Solar2Day(), Solar2Day1(), Lunar2Day();
void	Day2Lunar(), Day2Solar();
int     make_yday(), make_mday(), GZcycle();
void	CalGZ();
int	JieDate(), JieDate();
void     display3();
void	ReportGB();
void	usage(), Error();
//end by pax

int
modify_user_mode (int mode)
{
    uinfo.mode = mode;
    update_ulist (&uinfo, utmpent);
    return 0;
}

int
showperminfo (int pbits, int i)
{
    char buf[STRLEN];

    sprintf (buf, "%c. %-30s %2s", 'A' + i,
             (use_define) ? user_definestr[i] : permstrings[i],
             ((pbits >> i) & 1 ? "是" : "×"));
    move (i + 6 - ((i > 15) ? 16 : 0), 0 + ((i > 15) ? 40 : 0));
    prints (buf);
    refresh ();
    return YEA;
}

unsigned int
setperms (pbits, prompt, numbers, showfunc)
unsigned int pbits;
char *prompt;
int numbers;
int (*showfunc) ();
{
    int lastperm = numbers - 1;
    int i, done = NA;
    char choice[3], buf[80];

    move (4, 0);
    prints ("请按下你要的代码来设定%s，按 Enter 结束.\n", prompt);
    move (6, 0);
    clrtobot ();
    for (i = 0; i <= lastperm; i++)
    {
        (*showfunc) (pbits, i, NA);
    }
    while (!done)
    {
        sprintf (buf, "选择(ENTER 结束%s): ",
                 ((strcmp (prompt, "权限") != 0)) ? "" : "，0 停权");
        getdata (t_lines - 1, 0, buf, choice, 2, DOECHO, YEA);
        *choice = toupper (*choice);
        if (*choice == '0')
            return (0);
        else if (*choice == '\n' || *choice == '\0')
            done = YEA;
        else if (*choice < 'A' || *choice > 'A' + lastperm)
            bell ();
        else
        {
            i = *choice - 'A';
            pbits ^= (1 << i);
            if ((*showfunc) (pbits, i, YEA) == NA)
            {
                pbits ^= (1 << i);
            }
        }
    }
    return (pbits);
}

int
x_userdefine ()
{
    int id;
    unsigned int newlevel;
	
	/* add by rwzmm @ sjtubbs tech group */
	FILE* fp;
	char homefilepath[STRLEN*2];
	/*add end*/
	
    modify_user_mode (USERDEF);
    if (!(id = getuser (currentuser.userid)))
    {
        move (3, 0);
        prints ("错误的使用者 ID...");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    move (1, 0);
    clrtobot ();
    move (2, 0);
    use_define = 1;
    newlevel =
        setperms (lookupuser.userdefine, "参数", NUMDEFINES, showperminfo);
    move (2, 0);
    if (newlevel == lookupuser.userdefine)
        prints ("参数没有修改...\n");
    else
    {
#ifdef ALLOWSWITCHCODE
        if ((!convcode && !(newlevel & DEF_USEGB))
                || (convcode && (newlevel & DEF_USEGB)))
            switch_code ();
#endif

        lookupuser.userdefine = newlevel;
        currentuser.userdefine = newlevel;
        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);

/* add by rwzmm @ sjtubbs tech group */
		if(lookupuser.userdefine & DEF_NOGMAIL)
		{
			sethomefile(homefilepath, lookupuser.userid, "ALLOW_GMAIL");
			unlink(homefilepath);
		}
		else
		{
			sethomefile(homefilepath, currentuser.userid, "ALLOW_GMAIL");
			fp = fopen(homefilepath, "w");
			if(fp != NULL)
				fclose(fp);			
		}
/*add end*/
		
        uinfo.pager |= FRIEND_PAGER;
        if (!(uinfo.pager & ALL_PAGER))
        {
            if (!DEFINE (DEF_FRIENDCALL))
                uinfo.pager &= ~FRIEND_PAGER;
        }
        uinfo.pager &= ~ALLMSG_PAGER;
        uinfo.pager &= ~FRIENDMSG_PAGER;
        if (DEFINE (DEF_DELDBLCHAR))
            enabledbchar = 1;
        else
            enabledbchar = 0;
        /*uinfo.from[22] = 'S';*/
		uinfo.from[22] = DEFINE(DEF_NOTHIDEIP)?'S':'H';
        if (DEFINE (DEF_FRIENDMSG))
        {
            uinfo.pager |= FRIENDMSG_PAGER;
        }
        if (DEFINE (DEF_ALLMSG))
        {
            uinfo.pager |= ALLMSG_PAGER;
            uinfo.pager |= FRIENDMSG_PAGER;
        }
        update_utmp ();
        prints ("新的参数设定完成...\n\n");
    }
    iscolor = (DEFINE (DEF_COLOR)) ? 1 : 0;
    pressreturn ();
    clear ();
    use_define = 0;
    return 0;
}

int
x_cloak ()
{
    modify_user_mode (GMENU);
    report ("toggle cloak");
    uinfo.invisible = (uinfo.invisible) ? NA : YEA;
    update_utmp ();
    if (!uinfo.in_chat)
    {
        move (1, 0);
        clrtoeol ();
        prints ("隐身术 (cloak) 已经%s了!",
                (uinfo.invisible) ? "启动" : "停止");
        pressreturn ();
    }
    return 0;
}

void
x_edits ()
{
    int aborted;
    char ans[7], buf[STRLEN];
    int ch, num, confirm;
    extern int WishNum;
    static char *e_file[] =
        { "plans", "signatures", "notes", "logout", "GoodWish",
          ".blockmail",
          "autoreply", NULL
        };
    static char *explain_file[] =
        { "个人说明档", "签名档", "自己的备忘录", "离站的画面",
          "底部流动信息",
          "暂停接受站外邮件", "自动回信内容", NULL
        };

    modify_user_mode (GMENU);
    clear ();
    move (1, 0);
    prints ("编修个人档案\n\n");
    for (num = 0; e_file[num] != NULL && explain_file[num] != NULL; num++)
    {
        prints ("[\033[1;32m%d\033[m] %s\n", num + 1, explain_file[num]);
    }
    prints ("[\033[1;32m%d\033[m] 都不想改\n", num + 1);

    getdata (num + 5, 0, "你要编修哪一项个人档案: ", ans, 2, DOECHO, YEA);
    if (ans[0] - '0' <= 0 || ans[0] - '0' > num || ans[0] == '\n'
            || ans[0] == '\0')
        return;

    ch = ans[0] - '0' - 1;
    if (ch != 5)
        setuserfile (genbuf, e_file[ch]);
    else
        setmfile (genbuf, currentuser.userid, e_file[5]);
    move (3, 0);
    clrtobot ();
    sprintf (buf, "(E)编辑 (D)删除 %s? [E]: ", explain_file[ch]);
    getdata (3, 0, buf, ans, 2, DOECHO, YEA);
    if (ans[0] == 'D' || ans[0] == 'd')
    {
        confirm = askyn ("你确定要删除这个档案", NA, NA);
        if (confirm != 1)
        {
            move (5, 0);
            prints ("取消删除行动\n");
            pressreturn ();
            clear ();
            return;
        }
        unlink (genbuf);
        move (5, 0);
        prints ("%s 已删除\n", explain_file[ch]);
        sprintf (buf, "delete %s", explain_file[ch]);
        report (buf);
        pressreturn ();
        if (ch == 4)
            WishNum = 9999;
        clear ();
        return;
    }
    modify_user_mode (EDITUFILE);
    aborted = vedit (genbuf, NA, YEA);
    clear ();
    if (!aborted)
    {
        prints ("%s 更新过\n", explain_file[ch]);
        sprintf (buf, "edit %s", explain_file[ch]);
        if (!strcmp (e_file[ch], "signatures"))
        {
            set_numofsig ();
            prints ("系统重新设定以及读入你的签名档...");
        }
        report (buf);
    }
    else
        prints ("%s 取消修改\n", explain_file[ch]);
    pressreturn ();
    if (ch == 4)
        WishNum = 9999;
}

int
gettheuserid (int x, char *title, int *id)
{
    move (x, 0);
    usercomplete (title, genbuf);
    if (*genbuf == '\0')
    {
        clear ();
        return 0;
    }
    if (!(*id = getuser (genbuf)))
    {
        move (x + 3, 0);
        prints ("错误的使用者代号");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    return 1;
}

int
gettheboardname (x, title, pos, fh, bname)
int x, *pos;
struct boardheader *fh;
char *bname, *title;
{
    extern int cmpbnames ();

    move (x, 0);
    make_blist ();
    namecomplete (title, bname);
    if (*bname == '\0')
    {
        return 0;
    }
    *pos = search_record (BOARDS, fh, sizeof (struct boardheader), cmpbnames,
                          bname);
    if (!(*pos))
    {
        move (x + 3, 0);
        prints ("不正确的讨论区名称");
        pressreturn ();
        clear ();
        return 0;
    }
    return 1;
}

void
x_lockscreen ()
{
    char buf[PASSLEN + 1];
    time_t now;

    modify_user_mode (LOCKSCREEN);
    move (9, 0);
    clrtobot ();
    update_endline ();
    buf[0] = '\0';
    now = time (0);
    getdatestring (now, NA);
    move (9, 0);
    prints ("\033[1;37m");
    prints ("\n       _       _____   ___     _   _   ___     ___       __");
    prints
    ("\n      ( )     (  _  ) (  _`\\  ( ) ( ) (  _`\\  (  _`\\    |  |");
    prints ("\n      | |     | ( ) | | ( (_) | |/'/' | (_(_) | | ) |   |  |");
    prints ("\n      | |  _  | | | | | |  _  | , <   |  _)_  | | | )   |  |");
    prints ("\n      | |_( ) | (_) | | (_( ) | |\\`\\  | (_( ) | |_) |   |==|");
    prints ("\n      (____/' (_____) (____/' (_) (_) (____/' (____/'   |__|\n");

    prints
    ("\n\033[1;36m萤幕已在\033[33m %s\033[36m 时被\033[32m %-12s \033[36m暂时锁住了...\033[m",
     datestring, currentuser.userid);
    while (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
        move (18, 0);
        clrtobot ();
        update_endline ();
        getdata (19, 0, "请输入你的密码以解锁: ", buf, PASSLEN, NOECHO, YEA);
    }
}

heavyload ()
{
#ifndef BBSD
    double cpu_load[3];

    get_load (cpu_load);
    if (cpu_load[0] > 15)
        return 1;
    else
        return 0;
#else
#ifdef chkload

    register int load;
    register time_t uptime;

    if (time (0) > uptime)
    {
        load = chkload (load ? TH_LOW : TH_HIGH);
        uptime = time (0) + load + 45;
    }
    return load;
#else

    return 0;
#endif
#endif
}

//#define MY_DEBUG

void
exec_cmd (int umode, int pager, char *cmdfile, char *param1)
{
    char buf[160];
    char *my_argv[18], *ptr;
    int save_pager, i;

    signal (SIGALRM, SIG_IGN);
    modify_user_mode (umode);
    clear ();
    move (2, 0);
    if ((!HAS_PERM (PERM_EXT_IDLE)) && (num_useshell () > MAX_USESHELL))
    {
        prints ("太多人使用外部程式了，你等一下再用吧...");
        pressanykey ();
        return;
    }
    if (!HAS_PERM (PERM_SYSOP) && heavyload ())
    {
        clear ();
        prints ("抱歉，目前系统负荷过重，此功能暂时不能执行...");
        pressanykey ();
        return;
    }
    if (!dashf (cmdfile))
    {
        prints ("文件 [%s] 不存在！\n", cmdfile);
        pressreturn ();
        return;
    }
    save_pager = uinfo.pager;
    if (pager == NA)
    {
        uinfo.pager = 0;
    }
    sprintf (buf, "%s %d", cmdfile, getpid ());
    report (buf);
    my_argv[0] = cmdfile;
    strcpy (buf, param1);
    if (buf[0] != '\0')
        ptr = strtok (buf, " \t");
    else
        ptr = NULL;
    for (i = 1; i < 18; i++)
    {
        if (ptr)
        {
            my_argv[i] = ptr;
            ptr = strtok (NULL, " \t");
        }
        else
            my_argv[i] = NULL;
    }
#ifdef MY_DEBUG
    for (i = 0; i < 18; i++)
    {
        if (my_argv[i] != NULL)
            prints ("my_argv[%d] = %s\n", i, my_argv[i]);
        else
            prints ("my_argv[%d] = (none)\n", i);
    }
    pressanykey ();
#else

    child_pid = fork ();
    if (child_pid == -1)
    {
        prints ("资源紧缺，fork() 失败，请稍后再使用");
        child_pid = 0;
        pressreturn ();
        return;
    }
    if (child_pid == 0)
    {
        execv (cmdfile, my_argv);
        exit (0);
    }
    else
        waitpid (child_pid, NULL, 0);
#endif

    child_pid = 0;
    uinfo.pager = save_pager;
    clear ();
}

void
x_showuser ()
{
    char buf[STRLEN];

    modify_user_mode (SYSINFO);
    clear ();
    stand_title ("本站使用者资料查询");
    ansimore ("etc/showuser.msg", NA);
    getdata (20, 0, "Parameter: ", buf, 30, DOECHO, YEA);
    if ((buf[0] == '\0') || dashf ("tmp/showuser.result"))
        return;
    securityreport ("查询使用者资料");
    exec_cmd (SYSINFO, YEA, "bin/showuser", buf);
    sprintf (buf, "tmp/showuser.result");
    if (dashf (buf))
    {
        mail_file (buf, currentuser.userid, "使用者资料查询结果");
        unlink (buf);
    }
}

int
ent_sortdir ()
{
    char buf[STRLEN]= {0};
	char sysmsg[256]={0};
	struct boardheader fh;
	int isbm=0;
	char   *ptr;
    char    BMstrbuf[STRLEN - 1];
	extern int cmpbnames ();
	
    clear ();
    stand_title ("板面文章排序");
//    if(!get_a_boardname (buf, "输入讨论区名 (按空白键自动搜寻): "))
//		return 0;
    make_blist();
    namecomplete("输入讨论区名 (按空白键自动搜寻): ", buf);
    if (*buf == '\0')
    {
        return 0;
    }
    if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, buf) <= 0)
    {
        move(1, 0);
        prints("错误的讨论区名称\n");
        pressreturn();
        return 0;
    }
 
    strcpy(BMstrbuf, fh.BM);
    ptr = strtok(BMstrbuf, ",: ;|&()\0\n");
    while (1)
    {
        if (ptr == NULL)
		{
			isbm = 0;
			break;
		}
        if (!strcmp(ptr, currentuser.userid))
		{
			isbm=1;
            break;
		}
        ptr = strtok(NULL, ",: ;|&()\0\n");
    }

	if((!HAS_PERM(PERM_SYSOP)) && (!isbm))
	{
		move(3, 0);
		prints("对不起，你没有权限对此板面排序\n");
		pressreturn();
		return 0;
	}
	sprintf(sysmsg, "给 %s 板排序", buf);
	securityreport(sysmsg);
    exec_cmd (READBRD, YEA, "bin/sortdir", buf);
	move(3,0);
	prints("排序成功\n");
	pressreturn();
}

int
ent_bnet ()
{
    char buf[80];

    sprintf (buf, "etc/bbsnet.ini %s", currentuser.userid);
    exec_cmd (BBSNET, NA, "bin/bbsnet", buf);
}

int
ent_bnet2 ()
{
    char buf[80];

    sprintf (buf, "etc/bbsnet2.ini %s", currentuser.userid);
    exec_cmd (BBSNET, NA, "bin/bbsnet", buf);
}

void
fill_date ()
{
    time_t now, next;
    char *buf, *buf2, *index, index_buf[5], *t = NULL;
    char h[3], m[3], s[3];
    int foo, foo2, i;
    struct tm *mytm;
    FILE *fp;

    now = time (0);
    resolve_boards ();

    if (now < brdshm->fresh_date && strlen (brdshm->date) != 0)
        return;

    mytm = localtime (&now);
    strftime (h, 3, "%H", mytm);
    strftime (m, 3, "%M", mytm);
    strftime (s, 3, "%S", mytm);

    next = (time_t) time (0) - ((atoi (h) * 3600) + (atoi (m) * 60) + atoi (s)) + 86400;	/* 算出今天 0:0:00 的时间, 然後再往後加一天 */

    sprintf (genbuf, "纪念日更新, 下一次更新时间 %s", Cdate (&next));
    report (genbuf);

    buf = (char *) malloc (80);
    buf2 = (char *) malloc (30);
    index = (char *) malloc (5);

    fp = fopen (DEF_FILE, "r");

    if (fp == NULL)
        return;

    now = time (0);
    mytm = localtime (&now);
    strftime (index_buf, 5, "%m%d", mytm);

    strcpy (brdshm->date, DEF_VALUE);

    while (fgets (buf, 80, fp) != NULL)
    {
        if (buf[0] == ';' || buf[0] == '#' || buf[0] == ' ')
            continue;

        buf[35] = '\0';
        strncpy (index, buf, 4);
        index[4] = '\0';
        strcpy (buf2, buf + 5);
        t = strchr (buf2, '\n');
        if (t)
            *t = '\0';

        if (index[0] == '\0' || buf2[0] == '\0')
            continue;

        if (strcmp (index, "0000") == 0 || strcmp (index_buf, index) == 0)
        {
            foo = strlen (buf2);
            foo2 = (30 - foo) / 2;
            strcpy (brdshm->date, "");
            for (i = 0; i < foo2; i++)
                strcat (brdshm->date, " ");
            strcat (brdshm->date, buf2);
            for (i = 0; i < 30 - (foo + foo2); i++)
                strcat (brdshm->date, " ");
        }
    }

    fclose (fp);
    brdshm->fresh_date = next;

    free (buf);
    free (buf2);
    free (index);

    return;
}

int
is_birth (user)
struct userec user;
{
    struct tm *tm;
    time_t now;

    now = time (0);
    tm = localtime (&now);

    if (strcasecmp (user.userid, "guest") == 0)
        return NA;

    if (user.birthmonth == (tm->tm_mon + 1) && user.birthday == tm->tm_mday)
        return YEA;
    else
        return NA;
}

int
sendgoodwish (char *uid)
{
    return sendGoodWish (NULL);
}

int
sendGoodWish (char *userid)
{
    FILE *fp;
    int tuid, i, count, rejects = NA;
    time_t now;
    char buf[5][STRLEN], tmpbuf[STRLEN];
    char uid[IDLEN + 1], *timestr;

    modify_user_mode (GOODWISH);
    clear ();
    move (1, 0);
    prints ("\033[0;1;32m留言本\033[m\n您可以在这里给您的朋友送去您的祝福，");
    prints ("\n也可以为您给他/她捎上一句悄悄话。");
    move (5, 0);
    if (userid == NULL)
    {
        usercomplete ("请输入他的 ID: ", uid);
        if (uid[0] == '\0')
        {
            clear ();
            return 0;
        }
    }
    else
        strcpy (uid, userid);
    if (!(tuid = getuser (uid)))
    {
        move (7, 0);
        prints ("\033[1m您输入的使用者代号( ID )不存在！\033[m\n");
        pressanykey ();
        clear ();
        return -1;
    }
    if (!strcmp (uid, "guest"))
        rejects = YEA;
    if (!rejects)
    {
        struct override fh;
        extern int cmpfnames ();

        sethomefile (tmpbuf, uid, "rejects");
        rejects = search_record (tmpbuf, &fh, sizeof (fh),
                                 cmpfnames, currentuser.userid);
    }
    if (rejects)
    {
        move (7, 0);
        prints ("对不起， %s 不愿意接受您的祝福/留言\n", uid);
        pressanykey ();
        clear ();
        return -1;
    }
    move (5, 0);
    prints ("\033[m【给 \033[1m%s\033[m 留言】\n", uid);
    move (6, 0);
    prints ("您的留言[直接按 ENTER 结束留言，最多 5 句，每句最长 50 字符]:");
    for (count = 0; count < 5; count++)
    {
        getdata (7 + count, 0, ": ", tmpbuf, 51, DOECHO, YEA);
        if (tmpbuf[0] == '\0')
            break;
        ;
        if (killwordsp (tmpbuf) == 0)
        {
            count--;
            continue;
        }
        strcpy (buf[count], tmpbuf);
    }
    if (count == 0)
        return 0;
    sprintf (genbuf, "你确定要发送这条留言给 \033[1m%s\033[m 吗", uid);
    move (9 + count, 0);
    if (askyn (genbuf, YEA, NA) == NA)
    {
        clear ();
        return 0;
    }
    sethomefile (genbuf, uid, "GoodWish");
    if ((fp = fopen (genbuf, "a")) == NULL)
    {
        prints ("无法开启该用户的底部流动信息文件，请通知站长...\n");
        pressanykey ();
        return NA;
    }
    now = time (0);
    timestr = ctime (&now) + 11;
    *(timestr + 5) = '\0';
    for (i = 0; i < count; i++)
    {
        fprintf (fp, "%s(%s)[%d/%d]：%s\n",
                 currentuser.userid, timestr, i + 1, count, buf[i]);
    }
    fclose (fp);
    sethomefile (genbuf, uid, "HaveNewWish");
    if ((fp = fopen (genbuf, "w+")) != NULL)
    {
        fputs ("Have New Wish", fp);
        fclose (fp);
    }
    move (11 + count, 0);
    prints ("已经帮您送出您的留言了。");
    sprintf (genbuf, "SendGoodWish to %s", uid);
    report (genbuf);
    pressanykey ();
    clear ();
    return 0;
}

int
getdatestring (time_t now, int Chang)
{
    struct tm *tm;
    char weeknum[7][3] = { "天", "一", "二", "三", "四", "五", "六" };

    tm = localtime (&now);
    sprintf (datestring, "%4d年%02d月%02d日%02d:%02d:%02d 星期%2s",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
    return (tm->tm_sec % 10);
}

/* Added End. */

#ifdef ALLOWGAME
extern char BoardName[];
downline_title()
{
   if (chkmail()) strcpy(genbuf, "[您有信件]");
   else strcpy(genbuf, BoardName);
   showtitle("[下线名单]", genbuf);
   prints( " [以下是由您介绍而来的下线名单] [\033[1;32m←\033[m,\033[1;32me\033[m] 离开 [\033[1;32mh\033[m] 求助 [\033[1;32m↑\033[m,\033[1;32m↓\033[m] 选择 [\033[1;32md\033[m] 删除下线\n");
    prints("\033[1;44m 编号  下线代号       下线注册时间     上期奖励(元)  期数   累计奖励(元)  状态 \033[m\n");
}

char *downline_doentry(int ent, struct downline *fh)
{
   static char buf[STRLEN];
   getdatestring(fh->time,NA);
   sprintf(buf, " %4d  %-12.12s   %14.14s %9d %9d %11d      %s", 
      ent, fh->id, datestring,fh->last,fh->num,
      fh->total,fh->mode);
   return buf;
}

int downline_help() { 
   show_help("help/downlinehelp"); 
   return FULLUPDATE; 
}

int downline_del(int ent, struct downline *fh, char   *direct)
{
   FILE *fp;
   struct downline dl;
   int     deleted = 0, ssize = sizeof(struct downline);
   char    buf[STRLEN];
   saveline(t_lines - 1, 0);
   sprintf(buf, "是否把【%s】从下线名单中去除", fh->id);
   if (askyn(buf, NA, YEA) == YEA) {
      setuserfile(buf, "downline");
      memset(&dl,0,ssize);
      fp = fopen(buf,"r");
      if(fp){
         fseek(fp,(ent-1)*ssize,SEEK_SET);
	 fread(&dl, ssize, 1, fp);
	 fclose(fp);
      }
      if(dl.time == fh->time){ 
         deleted = 1;
         delete_record(buf, sizeof( struct downline), ent);
      } 
   }
   saveline(t_lines - 1, 1);
   return (deleted) ? PARTUPDATE : DONOTHING;
}

struct one_key downline_list[] = {
   'd', downline_del,
   'h', downline_help,
   'H', downline_help,
   '\0', NULL
};

t_downline() 
{
   char    buf[STRLEN];
   setuserfile(buf, "downline");
   i_read(DOWNLINE,buf,downline_title, downline_doentry, 
       downline_list, sizeof (struct downline));
   clear();
   return;
}

void update_game_money(void)
{
   FILE *fp;
   struct Money mdata;
   char buf[STRLEN];

   setuserfile(buf,"game_money.dat");
   if(dashf(buf)) return;
   fp = fopen(buf,"w+");
   if(fp==NULL) return;
   memset(&mdata,0,sizeof(struct Money));
   strcpy(mdata.id,currentuser.userid);
   mdata.money = currentuser.money;
   mdata.ingot = currentuser.medals;
   if(currentuser.inbank>0){
      mdata.outbank = currentuser.inbank;
      mdata.outime = currentuser.banktime;
   }
   if(!fwrite(&mdata, sizeof(mdata), 1, fp)){
      fclose(fp);
      report("write money.dat error.");
      unlink(buf);
      return;
   }
   fclose(fp);
   currentuser.money = 0;
   currentuser.medals = 0;
   currentuser.inbank = 0;
   currentuser.banktime = 0;
}

void check_downline(void)
{
   FILE *fp,*fpnew;
   struct tm *tm;
   struct stat st;
   struct downline tmp;
   char buf[STRLEN], new[STRLEN];
   time_t now = time(0);
   long int money = (long int) currentuser.money;

   tm = localtime(&now);
   if(tm->tm_mday!=6&&tm->tm_mday!=21) return;
   setuserfile(buf,"downline");
   setuserfile(new,"downline.new");
   if(stat(buf,&st)==-1||st.st_size==0) return;
   if(st.st_ctime/86400 >= now/86400)return;
   if(dashf(new)) unlink(new);
   fp = fopen(buf,"r");
   if(fp == NULL) return ;
   fpnew = fopen(new,"a+");
   if(fpnew == NULL) {
      fclose(fp);
      return;
   }
   #ifndef TIMECOUNTER
   getdatestring(now,NA);
   #else
   getnowdatestring();
   #endif
   while(fread(&tmp,sizeof(tmp),1,fp) > 0 ){
      if(tmp.status < 6 ){
         if(!strcasecmp(tmp.id,currentuser.userid)) continue;
	 tmp.num ++;
         if(tmp.status == 0) {
	    if(getuser(tmp.id)<=0) continue;
	    money += 200;
	    tmp.status = 1;
	    tmp.total = tmp.last = 200;
	    tmp.money = 1000;
	    strcpy(tmp.mode,"小子");
         } else if (getuser(tmp.id) <= 0){
	    if(tmp.status == 1){
	       strcpy(tmp.mode,"夭折");
	       money -= 300;
	       tmp.total -= 300;
	       tmp.last = -300;
	       tmp.status = 7;
	    } else  {
	       strcpy(tmp.mode,"死亡");  
	       money += 100;
	       tmp.total += 100;
	       tmp.last = 100;
	       tmp.status = 8;
            } 
         } else {
	    int oldmoney = tmp.money;
	    int total = lookupuser.money+lookupuser.numposts*5
	               +lookupuser.numlogins*2+lookupuser.medals*1000;
	    tmp.money = total/20;
	    total -= oldmoney*20;
	    if(total < 0 ) total =0;
	    if(total > 200000) total = 200000;
	    total /= 20;
	    tmp.last = total;
	    money += total;
	    tmp.total += total; 
	    if(tmp.total < 2000){
	       tmp.status = 1;
	       strcpy(tmp.mode,"小子");
	    } else if(tmp.total < 80000){
	       tmp.status = 2;
	       strcpy(tmp.mode,"少年");
	    } else if(tmp.total < 200000){
	       tmp.status = 3;
	       strcpy(tmp.mode,"青年");
	    } else if(tmp.total < 450000){
	       tmp.status = 4;
	       strcpy(tmp.mode,"壮年");
	    } else if(tmp.total < 500000){
	       tmp.status = 5;
	       strcpy(tmp.mode,"老年");
	    } else {
	       tmp.status = 6;
	       strcpy(tmp.mode,"永久");
	    }
	    if(tmp.num>=99){
	       tmp.status = 6;
	       strcpy(tmp.mode,"永久");
	    }
	 }
      }
      if(!fwrite(&tmp, sizeof(tmp), 1, fpnew)){
         fclose(fp);
	 fclose(fpnew);
	 report("write downline error.");
	 return;
      }	 
   }   
   if(money<0) currentuser.money =0;
   else if(money>400000000)currentuser.money = 400000000 ;
   else currentuser.money = money;
   fclose(fp);
   fclose(fpnew);
   unlink(buf);
   rename(new,buf);
}
#endif

void anywhere_lockscreen()
{
    char buf[PASSLEN +1];
    time_t now;
    modify_user_mode (LOCKSCREEN);
    buf[0]='\0';
    now = time(0);
    getdatestring(now,NA);
    if (!strcmp ("guest", currentuser.userid))
        return;
    while (*buf=='\0'||!checkpasswd(currentuser.passwd,buf))
    {
        move(t_lines-1,0);
        clrtobot();
        update_endline();
        move(t_lines-1,87);//这里的87和下面的66都是数出来的
        prints("\033[33m[\033[36m锁屏中\033[33m]\033[32m 请输入密码\033[33m:\033[m               ");
        getdata(t_lines-1,66,"",buf,PASSLEN,NOECHO,YEA);
    }
}

int money_fill()
{
	int id;
	clear();
	if(!HAS_PERM(PERM_SYSOP))
		return 0;
	if (!gettheuserid (2, "user id: ", &id))
        return 0;
	lookupuser.money += lookupuser.numposts * 20;
	substitute_record (PASSFILE, &lookupuser, sizeof (struct userec),
                               id);
	return 1;
}

int ent_winmine() { 
     char buf[80];
     sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
     exec_cmd(WINMINE,NA,"so/winmine",buf);
}
int tetris(){
     char buf[80];
     sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
     exec_cmd(TETRIS,NA,"so/tetris",buf);
}
int winmine2() {
     char buf[80];
     sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
     exec_cmd(WINMINE2,NA,"so/winmine2",buf);
}
int x_cal() { 
        char buf[32], filebuf[80]; 
        struct stat statbuf; 
        modify_user_mode(CAL); 
        clear(); 
        refresh(); 
        prints("本程序可以查询公元1--9999年的日历，欢迎使用!如果输入\033[32m89\033[0;1m,您查询的是公元89年;\n"); 
        prints("如果想查询1999年6月的日历，请输入\033[32m6 1999\033[0;1m!!\r\n\r\n"); 
        getdata(2,0,"请输入要查询的月份和年份: ",buf,8,DOECHO,YEA); 
	if(strstr(buf,";")||strstr(buf,"&")) {prints("错误输入");pressanykey();return;}
        sprintf(genbuf, "xcal %s", buf); 
        report(genbuf); 
        sprintf(filebuf, "tmp/%s.cal", currentuser.userid); 
        sprintf(genbuf, "/usr/bin/cal %s > %s", buf, filebuf); 
        system(genbuf); 
        bzero(&statbuf, sizeof(statbuf)); 
        stat(filebuf, &statbuf); 
        if(statbuf.st_size>1) { 
                ansimore(filebuf, YEA); 
                unlink (filebuf); 
        } else {prints("\r\n错误的输入");pressanykey();} 
} 
/*--Lunar-------------------------------------------------------------------------


  		Lunar 2.1: A Calendar Conversion Program
				for
	Gregorian Solar Calendar and Chinese Lunar Calendar
	---------------------------------------------------

	By Fung F. Lee and Ricky Yeung

	Copyright (C) 1988,1989,1991,1992  Fung F. Lee and Ricky Yeung

	All rights reserved.
	Permission to copy and distribute verbatim copies of this
	document for non-commercial purposes is granted, but changing
	it is not allowed.
	There is absolutely no warranty for this program.

	Please send your comments and suggestions to the authors:
	Fung F. Lee	lee@umunhum.stanford.edu
	Ricky Yeung	Ricky.Yeung@Eng.Sun.Com
	
	The special "bitmap" file "lunar.bitmap" was contributed
	by Weimin Liu (weimin@alpha.ece.jhu.edu).

	Special thanks to Hwei Chen Ti (chetihc@nuscc.nus.sg or
	CHETIHC@NUSVM.Bitnet) who extended the tables from 2001 to 2049. 

----------------------------------------------------------------------------*/

/*
This document contains Highest-bit-set GuoBiao (HGB) code, as adopted by
CCDOS on IBM PC compatibles, ChineseTalk 6.0 (GB version) on Macintosh,
and cxterm on UNIX and X window.  Hence, one may need to transfer this 
document as a **binary** file.

References:
1. "Zhong1guo2 yin1yang2 ri4yue4 dui4zhao4 wan4nian2li4" by Lin2 Qi3yuan2.
   《中国阴阳日月对照万年历》．林
2. "Ming4li3 ge2xin1 zi3ping2 cui4yan2" by Xu2 Le4wu2.
   《命理革新子平粹言》．徐
3. Da1zhong4 wan4nian2li4.
   《大众万年历》
*/ 
/* "Bitmap" constants */




int
lunar_syh( )
{
	char years[5];
	char months[3];
	char days[3];
	char hours[3];
    int year, month, day, hour/*, i, k, option, inverse=0, leap=0*/;
/*    int dateInfo[4];*/
    
   // progname = "lunar";
	
        modify_user_mode(CAL); 
        clear(); 
        refresh(); 
	
		getdata(2, 0, "请输入年份 [1900 - 2050] :", years, 5, DOECHO, YEA);
		year = atoi(years);

		if (!isdigit(years[0]) || year <= 1900 || year > 2050 )
		{
			move( 4, 0);
			prints("对不起，年份必须在[1900 - 2050]之间！\n");
			pressanykey();
			return 0;
		}

		getdata(3, 0, "请输入月份 [1 - 12] :", months, 3, DOECHO, YEA);
		month = atoi(months);

		if (!isdigit(months[0]) || month <= 0 || month > 12)
		{
			move( 5, 0);
			prints("对不起，月份必须在[1 - 12]之间！\n");
			pressanykey();
			return 0;
		}

		getdata(4, 0, "请输入日期 [1 - 31] :", days, 3, DOECHO, YEA);
		
		day = atoi(days);
		if (!isdigit(days[0]) || day <= 0 || day > 31 )
		{
			move( 6, 0);
			prints("对不起，日期必须在[1 - 31]之间！\n");
			pressanykey();
			return 0;
		}

//判断日期是否合法
		if ((month == 4 || month  ==  6 ||month  ==  9 ||month ==  11 )&&(day == 31))
		{
			move( 6, 0);
			prints("对不起，输入日期有误！\n");
			pressanykey();
			return 0;
		}

		if ((month  ==  2 && (year%4==0)&&(day>29))||(month==2 && (year % 4!=0)&&(day>28)))
		{
			move( 6, 0);
			prints("对不起，输入日期有误！\n");
			pressanykey();
			return 0;
		}

		hour=0;
		getdata(5, 0, "请输入小时 [0 - 23] :", hours, 3, DOECHO, YEA);
		hour = atoi(hours);
		if (!isdigit(hours[0]) )
		{
			move( 7, 0);
			prints("对不起，小时必须在[0 - 23]之间！\n");
			pressanykey();
			return 0;
		}

 


	solar.year = year;
	solar.month = month;
	solar.day = day;
	solar.hour = hour;
	Solar2Lunar();
    ReportGB();
	pressanykey();
	return 0;
}


void Solar2Lunar()
{

    long offset;
//    Date *d;

    offset = Solar2Day(&solar);
    solar.weekday = (offset + SolarFirstDate.weekday) % 7;
    
    /* A lunar day begins at 11 p.m. */
    if (solar.hour == 23)
	offset++;

    Day2Lunar(offset, &lunar);
    lunar.hour = solar.hour;
    CalGZ(offset, &lunar, &gan, &zhi);

    jieAlert = JieDate(&solar, &lunar2);
    lunar2.day = lunar.day;
    lunar2.hour = lunar.hour;
    CalGZ(offset, &lunar2, &gan2, &zhi2);
}


void Lunar2Solar()
{
    long offset;
    int adj;
//    Date *d;

    /* A solar day begins at 12 a.m. */
    adj = (lunar.hour == 23)? -1 : 0;
    offset = Lunar2Day(&lunar);
    solar.weekday = (offset+ adj + SolarFirstDate.weekday) % 7;
    Day2Solar(offset + adj, &solar);
    solar.hour = lunar.hour;
    CalGZ(offset, &lunar, &gan, &zhi);

    jieAlert = JieDate(&solar, &lunar2);
    lunar2.day = lunar.day;
    lunar2.hour = lunar.hour;
    CalGZ(offset, &lunar2, &gan2, &zhi2);
}


#define	LeapYear(y)	(((y)%4==0) && ((y)%100!=0) || ((y)%400==0))
#define BYEAR		1201
/* BYEAR % 4 == 1  and BYEAR % 400 == 1 for easy calculation of leap years */
/* assert(BYEAR <= SolarFirstDate.year) */

long Solar2Day(d)
Date *d;
{
    return (Solar2Day1(d) - Solar2Day1(&SolarFirstDate));
}


/* Compute the number of days from the Solar date BYEAR.1.1 */
long Solar2Day1(d)
Date *d;
{
    long offset, delta;
    int i;

    delta = d->year - BYEAR;
    if (delta<0) Error("Internal error: pick a larger constant for BYEAR.");
    offset = delta * 365 + delta / 4 - delta / 100 + delta / 400;
    for (i=1; i< d->month; i++)
    	offset += daysInSolarMonth[i];
    if ((d->month > 2) && LeapYear(d->year))
	offset++;
    offset += d->day - 1;

    if ((d->month == 2) && LeapYear(d->year))
    {
	if (d->day > 29) Error("Day out of range.");
    }
    else if (d->day > daysInSolarMonth[d->month]) Error("Day out of range.");
    return offset;
}


/* Compute offset days of a lunar date from the beginning of the table */
long Lunar2Day(d)
Date *d;
{
    long offset = 0;
    int year, i, m, nYear, leapMonth;

    nYear = make_yday();
    year = d->year - LunarFirstDate.year;
    for (i=0; i<year; i++)
	offset += yday[i];

    leapMonth = make_mday(year);
    if ((d->leap) && (leapMonth!=d->month))
    {
	printf("%d is not a leap month in year %d.\n", d->month, d->year);
	exit(1);
    }
    for (m=1; m<d->month; m++)
	offset += mday[m];
    if (leapMonth && 
	((d->month>leapMonth) || (d->leap && (d->month==leapMonth))))
	offset += mday[m++];
    offset += d->day - 1;

    if (d->day > mday[m]) Error("Day out of range.");

    return offset;
}


void Day2Lunar(offset, d)
long offset;
Date *d;

{
    int i, m, nYear, leapMonth;
    
    nYear = make_yday();
    for (i=0; i<nYear && offset > 0; i++)
	offset -= yday[i];
    if (offset<0)
	offset += yday[--i];
    if (i==Nyear) Error("Year out of range.");
    d->year = i + LunarFirstDate.year;

    leapMonth = make_mday(i);
    for (m=1; m<=Nmonth && offset>0; m++)
	offset -= mday[m];
    if (offset<0)
	offset += mday[--m];

    d->leap = 0;	/* don't know leap or not yet */

    if (leapMonth>0)	/* has leap month */
    {
	/* if preceeding month number is the leap month,
	   this month is the actual extra leap month */
	d->leap = (leapMonth == (m - 1));

	/* month > leapMonth is off by 1, so adjust it */
	if (m > leapMonth) --m;
    }

    d->month = m;
    d->day = offset + 1;
}


void Day2Solar(offset, d)
long offset;
Date *d;
{
    int	i, m, days;

    /* offset is the number of days from SolarFirstDate */
    offset -= Solar2Day(&LunarFirstDate);  /* the argument is negative */
    /* offset is now the number of days from SolarFirstDate.year.1.1 */

    for (i=SolarFirstDate.year;
         (i<SolarFirstDate.year+Nyear) && (offset > 0);	 i++) 
	offset -= 365 + LeapYear(i);
    if (offset<0)
    {
	--i; 	/* LeapYear is a macro */
	offset += 365 + LeapYear(i);
    }
    if (i==(SolarFirstDate.year + Nyear)) Error("Year out of range.");
    d->year = i;
    
    /* assert(offset<(365+LeapYear(i))); */
    for (m=1; m<=12; m++)
    {
	days = daysInSolarMonth[m];
	if ((m==2) && LeapYear(i))	/* leap February */
	    days++;
	if (offset<days)
	{
	    d->month = m;
	    d->day = offset + 1;
	    return;
	}
	offset -= days;
    }
}


int GZcycle(g, z)
int g, z;
{
    int gz;

    for (gz = z; gz % 10 != g && gz < 60; gz += 12) ;
    if (gz >= 60) printf("internal error\n");
    return gz+1;
}
	     

void CalGZ(offset, d, g, z)
long offset;
Date *d, *g, *z;
{
    int	year, month;
        
    year = d->year - LunarFirstDate.year;
    month = year * 12 + d->month - 1;   /* leap months do not count */

    g->year = (GanFirstDate.year + year) % 10;
    z->year = (ZhiFirstDate.year + year) % 12;
    g->month = (GanFirstDate.month + month) % 10;
    z->month = (ZhiFirstDate.month + month) % 12;
    g->day = (GanFirstDate.day + offset) % 10;
    z->day = (ZhiFirstDate.day + offset) % 12;
    z->hour = ((d->hour + 1) / 2) % 12;
    g->hour = (g->day * 12 + z->hour) % 10;
}


void Error(s)
char	*s;
{
    printf("%s\n",s);
    exit(1);
}


/* Compare two dates and return <,=,> 0 if the 1st is <,=,> the 2nd */
int CmpDate(month1, day1, month2, day2)
int month1, day1, month2, day2;
{
    if (month1!=month2) return(month1-month2);
    if (day1!=day2) return(day1-day2);
    return(0);
}


/*
   Given a solar date, find the "lunar" date for the purpose of
   calculating the "4-columns" by taking jie into consideration.
*/
int JieDate(ds, dl)
Date *ds, *dl;
{
    int m, flag;

    if (ds->month==1)
    {
	flag = CmpDate(ds->month, ds->day,
		       1, fest[ds->year - SolarFirstDate.year - 1][11]);
	if (flag<0) dl->month = 11;
	else if (flag>0) dl->month = 12;
	dl->year = ds->year - 1;
	return(flag==0);
    }
    for (m=2; m<=12; m++)
    {
	flag = CmpDate(ds->month, ds->day,
		       m, fest[ds->year - SolarFirstDate.year][m-2]);
	if (flag==0) m++;
	if (flag<=0) break;
    }
    dl->month = (m-2) % 12;
    dl->year = ds->year;
    if ((dl->month)==0)
    {
	dl->year = ds->year - 1;
	dl->month = 12;
    }
    return(flag==0);
}


/* Compute the number of days in each lunar year in the table */
int make_yday()
{
    int year, i, leap;
    long code;
    
    for (year = 0; year < Nyear; year++)
    {
	code = yearInfo[year];
	leap = code & 0xf;
	yday[year] = 0;
	if (leap != 0)
	{
	    i = (code >> 16) & 0x1;
	    yday[year] += moon[i];
	}
	code >>= 4;
	for (i = 0; i < Nmonth-1; i++)
	{
	    yday[year] += moon[code&0x1];
	    code >>= 1;
	}
	ymonth[year] = 12;
	if (leap != 0) ymonth[year]++;
    }
    return Nyear;
}


/* Compute the days of each month in the given lunar year */
int make_mday(year)
int year;
{
    int i, leapMonth;
    long code;
    
    code = yearInfo[year];
    leapMonth = code & 0xf;
    /* leapMonth == 0 means no leap month */
    code >>= 4;
    if (leapMonth == 0)
    {
	mday[Nmonth] = 0;
	for (i = Nmonth-1; i >= 1; i--)
	{
	    mday[i] = moon[code&0x1];
	    code >>= 1;
	}
    }
    else
    {
	i = (yearInfo[year] >> 16) & 0x1;
	mday[leapMonth+1] = moon[i];
	for (i = Nmonth; i >= 1; i--)
	{
	    if (i == leapMonth+1) i--;
	    mday[i] = moon[code&0x1];
	    code >>= 1;
	}
    }
    return leapMonth;
}




void ReportGB()
{
	sprintf(lunarstr,"%s%d%s%2d%s%2d%s%2d%s%s%s\n", "阳历：　",
		solar.year, "年", solar.month, "月", solar.day,
		"日", solar.hour, "时　",
		"星期", weekdayGB[solar.weekday]); 
	prints(lunarstr);
    sprintf(lunarstr,"%s%d%s%s%2d%s%2d%s%s%s%s%s\n", "阴历：　",
		lunar.year, "年", (lunar.leap? "闰":""),
		lunar.month, "月", lunar.day, "日", 
		ZhiGB[zhi.hour], "时　",
		"生肖属", ShengXiaoGB[zhi.year]);
	prints(lunarstr);
    sprintf(lunarstr,"%s%s%s%s%s%s%s%s%s%s%s%s%s\n", "干支：　",
		GanGB[gan.year], ZhiGB[zhi.year], "年　",
		GanGB[gan.month], ZhiGB[zhi.month], "月　",
		GanGB[gan.day], ZhiGB[zhi.day], "日　",
		GanGB[gan.hour], ZhiGB[zhi.hour], "时　");
	prints(lunarstr);
	sprintf(lunarstr,"%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		"用四柱神算推算之时辰八字：　",
		GanGB[gan2.year], ZhiGB[zhi2.year], "年　",
		GanGB[gan2.month], ZhiGB[zhi2.month], "月　",
		GanGB[gan2.day], ZhiGB[zhi2.day], "日　",
		GanGB[gan2.hour], ZhiGB[zhi2.hour], "时　");
	prints(lunarstr);
	sprintf(lunarstr,"%s\n",Festival[solar.month-1][solar.day-1] );
	prints(lunarstr);
	sprintf(lunarstr,"%s\n",LunarFestival[lunar.month-1][lunar.day-1] );
	prints(lunarstr);
}

