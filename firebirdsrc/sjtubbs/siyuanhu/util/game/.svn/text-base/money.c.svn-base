/* ¿ìÒâ¹àË®Õ¾ ½»Ò×ÊĞ³¡´úÂë 1999.12.19 */
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
	char msg[3];
	char money[7];
	int opt;
	int num_money;
	int id;

	id = getuser(currentuser.userid);

	if(gotomarket("Á÷Ã¥´óÒøĞĞ")) 
		return 0;
	prints("»¶Ó­¹âÁÙ Á÷Ã¥´óÒøĞĞ\n\n");

	prints("ÄúÄ¿Ç°Ğ¯´øÏÖ½ğ %d Ôª£¬ÒøĞĞÕÊ»§½áÓà %d Ôª¡£\n\n", currentuser.money, currentuser.inbank);

	getdata( 8, 0,"ÇëÊäÈëÄúÒª½øĞĞµÄ²Ù×÷ [1] È¡¿î [2]´æ¿î :", msg, 2, DOECHO, /*NULL,*/ YEA);
	opt = atoi(msg);
	if (!isdigit(msg[0]) || opt <= 0 || opt > 2 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(10,0);
		prints("¶Ô²»Æğ£¬Á÷Ã¥´óÒøĞĞ²»Ìá¹©´ËÏî·şÎñ£¡\n");
		pressanykey();
		outgame();
		return 0;
	}

	if(opt == 1)
	{
		getdata(10, 0, "ÇëÊäÈëÄãÒªÈ¡¿îµÄ½ğ¶î [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("¶Ô²»Æğ£¬È¡¿î½ğ¶î±ØĞëÔÚ 1 - 50000 Ö®¼ä£¡\n");
			pressanykey();
			outgame();
			return 0;
		}

		if(currentuser.inbank < num_money)
		{
			move(12, 0);
			prints("¶Ô²»Æğ£¬ÄúµÄÁ÷Ã¥´óÒøĞĞÕÊ»§²»ÄÜÍ¸Ö§£¡\n");
			pressanykey();
			outgame();
			return 0;
		}

		currentuser.inbank -= num_money;
		currentuser.money += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("ÄúµÄÈ¡¿îÊÖĞøÒÑ¾­Íê³É\n");
		pressanykey();
		outgame();
		return 1;

	}
		
	if(opt == 2)
	{
		getdata(10, 0, "ÇëÊäÈëÄã´æ¿îµÄ½ğ¶î [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
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

		currentuser.inbank += num_money;
		currentuser.money -= num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("ÄúµÄ´æ¿îÊÖĞøÒÑ¾­Íê³É\n");
		pressanykey();
		outgame();
		return 1;

	}

//	prints();
	move(12,0);
	prints("½ñÈÕÁ÷Ã¥´óÒøĞĞĞİÏ¢:)\n");
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
   
   if(gotomarket("½»Ò×ÊĞ³¡")) return 0;
   maxnum = currentuser.money - 1000;
   prints("[m»¶Ó­Ê¹ÓÃ[1;33m"BBSNAME"[m½»Ò×ÊĞ³¡[32mÓÑÇé×ªÕÊ[m¹¦ÄÜ");
   prints("\n\nÄúÄ¿Ç°µÄÇé¿öÊÇ£º\n×¢²áÌìÊı([32m%d[m Ìì) "
          "ÉÏÕ¾×ÜÊ±Êı([32m%d[m Ğ¡Ê±) "
	  "[44;37m¿É×ªÕÊ×Ê½ğ([32m%d[37m Ôª)[m",
	  (now - currentuser.firstlogin)/86400,
	  currentuser.stay/3600,currentuser.money-1000);
   if(currentuser.stay<=36000||now-currentuser.firstlogin<=2592000||maxnum<=0){
      prints("\n\nÄ¿Ç°ÊĞ³¡¹æ¶¨£º ²ÎÓë[32mÓÑÇé×ªÕÊ[mµÄ ID ±ØĞë¾ß±¸ÒÔÏÂ"
             "È«²¿Ìõ¼ş£º\n"
	     "    1. ±¾ÕÊºÅ×¢²áÌìÊı³¬¹ı 30 Ìì;\n"
	     "    2. ×ÜÉÏÕ¾Ê±Êı³¬¹ı 10 Ğ¡Ê±;\n"
	     "    3. ×îÖÕÓµÓĞ´æ¿î³¬¹ı 1000 Ôª.\n");
      prints("\n\n¸ù¾İÊĞ³¡¹æ¶¨£¬ÄúÄ¿Ç°ÉĞÃ»ÓĞ[32mÓÑÇé×ªÕÊ[mµÄ×Ê¸ñ¡£\n");
      pressanykey();
      outgame();
      return 0;
   }
   if (!gettheuserid(9,"ÄúÏë×ªÕÊµ½Ë­µÄÕÊ»§ÉÏ£¿ÇëÊäÈëËûµÄÕÊºÅ: ",&id))return 0;
   u_money = lookupuser.money+lookupuser.medals*10000+ lookupuser.inbank;
   if(!strcmp(currentuser.userid,lookupuser.userid) ||u_money > 20000){
         prints("\n¶Ô²»Æğ£¬%s Ä¿Ç°ÉĞ²»ĞèÒªÄúµÄ×ªÕÊ£¡",lookupuser.userid);
	 pressanykey();
         outgame();
	 return 0;
   }
   move(10,0);
   canlending = (maxnum>(20000-u_money))?(20000-u_money):maxnum;
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
   if(currentuser.money - 1000  != maxnum) {
      prints("\n¶Ô²»Æğ£¬ÄúµÄ¿É×ªÕÊ×Ê½ğÓĞËù±ä»¯£¬È¡Ïû´Ë´Î½»Ò×£¡");
      prints("\nÇëÖØĞÂÖ´ĞĞ±¾½»Ò×¡£");
      pressanykey();
      outgame();
      return 0;
   }
   currentuser.money -= num;
   substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);
   lookupuser.money += num;
   substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
   prints("\nĞ»Ğ»Äú£¬ÄúÒÑ¾­¸ø [1;32m%s[m ×ªÕÊ [1;33m%d[m Ôª¡£",
      lookupuser.userid,num);
   prints("\nÎª±íÊ¾¶ÔÄãµÄ×ªÕÊĞĞÎªµÄ¸ĞĞ»£¬±¾Õ¾ÒÑ¾­ÓÃĞÅ¼şÍ¨ÖªÁËËû¡£");
   sprintf(genbuf,"¸ø %s ×ªÕÊ %d Ôª",lookupuser.userid,num);
   gamelog(genbuf);
   sprintf(ans,"%s ¸øÄú¼ÄÀ´ÁË %d ÔªÓÑÇé×ªÕÊ",currentuser.userid,num);
   sprintf(genbuf,"\n %s :\n\tÄúºÃ£¡\n\tÄúµÄÅóÓÑ %s ¸øÄú¼ÄÀ´ÁË %d ÔªÓÑÇé×ªÕÊ×Ê½ğ¡£\n\tÇëÄúÔÚ Market °å¶ÔËûµÄÎŞË½ĞĞÎª·¢ÎÄ±íÊ¾¸ĞĞ»£¬\n\tÕâÑù£¬Äú¾Í¿ÉÒÔ»ñµÃÕâ±ÊÓÑÇé×ªÕÊ×Ê½ğ¡£\n\n\tµ±È»£¬ÄúÒ²¿ÉÒÔÍË³öÒ»´Îºó£¬ÔÙ½øÈë±¾Õ¾£¬\n\tÒ»Ñù¿ÉÒÔ»ñµÃÕâ±ÊÓÑÇé×ªÕÊ×Ê½ğ¡£\n  ",lookupuser.userid,currentuser.userid,num);
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
      {"ÉÏÕ¾´ÎÊı",2},
      {"ÎÄÕÂÊı",5},
      {"½±ÕÂÊı",10000},
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
   for(no = 0; no < 10; no ++) {
      move(5+no, 2);
      prints("(%2d) %s",no+1,exchanges[no].title);
      move(5+no, 20);
      prints("==> %6d Ôª", exchanges[no].value);
   }
   move(16,0);
   prints("[0;1mÄúÄ¿Ç°µÄÇé¿öÊÇ: (1)ÉÏÕ¾´ÎÊı([32m%d[m)  "
          "(2)ÎÄÕÂÊı([32m%d[37m)  (3)½±ÕÂÊı.([32m%d[37m)\n",
	  currentuser.numlogins,currentuser.numposts,currentuser.medals);
   getdata(18, 0, "ÄúÏëµäµ±ÄÄÒ»Ïî£¿", ans, 10, DOECHO, YEA);
   no = atoi(ans);
   if ( no < 1 || no > 10 ) {
      prints("\nÊäÈë´íÎó£¬È¡Ïû½»Ò× ...");
      pressanykey();
      outgame();
      return 0;
   }
   move(18, 30);
   prints("ÄãÑ¡Ôñµäµ±¡°[32m%s[m¡±¡£",exchanges[no-1].title);
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
         prints("¶Ô²»Æğ, ÄãµÄÊı¾İ·¢ÉúÁË±ä»¯, ÄãÄ¿Ç°Ã»ÓĞÕâÖÖÈ¨ÏŞ. ");
	 pressanykey();
         outgame();
	 return 0;
     }
  } else {
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
      prints("Äú¹²¼Æµäµ±%s[32m%d[m ·İ£¬ %s [33m%d[m Ôª¡£\n",
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
   }
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
   sprintf(genbuf,"µäµ±%s %d ·İ£¬»¨·Ñ %d Ôª.",exchanges[no-1].title,num,maxnum);
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
   gamelog(genbuf);
   pressanykey();
   outgame();
   return 1;
}	


/**************************************************************************/
/*   ÓÎÏ·Êı¾İ½»Ò×ÊĞ³¡						          */
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
      prints("±§Ç¸£¡ÄúÖ»ÓĞ %d Ôª¶Ä½ğ", money,now_money);
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
   prints("[0;1;37;44m                  ÄãÏÖÓĞÏÖ½ğ: [36m%-18d[37mÑº×¢½ğ¶î: [36m%-20d[m  ", inmoney(0), money);
}

int get_money(int m, char *welcome)
{
   int  money;
   char	buf[5];
   do {
      show_money(m,welcome,YEA);
      getdata(23,16,"¡îÒªÑº×¢¶àÉÙÇ®(1 - 2000)? ", buf, 5, DOECHO, YEA);
      if(buf[0] == '\0') return 0;
      money = abs(atoi(buf));
      if ( money <= 0) return 0;
      if(check_money(money))return 0;
   } while ((money < 1) || (money > 2000));
   demoney(money);
   show_money(money,NULL,YEA);
   return money;
}
