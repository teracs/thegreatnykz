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
#ifdef AIX
#include <sys/select.h>
#endif
#include <arpa/telnet.h>

#define OBUFSIZE  (4096)
#define IBUFSIZE  (4096)

#define INPUT_ACTIVE 0
#define INPUT_IDLE 1

extern int dumb_term;

static char outbuf[OBUFSIZE];
static int obufsize = 0;
struct user_info uinfo;

char inbuf[IBUFSIZE];
int ibufsize = 0;
int icurrchar = 0;
int KEY_ESC_arg;

static int i_mode = INPUT_ACTIVE;
extern struct screenline *big_picture;

#ifdef ALLOWSWITCHCODE

#include "g2b_table.h"

extern int convcode;
extern void redoscr(void);

void auto_insert(char *buffer, int each_length, char char_insert,
                 int maxline);

void switch_code(void)
{
    convcode = !convcode;
    redoscr();
}

int write2(int port, char *str, int len)
{
    register int i, locate;
    register unsigned char ch1, ch2, *ptr;

    for (i = 0, ptr = (unsigned char *) str; i < len; i++)
    {
        ch1 = (ptr + i)[0];
        if (ch1 < 0xA1 || (ch1 > 0xA9 && ch1 < 0xB0) || ch1 > 0xF7)
            continue;
        ch2 = (ptr + i)[1];
        i++;
        if (ch2 < 0xA0 || ch2 == 0xFF)
            continue;
        if ((ch1 > 0xA0) && (ch1 < 0xAA))
            locate = ((ch1 - 0xA1) * 94 + (ch2 - 0xA1)) * 2;
        else
            locate = ((ch1 - 0xB0 + 9) * 94 + (ch2 - 0xA1)) * 2;
        (ptr + i - 1)[0] = GtoB[locate++];
        (ptr + i - 1)[1] = GtoB[locate];
    }
    return write(port, str, len);
}

int read2(int port, char *str, int len)
{

    register unsigned char ch1, ch2, *ptr;
    register int locate, i = 0;

    if (len == 0)
        return 0;
    len = read(port, str, len);
    if (len < 1)
        return len;

    for (i = 0, ptr = (unsigned char *) str; i < len; i++)
    {
        ch1 = (ptr + i)[0];
        if (ch1 < 0xA1 || ch1 == 0xFF)
            continue;
        ch2 = (ptr + i)[1];
        i++;
        if (ch2 < 0x40 || (ch2 > 0x7E && ch2 < 0xA1) || ch2 == 255)
            continue;
        if ((ch2 >= 0x40) && (ch2 <= 0x7E))
            locate = ((ch1 - 0xA1) * 157 + (ch2 - 0x40)) * 2;
        else
            locate = ((ch1 - 0xA1) * 157 + (ch2 - 0xA1) + 63) * 2;
        (ptr + i - 1)[0] = BtoG[locate++];
        (ptr + i - 1)[1] = BtoG[locate];
    }
    return len;
}
#endif

void hit_alarm_clock(int num)
{
    if (HAS_PERM(PERM_NOTIMEOUT))
        return;

    if (i_mode == INPUT_IDLE || uinfo.mode == LOGIN)
    {
        clear();
        prints("Idle timeout exceeded! Booting...\n");
        kill(getpid(), SIGHUP);
    }
    i_mode = INPUT_IDLE;
    if (uinfo.mode == LOGIN)
        alarm(LOGIN_TIMEOUT);
    else
        alarm(IDLE_TIMEOUT);
}

void init_alarm(void)
{
    signal(SIGALRM, hit_alarm_clock);
    alarm(IDLE_TIMEOUT);
}

#ifdef ALLOWSWITCHCODE

#define owrite(fd, buf, len) ( (!convcode)? write(fd,buf,len):write2(fd,buf,len))
#define iread(fd, buf, len) ( (!convcode)? read(fd,buf,len):read2(fd,buf,len))

#else

#define owrite(fd,buf,len) write(fd,buf,len)
#define iread(fd,buf,len) read(fd,buf,len)

