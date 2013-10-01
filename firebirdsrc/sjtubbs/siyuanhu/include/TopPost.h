
#include "bbs.h"

struct posttop {
    char author[IDLEN + 1];     /* author name */
    char board[IDLEN + 6];      /* board name */
    char title[66];             /* title name */
    char date[16];                /* last post's date */
    char number[5];                 /* post number */
    char realPath[20];			/* real path*/ //add by pax
    unsigned int reid;
};

	
#define PUTTOP10CURS   move(2+counts*2-1,0);	prints("\033[1;32m  ◆\033[1;37m标题 : \033[44;37m%-60s\033[m\n", top[counts-1].title);
#define RMVTOP10CURS   move(2+counts*2-1,0);prints("    \033[1;37m标题 : \033[44;37m%-60s\033[m\n", top[counts-1].title);