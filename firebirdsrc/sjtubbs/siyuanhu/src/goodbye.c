/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: goodbye.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
*/

#include "bbs.h"

//#ifndef DLM
//#undef  ALLOWGAME
//#endif

extern char BoardName[];
typedef struct
{
    char *match;
    char *replace;
}
logout;

int
countlogouts (filename)
char filename[STRLEN];
{
    FILE *fp;
    char buf[256];
    int count = 0;

    if ((fp = fopen (filename, "r")) == NULL)
        return 0;

    while (fgets (buf, 255, fp) != NULL)
    {
        if (strstr (buf, "@logout@") || strstr (buf, "@login@"))
            count++;
    }
    return count + 1;
}

void showstuff (char *buf);

user_display (filename, number, mode)
char *filename;
int number, mode;
{
    FILE *fp;
    char buf[256];
    int count = 1;

    clear ();
    move (1, 0);
    if ((fp = fopen (filename, "r")) == NULL)
        return;

    while (fgets (buf, 255, fp) != NULL)
    {
        if (strstr (buf, "@logout@") || strstr (buf, "@login@"))
        {
            count++;
            continue;
        }
        if (count == number)
        {
            if (mode == YEA)
                showstuff (buf /*, 0 */ );
            else
            {
                prints ("%s", buf);
            }
        }
        else if (count > number)
            break;
        else
            continue;
    }
    refresh ();
    fclose (fp);
    return;
}


char *
cnumposts (num)
int num;
{

    if (num == 0)
        return GLY_CPOST0;
    if (num > 0 && num <= 500)
        return GLY_CPOST1;
    if (num > 500 && num <= 1500)
        return GLY_CPOST2;
    if (num > 1500 && num <= 4000)
        return GLY_CPOST3;
    if (num > 4000 && num <= 10000)
        return GLY_CPOST4;
    if (num > 10000)
        return GLY_CPOST5;

}

char   *
cexp(exp)
int     exp;
{
	if (exp == -9999)
		return GLY_CEXP0;
	if (exp <= 100 )
		return GLY_CEXP1;
	if (exp > 100  && exp <= 450 )
		return GLY_CEXP2;
	if (exp > 450 && exp <= 850 )
		return GLY_CEXP3;
	if (exp > 850 && exp <= 1500 )
		return GLY_CEXP4;
	if (exp > 1500 && exp <= 2500 )
		return GLY_CEXP5;
	if (exp > 2500 && exp <= 3000 )
		return GLY_CEXP6;
	if (exp > 3000 && exp <= 5000 )
		return GLY_CEXP7;
	if (exp > 5000 && exp <= 10000)
		return GLY_CEXP8;
	if (exp > 10000)
		return GLY_CEXP9;
}

#ifdef ALLOWGAME
char *
cmedals(num)
int num;
{
	
	if(num== 0 )
		return  GLY_MEDAL0;
	if(num<=300)
		return  GLY_MEDAL1;
	if(num>300&&num<=1000)
		return  GLY_MEDAL2;
	if(num>1000&&num<=3000)
		return  GLY_MEDAL3;
	if(num>3000)
		return  GLY_MEDAL4;
}

char *
cmoney(num)
int num;
{
	
	if(num<= 100 )
		return GLY_MONEY0;
	if(num>100&&num<=3000)
		return GLY_MONEY1;
	if(num>3000&&num<=10000)
		return GLY_MONEY2;
	if(num>10000&&num<=50000)
		return GLY_MONEY3;
	if(num>50000&&num<=150000)
		return GLY_MONEY4;
	if(num>150000&&num<=300000)
		return GLY_MONEY5;
	if(num>300000&&num<=500000)
		return GLY_MONEY6;
	if(num>500000)
		return GLY_MONEY7;
}
#endif

char   *
cperf(perf)
int     perf;
{
	
	if (perf == -9999)
		return GLY_CPERF0;
	if (perf <= 5)
		return GLY_CPERF1;
	if (perf > 5 && perf <= 12)
		return GLY_CPERF2;
	if (perf > 12 && perf <= 35)
		return GLY_CPERF3;
	if (perf > 35 && perf <= 50)
		return GLY_CPERF4;
	if (perf > 50 && perf <= 90)
		return GLY_CPERF5;
	if (perf > 90 && perf <= 140)
		return GLY_CPERF6;
	if (perf > 140 && perf <= 200)
		return GLY_CPERF7;
	if (perf > 200)
		return GLY_CPERF8;
}

int
countexp(udata)
struct userec *udata;
{
	int     exp;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	exp = udata->numposts +
		udata->numlogins / 3 +
		(time(0) - udata->firstlogin) / 86400 +
		udata->stay / 3600;
	return exp > 0 ? exp : 0;
}

int
countperf(udata)
struct userec *udata;
{
	int     perf;
	int     reg_days;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	reg_days = (time(0) - udata->firstlogin) / 86400 + 1;
	/*
	* 990530.edwardc 注册没成功或还在注册的人的人会导致 reg_days = 0,
	* 然後在下面会产生 SIGFPE, 除数为零的错误 ..
	*/
	if (reg_days <= 0)
		return 0;
	