#endif

static void speed_limit(int written_bytes);
inline static void speed_limit_read(void);

#ifdef BBSD

void oflush(void)
{
    if (obufsize)
    {
        speed_limit(owrite(1, outbuf, obufsize));
        obufsize = 0;
    }
}

void output(char *s, int len)
{

    if (obufsize + len > OBUFSIZE)
        oflush();
    if (len > OBUFSIZE)
    {
        speed_limit(owrite(1, s, len));
        return;
    }
    memcpy(outbuf + obufsize, s, len);
    obufsize += len;
}

int ochar(int c)
{
    register char *data;
    register int size;

    if (obufsize > OBUFSIZE - 2)
    {
        oflush();
    }

    data = outbuf;
    size = obufsize;

    data[size++] = c;
    if (c == IAC)
        data[size++] = c;
    obufsize = size;
    return c;
}
#else
void oflush(void)
{
    if (obufsize > 0)
    {
        speed_limit(owrite(1, outbuf, obufsize));
        obufsize = 0;
    }
}

void output(char *s, int len)
{

    if (len >= OBUFSIZE)
    {

        speed_limit(owrite(1, s, len));

    }
    else
    {

        if (obufsize + len >= OBUFSIZE)
        {
            oflush();

        }

        memcpy(outbuf + obufsize, s, len);
        obufsize += len;

    }
}
int ochar(int c)
{
    if (obufsize >= OBUFSIZE)
    {
        oflush();
    }
    outbuf[obufsize++] = c;
    return c;
}
#endif

int i_newfd = 0;
struct timeval i_to, *i_top = NULL;
int (*flushf) (void) = NULL;

void add_io(int fd, int timeout)
{
    i_newfd = fd;
    if (timeout)
    {
        i_to.tv_sec = timeout;
        i_to.tv_usec = 0;
        i_top = &i_to;
    }
    else
        i_top = NULL;
}

void add_flush(int (*flushfunc) (void))
{
    flushf = flushfunc;
}

int num_in_buf(void)
{
    int n;

    if ((n = icurrchar - ibufsize) < 0)
        n = 0;
    return n;
}

#ifdef BBSD
static int iac_count(char *current)
{
    switch (*(current + 1) & 0xff)
    {
    case DO:
    case DONT:
    case WILL:
    case WONT:
        return 3;
    case SB:
        {
            register char *look = current + 2;

            for (;;)
            {
                if ((*look++ & 0xff) == IAC)
                {
                    if ((*look++ & 0xff) == SE)
                    {
                        return look - current;
                    }
                }
            }
        }
    default:
        return 1;
    }
}
#endif

