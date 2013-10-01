#include "bbs.h"
#include "money.h"

//"bank/rate"

struct salary
{
	char userid[IDLEN + 2];
	int money;
};

int rate_edit()
{
	struct rate bankrate;
	char money[9];
	char sysbuf[STRLEN];
	char mail[1024];
	int fd;

	clear();
	modify_user_mode (QUERY);	
	if(fd=open("bank/rate",O_RDWR)!=-1)
	{
		fd=open("bank/rate",O_RDWR | O_APPEND);		
		lseek(fd,-sizeof(struct rate),SEEK_END);
		read(fd,&bankrate,sizeof(struct rate));
		move( 0, 0);
		sprintf (mail,"原先利率\n");
		prints("现行利率：\n");
		sprintf (sysbuf,"一月 %8f 二月 %8f 三月 %8f\n",bankrate.timedepo_01,bankrate.timedepo_02,bankrate.timedepo_03);
		prints(sysbuf);
		strcat (mail,sysbuf);
		sprintf (sysbuf,"四月 %8f 五月 %8f 六月 %8f 活期 %8f\n",bankrate.timedepo_04,bankrate.timedepo_05,bankrate.timedepo_06,bankrate.currdepo);
		prints(sysbuf);
		strcat (mail,sysbuf);
	}
	else
	{
		fd=open("bank/rate",O_APPEND | O_WRONLY | O_CREAT ,0644 );
		move( 2, 0);
		prints("无现行利率\n");	
	}
	if (askyn ("要更改么", NA, NA) == YEA)
	{


		getdata(4, 0, "一月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_01 = atof(money);
		}			
		getdata(5, 0, "二月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_02 = atof(money);
		}
		getdata(6, 0, "三月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{	
			bankrate.timedepo_03 = atof(money);
		}
		getdata(7, 0, "四月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_04 = atof(money);
		}
		getdata(8, 0, "五月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_05 = atof(money);
		}
		getdata(9, 0, "六月 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{				
			bankrate.timedepo_06 = atof(money);
		}
		getdata(10, 0, "活期 :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{				
			bankrate.currdepo= atof(money);
		}
		if (askyn ("确认修改?", NA, NA) == YEA)
		{
			bankrate.time=time(0);
			write(fd,&bankrate,sizeof(struct rate));//重写数据
			close(fd);
			strcat (mail,"\n\n调整后的利率\n");
			sprintf (sysbuf,"一月 %8f 二月 %8f 三月 %8f\n",bankrate.timedepo_01,bankrate.timedepo_02,bankrate.timedepo_03);
			strcat (mail,sysbuf);
			sprintf (sysbuf,"四月 %8f 五月 %8f 六月 %8f 活期 %8f\n",bankrate.timedepo_04,bankrate.timedepo_05,bankrate.timedepo_06,bankrate.currdepo);
			strcat (mail,sysbuf);	


			move( 12, 0);
			prints("银行利率已经修改\n");
			sprintf(sysbuf, "银行利率更新");
			securityreport(sysbuf);
			strcpy (currboard, "MoneyAdmin");
			autoreport(sysbuf,mail, NULL, NULL);
			pressanykey();
			return 0;
		}
		else
		{
			close(fd);
			move( 12, 0);
			prints("银行利率没有修改\n");

			pressanykey();
			return 0;
		}

	}
}






int salary_read()
{
	char money[6];
	struct stat kkk;
	struct salary sal;
	int size;
	int fd;
	int first;
	char msg[2];
	int opt;
	char ans[2];
	char someoneID[14];
	char buf[80];

	modify_user_mode (QUERY);
	refresh();
	clear();

		move (1,0);
	prints("欢迎进入行长办公室\n");
	//prints("\n[1]查询存款 [2]调整利率 [3]查询工资 [4]调整工资\n");
	prints("\n[1]调整工资 [2]清除工资\n");
	getdata( 4, 0,"请输入操作:", msg, 2, DOECHO, YEA);
	if((msg[0] == '\n') || (msg[0] == '\0'))
	{
	return 0;
	}
	opt = atoi(msg);
	if (!isdigit(msg[0]) || opt <= 0 || opt > 2 || msg[0] == '\n' || msg[0] == '\0')
	{
	move(5,0);
	prints("对不起，思源湖大银行不提供此项服务！\n");
	pressanykey();
	return 0;
	}
	if(opt == 1)
	{
		clear();
		first=0;

		usercomplete ("请输入您想查询的帐号: ", someoneID);

		if (!getuser (someoneID))
		{
			prints ("不正确的使用者代号\n");
			pressreturn ();
			return;
		}

		fd=open("bank/bank.salary",O_RDWR | O_CREAT, 0644);
		stat("bank/bank.salary",&kkk);
		size=kkk.st_size/sizeof(struct salary);
		while (size>0)
		{
			read(fd,&sal,sizeof(struct salary));
			if(strcmp(sal.userid,someoneID)==0)
			{
				first=1;
				break;
			}
			size--;
		}

		if(first==1)
		{
			move (2,0);

			sprintf(buf,"%s 已经存在 目前工资为 %d \n", sal.userid,sal.money);
			prints (buf);	
			getdata (3, 0, "是否更改该用户工资?[Y/N]: ", ans, 7, DOECHO, YEA);
			if ((*ans != 'Y') && (*ans != 'y'))
			{
				return;
			}
			getdata(4, 0, "请输入调整后的工资 :", money, 6, DOECHO, YEA);
			sal.money = atoi(money);

			if(askyn ("确定", NA, NA) == YEA)
			{
				lseek(fd,-sizeof(struct salary),SEEK_CUR);
				write(fd,&sal,sizeof(struct salary));
			}
			close(fd);
		}
		else
		{
			getdata(2, 0, "输入该用户工资 :", money, 6, DOECHO, YEA);
			sal.money = atoi(money);	
			if(askyn ("确定", NA, NA) == YEA)
			{
				sprintf(sal.userid,"%s", someoneID);

				write(fd,&sal,sizeof(struct salary));
			}
			close(fd);

		}
	}	
	if(opt == 2)
	{
		if(askyn ("确定", NA, NA) == YEA)
		{
			unlink("bank/bank.salary");
		}
	}

}

