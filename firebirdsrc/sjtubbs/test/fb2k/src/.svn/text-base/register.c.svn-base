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

char *sysconf_str(char *key);
char *genpasswd(char *pw);

extern char fromhost[60];
extern time_t login_start_time;

time_t system_time;

#ifdef ALLOWSWITCHCODE
extern int convcode;
#endif

int numberbuffer[10][10][8] = {

    {
     {0, 0, 0, 1, 1, 0, 0, 0,},
     {0, 0, 1, 0, 0, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 0, 0, 1, 0, 0,},
     {0, 0, 0, 1, 1, 0, 0, 0,},},
    {
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 1, 1, 0, 0, 0,},
     {0, 0, 1, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 1, 1, 1, 1, 1, 0,},},
    {
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 1, 1, 0, 0,},
     {0, 0, 0, 1, 0, 0, 0, 0,},
     {0, 0, 1, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 1, 1, 1, 1, 1, 0,},},
    {
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 1, 1, 1, 0, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},},
    {
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 1, 1, 0, 0,},
     {0, 0, 0, 1, 0, 1, 0, 0,},
     {0, 0, 1, 0, 0, 1, 0, 0,},
     {0, 1, 0, 0, 0, 1, 0, 0,},
     {0, 1, 0, 0, 0, 1, 0, 0,},
     {0, 1, 1, 1, 1, 1, 1, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},},
    {
     {0, 1, 1, 1, 1, 1, 1, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 1, 1, 1, 1, 0, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},},
    {
     {0, 0, 0, 1, 1, 1, 0, 0,},
     {0, 0, 1, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 1, 0, 0, 0, 0, 0, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},},
    {
     {0, 1, 1, 1, 1, 1, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},
     {0, 0, 0, 0, 1, 0, 0, 0,},},
    {
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},},
    {
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 1, 0, 0, 0, 0, 1, 0,},
     {0, 0, 1, 1, 1, 1, 0, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 0, 1, 0,},
     {0, 0, 0, 0, 0, 1, 0, 0,},
     {0, 0, 1, 1, 1, 0, 0, 0,},},

};

int danger_user_id(char *userid)
{
    char *ptr, ch;

    ptr = userid;
    while ((ch = *ptr++) != '\0')
    {
        if (!isalnum(ch) && ch != '_')
            return 1;
    }
    return 0;
}

int bad_user_id(char *userid)
{
    FILE *fp;
    char buf[STRLEN], ptr2[IDLEN + 2], *ptr, ch;

    ptr = userid;
    while ((ch = *ptr++) != '\0')
    {
        if (!isalnum(ch) && ch != '_')
            return 1;
    }
    if (!strcasecmp(userid, BBSID))
        return 1;
    if ((fp = fopen(".badname", "r")) != NULL)
    {
        strtolower(ptr2, userid);
        while (fgets(buf, STRLEN, fp) != NULL)
        {
            ptr = strtok(buf, " \n\t\r");
            if (ptr != NULL && *ptr != '#')
            {
                if ((ptr[0] == '*' && strstr(ptr2, &ptr[1]) != NULL)
                    || (ptr[0] != '*' && !strcmp(ptr2, ptr)))
                {
                    fclose(fp);
                    return 1;
                }
            }
        }
        fclose(fp);
    }
    return 0;
}

int compute_user_value(struct userec *urec)
{
    int value;

    if ((urec->userlevel & PERM_XEMPT)
        || strcmp(urec->userid, "SYSOP") == 0
        || strcmp(urec->userid, "guest") == 0)
        return 999;

    value = (time(0) - urec->lastlogin) / 60;

    if (strcmp(urec->userid, "new") == 0)
    {
        return (30 - value) * 60;
    }

    if (urec->userlevel & PERM_666LIFE)
    {
        return (667 * 1440 - value) / 1440;
    }
    if (urec->numlogins <= 3)
        return (15 * 1440 - value) / 1440;
    if (!(urec->userlevel & PERM_LOGINOK))
        return (30 * 1440 - value) / 1440;
    if (urec->stay > 1000000)
        return (365 * 1440 - value) / 1440;
    return (120 * 1440 - value) / 1440;
}

