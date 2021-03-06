/* 快意灌水站 交易市场代码 1999.12.19 */
#include <string.h>
#include <stdlib.h>

#include "bbs.h"
#include "money.h"

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
      prints("对不起，您已经有另外一个窗口在使用本站的游戏功能。\n");
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
   prints("欢迎进入 [[32m%s[37m]....\n\n",title);
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
	char msg[3];
	char money[7];
	int opt;
	int num_money;
	int id;

	id = getuser(currentuser.userid);

	if(gotomarket("流氓大银行")) 
		return 0;
	prints("欢迎光临 流氓大银行\n\n");

	prints("您目前携带现金 %d 元，银行帐户结余 %d 元。\n\n", currentuser.money, currentuser.inbank);

	getdata( 8, 0,"请输入您要进行的操作 [1] 取款 [2]存款 :", msg, 2, DOECHO, /*NULL,*/ YEA);
	opt = atoi(msg);
	if (!isdigit(msg[0]) || opt <= 0 || opt > 2 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(10,0);
		prints("对不起，流氓大银行不提供此项服务！\n");
		pressanykey();
		outgame();
		return 0;
	}

	if(opt == 1)
	{
		getdata(10, 0, "请输入你要取款的金额 [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("对不起，取款金额必须在 1 - 50000 之间！\n");
			pressanykey();
			outgame();
			return 0;
		}

		if(currentuser.inbank < num_money)
		{
			move(12, 0);
			prints("对不起，您的流氓大银行帐户不能透支！\n");
			pressanykey();
			outgame();
			return 0;
		}

		currentuser.inbank -= num_money;
		currentuser.money += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("您的取款手续已经完成\n");
		pressanykey();
		outgame();
		return 1;

	}
		
	if(opt == 2)
	{
		getdata(10, 0, "请输入你存款的金额 [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("对不起，存款金额必须在 1 - 50000 之间！\n");
			pressanykey();
			outgame();
			return 0;
		}

		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("对不起，您的现金不足！\n");
			pressanykey();
			outgame();
			return 0;
		}

		currentuser.inbank += num_money;
		currentuser.money -= num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("您的存款手续已经完成\n");
		pressanykey();
		outgame();
		return 1;

	}

//	prints();
	move(12,0);
	prints("今日流氓大银行休息:)\n");
	pressanykey();
	outgame();
	return 1;


}


