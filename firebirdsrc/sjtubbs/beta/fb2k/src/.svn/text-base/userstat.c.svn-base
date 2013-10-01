#include "bbs.h"

/********************************
 *	this file is used to stat more user info
 *	created by Jerry Zhou
 *	2005/10/16 20:27
 *********************************/

//the switch is used to show debug info.
//#define DEBUG_STAT

//the return value of the function checkip
#define FLAG_XUHUI				1
#define FLAG_MINHANG			2
#define FLAG_UNUSE_CAMPUS		3
#define FLAG_OUT_CAMPUS		0
#define FLAG_UNKOWN			10

//the data below is from the board SJTUNET(2005/10/17)
//the distributed of the IP of SJTU  
//1.   202.120.0   		~	 202.120.63    	64¸öC
//2.   202.120.128 	~	 202.120.143   	16¸öC(Ò½Ñ§Ôº)
//3.   211.80.32   		~	 211.80.63     		32¸öC
//4.   211.80.80  		~	 211.80.95     		16¸öC(ãÉÐÐ)
//5.   59.78.0     		~	 59.78.63      		64¸öC
//6.   219.228.96 		~	 219.228.127   	32¸öC
//7.   218.193.176		~	 218.193.191   	16¸öC
//8.   202.121.176 	~	 202.121.183    	8¸öC(Æß±¦,Å©Ñ§Ôº)
//9.   202.112.26.0	       				1¸öC(ºÆÈ»)

//202.120.0-35 xuhui
//202.120.36-63 minhang
//211.80.32-53 xh (´óÌå)
//211.80.54-63 mh
//218.193 mh
//59.78 mh
//219.228 mh

static IPTABLES currentIptable = 
{{
	{0xCA788000,	0xFFFFF000,	{0,			0},			'U',	NA},		//202.120.128~143(16) Medical School
	{0xD3505000,	0xFFFFF000,	{0,			0},			'M',	NA},		//211.80.80~95(32)
	{0x3B4E0000,	0xFFFFC000,	{0,			0},			'M',	NA},		//59.78.0~63(64)
	{0xDBE46000,	0xFFFFE000,	{0,			0},			'M',	NA},		//219.228.96~127(32)
	{0xDAC1B000,	0xFFFFF000,	{0,			0},			'M',	NA},		//218.193.176~191(16)
	{0xCA701A00,	0xFFFFFF00, {0,			0},			'X',	NA},		//202.112.26.0(1)
	{0xCA79B000,	0xFFFFF800,	{0,			0},			'U',	NA},		//202.121.176~183(8) Agriculture School	
	{0xCA780000,	0xFFFF0000,	{0xCA780000,	0xCA7823FF},	'X',	YEA},	//202.120.0~35(36)
	{0xCA780000,	0xFFFF0000,	{0xCA782400,	0xCA783FFF},	'M',	YEA},	//202.120.36~63(28)	
	{0xD3500000,	0xFFFF0000,	{0xD3502000,	0xD35035FF},	'X',	YEA},	//211.80.32~53(12)
	{0xD3503000,	0xFFFFF000,	{0xD3503600,	0xD3503FFF},	'M',	YEA},	//211.80.54~63(20)
	{0x3ac48000,    0xFFFFC000,	{0,         	0},         	'M',    NA}     //58.196.238.0/18
}};



unsigned long
ipintonum(char *ip)
{
        union ip_str
        {
        unsigned char ip_s[4];
        unsigned long ip_num;
        }ip_u;
        char buf[128];
        char *p;
        int i;

        sprintf(buf,"%s",ip);
		if( !strchr(buf, '.') )
			return 0;
        p = strtok(buf, ".");
        for(i=3; i>=0; i--) {
                ip_u.ip_s[i] = atoi(p);
                if((p = strtok(NULL, ".")) == NULL)
                        break;
        }
        return ip_u.ip_num;
}