int getnewuserid(void)
{
    struct userec utmp;

#if 0
#ifndef SAVELIVE

    struct userec zerorec;
    int size, j, re;
    FILE *fdtmp;
    char nname[50], buf[50], buf2[50];
#endif
#endif

    int fd, i;

    system_time = time(0);

#if 0
#ifndef SAVELIVE

    if (stat("tmp/killuser", &st) == -1 || st.st_mtime < system_time)
    {

        if ((fd = open("tmp/killuser", O_RDWR | O_CREAT, 0600)) == -1)
            return -1;
        getdatestring(system_time, NA);
        write(fd, datestring, 29);
        close(fd);
        strcpy(nname, "tmp/bbs.killid");
        fdtmp = fopen(nname, "w+");
        log_usies("CLEAN", "dated users.");
        prints("Ñ°ÕÒĞÂÕÊºÅÖĞ, ÇëÉÔ´ıÆ¬¿Ì...\n");
        log_usies("TEST", "1.");
        memset(&zerorec, 0, sizeof(zerorec));
        if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
            return -1;
        flock(fd, LOCK_EX);
        size = sizeof(utmp);
        log_usies("TEST", "2.");
        for (i = 0; i < MAXUSERS; i++)
        {
            if (read(fd, &utmp, size) != size)
                break;
            val = compute_user_value(&utmp);
            if (utmp.userid[0] != '\0' && val < 0)
            {
                log_usies("TEST", "3.");
                if (lseek(fd, (off_t) (-size), SEEK_CUR) == -1)
                {
                    flock(fd, LOCK_UN);
                    close(fd);
                    return -1;
                }
                write(fd, &zerorec, sizeof(utmp));
                log_usies("TEST", "4.");
                if (strcmp(utmp.userid, "new") == 0)
                    continue;
                log_usies("TEST", "5.");
                re = 0;
                lseek(fd, 0, SEEK_SET);
                log_usies("TEST", "6.");
                for (j = 0; j < i; j++)
                {
                    if (read(fd, &utmp2, size) != size)
                        break;
                    if (strcmp(utmp.userid, utmp2.userid) == 0)
                    {
                        re = 1;
                        break;
                    }
                }
                lseek(fd, (off_t) (i + 1) * size, SEEK_SET);
                if (re)
                    continue;
                getdatestring(utmp.lastlogin, NA);
                sprintf(genbuf, "#%d %-12s %14.14s %d %d %d", i + 1,
                        utmp.userid, datestring, utmp.numlogins,
                        utmp.numposts, val);
                log_usies("KILL ", genbuf);
                if (!danger_user_id(utmp.userid))
                {
                    sprintf(genbuf, "mail/%c/%c/%s",
                            toupper(utmp.userid[0]),
                            toupper(utmp.userid[1]), utmp.userid);
                    if (!strncmp
                        (genbuf + 7, utmp.userid, strlen(utmp.userid)))
                        f_rm(genbuf);
                    sprintf(genbuf, "home/%c/%c/%s",
                            toupper(utmp.userid[0]),
                            toupper(utmp.userid[1]), utmp.userid);
                    if (!strncmp
                        (genbuf + 7, utmp.userid, strlen(utmp.userid)))
                        f_rm(genbuf);

                    sprintf(buf, "[^[[1;3%dm%s×ù^[[m] ",
                            (utmp.gender == 'F') ? 5 : 6,
                            horoscope(utmp.birthmonth, utmp.birthday));
                    sprintf(buf2, "ÍøÁä[^[[1;32m%d^[[0;1m]Ìì",
                            (time(0) - utmp.firstlogin) / 86400);
                    fprintf(fdtmp,
                            "[1;37m%s [m([1;33m%s[m) ¹²ÉÏÕ¾ [1;32m%d[m ´Î  %s  %s\n",
                            utmp.userid, utmp.username, utmp.numlogins,
                            buf2, buf);
                    getdatestring(utmp.lastlogin, NA);
                    fprintf(fdtmp,
                            "ÉÏ ´Î ÔÚ: [[1;32m%s[m] ´Ó [[1;32m%s[m] µ½±¾Õ¾Ò»ÓÎ¡£\n",
                            datestring, utmp.lasthost);
                    getdatestring(utmp.lastlogout, NA);
                    fprintf(fdtmp,
                            "ÀëÕ¾Ê±¼ä: [[1;32m%s[m] ÉúÃüÁ¦:[[1;32m%d[m] ÎÄÕÂ:[[1;32m%d[m] \n\n",
                            datestring, val, utmp.numposts);
                }
            }
        }
        flock(fd, LOCK_UN);
        close(fd);
        touchnew();
        fclose(fdtmp);
        log_usies("TEST", "12.");
        Postfile(nname, "message", "±¾ÈÕËæ·çÆ®ÊÅµÄID", 1);
        log_usies("TEST", "13.");
    }
#endif
#endif

    if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
        return -1;
    flock(fd, LOCK_EX);
    i = searchnewuser();
    sprintf(genbuf, "uid %d from %s", i, fromhost);
    log_usies("APPLY", genbuf);
    if (i <= 0 || i > MAXUSERS)
    {
        flock(fd, LOCK_UN);
        close(fd);
        prints("±§Ç¸, Ê¹ÓÃÕßÕÊºÅÒÑ¾­ÂúÁË, ÎŞ·¨×¢²áĞÂµÄÕÊºÅ.\n\r");
        prints("ÇëÉÔºòÔÙÊÔ, ×£ÄãºÃÔË.\n\r");
        sleep(2);
        exit(1);
    }
    memset(&utmp, 0, sizeof(utmp));
    strcpy(utmp.userid, "new");
    utmp.lastlogin = time(0);
    if (lseek(fd, (off_t) (sizeof(utmp) * (i - 1)), SEEK_SET) == -1)
    {
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
    }
    write(fd, &utmp, sizeof(utmp));
    setuserid(i, utmp.userid);
    flock(fd, LOCK_UN);
    close(fd);
    return i;
}

