/* Ë¼Ô´ºş ½»Ò×ÊĞ³¡´úÂë 2005.07.19 */
#include <string.h>
#include <stdlib.h>
#include <math.h>



#include "bbs.h"
#include "money.h"



//#ifdef ALLOWGAME

#define CHANGE_MAX	1000000

//pax@sjtubbs 05-07
//int  BANK_CURRDEPO_RATE[6]={3,7,11,15,19,24};//ÒøĞĞÀûÂÊ 
double  BANK_CURRDEPO_RATE[6]={2.5,5.25,8.5,12.0,16.0,20.0};//ÒøĞĞÀûÂÊ 
//2.5?Á½¸öÔÂ5.25£¿Èı¸öÔÂ8.25£¿ËÄ¸öÔÂ11.5
//5¸öÔÂ15£¿ 6¸öÔÂ18.75

double callrate=0.0005;
//int  CALL_RATE£½2;//»îÆÚÀûÂÊ
char mail_title[STRLEN]={0};
char mail_buffer[1024] = {0};//pax@sjtubbs 05 07 16
char bankdatetime[20];
//end by pax;





int inGame(void)
{
	FILE *fp;
	char buf[STRLEN];
	extern int NowInGame ;
	
	if (!strcmp("guest", currentuser.userid)) return 1;
	setuserfile(buf,"inGame");
	if(dashf(buf)){
		clear();
		move(12,18);
		prints("¶Ô²»Æğ£¬ÄúÒÑ¾­ÓĞÁíÍâÒ»¸ö´°¿ÚÔÚÊ¹ÓÃ±¾Õ¾µÄÓÎÏ·¹¦ÄÜ¡£\n");
		pressanykey();
		return 1;
	} 
	NowInGame = YEA;
	fp = fopen(buf,"w+");
	fclose(fp);
	return 0;
}

int gotomarket(char *title)
{
	if(inGame()==1) return 1;
	modify_user_mode(MARKET);
	clear();
	set_safe_record();
	move(2,0);
	prints("»¶Ó­½øÈë [[32m%s[37m]....\n\n",title);
	return 0;
}

void outgame(void)
{
	char buf[STRLEN];
	extern int NowInGame ;
	setuserfile(buf,"inGame");
	if(dashf(buf))unlink(buf);
	NowInGame = NA;
}



