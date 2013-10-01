/*TopPostc*/
// SJTU bbs, preview version, fireium@sjtu.org, 2007.2.24 //
// Show etc/post/day_new
#include "TopPost.h"
const char *mytitle[] = { "日十大热门话题",
    "周五十大热门话题",
    "月百大热门话题",
    "年度百大热门话题"
};

const char *myfile[] = { "day_new", "week", "month", "year" };
/*
"%-16s%-20s%-5d%-16s%-60s%-20s\n", top[i].board, p, top[i].number, top[i].author, top[i].title,top2[i].realPath);
0    char author[IDLEN + 1];     // author name 
1   char board[IDLEN + 6];      // board name
2    char title[66];             // title name 
3    char date[16];                // last post's date 
4    char number[5];                 // post number 
5    char realPath[20];			// real path
6    unsigned int reid;
*/
void
show_top_ten (mytype)
int mytype;
{
    FILE *fp;
    int counts	= 0;
	int ch;
    char fname[256];
    char t[256],author[IDLEN+1],board[IDLEN + 6],title[66],date[16],number[5],realPath[20],szreid[12];
    unsigned int reid = 0;
    struct posttop top[10];
	char buf[1024];
	int temp_mode= 0;
	
memset(board,0,sizeof(board));
memset(date,0,sizeof(date));
memset(number,0,sizeof(number));
memset(author,0,sizeof(author));
memset(title,0,sizeof(title));
memset(realPath,0,sizeof(realPath));
    clear ();
   if (mytype > 3) return;
	
    sprintf(fname,"0Announce/bbslist/%s",myfile[mytype]);	
    fp = fopen (fname, "r");
   	if (fp == NULL)
        return;
	move(0,0);
	
   prints("                \033[1;34m-----\033[37m=====\033[41m 本%s \033[m=====\033[34m-----\033[m\n", mytitle[mytype]);	

   while (fgets (t, 255, fp) && counts <10)
    {

	strncpy(board, t, 16);   
	board[IDLEN+6]   =   '\0';   
	strncpy(date,t +16,16);   
	date[16]   =   '\0';  	 
	strncpy(number,t +36,4);   
	number[5]   =   '\0';   
	strncpy(author,t + 41,12);   
	author[IDLEN+1]   =   '\0';  
	strncpy(title,t + 57,59);   
	title[60]   =   '\0';   	
	strncpy(realPath,t + 117,19);   
	realPath[20]   =   '\0';   	
	strncpy(szreid,t + 137,12);   
	 
			

	
	strcpy(top[counts].board,board);
	strcpy(top[counts].date,date);
	strcpy(top[counts].number,number);
	strcpy(top[counts].author,author);
	strcpy(top[counts].title,title);
	strcpy(top[counts].realPath,realPath);
	top[counts].reid = atoi(chomp(szreid));

	move(2+counts*2,0);
	prints("\033[1m第\033[31m%3d\033[37m 名 信区 : \033[33m%-16s\033[37m【\033[32m%s\033[37m】\033[36m%4s \033[37m人\033[35m%16s\n" 
		"    \033[1;37m标题 : \033[44;37m%-60s\033[m\n", counts + 1, top[counts].board, top[counts].date, top[counts].number, top[counts].author, top[counts].title);
	 counts++;
    }
   counts = 1;
   PUTTOP10CURS;
    while (1)
    {
    	if ((ch = egetch ()) == EOF)
			continue;
			
	switch(ch)
    	{
		case KEY_DOWN:
			if(counts <10){
			RMVTOP10CURS;			
			counts++;
   			PUTTOP10CURS;
			move(22,0);
			}
			break;
		case KEY_UP:
			if(counts > 1){
			RMVTOP10CURS;
			counts--;
   			PUTTOP10CURS;
			move(22,0);			
			}
			break;	
		case KEY_LEFT:
				return;
		case KEY_RIGHT:
				{					
                char boardbuf[IDLEN + 6];
				char TempBoard[IDLEN + 6];
				chomp(top[counts-1].board);
				strcpy(TempBoard,currboard);
				strcpy(currboard,top[counts-1].board);
				temp_mode = digestmode;
				digestmode = 10;
				marked_Top_Ten(top[counts-1].reid);
				Read();
				digestmode = temp_mode;
				strcpy(currboard,TempBoard);
                break;
			}
		default:
			break;
	}

    }
}


char*   chomp(char*s)   
{   
	unsigned   len   =   strlen(s);  
	char   *p   =   s+len-1;   
	while   (*p   ==   ' ')   
	{
		*p = '\0';
		p--;
	}   	
	while   (*s   ==   ' ')   s++; 
	return   s;   
}   
