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

#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/telnet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <math.h>
#include <arpa/inet.h>
#include "bbs.h"

char host1[80][40], host2[80][40], ip[80][40];
int port[80], counts = 0;
char str[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";

char datafile[80] = BBSHOME "/etc/bbsnet.ini";
char userid[80] = "unknown.";

static int init_data(void)
{
    FILE *fp;
    char t[256], *t1, *t2, *t3, *t4;

    fp = fopen(datafile, "r");
    if (fp == NULL)
        return 0;
    while (fgets(t, 255, fp) && counts <= 72)
    {
        t1 = strtok(t, " \t");
        t2 = strtok(NULL, " \t\n");
        t3 = strtok(NULL, " \t\n");
        t4 = strtok(NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL || t3 == NULL)
            continue;
        strncpy(host1[counts], t1, 16);
        strncpy(host2[counts], t2, 36);
        strncpy(ip[counts], t3, 36);
        port[counts] = t4 ? atoi(t4) : 23;
        counts++;
    }
    fclose(fp);
    return 1;
}
static void locate(int);
int sh(int n)
{
    static int oldn = -1;

    if (n >= counts)
        return 0;
    if (oldn >= 0)
    {
        locate(oldn);
        printf("[1;32m %c.[m%s", str[oldn], host2[oldn]);
    }
    oldn = n;
    printf
        ("[22;3H[1;37mµ¥Î»: [1;33m%s                   [22;32H[1;37m Õ¾Ãû: [1;33m%s              \r\n",
         host1[n], host2[n]);
    printf("[1;37m[23;3HÁ¬Íù: [1;33m%s                   [21;1H",
           ip[n]);
    locate(n);
    printf("[%c][1;42m%s[m", str[n], host2[n]);
    return 0;
}

void show_all(void)
{
    int n;

    printf("[H[2J[m");
    printf
        ("©³©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥[1;35m ´©  Ëó  Òø  ºÓ [m©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©·\r\n");
    for (n = 1; n < 22; n++)
    {
        printf
            ("©§                                                                            ©§\r\n");
    }
    printf
        ("©§                                                               [1;36m°´[1;33mCtrl+C[1;36mÍË³ö[m ©§\r\n");
    printf
        ("©»©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¿");
    printf
        ("[21;3H©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤");
    for (n = 0; n < counts; n++)
    {
        locate(n);
        printf("[1;32m %c.[m%s", str[n], host2[n]);
    }
}

static void locate(int n)
{
    int x, y;
    char buf[20];

    if (n >= counts)
        return;
    y = n / 3 + 3;
    x = n % 3 * 25 + 4;

    sprintf(buf, "[%d;%dH", y, x);
    printf(buf);
}

static int getch(void)
{
    int c, d, e;
    static int lastc = 0;

    c = getchar();
    if (c == 10 && lastc == 13)
        c = getchar();
    lastc = c;
    if (c != 27)
        return c;
    d = getchar();
    e = getchar();
    if (d == 27)
        return 27;
    if (e == 'A')
        return 257;
    if (e == 'B')
        return 258;
    if (e == 'C')
        return 259;
    if (e == 'D')
        return 260;
    return 0;
}

static void syslog(char *s)
{
    char buf[512], timestr[16], *thetime;
    time_t dtime;
    FILE *fp;

    fp = fopen("reclog/bbsnet.log", "a");
    if (!fp)
        return;
    time(&dtime);
    thetime = (char *) ctime(&dtime);
    strncpy(timestr, &(thetime[4]), 15);
    timestr[15] = '\0';
    sprintf(buf, "%s %s %s\n", userid, timestr, s);
    fprintf(fp, buf);
    fclose(fp);
}

#define stty(fd, data) tcsetattr( fd, TCSANOW, data )
#define gtty(fd, data) tcgetattr( fd, data )
struct termios tty_state, tty_new;

static int get_tty(void)
{
    if (gtty(1, &tty_state) < 0)
        return 0;
    return 1;
}

static void init_tty(void)
{
    long vdisable;

    memcpy(&tty_new, &tty_state, sizeof(tty_new));
    tty_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ISIG);
    tty_new.c_cflag &= ~CSIZE;
    tty_new.c_cflag |= CS8;
    tty_new.c_cc[VMIN] = 1;
    tty_new.c_cc[VTIME] = 0;
    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) >= 0)
    {
        tty_new.c_cc[VSTART] = vdisable;
        tty_new.c_cc[VSTOP] = vdisable;
        tty_new.c_cc[VLNEXT] = vdisable;
    }
    tcsetattr(1, TCSANOW, &tty_new);
}

static void reset_tty(void)
{
    stty(1, &tty_state);
}

static void QuitTime(int num);
static void break_check(int num)
{
    int i, result;
    unsigned char buf[2048];
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    result = select(1, &readfds, NULL, NULL, &tv);
    if (FD_ISSET(0, &readfds))
    {
        result = read(0, buf, 80);
        for (i = 0; i < result; i++)
        {
            if (buf[i] == 3 || buf[i] == 4)
            {
                printf("\r\nBreak\r\n");
                QuitTime(0);
            }
        }
    }
    alarm(1);
}

