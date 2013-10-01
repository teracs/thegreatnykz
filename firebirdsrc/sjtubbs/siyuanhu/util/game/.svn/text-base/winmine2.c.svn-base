#include "time.h"
#include "stdio.h"
#include "termios.h"
#include <stdlib.h>

int a[32][20];  //À×
int m[32][20];  //marked
int o[32][20];  //opened
char topID[20][20],topFROM[20][32];
char userid[20]="unknown.", fromhost[20]="unknown.";
int topT[20], gameover=0;
static char buf[10000]; // output buffer
struct termios oldtty, newtty; 

int main(int n, char* cmd[]) {
    tcgetattr(0, &oldtty);
    cfmakeraw(&newtty);
    tcsetattr(0, TCSANOW, &newtty);
    if(n>=2) strcpy(userid, cmd[1]);
    if(n>=3) strcpy(fromhost, cmd[2]);
    syslog("ENTER");
    winmine();
    tcsetattr(0, TCSANOW, &oldtty);
}

int show_issue() {
	clear();
	prints("\r\n\r\n\r\nËæ×ÅÊ±¼ä½øÈëÁË¹«Ôª¶þÊ®Ò»ÊÀ¼Í£¬É¨À×¹¤¾ßÖð½¥ÖÇÄÜ»¯£¬ÓÚÊÇ¡£¡£¡£\r\n");
	prints("\r\n[1;32m¸ÐÓ¦Ê½É¨À×[m³öÏÖÁË£¡\r\n\r\n");
	prints("[20;1H[1;33mftp://bbs.nju.edu.cn/pub/bbs/dosmine.exe [m»¹ÓÐ±¾ÓÎÏ·µÄWindowsÊó±ê°æ, »¶Ó­ÏÂÔØ.\r\n");
	refresh();
	pressanykey();
}

int winmine() {
    int x,y;
    show_issue();
    win_showrec();
    clear();
    refresh();
    while(1) {
        clear();
        for (x=0;x<=31;x++)
        for (y=2;y<=19;y++) {
             a[x][y]= 0;
             m[x][y]= 0;
             o[x][y]= 0;
        }
        winrefresh();
        move(0,0);
        prints("[44;37m                     --== ¼üÅÌÉ¨À× ==--                     [m");
        winloop();
        pressanykey();
    }
}

int num_mine_beside(int x1, int y1) {
    int dx, dy, s;
    s= 0;
    for(dx= x1-1; dx<=x1+1; dx++)
    for(dy= y1-1; dy<=y1+1; dy++)
        if(!(dx==x1&&dy==y1)&&a[dx][dy]) s++;
    return s; 
}

int num_mark_beside(int x1, int y1) {
    int dx, dy, s;
    s= 0;
    for(dx= x1-1; dx<=x1+1; dx++)
    for(dy= y1-1; dy<=y1+1; dy++)
        if(!(dx==x1&&dy==y1)&&m[dx][dy]) s++;
    return s;    
}

int wininit(int x1, int y1) {
    int n, x, y;
    randomize();
    for(n=1; n<=99; n++) {
        do {
            x= rand()%30 +1;
            y= rand()%16 +3;
        }
        while(a[x][y]!=0||(abs(x-x1)<2&&abs(y-y1)<2));
        a[x][y]=1;
    }
}

/* Ë«¼ü */
int dblclick(int x, int y) {
    int dx, dy;
    if(x<1|| x>30|| y<3|| y>18) return;
    if(!o[x][y]) return;
    if(num_mine_beside(x, y)!=num_mark_beside(x, y)) return; 
    for(dx=x-1;dx<=x+1;dx++)
    for(dy=y-1;dy<=y+1;dy++)
        windig(dx, dy);
}

/* ×ó¼ü */
int windig(int x, int y) {
    int dx, dy;
    if(x< 1|| x> 30|| y< 3|| y> 18) return;
    if(o[x][y]||m[x][y]) return;
    o[x][y]=1;
    winsh(x, y);
    if(a[x][y]) {
    	 show_mines();
         gameover=1;
         return;
    }
    if(num_mine_beside(x, y)==0) {
        for(dx=x-1;dx<=x+1;dx++)
        for(dy=y-1;dy<=y+1;dy++)
            windig(dx, dy);
    }
}

