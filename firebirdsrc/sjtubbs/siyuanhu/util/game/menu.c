/*******************************************************************/
/*  deardragon.bbs@fb2000.dhs.org              zjinglong@163.net   */
/*  游戏、管理菜单				2000.12.18         */
/*******************************************************************/

#include "bbs.h"
#include "money.h"

#define PERM_MANAGE     PERM_SYSOP
 
/*******************************************************************/
/* 游戏菜单 games     游戏选单                                     */
/*******************************************************************/
int game_manage(void)
{
#define GAME_NUM 10
   int i, num;
   char buf[STRLEN],ans[4];
   EXCHANGES iterms[GAME_NUM] = {
      {"扫雷游戏,1},
      {"猜数字",1},
      {"黑甲克",1},
      {"西八拉",1},
      {"金扑克梭哈",1},
      {"天地九九",1},
      {"宾果宾果",1},
      {"电子小鸡",1},
      {"超级小玛丽",1},
      {"星空战斗鸡",1}
   }
   if(inGame()) return 0;
   modify_user_mode(ALL_GAMES);
   while(1){
      clear();
      move(2,0);
      prints("欢迎进入 [^[[32m游戏资料管理中心^[[37m]....\n\n");
      for(i=0;i<THENUM;i++) prints("(%2d) %s\n",i,iterms[i]);
      getdata(THENUM+5,0,"您的选择: ",ans,4,DOECHO,YEA);
      i = atoi(ans);
      if(i <= 0 || i >= THENUM) break;
      switch ( i ) {
         case 1:
            prints("\n");
            usercomplete("请输入对方的 ID (按空格键可以自动匹配): ",uident);
            if(uident[0]=='\0'|| getuser(uident)<=0){
               prints("\n无输入，取消操作。\n");
            } else {
               getdata(THENUM+8,0,"请输入欲增加的金钱数目(负数表示减少): ",
                  buf,8,DOECHO,YEA);
               num = atoi(buf);
               if(num!=0) {
                  incmoney(uident,num);
                  prints("\n操作已接受。\n");
               } else prints("\n无输入，取消操作。\n");
            }
            pressanykey();
            break;
         case 2:
            break;
      }
   }
   outgame();
   return 1;
}
/*******************************************************************/
/* 金钱银行菜单  money                                             */
/*******************************************************************/
   char    uident[STRLEN];
/*******************************************************************/
/* 数据管理菜单  manage                                            */
/*******************************************************************/
   if(!HAS_PERM(PERM_MANAGE)) return 0;
/*******************************************************************/
/* 穿梭银河菜单  bbsnet                                            */
/*******************************************************************/