int checkip(char *ipaddr)
{
	int index;
	unsigned long	ipnum;
	char iptemp[16];
	IPTABLES * iptabp;
	IPRULE * iprule;

	iptabp = &currentIptable;
	strncpy(iptemp,ipaddr,sizeof(iptemp));
	iptemp[sizeof(iptemp)-1] = '\0';
	if((ipnum = ipintonum(iptemp))==0)
		return FLAG_UNKOWN;

#ifdef DEBUG_STAT
	prints("ip is %s and ip3 is %d .\n",ipaddr,ipnum);
#endif

	for(index=0;index<RULES_MAX;index++)
	{
		iprule = &iptabp->iprules[index];

		if((ipnum & iprule->mask) == iprule->match)
		{

#ifdef DEBUG_STAT
	prints("It is matched. range is %d/%d . \n",
		iprule->range[INDEX_MIN],
		iprule->range[INDEX_MAX]);
#endif

			if((iprule->isRanged==NA)||(ipnum >= iprule->range[INDEX_MIN] && ipnum <= iprule->range[INDEX_MAX]))			
			{

#ifdef DEBUG_STAT
	prints("It is in ranged. the area is %c \n",iprule->area);
#endif

				if(iprule->area == 'X')
					return FLAG_XUHUI;
				else if(iprule->area == 'M')
					return FLAG_MINHANG;
				else if(iprule->area == 'U')
					return FLAG_UNUSE_CAMPUS;
				else
					return FLAG_UNKOWN;
			}
			
		}
	}
	return FLAG_OUT_CAMPUS;
}

int insjtunetwork(char *ip)
{
	return checkip(ip) != FLAG_OUT_CAMPUS;
}


void cleanstats(struct userstat * userstatsp)
{
	userstatsp->malesum = 0;
	userstatsp->femalesum = 0;
	userstatsp->incamsum = 0;
	userstatsp->outcamsum = 0;
	userstatsp->xuhuisum = 0;
	userstatsp->minhangsum = 0;
}

struct userstat userstatist(void)
{
	struct userstat currentstats, * statsp;
	struct user_info *uentp;
	extern struct UTMPFILE * utmpshm;
	extern struct userec lookupuser;
	int i, max;

	statsp = &currentstats;
	cleanstats(statsp);
	resolve_utmp ();
	max = USHM_SIZE - 1;
	while (max > 0 && utmpshm->uinfo[max].active == 0)
		max--;
	for (i = 0; i <= max; i++)
	{
		uentp = &(utmpshm->uinfo[i]);
		if(uentp->uid && uentp->pid && uentp->active)
		{
			getuser(uentp->userid);
			if(lookupuser.gender == 'M')
				statsp->malesum ++;			//count the num of Male;
			if(lookupuser.gender == 'F')
				statsp->femalesum ++;

			//check the ip address
			if(lookupuser.lasthost[0] != '\0')
				switch(checkip(lookupuser.lasthost))
				{
					case FLAG_OUT_CAMPUS:
						statsp->outcamsum++;
						break;
	
					case FLAG_XUHUI:
						statsp->xuhuisum++;
						statsp->incamsum++;
						break;
						
					case FLAG_MINHANG:
						statsp->minhangsum++;
						statsp->incamsum++;
						break;

					case FLAG_UNUSE_CAMPUS:
						statsp->incamsum++;
						break;

					case FLAG_UNKOWN:
					default:
						break;
				}
		}	
	}
	return currentstats;
}


int
showStatist(void)
{
   struct userstat currentStat, *statsp;
   int ratioGG,ratioInCam,ratioMh;
   int sum;
   	
//display more actived user statist.
//added by Jerry Zhou.

    modify_user_mode (LUSERS);
    clear();
    prints("[1;32mÍ³¼ÆÖÐ£¬ÇëµÈ´ý¡£¡£¡£ \n\n");
	
    currentStat = userstatist();
    statsp = &currentStat;

    sum = statsp->femalesum + statsp->malesum;
    ratioGG = (int)(statsp->malesum * 100 / sum);
    ratioInCam = (int)(statsp->incamsum * 100 / sum);
    ratioMh = (int)(statsp->minhangsum * 100 / statsp->incamsum);

    clear();
    prints("[1;32mµ±Ç°ÔÚÏßÓÃ»§ÖÐ: \n\n");
    if(HAS_PERM(PERM_SYSOP))
    {
    	prints	
		("[1;32mÐ£ÄÚ/Ð£Íâ: [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n\n",
			statsp->incamsum, statsp->outcamsum,
			ratioInCam,100 -ratioInCam
		);
    }
   	prints
   		(" GG / MM : [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n\n",
	     		statsp->malesum, statsp->femalesum,
	     		ratioGG,100 -ratioGG
		 );
	prints
		("[1;32mÐì»ã/ãÉÐÐ: [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n",
	     		statsp->xuhuisum, statsp->minhangsum,
	     		100 - ratioMh, ratioMh
		);
	 
     pressreturn();	 
     return 0;

}