/* ÏÔÊ¾[x][y]´¦ */
int winsh(int x, int y) {
    move(x*2-2, y-1);
    winsh0(x, y); 
}

/* Í¬ÉÏ, ¼Ó¿ìËÙ¶È */
int winsh0(int x, int y) {
    int c, d;
    static char word[9][10]= {
        "¡¤", "£±", "£²", "£³", "£´", "£µ", "£¶", "£·", "£¸"
    };
    static int cc[9]= {38, 37, 33, 31, 35, 36, 32, 40, 39};  
    char buf[100];
    if (!o[x][y]&&!m[x][y]) {
        prints("[0;32m¡ù");
        return;
    }
    if (m[x][y]) {
        prints("[1;30m¡ñ");
        return;
    }
    if (a[x][y]) {
        prints("[1;31mÀ×[m"); 
        return;  
    } 
    c= num_mine_beside(x, y);
    d= 1;
    if(c==0) d=0; 
    sprintf(buf, "[%d;%dm%s[m", d, cc[c], word[c]); 
    prints(buf);  
}

int marked;

int winloop()
{
    int x, y, c, marked, t0, inited;
    char buf[100];
    x= 10;
    y= 10;
    inited= 0; 
    marked= 0; 
    clearbuf();
    t0= time(0);
    while(1) {
    	int oldx, oldy;
        c= egetch();
        if((c==257||c=='k')&&y>3) y--;
        if((c==258||c=='j')&&y<18) y++;
        if((c==260||c=='h')&&x>1) x--;
        if((c==259||c=='l')&&x<30) x++;
        move(0, 20);
        sprintf(buf, "[1;30mÊ±¼ä:[1;33m %d [m", time(0)-t0);
        prints(buf);
        move(40, 20);
        sprintf(buf, "[1;30m±ê¼Ç:[1;33m %d [m", marked);
        prints(buf);
        move(0, 21);
        sprintf(buf, "[1;30m×ø±ê:[1;33m %3d, %3d[m", x, y);
        prints(buf);
        move(x*2-2, y-1);
        if(c=='h'|| c=='H') winhelp();
        if(c=='d'|| c=='D') winrefresh();
		if(x!=oldx || y!=oldy) {
			windig2(x, y);
			oldx=x; 
			oldy=y;
		}
        if(c=='f'|| c=='F'){
             if(!inited) {
                  wininit(x, y);
                  inited= 1;
             } 
             dig(x, y);      
        } 
        if((c==83|| c==115)&&!o[x][y]) {
             if(m[x][y]){
                  m[x][y]=0;
                  marked--;
             } else {
                  m[x][y]=1;
                  marked++;
             }
             winsh(x, y);
        }
        if(checkwin()==1) {
            move(0, 22);
            prints("×£ºØÄã£¡Äã³É¹¦ÁË£¡                    ");
            {  char buf[100];
               sprintf(buf, "finished in %d s.", time(0)-t0);
               syslog(buf);
            }
            gameover= 0;
            win_checkrec(time(0)-t0);/* added by soff ½øÐÐÅÅÐÐ¼ì²é */
            return;
        }
        if(gameover) {
            move(0, 22);
            prints("ºÜÒÅº¶£¬ÄãÊ§°ÜÁË... ÔÙÀ´Ò»´Î°É£¡                                 ");
            {  char buf[100];
               sprintf(buf, "failed in %d s.", time(0)-t0);
               syslog(buf);
            }
            gameover= 0;
            return;
        }
        move(x*2-2, y-1);
        refresh();
    }
}

int checkwin() {
    int x,y,s;
    s=0;
    for(x=1; x<=30; x++)
    for(y=3; y<=18; y++)
        if(!o[x][y])s++;
    if(s==99) return 1;
    return 0;
}

int dig(int x, int y) {
    if (!o[x][y]) 
           windig(x, y);
    else 
           dblclick(x, y);
}

int num_untouched_beside(int x, int y) {
	int x1, y1;
	int s=0;
	for(x1=x-1; x1<=x+1; x1++)
	for(y1=y-1; y1<=y+1; y1++) {
		if(x1<1 || x1> 30 || y1 <3 || y1>18) continue;
		if(!o[x1][y1]) s++;
	}
	return s;
}

