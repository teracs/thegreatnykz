#include <stdio.h>
#include <string.h>
#include "bbs.h"



struct old_userec
{                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    time_t firstlogin;
    char lasthost[16];
    unsigned int numlogins;
    unsigned int numposts;
    unsigned int medals;        /* 奖章数 */
    unsigned int money;         /* 金钱 */
    unsigned int inbank;        /* 存款 */
    time_t banktime;            /* 存入时间 */
    char flags[2];
#ifdef ENCPASSLEN
    char passwd[ENCPASSLEN];
#else
    char passwd[PASSLEN];
#endif
    char username[NAMELEN];
    char ident[NAMELEN];
    char termtype[16];
    char reginfo[STRLEN - 16];
    unsigned int userlevel;
    time_t lastlogin;
    time_t lastlogout;          /* 最近离线时间 */
    time_t stay;
    char realname[NAMELEN];
    char address[STRLEN];
    char email[STRLEN - 12];
    unsigned int nummails;
    time_t lastjustify;
    char gender;
    unsigned char birthyear;
    unsigned char birthmonth;
    unsigned char birthday;
    int signature;
    unsigned int userdefine;
    time_t notedate;
    int noteline;
};


struct new_userec
{                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    time_t firstlogin;
    char lasthost[60];
    unsigned int numlogins;
    unsigned int numposts;
    unsigned int medals;        /* 奖章数 */
    unsigned int money;         /* 金钱 */
    unsigned int inbank;        /* 存款 */
    time_t banktime;            /* 存入时间 */
    char flags[2];
#ifdef ENCPASSLEN
    char passwd[ENCPASSLEN];
#else
    char passwd[PASSLEN];
#endif
    char username[NAMELEN];
    char ident[NAMELEN];
    char termtype[16];
    char reginfo[STRLEN - 16];
    unsigned int userlevel;
    time_t lastlogin;
    time_t lastlogout;          /* 最近离线时间 */
    time_t stay;
    char realname[NAMELEN];
    char address[STRLEN];
    char email[STRLEN - 12];
    unsigned int nummails;
    time_t lastjustify;
    char gender;
    unsigned char birthyear;
    unsigned char birthmonth;
    unsigned char birthday;
    int signature;
    unsigned int userdefine;
    time_t notedate;
    int noteline;
};

void cp_userec(struct old_userec *pou, struct new_userec *pnu)
{
	memcpy(pnu,pou,(char*) &(pou->numlogins) - (char *) pou);
	memcpy( &(pnu->numlogins), &(pou->numlogins),  sizeof(*pou) - ( (char*) &(pou->numlogins) - (char *) pou));
}
int main()
{
	struct old_userec ou;
	struct new_userec nu;

	bzero(&ou,sizeof(ou));
	bzero(&nu,sizeof(nu));
	while( fread(&ou,sizeof(ou),1,stdin) == 1)
	{
		cp_userec(&ou,&nu);
		fwrite(&nu,sizeof(nu),1,stdout);
		bzero(&ou,sizeof(ou));
		bzero(&nu,sizeof(nu));
	}
	return 0;
}

