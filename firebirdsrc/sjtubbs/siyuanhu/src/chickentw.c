/* ëŠ×Óëu Ğ¡´aÁ¦..×a×bß[‘ò.? */
/* Writed by Birdman From 140.116.102.125 „“ÒâÍÛ¹ş¹ş*/
  


#include "bbs.h"
#define DATA_FILE  "chicken"


char
*cstate[10]={"ÎÒÔÚ³Ôïˆ","Íµ³ÔÁãÊ³","À­±ã±ã","±¿µ°..İ”½oëu?","¹ş..ÚAĞ¡ëuÒ²›]¶à¹â˜s"
            ,"›]Ê³ÎïÀ²..","Æ£„ÚÈ«Ïû!"};
char *cage[9]={"ÕQÉú","ßLšq","Ó×Äê","ÉÙÄê","ÇàÄê","»îÁ¦","‰ÑÄê","ÖĞÄê"};
char *menu[8]={"ß[‘ò","ß\„Ó","Õ{½ÌÓ‹ÄÜ","ÙIÙu¹¤¾ß","ÇåÀíëuÉá"};

time_t birth;
int weight,satis,mon,day,age,angery,sick,oo,happy,clean,tiredstrong,play;
int winn,losee,last,chictime,agetmp,food,zfood;
char Name[20];
FILE *cfp;

int
chicken_main()
{
  FILE *fp;
  time_t now = time(0);
  struct tm *ptime;
  char fname[50];
  agetmp=1;

  setutmpmode(CHIC);
  time(&now);
  ptime = localtime(&now);
  setuserfile(fname, DATA_FILE);
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       creat_a_egg();
       last=1;
       fp = fopen(fname, "r");
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);

    }
   else
   {
       last=0;
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
   }
/*æm*/
  if(day<(ptime->tm_mon+1))
   { age=ptime->tm_mday;
    age=age+31-mon;}
  else
    age=ptime->tm_mday-mon;

  show_chicken();
  select_menu();
    fp = fopen(fname, "r+");
  /*if (last!=1)
    { */
        fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
          ,weight,mon,day,satis,age,oo,happy,clean,tiredstrong,play
          ,winn,losee,food,zfood,Name);

  fclose(fp);
  return 0;
}

int creat_a_egg()
{
  char name[20];
  char fname[50];
  struct tm *ptime;
  FILE *fp;
  time_t now;

  time(&now);
  ptime = localtime(&now);
  move(3,0);
  clrtobot();
  while(strlen(Name)<1)
   getdata(4, 0, "ÍĞ¡ëuÈ¡‚€ºÃÃû×Ö£º", Name, 21, DOECHO,NULL);
  setuserfile(fname, DATA_FILE);
  fp = fopen(fname, "w");
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
      ,ptime->tm_hour*2,ptime->tm_mday,ptime->tm_mon+1
        ,0,0,0,0,0,0,0,0,0,10,5,Name);
  fclose(fp);

  if((fp != fopen("etc/chicken", "a"))==NULL);
   /*fp=fopen("etc/chicken,"ab");*/
  fprintf(fp,"[32m%s[m ÔÚ [34;43m[%d/%d  %d:%02d][m  ğBÁËÒ»ëb½Ğ [33m%s[m µÄĞ¡ëu\n",
          cuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
          ptime->tm_hour, ptime->tm_min,Name);
  fclose(fp);
  return;
}

