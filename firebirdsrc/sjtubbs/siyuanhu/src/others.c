/*±¾º¯ÊıÓÃÓÚÌí¼ÓFB2000ÒÔÍâµÄ¹¦ÄÜ£¬±ãÓÚÉı¼¶ jieer jb@xiao.com.cn 2001.05.01 */


#include <bbs.h>
#include <netdb.h>
#include <netinet/in.h>

#ifdef ALLOWGAME
#define BBS_PAGESIZE    (t_lines - 4)
char    replytitle[STRLEN];
extern char quote_file[120], quote_user[120];
unsigned int free_addr[2000], free_mask[2000], free_num;
char tt_id[20][20], tt_ip[20][30];
int tt_scr[20];



search_ip(unsigned int addr) {
        FILE *fp;
        char buf[512], buf2[80];
        int b1, b2, b3, b4, c1, c2, c3, c4, find=0;
        unsigned int i1=addr/(256*256*256), i2=(addr/65536)%256;
        unsigned int i3=(addr/256)%256, i4=(addr%256);
        unsigned int v=i4*65536*256+i3*256*256+i2*256+i1, v1, v2;
        fp=fopen("etc/ip_range.txt", "r");
        if(fp==0) return;
        prints("IPµØÖ· [%d.%d.%d.%d]\n", i4, i3, i2, i1);
        while(1) {
                buf[0]=0;
                if(fgets(buf, 500, fp)<=0) break;
                if(strlen(buf)<10) continue;
                sscanf(buf, "%d.%d.%d.%d %d.%d.%d.%d %s",&b1, &b2, &b3, &b4, &c1, &c2, &c3, &c4, buf2);
                v1=b1*65536*256+b2*256*256+b3*256+b4;
                v2=c1*65536*256+c2*256*256+c3*256+c4;
                if(v1<=v && v<=v2) {
                        prints("¸ÃIPÊôÓÚ: %s\n", buf2);
                        find++;
                }
        }
        fclose(fp);
        if(find==0)
                prints("Ã»ÓĞÕÒµ½Æ¥ÅäµÄ¼ÇÂ¼£¡\n");        else
                prints("Ò»¹²ÕÒµ½ %d ¸öÆ¥Åä¼ÇÂ¼¡£\n", find);
        prints("\n\n\n»¶Ó­Ê¹ÓÃ±¾²éÑ¯ÏµÍ³¡£Èç¹û·¢ÏÖ´íÎóÖ®´¦»òÓĞ¸Ä½øÒâ¼û£¬ÇëÔÚsysop°æ·¢ÎÄ¡£\n");
        }
int get_free_list() {
FILE *fp;
char buf1[100], buf2[100], buf3[100];
free_num= 0;
fp=fopen("etc/free.txt", "r");
if (fp== NULL) return;
while(fscanf(fp, "%s%s%s", buf1, buf2, buf3)>0) {
free_addr[free_num]= inet_addr(buf1);
free_mask[free_num]= inet_addr(buf2);
free_num++;
}
fclose(fp);
}

int is_free(unsigned int x) {
int n;
for (n= 0; n< free_num; n++)
if(((x^ free_addr[n])| free_mask[n])==free_mask[n]) return 1;
return 0;
}