int send_salary()
{

	struct stat kkk;
	struct salary sal;
	int id;
	int size;
	int fd;
	char buf[80];
	char mail_buf[65535];

	refresh();
	clear();
	modify_user_mode (ADMIN);
	if(askyn ("确定发放工资？", NA, NA) == YEA)
	{
		fd=open("bank/bank.salary",O_RDONLY );
		stat("bank/bank.salary",&kkk);
		size=kkk.st_size/sizeof(struct salary);
		sprintf(mail_buf,"\n思源实业银行工资发放系统 "
			"\n本贴由银行系统自动发布\n"
			);
		while (size>0)
		{
			read(fd,&sal,sizeof(struct salary));
			id = getuser(sal.userid);
			lookupuser.money+=  sal.money;
			substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
			sprintf(buf,"您上月的工资为 %d ", sal.money);
			bankreport("发工资啦！", buf, lookupuser.userid, NULL);
			sprintf(buf,"%s 的工资为 %d ", lookupuser.userid,sal.money);
			strcat (mail_buf, buf);
			size--;
		}
		close(fd);

		sprintf(buf,"上月工资已经发放 by %s ", currentuser.userid);
		securityreport(buf);
		strcpy (currboard, "MoneyAdmin");
		autoreport(buf,mail_buf, NULL, NULL);
	}


}



int count_all()
{
	/*	struct Money mdata;
	struct userec user;	
	struct callbill cBill;
	struct bankbill Bill;
	struct stat kkk;
	int result;
	int fd,fd1;
	long i;
	long size;
	long count;//总额
	long all_free;//总自由资金
	long all_principal;//总本金
	long all_coin;//游戏币
	double all_interest;//利息
	char temp[1024];
	//char coin_buf[655350];
	char mail_title[STRLEN]={0};
	char mail_buffer[1024];
	//	char someoneID[20];
	char fname[80];
	//	char ans[10];


	clear();
	modify_user_mode (QUERY);
	//初始化数据
	all_interest=0;
	all_principal=0;
	all_coin=0;
	all_free=0;
	count=0;
	result=0;
	sprintf (mail_title,"%s","思源湖金融状况汇总");
	//sprintf(coin_buf,"游戏币汇总\n\nedit by pax\n\n");
	//结束


	//=========================自由资金、活期和游戏币统计============================

	fd=open(PASSFILE,O_RDONLY);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	close(fd);
	i=0;
	while(i<size )
	{
	get_record (PASSFILE, &user,sizeof(struct userec), i+1);
	//自有资金
	all_free+=user.money;
	//活期统计
	sprintf (fname,"bank/%s.bank", user.userid);
	if(fd1=open(fname,O_RDONLY )!=-1)
	{
	get_record (fname, &cBill,sizeof(struct callbill), 1);
	all_principal+=cBill.principal;//本金累加
	all_interest+=cBill.interest;//利息累加
	}	
	//游戏币统计
	//	sprintf (fname, "home/%c/%s/game_money.dat", toupper (user.userid[0]),user.userid);
	sethomefile(fname, user.userid, "game_money.dat");
	if( dashf(fname) )
	{
	get_record (fname, &mdata,sizeof(struct Money), 1);
	all_coin+=(mdata.money);
	sprintf (temp,"\n%-15s%8ld",user.userid,mdata.money);
	//	strcat (coin_buf, temp);
	}

	/*	if(	strcmp(currentuser.userid, "pax")==0)
	{
	user.money += mdata.money;
	mdata.money= 0;
	substitute_record(PASSFILE, &user, sizeof(struct userec),i+1);
	sethomefile(fname, user.userid, "game_money.dat");
	fp = fopen(fname,"w");
	if(fp != NULL)
	{
	result = fwrite(&mdata, sizeof(mdata), 1, fp);
	fclose(fp);
	}
	bankreport("done",fname,"pax",NULL);
	}*
	/
	*		i++;
	}


	//bankreport("coin", coin_buf, currentuser.userid, NULL);
	sprintf (mail_buffer, "发自pax\n思源湖金融状况统计\n");
	sprintf (temp,"\n总共累计 自由资金 %10ld ",all_free);
	strcat (mail_buffer, temp);
	sprintf (temp,"\n总共累计 游戏币   %10ld ",all_coin);
	strcat (mail_buffer, temp);
	sprintf (temp,"\n总共累计 活期存款 %10ld 利息：%10f",all_principal,all_interest);
	strcat (mail_buffer, temp);


	//*=========================定期统计============================
	i=0;
	fd=open("bank/bank",O_RDWR);
	stat("bank/bank",&kkk);
	size=kkk.st_size/sizeof(struct bankbill);
	while (i<size)
	{
	read(fd,&Bill,sizeof(struct bankbill));
	if(Bill.dateout==0)
	{
	count=count+Bill.money;
	}			
	i++;
	}
	sprintf (temp,"\n总共累计 定期存款 %10ld",count);
	strcat (mail_buffer, temp);

	sprintf (temp,"\n思源湖总资产      %10ld",count+all_principal+all_free);
	strcat (mail_buffer, temp);
	close(fd);	
	bankreport(mail_title, mail_buffer, currentuser.userid, NULL);*/
}