int lending()
{
   int     id, canlending=0,maxnum = 0, num = 0,u_money=0;
   char	ans[STRLEN];
   time_t 	now=time(0);
   extern int gettheuserid();
   
   if(gotomarket("交易市场")) return 0;
   maxnum = currentuser.money - 1000;
   prints("[m欢迎使用[1;33m"BBSNAME"[m交易市场[32m友情转帐[m功能");
   prints("\n\n您目前的情况是：\n注册天数([32m%d[m 天) "
          "上站总时数([32m%d[m 小时) "
	  "[44;37m可转帐资金([32m%d[37m 元)[m",
	  (now - currentuser.firstlogin)/86400,
	  currentuser.stay/3600,currentuser.money-1000);
   if(currentuser.stay<=36000||now-currentuser.firstlogin<=2592000||maxnum<=0){
      prints("\n\n目前市场规定： 参与[32m友情转帐[m的 ID 必须具备以下"
             "全部条件：\n"
	     "    1. 本帐号注册天数超过 30 天;\n"
	     "    2. 总上站时数超过 10 小时;\n"
	     "    3. 最终拥有存款超过 1000 元.\n");
      prints("\n\n根据市场规定，您目前尚没有[32m友情转帐[m的资格。\n");
      pressanykey();
      outgame();
      return 0;
   }
   if (!gettheuserid(9,"您想转帐到谁的帐户上？请输入他的帐号: ",&id))return 0;
   u_money = lookupuser.money+lookupuser.medals*10000+ lookupuser.inbank;
   if(!strcmp(currentuser.userid,lookupuser.userid) ||u_money > 20000){
         prints("\n对不起，%s 目前尚不需要您的转帐！",lookupuser.userid);
	 pressanykey();
         outgame();
	 return 0;
   }
   move(10,0);
   canlending = (maxnum>(20000-u_money))?(20000-u_money):maxnum;
   prints("您将转帐给 [[1;32m%s[m]，转帐最多金额为 [1;33m%d[m 元。",
      lookupuser.userid, canlending);
   getdata(12, 0, "确实要转帐，请输入转帐数目，否则，请直接回车取消转帐: ",
      ans, 6, DOECHO, YEA);
   num = atoi(ans);
   if ( num <= 0 || num > canlending ) {
      prints("\n输入数目有错误。 取消交易 ...");
      pressanykey();
      outgame();
      return 0;
   }
   set_safe_record();
   if(currentuser.money - 1000  != maxnum) {
      prints("\n对不起，您的可转帐资金有所变化，取消此次交易！");
      prints("\n请重新执行本交易。");
      pressanykey();
      outgame();
      return 0;
   }
   currentuser.money -= num;
   substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);
   lookupuser.money += num;
   substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
   prints("\n谢谢您，您已经给 [1;32m%s[m 转帐 [1;33m%d[m 元。",
      lookupuser.userid,num);
   prints("\n为表示对你的转帐行为的感谢，本站已经用信件通知了他。");
   sprintf(genbuf,"给 %s 转帐 %d 元",lookupuser.userid,num);
   gamelog(genbuf);
   sprintf(ans,"%s 给您寄来了 %d 元友情转帐",currentuser.userid,num);
   sprintf(genbuf,"\n %s :\n\t您好！\n\t您的朋友 %s 给您寄来了 %d 元友情转帐资金。\n\t请您在 Market 板对他的无私行为发文表示感谢，\n\t这样，您就可以获得这笔友情转帐资金。\n\n\t当然，您也可以退出一次后，再进入本站，\n\t一样可以获得这笔友情转帐资金。\n  ",lookupuser.userid,currentuser.userid,num);
   autoreport(ans,genbuf,NA,lookupuser.userid);
   pressanykey();
   outgame();
   return 1;
}