int withdraw()
{
	char sysbuf[STRLEN];
	//,uident[13];
	char msg[3];
	char money[7];
	int opt;
	int num_money;
	int id;
	time_t now;	
	//pax
	struct bankbill Bill;
	struct callbill cBill;
	struct rate bankrate ;
	long size;
	struct stat kkk;
	char fname[80];
	int fd;
	int fd_rate;
	char length[3];
	int num_length;//¶¨¶î´æ¿îµÄ´æ¿îÊ±¼ä³¤¶È
	char BillNo[9];
	int num_bill;
	//end by pax*/
	
	FILE *fp;
	struct Money mdata;
	char buf[STRLEN]; 
	int result=0;
	id = getuser(currentuser.userid);
	
	// get game coin
	/*	sethomefile(buf,currentuser.userid,"game_money.dat");
	if(dashf(buf)) update_game_money();
	fp = fopen(buf,"r");
	if(fp==NULL) return 0;
	result = fread(&mdata,sizeof(mdata),1,fp);
	fclose(fp);
	*/
	// modified by pax 05-08-02 
	//Ã»ÓĞÓÎÏ·±Ò¼ÍÂ¼µÄID×Ô¶¯´´½¨ÓÎÏ·±ÒÎÄ¼ş
	sethomefile(buf,currentuser.userid,"game_money.dat");
	if(dashf(buf)) update_game_money();
	fp = fopen(buf,"r");
	
	if(fp==NULL) 
	{
		mdata.money=0;
	}
	else
	{
		result = fread(&mdata,sizeof(mdata),1,fp);
		fclose(fp);
	}
	
	// get end	
	
	if(gotomarket("Ë¼Ô´ºş´óÒøĞĞ")) 
		return 0;
	prints("»¶Ó­¹âÁÙ Ë¼Ô´ºş´óÒøĞĞ\n");
	sprintf(fname,"ÄúÄ¿Ç°Ğ¯´øÏÖ½ğ %d Ôª£¬ÒøĞĞÕÊ»§½áÓà %d Ôª£¬ÓÎÏ·±Ò %ld Ôª¡£\n", currentuser.money, currentuser.inbank, mdata.money);

	prints(fname);

		if ((currentuser.inbank<0) || (currentuser.money<0 ))
		{
			move(8,0);
			prints("¶Ô²»Æğ£¬ÄúµÄ½ğÈÚÊı¾İÒì³££¬ÇëºÍÏµÍ³¹ÜÀíÔ±ÁªÏµ£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
//	prints("\n[1]»îÆÚ´æ¿î [2]»îÆÚÈ¡¿î [3]¹ºÂòÓÎÏ·±Ò [4]³öÊÛÓÎÏ·±Ò [5]¶¨ÆÚ´æ¿î [6]¶¨ÆÚÈ¡¿î\n");
	prints("\n[1]»îÆÚ´æ¿î [2]»îÆÚÈ¡¿î [3]¹ºÂòÓÎÏ·±Ò [4]¶¨ÆÚ´æ¿î [5]¶¨ÆÚÈ¡¿î\n");
	
	getdata( 8, 0,"ÇëÊäÈë²Ù×÷:", msg, 2, DOECHO, /*NULL,*/ YEA);
	if((msg[0] == '\n') || (msg[0] == '\0'))
	{
		outgame();
		return 0;
	}
	opt = atoi(msg);
//	if (!isdigit(msg[0]) || opt <= 0 || opt > 6 || msg[0] == '\n' || msg[0] == '\0')
	if (!isdigit(msg[0]) || opt <= 0 || opt > 5 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(10,0);
		prints("¶Ô²»Æğ£¬Ë¼Ô´ºş´óÒøĞĞ²»Ìá¹©´ËÏî·şÎñ£¡\n");
		pressanykey();
		outgame();
		return 0;
	}
	
	if(opt == 2)
	{
		update_deposit(currentuser.userid);//¸üĞÂÀûÏ¢ pax@2005-8-8

		/*ÅĞ¶ÏÊÇ·ñ´æ¿îÎÄ¼şÊÇ·ñ´æÔÚ*/
		sprintf (fname,"bank/%s.bank", currentuser.userid);
		if(fd=open(fname,O_RDWR )==-1)
		{
			prints("¶Ô²»Æğ£¬ÄúÉĞÎ´¿ªÍ¨»îÆÚ´æ¿îÕÊ»§\n");
			pressanykey();
			outgame();
			return 0;		
		}
	    get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified
		move( 9, 0);

		/*ÏÔÊ¾¿ÉÒÔÌáÈ¡µÄ×Ê½ğÒÔ¼°ÀûÏ¢*/
		sprintf (mail_buffer,"ÏÖÔÚ¿ÉÌáÈ¡µÄÊµ¼Ê½ğ¶î£º%8d(ÆäÖĞÀûÏ¢£º%8d)\n",(int)(cBill.principal+cBill.interest),(int)cBill.interest);
		prints (mail_buffer);
		
		/*ÅĞ¶ÏÈ¡¿îµÄÊıÄ¿ºÍºÏ·¨ĞÔ*/
		getdata(10, 0, "ÇëÊäÈëÄãÒªÈ¡¿îµÄ½ğ¶î [1 - 50000] (±ØĞëÎªÕûÊı):", money, 6, DOECHO, YEA);
		num_money = (int)atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬È¡¿î½ğ¶î±ØĞëÔÚ 1 - 50000 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if((cBill.principal+(int)cBill.interest) < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúµÄË¼Ô´ÊµÒµÒøĞĞÕÊ»§²»ÄÜÍ¸Ö§£¡\n");
			pressanykey();
			outgame();
			return 0;
		}

		/*¸üĞÂ.bankÎÄ¼ş*/
		/*½«ÀûÏ¢·Åµ½ÒøĞĞÕË»§ºÍ±¾½ğÉÏ£¬ÀûÏ¢Çå¿Õ£¬Ëæºó±¾½ğÖĞ¿Û³ıÈ¡¿îÊıÄ¿*/
			cBill.principal+=(int)cBill.interest;
			currentuser.inbank += (int)cBill.interest;
			cBill.interest-=(int)cBill.interest;	
			cBill.principal-=num_money;		
			cBill.date=time(0);
			substitute_record(fname, &cBill,sizeof(struct callbill), 1);
		/*¸üĞÂPASSFILEÎÄ¼ş*/
		currentuser.money += num_money;//Ôö¼Ó±¾½ğºÍÀûÏ¢
		currentuser.inbank -= num_money;//¿Û³ı±¾½ğ
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		/*·¢ĞÅÍ¨Öª*/
		getdatestring (cBill.date, NA);
		sprintf (mail_title, "%s %.23s È¡¿î %d Ôª", currentuser.userid,datestring + 6,num_money);
		securityreport(mail_title);		
		//pax@sjtubbs 05-07 È¡¿î·¢ĞÅ
		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		{
			sprintf (mail_buffer,
				"·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n\n"
				"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n\n"
				"\n ÔÚË¼Ô´ºşÒøĞĞ»îÆÚÈ¡¿î %d Ôª",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		move(14,0);
		prints("ÄúµÄ´æ¿îÊÖĞøÒÑ¾­Íê³É\n");
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 1)
	{
		update_deposit(currentuser.userid);//¸üĞÂÀûÏ¢ pax@2005-8-8
		getdata(10, 0, "ÇëÊäÈëÄã´æ¿îµÄ½ğ¶î [1 - 50000] (±ØĞëÎªÕûÊı):", money, 6, DOECHO, YEA);
		num_money = (int)atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬´æ¿î½ğ¶î±ØĞëÔÚ 1 - 50000 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúµÄÏÖ½ğ²»×ã£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		sprintf (fname,"bank/%s.bank", currentuser.userid);
		if(fd=open(fname,O_RDWR )==-1)//µÚÒ»´Î´æ¿î
		{
			fd=open(fname,O_RDWR | O_CREAT ,0644 );		
			cBill.date=time(0);			
			cBill.principal=num_money;	
			cBill.interest=0;
			write(fd,&cBill,sizeof(struct callbill));//ÖØĞ´ÎÄ¼ş
			close(fd);		
		}
		else{

			get_record (fname, &cBill,sizeof(struct callbill), 1);
			cBill.principal+=(int )cBill.interest;
			cBill.interest-=(int )cBill.interest;	
			cBill.principal+=num_money;		
			cBill.date=time(0);

			substitute_record(fname, &cBill,sizeof(struct callbill), 1);
		}
		currentuser.money -= num_money;
		currentuser.inbank += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		getdatestring (cBill.date, NA);		
		sprintf (mail_title, "%s %.23s ´æ¿î %d Ôª", currentuser.userid,datestring + 6,num_money);
		securityreport(mail_title);		
		//pax@sjtubbs 05-07 ´æ¿î·¢ĞÅ
		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		{

			sprintf (mail_buffer,
				"·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n\n"
				"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n\n"
				"\n ÔÚË¼Ô´ºşÒøĞĞ»îÆÚ´æ¿î %d Ôª",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		move(14,0);
		prints("ÄúµÄ´æ¿îÊÖĞøÒÑ¾­Íê³É\n");
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 3)//buy
	{
		getdata(10, 0, "ÇëÊäÈëÄãĞèÒª¹ºÂòµÄÓÎÏ·±ÒÊıÄ¿ [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬¹ºÂòÊıÁ¿±ØĞëÔÚ 1 - 50000 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúµÄÏÖ½ğ²»×ã£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		currentuser.money -= num_money;
		mdata.money += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		fp = fopen(buf,"w");
		if(fp != NULL)
		{
			result = fwrite(&mdata, sizeof(mdata), 1, fp);
			fclose(fp);
		}		
		move(12,0);
		prints("ÄúµÄÓÎÏ·±Ò¶Ò»»ÊÖĞøÒÑ¾­Íê³É\n");

		sprintf(sysbuf, "%s ¹ºÂòÓÎÏ·±Ò %d Ôª", currentuser.userid, num_money);
		securityreport(sysbuf);
		
		//pax@sjtubbs 05-07 ¹ºÂòÓÎÏ·±Ò
		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		{
			now = time (0);	
			getdatestring (now, NA);
			sprintf (mail_title, "%.23s ¹ºÂòÓÎÏ·±Ò %d Ôª", datestring + 6,num_money);
			sprintf (mail_buffer,
				"\n·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n\n"
				"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n\n"
				"\n ¹ºÂòÓÎÏ·±Ò %d Ôª",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		pressanykey();
		outgame();
		return 1;
	}
	
/*	if(opt == 4)//sell
	{
		getdata(10, 0, "ÇëÊäÈëÄãĞèÒª³öÊÛµÄÓÎÏ·±ÒÊıÄ¿ [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬³öÊÛÊıÁ¿±ØĞëÔÚ 1 - 50000 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(mdata.money < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúÃ»ÓĞÕâÃ´¶àÓÎÏ·±ÒÌ×ÏÖ£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		mdata.money -= num_money;		
		currentuser.money += (num_money/10);
		
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		fp = fopen(buf,"w");
		if(fp != NULL)
		{
			result = fwrite(&mdata, sizeof(mdata), 1, fp);
			fclose(fp);
		}		
		move(12,0);
		prints("ÄúµÄÓÎÏ·±Ò¶Ò»»ÊÖĞøÒÑ¾­Íê³É\n");
		sprintf(sysbuf, "%s ³öÊÛÓÎÏ·±Ò %d Ôª", currentuser.userid, num_money);
		securityreport(sysbuf);
		//pax@sjtubbs 05-07 ³öÊÛÓÎÏ·±Ò
		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		{
			now = time (0);
			getdatestring (now, NA);
			sprintf (mail_title, "%.23s ³öÊÛÓÎÏ·±Ò %d Ôª", datestring + 6,num_money);
			sprintf (mail_buffer,
				"\n·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n\n"
				"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n\n"
				"\n ³öÊÛÓÎÏ·±Ò %d Ôª",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		pressanykey();
		outgame();
		return 1;
	}
*/	
	if(opt == 4)
			//if(opt == 5)
	{
		getdata(10, 0, "ÇëÊäÈëÄã´æ¿îµÄ½ğ¶î [1 - 50(µ¥Î» Ç§Ôª)] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬´æ¿î½ğ¶î±ØĞëÔÚ 1 - 50 (Ç§Ôª) Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		num_money=num_money*1000;
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúµÄÏÖ½ğ²»×ã£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		
		getdata(12, 0, "ÇëÊäÈëÄã´Ë´Î´æ¿îµÄÊ±¼ä³¤¶È [1 - 6]£¨µ¥Î»£ºÔÂ£© :", length, 2, DOECHO, YEA);
		num_length = atoi(length);
		
		if (!isdigit(length[0]) || num_length <= 0 || num_length > 6 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 14, 0);
			prints("¶Ô²»Æğ£¬´æ¿îµÄÊ±¼ä³¤¶È±ØĞëÔÚ 1 - 6 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		fd=open("bank/bank",O_APPEND | O_WRONLY | O_CREAT ,0644 );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);
		strcpy(Bill.userid,currentuser.userid);
		Bill.usernum=size+1;
		Bill.datein=time(0);			
		Bill.money=num_money;		
		Bill.dateout=0;	
		Bill.length=num_length;
		write(fd,&Bill,sizeof(struct bankbill));
		close(fd);
		
		currentuser.money -= num_money;
		currentuser.inbank += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		//¶¨ÆÚ´æ¿îÇ¿ÖÆ·¢ĞÅ
		//		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		//		{
		now = time (0);
		getdatestring (now, NA);
		sprintf (mail_title, "%s °ìÀí %dÔÂÆÚ´æ¿î %d Ôª", currentuser.userid,num_length,num_money);
		sprintf (mail_buffer,
			"·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n"
			"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n"
			"\n ÔÚË¼Ô´ºşÒøĞĞ´æ¿î %d Ôª£¬´æÆÚÎª %d ¸öÔÂ\n"
			"\nÕÊµ¥ºÅÎª£º%ld",currentuser.userid, datestring + 6,num_money ,num_length,size+1);
		bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		//		}
		move(12,0);
		prints("ÄúµÄ´æ¿îÊÖĞøÒÑ¾­Íê³É\n");
		sprintf(sysbuf, "%s °ìÀí%d ÔÂÆÚ´æ¿î %d Ôª£¬ÕÊµ¥ºÅÎª %ld", currentuser.userid,num_length, num_money, Bill.usernum);
		securityreport(sysbuf);
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 5)//	if(opt == 6)
	{
		getdata(10, 0, "ÇëÊäÈëÄã´æ¿îµÄÕÊµ¥ºÅ :", BillNo, 8, DOECHO, YEA);
		num_bill = atoi(BillNo);
		
		if (!isdigit(BillNo[0]) || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬ÇëÊäÈëÕıÈ·ÕÊµ¥ºÅ£¡\n");
			pressanykey();
			outgame();
			return 0;
		}
		sprintf (fname,"bank/bank");
		get_record (fname, &Bill,sizeof(struct bankbill), num_bill);
		
		if(strcmp("pax",currentuser.userid)!=0)//ÅĞ¶ÏÊÇ·ñÎªµ±Ç°ÓÃ»§µÄÕË»§£¬ÅĞ¶ÏÊÇ·ñÒÑ¾­±»È¡×ßÁË
		{
			if((strcmp(Bill.userid,currentuser.userid)!=0) || (Bill.dateout!=0))
			{
				move( 12, 0);
				prints("¶Ô²»Æğ£¬ÇëÊäÈëÕıÈ·ÕÊµ¥ºÅ£¡\n");
				pressanykey();
				outgame();
				return 0;
			}
		}
		Bill.dateout=time(0);//Éè¶¨È¡³öÊ±¼ä
		if(Bill.dateout<Bill.datein+Bill.length*86400*30)
		{
			if(askyn ("ÄãÈ·¶¨ÒªÌáÔç½áÊøÂğ", NA, NA) != YEA)
			{
				move (2, 0);
				prints ("È¡Ïû²Ù×÷\n");
				pressreturn ();
				outgame();
				return 0;
			}
			else
			{
				callrate=0.0005;
				if(fd_rate=open("bank/rate",O_RDONLY)!=1)
				{
					fd_rate=open("bank/rate",O_RDONLY);
					stat("bank/rate",&kkk);
					size=kkk.st_size/sizeof(struct rate);
					read(fd_rate,&bankrate,sizeof(struct rate));	
					while((size>1) && (bankrate.time<Bill.datein))
					{
						read(fd_rate,&bankrate,sizeof(struct rate));
						callrate=bankrate.currdepo;
						size--;
					}
					close(fd_rate);
				}
				num_money=(int)Bill.money*(1+callrate*ceil((Bill.dateout-Bill.datein)/86400));//°´ÕÕ»îÆÚµÄËã
			}
		}
		else
		{
			if(fd_rate=open("bank/rate",O_RDONLY)!=1)
			{
				fd_rate=open("bank/rate",O_RDONLY);
				stat("bank/rate",&kkk);
				size=kkk.st_size/sizeof(struct rate);
				read(fd_rate,&bankrate,sizeof(struct rate));
    			while((size>1) && (bankrate.time<Bill.datein))
				{
					read(fd_rate,&bankrate,sizeof(struct rate));	
					BANK_CURRDEPO_RATE[0]=bankrate.timedepo_01;
					BANK_CURRDEPO_RATE[1]=bankrate.timedepo_02;
					BANK_CURRDEPO_RATE[2]=bankrate.timedepo_03;
					BANK_CURRDEPO_RATE[3]=bankrate.timedepo_04;
					BANK_CURRDEPO_RATE[4]=bankrate.timedepo_05;
					BANK_CURRDEPO_RATE[5]=bankrate.timedepo_06;
					size--;
				}	
				close(fd_rate);
			}
			num_money=(int)Bill.money*(1+BANK_CURRDEPO_RATE[Bill.length-1]/100);//°´ÕÕ¶¨ÆÚµÄËã
		}
		sprintf (fname,"bank/bank");
		substitute_record(fname, &Bill,sizeof(struct bankbill), num_bill);
		currentuser.money += num_money;//ÏÖ½ğ¼ÓÉÏ±¾½ğºÍÀûÏ¢
		currentuser.inbank -= Bill.money;//ÕÊ»§Ö»¿ÛÈ¥±¾½ğ
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
//		close(fd);
		
		//pax@sjtubbs 05-07 ´æ¿î·¢ĞÅ
		if (askyn ("Òª¸øÄú·¢±¸·İĞÅ¼şÂğ£¿", YEA, NA) == YEA)//pax@05-07 È·ÈÏ
		{
			getdatestring (Bill.dateout, NA);
			sprintf (mail_title, "%s È¡¿î %dÔÂÆÚÈ¡¿î %d Ôª", currentuser.userid,Bill.length,num_money );
			sprintf (mail_buffer,
				"·¢ĞÅÕ¾: Ë¼Ô´ÊµÒµÒøĞĞ·¢ĞÅÏµÍ³\n"
				"\nÇ×°®µÄÓÃ»§%s£¬ÄúÓÚ¡¾%.23s¡¿\n"
				"\n ÔÚË¼Ô´ºşÒøĞĞÈ¡¿î %d Ôª\n"
				"\nÕÊµ¥ºÅÎª£º%ld",currentuser.userid, datestring + 6,num_money ,Bill.usernum);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		move(12,0);
		prints("ÄúµÄÈ¡¿îÊÖĞøÒÑ¾­Íê³É\n");
		sprintf(sysbuf, "%s ÁìÈ¡´æ¿î %d Ôª£¬ÕÊµ¥ºÅÎª %ld", currentuser.userid, num_money,Bill.usernum);
		
		securityreport(sysbuf);
		pressanykey();
		outgame();
		return 1;
	}
	
	move(12,0);
	prints("½ñÈÕË¼Ô´ºş´óÒøĞĞĞİÏ¢:)\n");
	pressanykey();
	outgame();
	return 1;
	
	
}


int 
bank_record(){
	struct bankbill Bill;
	long size;
	struct stat kkk;
	//	char fname[80];
	int i;
	long count;
	int fd;
	char mail_title[STRLEN]={0};
	char mail_buffer[1024] = {0};
	int all;
	char ans[5];
	char temp[65535];
	char someoneID[20];
	clear();
	all = 0;
	
	//-----------------------------------------------------------------
    modify_user_mode (QUERY);
    usercomplete ("ÇëÊäÈëÄúÏë²éÑ¯µÄÕÊºÅ: ", someoneID);
    if (someoneID[0] == 0)
    {
        getdata (0, 30, "²éÑ¯ËùÓĞµÄ×÷ÕßÂğ?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return;
        }
        else
            all = 1;
    }
	else if (!getuser (someoneID))
    {
        prints ("²»ÕıÈ·µÄÊ¹ÓÃÕß´úºÅ\n");
        pressreturn ();
        return;
    }
	
	
	if (all==1){
		fd=open("bank/bank",O_RDWR | O_CREAT );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);
		i=0;
		count=0;
		sprintf (temp,"ËùÓĞÈËµÄÒøĞĞ¼ÇÂ¼\n");
		sprintf (mail_title,"ËùÓĞÈËµÄÒøĞĞ¼ÇÂ¼");		
		sprintf(mail_buffer,"%8s %12s %20s %20s %8s %s\n", "ÕÊµ¥ºÅ", "ID", "´æÈëÈÕÆÚ","È¡¿îÈÕÆÚ","½ğ¶î","´æÆÚ");
		strcat (temp,mail_buffer);
		
		while (i<size)
		{
			read(fd,&Bill,sizeof(struct bankbill));
			if(Bill.dateout==0){
				count=count+Bill.money;}			
			//	sprintf (mail_buffer,"\n%8d %12s %20s %20s %8d %d",Bill.usernum,Bill.userid,getdatebank(Bill.datein),getdatebank(Bill.dateout),Bill.money,Bill.length);
			getdatebank(Bill.datein);
			sprintf (mail_buffer,"\n%8d %12s %s ",Bill.usernum,Bill.userid,bankdatetime);
			strcat (temp, mail_buffer);
			getdatebank(Bill.dateout);
			sprintf (mail_buffer,"%s %8d %2d",bankdatetime,Bill.money,Bill.length);
			strcat (temp, mail_buffer);
			
			i++;
		}
		sprintf (mail_buffer,"\n×Ü¶î:%ld",count);
		strcat (temp, mail_buffer);
		bankreport(mail_title, temp, currentuser.userid, NULL);
		close(fd);	
	}
	else{
		fd=open("bank/bank",O_RDWR | O_CREAT );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);	
		i=0;
		count=0;
		
		sprintf (mail_title,"%sµÄÒøĞĞ¼ÇÂ¼",someoneID);	
		sprintf (temp,"%sµÄÒøĞĞ¼ÇÂ¼\n",someoneID);				
		sprintf(mail_buffer,"%8s %20s %20s %8s %s\n", "ÕÊµ¥ºÅ", "´æÈëÈÕÆÚ","È¡¿îÈÕÆÚ","½ğ¶î","´æÆÚ");
		strcat (temp,mail_buffer);
		while (i<size)
		{
			read(fd,&Bill,sizeof(struct bankbill));
			//if(strcmp(Bill.userid,someoneID)==0)
			if((strcmp(Bill.userid,someoneID)==0)&&(Bill.dateout==0))
			{
				count=count+Bill.money;
				getdatebank(Bill.datein);
				sprintf (mail_buffer,"\n%8d  %s ",Bill.usernum,bankdatetime);
				strcat (temp, mail_buffer);
				getdatebank(Bill.dateout);
				sprintf (mail_buffer,"%s %8d %2d",bankdatetime,Bill.money,Bill.length);
				strcat (temp, mail_buffer);
			}
			i++;
		}
		sprintf (mail_buffer,"\ncount=%ld,size=%d,id=%s",count,size,someoneID);
		strcat (temp, mail_buffer);
		bankreport(mail_title, temp, currentuser.userid, NULL);
		close(fd);	}
}



int
getdatebank (time_t now)
{
	struct tm *tm;
	
	tm = localtime (&now);
	sprintf (bankdatetime, "%4d-%02d-%02d %02d:%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}
int lending()
{
	//	char sysbuf[STRLEN];
	int     id, canlending=0,maxnum = 0, num = 0,u_money=0;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	int i = 0;
	time_t 	now=time(0);
	extern int gettheuserid();
	
	if(gotomarket("½»Ò×ÊĞ³¡")) return 0;
	maxnum = currentuser.money - 100;
	prints("[m»¶Ó­Ê¹ÓÃ[1;33m"BBSNAME"[m½»Ò×ÊĞ³¡[32mÓÑÇé×ªÕÊ[m¹¦ÄÜ");
	prints("\n\nÄúÄ¿Ç°µÄÇé¿öÊÇ£º\n×¢²áÌìÊı([32m%d[m Ìì) "
		"ÉÏÕ¾×ÜÊ±Êı([32m%d[m Ğ¡Ê±) "
		"[44;37m¿É×ªÕÊ×Ê½ğ([32m%d[37m Ôª)[m",
		(now - currentuser.firstlogin)/86400,
		currentuser.stay/3600,currentuser.money-100);
	if(currentuser.stay<=3600||now-currentuser.firstlogin<=259200||maxnum<=0){
		prints("\n\nÄ¿Ç°ÊĞ³¡¹æ¶¨£º ²ÎÓë[32mÓÑÇé×ªÕÊ[mµÄ ID ±ØĞë¾ß±¸ÒÔÏÂ"
			"È«²¿Ìõ¼ş£º\n"
			"    1. ±¾ÕÊºÅ×¢²áÌìÊı³¬¹ı 3 Ìì;\n"
			"    2. ×ÜÉÏÕ¾Ê±Êı³¬¹ı 1 Ğ¡Ê±;\n"
			"    3. ×îÖÕÓµÓĞ´æ¿î³¬¹ı 100 Ôª.\n");
		prints("\n\n¸ù¾İÊĞ³¡¹æ¶¨£¬ÄúÄ¿Ç°ÉĞÃ»ÓĞ[32mÓÑÇé×ªÕÊ[mµÄ×Ê¸ñ¡£\n");
		pressanykey();
		outgame();
		return 0;
	}
	if (!gettheuserid(9,"ÄúÏë×ªÕÊµ½Ë­µÄÕÊ»§ÉÏ£¿ÇëÊäÈëËûµÄÕÊºÅ: ",&id))
	{
		outgame();
		return 0;
	}
	u_money = lookupuser.money+lookupuser.medals*10000+ lookupuser.inbank;
	if(!strcmp(currentuser.userid,lookupuser.userid) ||u_money > CHANGE_MAX){
		prints("\n¶Ô²»Æğ£¬%s Ä¿Ç°ÉĞ²»ĞèÒªÄúµÄ×ªÕÊ£¡",lookupuser.userid);
		pressanykey();
		outgame();
		return 0;
	}
	move(10,0);
	canlending = (maxnum>(CHANGE_MAX-u_money))?(CHANGE_MAX-u_money):maxnum;
	prints("Äú½«×ªÕÊ¸ø [[1;32m%s[m]£¬×ªÕÊ×î¶à½ğ¶îÎª [1;33m%d[m Ôª¡£",
		lookupuser.userid, canlending);
	getdata(12, 0, "È·ÊµÒª×ªÕÊ£¬ÇëÊäÈë×ªÕÊÊıÄ¿£¬·ñÔò£¬ÇëÖ±½Ó»Ø³µÈ¡Ïû×ªÕÊ: ",
		ans, 6, DOECHO, YEA);
	num = atoi(ans);
	if ( num <= 0 || num > canlending ) {
		prints("\nÊäÈëÊıÄ¿ÓĞ´íÎó¡£ È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	if(currentuser.money - 100  != maxnum) {
		prints("\n¶Ô²»Æğ£¬ÄúµÄ¿É×ªÕÊ×Ê½ğÓĞËù±ä»¯£¬È¡Ïû´Ë´Î½»Ò×£¡");
		prints("\nÇëÖØĞÂÖ´ĞĞ±¾½»Ò×¡£");
		pressanykey();
		outgame();
		return 0;
	}
	
	for (i = 0; i < 5; i++)
		ps[i][0] = '\0';
	move (14, 0);
	prints ("ÇëÊäÈë×ªÕÊ¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
	for (i = 0; i < 5; i++)
	{
		getdata (i + 15, 0, ": ", ps[i], STRLEN - 5, DOECHO, YEA);
		if (ps[i][0] == '\0')
			break;
	}
	if (askyn ("È·ÈÏ×ªÕÊ£¿", YEA, NA) == YEA)//pax@05-07 ×ªÕÊÈ·ÈÏ
	{
		currentuser.money -= num;
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);
		lookupuser.money += num;
		substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
		
		prints("\nĞ»Ğ»Äú£¬ÄúÒÑ¾­¸ø [1;32m%s[m ×ªÕÊ [1;33m%d[m Ôª¡£",
			lookupuser.userid,num);
		prints("\nÎª±íÊ¾¶ÔÄãµÄ×ªÕÊĞĞÎªµÄ¸ĞĞ»£¬±¾Õ¾ÒÑ¾­ÓÃĞÅ¼şÍ¨ÖªÁËËû¡£");
		sprintf(genbuf,"¸ø %s ×ªÕÊ %d Ôª",lookupuser.userid,num);
		gamelog(genbuf);
		sprintf(ans,"%s ¸ø %s ¼ÄÀ´ÁË %d ÔªÓÑÇé×ªÕÊ",currentuser.userid, lookupuser.userid, num);
		sprintf(genbuf,"\n %s :\n\n\tÄúºÃ£¡\n\n\tÄúµÄÅóÓÑ %s ¸øÄú¼ÄÀ´ÁË %d ÔªÓÑÇé×ªÕÊ×Ê½ğ¡£\n",lookupuser.userid,currentuser.userid,num);
		//   bankreport(ans,genbuf,NA,lookupuser.userid);
		
		for (i = 0; i < 5; i++)
		{
			if (ps[i][0] == '\0')
				break;
			if (i == 0)
				strcat (genbuf, "\n\n×ªÕÊ¸½ÑÔ£º\n");
			strcat (genbuf, ps[i]);
			strcat (genbuf, "\n");
		}
		
		strcpy (currboard, "MoneyAdmin");
		autoreport(ans,genbuf, lookupuser.userid, NULL);
		//add by pax@sjtubbs 05-07 ×ªÕÊ±¸·İ¸ø×Ô¼º
		if (askyn ("±¸·İ¸ø×Ô¼º£¿", YEA, NA) == YEA)//pax@05-07 ×ªÕÊÈ·ÈÏ
		{
			sprintf(ans,"%s ¸ø %s ¼ÄÈ¥ÁË %d ÔªÓÑÇé×ªÕÊ",currentuser.userid, lookupuser.userid, num);
			sprintf(genbuf,"\n %s :\n\n\tÄúºÃ£¡\n\n\t Äú¸øÄúµÄÅóÓÑ %s ¼ÄÈ¥ÁË %d ÔªÓÑÇé×ªÕÊ×Ê½ğ¡£\n",currentuser.userid,lookupuser.userid,num);
			for (i = 0; i < 5; i++)
			{
				if (ps[i][0] == '\0')
					break;
				if (i == 0)
					strcat (genbuf, "\n\n×ªÕÊ¸½ÑÔ£º\n");
				strcat (genbuf, ps[i]);
				strcat (genbuf, "\n");
			}
			bankreport(ans,genbuf,currentuser.userid, NULL);
		}
		//end by pax
		
	}//pax
	pressanykey();
	outgame();
	return 1;
}

int popshop(void)
{
	int no,num,maxnum/*,templog*/;
	char ans[10];
	//char sysbuf[STRLEN];
	EXCHANGES exchanges[10] = {
		//      {"ÉÏÕ¾´ÎÊı",2},
		//      {"ÎÄÕÂÊı",5},
		//      {"½±ÕÂÊı",10000},
		{"ÒşÉíÊõ",40000},
		{"¿´´©ÒşÉíÊõ",100000},
		{"ÕÊºÅÓÀ¾Ã±£Áô",200000},
		{"Ç¿ÖÆºô½Ğ",20000},
		{"ÑÓ³¤·¢´ôÊ±¼ä",10000},
		{"666 ÉúÃüÁ¦",20000},
		{"ĞÅÏäÎŞÉÏÏŞ",40000}
	};
	if(gotomarket("ÊĞ³¡µäµ±ĞĞ")) return 1;
	prints("½ñÈÕµäµ±±¨¼Û:");
	for(no = 0; no < 7; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ôª", exchanges[no].value);
	}
	//   move(16,0);
	//   prints("[0;1mÄúÄ¿Ç°µÄÇé¿öÊÇ: (1)ÉÏÕ¾´ÎÊı([32m%d[m)  "
	//          "(2)ÎÄÕÂÊı([32m%d[37m)  (3)½±ÕÂÊı.([32m%d[37m)\n",
	//	  currentuser.numlogins,currentuser.numposts,currentuser.medals);
	getdata(18, 0, "ÄúÏëµäµ±ÄÄÒ»Ïî£¿", ans, 10, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > 7/*10*/ ) {
		prints("\nÊäÈë´íÎó£¬È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	move(18, 30);
	prints("ÄãÑ¡Ôñµäµ±¡°[32m%s[m¡±¡£",exchanges[no-1].title);
	if(no>0/*3*/){
		set_safe_record();
		maxnum = exchanges[no-1].value;
		switch(no) {
		case 1:
			if(!HAS_PERM(PERM_CLOAK)) { num = 0; break; }
			break;
		case 2:
			if(!HAS_PERM(PERM_SEECLOAK)) { num = 0; break; }
			break;
		case 3:
			if(!HAS_PERM(PERM_XEMPT)) { num = 0; break; }
			break;
		case 4:
			if(!HAS_PERM(PERM_FORCEPAGE)) { num = 0; break; }
			break;
		case 5:
			if(!HAS_PERM(PERM_EXT_IDLE)) { num = 0; break; }
			break;
		case 6:
			if(!HAS_PERM(PERM_666LIFE)) { num = 0; break; }
			break;
		case 7:
			if(!HAS_PERM(PERM_MAILINFINITY)) { num = 0; break; }
			break;
		}
		prints("\n\n");
		if(!num) {
			prints("¶Ô²»Æğ, Äã»¹Ã»ÓĞÕâÖÖÈ¨ÏŞ. ");
			pressanykey();
			outgame();
			return 0;
		}
		if(askyn("ÄúÈ·¶¨Òªµäµ±Âğ£¿",NA,NA) == NA ) {
			move(23,0);clrtoeol();
			prints("½»Ò×È¡Ïû ...");
			pressanykey();
			outgame();
			return 0;
		}
		set_safe_record();
		switch(no) {
		case 1:
			num = HAS_PERM(PERM_CLOAK);
			currentuser.userlevel &= ~PERM_CLOAK ;
			break;
		case 2:
			num = HAS_PERM(PERM_SEECLOAK);
			currentuser.userlevel &= ~PERM_SEECLOAK ;
			break;
		case 3:
			num = HAS_PERM(PERM_XEMPT);
			currentuser.userlevel &= ~PERM_XEMPT ;
			break;
		case 4:
			num = HAS_PERM(PERM_FORCEPAGE);
			currentuser.userlevel &= ~PERM_FORCEPAGE ;
			break;
		case 5:
			num = HAS_PERM(PERM_EXT_IDLE);
			currentuser.userlevel &= ~PERM_EXT_IDLE ;
			break;
		case 6:
			num = HAS_PERM(PERM_666LIFE);
			currentuser.userlevel &= ~PERM_666LIFE ;
			break;
		case 7:
			num = HAS_PERM(PERM_MAILINFINITY);
			currentuser.userlevel &= ~PERM_MAILINFINITY ;
			break;
		}
		if(!num) {
			prints("¶Ô²»Æğ, ÄãµÄÊı¾İ·¢ÉúÁË±ä»¯, ÄãÄ¿Ç°Ã»ÓĞÕâÖÖÈ¨ÏŞ. ");
			pressanykey();
			outgame();
			return 0;
		}
		} /*
		  else {
		  if( no == 1 )maxnum = currentuser.numlogins-1;
		  else if ( no == 2) maxnum = currentuser.numposts;  
		  else	maxnum = currentuser.medals;
		  templog = maxnum;
		  sprintf(genbuf,"ÄúÏëµäµ±¶àÉÙÄØ(×î¶à%d)£¿",maxnum);
		  getdata(19, 0, genbuf,ans, 10, DOECHO, YEA);
		  num = atoi(ans);
		  if ( num <= 0 || num > maxnum ) {
		  prints("ÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		  pressanykey();
		  outgame();
		  return 0;
		  }
		  maxnum = num*exchanges[no-1].value;
		  move(19,0);
		  prints("Äú¹²¼Æµäµ±%s.[32m%d.[m ·İ£¬ %s .[33m%d.[m Ôª¡£\n",
		  exchanges[no-1].title,num,"¿ÉÒÔ»ñµÃ",maxnum);
		  if(askyn("ÄúÈ·¶¨Òªµäµ±Âğ£¿",NA,NA) == NA ) {
		  move(21,0);clrtoeol();
		  prints("È¡Ïû½»Ò× ...");
		  pressanykey();
		  outgame();
		  return 0;
		  }
		  set_safe_record();
		  if (no == 1) {
		  if(templog==currentuser.numlogins-1)
		  currentuser.numlogins -= num;
		  else templog = -1;
		  } else if (no == 2)  {
		  if(templog == currentuser.numposts)
		  currentuser.numposts -= num;
		  else templog = -1;
		  } else {
		  if(templog == currentuser.medals)
		  currentuser.medals -= num;
		  else templog = -1;
		  }
		  if( templog == -1) {
		  move(21,0); clrtoeol();
		  prints("¶Ô²»Æğ, ÔÚ½»Ò×¹ı³ÌÖĞÄúµÄÊı¾İ·¢ÉúÁË±ä»¯.\n");
		  prints("Îª±£Ö¤½»Ò×µÄÕı³£½øĞĞ, ´Ë´Î½»Ò×È¡Ïû.\nÄú¿ÉÒÔÖØĞÂ½øĞĞ±¾½»Ò×.");
		  pressanykey();
		  outgame();
		  return 0;
		  }
	  }*/
	  
	  currentuser.money += maxnum;
	  if( currentuser.money > 900000000 ){
		  move(21,0); clrtoeol();
		  prints("¶Ô²»Æğ£¬½»Ò×Êı¾İ¹ı´ó£¬²úÉúÒç³ö£¬ÇëÖØĞÂ½»Ò×£¡\n");
		  prints("½¨ÒéÄú½«Ò»²¿·Ö½ğÇ®´æÈëÒøĞĞ¡£");
		  pressanykey();
		  outgame();
		  return 0;
	  }
	  substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	  move(21,0); clrtoeol();
	  prints("Ë³ÀûÍê³É½»Ò×£¬»¶Ó­ÄúµÄÔÙ´Î»İ¹Ë¡£;P");
	  if(no<=3)
	  {
		  sprintf(genbuf,"µäµ±%s %d ·İ£¬»¨·Ñ %d Ôª.",exchanges[no-1].title,num,maxnum);
	  }
	  else
	  {
		  sprintf(genbuf,"µäµ±%s£¬»¨·Ñ %d Ôª.",exchanges[no-1].title,maxnum);
	  }
	  securityreport(genbuf);
	  gamelog(genbuf);
	  pressanykey();
	  outgame();
	  return 1;
}

/*
int shopping()//modified by smartfish, 2003.09.08.
{
	int no,hasperm=1,maxnum;
	char ans[10];
	EXCHANGES exchanges[10] = {
		{"ÒşÉíÊõ",100000},
		{"¿´´©ÒşÉíÊõ",4000000},
		{"ÕÊºÅÓÀ¾Ã±£Áô",4000000},
		{"Ç¿ÖÆºô½Ğ",4000000},
		{"ÑÓ³¤·¢´ôÊ±¼ä",400000},
		{"666ÉúÃüÁ¦",1000000},
		{"ÎŞÉÏÏŞĞÅÏä",9900000}
	};
	if(gotomarket("ÊĞ³¡¹ºÎïÖĞĞÄ")) return 1;
	prints("½ñÈÕÉÌÆ·±¨¼Û:");
	for(no = 0; no < 7; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ôª", exchanges[no].value);
	}
	move(16,0);
	prints("ÄúÄ¿Ç°ÉĞÓĞ %d Ôª (½±ÕÂ %d ¸ö)\n",
		currentuser.money,currentuser.medals);
	getdata(18, 0, "ÄúÏë¹ºÂòÄÄÒ»¼şÉÌÆ·£¿", ans, 10, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > 7 ) {
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	maxnum = exchanges[no-1].value;
	switch(no) {
	case 1:
		hasperm = HAS_PERM(PERM_CLOAK);
		break;
	case 2:
		hasperm = HAS_PERM(PERM_SEECLOAK);
		break;
	case 3:
		hasperm = HAS_PERM(PERM_XEMPT);
		break;
	case 4:
		hasperm = HAS_PERM(PERM_FORCEPAGE);
		break;
	case 5:
		hasperm = HAS_PERM(PERM_EXT_IDLE);
		break;
	case 6:
		hasperm = HAS_PERM(PERM_666LIFE);
		break;
	case 7:
		hasperm = HAS_PERM(PERM_MAILINFINITY);
		break;
	}
	prints("\n\n");
	if(hasperm) {
		prints("ÄúÒÑ¾­ÓĞÕâÖÖÈ¨ÏŞ, ²»ĞèÒªÔÙ¹ºÂò. ");
		pressanykey();
		outgame();
		return 0;
	}
	if(currentuser.money < maxnum) {
		prints("\n¶Ô²»Æğ, ÄãÃ»ÓĞ×ã¹»µÄÇ®¹ºÂòÕâÖÖÈ¨ÏŞ.");
		pressanykey();
		outgame();
		return 0;
	}
	if(askyn("ÄúÈ·¶¨Òª¹ºÂòÂğ£¿",NA,NA) == NA ) {
		move(23,0);clrtoeol();
		prints("È¡Ïû½»Ò×¡£");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	switch(no) {
	case 1:
		hasperm = HAS_PERM(PERM_CLOAK);
		currentuser.userlevel |= PERM_CLOAK ;
		break;
	case 2:
		hasperm = HAS_PERM(PERM_SEECLOAK);
		currentuser.userlevel |= PERM_SEECLOAK ;
		break;
	case 3:
		hasperm = HAS_PERM(PERM_XEMPT);
		currentuser.userlevel |= PERM_XEMPT ;
		break;
	case 4:
		hasperm = HAS_PERM(PERM_FORCEPAGE);
		currentuser.userlevel |= PERM_FORCEPAGE ;
		break;
	case 5:
		hasperm = HAS_PERM(PERM_EXT_IDLE);
		currentuser.userlevel |= PERM_EXT_IDLE ;
		break;
	case 6: 
		hasperm = HAS_PERM(PERM_666LIFE); 
		currentuser.userlevel |= PERM_666LIFE ; 
		break;
	case 7: 
		hasperm = HAS_PERM(PERM_MAILINFINITY); 
		currentuser.userlevel |= PERM_MAILINFINITY ;
		break;
	}
	if(hasperm) {
		prints("ÔÚ½»Ò×½øĞĞÇ°ÄúÒÑ¾­ÓĞÁËÕâÖÖÈ¨ÏŞ, ËùÒÔÈ¡Ïû´Ë´Î½»Ò×. ");
		pressanykey();
		outgame();
		return 0;
	}
	if(currentuser.money < maxnum) {
		prints("\n¶Ô²»Æğ, ÄãÃ»ÓĞ×ã¹»µÄÇ®¹ºÂòÕâÖÖÈ¨ÏŞ.");
		pressanykey();
		outgame();
		return 0;
	}
	currentuser.money -= maxnum;
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);        move(23,0); clrtoeol();
	prints("Ë³ÀûÍê³É½»Ò×£¬»¶Ó­ÄúµÄÔÙ´Î»İ¹Ë¡£;P");
	sprintf(genbuf,"¹ºÂò[%s]£¬»¨·Ñ %d Ôª.",exchanges[no-1].title,maxnum);
	securityreport(genbuf);
	gamelog(genbuf);
	pressanykey();
	outgame();
	return 1;
}	


*/




int shopping()//modified by pax, 2005.09.21.
{
	int i,no,maxnum;
	int id,counts=0;
	int fd;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	char datafile[80] = BBSHOME "/etc/shop.ini";
	int type[64];
	EXCHANGES exchanges[64];
    FILE *fp;
    char t[256], *t1, *t2, *t3;
	struct gift cgift;

    fp = fopen (datafile, "r");
    if (fp == NULL)
        return;
    while (fgets (t, 255, fp) && counts <= 10)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL)
            continue;
		strncpy (exchanges[counts].title, t1, 16);
		exchanges[counts].value=atoi(strncpy (ans, t2, 36));
		type[counts] = t3 ? atoi (t3):0;
        counts++;
    }
    fclose (fp);

	if(gotomarket("ÊĞ³¡¹ºÎïÖĞĞÄ")) return 1;
	prints("½ñÈÕÉÌÆ·±¨¼Û:");
	for(no = 0; no < counts; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ôª", exchanges[no].value);
	}
	move(14,0);
	prints("ÄúÄ¿Ç°ÉĞÓĞ %d Ôª (½±ÕÂ %d ¸ö)\n",	currentuser.money,currentuser.medals);
	if (!gettheuserid(15,"ÄúÏë¸øË­ÂòÀñÎï£¿ÇëÊäÈëËû/ËıµÄÕÊºÅ: ",&id))
	{
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	getdata(15, 0, "ÄúÏë¹ºÂòÄÄÒ»¼şÉÌÆ·£¿", ans, 3, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > counts ) {
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	maxnum = exchanges[no-1].value;
	if(currentuser.money < maxnum) {
		prints("\n¶Ô²»Æğ, ÄúµÄ×Ê½ğ²»¹»¡£");
		pressanykey();
		outgame();
		return 0;
	}
	if(askyn("ÄúÈ·¶¨Òª¹ºÂòÂğ£¿",NA,NA) == NA ) {
		move(23,0);
		clrtoeol();
		prints("È¡Ïû½»Ò×¡£");
		pressanykey();
		outgame();
		return 0;
	}
	currentuser.money -= maxnum;
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	sprintf(ans,"%s ¸ø %s ¼ÄÀ´ÁËÀñÎï",currentuser.userid, lookupuser.userid);
	sprintf(genbuf,"\n %s :\n\n\tÄúºÃ£¡\n\n\tÄúµÄÅóÓÑ %s ¸øÄú¼ÄÀ´ÀñÎï£º %s¡£\n",lookupuser.userid,currentuser.userid,exchanges[no-1].title);
	for (i = 0; i < 5; i++)
		ps[i][0] = '\0';
	move (16, 0);
	prints ("ÇëÊäÈë×ªÕÊ¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
	for (i = 0; i < 5; i++)
	{
		getdata (i + 17, 0, ": ", ps[i], STRLEN - 5, DOECHO, YEA);
		if (ps[i][0] == '\0')
			break;
	}

		for (i = 0; i < 5; i++)
		{
			if (ps[i][0] == '\0')
				break;
			if (i == 0)
				strcat (genbuf, "\n\n¸½ÑÔ£º\n");
			strcat (genbuf, ps[i]);
			strcat (genbuf, "\n");
		}		
	sprintf(ans,"%s Îª %s ¹ºÂò[%s]£¬»¨·Ñ %d Ôª.",currentuser.userid,lookupuser.userid,exchanges[no-1].title,maxnum);
	if(askyn("ÊÇ·ñ¹«¿ª´Ë´Î½»Ò×ĞĞÎª£¿",YEA,YEA) == YEA ) {
		strcpy (currboard, "gift");
		autoreport(ans,genbuf,lookupuser.userid, NULL);
		strcpy (currboard, "giftadmin");
		autoreport(ans,genbuf,NULL, NULL);
	}
	else
	{
	bankreport(ans,genbuf,lookupuser.userid,NA);
	strcpy (currboard, "giftadmin");
	autoreport(ans,genbuf,NULL, NULL);
	}

	//bankreport(t3,"create","pax",NULL);
	/*Ğ´ÈëÊÕÀñÈËµÄ¸öÈËÎÄ¼ş*/
	if(type[no-1]){
	sethomefile(genbuf,lookupuser.userid,"gift.dat");
	fd=open(genbuf,O_APPEND | O_WRONLY | O_CREAT ,0644 );
	strcpy(cgift.id,currentuser.userid);
	strcpy(cgift.name,exchanges[no-1].title);
	lseek(fd, 0, SEEK_END);
	write(fd,&cgift,sizeof(cgift));
	close(fd);
}
	securityreport(ans);	
	gamelog(ans);
	move(22,0);
	prints("Ë³ÀûÍê³É½»Ò×£¬»¶Ó­ÄúµÄÔÙ´Î»İ¹Ë¡£;P");
	pressanykey();
	outgame();
	return 1;
}	


bankreport (title, str, userid, sysboard)
char *title;
char *str;
char *userid;
char *sysboard;
{
	FILE *se;
	char fname[STRLEN];
	int savemode;
	savemode = uinfo.mode;
	report (title);
	sprintf (fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
	if ((se = fopen (fname, "w")) != NULL)
	{
		fprintf (se, "%s", str);
		fclose (se);
		if (userid != NULL)
			mail_file (fname, userid, title);
		unlink (fname);
		modify_user_mode (savemode);
	}
}


/*µ÷ÕûÒøĞĞ´æ¿î*/

/*int shopping_admin()//modified by pax, 2005.09.21.
{
	int i,no,hasperm=1,maxnum;
	int id,id2,counts=0;
	int fd;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	char datafile[80] = BBSHOME "/etc/shop.ini";
	int type[64];
	EXCHANGES exchanges[64];
    FILE *fp;
    char t[256], *t1, *t2, *t3;
	struct gift cgift;

    fp = fopen (datafile, "r");
    if (fp == NULL)
        return;
    while (fgets (t, 255, fp) && counts <= 10)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL)
            continue;
		strncpy (exchanges[counts].title, t1, 16);
		exchanges[counts].value=atoi(strncpy (ans, t2, 36));
		type[counts] = t3 ? atoi (t3):0;
        counts++;
    }
    fclose (fp);

	prints("½ñÈÕÉÌÆ·±¨¼Û:");
	for(no = 0; no < counts; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ôª", exchanges[no].value);
	}
	move(14,0);
	if (!gettheuserid(15,"»ñÒæÕß: ",&id2))
	{
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	getdata(15, 0, "ÄúÏë¹ºÂòÄÄÒ»¼şÉÌÆ·£¿", ans, 3, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > counts ) {
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}
	
	if(askyn("È·¶¨Òª¹ºÂòÂğ£¿",NA,NA) == NA ) {
		move(23,0);
		clrtoeol();
		prints("È¡Ïû½»Ò×¡£");
		pressanykey();
		outgame();
		return 0;
	}
	/*Ğ´ÈëÊÕÀñÈËµÄ¸öÈËÎÄ¼ş
	if(type[no-1]){
	sethomefile(genbuf,lookupuser.userid,"gift.dat");
	fd=open(genbuf,O_APPEND | O_WRONLY | O_CREAT ,0644 );

	if (!gettheuserid(15,"¹ºÂòÕß: ",&id))
	{
		prints("\nÊäÈë´íÎó¡£È¡Ïû½»Ò× ...");
		pressanykey();
		outgame();
		return 0;
	}

	strcpy(cgift.id,lookupuser.userid);
	strcpy(cgift.name,exchanges[no-1].title);
	lseek(fd, 0, SEEK_END);
	write(fd,&cgift,sizeof(cgift));
	close(fd);
}

	pressanykey();
	return 1;
}	*/
/*int refreshbank(){

struct bankbill Bill;
struct userec user;	
struct callbill cBill;
long size,size1;
int money;
struct stat kkk;
struct stat kkk1;
int id;
int i;
long count;
int fd,fd1,fd2;
char temp[60000];
char temp2[60000];
char mail_title[STRLEN]={0};
char mail_buffer[1024] = {0};
char someoneID[20];
char fname[80];
clear();

  
	fd=open(PASSFILE,O_RDWR);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	sprintf (mail_title,"%s","²âÊÔ²éÑ¯");
	
	  while(size>0){
	  read(fd,&user,sizeof(struct userec));
	  fd1=open("bank/bank",O_RDWR | O_CREAT );
	  stat("bank/bank",&kkk1);
	  size1=kkk1.st_size/sizeof(struct bankbill);	
	  i=0;
	  count=0;
	  while (i<size1)
	  {
	  read(fd1,&Bill,sizeof(struct bankbill));
	  if((strcmp(Bill.userid,user.userid)==0)&&(Bill.dateout==0))
	  {
	  count=count+Bill.money;
	  }
	  i++;
	  }
	  sprintf (mail_buffer,"\n%-15s,×ÜÕË»§:%ld,¶¨ÆÚ:%ld,",user.userid,user.inbank,count);
	  strcat (temp, mail_buffer);
	  
		sprintf (fname,"bank/%s.bank", user.userid);
		if(fd2=open(fname,O_RDONLY )!=-1)
		{
		fd2=open(fname,O_RDWR);
		read(fd2,&cBill,sizeof(struct callbill));//¶ÁÈ¡Ô­ÓĞĞÅÏ¢
		close(fd2);
		}
		else
		{
		cBill.principal=0;
		}
		money=user.inbank-count-cBill.principal;
		sprintf (mail_buffer,"»îÆÚ=%5ld,Ô­ÏÈ´æ¿î=%5ld",cBill.principal,money);
		strcat (temp, mail_buffer);
		if(money!=0)
		{
		
		  sprintf (mail_buffer,"\n%-15s£¬Ô­ÏÈ´æ¿î£º%-5ld",user.userid,money);
		  strcat (temp2, mail_buffer);
		  
			id = getuser(user.userid);
			user.money += money;
			user.inbank -= money;
			substitute_record(PASSFILE, &user, sizeof(struct userec),id);
			
			  
				}
				close(fd1);	
				size--;
				}
				close(fd);
				bankreport(mail_title, temp, currentuser.userid, NULL);
	bankreport(mail_title, temp2, currentuser.userid, NULL);
	
}*/


int research_current_deposit(){
	struct userec user;	
	struct callbill cBill;
	struct stat kkk;
	int all;
	int fd,fd1;
	int size;
	int cmoney;
	double cinterest;
	char temp[65535]={0};
	char mail_title[STRLEN]={0};
	char mail_buffer[1024] = {0};
	char someoneID[20];
	char fname[80];
	char ans[10];


	
	cmoney=0;
	cinterest=0.0;
	clear();
	all=0;
    modify_user_mode (QUERY);
    usercomplete ("ÇëÊäÈëÄúÏë²éÑ¯µÄÕÊºÅ: ", someoneID);
    if (someoneID[0] == 0)
    {
        getdata (0, 30, "²éÑ¯ËùÓĞµÄ×÷ÕßÂğ?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return;
        }
        else
            all = 1;
    }
	else if (!getuser (someoneID))
    {
        prints ("²»ÕıÈ·µÄÊ¹ÓÃÕß´úºÅ\n");
        pressreturn ();
        return;
    }
	
	fd=open(PASSFILE,O_RDWR);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	
	if(all==1){
		
		sprintf (mail_title,"%s(ÀÛ¼Æ%d)","ËùÓĞÓÃ»§»îÆÚÀûÂÊ²éÑ¯",size);
		while(size>0){


	


			sprintf (fname,"bank/%s.bank", user.userid);
			if(fd1=open(fname,O_RDONLY )!=-1)
			{	
				get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified
				cmoney+=cBill.principal;
				cinterest+=cBill.interest;
			}
			else
			{
				cBill.principal=0;
				cBill.interest=0.0;
			}
			sprintf (mail_buffer,"\n%-15s,»îÆÚ£º%16ld,ÀûÏ¢£º%8f",user.userid,cBill.principal,cBill.interest);
			strcat (temp, mail_buffer);
			size--;
		}
		sprintf (mail_buffer,"\n×ÜÊı %ld,×ÜÀûÏ¢£º%8.4f",cmoney,cinterest);
		strcat (temp, mail_buffer);
		
	}
	else
	{	
		sprintf (mail_title,"%s»îÆÚÀûÂÊ²éÑ¯",someoneID);
		sprintf (fname,"bank/%s.bank", someoneID);
		if(fd1=open(fname,O_RDONLY )!=-1)
		{
			get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified

		}
		else
		{
			cBill.principal=0;
		}
		sprintf (temp,"\n%-15s,»îÆÚ£º%6ld,ÀûÏ¢£º%8f",someoneID,cBill.principal,cBill.interest);	
	}
	close(fd);
	bankreport(mail_title, temp, currentuser.userid, NULL);
}