int update_deposit(userid)//存款利息更新
char *userid;
{
	char fname[80];
	struct callbill cBill;
	struct rate bankrate;
	int fd;
	double callrate=0.0005;
	if(fd=open("bank/rate",O_RDONLY)!=1)
	{
		fd=open("bank/rate",O_RDONLY);
		lseek(fd,-sizeof(struct rate),SEEK_END);
		read(fd,&bankrate,sizeof(struct rate));	
		callrate=bankrate.currdepo;
		close(fd);
	}
	sprintf (fname,"bank/%s.bank", userid);
	get_record (fname, &cBill,sizeof(struct callbill), 1);
	cBill.interest+=cBill.principal*callrate*(time(0)-cBill.date)/86400;
	cBill.date=time(0);
	substitute_record(fname, &cBill,sizeof(struct callbill), 1);
}


int house_agent_input()
{
	char msg[8];
	char fname[80];
	char mailbuf[256];
	int fd;
	int size;
	struct stat kkk;
	struct house chouse;


	clear();
	modify_user_mode (QUERY);
	usercomplete ("请输入屋主的帐号: ", chouse.ida);

	if (!getuser (chouse.ida))
	{
		prints ("不正确的使用者代号\n");
		pressreturn ();
		return;
	}

	if(askyn ("是否为单人居住？", YEA, NA) == NA)
	{
		usercomplete ("请输入对方的帐号: ", chouse.idb);
		if (!getuser (chouse.idb))
		{
			prints ("不正确的使用者代号\n");
			pressreturn ();
			return;
		}
	}

	getdata(3, 0, "输入房子的总值 :", msg, 8, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		chouse.price=atoi(msg);	
	}
	getdata(3, 0, "输入房子的号码 :", msg, 5, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		strcpy(chouse.num,msg);	
	}
	getdata(3, 0, "输入房子的利率 :", msg, 8, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		chouse.rate=atof(msg);	
	}

	chouse.datebuy=time(0);
	chouse.interest=0.0;
	chouse.lastpay=time(0);
	chouse.payed=0;

	sprintf (fname,"./.House");
	fd=open(fname,O_WRONLY |O_APPEND | O_CREAT, 0644);	
	write(fd,&chouse,sizeof(struct house));
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct house);
	close(fd);

	sprintf(mailbuf,"您的房产编号为 %d 房舍号码为 %s 房价/ 分期付款的日利率为 %d/%f", size,chouse.num,chouse.price,chouse.rate);
	strcpy (currboard, "MoneyAdmin");
	//autoreport("您的房产已经安置",mailbuf,NULL, NULL);
	autoreport("您的房产已经安置",mailbuf, chouse.ida, NULL);
	sprintf(mailbuf,"售出房子 编号为 %d 房舍号码为 %s 经手人 %s",size,chouse.num,currentuser.userid);
	securityreport(mailbuf);
}