int popshop(void)
{
   int no,num,maxnum,templog;
   char ans[10];
   
   EXCHANGES exchanges[10] = {
      {"上站次数",2},
      {"文章数",5},
      {"奖章数",10000},
      {"隐身术",40000},
      {"看穿隐身术",100000},
      {"帐号永久保留",200000},
      {"强制呼叫",20000},
      {"延长发呆时间",10000},
      {"666 生命力",20000},
      {"信箱无上限",40000}
   };
   if(gotomarket("市场典当行")) return 1;
   prints("今日典当报价:");
   for(no = 0; no < 10; no ++) {
      move(5+no, 2);
      prints("(%2d) %s",no+1,exchanges[no].title);
      move(5+no, 20);
      prints("==> %6d 元", exchanges[no].value);
   }
   move(16,0);
   prints("[0;1m您目前的情况是: (1)上站次数([32m%d[m)  "
          "(2)文章数([32m%d[37m)  (3)奖章数.([32m%d[37m)\n",
	  currentuser.numlogins,currentuser.numposts,currentuser.medals);
   getdata(18, 0, "您想典当哪一项？", ans, 10, DOECHO, YEA);
   no = atoi(ans);
   if ( no < 1 || no > 10 ) {
      prints("\n输入错误，取消交易 ...");
      pressanykey();
      outgame();
      return 0;
   }
   move(18, 30);
   prints("你选择典当“[32m%s[m”。",exchanges[no-1].title);
   if(no>3){
      set_safe_record();
      maxnum = exchanges[no-1].value;
      switch(no) {
         case 4:
	    if(!HAS_PERM(PERM_CLOAK)) { num = 0; break; }
	    break;
	 case 5:
	    if(!HAS_PERM(PERM_SEECLOAK)) { num = 0; break; }
	    break;
	 case 6:
	    if(!HAS_PERM(PERM_XEMPT)) { num = 0; break; }
	    break;
	 case 7:
	    if(!HAS_PERM(PERM_FORCEPAGE)) { num = 0; break; }
	    break;
	 case 8:
	    if(!HAS_PERM(PERM_EXT_IDLE)) { num = 0; break; }
	    break;
	 case 9:
	    if(!HAS_PERM(PERM_666LIFE)) { num = 0; break; }
	    break;
	 case 10:
	    if(!HAS_PERM(PERM_MAILINFINITY)) { num = 0; break; }
	    break;
      }
      prints("\n\n");
      if(!num) {
         prints("对不起, 你还没有这种权限. ");
	 pressanykey();
         outgame();
	 return 0;
      }
      if(askyn("您确定要典当吗？",NA,NA) == NA ) {
         move(23,0);clrtoeol();
	 prints("交易取消 ...");
	 pressanykey();
         outgame();
	 return 0;
      }
      set_safe_record();
      switch(no) {
         case 4:
	    num = HAS_PERM(PERM_CLOAK);
	    currentuser.userlevel &= ~PERM_CLOAK ;
	    break;
         case 5:
	    num = HAS_PERM(PERM_SEECLOAK);
	    currentuser.userlevel &= ~PERM_SEECLOAK ;
	    break;
         case 6:
	    num = HAS_PERM(PERM_XEMPT);
	    currentuser.userlevel &= ~PERM_XEMPT ;
	    break;
         case 7:
	    num = HAS_PERM(PERM_FORCEPAGE);
	    currentuser.userlevel &= ~PERM_FORCEPAGE ;
	    break;
	 case 8:
	    num = HAS_PERM(PERM_EXT_IDLE);
	    currentuser.userlevel &= ~PERM_EXT_IDLE ;
	    break;
	 case 9:
	    num = HAS_PERM(PERM_666LIFE);
	    currentuser.userlevel &= ~PERM_666LIFE ;
	    break;
	 case 10:
	    num = HAS_PERM(PERM_MAILINFINITY);
	    currentuser.userlevel &= ~PERM_MAILINFINITY ;
	    break;
      }
      if(!num) {
         prints("对不起, 你的数据发生了变化, 你目前没有这种权限. ");
	 pressanykey();
         outgame();
	 return 0;
     }
  } else {
      if( no == 1 )maxnum = currentuser.numlogins-1;
      else if ( no == 2) maxnum = currentuser.numposts;  
      else	maxnum = currentuser.medals;
      templog = maxnum;
      sprintf(genbuf,"您想典当多少呢(最多%d)？",maxnum);
      getdata(19, 0, genbuf,ans, 10, DOECHO, YEA);
      num = atoi(ans);
      if ( num <= 0 || num > maxnum ) {
         prints("输入错误。取消交易 ...");
	 pressanykey();
         outgame();
	 return 0;
      }
      maxnum = num*exchanges[no-1].value;
      move(19,0);
      prints("您共计典当%s[32m%d[m 份， %s [33m%d[m 元。\n",
         exchanges[no-1].title,num,"可以获得",maxnum);
      if(askyn("您确定要典当吗？",NA,NA) == NA ) {
         move(21,0);clrtoeol();
	 prints("取消交易 ...");
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
	 prints("对不起, 在交易过程中您的数据发生了变化.\n");
	 prints("为保证交易的正常进行, 此次交易取消.\n您可以重新进行本交易.");
	 pressanykey();
         outgame();
	 return 0;
      }
   }
   currentuser.money += maxnum;
   if( currentuser.money > 900000000 ){
      move(21,0); clrtoeol();
      prints("对不起，交易数据过大，产生溢出，请重新交易！\n");
      prints("建议您将一部分金钱存入银行。");
      pressanykey();
      outgame();
      return 0;
   }
   substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
   move(21,0); clrtoeol();
   prints("顺利完成交易，欢迎您的再次惠顾。;P");
   sprintf(genbuf,"典当%s %d 份，花费 %d 元.",exchanges[no-1].title,num,maxnum);
   gamelog(genbuf);
   pressanykey();
   outgame();
   return 1;
}