int windig2(int x, int y) {
	int x1, y1;
	dblclick(x, y);
	if(o[x][y]) {
		if(num_untouched_beside(x, y)==num_mine_beside(x, y)) {
			for(x1=x-1; x1<=x+1; x1++)
			for(y1=y-1; y1<=y+1; y1++)
				if(!o[x1][y1] && !m[x1][y1] && (x1>0 && x1 < 31 && y1>2 && y1<19)) {
					m[x1][y1]=1;	
					marked++;
					winsh(x1, y1);
				}

		}
	} 
}

int winrefresh() {
    int x, y;
    clear();
    move(0, 22);
    prints("[1;32m¡î¼üÅÌÉ¨À×¡î[0;1m [[35m°ïÖú: H[37m] [[36mÍË³ö: Q[37m] [[35m´ò¿ª: F[37m] [[36m±êÀ×: S][m\n\r");
    prints("[v2.01 by soff bitbbs.org ±±¾©Àí¹¤´óÑ§] °´ \'[1;32mH[m\' »ñµÃ°ïÖú, \'[1;32mCtrl+C[m\' ÍË³ö¡£");
    for(y=3; y<=18; y++) { 
        move(0, y-1);
        for(x=1; x<=30; x++)
            winsh0(x, y);
    }
    refresh();  
}

int winhelp() {
    clear();
    prints("==»¶Ó­À´Íæ¼üÅÌÉ¨À×ÓÎÏ·==  (³ÌÐòÓÉnjuBBSÕ¾³¤zhchÉè¼Æ BITBBSÕ¾³¤soff¸Ä½ø)\r\n---------------------------------\\r\n\r\n");
    prints("Íæ·¨ºÜ¼òµ¥£¬ºÍ[1;34mwindows[mÏÂµÄÊó±êÉ¨À×²î²»¶à.\r\n");
        prints("  '[1;32mF[m'¼üµÄ×÷ÓÃÏàµ±ÓÚÊó±êµÄ×ó¼ü¼°Ë«»÷µÄ×÷ÓÃ£¬ ³ÌÐò¸ù¾ÝÄãµã»÷µÄÎ»ÖÃ\r\n");
        prints("  ×Ô¶¯ÅÐ¶ÏÒª½øÐÐÄÄÖÖ²Ù×÷¡£\r\n");
        prints("  '[1;32mS[m'¼üÔòÏàµ±ÓÚÊó±êÓÒ¼üµÄ¹¦ÄÜ, ¿ÉÓÃÀ´±êÀ×.\r\n");
        prints("  '[1;32mH[m'¼üÓÃÀ´ÏÔÊ¾±¾°ïÖúÐÅÏ¢.\r\n");
        prints("  '[1;32mQ[m'¼üÍË³öÓÎÏ·.\r\n");
        prints("  µ±ÆÁÄ»ÂÒµôÊ±£¬¿ÉÓÃ'[1;32mD[m'¿ÉÓÃÀ´Ë¢ÐÂÆÁÄ»¡£\r\n");
        prints("½¨ÒéÓÃ[1;32mNetterm[mÀ´Íæ(µ±È»njutermÒ²¿ÉÒÔ,:)),[1;32mtelnet[mÐ§¹û²»ÊÇÌ«ºÃ\r\n");
        prints("µÚÒ»´Îµã»÷Ò»¶¨»á¿ªÒ»Æ¬£¬ºÜÊæ·þ°É¡£\r\n");
        prints("ÊìÁ·ºó£¬ËÙ¶È»¹ÊÇºÜ¿ìµÄ£¬¼¸ºõ¿ÉÒÔ´ïµ½Êó±êÉ¨À×µÄËÙ¶È.\r\n");
        pressanykey();
        winrefresh();
}

int win_loadrec() {
    FILE *fp;
    int n;
    for(n=0; n<=19; n++) {
        strcpy(topID[n], "null.");
        topT[n]=999;
        strcpy(topFROM[n], "unknown.");
    }
    fp=fopen("mine3.rec", "r");
    if(fp==NULL) {
        win_saverec();
        return;
    }
    for(n=0; n<=19; n++)
        fscanf(fp, "%s %d %s\n", topID[n], &topT[n], &topFROM[n]);
    fclose(fp);
}

