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
$Id: io.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
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

#define BtoGtablefile "etc/b2g_table"
#define GtoBtablefile "etc/g2b_table"

unsigned char *GtoB, *BtoG;

#define GtoB_count 7614
#define BtoG_count 13973

extern int convcode;
extern void redoscr ();


void auto_insert(char* buffer, int each_length, char char_insert, int maxline);


int
switch_code ()
{
    convcode = !convcode;
    redoscr ();
}

void
resolve_GbBig5Files (void)
{
    int fd;
    int i;

    BtoG =
        (unsigned char *) attach_shm ("CONV_SHMKEY", 3013,
                                      GtoB_count * 2 + BtoG_count * 2);
    fd = open (BtoGtablefile, O_RDONLY);
    if (fd == -1)
        for (i = 0; i < BtoG_count; i++)
        {
            BtoG[i * 2] = 0xA1;
            BtoG[i * 2 + 1] = 0xF5;
        }
    else
    {
        read (fd, BtoG, BtoG_count * 2);
        close (fd);
    }
    fd = open (GtoBtablefile, O_RDONLY);
    if (fd == -1)
        for (i = 0; i < GtoB_count; i++)
        {
            BtoG[BtoG_count * 2 + i * 2] = 0xA1;
            BtoG[BtoG_count * 2 + i * 2 + 1] = 0xBC;
        }
    else
    {
        read (fd, BtoG + BtoG_count * 2, GtoB_count * 2);
        close (fd);
    }
    GtoB = BtoG + BtoG_count * 2;
}

int
write2 (int port, char *str, int len)	// write gb to big
{
    register int i, locate;
    register unsigned char ch1, ch2, *ptr;

    for (i = 0, ptr = str; i < len; i++)
    {
        ch1 = (ptr + i)[0];
        if (ch1 < 0xA1 || (ch1 > 0xA9 && ch1 < 0xB0) || ch1 > 0xF7)
            continue;
        ch2 = (ptr + i)[1];
        i++;
        if (ch2 < 0xA0 || ch2 == 0xFF)
            continue;
        if ((ch1 > 0xA0) && (ch1 < 0xAA))	//01¡«09ÇøÎª·ûºÅÊý×Ö
            locate = ((ch1 - 0xA1) * 94 + (ch2 - 0xA1)) * 2;
        else			//if((buf > 0xAF) && (buf < 0xF8)){ //16¡«87ÇøÎªºº×Ö
            locate = ((ch1 - 0xB0 + 9) * 94 + (ch2 - 0xA1)) * 2;
        (ptr + i - 1)[0] = GtoB[locate++];
        (ptr + i - 1)[1] = GtoB[locate];
    }
    return write (port, str, len);
}