int shopping()//modified by smartfish, 2003.09.08.
{
   int no,hasperm=1,maxnum;
   char ans[10];
   EXCHANGES exchanges[10] = {
      {"隐身术",100000},
      {"看穿隐身术",4000000},
      {"帐号永久保留",4000000},
      {"强制呼叫",4000000},
      {"延长发呆时间",400000},
      {"666生命力",1000000},
      {"无上限信箱",9900000}
   };
   if(gotomarket("市场购物中心")) return 1;
   prints("今日商品报价:");
   for(no = 0; no < 7; no ++) {
      move(5+no, 2);
      prints("(%2d) %s",no+1,exchanges[no].title);
      move(5+no, 20);
      prints("==> %6d 元", exchanges[no].value);
   }
   move(16,0);
   prints("您目前尚有 %d 元 (奖章 %d 个)\n",
   currentuser.money,currentuser.medals);
   getdata(18, 0, "您想购买哪一件商品？", ans, 10, DOECHO, YEA);
   no = atoi(ans);
   if ( no < 1 || no > 7 ) {
      prints("\n输入错误。取消交易 ...");
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
      prints("您已经有这种权限, 不需要再购买. ");
      pressanykey();
      outgame();
      return 0;
   }
   if(currentuser.money < maxnum) {
      prints("\n对不起, 你没有足够的钱购买这种权限.");
      pressanykey();
      outgame();
      return 0;
   }
   if(askyn("您确定要购买吗？",NA,NA) == NA ) {
      move(23,0);clrtoeol();
      prints("取消交易。");
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
      prints("在交易进行前您已经有了这种权限, 所以取消此次交易. ");
      pressanykey();
      outgame();
      return 0;
   }
   if(currentuser.money < maxnum) {
      prints("\n对不起, 你没有足够的钱购买这种权限.");
      pressanykey();
      outgame();
      return 0;
   }
   currentuser.money -= maxnum;
   substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);        move(23,0); clrtoeol();
   prints("顺利完成交易，欢迎您的再次惠顾。;P");
   sprintf(genbuf,"购买[%s]，花费 %d 元.",exchanges[no-1].title,maxnum);
   gamelog(genbuf);
   pressanykey();
   outgame();
   return 1;
}	


/**************************************************************************/
/*   游戏数据交易市场						          */
/**************************************************************************/

int incmoney(char *userid, int money)
{
   FILE *fp;
   struct Money mdata;
   char buf[STRLEN]; 
   int result =0;
   
   sethomefile(buf,userid,"game_money.dat");
   if(dashf(buf)) update_game_money();
   fp = fopen(buf,"r");
   if(fp==NULL) return 0;
   result = fread(&mdata,sizeof(mdata),1,fp);
   fclose(fp);
   if(result <=0) return 0;
   if(money == 0) return mdata.money;
   fp = fopen(buf,"w");
   if(fp==NULL) return 0;
   mdata.money += money;
   result = fwrite(&mdata, sizeof(mdata), 1, fp);
   fclose(fp);
   if(result == 0)return 0;
   return money;
}

int inmoney(int money) 
{ 
   return incmoney(currentuser.userid,money); 
}   

int demoney(int money)
{
   return inmoney((-1)*money);
}

int check_money(int money)
{
   int now_money = inmoney(0);
   if(inmoney(0) < money) {
      move(22, 0);
      clrtobot();
      prints("抱歉！您只有 %d 元赌金", money,now_money);
      pressanykey();
      return 1;
   }
   return 0;
}

void show_money(int money, char *welcome,int Clear)
{
   if(Clear) clear();
   if(welcome) ansimore(welcome, NA);
   move(22, 0);
   clrtobot();
   prints("[0;1;37;44m                  你现有现金: [36m%-18d[37m押注金额: [36m%-20d[m  ", inmoney(0), money);
}

int get_money(int m, char *welcome)
{
   int  money;
   char	buf[5];
   do {
      show_money(m,welcome,YEA);
      getdata(23,16,"☆要押注多少钱(1 - 2000)? ", buf, 5, DOECHO, YEA);
      if(buf[0] == '\0') return 0;
      money = abs(atoi(buf));
      if ( money <= 0) return 0;
      if(check_money(money))return 0;
   } while ((money < 1) || (money > 2000));
   demoney(money);
   show_money(money,NULL,YEA);
   return money;
}