	perf = ((float) udata->numposts / (float) udata->numlogins +
		(float) udata->numlogins / (float) reg_days) * 10;
	return perf > 0 ? perf : 0;
}



void
countdays (year, month, day, now)
int *year, *month, *day;
time_t now;
{
    struct tm *GoodTime;
    time_t tmptime;

    GoodTime = localtime (&now);
    GoodTime->tm_year = *year - 1900;
    GoodTime->tm_mon = *month - 1;
    GoodTime->tm_mday = *day;
    GoodTime->tm_hour = 0;
    GoodTime->tm_min = 0;
    tmptime = mktime (GoodTime);
    *year = (tmptime - now) / 86400;
    *month = (tmptime - now - *year * 86400) / 3600;
    *day = (tmptime - now - *year * 86400 - *month * 3600) / 60;
}

/*
void showstuff(buf, limit)
char    buf[256];
int     limit;
*/
void
showstuff (char *buf)
{
    extern time_t login_start_time;
    int frg, i, matchfrg, strlength, cnt;
    static char numlogins[10], numposts[10], nummails[10], rgtday[30],
    lasttime[30], lastjustify[30], thistime[30], stay[10],
    alltime[20], ccperf[20],perf[10], exp[10], ccexp[20], star[7];
    char buf2[STRLEN], *ptr, *ptr2;
    time_t now;
#ifdef ALLOWGAME
	static char moneys[10];
#endif
	
    static logout loglst[] = {
                                 "userid", currentuser.userid,
                                 "username", currentuser.username,
                                 "realname", currentuser.realname,
                                 "address", currentuser.address,
                                 "email", currentuser.email,
                                 "termtype", currentuser.termtype,
                                 "realemail", currentuser.reginfo,
                                 "ident", currentuser.ident,
                                 "rgtday", rgtday,
                                 "login", numlogins,
                                 "post", numposts,
                                 "mail", nummails,
                                 "lastlogin", lasttime,
                                 "lasthost", currentuser.lasthost,
                                 "lastjustify", lastjustify,
                                 "now", thistime,
                                 "bbsname", BoardName,
                                 "stay", stay,
                                 "alltime", alltime,
								 "exp", exp,
								 "cexp", ccexp,
								"perf", perf,
								"cperf", ccperf,
                                 "star", star,
                                 "pst", numposts,
                                 "log", numlogins,
#ifdef ALLOWGAME
								"money",moneys,
#endif
								
                                 "bbsip", BBSIP,
                                 "bbshost", BBSHOST,
                                 "version", BBSVERSION,
                                 NULL, NULL,
                             };

    if (!strchr (buf, '$'))
    {
        //if (!limit)
        prints ("%s", buf);
        //else
        //      prints("%.82s", buf);
        return;
    }
    now = time (0);
    /* for ansimore3() */

    if (currentuser.numlogins > 0)
    {
        sprintf (alltime, "%d小时%d分钟", currentuser.stay / 3600,
                 (currentuser.stay / 60) % 60);
        getdatestring (currentuser.firstlogin, NA);
        sprintf (rgtday, "%s", datestring);
        getdatestring (currentuser.lastlogin, NA);
        sprintf (lasttime, "%s", datestring);
        getdatestring (now, NA);
        sprintf (thistime, "%s", datestring);
        getdatestring (currentuser.lastjustify, NA);
        sprintf (lastjustify, "%24.24s", datestring);
        sprintf (stay, "%d", (time (0) - login_start_time) / 60);
        sprintf (numlogins, "%d", currentuser.numlogins);
        sprintf (numposts, "%d", currentuser.numposts);
#ifdef ALLOWGAME
		sprintf(moneys,"%d",currentuser.money);
#endif
        sprintf (nummails, "%d", currentuser.nummails);
        sprintf (star, "%s座",
                 horoscope (currentuser.birthmonth, currentuser.birthday));
    }
    frg = 1;
    ptr2 = buf;
    do
    {
        if (ptr = strchr (ptr2, '$'))
        {
            matchfrg = 0;
            *ptr = '\0';
            prints ("%s", ptr2);
            ptr += 1;
            for (i = 0; loglst[i].match != NULL; i++)
            {
                if (strstr (ptr, loglst[i].match) == ptr)
                {
                    strlength = strlen (loglst[i].match);
                    ptr2 = ptr + strlength;
                    for (cnt = 0; *(ptr2 + cnt) == ' '; cnt++)
                        ;
                    sprintf (buf2, "%-*.*s",
                             cnt ? strlength + cnt : strlength + 1,
                             strlength + cnt, loglst[i].replace);
                    prints ("%s", buf2);
                    ptr2 += (cnt ? (cnt - 1) : cnt);
                    matchfrg = 1;
                    break;
                }
            }
            if (!matchfrg)
            {
                prints ("$");
                ptr2 = ptr;
            }
        }
        else
        {
            //if (!limit)
            prints ("%s", ptr2);
            //else
            //      prints("%.82s", ptr2);
            frg = 0;
        }
    }
    while (frg);
    return;
}