int house_agent_pay()
{
	char fname[80];
	int fd;
	struct house chouse;
	struct stat kkk;
	int size;
	int id;
	int usernum;
	char msg[8];
	int num;

	clear();
	modify_user_mode (QUERY);


	sprintf (fname,"./.House");
	fd=open(fname,O_RDONLY);	
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct house);
	close(fd);


	getdata(3, 0, "输入房产编号:", msg, 4, DOECHO, YEA);
	id = atoi(msg);
	if( !isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0' || id<0 || id>size)
	{	
		move(4,0);
		prints("对不起，输入错误！\n");
		pressanykey();
		return 0;
	}


	sprintf (fname,"./.House");
	get_record (fname, &chouse,sizeof(struct house), id);	
	num=chouse.price-chouse.payed;
	if(num==0)
	{
		move(4,0);
		prints(" 房产已经付清！\n");
		pressanykey();
		return 0;
	}

	//利息更新


	chouse.interest+=((double)(time(0)-chouse.lastpay)/86400)*chouse.rate*num;

	move(4,0);

	sprintf (fname,"此房产的户主为 %s ,房舍号码为 %s",chouse.ida,chouse.num);
	prints(fname);
	sprintf (fname,"\n总产值为 %ld ,已经支付 %d ",chouse.price,chouse.payed);
	prints(fname);
	sprintf (fname,"\n还需支付 %ld ,其中利息为 %8ld ",(int)(num+chouse.interest),(int)chouse.interest);
	prints(fname);

	getdata(10, 0, "输入支付金额 :", msg, 8, DOECHO, YEA);
	num = (int)atoi(msg);
	if( !isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0' || num<0 || num>chouse.price-chouse.payed)
	{	
		move(11,0);
		prints("对不起，输入错误！\n");
		pressanykey();
		return 0;
	}
	if(num>currentuser.money)
	{
		move(11,0);
		prints("对不起，您的现金不足！\n");
		pressanykey();
		return 0;
	}



	currentuser.money-=num;

	usernum=getuser(currentuser.userid);

	substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);

	if((int)chouse.interest<num)
	{
		chouse.interest-=(int)chouse.interest;
		chouse.payed+=(num-(int)chouse.interest);
	}		
	else
	{
		chouse.interest-=num;
	}

	chouse.lastpay=time(0);
	sprintf (fname,"./.House");
	substitute_record(fname, &chouse, sizeof(struct house),id);

	sprintf (fname,"%s 支付房款 %d ,%s",currentuser.userid,num,chouse.num);
	strcpy (currboard, "MoneyAdmin");
	autoreport(fname,fname, NULL, NULL);
}

/*pax@syb bbs 05-09-29*/
/*全站红包*/
int festival_money(){

	struct userec user;	
	char ps[STRLEN];
	char genbuf[255];
	char tmpbname[20];
	struct stat kkk;
	int fd;
	long i;
	long size;
	int money;//总额
	char fname[80];
	clear();
	modify_user_mode (ADMIN);
	getdata(10, 0, "请输入红包金额 [1 - 50000] :", fname, 6, DOECHO, YEA);
	money = atoi(fname);
	if (!isdigit(fname[0]) || money <= 0 || money > 50000 || fname[0] == '\n' || fname[0] == '\0')
	{
		move( 12, 0);
		prints("对不起，红包金额必须在 1 - 50000 之间！\n");
		pressanykey();
		return 0;
	}
	move (14, 0);
	prints ("请输入发红包的原因(按 Enter 结束)");
	getdata (15, 0, ": ", ps, STRLEN - 5, DOECHO, YEA);

	sprintf(genbuf,"\n亲爱的用户:\n\n"
				"    在此喜庆之日，思源湖大银行给您送上红包一个(%d湖币)。\n\n\n",money);				
	strcat(genbuf,ps);

	if (askyn ("确认要给全站发红包吗？", YEA, NA) == YEA)
	{
		fd=open(PASSFILE,O_RDONLY);
		stat(PASSFILE,&kkk);
		size=kkk.st_size/sizeof(struct userec);
		close(fd);
		i=1;
		while(i<=size )
		{
			get_record (PASSFILE, &user,sizeof(struct userec), i);
			
			if(((time (0) - user.lastlogin)/3600 < 720 ) && //30天内没有访问的
				((time (0) - user.firstlogin)/3600 > 84))//网龄7天以上
			{
				user.money+=money;
				substitute_record(PASSFILE, &user, sizeof(struct userec),i);
				//strcpy (tmpbname, currboard);
				//strcpy (currboard, NULL);
				bankreport("您收到一个红包",genbuf, user.userid, NULL);
				//strcpy (currboard, tmpbname);
			}
			i++;
		}
		sprintf(fname, "%s 发送全站红包 %d 元", currentuser.userid, money);
		securityreport(fname);

		move(20,0);
		prints("红包发送！\n");
		pressanykey();
		return 0;
	}
}
