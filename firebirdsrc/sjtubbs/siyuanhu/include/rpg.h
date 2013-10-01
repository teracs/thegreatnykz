extern int cmpuids ();
extern int t_cmpuids ();
extern struct UTMPFILE *utmpshm;


#define MINTIME   20      //最小攻击间隔
#define MIN_STEAL_TIME   5      //最小攻击间隔
#define RECOVER   1		//每秒恢复的生命力

#define ARCHIVE_ADMIN_A "Kimi"
#define ARCHIVE_ADMIN_B "nilijun"

#define PK_ADMIN_A "pax" 
#define PK_ADMIN_B "SpiritRain" 

#define ARTICLE_ADMIN_A "pax"
#define ARTICLE_ADMIN_B "SpiritRain"

#define PK_DISTANCE   000001//远程攻击

#define Max_TiLi 120 //体力最大
#define Max_TongShuai 80 //体力最大
#define Max_WuLi 80 //体力最大
#define Max_ZhiLi 80 //体力最大
#define Max_MeiLi 80 //体力最大
//#define PK_CHAT       000002//


#define MAGIC_ADMIN_A "pax"
#define MAGIC_ADMIN_B "SpiritRain"


#define FILE_PLACE "place" 
#define FILE_MONSTER "MONSTER" 
#define FILE_PLACE_MONSTER "PLACE_MONSTER" 



char *WuXingName[5]={"金","水","木","火","土"};
char *perp[6]={	"火","水","风","土","圣","暗"};
char *magic_type[3]={"攻击","辅助","治疗"};
char *zz[5]={"人类", "天使", "精灵", "矮人","魔族"};
char *zy[8]={"剑士","拳法家","魔法师","神官","弓箭手","忍者","盗客","贤者"};

static int TianGan[10]={
	0,0,0,0,0,0,0,0,0,0};

static float WuXing[5]={
	0,0,0,0,0};

static float ZhuYue[12][5]={
{1.414,0.500,0.707,1.000,2.000},
{0.898,0.821,1.512,1.348,1.041},
{1.571,1.548,0.924,0.716,0.862},
{2.000,1.414,0.500,0.707,1.000},
{1.166,1.074,1.421,1.161,0.800},
{0.862,1.571,1.548,0.924,0.716},
{0.912,1.700,1.590,0.774,0.645},
{0.924,1.341,1.674,1.069,0.612},
{0.795,0.674,1.012,1.641,1.498},
{0.500,0.707,1.000,2.000,1.414},
{0.674,1.012,1.641,1.498,0.795},
{1.590,0.774,0.645,0.912,1.700}
	};//逐月系数	

static int FenTianGan[10][12]={
{0,0,60,0,0,0,0,0,0,0,0,10},
{0,0,0,100,30,0,0,10,0,0,0,0},
{0,0,30,0,0,60,0,0,0,0,0,0},
{0,0,0,0,0,0,70,30,0,0,10,0},
{0,0,10,0,60,30,0,0,10,0,60,0},
{0,60,0,0,0,0,30,60,0,0,0,0},
{0,0,0,0,0,10,0,0,60,0,0,0},
{0,10,0,0,0,0,0,0,0,100,30,0},
{0,0,0,0,0,0,0,0,30,0,0,70},
{100,30,0,0,10,0,0,0,0,0,0,0}
};


int getitimer(int which,struct itimerval *value); 
//int setitimer(int which,struct itimerval *newval, struct itimerval *oldval); 


#define MAX_MONSTER_PLACE 10
#define MAX_PLACE 100