free_ip() {
struct hostent *he;
struct in_addr i;
char buf1[200];
int addr;
report("free_ip lookup");
get_free_list();
modify_user_mode(FREEIP);
while(1) {
clear();
getdata(3, 0, "ÇëÊäÈëÒª²éÑ¯µØÖ·µÄ[1;32mip[m»òÕß[1;32mÓòÃû[m(Ö±½Ó»Ø³µÍË³ö): ", buf1, 40, DOECHO, YEA);
if(buf1[0]==0) return;
if ((he = gethostbyname(buf1)) != NULL)
addr=*(int*) he->h_addr;
else
addr=inet_addr(buf1);
if(addr==-1)
prints("²»´æÔÚµÄµØÖ·\n\n");
else {
i.s_addr=addr;
if(is_free(addr))
prints("\n\n¸ù¾İCERNET freeip v20010317\n%s(%s) ÊÇÒ»¸öÃâ·Ñip.\n\n", buf1, inet_ntoa(i));
else
prints("\n\n¸ù¾İCERNET freeip v20010317\n%s(%s) ²»ÊÇÃâ·Ñip.\n\n", buf1, inet_ntoa(i));
search_ip(addr);
}
pressanykey();
}
}
int
show_logrecord(){
        char    fname[STRLEN];
        if(!strcmp(currentuser.userid,"guest"))return;
        setuserfile(fname, "logrecord");
        clear();
        modify_user_mode(LOOKLOG);
        if (dashf(fname)) {
                mesgmore(fname, YEA, 0, 9999);
                clear();
        } else {
                move(5, 30);
                prints("Ã»ÓĞÈÎºÎµÄ¼ÇÂ¼´æÔÚ£¡£¡");
                pressanykey();
                clear();
        }
}
int tt_load_record() {
  int n;
  FILE *fp=fopen("game/tt.dat", "r");
  if(fp==0) {
    fp=fopen("game/tt.dat", "w");
    for(n=0; n<20; n++)
      fprintf(fp, "%s %s %d\n", "none", "0.0.0.0", 0);
    fclose(fp);
    fp=fopen("game/tt.dat", "r");
  }
  for(n=0; n<20; n++)
    fscanf(fp, "%s %s %d", tt_id[n], tt_ip[n], &tt_scr[n]);
  fclose(fp);
}

int tt_save_record() {
  int n, m1, m2;
  char id[20], ip[30];
  int scr;
  FILE *fp=fopen("game/tt.dat", "w");
  for(m1=0; m1<20; m1++)
  for(m2=m1+1; m2<20; m2++)
    if(tt_scr[m1]<tt_scr[m2]) {
      strcpy(id, tt_id[m1]);
      strcpy(ip, tt_ip[m1]);
      scr=tt_scr[m1];
      strcpy(tt_id[m1], tt_id[m2]);
      strcpy(tt_ip[m1], tt_ip[m2]);
      tt_scr[m1]=tt_scr[m2];
      strcpy(tt_id[m2], id);
      strcpy(tt_ip[m2], ip);
      tt_scr[m2]=scr;
    }
  for(n=0; n<20; n++)
    fprintf(fp, "%s %s %d\n", tt_id[n], tt_ip[n], tt_scr[n]);
  fclose(fp);
}

int tt_check_record(int score) {
  int n;
  tt_load_record();
  for(n=0; n<20; n++)
    if(!strcasecmp(tt_id[n], currentuser.userid)) {
      if(tt_scr[n]>score) return 0;
      tt_scr[n]=score;
      strncpy(tt_ip[n], uinfo.from, 16);
      tt_ip[n][16]=0;
      tt_save_record();
      return 1;
    }
  if(tt_scr[19]<score) {
    tt_scr[19]=score;
    strcpy(tt_id[19], currentuser.userid);
    strncpy(tt_ip[19], uinfo.from, 16);
    tt_ip[19][16]=0;
    tt_save_record();
    return 1;
  }
  return 0;
}

