/********************************************************************/
/*  deardragon.bbs@fb2000.dhs.org              zjinglong@163.net    */
/*  关于金钱和游戏的相关结构 money.h           2000.12.16           */
/********************************************************************/
/* money.h */
#ifndef  _MONEY_H_
#define _MONEY_H_

#define MAXBET  10
#define MAXCARD  10

struct Bet {
   char id[16];
   long int bet;
   time_t time;
   float x;
};   

struct Card {
   char id[16];			// 主卡人 ID ( 用于子母卡 )
   char card_No[16];		// 信用卡卡号
   int	status;			// 状态 ( 包括：无效、停用、母卡、子卡 )
   char passwd[8];		// 信用卡密码 7 位有效
   long int money;		// 金钱数目
   long int money1;		// 金钱数目1
   long int money2;		// 金钱数目2
   long int money3;		// 金钱数目3
   time_t valid;		// 信用卡到期时间
   char info[32];		// 用户备忘说明
   char unuse[80];		
};   

struct Money {
   char id[16];			// 用户名
   long int money;		// 赌金
   long int ingot;		// 赌注金条
   long int inbank;		// 存入银行的金条数
   time_t  intime;		// 存款到期时间
   long int outbank;		// 银行贷款(赌金)
   time_t  outime;		// 还贷时间
   int betnum;			// 私人借贷记录数
   int cardnum;			// 信用卡数目
   struct Bet bet[MAXBET];	// 私人借贷记录
   struct Card card[MAXCARD];	// 信用卡
   char unuse[80];
};   
typedef struct exchanges {
   char title[STRLEN];
   int  value;
   char information[STRLEN];
} EXCHANGES;

#endif