#ifdef BBSD
int igetch(void)
{
    static int trailing = 0;
    register int ch;
    register char *data;

    data = inbuf;

    speed_limit_read();
    for (;;)
    {
        if (ibufsize == icurrchar)
        {
            fd_set readfds;
            struct timeval to;
            register fd_set *rx;
            register int fd, nfds;

            rx = &readfds;
            fd = i_newfd;

          igetnext:

            uinfo.idle_time = time(0);
            update_utmp();

            FD_ZERO(rx);
            FD_SET(0, rx);
            if (fd)
            {
                FD_SET(fd, rx);
                nfds = fd + 1;
            }
            else
                nfds = 1;
            to.tv_sec = to.tv_usec = 0;
            if ((ch = select(nfds, rx, NULL, NULL, &to)) <= 0)
            {
                if (flushf)
                    (*flushf) ();

                if (big_picture)
                    refresh();
                else
                    oflush();

                FD_ZERO(rx);
                FD_SET(0, rx);
                if (fd)
                    FD_SET(fd, rx);

                while ((ch = select(nfds, rx, NULL, NULL, i_top)) < 0)
                {
                    if (errno != EINTR)
                        return -1;
                }
                if (ch == 0)
                    return I_TIMEOUT;
            }
            if (fd && FD_ISSET(fd, rx))
                return I_OTHERDATA;

            for (;;)
            {
                ch = iread(0, data, IBUFSIZE);

                if (ch > 0)
                    break;
                if ((ch < 0) && (errno == EINTR))
                    continue;

                abort_bbs();
            }
            icurrchar = (*data & 0xff) == IAC ? iac_count(data) : 0;
            if (icurrchar >= ch)
                goto igetnext;
            ibufsize = ch;
            i_mode = INPUT_ACTIVE;
        }
        ch = data[icurrchar++];
        if (trailing)
        {
            trailing = 0;
            if (ch == 0 || ch == 0x0a)
                continue;
        }
        if (ch == Ctrl('L'))
        {
            redoscr();
            continue;
        }
        if (ch == 0x0d)
        {
            trailing = 1;
            ch = '\n';
        }
        uinfo.idle_time = time(0);
        update_utmp();
        return (ch);
    }
}
#else
int igetch()
{
    speed_limit_read();
  igetagain:
    if (ibufsize == icurrchar)
    {
        fd_set readfds;
        struct timeval to;
        int sr;

        to.tv_sec = 0;
        to.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        if (i_newfd)
            FD_SET(i_newfd, &readfds);
        if ((sr = select(FD_SETSIZE, &readfds, NULL, NULL, &to)) <= 0)
        {
            if ((sr == -1) && (errno != EINTR))
                return -1;
            if (flushf)
                (*flushf) ();
            if (dumb_term)
                oflush();
            else
                refresh();
            FD_ZERO(&readfds);
            FD_SET(0, &readfds);
            if (i_newfd)
                FD_SET(i_newfd, &readfds);
            while ((sr =
                    select(FD_SETSIZE, &readfds, NULL, NULL, i_top)) < 0)
            {
                if (errno != EINTR)
                    return -1;
            }
            if (sr == 0)
                return I_TIMEOUT;
        }
        if (i_newfd && FD_ISSET(i_newfd, &readfds))
            return I_OTHERDATA;
        ibufsize = iread(0, inbuf, IBUFSIZE);
        while (ibufsize <= 0)
        {
            if (ibufsize == 0)
                longjmp(byebye, -1);
            if (ibufsize < 0 && errno != EINTR)
                longjmp(byebye, -1);
            ibufsize = iread(0, inbuf, IBUFSIZE);
        }
        icurrchar = 0;
    }
    i_mode = INPUT_ACTIVE;
    switch (inbuf[icurrchar])
    {
    case Ctrl('L'):
        redoscr();
        icurrchar++;
        goto igetagain;
    default:
        break;
    }
    return inbuf[icurrchar++];
}
#endif

int igetkey(void)
{
    int mode;
    int ch = 0, last;
    extern int RMSG;
    int readbyte;

    mode = last = 0;
    while (1)
    {
        if ((uinfo.in_chat == YEA || uinfo.mode == TALK
             || uinfo.mode == PAGE) && RMSG == YEA)
        {
            char a;

            readbyte = iread(0, &a, 1);
            if (readbyte <= 0)
            {
                if (!readbyte)
                {
                    abort_bbs();
                }
                else
                {
                    if (errno != EINTR)
                    {
                        abort_bbs();
                    }
                    ch = EOF;
                }
            }
            else
            {
                ch = (int) a;
            }
        }
        else
        {
            ch = igetch();
            if (ch == -1)
                abort_bbs();
        }
        if ((ch == Ctrl('Z')) && (RMSG == NA))
        {
            r_msg2();
            return 0;
        }
        if (mode == 0)
        {
            if (ch == KEY_ESC)
                mode = 1;
            else
                return ch;
        }
        else if (mode == 1)
        {
            if (ch == '[' || ch == 'O')
                mode = 2;
            else if (ch == '1' || ch == '4')
                mode = 3;
            else
            {
                KEY_ESC_arg = ch;
                return KEY_ESC;
            }
        }
        else if (mode == 2)
        {
            if (ch >= 'A' && ch <= 'D')
                return KEY_UP + (ch - 'A');
            else if (ch >= '1' && ch <= '6')
                mode = 3;
            else
                return ch;
        }
        else if (mode == 3)
        {
            if (ch == '~')
                return KEY_HOME + (last - '1');
            else
                return ch;
        }
        last = ch;
    }
}