int tt_game() {
  char c[30], a;
  char chars[]="£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ğ£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú";
  int m, n, t, score;
  randomize();
  modify_user_mode(TT);
  report("tt game");
  tt_load_record();
  printf("[2J±¾Õ¾´ò×Ö¸ßÊÖÅÅĞĞ°ñ\r\n\r\n%4s %12.12s %24.24s %5s(WPMs)\r\n", "Ãû´Î", "ÕÊºÅ", "À´Ô´", "ËÙ¶È");
  for(n=0; n<20; n++)
    printf("%4d %12.12s %24.24s %5.2f\r\n", n+1, tt_id[n], tt_ip[n], tt_scr[n]/10.);
  fflush(stdout);
  read(0, genbuf, 32);
  printf("[2J[1;32m£Â£Â£Ó[0;1m´ò×ÖÁ·Ï°³ÌĞò. (´óĞ¡Ğ´¾ù ¿É, ÊäÈëµÚÒ»×Ö·ûÇ°µÄµÈ´ı²»¼ÆÊ±. [1;32m^C[m or[1;32m^D[m ÍË³ö.)\r\n\r\n\r\n");

start:
  for(n=0; n<30; n++) {
    c[n]=rand()%26;
    printf("%c%c", chars[c[n]*2], chars[c[n]*2+1]);
  }
  printf("\r\n");
  fflush(stdout);
  m=0;
  t=times(0);
  while(m<30) {
    while(read(0, genbuf, 32)>1);
    a=genbuf[0];
    if(a==c[m]+65 || a==c[m]+97) {
      printf("%c%c", chars[c[m]*2], chars[c[m]*2+1]);
      if(m==0) {
        if(abs(times(0)-t)>300) {
           printf("\r\n³¬Ê±! Äã±ØĞëÔÚ[1;32m3[0;1mÃëÖÓÒÔÄÚ¿ªÊ¼!\r\n");
           goto start;
        }
        t=times(0);
      }
      m++;
      fflush(stdout);
      usleep(60000);
    }
    if(genbuf[0]==3||genbuf[0]==4) return 0;
  }
  score=360000/(times(0)-t);
  printf("\r\n\r\nSpeed=%5.2f WPMs\r\n\r\n", score/10.);
  if(tt_check_record(score)) printf("[1;33m×£ºØ£¡ÄúË¢ĞÂÁË×Ô¼ºµÄ¼ÍÂ¼£¡[m\r\n\r\n");
  fflush(stdout);
  goto start;
}

int x_queryuser(){
   char buf[100],filebuf[80];
   struct stat statbuf;
   modify_user_mode(QUERYUSER);
   clear();refresh();
   sprintf(buf,"bin/query %u %u %u %u",currentuser.numlogins,currentuser.numposts,currentuser.stay,currentuser.firstlogin);
   sprintf(genbuf,"xquery %s",buf);
   report(genbuf);
   sprintf(filebuf,"tmp/%s.queryuser",currentuser.userid);
   sprintf(genbuf,"%s > %s",buf,filebuf);
   system(genbuf);
   bzero(&statbuf, sizeof(statbuf));
   stat(filebuf, &statbuf);
   if(statbuf.st_size>1) {
      ansimore(filebuf, YEA);
      unlink (filebuf);
   } else {prints("\r\n³ÌĞò³ö´í£¬Çë±¨¸æµ½BBS_Dev°å");pressanykey();}
}
int
timecmp(one,two)
char *one,*two;
{
  char a[5],b[5];
  int i;

  for(i=0;i<4;i++) {
     a[i] = one[i];
     b[i] = two[i];
  }
  if( atoi(a) > atoi(b) )
        return 1;
  else if( atoi(a) < atoi(b) )
        return -1;
  for(i=0;i<2;i++) {
     a[i] = one[i+6];
     b[i] = two[i+6];
  }
  a[2]='\0';b[2]='\0';
  if( atoi(a) > atoi(b) )
        return 1;
  else if( atoi(a) < atoi(b) )
        return -1;
  for(i=0;i<2;i++) {
     a[i] = one[i+10];
     b[i] = two[i+10];
  }
  a[2]='\0';b[2]='\0';
  if( atoi(a) > atoi(b) )
        return 1;
  else if( atoi(a) < atoi(b) )
        return -1;
  else
        return 0;
}

void
gtime(ret,flist)
char *ret,*flist;
{
  int i;
  for(i=18;i<32;i++)
        ret[i-18] = flist[i];
  return;
  return;
}