int show_chicken()
{
  int diff;
  /*int chictime;*/

  diff = (time(0) - login_start_time) / 120;

  if(chictime>=200)
      {
        weight-=5;
        clean+=3;
        if(tiredstrong>2)
        tiredstrong-=2;
       }
    /*food=food-diff*3;*/
  if(weight<0)
    death();
/*  if((diff-age)>1 && agetmp) ß€ÊÇÓĞ†–î}
   {age++;
    agetmp=0;}
*/
  move(1,0);
  clrtobot();
  prints(
     "[33mName:%s[m"
     "  [45mAge :%dšq[m"
     "  ÖØÁ¿:%d"
     "  Ê³Îï:%d"
     "  ÁãÊ³:%d"
     "  Æ£„Ú:%d"
     "  ±ã±ã:%d\n"
     "ÉúÈÕ:%dÔÂ%dÈÕ"
     "   ÌÇÌÇ:$%ld"
     "   ¿ì˜·¶È:%d"
     "   MÒâ¶È:%d"
     "  ´óÑaÍè:%d\n",
     Name,age,weight,food,zfood,tiredstrong,clean,day
     ,mon,cuser.money,happy,satis,oo);

  move(3,0);
 if(age<=16){
  switch(age)
  {
     case 0:
     case 1:
      outs("  ¡ñ¡ñ¡ñ¡ñ
¡ñ  ¡ñ¡ñ  ¡ñ
¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
¡ñ¡ñ    ¡ñ¡ñ
¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
  ¡ñ¡ñ¡ñ¡ñ   ");

      break;
     case 2:
     case 3:
      outs("    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
  ¡ñ            ¡ñ
¡ñ    ¡ñ    ¡ñ    ¡ñ
¡ñ                ¡ñ
¡ñ      ¡ñ¡ñ      ¡ñ
¡ñ                ¡ñ
¡ñ                ¡ñ
  ¡ñ            ¡ñ
    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ   ");

       break;
     case 4:
     case 5:

        outs("      ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
    ¡ñ            ¡ñ
  ¡ñ  ¡ñ        ¡ñ  ¡ñ
  ¡ñ                ¡ñ
  ¡ñ      ¡ñ¡ñ      ¡ñ
¡ñ¡ñ¡ñ    ¡ñ¡ñ      ¡ñ¡ñ
  ¡ñ                ¡ñ
  ¡ñ                ¡ñ
    ¡ñ  ¡ñ¡ñ¡ñ¡ñ  ¡ñ
      ¡ñ      ¡ñ  ¡ñ
                ¡ñ    ");
        break;
        case 6:
        case 7:
         outs("   ¡ñ    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
¡ñ  ¡ñ¡ñ  ¡ñ        ¡ñ
¡ñ              ¡ñ    ¡ñ
  ¡ñ¡ñ¡ñ                ¡ñ
¡ñ                      ¡ñ
¡ñ  ¡ñ¡ñ                ¡ñ
  ¡ñ  ¡ñ                ¡ñ
      ¡ñ                ¡ñ
        ¡ñ            ¡ñ
          ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ        ");
        break;

        case 8:
        case 9:
        case 10:
         outs("    ¡ñ¡ñ          ¡ñ¡ñ
  ¡ñ¡ñ¡ñ¡ñ      ¡ñ¡ñ¡ñ¡ñ
  ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
  ¡ñ                  ¡ñ
  ¡ñ    ¡ñ      ¡ñ    ¡ñ
¡ñ                      ¡ñ
¡ñ        ¡ñ¡ñ¡ñ        ¡ñ
  ¡ñ                  ¡ñ
¡ñ    ¡ñ          ¡ñ  ¡ñ
  ¡ñ¡ñ            ¡ñ¡ñ¡ñ
  ¡ñ                  ¡ñ
    ¡ñ              ¡ñ
      ¡ñ  ¡ñ¡ñ¡ñ  ¡ñ
      ¡ñ  ¡ñ    ¡ñ
        ¡ñ               ");

        break;

        case 11:
        case 12:
        outs("        ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
      ¡ñ    ¡ñ    ¡ñ¡ñ
    ¡ñ  ¡ñ      ¡ñ  ¡ñ¡ñ
  ¡ñ¡ñ              ¡ñ¡ñ¡ñ
¡ñ              ¡ñ    ¡ñ¡ñ
¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ      ¡ñ¡ñ
  ¡ñ                  ¡ñ¡ñ
    ¡ñ        ¡ñ  ¡ñ    ¡ñ
    ¡ñ        ¡ñ  ¡ñ    ¡ñ
    ¡ñ          ¡ñ      ¡ñ
      ¡ñ              ¡ñ
        ¡ñ  ¡ñ¡ñ¡ñ  ¡ñ
        ¡ñ  ¡ñ  ¡ñ  ¡ñ
          ¡ñ      ¡ñ             ");

        break;
        case 13:
        case 14:
        outs("              ¡ñ¡ñ¡ñ¡ñ
      ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
  ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
  ¡ñ    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
¡ñ¡ñ    ¡ñ            ¡ñ¡ñ
¡ñ¡ñ¡ñ¡ñ                ¡ñ
  ¡ñ                    ¡ñ
    ¡ñ¡ñ            ¡ñ¡ñ
  ¡ñ    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ  ¡ñ
  ¡ñ                  ¡ñ
    ¡ñ                  ¡ñ
      ¡ñ                ¡ñ
    ¡ñ¡ñ¡ñ            ¡ñ¡ñ¡ñ        ");
        break;
        case 15:
        case 16:
        outs("  ¡ñ    ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
¡ñ  ¡ñ¡ñ  ¡ñ        ¡ñ
¡ñ              ¡ñ    ¡ñ
  ¡ñ¡ñ¡ñ                ¡ñ
¡ñ                      ¡ñ
¡ñ  ¡ñ¡ñ                ¡ñ
  ¡ñ  ¡ñ                ¡ñ
      ¡ñ        ¡ñ  ¡ñ    ¡ñ
      ¡ñ          ¡ñ      ¡ñ
      ¡ñ                  ¡ñ
        ¡ñ              ¡ñ
        ¡ñ  ¡ñ¡ñ  ¡ñ¡ñ¡ñ
        ¡ñ  ¡ñ¡ñ  ¡ñ
          ¡ñ    ¡ñ             ");

       break;
        }
    }
     else{
        outs("          ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ
        ¡ñ          ¡ñ¡ñ¡ñ
      ¡ñ    ¡ñ    ¡ñ  ¡ñ¡ñ¡ñ
  ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ        ¡ñ¡ñ
  ¡ñ          ¡ñ          ¡ñ
  ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ          ¡ñ            [1;5;31mÎÒÊÇ´ó¹ÖøB[m
  ¡ñ        ¡ñ            ¡ñ
  ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ            ¡ñ
  ¡ñ                    ¡ñ
  ¡ñ                    ¡ñ
    ¡ñ                ¡ñ
¡ñ¡ñ  ¡ñ            ¡ñ
¡ñ      ¡ñ¡ñ¡ñ¡ñ¡ñ¡ñ  ¡ñ¡ñ
  ¡ñ                      ¡ñ
¡ñ¡ñ¡ñ    ÎÒÊÇ´ó¹ÖøB       ¡ñ¡ñ¡ñ ");

   }
  if(clean>10)
    {
        move(10,30);
        outs("±ã±ãºÃ¶à..³ô³ô...");
/*        if(clean>15)
          death();*/
        pressanykey();
     }

   move(17,0);
   outs("[32m[1]-³Ôïˆ    [2]-³ÔÁãÊ³   [3]-ÇåÀíëuÉá   [4]-¸úĞ¡ëu²ÂÈ­  [5]-Ä¿Ç°‘ğ¿ƒ[m");
   outs("\n[32m[6]-ÙIëuï•ÁÏ$20 [7]-ÙIÁãÊ³$30  [8]-³Ô´óÑaÍè  [9]-Ùuëuà¸[m");
  /*pressanykey();*/
  return;
}

int select_menu()
{
  extern struct commands chiclist[];
  char inbuf[80];
  int diff;
  struct tm *ptime;
  time_t now;

  time(&now);
  ptime = localtime(&now);
  diff = (time(0) - login_start_time) / 60;

  while(1){
   getdata(b_lines-1, 0, "Òª×öĞ©Ê²üNÄØ?£º[0]", inbuf, 4, DOECHO,NULL);
        if(tiredstrong>20)
          {
           clear();
           move(15,30);
           outs("†è~~~Ğ¡ëu•şÀÛ‰ÄµÄ...ÒªÏÈÈ¥ĞİÏ¢Ò»ÏÂ..");
           outs("\n\nĞİ    ğB     ÖĞ");
           switch(inbuf[0])
            {
              case'8':
               move(b_lines-2,0);
       if(oo<=0)
          {
        outs("›]´óÑaÍèÀ²!!");
        pressanykey();
        break;}
        move(10,0);
        outs("
               ¨¨€¨€¨
               ¨€¨€¨€¨€
               ¨¨€¨€¨
                          Íµ³Ô½ûË´óÑaÍè.....");
        tiredstrong = 0;
        happy+=3;
        oo--;
        pressany(6);
        return;
        break;
        }
           pressanykey();

           return;
           }
   switch(inbuf[0])
   { case '1':
        if (food<=0){
         pressany(5);
         break;}
        move(10,0);
        outs("       ¨x¨x¨x¨x¨U¨x
         ¡ß¡à ¨€  ¨„
              ¨‚  ¨„                             ¨„¨„¨„¨„  ¨„
              ¨„  ¨„     ¨              ¨      ¨„¨„¨„¨„¨„¨„¨„
         £Ã£ï£ë£å ¨„    _¨y¨y¨y¨y¨y¨y¨y¨y¨y_    ¨T¨T¨T¨T¨T¨T¨T¨T
             ¨„   ¨„     £¥£¥£¥£¥£¥£¥£¥£¥£¥       ¨q©¤¡É¡É©¤¨r
            ¨‚    ¨„     ¨}¨}¨}¨}¨}¨}¨}¨}¨}       ©¦Mcdonald©¦      ¡¡¡¡¡¡¡¡
           ¨€     ¨„     ¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡¡     ¨t©¤©¤©¤©¤¨s
       ¨¨}¨}¨}¨}¨      ¨¨€¨€¨€¨€¨€¨€¨€¨     ¨¨€¨€¨€¨€¨€¨€¨ ");

        pressany(0);
/*        refresh();
        sleep(1);*/
        food--;
        tiredstrong++;
        satis++;
        if(age<5)
          weight=weight+(5-age);
        else
          weight++;
        if(weight>100)
          {move(9,30);
           outs("Ì«ÖØÁËÀ²..·Êëu~~");
           pressanykey();
           }
        if(weight>150)
          {move(9,30);
           outs("ëu“Î•ÁË~~");
           pressanykey();
           }
        if(weight>200)
           death();
                /* if(weight>100)
                    {  move(15,40);
                       outs("ÄãÏë“ÎËÀëu°¡£¿....ÍÛßÖ¡ğ¡ñ¡Á¡Á");
                       pressanykey();
                       food=100;
                      }*/
        break;
     case '2':
        if (zfood<=0){
         pressany(5);
         break;}
        move(10,0);
        outs("             ¨u
       [33;1m¨[m¨q¡ğ
       [37;42m¡ö¡ö[m
       [32m¨€¨€[m
       [32;40;1m¨€¨€[m
       [31m ¨€ [m
      [31m ¨‰¨‰[m   [32;1mË®¹û¾Æ±ùä¿ÁÜÌK´ò[m   àÅ!ºÃºÈ!   ");
        pressany(1);
        zfood--;
        tiredstrong++;
        happy++;
        weight+=2;
        if(weight>100)
          {move(9,30);
           outs("Ì«ÖØÁËÀ²..·Êëu~~");
           pressanykey();
           }
        if(weight>200)

          death();
        break;
     case '3':
        move(10,0);
        outs("[1;36m                              ¨q¨T¨T¨T¨r[m
[1;33m                             ¡º[37m¨€¨€¨€¨€[m
[1;37m                               ¨€¨€¨€¨€[m
[1;37m             ¨x¨x¨x¨x¨x¨x¨x¨x[32m¡ò[37m¨€¨€¨€¨€[m
[37m             ¨}¨}¨}¨}¨}¨}¨}¨}¨[1;37m¨€¨€¨€¨€[m
[37m             ¨¨ˆ¨ˆ¨ˆ¨ˆ¨ˆ¨ˆ¨ˆ¨ˆ[1;33m ¨U[m
[36m                  ¨¨€¨€¨€¨€¨€[1;33m¨T¨s[m
[1;36m                  ¨¨ˆ¨ˆ¨ˆ¨ˆ¨ˆ[m
  [1;37m                ¨‰¨‰¨‰¨‰¨‰¨‰[m
                  Ò®Ò®Ò®...±ã±ãÀ­¹â¹â...                              ");

        pressany(2);
        tiredstrong+=5;
        clean=0;
        break;
     case '4':
        guess();
        satis+= (ptime->tm_sec%2);
        tiredstrong++;
        break;
     case '5':
        situ();
        break;
     case '6':
          move(b_lines-3,0);
        if(cuser.money<20)
          {    outs("ÌÇ¹û²»×ã!!");
        pressanykey();
        break;}
        food+=5;
        prints("\nÊ³ÎïÓĞ [33;41m %d [m·İ",food);
        prints("   Ê£ÏÂ [33;41m %d [mÌÇ",demoney(20));
        pressanykey();
        break;
     case '7':
          move(b_lines-3,0);
        if(cuser.money<30)
          {    outs("ÌÇ¹û²»×ã!!");
        pressanykey();
        break;}
        zfood+=5;
        prints("\nÁãÊ³ÓĞ [33;41m %d [m·İ",zfood);
        prints("  Ê£ÏÂ [33;41m %d [mÌÇ",demoney(30));
        pressanykey();
        break;
     case '8':
       move(b_lines-2,0);
       if(oo<=0)
          {
        outs("›]´óÑaÍèÀ²!!");
        pressanykey();
        break;}
        move(10,0);
        outs("
               ¨¨€¨€¨
               ¨€¨€¨€¨€
               ¨¨€¨€¨
                          Íµ³Ô½ûË´óÑaÍè.....");
        tiredstrong = 0;
        happy+=3;
        oo--;
        pressany(6);
        break;
     case '9':
               move(b_lines-2,0);
        if(age<5)
        {prints("Ì«Ğ¡ÁË...²»µÃØœÊÛÎ´³ÉÄêĞ¡ëu.....");
          pressanykey();
          break;
        }
        sell();
        break;
     case 'k':
        death();
        break;
     case 't':
        tiredstrong = 0;
        break;
     case 'c':
        getdata(b_lines-1, 0, "ÍĞ¡ëuÈ¡‚€ºÃÃû×Ö£º", Name, 21, DOECHO,NULL);
        break;
     default:
        return;
     break;
    }
    show_chicken();
   }
  return;
}

int death()
{
  char fname[50];
  FILE *fp;
  struct tm *ptime;

      time_t now;

        time(&now);
          ptime = localtime(&now);
  clear();
  move(5,0);
  clrtobot();
  if((fp = fopen("etc/chicken", "a"))!=NULL);
     /*fp=fopen("etc/chicken,"ab");*/
fprintf(fp,"[32m%s[m ÔÚ [34;43m[%d/%d  %d:%02d][m  µÄĞ¡ëu [33m%s  [36m’ìÁË~~[m \n",
                 cuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
                           ptime->tm_hour, ptime->tm_min,Name);
                             fclose(fp);
    outs("†è...Ğ¡ëu’ìÁË....");
  outs("\n±¿Ê·ÁË...Ús³öÏµ½y...");
  pressanykey();
  setuserfile(fname, DATA_FILE);

  unlink(fname);
/*  strcpy(Name,"");
  creat_a_egg();
  chicken_main();*/
  abort_bbs();

}

int comeclear()
{
   extern struct commands cmdlist[];

  domenu(MMENU, "Ö÷¹¦\ÄÜ±í", (chkmail(0) ? 'M' : 'C'), cmdlist);
}


int
pressany(i)
{
  int ch;
  move(b_lines,0);
  prints("[33;46;1m                           [34m%s[37m                         [0m",cstate[i]);
  do
  {
    ch = igetkey();
    if (ch == KEY_ESC && KEY_ESC_arg == 'c')
       capture_screen();
  } while ((ch != ' ') && (ch != KEY_LEFT) && (ch != '\r') && (ch != '\n'));
  move(b_lines, 0);
  clrtoeol();
  refresh();

}

int guess()
{
   int ch,com;
   struct tm *qtime;
   time_t now;

   time(&now);
   qtime = localtime(&now);

   do
   {
    /*getdata(b_lines-1, 0, "[1]-¼ôµ¶ [2]-Ê¯î^ [3]-²¼£º", inbuf, 4,
DOECHO,NULL);*/
    move(b_lines,0);
    outs("[1]-¼ôµ¶ [2]-Ê¯î^ [3]-²¼£º");
    ch = igetkey();
    }while((ch!='1')&&(ch!='2')&&(ch!='3'));

   /* com=qtime->tm_sec%3;*/
    com=rand()%3;
    move(b_lines-4,35);
    switch(com){
        case 0:
          outs("Ğ¡ëu:¼ôµ¶");
         break;
        case 1:
          outs("Ğ¡ëu:Ê¯î^");
         break;
        case 2:
          outs("Ğ¡ëu:²¼");
         break;
     }

    move(b_lines-4,0);


    switch(ch){
    case '1':
      outs("±¿ëu---¿´ÎÒ“ìíµÄµ¶---");
      if (com==0)
        tie();
     else  if (com==1)
        lose();
     else if (com==2)
        win();
    break;
    case '2':
      outs("´ôëu---ÔÒÄãÒ»‰KÊ¯î^!!---");
      if (com==0)
        win();
     else if (com==1)
        tie();
     else if (com==2)
        lose();
    break;
    case '3':
      outs("´Àëu---ËÍÄãÒ»¶ÑÆÆ²¼!---");
      if (com==0)
        lose();
     else if (com==1)
        win();
     else if (com==2)
        tie();
    break;
  }
  /* sleep(1);*/
   pressanykey();

}

int win()
{
        winn++;
       /* sleep(1);*/
        move (b_lines-3,0);
        outs("ÅĞ¶¨:Ğ¡ëuİ”ÁË....    >_<~~~~~

                                 ");
        return;
}
int tie()
{
       /* sleep(0);*/
         move (b_lines-2,0);
        outs("ÅĞ¶¨:Æ½ÊÖ                    -_-

                                              ");
        return;
}
int lose()
{
        losee++;
        happy+=2;
        /*sleep(0);*/
         move (b_lines-2,0);
        outs("Ğ¡ëuÚA‡Ó                      ¡É¡É
                               ¨Œ       ");
        return;
}

int situ()
{

        move(16,0);
        outs("           ");
        move(17,0);
        prints("Äã:[44m %d„Ù %dØ“[m                   ",winn,losee);
        move(18,0);
        prints("ëu:[44m %d„Ù %dØ“[m                   ",losee,winn);

       if (winn>=losee)
        pressany(4);
       else
        pressany(3);

       return;
}
void
p_bf()
{
  FILE *fp;
  char fname[50];

  move(b_lines-2,0);
  if(cuser.money<30){
    outs("ÌÇ¹û²»×ã!!");
        pressanykey();
        return;}
     setuserfile(fname, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
      outs("›]ğBëu..²»½oÄãÙI..¹ş¹ş...");
       pressanykey();
        return;
    }
   else{
        fp = fopen(fname, "r");
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %s %d %d"
,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play,Name
         ,&winn,&losee);
        fclose(fp);
      oo++;
      prints("\n´óÑaÍèÓĞ %d îw",oo);
      prints("  Ê£ÏÂ %d ÌÇ",demoney(30));
      pressanykey();
    fp = fopen(fname, "r+");
  /*if (last!=1)
    { */
        fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %s %d %d"
          ,weight,mon,day,satis,age,oo,happy,clean,tiredstrong,play,Name
          ,winn,losee);
  fclose(fp);
    }
        return;
}


int year(useri)
{
  FILE *fp;
  char fname[50];
        getuser(useri);
      sethomefile(fname, useri, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       return NULL;
    }
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
  return age;

    }
int sell()
{
  int sel=0;
  char ans[5];
  struct tm *ptime;
  FILE *fp;
    time_t now;

      time(&now);
        ptime = localtime(&now);

  getdata(b_lines-2, 0, "´_¶¨ÒªÙuµôĞ¡ëu?[y/N]",ans,3,LCECHO,NULL);
  if(ans[0]!='y') return;
  sel+=(happy*10);
  sel+=(satis*7);
  sel+= ((ptime->tm_sec%9)*10);
  sel+= weight;
  sel+=age*10;
  move(b_lines-3,0);
  prints("Ğ¡ëuÖµ[33;45m$$ %d [mÌÇÌÇ",sel);
    getdata(b_lines-4, 0, "ÕæµÄÒªÙuµôĞ¡ëu?[y/N]",ans,3,LCECHO,NULL);
  if(ans[0]!='y') return;

  if((fp = fopen("etc/chicken", "a"))!=NULL);
fprintf(fp,"[32m%s[m ÔÚ [34;43m[%d/%d  %d:%02d][m  °ÑĞ¡ëu [33m%s  [31mÒÔ [37;44m%d[m [31mÌÇ¹ûÙuÁË[m\n",
                 cuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
                           ptime->tm_hour, ptime->tm_min,Name,sel);
                             fclose(fp);
  clear();

  inmoney(sel);
  strcpy(Name,"");
  creat_a_egg();
  chicken_main();

}
int gagb()
{
  char buf[5],buf1[6];
  char ans[5];
  int i,k,flag[11],count=0,GET=0,q,ok=0;
  int l=1,money=0;

  setutmpmode(NANB);
    clear();
  do{
   /* while(strlen(buf1)<1)*/
  getdata(0, 0, "ÒªÑº¶àÉÙÌÇ¹û°¡(×î´ó2000)£º", buf1, 5, DOECHO,NULL);
  money=atoi(buf1);
     if(cuser.money<money){
      outs("²»‰ò$$");
      pressanykey();
      return 0;}
     }while((money<=0)||(money>2000));
  demoney(money);
  for(i=0;i<11;i++)
   flag[i]=0;
  for(i=0;i<4;i++){
   do{
     k=rand()%9;
     } while (flag[k]!=0);
   flag[k]=1;
   ans[i]=k+'0';
   }
  while(!GET)
  {
   ga(buf,l);
   if (buf[0]=='q'&&buf[1]=='k'){
     prints("Í¶½µ..²ÂÁË %d´Î",count);
     prints("\n´ğ°¸ÊÇ:%s",ans);
     pressanykey();
    /*return 0*/;}
   if(buf[0]=='q'){
     prints("\n´ğ°¸ÊÇ:%s",ans);
     pressanykey();
    return 0;}
   if(compare(ans,buf,count))
   /* GET=1;*/break;
   if(count>8){
     outs("[1;32mÍÛßÖ..²ÂÊ®´Îß€²»Œ¦...ÌÇÌÇ›]ÊÕ..[m");
     pressanykey();
     return 0;}
   count++;
   l+=2;
  }
  count++;
  switch(count){
   case 1:
     money*=10;
     break;
   case 2:
   case 3:
     money*=6;
     break;
   case 4:
   case 5:
     money*=3;
     break;
   case 6:
     money*=2;
     break;
   case 7:
     money*=2;
     break;
   case 8:
     money*=1.1;
     break;
   case 9:
     money+=10;
     break;
/*   case 8:
     money*=2;
     break;*/
   default:
 /*    money/=2;*/
     money=1;
     break;}
   inmoney(money);

  prints("\n½Kì¶Œ¦ÁË..²ÂÁË[32m %d[m ´Î ÙpÌÇÌÇ [33;45m%d[m îw",count,money);
  pressanykey();

  return 0;
}

int compare(char ans[],char buf[],int c)
{
 int i,j,A,B;

 A=0;
 B=0;
 for(i=0;i<4;i++)
  if(ans[i]==buf[i])
  A++;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   if((ans[i]==buf[j])&&(i!=j))
    B++;
 prints("%s",buf);
 prints("  ½Y¹û: %d A %d B Ê£ %d ´Î\n",A,B,9-c);
 /*  pressanykey(); */
  if(A==4)
   return 1;
 else
   return 0;
}

int ga(char buf[],int l)
{
  int q,ok=0;

  getdata(l, 0, "İ”ÈëËù²ÂµÄ”µ×Ö(ËÄÎ»²»ÖØ¸²)£º", buf, 5, DOECHO,NULL);
  if (q=(strlen(buf))!=4){
       if (buf[0]=='z'&&buf[1]=='k')
   return 0;
       if (buf[0]=='q')
   return 0;
    outs("yí..²»×ã4Î»");
   /* pressanykey();*/
    return 0; }
  if((buf[0]!=buf[1])&&(buf[0]!=buf[2])&&(buf[0]!=buf[3])
   &&(buf[1]!=buf[2])&&
     (buf[1]!=buf[3])&&(buf[2]!=buf[3])) ok=1;
   if(ok!=1){
    outs("ÖØ¸²‡Ó");
    /*pressanykey();*/
    return 0;}

  return 0;
}
int nam(useri)
{
  FILE *fp;
  char fname[50];
        getuser(useri);
      sethomefile(fname, useri, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       return NULL;
    }
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
  return Name;

    }


  int gold,x[9]={0},ran,q_mon,p_mon;
  unsigned long int bank;
  char buf[1],buf1[6];

int mary_m()
{
   FILE *fp;

     setutmpmode(MARY);
     if ((fp = fopen("etc/mary", "r")) == NULL){
            fp = fopen("etc/mary", "w");
                 fprintf(fp,"%ld",1000000);
            fclose(fp);}
     fp = fopen("etc/mary", "r");
     fscanf(fp,"%ld",&bank);
     fclose(fp);
   clear();
   clrtobot();
   p_mon=0;
   q_mon=cuser.money;
   show_m();

   fp = fopen("etc/mary", "r+");
   fprintf(fp,"%ld",bank);
   fclose(fp);
   return;
}

int show_m()
{
   int i,j,k,m;

   move(0,0);
   clear();
   outs("              ??    ??
            £¯    £Ü£¯    £Ü
           £ü ¨T¨r £ü ¨q¨T £ü
            £Ü___£¯¡¡£Ü??£¯
            ©¦  ___  ___  ©¦
          £¨©¦¨Š_¡ö¨Š_¡ö  ©¦£©
        (~~.©¦   £Ü£÷£¯    ©¦.~~)
       `£Ü£¯ £Ü    £ï    £¯ £Ü£¯
   ¡¡     £Ü   £Ü???£¯   £¯
   ¡¡       £Ü£¯£ü £ü £ü£Ü£¯
     ¡¡      ©¦  ¨‰£Ï¨‰  ©¦
     ¡¡     ¨Š___£¯£Ï£Ü___¨‡
       ¡¡      £Ü__£ü__£¯    [31;40mšgÓ­¹âÅRĞ¡¬”Àò..[m");

      move(b_lines-10,0);
   prints("¬FÓĞÌÇ¹û: %-d            ±¾™CÅ_ƒÈ¬F½ğ: %-ld",q_mon,bank);
   move(b_lines-9,0);


prints("[36m©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤[m\n");

   prints("ÌO¹û-1 bar-2  777-3  Íõ¹Ú-4 BAR-5  âèK-6 Î÷¹Ï-7 éÙ×Ó-8 ÀóÖ¦-9\n");
   prints("x5     x40    x30    x25    x50    x20    x15    x10    x2±¶\n");
   prints("%-7d%-7d%-7d%-7d%-7d%-7d%-7d%-7d%-7d\n",x[0],x[1],x[2],x[3],x[4],x[5],
           x[6],x[7],x[8]);

prints("\n[36m©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤°´aÈ«‰º©¤©¤©¤©¤°´sé_Ê¼©¤©¤°´qëxé_©¤©¤[m");
   getdata(b_lines-3, 0, "ÒªÑº×Ì–(¿ÉÑº¶à´Î)£º", buf, 2, DOECHO,NULL);
   switch(buf[0]){
   case 's':
        doit();
        return;
        break;
   case 'a':
        getdata(b_lines-2, 0, "ÒªÑº¶àÉÙÌÇ£º", buf1, 6, DOECHO,NULL);
        for(i=0;i<=8;i++)
         x[i]=atoi(buf1);
         j=(x[0]*9);
         j=abs(j);
        if(q_mon<j)
          {outs("ÌÇ¹û²»×ã");
           pressanykey();
           for(i=0;i<=8;i++)
                x[i]=0;
           show_m();
           return;
           j=0;
           }
     /*    demoney(j);*/
        q_mon-=j;
        p_mon+=j;
 /*       strcpy(buf," ");*/
        show_m();
        return;
        break;
   case 'q':
     for(i=0;i<=8;i++)
      x[i]=0;
        return;
        break;
   case 't':
        m=10000000;
        for(i=1;i<=5;i++){
         clear();
         move(20,i);
         outs("x");
         if(i%3==0)
          m*=10;
          for(j=1;j<=m;j++)
          k=0;

          refresh();

         }
        return;
        break;
   default:
   i=atoi(buf);
        break;
   }
   k=x[i-1];
   do{
   getdata(b_lines-2, 0, "ÒªÑº¶àÉÙÌÇ£º", buf1, 6, DOECHO,NULL);
   x[i-1]+=atoi(buf1);
        j=atoi(buf1); }while(x[i-1]<0);

/*   j=x[i-1];*/
   if(j<0)
        j=abs(j);
   if(q_mon<j)
        {outs("ÌÇ¹û²»×ã");
         pressanykey();
         x[i-1]=k;
         clear();
         j=0;}
      q_mon-=j;
      p_mon+=j;
  /* demoney(j);*/
   show_m();
   return;
}

int doit()
{
   int i,j,k,m,seed,flag=0,flag1=0;
   int g[10]={5,40,30,25,50,20,15,10,2,0};

   clear();
   move(0,0);
/*   outs("
                       ¨€¨€¨€¨€¨€¨€¨€¨€¨€¨€¨€
                       ¨€                  ¨€
                       ¨€                  ¨€
                       ¨€ £Î£É£Î£Ô£Å£Î£Ä£Ï ¨€
                       ¨€  ÕıÔÚŞD®”ÖĞ      ¨€
                       ¨€      ×ÔĞĞÏëÏñ    ¨€
                       ¨€                  ¨€
                       ¨€¨€¨€¨€¨€¨€¨€¨€¨€¨€¨€
                              NINTENDO

                          ¡ü
                        ¡û¡ò¡ú           ¡ñ
                          ¡ı          ¡ñ
                               ¨x  ¨x    .....
                                        .......
                                        .....¨
                                                              ");
*/
        m=1000000;
        for(i=1;i<=30;i++){
         clear();
         move(10,i);
         outs("¡ñ");
         if(i%23==0)
          m*=10;
          for(j=1;j<=m;j++)
          k=0;

          refresh();

         }
   demoney(p_mon);
   refresh();
   sleep(1);
   clear();
   move(10,31);
   gold=0;
   seed=1000;
   if(p_mon>=50000)
     seed=1500;

   do{
   ran=rand()%seed;
   flag1=0;

   move(10,31);
   if(ran<=480)
     {  outs("ÀóÖ¦");
        j=8;}
    else if(ran<=670){
        outs("ÌO¹û");
        j=0;}
    else if(ran<=765){
        outs("éÙ×Ó");
        j=7;}
    else if(ran<=830){
        outs("Î÷¹Ï");
        j=6;}
    else if(ran<=875){
        outs("âèK");
        j=5;}
    else if(ran<=910){
        outs("Íõ¹Ú");
        j=3;}
    else if(ran<=940){
        outs("777!");
        j=2;}
    else if(ran<=960){
        outs("bar!");
        j=1;}
    else if(ran<=975){
        outs("BAR!");
        j=4;}
    else {
      /*  outs("test          ÏûÈ¥ÓÒß…  ÔÙÅÜÒ»´Î\n");
        for(i=4;i<=8;i++)*/
          outs("ã‘Öx»İî™");
       /* for(i=0;i<=8;i++)
         x[i]=0;*/
        j=9;
          }
   gold=x[j]*g[j];
   if(!flag)
    if(gold>=10000){
       flag=1;
       flag1=1;
     }
            /*    } while( ran>976 || flag1 );*/
                  } while(flag1);
   refresh();
   sleep(1);
   move(11,25);
   prints("[32;40mÄã¿ÉµÃ[33;41m %d [32;40mÌÇÌÇ[m",gold);
   refresh();
   if (gold>0){
      bank-=gold;
      bank+=p_mon;
      }
   else
      bank+=p_mon;

   inmoney(gold);
   pressanykey();
   for(i=0;i<=8;i++)
   x[i]=0;
   p_mon=0;
   q_mon=cuser.money;

   show_m();
   return;
}