void top_show(char *prompt)
{
    if (editansi)
    {
        prints(ANSI_RESET);
        refresh();
    }
    move(0, 0);
    clrtoeol();
    standout();
    prints("%s", prompt);
    standend();
}

int ask(char *prompt)
{
    int ch;

    top_show(prompt);
    ch = igetkey();
    move(0, 0);
    clrtoeol();
    return (ch);
}

extern int enabledbchar;

int getdata(int line, int col, char *prompt, char *buf, int len, int echo,
            int clearlabel)
{
    int ch, clen = 0, curr = 0, x, y;
    int currDEC = 0, i, patch = 0, intact = 0;
    char tmp[STRLEN];
    extern unsigned char scr_cols;
    extern int RMSG;
    extern int msg_num;

    if (clearlabel == YEA && len > 0)
    {
        buf[0] = '\0';
    }
    if (clearlabel == 2)
        intact = 1;
    move(line, col);
    if (prompt)
        prints("%s", prompt);
    y = line;
    col += (prompt == NULL) ? 0 : strlen(prompt);
    x = col;
    clen = strlen(buf);
    curr = (clen >= len) ? len - 1 : clen;
    buf[curr] = '\0';
    prints("%s", buf);

    if (dumb_term || echo == NA)
    {
        while ((ch = igetkey()) != '\r')
        {
            if (RMSG == YEA && msg_num == 0)
            {
                if (ch == Ctrl('Z') || ch == KEY_UP)
                {
                    buf[0] = Ctrl('Z');
                    clen = 1;
                    break;
                }
                if (ch == Ctrl('A') || ch == KEY_DOWN)
                {
                    buf[0] = Ctrl('A');
                    clen = 1;
                    break;
                }
            }
            if (ch == '\n')
                break;
            if (ch == '\177' || ch == Ctrl('H'))
            {
                if (clen == 0)
                {
                    continue;
                }
                clen--;
                ochar(Ctrl('H'));
                ochar(' ');
                ochar(Ctrl('H'));
                continue;
            }
            if (!isprint2(ch))
            {
                continue;
            }
            if (clen >= len - 1)
            {
                continue;
            }
            buf[clen++] = ch;
            if (echo)
                ochar(ch);
            else
                ochar('*');
        }
        buf[clen] = '\0';
        outc('\n');
        oflush();
        return clen;
    }
    clrtoeol();
    while (1)
    {
        if ((uinfo.in_chat == YEA || uinfo.mode == TALK) && RMSG == YEA)
        {
            refresh();
        }
        ch = igetkey();
        if ((RMSG == YEA) && msg_num == 0)
        {
            if (ch == Ctrl('Z') || ch == KEY_UP)
            {
                buf[0] = Ctrl('Z');
                clen = 1;
                break;
            }
            if (ch == Ctrl('A') || ch == KEY_DOWN)
            {
                buf[0] = Ctrl('A');
                clen = 1;
                break;
            }
        }
        if (ch == '\n' || ch == '\r')
            break;
        if (ch == Ctrl('R'))
        {
            enabledbchar = ~enabledbchar & 1;
            continue;
        }
        if (intact)
        {
            intact = 0;
            buf[0] = '\0';
            clen = curr = 0;
            move(y, x);
            clrtoeol();
        }
        if (ch == '\177' || ch == Ctrl('H'))
        {
            if (curr == 0)
            {
                continue;
            }
            currDEC = patch = 0;
            if (enabledbchar && buf[curr - 1] & 0x80)
            {
                for (i = curr - 2; i >= 0 && buf[i] & 0x80; i--)
                    patch++;
                if (patch % 2 == 0 && buf[curr] & 0x80)
                    patch = 1;
                else if (patch % 2)
                    patch = currDEC = 1;
                else
                    patch = 0;
            }
            if (currDEC)
                curr--;
            strcpy(tmp, &buf[curr + patch]);
            buf[--curr] = '\0';
            (void) strcat(buf, tmp);
            clen--;
            if (patch)
                clen--;
            move(y, x);
            prints("%s", buf);
            clrtoeol();
            move(y, x + curr);
            continue;
        }
        if (ch == KEY_DEL)
        {
            if (curr >= clen)
            {
                curr = clen;
                continue;
            }
            strcpy(tmp, &buf[curr + 1]);
            buf[curr] = '\0';
            (void) strcat(buf, tmp);
            clen--;
            move(y, x);
            prints("%s", buf);
            clrtoeol();
            move(y, x + curr);
            continue;
        }
        if (ch == KEY_LEFT)
        {
            if (curr == 0)
            {
                continue;
            }
            curr--;
            move(y, x + curr);
            continue;
        }
        if (ch == Ctrl('E') || ch == KEY_END)
        {
            curr = clen;
            move(y, x + curr);
            continue;
        }
        if (ch == Ctrl('A') || ch == KEY_HOME)
        {
            curr = 0;
            move(y, x + curr);
            continue;
        }
        if (ch == KEY_RIGHT)
        {
            if (curr >= clen)
            {
                curr = clen;
                continue;
            }
            curr++;
            move(y, x + curr);
            continue;
        }
        if (!isprint2(ch))
        {
            continue;
        }
        if (x + clen >= scr_cols || clen >= len - 1)
        {
            continue;
        }
        if (!buf[curr])
        {
            buf[curr + 1] = '\0';
            buf[curr] = ch;
        }
        else
        {
            strncpy(tmp, &buf[curr], len);
            buf[curr] = ch;
            buf[curr + 1] = '\0';
            strncat(buf, tmp, len - curr);
        }
        curr++;
        clen++;
        move(y, x);
        prints("%s", buf);
        move(y, x + curr);
    }
    buf[clen] = '\0';
    if (echo)
    {
        move(y, x);
        prints("%s", buf);
    }
    outc('\n');
    refresh();
    return clen;
}