char * ptime(cti,clock,mo)
char *cti;
int mo;
time_t *clock;
{
   struct tm *nt;
   char weeknum[7][3]={"Ìì","Ò»","¶ş","Èı","ËÄ","Îå","Áù"};

   nt = localtime(clock);

   switch(mo)
   {
     case 0:
      sprintf(cti,"%02d:%02d",nt->tm_hour,nt->tm_min);
      break;
     case 1:
      sprintf(cti," %4dÄê%02dÔÂ%02dÈÕ ",nt->tm_year+1900,
                  nt->tm_mon+1,nt->tm_mday);
      break;
     case 2:
      sprintf(cti,"ĞÇÆÚ%2s  %02d:%02d ",weeknum[nt->tm_wday],
                  nt->tm_hour,nt->tm_min);
      break;
     case 3:
      sprintf(cti,"%4dÄê%02dÔÂ%02dÈÕ ĞÇÆÚ%2s",nt->tm_year+1900,
                  nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday]);
      break;
     case 4:
      sprintf(cti,"%4dÄê%02dÔÂ%02dÈÕ ĞÇÆÚ%2s %02d:%02d",nt->tm_year+1900,
                  nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                  nt->tm_min);
      break;
     case 5:
      sprintf(cti,"%4dÄê%02dÔÂ%02dÈÕ ĞÇÆÚ%2s %02d:%02d:%02d",nt->tm_year+1900,
                  nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                  nt->tm_min,nt->tm_sec);
      break;
     case 6:
      sprintf(cti,"%4dÄê%02dÔÂ%02dÈÕ %02d:%02d",nt->tm_year+1900,
                  nt->tm_mon+1,nt->tm_mday,nt->tm_hour,nt->tm_min);
      break;
     case 7:
      sprintf(cti,"%02dÔÂ%02dÈÕ ĞÇÆÚ%2s %02d:%02d:%02d",
                  nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                  nt->tm_min,nt->tm_sec);
      break;
     default:
      sprintf(cti,"º¯ÊıÊ¹ÓÃ´íÎó");
   }
   return cti;
}