int id_with_num(char *userid)
{
    char *s;

    for (s = userid; *s != '\0'; s++)
        if (*s < 1 || !isalpha(*s))
            return 1;
    return 0;
}

void generatenumber(char inout[][40], int number)
{
    int i, j, k;
    int temp = number;
    int single[5] = { 0 };
    unsigned int rand[20] = { 0 };
    int seed = 0;
    int sub = 0;
    int shear[5] = { 0 };
    int shearindex = 0;

    for (i = 0; i < 5; i++)
    {
        single[4 - i] = temp % 10;
        temp = (temp - single[4 - i]) / 10;
        printf("%d\n", single[4 - i]);
    }
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 8; k++)
            {
                if (numberbuffer[single[j]][i][k] == 1)
                {
                    inout[i][j * 8 + k] = '*';
                }
                else
                    inout[i][j * 8 + k] = ' ';
            }
        }
    }
    get_random(rand, 20);
    memset(shear, 0x00, 5);
    for (i = 0; i < 12; i++)
    {
        sub = rand[i] % 40;
        seed = ((rand[i] - sub) / 40) % 10;
        shearindex = (sub - sub % 8) / 8;
        if (inout[seed][sub] == ' ')
            inout[seed][sub] = '*';
        else if (inout[seed][sub] == '*')
        {
            if (shear[shearindex] < 1)
            {
                inout[seed][sub] = ' ';
                shear[shearindex] += 1;
            }
        }
    }

    return;
}