#define LINELEN 132

int ingetdata = 0;
int UPDOWN = 0;
extern unsigned char scr_cols, scr_lns;

int multi_getdata(int line, int col, int maxcol, char *prompt, char *buf,
                  int len, int maxline, int clearlabel)
{
    int ch, x, y, startx, starty, now, i, j, k, i0, chk, cursorx =
        0, cursory = 0;
    char savebuffer[25][LINELEN * 3];
    int init = 1;
    int nCharNum[23];
    char tmp[1024];
    extern int RMSG;

    if (uinfo.mode != MSG)
        ingetdata = 1;
    if (clearlabel == 1)
    {
        buf[0] = 0;
    }
    move(line, col);
    if (prompt)
        prints("%s", prompt);
    getyx(&starty, &startx);
    now = strlen(buf);
    for (i = 0; i <= 24; i++)
        saveline2(i, 0, savebuffer[i]);

    strncpy(tmp, buf, 1024);
    tmp[1023] = 0;

    while (1)
    {

        memset(nCharNum, 0, 23 * sizeof(int));
        y = starty;
        x = startx;
        move(y, x);
        chk = 0;
        if (now == 0)
        {
            cursory = y;
            cursorx = x;
        }
        for (i = 0; i < (int) strlen(buf); i++)
        {
            if (chk)
                chk = 0;
            else if (buf[i] < 0)
                chk = 1;

            if ((buf[i] > 0))
                nCharNum[y]++;

            if (chk && x > maxcol)
                x++;
            if (buf[i] != 10)
            {

                if ((x == maxcol) && ((maxcol - nCharNum[y]) % 2 == 0)
                    && (buf[i] < 0))
                {

                    clrtoeol();

                    x = 0;
                    y++;

                    if (y >= scr_lns)
                    {
                        scroll();
                        starty--;
                        cursory--;
                        y--;
                    }

                    move(y, x);
                    clrtoeol();
                    prints("%c", buf[i]);
                    x = 1;
                }

                else if (x > maxcol)
                {

                    clrtoeol();

                    x = 0;
                    y++;

                    if (y >= scr_lns)
                    {
                        scroll();
                        starty--;
                        cursory--;
                        y--;
                    }

                    move(y, x);
                    clrtoeol();

                    if ((buf[i] > 0))
                        nCharNum[y]++;
                    prints("%c", buf[i]);
                    x = 1;
                }
                else
                {
                    if (init)
                        prints("\x1b[4m");
                    prints("%c", buf[i]);

                    x++;
                }
            }
            else
            {
                clrtoeol();
                x = 0;
                y++;

                if (y >= scr_lns)
                {
                    scroll();
                    starty--;
                    cursory--;
                    y--;
                }
                move(y, x);
            }
            if (i == now - 1)
            {
                cursory = y;
                cursorx = x;
            }
            move(y + 1, 0);
            clrtoeol();
            move(y, x);
        }
        clrtoeol();
        move(cursory, cursorx);
        ch = igetkey();
        if (ch == -1)
            break;

        if (ch == '\n')
        {
            if (buf[i - 1] == '\n')
            {
                buf[i - 1] = 0;
            }
            else
            {
                buf[i] = 0;
            }
            break;
        }

        for (i = starty; i <= y; i++)
            saveline2(i, 1, savebuffer[i]);

        if (1 == RMSG && (KEY_UP == ch || KEY_DOWN == ch) && (!buf[0]))
        {
            ingetdata = 0;
            return -ch;
        }
        if (1 == UPDOWN && (KEY_UP == ch || KEY_DOWN == ch))
        {
            ingetdata = 0;
            return -ch;
        }
        if (RMSG && (ch == Ctrl('Z')) && (!buf[0]))
        {
            ingetdata = 0;
            return ch;
        }

        switch (ch)
        {
        case KEY_ESC:
            init = 1;
            strncpy(buf, tmp, Min(len, STRLEN));
            buf[Min(len, 1024 - 1)] = 0;
            now = strlen(buf);
            break;
        case Ctrl('Q'):

        case '\r':
            if (init)
            {
                init = 0;
                buf[0] = 0;
                now = 0;
            }
            if (UPDOWN)
                break;
            if (y - starty + 1 <= maxline)
            {
                for (i = strlen(buf) + 1; i > now; i--)
                    buf[i] = buf[i - 1];
                buf[now++] = '\n';
            }
            break;
        case KEY_UP:
            init = 0;
            if (cursory > starty)
            {
                y = starty;
                x = startx;
                chk = 0;
                if (y == cursory - 1 && x <= cursorx)
                    now = 0;
                for (i = 0; i < (int) strlen(buf); i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                    if (chk && x > maxcol)
                        x++;
                    if (buf[i] != 13 && buf[i] != 10)
                    {
                        if (x > maxcol)
                        {
                            x = col;
                            y++;
                        }
                        x++;
                    }
                    else
                    {
                        x = col;
                        y++;
                    }
                    if (y == cursory - 1 && x <= cursorx)
                        now = i + 1;
                }
            }
            break;
        case KEY_DOWN:
            init = 0;
            if (cursory < y)
            {
                y = starty;
                x = startx;
                chk = 0;
                if (y == cursory + 1 && x <= cursorx)
                    now = 0;
                for (i = 0; i < (int) strlen(buf); i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                    if (chk && x > maxcol)
                        x++;
                    if (buf[i] != 13 && buf[i] != 10)
                    {
                        if (x > maxcol)
                        {
                            x = col;
                            y++;
                        }
                        x++;
                    }
                    else
                    {
                        x = col;
                        y++;
                    }
                    if (y == cursory + 1 && x <= cursorx)
                        now = i + 1;
                }
            }
            break;
        case '\177':
        case Ctrl('H'):
            if (init)
            {
                init = 0;
                buf[0] = 0;
                now = 0;
            }
            if (now > 0)
            {

                for (i = now - 1; i < (int) strlen(buf); i++)
                    buf[i] = buf[i + 1];
                now--;
                chk = 0;
                for (i = 0; i < now; i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                }
                if (chk)
                {
                    for (i = now - 1; i < (int) strlen(buf); i++)
                        buf[i] = buf[i + 1];
                    now--;
                }
            }
            break;

        case KEY_DEL:
            if (init)
            {
                init = 0;
                buf[0] = 0;
                now = 0;
            }
            if (now < (int) strlen(buf))
            {
                chk = 0;
                for (i = 0; i < now + 1; i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                }
                if (chk)
                    for (i = now; i < (int) strlen(buf); i++)
                        buf[i] = buf[i + 1];
                for (i = now; i < (int) strlen(buf); i++)
                    buf[i] = buf[i + 1];
            }
            break;
        case KEY_LEFT:
            init = 0;
            if (now > 0)
            {
                now--;
                chk = 0;
                for (i = 0; i < now; i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                }
                if (chk)
                    now--;
            }
            break;
        case KEY_RIGHT:
            init = 0;
            if (now < (int) strlen(buf))
            {
                now++;
                chk = 0;
                for (i = 0; i < now; i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;
                }
                if (chk)
                    now++;
            }
            break;
        case KEY_HOME:
        case Ctrl('A'):
            init = 0;
            now--;
            while (now >= 0 && buf[now] != '\n' && buf[now] != '\r')
                now--;
            now++;
            break;
        case KEY_END:
        case Ctrl('E'):
            init = 0;
            while (now < (int) strlen(buf) && buf[now] != '\n'
                   && buf[now] != '\r')
                now++;
            break;
        case KEY_PGUP:
            init = 0;
            now = 0;
            break;
        case KEY_PGDN:
            init = 0;
            now = strlen(buf);
            break;
        case Ctrl('Y'):
            if (init)
            {
                init = 0;
                buf[0] = 0;
                now = 0;
            }
            i0 = strlen(buf);
            i = now - 1;
            while (i >= 0 && buf[i] != '\n' && buf[i] != '\r')
                i--;
            i++;
            if (!buf[i])
                break;
            j = now;
            while (j < i0 - 1 && buf[j] != '\n' && buf[j] != '\r')
                j++;
            if (j >= i0 - 1)
                j = i0 - 1;
            j = j - i + 1;
            if (j < 0)
                j = 0;
            for (k = 0; k < i0 - i - j + 1; k++)
                buf[i + k] = buf[i + j + k];

            y = starty;
            x = startx;
            chk = 0;
            if (y == cursory && x <= cursorx)
                now = 0;
            for (i = 0; i < (int) strlen(buf); i++)
            {
                if (chk)
                    chk = 0;
                else if (buf[i] < 0)
                    chk = 1;

                if (chk && x > maxcol)
                    x++;
                if (buf[i] != 13 && buf[i] != 10)
                {

                    if (x > maxcol)
                    {
                        x = col;
                        y++;
                    }
                    x++;
                }
                else
                {
                    x = col;
                    y++;
                }
                if (y == cursory && x <= cursorx)
                    now = i + 1;
            }

            if (now > (int) strlen(buf))
                now = strlen(buf);
            break;
        default:
            if (isprint2(ch) && (int) strlen(buf) < len - 1)
            {
                if (init)
                {
                    init = 0;
                    buf[0] = 0;
                    now = 0;
                }
                for (i = strlen(buf) + 1; i > now; i--)
                    buf[i] = buf[i - 1];
                buf[now++] = ch;
                y = starty;
                x = startx;
                chk = 0;
                for (i = 0; i < (int) strlen(buf); i++)
                {
                    if (chk)
                        chk = 0;
                    else if (buf[i] < 0)
                        chk = 1;

                    if (chk && x > maxcol)
                        x++;
                    if (buf[i] != 13 && buf[i] != 10)
                    {

                        if (x > maxcol)
                        {
                            x = col;
                            y++;
                        }
                        x++;
                    }
                    else
                    {
                        x = col;
                        y++;
                    }
                }
                if (y - starty + 1 > maxline)
                {
                    for (i = now - 1; i < (int) strlen(buf); i++)
                        buf[i] = buf[i + 1];
                    now--;
                }
            }
            init = 0;
            break;
        }
        if (buf[0] == '\n')
        {
            buf[0] = 0;
            break;
        }
    }

    ingetdata = 0;

    auto_insert(buf, EACH_LINE_LENGTH, '\n', maxline);

    return y - starty + 1;
}