int win_saverec() {
    FILE *fp;
    int n;
    fp=fopen("mine3.rec", "w");
    for(n=0; n<=19; n++) {
        fprintf(fp, "%s %d %s\n", topID[n], topT[n], topFROM[n]);
    }
    fclose(fp);
}

int win_showrec() {
    char buf[200];
    int n;
    win_loadrec();
    clear();
    prints("[44;37m                         --== É¨À×ÅÅÐÐ°ñ ==--                             \r\n[m");
    prints("[41m No.          ID        Ê±¼ä                         À´×Ô                      [m\r\n");
    for(n=0; n<=19; n++) {
        sprintf(buf, "[1;37m%3d[32m%13s[0;37m%12d[m%29s\r\n", n+1, topID[n], topT[n], topFROM[n]);
        prints(buf);
    }
    sprintf(buf, "[41m                                                                               [m\r\n");
    prints(buf);
    pressanykey();
}

int win_checkrec(int dt) {
    char id[20];
    int n;
    win_loadrec();
    strcpy(id, userid);
    for(n=0; n<=19; n++)
    if(!strcmp(topID[n], id)) {
        if(dt< topT[n]) {
            topT[n]= dt;
            strcpy(topFROM[n], fromhost);
            win_sort();
            win_saverec();
        }
        return;
    }
    if(dt<topT[19]) {
        strcpy(topID[19], id);
        topT[19]= dt;
        strcpy(topFROM[19], fromhost);
        win_sort();
        win_saverec();
        return;
    }
}

int win_sort() {
    int n, n2, tmp;
    char tmpID[20];
    clear();
    prints("×£ºØ! ÄúË¢ÐÂÁË×Ô¼ºµÄ¼ÍÂ¼!\r\n");
    pressanykey();
    for(n=0; n<=18; n++)
    for(n2=n+1; n2<=19; n2++)
    if(topT[n]> topT[n2]) {
        tmp= topT[n];
        topT[n]= topT[n2];
        topT[n2]= tmp;
        strcpy(tmpID, topID[n]);
        strcpy(topID[n], topID[n2]);
        strcpy(topID[n2], tmpID);
        strcpy(tmpID, topFROM[n]);
        strcpy(topFROM[n], topFROM[n2]);
        strcpy(topFROM[n2], tmpID);
    }
}

int clear() {
    prints("[H[J");
}

int refresh() {
    write(0, buf, strlen(buf));
    buf[0]= 0;
}

int prints(char* b) {
    strcat(buf, b);
}

int randomize() {
    srandom(time(0));
}

int move(int x, int y) {
    char c[100];
    sprintf(c, "[%d;%dH", y+1, x+1);
    prints(c); 
}

int egetch() {
    int c,d,e;
    c=getch0();
    if(c==3||c==4||c==-1||c=='q'||c=='Q') quit();
    if(c!=27) return c;
    d=getch0();
    e=getch0();
    if(e=='A') return 257;
    if(e=='B') return 258;
    if(e=='C') return 259;
    if(e=='D') return 260;
    return 0;
}

int getch0() {
    char c;
    if(read(0, &c, 1)<=0) quit();
    return c; 
}

int quit() {
    tcsetattr(0, TCSANOW, &oldtty);
    clear();
    refresh();
    syslog("QUIT");
    exit(0); 
}

int pressanykey() {
    refresh();
    clearbuf(); 
}

int clearbuf() {
    char buf[128];
    refresh();
    read(0, buf, 100); 
}

syslog(char* cc )
{
   FILE *fp;
   time_t t;
   t=time(0);
   fp=fopen("winmine.log","a");
   fprintf(fp,"%s did %s on %s", userid, cc, ctime(&t));
   fclose(fp);
}

// added by soff ÏÔÊ¾ËùÓÐµÄÀ× 
int show_mines() { 
 int x, y; 
 for(x=1; x<=30; x++) 
      for(y=3; y<=18; y++) 
      { 
       if (a[x][y]) { 
           move(x*2-2, y-1); 
           prints("[31mÀ×"); 
          } 
         } 
        return; 
}