void new_register(void)
{
    struct userec newuser;
    char passbuf[STRLEN];
    int allocid, try;
    unsigned int verify = 0;
    char inout[10][40];
    int i, j;
    char numbuf[7] = { 0 };
    unsigned int mynum = 0;
    int gap = 0;
    unsigned int rand[2] = { 0, 0 };

    if (dashf("NOREGISTER"))
    {
        ansimore("NOREGISTER", NA);
        pressreturn();
        exit(1);
    }
    ansimore("etc/register", NA);
    try = 0;

    get_random(rand, 2);
    verify = rand[0] % 90000 + 10000;
    generatenumber(inout, verify);
    gap = rand[1] % 4 + 1;
    printf("\n");
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 40; j++)
        {
            if ((j % 8 == 0) && (j / 8 == gap))
            {
                prints(" ");
            }
            prints("%c", inout[i][j]);
        }
        prints("\n");
    }
    prints("\n");
    getdata(0, 0, "ÇëÊäÈëÉÏÃæµÄÊı×Ö (Please input the number above): ",
            numbuf, 6, DOECHO, YEA);
    mynum = (unsigned int) atoi(numbuf);
    if (verify != mynum)
    {
        prints("\nSorry, you input the wrong number\n");
        pressreturn();
        longjmp(byebye, -1);
    }

    while (1)
    {
        if (++try >= 9)
        {
            prints("\n°İ°İ£¬°´Ì«¶àÏÂ  <Enter> ÁË...\n");
            refresh();
            longjmp(byebye, -1);
        }
        getdata(0, 0, "ÇëÊäÈëÕÊºÅÃû³Æ (Enter User ID, \"0\" to abort): ",
                passbuf, IDLEN + 1, DOECHO, YEA);
        if (passbuf[0] == '0')
        {
            longjmp(byebye, -1);
        }
        if (id_with_num(passbuf))
        {
            prints("ÕÊºÅ±ØĞëÈ«ÎªÓ¢ÎÄ×ÖÄ¸!\n");
        }
        else if (strlen(passbuf) < 2)
        {
            prints("ÕÊºÅÖÁÉÙĞèÓĞÁ½¸öÓ¢ÎÄ×ÖÄ¸!\n");
        }
        else if ((*passbuf == '\0') || bad_user_id(passbuf))
        {
            prints("±§Ç¸, Äú²»ÄÜÊ¹ÓÃÕâ¸ö×Ö×÷ÎªÕÊºÅ¡£ ÇëÏë¹ıÁíÍâÒ»¸ö¡£\n");
        }
        else if (dosearchuser(passbuf))
        {
            prints("´ËÕÊºÅÒÑ¾­ÓĞÈËÊ¹ÓÃ\n");
        }
        else
        {

            if (!danger_user_id(passbuf))
            {
                sprintf(genbuf, "mail/%c/%c/%s", toupper(passbuf[0]),
                        toupper(passbuf[1]), passbuf);
                f_rm(genbuf);
                sprintf(genbuf, "home/%c/%c/%s", toupper(passbuf[0]),
                        toupper(passbuf[1]), passbuf);
                f_rm(genbuf);
            }

            break;
        }
    }

    memset(&newuser, 0, sizeof(newuser));
    allocid = getnewuserid();
    if (allocid > MAXUSERS || allocid <= 0)
    {
        prints("No space for new users on the system!\n\r");
        exit(1);
    }
    strcpy(newuser.userid, passbuf);
    strcpy(passbuf, "");
    while (1)
    {
        getdata(0, 0, "ÇëÉè¶¨ÄúµÄÃÜÂë (Setup Password): ",
                passbuf, 9, NOECHO, YEA);
        if (strlen(passbuf) < 4 || !strcmp(passbuf, newuser.userid))
        {
            prints("ÃÜÂëÌ«¶Ì»òÓëÊ¹ÓÃÕß´úºÅÏàÍ¬, ÇëÖØĞÂÊäÈë\n");
            continue;
        }
        strncpy(newuser.passwd, passbuf, PASSLEN);
        getdata(0, 0, "ÇëÔÙÊäÈëÒ»´ÎÄãµÄÃÜÂë (Reconfirm Password): ",
                passbuf, 9, NOECHO, YEA);
        if (strncmp(passbuf, newuser.passwd, PASSLEN) != 0)
        {
            prints("ÃÜÂëÊäÈë´íÎó, ÇëÖØĞÂÊäÈëÃÜÂë.\n");
            continue;
        }
        passbuf[8] = '\0';
#ifdef ENCPASSLEN

        strncpy(newuser.passwd, genpasswd(passbuf), ENCPASSLEN);
#else

        strncpy(newuser.passwd, genpasswd(passbuf), PASSLEN);
#endif

        break;
    }

    strcpy(newuser.termtype, "vt100");
    newuser.gender = 'X';
    newuser.userdefine = -1;
    if (!strcmp(newuser.userid, "guest"))
    {
        newuser.userlevel = 0;
        newuser.userdefine &=
            ~(DEF_FRIENDCALL | DEF_ALLMSG | DEF_FRIENDMSG);
    }
    else
    {
        newuser.userlevel = PERM_BASIC;
        newuser.flags[0] = PAGER_FLAG;
    }

    newuser.userdefine &= ~(DEF_NOLOGINSEND);