int
read2 (int port, char *str, int len)	// read big from gb
{
    /*
     * Big-5 ÊÇÒ»¸öË«×Ö½Ú±àÂë·½°¸£¬ÆäµÚÒ»×Ö½ÚµÄÖµÔÚ 16 ½ø
     * ÖÆµÄ A0¡«FE Ö®¼ä£¬µÚ¶þ×Ö½ÚÔÚ 40¡«7E ºÍ A1¡«FE Ö®¼ä¡£
     * Òò´Ë£¬ÆäµÚÒ»×Ö½ÚµÄ×î¸ßÎ»ÊÇ 1£¬µÚ¶þ×Ö½ÚµÄ×î¸ßÎ»Ôò¿É
     * ÄÜÊÇ 1£¬Ò²¿ÉÄÜÊÇ 0¡£
     */
    register unsigned char ch1, ch2, *ptr;
    register int locate, i = 0;

    if (len == 0)
        return 0;
    len = read (port, str, len);
    if (len < 1)
        return len;

    for (i = 0, ptr = str; i < len; i++)
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

void
hit_alarm_clock ()
{
    if (HAS_PERM (PERM_NOTIMEOUT))
        return;
    //      if (i_mode == INPUT_IDLE) {
    if (i_mode == INPUT_IDLE || uinfo.mode == LOGIN)
    {
        clear ();
        prints ("Idle timeout exceeded! Booting...\n");
        kill (getpid (), SIGHUP);
    }
    i_mode = INPUT_IDLE;
    if (uinfo.mode == LOGIN)
        alarm (LOGIN_TIMEOUT);
    else
        alarm (IDLE_TIMEOUT);
}

void
init_alarm ()
{
    signal (SIGALRM, hit_alarm_clock);
    alarm (IDLE_TIMEOUT);
}

#ifdef BBSD
void
oflush ()
{
    register int size;

    if (size = obufsize)
    {
#ifdef ALLOWSWITCHCODE
        if (convcode)
            write2 (0, outbuf, size);
        else
#endif

            write (0, outbuf, size);
        obufsize = 0;
    }
}
//output,rewrite by cf
void
output (char *s, int len)
{
    /* Invalid if len >= OBUFSIZE */
    if(obufsize + len > OBUFSIZE)
        oflush();
    if ( len > OBUFSIZE)
    {                           /* doin a oflush */
#ifdef ALLOWSWITCHCODE
        if (convcode)
            write2 (1,s, len);
        else
#endif

            write (1, s, len);
        return;
    }
    memcpy (outbuf + obufsize, s, len);
    obufsize += len;
}
/*void
output (s, len)
     char *s;
     int len;
{
 
  register int size;
  register char *data;
 
  size = obufsize;
  data = outbuf;
  if (size + len > OBUFSIZE)
    {
#ifdef ALLOWSWITCHCODE
      if (convcode)
	write2 (0, data, size);
      else
#endif
	write (0, data, size);
      size = len;
    }
  else
    {
      data += size;
      size += len;
    }
  memcpy (data, s, len);
  obufsize = size;
}
*/
void
ochar (c)
register int c;
{
    register char *data;
    register int size;

    data = outbuf;
    size = obufsize;

    if (size > OBUFSIZE - 2)
    {				/* doin a oflush */
#ifdef ALLOWSWITCHCODE
        if (convcode)
            write2 (0, data, size);
        else
#endif

            write (0, data, size);
        size = 0;
    }
    data[size++] = c;
    if (c == IAC)
        data[size++] = c;

    obufsize = size;
}
#else
void
oflush ()
{
    //  if (obufsize)
   // register int size;
  /*  if(size=obufsize)
    {
        int i;
        for(i=0;i<size;i++)
        {
            if(outbuf[i] == -1)
                outbuf[i]=' ';
        }*/
        if(obufsize > 0) {
#ifdef ALLOWSWITCHCODE
        if (convcode)
            write2 (1, outbuf, obufsize);
        else
#endif

            write (1, outbuf, obufsize);
        obufsize = 0;
        	}
}


void
output (s, len)
char *s;
int len;
{
/* Invalid if len >= OBUFSIZE */

if(len >= OBUFSIZE) {
	#ifdef ALLOWSWITCHCODE
        if (convcode)
            write2 (1, s, len);
        else
#endif
            write (1, s, len);
	

} else {

if (obufsize + len >= OBUFSIZE)
{				
	oflush();		/* doin a oflush */

} 

	memcpy (outbuf + obufsize, s, len);
	obufsize += len;

}
}
void
ochar (c)
int c;
{
    if (obufsize >= OBUFSIZE )
    {
        oflush();				/* doin a oflush */
    }
    outbuf[obufsize++] = c;
}
#endif

int i_newfd = 0;
struct timeval i_to, *i_top = NULL;
int (*flushf) () = NULL;

void
add_io (fd, timeout)
int fd;
int timeout;
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

void
add_flush (flushfunc)
int (*flushfunc) ();
{
    flushf = flushfunc;
}

/*
int
num_in_buf()
{
	return icurrchar - ibufsize;
}
*/
int
num_in_buf ()
{
    int n;

    if ((n = icurrchar - ibufsize) < 0)
        n = 0;
    return n;
}

#ifdef BBSD
static int
iac_count (current)
char *current;
{
    switch (*(current + 1) & 0xff)
    {
    case DO:
    case DONT:
    case WILL:
    case WONT:
        return 3;
    case SB:			/* loop forever looking for the SE */
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
int
igetch ()
{
    static int trailing = 0;
    register int ch;
    register char *data;

    data = inbuf;

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

            uinfo.idle_time = time (0);
            update_utmp ();	/* Ó¦¸ÃÊÇÐèÒª update Ò»ÏÂ :X */

            FD_ZERO (rx);
            FD_SET (0, rx);
            if (fd)
            {
                FD_SET (fd, rx);
                nfds = fd + 1;
            }
            else
                nfds = 1;
            to.tv_sec = to.tv_usec = 0;
            if ((ch = select (nfds, rx, NULL, NULL, &to)) <= 0)
            {
                if (flushf)
                    (*flushf) ();

                if (big_picture)
                    refresh ();
                else
                    oflush ();

                FD_ZERO (rx);
                FD_SET (0, rx);
                if (fd)
                    FD_SET (fd, rx);

                while ((ch = select (nfds, rx, NULL, NULL, i_top)) < 0)
                {
                    if (errno != EINTR)
                        return -1;
                }
                if (ch == 0)
                    return I_TIMEOUT;
            }
            if (fd && FD_ISSET (fd, rx))
                return I_OTHERDATA;

            for (;;)
            {
#ifdef ALLOWSWITCHCODE
                if (convcode)
                    ch = read2 (0, data, IBUFSIZE);
                else
#endif

                    ch = read (0, data, IBUFSIZE);

                if (ch > 0)
                    break;
                if ((ch < 0) && (errno == EINTR))
                    continue;
                //longjmp(byebye, -1);
                abort_bbs ();	/* ·ÇÕý³£¶ÏÏßµÄ´¦Àí */
            }
            icurrchar = (*data & 0xff) == IAC ? iac_count (data) : 0;
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
        if (ch == Ctrl ('L'))
        {
            redoscr ();
            continue;
        }
        if (ch == 0x0d)
        {
            trailing = 1;
            ch = '\n';
        }
        uinfo.idle_time = time (0);
        update_utmp ();
        return (ch);
    }
}
#else
int
igetch ()
{
igetagain:
    if (ibufsize == icurrchar)
    {
        fd_set readfds;
        struct timeval to;
        int sr;

        to.tv_sec = 0;
        to.tv_usec = 0;
        FD_ZERO (&readfds);
        FD_SET (0, &readfds);
        if (i_newfd)
            FD_SET (i_newfd, &readfds);
        if ((sr = select (FD_SETSIZE, &readfds, NULL, NULL, &to)) <= 0)
        {
            if((sr == -1) && (errno != EINTR))
                return -1;
            if (flushf)
                (*flushf) ();
            if (dumb_term)
                oflush ();
            else
                refresh ();
            FD_ZERO (&readfds);
            FD_SET (0, &readfds);
            if (i_newfd)
                FD_SET (i_newfd, &readfds);
            while ((sr = select (FD_SETSIZE, &readfds, NULL, NULL, i_top)) < 0)
            {
                if (errno != EINTR)
                    return -1;
            }
            if (sr == 0)
                return I_TIMEOUT;
        }
        if (i_newfd && FD_ISSET (i_newfd, &readfds))
            return I_OTHERDATA;
#ifdef ALLOWSWITCHCODE

        if (convcode)
            ibufsize = read2 (0, inbuf, IBUFSIZE);
        else
#endif

            ibufsize = read (0, inbuf, IBUFSIZE);
        while (ibufsize <= 0)
        {
            if (ibufsize == 0)
                longjmp (byebye, -1);
            if (ibufsize < 0 && errno != EINTR)
                longjmp (byebye, -1);
#ifdef ALLOWSWITCHCODE

            if (convcode)
                ibufsize = read2 (0, inbuf, IBUFSIZE);
            else
#endif

                ibufsize = read (0, inbuf, IBUFSIZE);
        }
        icurrchar = 0;
    }
    i_mode = INPUT_ACTIVE;
    switch (inbuf[icurrchar])
    {
    case Ctrl ('L'):
                    redoscr ();
        icurrchar++;
        goto igetagain;
    default:
        break;
    }
    return inbuf[icurrchar++];
}
#endif

int
igetkey ()
{
    int mode;
    int ch, last;
    extern int RMSG;
    int readbyte;

    mode = last = 0;
    while (1)
    {
        if ((uinfo.in_chat == YEA || uinfo.mode == TALK
                || uinfo.mode == PAGE) && RMSG == YEA)
        {
            char a;

#ifdef ALLOWSWITCHCODE

            if (convcode)
                read2 (0, &a, 1);
            else
#endif
                /*			read(0, &a, 1);
                			ch = (int) a;*/
                readbyte = read (0, &a, 1);
            if (readbyte <= 0)
            {
                if (!readbyte)
                {
                    abort_bbs ();
                }
                else
                {
                    if (errno != EINTR)
                    {
                        abort_bbs ();
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
            ch = igetch ();
            if(ch == -1)
                abort_bbs();
        }
        if ((ch == Ctrl ('Z')) && (RMSG == NA))
        {
            r_msg2 ();
            return 0;
        }
        if (mode == 0)
        {
            if (ch == KEY_ESC)
                mode = 1;
            else
                return ch;		/* Normal Key */
        }
        else if (mode == 1)
        {			/* Escape sequence */
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
        {			/* Cursor key */
            if (ch >= 'A' && ch <= 'D')
                return KEY_UP + (ch - 'A');
            else if (ch >= '1' && ch <= '6')
                mode = 3;
            else
                return ch;
        }
        else if (mode == 3)
        {			/* Ins Del Home End PgUp PgDn */
            if (ch == '~')
                return KEY_HOME + (last - '1');
            else
                return ch;
        }
        last = ch;
    }
}

void
top_show (prompt)
char *prompt;
{
    if (editansi)
    {
        prints (ANSI_RESET);
        refresh ();
    }
    move (0, 0);
    clrtoeol ();
    standout ();
    prints ("%s", prompt);
    standend ();
}

int
ask (prompt)
char *prompt;
{
    int ch;

    top_show (prompt);
    ch = igetkey ();
    move (0, 0);
    clrtoeol ();
    return (ch);
}

extern int enabledbchar;

int
getdata (line, col, prompt, buf, len, echo, clearlabel)
int line, col, len, echo, clearlabel;
char *prompt, *buf;
{
    int ch, clen = 0, curr = 0, x, y;
    int currDEC = 0, i, patch = 0;
    char tmp[STRLEN];
    extern unsigned char scr_cols;
    extern int RMSG;
    extern int msg_num;

    if (clearlabel == YEA)
    {
        memset (buf, 0, sizeof (buf));
    }
    move (line, col);
    if (prompt)
        prints ("%s", prompt);
    y = line;
    col += (prompt == NULL) ? 0 : strlen (prompt);
    x = col;
    clen = strlen (buf);
    curr = (clen >= len) ? len - 1 : clen;
    buf[curr] = '\0';
    prints ("%s", buf);

    if (dumb_term || echo == NA)
    {
        while ((ch = igetkey ()) != '\r')
        {
            if (RMSG == YEA && msg_num == 0)
            {
                if (ch == Ctrl ('Z') || ch == KEY_UP)
                {
                    buf[0] = Ctrl ('Z');
                    clen = 1;
                    break;
                }
                if (ch == Ctrl ('A') || ch == KEY_DOWN)
                {
                    buf[0] = Ctrl ('A');
                    clen = 1;
                    break;
                }
            }
            if (ch == '\n')
                break;
            if (ch == '\177' || ch == Ctrl ('H'))
            {
                if (clen == 0)
                {
                    continue;
                }
                clen--;
                ochar (Ctrl ('H'));
                ochar (' ');
                ochar (Ctrl ('H'));
                continue;
            }
            if (!isprint2 (ch))
            {
                continue;
            }
            if (clen >= len - 1)
            {
                continue;
            }
            buf[clen++] = ch;
            if (echo)
                ochar (ch);
            else
                ochar ('*');
        }
        buf[clen] = '\0';
        outc ('\n');
        oflush ();
        return clen;
    }
    clrtoeol ();
    while (1)
    {
        if ((uinfo.in_chat == YEA || uinfo.mode == TALK) && RMSG == YEA)
        {
            refresh ();
        }
        ch = igetkey ();
        if ((RMSG == YEA) && msg_num == 0)
        {
            if (ch == Ctrl ('Z') || ch == KEY_UP)
            {
                buf[0] = Ctrl ('Z');
                clen = 1;
                break;
            }
            if (ch == Ctrl ('A') || ch == KEY_DOWN)
            {
                buf[0] = Ctrl ('A');
                clen = 1;
                break;
            }
        }
        if (ch == '\n' || ch == '\r')
            break;
        if (ch == Ctrl ('R'))
        {
            enabledbchar = ~enabledbchar & 1;
            continue;
        }
        if (ch == '\177' || ch == Ctrl ('H'))
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
            strcpy (tmp, &buf[curr + patch]);
            buf[--curr] = '\0';
            (void) strcat (buf, tmp);
            clen--;
            if (patch)
                clen--;
            move (y, x);
            prints ("%s", buf);
            clrtoeol ();
            move (y, x + curr);
            continue;
        }
        if (ch == KEY_DEL)
        {
            if (curr >= clen)
            {
                curr = clen;
                continue;
            }
            strcpy (tmp, &buf[curr + 1]);
            buf[curr] = '\0';
            (void) strcat (buf, tmp);
            clen--;
            move (y, x);
            prints ("%s", buf);
            clrtoeol ();
            move (y, x + curr);
            continue;
        }
        if (ch == KEY_LEFT)
        {
            if (curr == 0)
            {
                continue;
            }
            curr--;
            move (y, x + curr);
            continue;
        }
        if (ch == Ctrl ('E') || ch == KEY_END)
        {
            curr = clen;
            move (y, x + curr);
            continue;
        }
        if (ch == Ctrl ('A') || ch == KEY_HOME)
        {
            curr = 0;
            move (y, x + curr);
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
            move (y, x + curr);
            continue;
        }
        if (!isprint2 (ch))
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
            strncpy (tmp, &buf[curr], len);
            buf[curr] = ch;
            buf[curr + 1] = '\0';
            strncat (buf, tmp, len - curr);
        }
        curr++;
        clen++;
        move (y, x);
        prints ("%s", buf);
        move (y, x + curr);
    }
    buf[clen] = '\0';
    if (echo)
    {
        move (y, x);
        prints ("%s", buf);
    }
    outc ('\n');
    refresh ();
    return clen;
}
#define LINELEN 132

int ingetdata = 0;
int UPDOWN = 0;
extern int scr_cols, scr_lns;



//modified by rwzmm @ sjtubbs
//debug for multi-msg

int multi_getdata(int line, int col, int maxcol, char *prompt, char *buf, int len, int maxline, int clearlabel)
{
    int ch, x, y, startx, starty, now, i, j, k, i0, chk, cursorx, cursory;
    char savebuffer[25][LINELEN * 3];
    int init = 1;
    int nCharNum[23];	//½â¾öºº×Ö»»ÐÐÎÊÌâµÄ±äÁ¿, by rwzmm @ sjtubbs
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

        memset(nCharNum, 0, 23*sizeof(int));
        y = starty;
        x = startx;
        move(y, x);
        chk = 0;
        if (now == 0)
        {
            cursory = y;
            cursorx = x;
        }
        for (i = 0; i < strlen(buf); i++)
        {
            if (chk)
                chk = 0;
            else if (buf[i] < 0)
                chk = 1;

            //			y = starty;

            //modified by rwzmm @ sjtubbs
            if ((buf[i] <= 127) && (buf[i] > 0))
                nCharNum[y]++;

            if (chk && x > maxcol)
                x++;
            if (buf[i] != 10)
            {

                if ((x == maxcol) && ((maxcol - nCharNum[y]) % 2 == 0) && (buf[i] < 0 || buf[i] > 127))
                {

                    clrtoeol();
                    //					nCharNum = 0;
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
                    prints("%c",buf[i]);
                    x = 1;
                }

                //end modify
                else if (x > maxcol)
                {

                    clrtoeol();
                    // 					nCharNum[y] = 0;
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

                    if((buf[i] <= 127) && (buf[i] > 0))
                        nCharNum[y] ++;
                    prints("%c",buf[i]);
                    x = 1;
                }
                else
                {
                    if (init)
                        prints("\x1b[4m");
                    prints("%c", buf[i]);
                    //                prints("[m");
                    x++;
                }
            }
            else
            {
                clrtoeol();
                x = 0;
                y++;
                //				nCharNum[y] = 0;
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
            move(y+1, 0);
            clrtoeol();
            move(y,x);
        }
        clrtoeol();
        move(cursory, cursorx);
        ch = igetkey();
        if(ch ==-1 )
            break;

        //Ô­À´µÄÁ½¸ö»Ø³µ·¢ËÍ¡£ÐÞ¸Ä

        if(ch == '\n')
        {
            if(buf[i - 1] == '\n')
            {
                buf[i-1] = 0;
            }
            else
            {
                buf[i] = 0;
            }
            break;
        }
        /*
        		
                if ((ch == '\n' && buf[i-1] =='\n')){
        			buf[i-1]=0;
                    break;
         
        		}*/

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
                        //»Ø³µ»»ÐÐ£¬¿¼ÂÇÈ¥µôÕâ¸ö
                        //        case '\n':
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
                for (i = 0; i < strlen(buf); i++)
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
                for (i = 0; i < strlen(buf); i++)
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
                // 				if((buf[now-1]>0) && (buf[now-1]<=127))
                for (i = now - 1; i < strlen(buf); i++)
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
                    for (i = now - 1; i < strlen(buf); i++)
                        buf[i] = buf[i + 1];
                    now--;
                }
            }
            break;

            /*
            case Ctrl('U'):
            	init = 0;
            	if (now > 0) {
            		for(i = now-1; i > 0 && buf[i] != '\n'; i--);
            		for(i++; i < strlen(buf); i++)
            			buf[i] = buf[i + 1];
            		now = i;
            	}
            	break;
            */

        case KEY_DEL:
            if (init)
            {
                init = 0;
                buf[0] = 0;
                now = 0;
            }
            if (now < strlen(buf))
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
                    for (i = now; i < strlen(buf); i++)
                        buf[i] = buf[i + 1];
                for (i = now; i < strlen(buf); i++)
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
            if (now < strlen(buf))
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
            while (now < strlen(buf) && buf[now] != '\n' && buf[now] != '\r')
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
            for (i = 0; i < strlen(buf); i++)
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

            if (now > strlen(buf))
                now = strlen(buf);
            break;
        default:
            if (isprint2(ch) && strlen(buf) < len - 1)
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
                for (i = 0; i < strlen(buf); i++)
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
                    for (i = now - 1; i < strlen(buf); i++)
                        buf[i] = buf[i + 1];
                    now--;
                }
            }
            init = 0;
            break;
        }
        if(buf[0]=='\n')
        {
            buf[0]=0;
            break;
        }
    }

    ingetdata = 0;

    auto_insert(buf, EACH_LINE_LENGTH, '\n', maxline);		//rwzmm @ sjtubbs

    return y - starty + 1;
}