void
search_all(int k)
{
  struct fileheader xfh;
  struct boardheader abo;
  char uident[STRLEN],date[20],buf[STRLEN],choice[2];
//  char searchtitle[STRLEN];
  char flist[10000][80];
  char buf1[STRLEN],buf2[STRLEN];
  int i,j,n,cnum,cpage,num,ch,just=1,z;
  int xiaojb=0;
  int snum=0;
  time_t filetime;

  struct art
  {
        char owner[15],name[30],title[STRLEN],board[30],btitle[STRLEN];
  } art[10000],artbuf;

	if(!HAS_PERM(PERM_SYSOP))
		return;
  
  clear();
  prints("                         çóçìÉ½Ë®BBSÕ¾ÎÄÕÂÈ«Õ¾ËÑË÷¹¦ÄÜ                ");
  modify_user_mode( SAID );
  if(k){move(3,0);prints("ÇëÊäÈëÒªËÑË÷µÄÓÃ»§ÕÊºÅ£º");move(3,24);
  usercomplete(NULL,uident);
  if(uident[0] == '\0') return;
  if( !getuser(uident) ) {
        move(5,0) ;
        clrtoeol();
        prints("\n ²»ÕıÈ·µÄÊ¹ÓÃÕß´úºÅ \n") ;
        pressanykey() ;
        return;
  }}
  else {
  getdata(3,0,"ÇëÊäÈëÒªËÑË÷µÄ±êÌâ¹Ø¼ü×Ö: ",uident,24,DOECHO,YEA);
  if(uident[0]=='\0') return;
  }
  clear();
  prints("                         çóçìÉ½Ë®BBSÕ¾ÎÄÕÂÈ«Õ¾ËÑË÷¹¦ÄÜ                 ");
  move(3,0);
  prints("ÕıÔÚËÑË÷%s %s µÄËùÓĞÎÄÕÂ£¬ÇëÉÔºî...",k?"ÓÃ»§":"±êÌâÎª",uident);
  refresh();
  for( n = 0; n < 133; n++ ) {
        if( get_record(".BOARDS", &abo, sizeof(abo), n) )  continue;
        if( !strcmp("deleted",abo.filename) || !strcmp("junk",abo.filename)
           || !strcmp("newcomers",abo.filename)
           || !strcmp("notepad",abo.filename)
           || !strcmp("syssecurity",abo.filename)
           || !strcmp("Test",abo.filename)
           || !strcmp("leader",abo.filename)
           || !strcmp("Announce",abo.filename)
           || !strcmp("vote",abo.filename)  )
                continue;
        move(5,0);clrtoeol();
        prints("ËÑË÷½ø¶È£º%2d%%",(n*100)/132);
        move(7,0);clrtoeol();
        prints("ÕıÔÚËÑË÷ %s ÌÖÂÛÇø...",abo.filename);
        refresh();
        setbdir(buf,abo.filename);
        i = 1;
        while(!get_record(buf, &xfh, sizeof(xfh), i)) {
              if(k){ if(!strcmp(xfh.owner,uident)) xiaojb=1;else xiaojb=0;}
              else {if(strstr(xfh.title,uident)) xiaojb=1;else xiaojb=0;}
              if( xiaojb ) {
                    snum++;
                    filetime = atoi( xfh.filename + 2 );
                    if( filetime > 740000000 ) {
                        ptime(date,&filetime,1);
                    } else {
                        date[0] = '\0';
                    }
                    if (!strncmp("Re:",xfh.title,3) || !strncmp("RE:",xfh.title,
3)) {
                        sprintf(flist[snum-1],"%-16s %14s %-40s",
                                abo.filename, date, xfh.title);
                        flist[snum-1][72] = '\0';
                    } else {
                        sprintf(flist[snum-1],"%-16s %14s ¡ñ %-37s",
                                abo.filename, date, xfh.title);
                        flist[snum-1][72] = '\0';
                    }
                    z=0;buf1[0]='\0';
                    while(abo.title[z]) {
                        buf1[z] = abo.title[z+11];
                        z++;
                    }buf1[z]='\0';z=0;
                    strcpy(art[snum-1].name,xfh.filename);
                    strcpy(art[snum-1].title,xfh.title);
                    strcpy(art[snum-1].board,abo.filename);
                    strcpy(art[snum-1].btitle,buf1);
                    strcpy(art[snum-1].owner,xfh.owner);
                    }
              i++;
        }
   }
   clear();
   prints("                           çóçìÉ½Ë®BBSÕ¾ÎÄÕÂÈ«Õ¾ËÑË÷¹¦ÄÜ                  ");
   if( snum == 0 ) {
        move(3,0);
        prints(" Ã»ÓĞÕÒµ½%s¡°%s¡±µÄÎÄÕÂ ",k?"ÍøÓÑ":"±êÌâº¬ÓĞ",uident);
        pressanykey();
        return;
   }
   move(3,0);
   prints("È«³ÌËÑË÷Íê±Ï£¬¹²ÕÒµ½%s %s µÄÎÄÕÂ %d Æª\n",k?"":"±êÌâº¬ÓĞ",uident,snum);
   prints("\nÇëÑ¡ÔñÎÄÕÂµÄÅÅÁĞ·½Ê½\n\n");
   prints("(0) Àë ¿ª\n\n");
   prints("(1) °´ÌÖÂÛÇøË³Ğò\n\n");
   prints("(2) °´Ê±¼äË³Ğò (½ÏÂı£¬½¨ÒéÔ¶³ÌÓÃ»§²»ÒªÊ¹ÓÃ)");
   getdata(13,0,"Ñ¡Ôñ [1]£º", choice, 2, DOECHO, NULL, YEA);
lab:
   if( z == 1 )   choice[0]='2';
   if( choice[0] == '2' ) {
//      qsort(art,i,sizeof(struct art),timecmp)
        for(i=0;i<snum-2;i++) {
           for(j=0;j<snum-1-i;j++) {
                gtime(buf1,flist[j]);gtime(buf2,flist[j+1]);
                buf1[14]='\0';buf2[14]='\0';
                if( timecmp(buf1,buf2) > 0 ) {
                       buf[0] = '\0';
                       strncpy(buf,flist[j],72);flist[j][0]='\0';
                       strncpy(flist[j],flist[j+1],72);flist[j+1][0]='\0';
                       strncpy(flist[j+1],buf,72);
                       artbuf=art[j];art[j]=art[j+1];art[j+1]=artbuf;
                }
           }
           if(z==0) {
                move(13,0);prints("Íê³É½ø¶È£º%2d%%",i*100/snum);refresh();
           } else {
                move(23,0);clrtoeol();prints("ÕıÔÚ°´Ê±¼ä½øĞĞÅÅĞò£¬ÇëÉÔºî..                                     [ ½ø¶È£º%2d%% ]",i*100/snum);refresh();
           }
        }
   } else if( choice[0] == '0' )
        return;
   i=0;j=0;z=0;buf[0]='\0';
   num = snum;
   just = 1;
   clear();
   while( 1 )
   {
     move(0,0);prints("                          ·ûºÏÌõ¼şµÄÈ«Õ¾ÎÄÕÂÁĞ±í                     \n");
 move(1,0);prints("[1;44;37m ¿´ÎÄÕÂ[Enter,¡ú] Àë¿ª[q,¡û] ÈçÓĞÎÊÌâÇëµ½BBSDev°å±¨¸æBUG çóçìÉ½Ë®Õ¾Îñ×é[m");
     move(2,0);prints(" ±à ºÅ ÌÖ ÂÛ Çø          ·¢ ±í ÈÕ ÆÚ     ÎÄ  ÕÂ  ±ê  Ìâ                        ");
     move(23,0);clrtoeol();prints("±êÌâ£º%-44s ÌÖÂÛÇø£º%-20s",art[num-1].title,art[num-1].btitle);
     if( num < cnum || num > cnum+BBS_PAGESIZE-1 || just == 1) {
        cpage = num / BBS_PAGESIZE;
        if( cpage*BBS_PAGESIZE != num  ) cpage++;
        cnum = (cpage-1) * BBS_PAGESIZE + 1;
        for(i=cnum;i<BBS_PAGESIZE+cnum;i++)
        {
                move(3+i-cnum,0);
                if( i > snum )  { clrtoeol();move(3+num-cnum,1);continue; }
                if( i == num ) {
                    clrtoeol();prints(">%5d %-72s",i,flist[i-1]);
                } else if( i <= snum ) {
                    clrtoeol();prints(" %5d %-72s",i,flist[i-1]);
                }
                move(3+num-cnum,1);
                just = 0;
        }
      } else {
        move(3+num-cnum,0);prints(">");
      }
        ch = egetch();
        move(2+num-cnum,0);prints(" ");
        switch( ch ) {
            case KEY_UP:
                if( --num <= 0 ) { num = snum;just=1; }
                break;
            case KEY_DOWN:
                if( ++num > snum ) { num = 1;just=1; }
                break;
            case KEY_RIGHT: case '\r':case '\n':
                sprintf(buf,"boards/%s/%s",art[num-1].board,art[num-1].name);
                ansimore(buf,YEA);
                just = 1;
                break;
            case 'r': case 'R':
                strcpy(currboard, art[num-1].board);
                strcpy(replytitle, art[num-1].title);
                setbfile( quote_file, currboard, art[num-1].name );
                strcpy( quote_user, art[num-1].owner );
                digestmode=NA;
                post_article();
                digestmode=YEA;
                replytitle[0] = '\0';
                just = 1;
                break;
            case KEY_LEFT:
                return;
            case Ctrl('B'): num-=BBS_PAGESIZE;
                if(num<0) num=snum+num;break;
            case ' ':case Ctrl('F'):
                num += BBS_PAGESIZE;
                if( num > snum )  num = snum;
                break;
            case Ctrl('S'):
                z = 1;
                goto lab;
                break;
        }
  }
}
int search_all_id()
{ search_all(1);
}
int search_all_title()
{ search_all(0);
}

#endif

  