#ifdef ALLOWSWITCHCODE

    if (convcode)
        newuser.userdefine &= ~DEF_USEGB;
#endif

    newuser.flags[1] = 0;
    newuser.firstlogin = newuser.lastlogin = time(0);
    if (substitute_record(PASSFILE, &newuser, sizeof(newuser), allocid) ==
        -1)
    {
        prints("too much, good bye!\n");
        oflush();
        sleep(2);
        exit(1);
    }
    setuserid(allocid, newuser.userid);
    if (!dosearchuser(newuser.userid))
    {
        prints("User failed to create\n");
        oflush();
        sleep(2);
        exit(1);
    }
    report("new account");
}

int invalid_email(char *addr)
{
    FILE *fp;
    char temp[STRLEN], tmp2[STRLEN];

    if (strlen(addr) < 3)
        return 1;

    strtolower(tmp2, addr);
    if (strstr(tmp2, "bbs") != NULL)
        return 1;

    if ((fp = fopen(".bad_email", "r")) != NULL)
    {
        while (fgets(temp, STRLEN, fp) != NULL)
        {
            strtok(temp, "\n");
            strtolower(genbuf, temp);
            if (strstr(tmp2, genbuf) != NULL
                || strstr(genbuf, tmp2) != NULL)
            {
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
    }
    return 0;
}

int check_register_ok(void)
{
    FILE *fn;
    char fname[STRLEN];

    sethomefile(fname, currentuser.userid, "register");
    if ((fn = fopen(fname, "r")) != NULL)
    {
        fgets(genbuf, STRLEN, fn);
        fclose(fn);
        strtok(genbuf, "\n");
        if (valid_ident(genbuf) && ((strchr(genbuf, '@') != NULL)
                                    || strstr(genbuf, "usernum")))
        {
            move(21, 0);
            prints("¹§ºØÄú!! ÄúÒÑË³ÀûÍê³É±¾Õ¾µÄÊ¹ÓÃÕß×¢²áÊÖĞø,\n");
            prints("´ÓÏÖÔÚÆğÄú½«ÓµÓĞÒ»°ãÊ¹ÓÃÕßµÄÈ¨ÀûÓëÒåÎñ...\n");
            pressanykey();
            return 1;
        }
    }
    return 0;
}

#ifdef MAILCHECK
#ifdef CODE_VALID
char *genrandpwd(int seed)
{
    char panel[] =
        "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *result;
    int i, rnd;

    result = (char *) malloc(RNDPASSLEN + 1);
    bzero(result, RNDPASSLEN + 1);
    for (i = 0; i < RNDPASSLEN; i++)
    {
        rnd = rand() % sizeof(panel);
        if (panel[rnd] == '\0')
        {
            i--;
            continue;
        }
        result[i] = panel[rnd];
    }
    sethomefile(genbuf, currentuser.userid, ".regpass");
    unlink(genbuf);
    file_append(genbuf, result);
    return ((char *) result);
}
#endif

void send_regmail(struct userec *trec)
{
    time_t code;
    FILE *fin, *fout, *dp;

#ifdef CODE_VALID

    char buf[RNDPASSLEN + 1];
#endif

    sethomefile(genbuf, trec->userid, "mailcheck");
    if ((dp = fopen(genbuf, "w")) == NULL)
        return;
    code = time(0);
    fprintf(dp, "%9.9d:%d\n", code, getpid());
    fclose(dp);

    sprintf(genbuf, "%s -f %s.bbs@%s %s ",
            SENDMAIL, trec->userid, BBSHOST, trec->email);
    fout = popen(genbuf, "w");
    fin = fopen("etc/mailcheck", "r");
    if ((fin != NULL) && (fout != NULL))
    {
        fprintf(fout, "Reply-To: SYSOP.bbs@%s\n", BBSHOST);
        fprintf(fout, "From: SYSOP.bbs@%s\n", BBSHOST);
        fprintf(fout, "To: %s\n", trec->email);
        fprintf(fout, "Subject: @%s@[-%9.9d:%d-]%s mail check.\n",
                trec->userid, code, getpid(), BBSID);
        fprintf(fout, "X-Purpose: %s registration mail.\n", BBSNAME);
        fprintf(fout, "\n");
        fprintf(fout, "[ÖĞÎÄ]\n");
        fprintf(fout, "BBS Î»Ö·         : %s (%s)\n", BBSHOST, BBSIP);
        fprintf(fout, "Äú×¢²áµÄ BBS ID  : %s\n", trec->userid);
        fprintf(fout, "ÉêÇëÈÕÆÚ         : %s", ctime(&trec->firstlogin));
        fprintf(fout, "µÇÈëÀ´Ô´         : %s\n", fromhost);
        fprintf(fout, "ÄúµÄÕæÊµĞÕÃû/êÇ³Æ: %s (%s)\n", trec->realname,
                trec->username);
#ifdef CODE_VALID

        sprintf(buf, "%s", (char *) genrandpwd((int) getpid()));
        fprintf(fout, "ÈÏÖ¤°µÂë         : %s (Çë×¢Òâ´óĞ¡Ğ´)\n", buf);
#endif

        fprintf(fout, "ÈÏÖ¤ĞÅ·¢³öÈÕÆÚ   : %s\n", ctime(&code));

        fprintf(fout, "[English]\n");
        fprintf(fout, "BBS LOCATION     : %s (%s)\n", BBSHOST, BBSIP);
        fprintf(fout, "YOUR BBS USER ID : %s\n", trec->userid);
        fprintf(fout, "APPLICATION DATE : %s", ctime(&trec->firstlogin));
        fprintf(fout, "LOGIN HOST       : %s\n", fromhost);
        fprintf(fout, "YOUR NICK NAME   : %s\n", trec->username);
        fprintf(fout, "YOUR NAME        : %s\n", trec->realname);
#ifdef CODE_VALID

        fprintf(fout, "VALID CODE       : %s (case sensitive)\n", buf);
#endif

        fprintf(fout, "THIS MAIL SENT ON: %s\n", ctime(&code));

        while (fgets(genbuf, 255, fin) != NULL)
        {
            if (genbuf[0] == '.' && genbuf[1] == '\n')
                fputs(". \n", fout);
            else
                fputs(genbuf, fout);
        }
        fprintf(fout, ".\n");
        fclose(fin);
        fclose(fout);
    }
}
#endif

void check_register_info(void)
{
    struct userec *urec = &currentuser;
    FILE *fout;
    char buf[192], buf2[STRLEN];
    char qry_mail_dir[STRLEN];

#ifdef MAILCHECK

    char ans[4];

#ifdef CODE_VALID

    int i;
#endif
#endif

    if (!(urec->userlevel & PERM_BASIC))
    {
        urec->userlevel = 0;
        return;
    }
#ifdef NEWCOMERREPORT
    if (urec->numlogins == 1 && abs(time(0) - urec->firstlogin) < 86400)
    {
        clear();
        sprintf(buf, "tmp/newcomer.%s", currentuser.userid);
        if ((fout = fopen(buf, "w")) != NULL)
        {
            fprintf(fout, "´ó¼ÒºÃ,\n\n");
            fprintf(fout, "ÎÒÊÇ %s (%s), À´×Ô %s\n",
                    currentuser.userid, urec->username, fromhost);
            fprintf(fout, "½ñÌì%s³õÀ´´ËÕ¾±¨µ½, Çë´ó¼Ò¶à¶àÖ¸½Ì¡£\n",
                    (urec->gender == 'M') ? "Ğ¡µÜ" : "Ğ¡Å®×Ó");
            move(2, 0);
            prints
                ("·Ç³£»¶Ó­ %s ¹âÁÙ±¾Õ¾£¬Ï£ÍûÄúÄÜÔÚ±¾Õ¾ÕÒµ½ÊôÓÚ×Ô¼ºµÄÒ»Æ¬Ìì¿Õ£¡\n\n",
                 currentuser.userid);
            prints("ÇëÄú×÷¸ö¼ò¶ÌµÄ¸öÈË¼ò½é, Ïò±¾Õ¾ÆäËûÊ¹ÓÃÕß´ò¸öÕĞºô\n");
            prints("(¼ò½é×î¶àÈıĞĞ, Ğ´Íê¿ÉÖ±½Ó°´ <Enter> ÌøÀë)....");
            getdata(6, 0, ":", buf2, 75, DOECHO, YEA);
            if (buf2[0] != '\0')
            {
                fprintf(fout, "\n\n×ÔÎÒ½éÉÜ:\n\n");
                fprintf(fout, "%s\n", buf2);
                getdata(7, 0, ":", buf2, 75, DOECHO, YEA);
                if (buf2[0] != '\0')
                {
                    fprintf(fout, "%s\n", buf2);
                    getdata(8, 0, ":", buf2, 75, DOECHO, YEA);
                    if (buf2[0] != '\0')
                    {
                        fprintf(fout, "%s\n", buf2);
                    }
                }
            }
            fclose(fout);
            sprintf(buf2, "ĞÂÊÖÉÏÂ·: %s", urec->username);
            Postfile(buf, "message", buf2, 2);
            unlink(buf);
        }
    }
#endif
#ifdef PASSAFTERTHREEDAYS
    if (urec->lastlogin - urec->firstlogin < 3 * 86400)
    {
        if (!HAS_PERM(PERM_SYSOP))
        {
            set_safe_record();
            urec->userlevel &= ~(PERM_DEFAULT);
            urec->userlevel |= PERM_BASIC;
            substitute_record(PASSFILE, urec, sizeof(struct userec),
                              usernum);
            ansimore("etc/newregister", YEA);
            return;
        }
    }
#endif

    clear();
    if (HAS_PERM(PERM_LOGINOK))
        return;
#ifndef AUTOGETPERM

    if (check_register_ok())
    {
#endif
        set_safe_record();
        urec->lastjustify = time(0);
        urec->userlevel |= PERM_DEFAULT;
        substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
        return;
#ifndef AUTOGETPERM

    }
#endif

#ifdef MAILCHECK
#ifdef CODE_VALID
    sethomefile(buf, currentuser.userid, ".regpass");
    if (dashf(buf))
    {
        move(13, 0);
        prints("ÄúÉĞÎ´Í¨¹ıÉí·İÈ·ÈÏ... \n");
        prints("ÄúÏÖÔÚ±ØĞëÊäÈë×¢²áÈ·ÈÏĞÅÀï, \"ÈÏÖ¤°µÂë\"À´×öÎªÉí·İÈ·ÈÏ\n");
        prints("Ò»¹²ÊÇ %d ¸ö×Ö·û, ´óĞ¡Ğ´ÊÇÓĞ²î±ğµÄ, Çë×¢Òâ.\n",
               RNDPASSLEN);
        prints("ÈôÏëÈ¡Ïû¿ÉÒÔÁ¬°´ÈıÏÂ [Enter] ¼ü.\n");
        prints
            ("[1;33mÇë×¢Òâ, ÇëÊäÈë×îĞÂÒ»·âÈÏÖ¤ĞÅÖĞËù°üº¬µÄÂÒÊıÃÜÂë£¡[m\n");
        if ((fout = fopen(buf, "r")) != NULL)
        {
            fscanf(fout, "%s", buf2);
            fclose(fout);
            for (i = 0; i < 3; i++)
            {
                move(18, 0);
                prints("Äã»¹ÓĞ %d ´Î»ú»á\n", 3 - i);
                getdata(19, 0, "ÇëÊäÈëÈÏÖ¤°µÂë: ", genbuf,
                        (RNDPASSLEN + 1), DOECHO, YEA);
                if (strcmp(genbuf, "") != 0)
                {
                    if (strcmp(genbuf, buf2) != 0)
                        continue;
                    else
                        break;
                }
            }
        }
        else
            i = 3;
        if (i == 3)
        {
            prints
                ("°µÂëÈÏÖ¤Ê§°Ü! ÄúĞèÒªÌîĞ´×¢²áµ¥»ò½ÓÊÕÈ·ÈÏĞÅÒÔÈ·¶¨ÄúµÄÉí·İ\n");
            getdata(22, 0, "ÇëÑ¡Ôñ£º1.Ìî×¢²áµ¥ 2.½ÓÊÕÈ·ÈÏĞÅ [1]:", ans, 2,
                    DOECHO, YEA);
            if (ans[0] == '2')
            {
                send_regmail(&currentuser);
                pressanykey();
            }
            else
                x_fillform();
        }
        else
        {
            set_safe_record();
            urec->userlevel |= PERM_DEFAULT;
            urec->lastjustify = time(0);
            strncpy(urec->reginfo, urec->email, 62);
            substitute_record(PASSFILE, urec, sizeof(struct userec),
                              usernum);
            prints("¹§ºØÄú!! ÄúÒÑË³ÀûÍê³É±¾Õ¾µÄÊ¹ÓÃÕß×¢²áÊÖĞø,\n");
            prints("´ÓÏÖÔÚÆğÄú½«ÓµÓĞÒ»°ãÊ¹ÓÃÕßµÄÈ¨ÀûÓëÒåÎñ...\n");
            unlink(buf);
            mail_file("etc/smail", "SYSOP", "»¶Ó­¼ÓÈë±¾Õ¾ĞĞÁĞ");
            pressanykey();
        }
        return;
    }
#endif
    if ((!strstr(urec->email, BBSHOST)) && (!invalidaddr(urec->email)) &&
        (!invalid_email(urec->email)))
    {
        move(13, 0);
        prints("ÄúµÄµç×ÓĞÅÏä ÉĞĞëÍ¨¹ı»ØĞÅÑéÖ¤...  \n");
        prints("    ±¾Õ¾½«ÂíÉÏ¼ÄÒ»·âÑéÖ¤ĞÅ¸øÄú,\n");
        prints("    ÄúÖ»Òª´Ó %s »ØĞÅ, ¾Í¿ÉÒÔ³ÉÎª±¾Õ¾ºÏ¸ñ¹«Ãñ.\n\n",
               urec->email);
        prints("    ³ÉÎª±¾Õ¾ºÏ¸ñ¹«Ãñ, ¾ÍÄÜÏíÓĞ¸ü¶àµÄÈ¨Òæà¸!\n");
        prints("    ÄúÒ²¿ÉÒÔÖ±½ÓÌîĞ´×¢²áµ¥£¬È»ºóµÈ´ıÕ¾³¤µÄÊÖ¹¤ÈÏÖ¤¡£\n");
        getdata(21, 0, "ÇëÑ¡Ôñ£º1.Ìî×¢²áµ¥ 2.·¢È·ÈÏĞÅ [1]: ", ans, 2,
                DOECHO, YEA);
        if (ans[0] == '2')
        {
            send_regmail(&currentuser);
            getdata(21, 0, "È·ÈÏĞÅÒÑ¼Ä³ö, µÈÄú»ØĞÅÅ¶!! ", ans, 2, DOECHO,
                    YEA);
            return;
        }
    }
#endif
    sprintf(qry_mail_dir, "mail/%c/%c/%s/%s",
            toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
            currentuser.userid, DOT_DIR);
    if (!check_query_mail(qry_mail_dir))
    {
        x_fillform();
    }
}