void auto_insert(char* buffer, int each_length, char char_insert, int maxline)
{
    char temp[MAX_MSG_SIZE];
/*    int i = 0;*/
    char* anchor = buffer;
    int nCount = -1;
    int nCharNum = 0;

    //Í³¼ÆbufÀïÃæ»Ø³µÊýÁ¿£¬°Ñmessage³¤¶È¶¨Îª5ÐÐ
    int nReturnNum = 0;

    while(*anchor != '\0')
    {
        if(*anchor == char_insert)
        {
            nCount = 0;
            nCharNum = 0;
            nReturnNum ++;
        }
        else if((*anchor <= 127) && (*anchor >0))
        {
            nCharNum ++;
            nCount ++;
        }
        else
            nCount ++;

        if((nCount == each_length) && ((each_length - nCharNum) % 2 == 0) && ((*anchor > 127) || (*anchor < 0)))
        {
            memset(temp, 0, MAX_MSG_SIZE);
            strcpy(temp, anchor);
            *anchor = char_insert;
            //			anchor ++;
            nCount = -1;
            nCharNum = 0;
            nReturnNum ++;
            strcpy(anchor+1, temp);
        }

        else if (nCount > each_length)
        {
            memset(temp, 0, MAX_MSG_SIZE);
            strcpy(temp, anchor);
            *anchor = char_insert;
            //			anchor ++;
            nCount = -1;
            nCharNum = 0;
            nReturnNum ++;
            strcpy(anchor+1, temp);
        }

        anchor++;
    }

    /*
    	if(nReturnNum < maxline)
    	{
    		for(i = 0; i < maxline - nReturnNum; i++)
    		{
    			*anchor = '\n';
    			anchor ++;
    		}
    	}*/

}