static void telnetopt(int fd, unsigned char *buf, int max);
static void telnet(char *hostname, char *server, int port)
{
    int fd;
    struct sockaddr_in blah;
    struct hostent *he;
    fd_set readfds;
    struct timeval tv;
    int result;
    char buf[2048];

    signal(SIGALRM, break_check);
    alarm(1);
    bzero((char *) &blah, sizeof(blah));
    blah.sin_family = AF_INET;
    blah.sin_addr.s_addr = inet_addr(server);
    blah.sin_port = htons(port);
    printf("ÕýÁ¬½Ó %s£¬ÇëÉÔºò ... \r\n", server);
    fflush(stdout);
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((he = gethostbyname(server)) != NULL)
        bcopy(he->h_addr, (char *) &blah.sin_addr, he->h_length);
    else
    {
        blah.sin_addr.s_addr = inet_addr(server);
        if (blah.sin_addr.s_addr == INADDR_NONE)
            return;
    }
    if (connect(fd, (struct sockaddr *) &blah, 16) < 0)
        return;
    signal(SIGALRM, SIG_IGN);
    sprintf(buf, "%s (%s)", hostname, server);
    syslog(buf);
    printf("ÒÑÁ¬ÉÏÖ÷»ú£¬°´ \'Ctrl+]\' ¿ìËÙÍË³ö¡£\r\n\r\n");
    fflush(stdout);
    while (1)
    {
        tv.tv_sec = 2400;
        tv.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        FD_SET(0, &readfds);
        result = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (result <= 0)
            break;
        if (FD_ISSET(0, &readfds))
        {
            result = read(0, buf, 2048);
            if (result <= 0)
                break;
            if (result == 1 && (buf[0] == 10 || buf[0] == 13))
            {
                buf[0] = 13;
                buf[1] = 10;
                result = 2;
            }
            if (buf[0] == 29)
            {
                close(fd);
                return;
            }
            write(fd, buf, result);
        }
        else
        {
            result = read(fd, buf, 2048);
            if (result <= 0)
                break;
            if (strchr(buf, '\xff'))
                telnetopt(fd, (unsigned char *) buf, result);
            else
                write(0, buf, result);
        }
    }
}

static void telnetopt(int fd, unsigned char *buf, int max)
{
    unsigned char c2, c3;
    int i;
    unsigned char tmp[30];

    for (i = 0; i < max; i++)
    {
        if (buf[i] != 255)
        {
            write(0, &buf[i], 1);
            continue;
        }
        c2 = buf[i + 1];
        c3 = buf[i + 2];
        i += 2;
        if (c2 == 253 && (c3 == 3 || c3 == 24))
        {
            tmp[0] = 255;
            tmp[1] = 251;
            tmp[2] = c3;
            write(fd, tmp, 3);
            continue;
        }
        if ((c2 == 251 || c2 == 252) && (c3 == 1 || c3 == 3 || c3 == 24))
        {
            tmp[0] = 255;
            tmp[1] = 253;
            tmp[2] = c3;
            write(fd, tmp, 3);
            continue;
        }
        if (c2 == 251 || c2 == 252)
        {
            tmp[0] = 255;
            tmp[1] = 254;
            tmp[2] = c3;
            write(fd, tmp, 3);
            continue;
        }
        if (c2 == 253 || c2 == 254)
        {
            tmp[0] = 255;
            tmp[1] = 252;
            tmp[2] = c3;
            write(fd, tmp, 3);
            continue;
        }
        if (c2 == 250)
        {
            while (c3 != 240 && i < max)
            {
                c3 = buf[i];
                i++;
            }
            tmp[0] = 255;
            tmp[1] = 250;
            tmp[2] = 24;
            tmp[3] = 0;
            tmp[4] = 65;
            tmp[5] = 78;
            tmp[6] = 83;
            tmp[7] = 73;
            tmp[8] = 255;
            tmp[9] = 240;
            write(fd, tmp, 10);
        }
    }
    fflush(stdout);
}
static void QuitTime(int num)
{
    reset_tty();
    exit(0);
}

static void SetQuitTime(void)
{
    signal(SIGALRM, QuitTime);
    alarm(60);
}

static void bbsnet(int n);

void main_loop(void)
{
    int p = 0;
    int c, n;

  L:
    show_all();
    sh(p);
    fflush(stdout);
    while (1)
    {
        c = getch();
        alarm(60);
        if (c == 3 || c == 4 || c == 27 || c < 0)
            break;
        if (c == 257)
        {
            if (p < 3)
            {
                p += 3 * ((counts - 1) / 3 - (p > ((counts - 1) % 3)));
            }
            else
                p -= 3;
        }
        if (c == 258)
        {
            if (p + 3 > counts - 1)
                p %= 3;
            else
                p += 3;
        }
        if (c == 259)
        {
            p++;
            if (p > counts - 1)
                p = 0;
        }
        if (c == 260)
        {
            p--;
            if (p < 0)
                p = counts - 1;
        }
        if (c == 13 || c == 10)
        {
            alarm(0);
            bbsnet(p);
            SetQuitTime();
            goto L;
        }
        for (n = 0; n < counts; n++)
            if (str[n] == c)
                p = n;
        sh(p);
        fflush(stdout);
    }
}

static void bbsnet(int n)
{
    if (n >= counts)
        return;
    printf("[H[2J[1;32mo Á¬Íù: %s (%s)\r\n", host2[n], ip[n]);
    printf("%s\r\n\r\n[m", "o Á¬²»ÉÏÊ±Çë°´ \'Ctrl+C\' ÍË³ö");
    fflush(stdout);
    telnet(host2[n], ip[n], port[n]);
}

int main(int n, char *cmd[])
{
    SetQuitTime();
    get_tty();
    init_tty();
    if (n >= 2)
        strcpy(datafile, cmd[1]);
    if (n >= 3)
        strcpy(userid, cmd[2]);
    init_data();
    main_loop();
    printf("[m");
    reset_tty();
    return 0;
}
