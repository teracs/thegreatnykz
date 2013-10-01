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

#include "bbs.h"

extern char BoardName[];
typedef struct
{
    char *match;
    char *replace;
} logout;

int countlogouts(char *filename)
{
    FILE *fp;
    char buf[256];
    int count = 0;

    if ((fp = fopen(filename, "r")) == NULL)
        return 0;

    while (fgets(buf, 255, fp) != NULL)
    {
        if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
            count++;
    }
    return count + 1;
}

void user_display(char *filename, int number, int mode)
{
    FILE *fp;
    char buf[256];
    int count = 1;

    clear();
    move(1, 0);
    if ((fp = fopen(filename, "r")) == NULL)
        return;

    while (fgets(buf, 255, fp) != NULL)
    {
        if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
        {
            count++;
            continue;
        }
        if (count == number)
        {
            if (mode == YEA)
                showstuff(buf);
            else
            {
                prints("%s", buf);
            }
        }
        else if (count > number)
            break;
        else
            continue;
    }
    refresh();
    fclose(fp);
    return;
}

char *cnumposts(int num)
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
    return GLY_CPOST0;
}

void countdays(int *year, int *month, int *day, time_t now)
{
    struct tm *GoodTime;
    time_t tmptime;

    GoodTime = localtime(&now);
    GoodTime->tm_year = *year - 1900;
    GoodTime->tm_mon = *month - 1;
    GoodTime->tm_mday = *day;
    GoodTime->tm_hour = 0;
    GoodTime->tm_min = 0;
    tmptime = mktime(GoodTime);
    *year = (tmptime - now) / 86400;
    *month = (tmptime - now - *year * 86400) / 3600;
    *day = (tmptime - now - *year * 86400 - *month * 3600) / 60;
}

void showstuff(char *buf)
{
    extern time_t login_start_time;
    int frg, i, matchfrg, strlength, cnt;
    static char numlogins[10], numposts[10], nummails[10], rgtday[30],
        lasttime[30], lastjustify[30], thistime[30], stay[10],
        alltime[20], star[7];
    char buf2[STRLEN], *ptr, *ptr2;
    time_t now;

    static logout loglst[] = {
        {"userid", currentuser.userid,},
        {"username", currentuser.username,},
        {"realname", currentuser.realname,},
        {"address", currentuser.address,},
        {"email", currentuser.email,},
        {"termtype", currentuser.termtype,},
        {"realemail", currentuser.reginfo,},
        {"ident", currentuser.ident,},
        {"rgtday", rgtday,},
        {"login", numlogins,},
        {"post", numposts,},
        {"mail", nummails,},
        {"lastlogin", lasttime,},
        {"lasthost", currentuser.lasthost,},
        {"lastjustify", lastjustify,},
        {"now", thistime,},
        {"bbsname", BoardName,},
        {"stay", stay,},
        {"alltime", alltime,},
        {"star", star,},
        {"pst", numposts,},
        {"log", numlogins,},
        {"bbsip", BBSIP,},
        {"bbshost", BBSHOST,},
        {"version", BBSVERSION,},
        {NULL, NULL,},
    };

    if (!strchr(buf, '$'))
    {

        prints("%s", buf);

        return;
    }
    now = time(0);

    if (currentuser.numlogins > 0)
    {
        sprintf(alltime, "%ldÐ¡Ê±%ld·ÖÖÓ", currentuser.stay / 3600,
                (currentuser.stay / 60) % 60);
        getdatestring(currentuser.firstlogin, NA);
        sprintf(rgtday, "%s", datestring);
        getdatestring(currentuser.lastlogin, NA);
        sprintf(lasttime, "%s", datestring);
        getdatestring(now, NA);
        sprintf(thistime, "%s", datestring);
        getdatestring(currentuser.lastjustify, NA);
        sprintf(lastjustify, "%24.24s", datestring);
        sprintf(stay, "%ld", (time(0) - login_start_time) / 60);
        sprintf(numlogins, "%d", currentuser.numlogins);
        sprintf(numposts, "%d", currentuser.numposts);
        sprintf(nummails, "%d", currentuser.nummails);
        sprintf(star, "%s×ù",
                horoscope(currentuser.birthmonth, currentuser.birthday));
    }
    frg = 1;
    ptr2 = buf;
    do
    {
        if ((ptr = strchr(ptr2, '$')))
        {
            matchfrg = 0;
            *ptr = '\0';
            prints("%s", ptr2);
            ptr += 1;
            for (i = 0; loglst[i].match != NULL; i++)
            {
                if (strstr(ptr, loglst[i].match) == ptr)
                {
                    strlength = strlen(loglst[i].match);
                    ptr2 = ptr + strlength;
                    for (cnt = 0; *(ptr2 + cnt) == ' '; cnt++);
                    sprintf(buf2, "%-*.*s",
                            cnt ? strlength + cnt : strlength + 1,
                            strlength + cnt, loglst[i].replace);
                    prints("%s", buf2);
                    ptr2 += (cnt ? (cnt - 1) : cnt);
                    matchfrg = 1;
                    break;
                }
            }
            if (!matchfrg)
            {
                prints("$");
                ptr2 = ptr;
            }
        }
        else
        {

            prints("%s", ptr2);

            frg = 0;
        }
    }
    while (frg);
    return;
}