void auto_insert(char *buffer, int each_length, char char_insert,
                 int maxline)
{
    char temp[MAX_MSG_SIZE];

    char *anchor = buffer;
    int nCount = -1;
    int nCharNum = 0;

    int nReturnNum = 0;

    while (*anchor != '\0')
    {
        if (*anchor == char_insert)
        {
            nCount = 0;
            nCharNum = 0;
            nReturnNum++;
        }
        else if ((*anchor > 0))
        {
            nCharNum++;
            nCount++;
        }
        else
            nCount++;

        if ((nCount == each_length) && ((each_length - nCharNum) % 2 == 0)
            && ((*anchor < 0)))
        {
            memset(temp, 0, MAX_MSG_SIZE);
            strcpy(temp, anchor);
            *anchor = char_insert;

            nCount = -1;
            nCharNum = 0;
            nReturnNum++;
            strcpy(anchor + 1, temp);
        }

        else if (nCount > each_length)
        {
            memset(temp, 0, MAX_MSG_SIZE);
            strcpy(temp, anchor);
            *anchor = char_insert;

            nCount = -1;
            nCharNum = 0;
            nReturnNum++;
            strcpy(anchor + 1, temp);
        }

        anchor++;
    }

}

#ifdef SPEED_LIMIT

#define SL_CHECK_POINT (96*1024)
#define SL_SPEED_LIMIT (2048)

#define SL_COUNTER_MAX (120)

#define SL_SLEEP_FACTOR (20)
#if 0
static float get_cputime(void)
{
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);
return (usage.ru_utime.tv_usec / 1000000.0 + usage.ru_utime.tv_sec
            + usage.ru_stime.tv_usec / 1000000.0 + usage.ru_stime.tv_sec)}
#endif
static int sl_read_counter = 0;
inline static void speed_limit_read(void)
{
    ++sl_read_counter;
}
static void speed_limit(int written_bytes)
{

    static int bytes_counter = -1;
    static time_t prev_chktime = 0;
    static int counter = 0;
    static char warn[] =
        "detect robot. if any problem, report to board SYSOP";

    register int r_bcounter = bytes_counter;

    if (written_bytes < 1)
        return;

    if (-1 == r_bcounter)
    {
        prev_chktime = time(NULL);
        bytes_counter = written_bytes;
        return;
    }

    r_bcounter += written_bytes;

    if (r_bcounter > SL_CHECK_POINT)
    {
        time_t now;

        now = time(NULL);
        r_bcounter -=
            SL_SPEED_LIMIT * (now - prev_chktime) + sl_read_counter * 4;
        sl_read_counter = 0;
        prev_chktime = now;
        if (r_bcounter < 1)
        {
            r_bcounter = 0;
            --counter;
            counter = (counter < 1 ? 0 : counter);
        }
        else
        {
            int stime =
                r_bcounter * SL_SLEEP_FACTOR / SL_SPEED_LIMIT *
                written_bytes;
            stime += counter * 50000;
            if (stime > 100000)
            {
                ++counter;
                if (counter > SL_COUNTER_MAX)
                {
                    securityreport("detect robot");
                    write(1, warn, sizeof(warn));
                    abort_bbs();
                }
                usleep(stime);
            }
        }
    }

    bytes_counter = r_bcounter;
}

#else

inline static void speed_limit(int written_bytes)
{
}
inline static void speed_limit_read(void)
{
}
#endif
