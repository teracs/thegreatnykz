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
time_t calltime = 0;
struct shortfile *getbcache(char *bname);

extern struct SHM_t *bbsshm;
struct ACSHM *movieshm = NULL;
int nnline = 0, xxxline = 0;

#define MORE_SCREEN_WIDTH 80

char more_buf[MORE_BUFSIZE*2+1];
char more_next[MORE_BUFSIZE];
int islinkmore;
int more_size, more_num,next_size;
int search_mode;
char search_string[42];
int isin, titleshow;
char readln_buf[512], tempbuf[512], second_buf[512];
int cursormode_def = -1;
int more_x = 0, more_y = 0;
int maxline;
int maxbytes;
int more_mode;
int is_reading;
int promptend;
int show_fastjump = 0;
struct more_link
{
    int y;
    int start_x, end_x;
    int type;
    char dest[150];
    int unfold;
};

struct more_link more_links[100];
int more_links_count;

#define MORE_LINK_USER          1
#define MORE_LINK_POST          2
#define MORE_LINK_ANNOUNCE      3
#define MORE_LINK_WIKI_ARCHIVE  4
#define MORE_LINK_ANNOUNCEDIR   5
#define MORE_LINK_URL           6

void ActiveBoard_Init(void)
{
    struct fileheader fh;
    FILE *fp;
    char *ptr;
    char buf[1024], buf2[1024];
    struct stat st;
    int max = 0, i = 0, j = 0, x, y = 0;
    int flag;

    if (movieshm == NULL)
    {
        attach_shm();
        movieshm = &(bbsshm->movieshm);
    }

    if (stat("boards/Bulletin/.DIGEST", &st) < 0)
    {
        empty_movie(1);
        return;
    }
    if (movieshm->update > st.st_mtime)
        return;

    for (i = 0; i < ACBOARD_MAXLINE; i++)
        movieshm->data[i][0] = 0;

    max = get_num_records("boards/Bulletin/.DIGEST", sizeof(fh));

    i = 1;
    j = 0;
    while (i <= max && j < ACBOARD_MAXLINE)
    {
        get_record("boards/Bulletin/.DIGEST", &fh, sizeof(fh), i++);
        sprintf(buf, "boards/Bulletin/%s/%s", getfolderstr(fh.filename),
                fh.filename);

        fp = fopen(buf, "r");
        if (fp == NULL)
            continue;

        y++;

        if (fh.title[0] == '$')
            flag = (int) (fh.title[1] - '0');
        else
            flag = 4;
        for (x = 0; x < flag; x++)
            fgets(buf, 1024, fp);

        flag = 0;
        for (x = 0; x < MAXMOVIE - 1 && j < ACBOARD_MAXLINE; x++)
        {
            if (fgets(buf, 1024, fp) != 0)
            {
                buf[ACBOARD_BUFSIZE - 4] = '\0';
                if (flag == 1 || strcmp(buf, "--\n") == 0)
                {
                    strcpy(buf2, "[K");
                    flag = 1;
                }
                ptr = movieshm->data[j];
                if (flag == 0)
                {
                    strcpy(buf2, "[K");
                    strcat(buf2, buf);
                }
                buf2[ACBOARD_BUFSIZE - 1] = '\0';
                memcpy(ptr, buf2, ACBOARD_BUFSIZE);
            }
            else
            {
                strcpy(movieshm->data[j], "[K");
            }
            j++;
        }
        fclose(fp);
    }
    if (j == 0)
    {
        empty_movie(3);
        return;
    }
    movieshm->movielines = j;
    movieshm->movieitems = y;
    movieshm->update = time(0);

    sprintf(buf, "»î¶¯¿´°å¸üÐÂ, ¹² %d ÐÐ, %d ²¿·Ý.", j, y);
    report(buf);
    return;
}

void empty_movie(int x)
{
    sprintf(genbuf, "Empty Movie!!! (error = %d)", x);
    report(genbuf);

    strcpy(movieshm->data[2], "[K      ** ÉÐÎ´Éè¶¨»î¶¯¿´°å ** ");
    strcpy(movieshm->data[3],
           "[K         ÇëÏê¼û°²×°ËµÃ÷Êé Firebird-2000 ");
    strcpy(movieshm->data[4], "[K         Éè¶¨ Bulletin °å");

    movieshm->movielines = MAXMOVIE;
    movieshm->movieitems = 1;
    movieshm->update = time(0);

}

void setcalltime(void)
{
    char ans[6];
    int ttt;

    move(1, 0);
    clrtoeol();
    getdata(1, 0, "¼¸·ÖÖÓºóÒªÏµÍ³ÌáÐÑÄã: ", ans, 3, DOECHO, YEA);
    ttt = atoi(ans);
    if (ttt <= 0)
        return;
    calltime = time(0) + ttt * 60;
}

int countln(char *fname)
{
    FILE *fp;
    char tmp[256];
    int count = 0;

    if ((fp = fopen(fname, "r")) == NULL)
        return 0;

    while (fgets(tmp, sizeof(tmp), fp) != NULL)
        count++;
    fclose(fp);
    return count;
}

void netty_more(void)
{
    char buf[256];
    int ne_row = 1;
    int x, y;
    time_t thetime = time(0);

    getyx(&y, &x);
    update_endline();
    if (!DEFINE(DEF_ACBOARD))
        return;
    nnline = (thetime / 10 % movieshm->movieitems) * (MAXMOVIE - 1);

    while ((nnline < movieshm->movielines))
    {
#ifdef BIGGER_MOVIE
        move(1 + ne_row, 0);
#else

        move(2 + ne_row, 0);
#endif

        clrtoeol();
        strcpy(buf, movieshm->data[nnline]);
        showstuff(buf);
        nnline = nnline + 1;
        ne_row = ne_row + 1;
        if (nnline == movieshm->movielines)
        {
            nnline = 0;
            break;
        }
        if (ne_row > MAXMOVIE - 1)
        {
            break;
        }
    }
    move(y, x);
}

int printacbar(void)
{
#ifndef BIGGER_MOVIE
    struct shortfile *bp;
    int x, y;

    getyx(&y, &x);
    bp = getbcache(DEFAULTBOARD);
    move(2, 0);
    if (bp->flag & VOTE_FLAG)
        prints
            (" [1;36m©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©È[37mÏµÍ³Í¶Æ±ÖÐ [ Config->Vote ] [36m©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´ [m\n");
    else
        prints
            (" [1;36m©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©È[37m»î  ¶¯  ¿´  °å[36m©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´ [m\n");
    move(2 + MAXMOVIE, 0);
    prints
        (" [1;36m©¸¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©¼[m\n");
    move(y, x);
#endif

    refresh();
    return 0;
}

int check_calltime(void)
{
    int line;

    if (calltime != 0 && time(0) >= calltime)
    {
        if (uinfo.mode == TALK)
            line = t_lines / 2 - 1;
        else
            line = 0;

        saveline(line, 0);
        bell();
        bell();
        bell();
        move(line, 0);
        clrtoeol();
        prints("[1;44;32mÏµÍ³Í¨¸æ: [37m%-65s[m",
               "ÏµÍ³ÄÖÖÓ Áå¡«¡«¡«¡«¡«¡«");
        igetkey();
        move(line, 0);
        clrtoeol();
        saveline(line, 1);
        calltime = 0;
    }
    return 0;
}
static void signal_r_monitor(int num)
{
    R_monitor();
}
void R_monitor(void)
{
    if (uinfo.mode != MMENU)
        return;
    if (!DEFINE(DEF_ACBOARD) && !DEFINE(DEF_ENDLINE))
        return;

    alarm(0);
    signal(SIGALRM, signal_r_monitor);

    if (uinfo.IsMsg == 0)
    {
        netty_more();
        printacbar();
    }
    if (!DEFINE(DEF_ACBOARD))
        alarm(55);

    else
        alarm(20);
}

#ifdef BELL_DELAY_FILTER
void bell_delay_filter(char *buf)
{
    int i;
    char *t;

    t = strstr(buf, "\033");
    if (t == NULL)
        return;
    for (i = 1; strchr("[0123456789; ", t[i]); i++);
    if (t[i] == '\107' || t[i] == '\115')
        *t = '\052';
    bell_delay_filter(t + 1);
}
#endif

int parse_more_link(char *buf, int *bytes, int y, int start_x, int buildlinks, char *seek_start)
{

    char parsebuf[STRLEN*2+5];
    char *startpos, *endpos, *colonpos, *pipepos,*newlinepos;
    char *type, *dest, *display;
    int parselen, is_wiki_archive,newlinep;
    islinkmore = 0;
    if (more_num > 2 && more_buf[more_num - 3] == '#')
        return -1;

    is_wiki_archive = 0;
    if (seek_start != NULL)
        startpos = seek_start;
    else
        startpos = more_buf + more_num;
    endpos = strstr(startpos, "]]");
    if(endpos == NULL){
        strncat(more_buf,more_next,MORE_BUFSIZE);
        endpos = strstr(startpos,"]]");
        islinkmore = endpos - more_buf - MORE_BUFSIZE + 2;
        if(islinkmore < 0 || islinkmore > STRLEN *2)
            islinkmore = 0;

    }
    parselen = endpos - startpos;
    if (parselen > STRLEN*2 - 2 || endpos == NULL)
        return 0;
    newlinepos = strstr(startpos, "\n");

    if(newlinepos != NULL&&newlinepos < endpos)
    {
        if (*(newlinepos+1) == ':') //if the next line refers to the origin text 
            return 0; 
      	newlinep = newlinepos-startpos;
    	strncpy(parsebuf, startpos, newlinep);
    	strncpy(&parsebuf[newlinep],&startpos[newlinep+1],parselen-newlinep-1);
    	parsebuf[parselen-1] = '\0';
    }
    else
    {
        strncpy(parsebuf, startpos, parselen);
        parsebuf[parselen] = '\0';
    }
    if (strncmp(parsebuf, "Wiki", 4) 
       && strncmp(parsebuf, "User", 4)  
       && strncmp(parsebuf, "Post", 4)  
       && strncmp(parsebuf, "Digest", 6) 
       && strncmp(parsebuf, "Digdir", 6) 
       && strncmp(parsebuf, "Url", 3)) 
        return 0;     
    if (more_mode == MORE_MODE_WIKI)
    {
        is_wiki_archive = 1;
        pipepos = strchr(parsebuf, '|');
        if (pipepos != NULL)
        {
            *pipepos = '\0';
            dest = parsebuf;
            display = pipepos + 1;      
        }
        else
        {
            dest = parsebuf;
            display = parsebuf;
        }
    }
    else
    {
        colonpos = strchr(parsebuf, ':');
        if (colonpos == NULL)
            return 0;
        else
        {
            *colonpos++ = '\0';
            type = parsebuf;
        
            pipepos = strchr(colonpos, '|');
            if (pipepos != NULL)
            {
                if (*(pipepos+1) == '\0')
                {
                    *pipepos = '\0';
                    dest = colonpos;
                    display = dest;
                }
                else
                {
                    *pipepos = '\0';
                    dest = colonpos;
                    display = pipepos + 1;        
                }
            }
            else
            {
                dest = colonpos;
                display = dest;
            }
        }
    }
        
    if (buildlinks&&more_links_count < 100)
    {
        more_links[more_links_count].y = y;
        more_links[more_links_count].start_x = start_x;
        more_links[more_links_count].end_x = start_x + strlen(display);
        strncpy(more_links[more_links_count].dest, dest,150);
        more_links[more_links_count].dest[149] = '\0';
        if (is_wiki_archive || !strcmp(type, "Wiki"))
            more_links[more_links_count].type = MORE_LINK_WIKI_ARCHIVE;
        else if (!strcmp(type, "User"))
            more_links[more_links_count].type = MORE_LINK_USER;
        else if (!strcmp(type, "Post"))
            more_links[more_links_count].type = MORE_LINK_POST;
        else if (!strcmp(type, "Digest"))
            more_links[more_links_count].type = MORE_LINK_ANNOUNCE;
        else if (!strcmp(type, "Digdir"))
            more_links[more_links_count].type = MORE_LINK_ANNOUNCEDIR;
        else if (!strcmp(type, "Url"))
            more_links[more_links_count].type = MORE_LINK_URL;

        more_links_count ++;

    }
    if(show_fastjump)
        sprintf(display,"%s [%2d] ",display ,more_links_count);
    else if (more_links[more_links_count - 1].unfold && *dest != '\0')
        display = dest;
    if (buf != NULL)
        sprintf(buf, "[0;1;4;37m%s[m", display);

    *bytes = parselen + 2;
    more_num += parselen + 2;

    return strlen(display);
}

char *more_get_strpos(char *buf, int given_x)
{
    char *pos = buf;
    int x = 0, in_esc = 0;
    char ch;
    while (*pos && x < given_x)
    {
        ch = *pos;
        if (ch == '\n')
            break;
        else if (ch == '\t')
            x += 4;
        else if (ch == '')
            in_esc = 1;
        else if (in_esc)
        {
            if (strchr("[0123456789;,", ch) == NULL)
                in_esc = 0;
        }
        else if (isprint2(ch))
            x ++;

        pos ++;
    }

    return pos;
}

int more_get_display_x(char *buf, char *given_pos)
{
    char *pos = buf;
    int x = 0, in_esc = 0;
    char ch;
    while (*pos && pos < given_pos)
    {
        ch = *pos;
        if (ch == '\n')
            break;
        else if (ch == '\t')
            x += 4;
        else if (ch == '')
            in_esc = 1;
        else if (in_esc)
        {
            if (strchr("[0123456789;,", ch) == NULL)
                in_esc = 0;
        }
        else if (isprint2(ch))
            x ++;

        pos ++;
    }

    return x;
}

int readln(int fd, char *buf, int linenum, int buildlinks)
{
    int len, bytes, in_esc, ch = 0, c;
    int link_candi = 0, link_len = 0;
    int link_bytes;

    len = bytes = in_esc = c = 0;
    while (1)
    {
        if (more_num >= more_size)
        {
              
            if(more_size == 0)
                next_size = read(fd, more_next, MORE_BUFSIZE);
            more_size = next_size;
            strncpy(more_buf,more_next,MORE_BUFSIZE);
            more_buf[MORE_BUFSIZE] = '\0';
            next_size = read(fd, more_next, MORE_BUFSIZE);
            if (more_size == 0)
                break;
            more_num = 0;
            if(islinkmore)
                more_num = islinkmore;
            islinkmore = 0;

        }
        ch = more_buf[more_num++];
        bytes++;
        if (ch == '\n' || bytes > 255)
            break;
        else if (ch == '\t')
        {
            c = 0;
            link_candi = 0;
            do
            {
                len++, *buf++ = ' ';
            }
            while ((len % 4) != 0);
        }
        else if (ch == '')
        {
            c = 0;
            link_candi = 0;
            if (showansi)
                *buf++ = ch;
            in_esc = 1;
        }
        else if (in_esc)
        {
            if (showansi)
                *buf++ = ch;
            if (strchr("[0123456789;,", ch) == NULL)
                in_esc = 0;
        }
        else if (isprint2(ch))
        {
            if (ch == '[')
            {
                link_candi ++;
                if (link_candi == 2)
                {
                    link_candi = 0;
                    link_bytes = 0;
                    link_len = parse_more_link(buf - 1, &link_bytes, linenum, len, buildlinks, NULL);
                    if (link_len > 0)
                    {
                        buf += link_len + 13;
                        len += link_len - 1;
                        bytes += link_bytes;
                        continue;
                    }
                    else if (link_len == -1)
                    {
                        *(buf - 2) = '[';
                        continue;
                    }
                }
            }
            else
                link_candi = 0;

            if (ch & 0x80)
                c++;
            else
                c = 0;
            if (len >= MORE_SCREEN_WIDTH)
            {
                if (c > 0 && c % 2 == 0)
                {
                    more_num -= 2;
                    *(buf - 1) = '\n';
                    *buf = '\0';
                    return bytes;
                }
                else
                {
                    more_num --;
                    *buf++ = '\n';
                    *buf = '\0';
                    return bytes;
                }

            }
            len++;
            *buf++ = ch;
        }
    }
    if (ch != '\n' && more_num > 0)
        more_num--;
    *buf++ = '\n';
    *buf = '\0';

    return bytes;
}

int seek_nth_line(int fd, int line_num)
{
    int line_count, viewed;

    lseek(fd, 0, SEEK_SET);
    more_num = more_size = 0;
    line_count = viewed = 0;
    while (line_count < line_num)
    {
        viewed += readln(fd, readln_buf, 0, 0);
        line_count ++;
    }
    return viewed;
}

char*
get_displaybuf(char *head, int topline, int currline, int *headpos, int headlen)
{
    if (search_mode)
    {
        char *start = head;
        int search_len = strlen(search_string);

        tempbuf[0] = '\0';
        if (*headpos > 0)
        {
           headlen += 12;
           sprintf(tempbuf, "[47;30m%s[0m", &search_string[search_len - *headpos]);
           start += *headpos;
        }    
        char *left = strstr(start, search_string);
        while (left)
        {
            *left = '\0';
            headlen += 12;
            sprintf(tempbuf, "%s%s[47;30m%s[0m", tempbuf, start, search_string);
            if (strlen(tempbuf) >= (size_t)headlen)
            {
                *headpos = strlen(tempbuf) - headlen;
                tempbuf[headlen - 4] = 0;
                sprintf(tempbuf, "%s[0m\n", tempbuf);
                return tempbuf;
            }    
            *left = search_string[0];
            start = left + search_len;
            left = strstr(start, search_string);
        }
        *headpos = 0;
        if (start){
            strncat(tempbuf, start, strlen(start));
        }    
        tempbuf[headlen] = '\n';
        tempbuf[headlen + 1] = 0;
        return tempbuf;
    }
    else if (more_links_count > 0 && topline + more_y == currline)
    {
        int i;
        char *startpos;
        struct more_link *link;
        
        for (i = 0; i< more_links_count; i++)
        {
            link = &more_links[i];
            if (topline + more_y == link->y && more_x + 1 >= link->start_x && more_x + 1 < link->end_x)
            {    
                startpos = more_get_strpos(head, link->start_x - 1);
                startpos[9] = '3';
                return head;
            }
        }
    }        

    return head;
}

int
display_more_screen(int fd, int topline)
{
    char buf2[80];
    int bottomline, currline, relative_y, numbytes, viewedbytes, secondnum;
    int search_len, headpos = 0;
    char *displaybuf;

    clear();
    move(0, 0);

    bottomline = topline + t_lines - 2 < maxline ? topline + t_lines - 2 : maxline;

    viewedbytes = seek_nth_line(fd, topline - 1);

    search_len = strlen(search_string);
    more_links_count = 0;

    numbytes = readln(fd, readln_buf, topline, 1);

    relative_y = 1;
    currline = topline + relative_y - 1;
    if (currline < bottomline)
        secondnum = readln(fd, second_buf, currline+1, 1);
    else
    {
        second_buf[0] = 0;
        secondnum = 0;
    }    
    while (numbytes != 0 && currline <= bottomline)
    {
        viewedbytes += numbytes;

#ifdef BELL_DELAY_FILTER
        bell_delay_filter(readln_buf);
#endif        
        if ((!titleshow && (!strncmp(readln_buf, "¡õ ÒýÓÃ", 7)))
            || (!strncmp(readln_buf, "==>", 3))
            || (!strncmp(readln_buf, "¡¾ ÔÚ", 5))
            || (!strncmp(readln_buf, "¡ù ÒýÊö", 7)))
        {
            prints("[1;33m%s[m", readln_buf);
            titleshow = YEA;
        }
        else
        {
            if (readln_buf[0] != ':' && readln_buf[0] != '>')
            {
                if (isin == YEA)
                {
                    isin = NA;
                    prints("[m");
                }
            }
            else
            {
                prints("[36m");
                isin = YEA;
            }
            if (search_mode)
            {
                numbytes = strlen(readln_buf);
                numbytes--;
                readln_buf[numbytes] = 0;
                strncat(readln_buf, second_buf, search_len - 1);
                readln_buf[numbytes + search_len - 1] = 0;
            }    
            displaybuf = get_displaybuf(readln_buf, topline, currline, &headpos, numbytes);
            if (check_stuffmode() || more_mode == MORE_MODE_STUFF)
                showstuff(displaybuf);
            else
                prints("%s", displaybuf);
        }

        strcpy(readln_buf, second_buf);
        numbytes = secondnum;
        relative_y ++;
        currline = topline + relative_y - 1;
        if (numbytes != 0 && currline < bottomline)
            secondnum = readln(fd, second_buf, currline+1, 1);
        else
        {
            second_buf[0] = 0;
            secondnum = 0;
        }    
    }

    move(t_lines - 1, 0);
    clrtoeol();
    if (maxline >= t_lines)
    {
        sprintf(buf2, "(%d%%) %d-%dÐÐ", (viewedbytes * 100) / maxbytes,
                topline, bottomline);
        prints("[0;1;44;32m%s%-18s[33m| ½áÊø¡û ÒÆ¶¯¡ü¡ýPgUp/Dn Home,End Ìø×ª[ ] ÇóÖúh |  [m",
               ((bottomline < maxline) && !(viewedbytes / maxbytes)) ? "ÏÂÃæ»¹ÓÐà¸" : "ÏÂÃæÃ»ÓÐÁË", buf2);
    }
    else if (!promptend)
    {
        if (more_mode == MORE_MODE_SREAD)
            prints("[0;1;44;31m[Ö÷ÌâÔÄ¶Á] [33mÏÂÒ»·â <Space>,<Enter>,¡ý©¦ÉÏÒ»·â ¡ü,U                              [m");
        else if (more_mode == MORE_MODE_AUSER)
            prints("[0;1;44;31m[ÏàÍ¬×÷Õß] [33mÏÂÒ»·â <Space>,<Enter>,¡ý©¦ÉÏÒ»·â ¡ü,U                              [m");
        else if (more_mode == MORE_MODE_POST)
            prints("[0;1;44;31m[ÔÄ¶ÁÎÄÕÂ]  [33m»ØÐÅ R ©¦ ½áÊø Q,¡û ©¦ÉÏÒ»·â ¡ü©¦ÏÂÒ»·â <Space>,¡ý©¦Ö÷ÌâÔÄ¶Á ^X»òp [m");
        else if (more_mode == MORE_MODE_MAIL)
            prints("(R)»ØÐÅ, (D)É¾³ý, (G)¼ÌÐø? [G]: ");
        else if (more_mode == MORE_MODE_DIGEST)
            prints("[0;1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ý [m");

    }

    return 1;
}

int
leave_more(int fd, int ch)
{
    close(fd);
    if (promptend)
        pressanykey();
    return ch;
}

int
search_result(int fd, int topline, int more_x, int more_y, int *new_topline, int *new_x, int *new_y)
{
    int startline, currline;
    int searchlen, readlen;
    char *startpos;
    char *first_appear;

    searchlen = strlen(search_string);
    startline = topline + more_y;
    startpos = 0;
    if (startline > maxline)
        startline = maxline;
    seek_nth_line(fd, startline - 1);
    readln(fd, readln_buf, 0, 0);
    if (startline < maxline )
    {
       readln(fd, second_buf, 0, 0);
       readlen = strlen(readln_buf) - 1;
       readln_buf[readlen] = 0;
       strncat(readln_buf, second_buf, searchlen - 1);
       readln_buf[readlen + searchlen - 1] = 0;
    }    
    else
    {
       seek_nth_line(fd, 0);
       readln(fd, second_buf, 0, 0);
    }    
    currline = startline;
    startpos = more_get_strpos(readln_buf, more_x) + 2;
    do
    {
        first_appear = strstr(startpos, search_string);
        if (first_appear != NULL)
        {
            if (maxline >= t_lines)
            {
                if (currline <= maxline - t_lines + 2)
                {
                    *new_topline = currline;
                    *new_y = 0;
                    *new_x = more_get_display_x(readln_buf, first_appear);
                    return 1;
                }
                else
                {
                    *new_topline = maxline - t_lines + 2;
                    *new_y = currline - *new_topline;
                    *new_x = more_get_display_x(readln_buf, first_appear);
                    return 1;
                }
            }
            else
            {
                *new_topline = 1;
                *new_y = currline - 1;
                *new_x = more_get_display_x(readln_buf, first_appear);
                return 1;
            }
        }
        strcpy(readln_buf, second_buf);
        startpos = readln_buf;
        currline ++;
        if (currline >= maxline )
        {
           seek_nth_line(fd, 0);
           readln(fd, second_buf, 0, 0);
        }    
        else
        {
           readln(fd, second_buf, 0, 0);
           readlen = strlen(readln_buf) - 1;
           readln_buf[readlen] = 0;
           strncat(readln_buf, second_buf, searchlen - 1);
           readln_buf[readlen + searchlen - 1] = 0;
        }    
        if (currline > maxline){
            currline = 0;
        }    
    }while (startline != currline);

    return 0;
}

void
init_rawmore(int fd)
{
    struct stat st;

    if (cursormode_def == -1)
        cursormode_def = !DEFINE(DEF_MORENOCURSOR);

    more_size = more_num = 0;
    isin = titleshow = NA;
    more_links_count = 0;

    search_mode = 0;
    search_string[0] = '\0';

    lseek(fd, 0, SEEK_SET);

    fstat(fd, &st);
    maxbytes = st.st_size;

    maxline = 0;
    while (readln(fd, readln_buf, maxline + 1, 0) > 0)
        maxline++;

    if ((more_mode == MORE_MODE_POST) || (more_mode == MORE_MODE_SREAD)
        || (more_mode == MORE_MODE_MAIL) || (more_mode == MORE_MODE_WIKI)
        || (more_mode == MORE_MODE_READING) || (more_mode == MORE_MODE_DIGEST))
        is_reading = 1;
    else
        is_reading = 0;

}
static int validate_path(char *path)
{
    if(path[0]=='/')
    {
        return 0;
    }
    if(strstr(path,".."))
    {
        return 0;
    }
    return 1;
}

int
goto_link(int fd, int y, int x)
{
    int i;
    int has_link = 0;
    struct more_link *link;
    char *pos;
    char dest[150];
    struct fileheader fh;
    int allowed = 1;
    struct shortfile *bp;
    int curr_more_mode, curr_promptend;
    curr_more_mode = more_mode;
    curr_promptend = promptend;

    for (i = 0; i< more_links_count; i++)
    {
        link = &more_links[i];
        if (y == link->y && x >= link->start_x && x < link->end_x)
        {
            has_link = 1;
            if (strlen(link->dest) > 150)
                break;
            strcpy(dest, link->dest);
            switch (link->type)
            {
            case MORE_LINK_USER:
                uinfo.mode = LUSERS;
                clear();
                t_query(dest);
                uinfo.mode = READING;
                pressanykey();
                break;
            case MORE_LINK_POST:
                pos = strchr(dest, ',');
                if (pos)
                {
                    *pos++ = '\0';

                    if( (bp=getbcache(dest)) == NULL )
                    {
                        allowed = 0;
                        break;
                    }

                    if ((!HAS_PERM(PERM_SYSOP)) || (!strncmp(dest, "Arbitrator", 10))
                        || (!strncmp(dest, "OscarAward", 10)))
                    {
                        setbfile(genbuf, dest, "board.allow");
                        if (dashf(genbuf) && !seek_in_file(genbuf, currentuser.userid))
                        {
                            allowed = 0;    
                            break;
                        }
                    }

                    setbfile(genbuf, dest, DOT_DIR);
                    if (search_record(genbuf, &fh, sizeof(fh), cmpdigestfilename, pos) <= 0)
                    {
                        setbfile(genbuf, dest, RETURN_DIR);
                        if (search_record(genbuf, &fh, sizeof(fh), cmpdigestfilename, pos) <= 0)
                        {
                            allowed = 0;    
                            break;
                        }

                        if (strcmp(fh.owner, currentuser.userid) != 0)
                        {
                            if (strncmp(fh.owner, "Anony.", 6)
                                 || strcmp(fh.owner, get_anonyid(currentuser.userid)))
                            {
                                if (!chk_currBM(bp->BM) && !HAS_PERM(PERM_SPECIAL7))
                                {
                                    allowed = 0;
                                    break;
                                }
                            }
                        }
                    }

                    setbfile(genbuf, dest, fh.filename);
                    ansimore_withmode(genbuf, 0, MORE_MODE_POST);
                    more_mode = curr_more_mode;
                    promptend = curr_promptend;
                    init_rawmore(fd);
                }
                break;
            case MORE_LINK_ANNOUNCE:
                {
                    char found[STRLEN];
                    pos = strchr(dest, '/');
                    if(pos)
                    {
                        *(pos ++) = '\0';
                        if (a_menusearch("0Announce", dest, found))
                        {
                            sprintf(genbuf, "%s/%s", found, pos);
                            if(!validate_path(genbuf))
                            {
                                allowed = 0;
                                break;
                            }
                            if (dashf(genbuf))
                            {
                                ansimore_withmode(genbuf, 0, MORE_MODE_POST);
                                more_mode = curr_more_mode;
                                promptend = curr_promptend;
                                init_rawmore(fd);
                            }
                        }
                    }
                }
                break;
            case MORE_LINK_ANNOUNCEDIR:
                {
                    char found[STRLEN];
                    pos = strchr(dest, '/');
                    if(pos)
                    {
                        *(pos ++) = '\0';
                        if (a_menusearch("0Announce", dest, found))
                        {
                            sprintf(genbuf, "%s/%s", found, pos);
                            if(genbuf[strlen(genbuf)] == '/')
                                genbuf[strlen(genbuf)-1] = '\0';
                            else
                                genbuf[strlen(genbuf)] = '\0';
                            if(!validate_path(genbuf))
                            {
                                allowed = 0;
                                break;
                            }
                            if(dashd(genbuf))
                            {
                                a_menu("", genbuf, "", 
                               (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_OBOARDS) ? PERM_BOARDS : 0), 0, 0);
                               clear();
                                more_mode = curr_more_mode;
                                promptend = curr_promptend;
                                init_rawmore(fd);
                            }
                        }
                    }
                }
                break;        
                       
            case MORE_LINK_WIKI_ARCHIVE:
                sprintf(genbuf, "wikitext/%s", dest);
                if(!validate_path(genbuf))
                {
                    allowed = 0;
                    break;
                }
                if (dashf(genbuf))
                {    
                    ansimore_withmode(genbuf, 0, MORE_MODE_WIKI);
                    more_mode = curr_more_mode;
                    promptend = curr_promptend;
                    init_rawmore(fd);
                }
                else
                {
                    allowed = 0;
                    break;
                }
                break;
            case MORE_LINK_URL:
                link->unfold = !link->unfold;
                break;
            }
            break;
        }
    }
    if (!allowed)
    {
        clear();
        move(10, 20);
        prints("´íÎó£º¸ÃÒ³Ãæ²»´æÔÚ»òÃ»ÓÐÈ¨ÏÞ·ÃÎÊ¸ÃÒ³Ãæ");
        move(11, 20);
        prints("%s", dest);
        pressanykey();
    }
    return has_link;
}

int
recalc_more_ypos(int *more_y, int *topline, int new_absolute_y)
{
    if (maxline < t_lines)
    {
        *more_y = new_absolute_y;
        return 1;
    }
    else
    {
        if (new_absolute_y >= *topline - 1 && new_absolute_y < *topline + t_lines - 2)
        {
            *more_y = new_absolute_y - *topline + 1;
            return 1;
        }
        else
        {
            if (new_absolute_y < maxline - t_lines + 2)
            {
                *topline = new_absolute_y + 1;
                *more_y = new_absolute_y - *topline + 1;
            }
            else
            {
                *topline = maxline - t_lines + 2;
                *more_y = new_absolute_y - *topline + 1;
            }
            //printf("\n\ntopline = %d, more_y = %d\t\t\n\n", *topline, *more_y);
            //exit(0);
            return 1;
        }
    }
}

int
rawmore(char *filename, int row, int numlines)
{
    int fd;
    int ch;
    int topline, bottomline, jumpto;
    int is_shortpost;
    char str_jumpto[6];
    int cursormode, cursorchanged;
    int new_y, new_x, new_topline;
    char input_string[42];
    int linknum;
    
    clrtobot();
        
    if ((fd = open(filename, O_RDONLY)) == -1)
    {
        return -1;
    }

    init_rawmore(fd);

    linknum = -1;

    if (numlines != 0)
        maxline = numlines;

    if (maxline < t_lines)
        is_shortpost = 1;
    else
        is_shortpost = 0;    

    topline = row + 1;
    display_more_screen(fd, topline);
    if (maxline < t_lines)
    {
        if (promptend)
            return leave_more(fd, 0);
        if (!is_reading)
            return leave_more(fd, 0);
    }
    cursormode = cursormode_def && is_reading;
    if (cursormode)
    {
        if (is_shortpost && more_y > maxline - 1)
            more_y = maxline - 1;
        move(more_y, more_x);
    }
    ch = egetch();
	if((more_mode == MORE_MODE_MAIL || more_mode == MORE_MODE_DIGEST) && !more_links_count && is_shortpost)
		return leave_more(fd, ch);

    while (1)
    {
        cursormode = cursormode_def && is_reading;
        bottomline = topline + t_lines - 2;
        cursorchanged = 0;
        switch (ch)
        {
        case 'k':
        case KEY_UP:
            if (cursormode)
            {    
                more_y --;
                cursorchanged = 1;
            }
            else
            {
                if (topline <= 1)
                    return leave_more(fd, KEY_UP);
                linknum = -1;
                topline --;
                display_more_screen(fd, topline);
            }
            break;
        case '\r':
        case '\n':
        case KEY_DOWN:
        case 'j':
            if ((ch == '\r' || ch == '\n') && goto_link(fd, topline + more_y, more_x + 1))
            {
                display_more_screen(fd, topline);
                move(more_y, more_x);
                break;
            }
            if (cursormode)
            {    
                more_y ++;
                cursorchanged = 1;
            }
            else
            {
                linknum = -1;
                if (bottomline >= maxline || is_shortpost)
                    return leave_more(fd, ch);
                topline ++;
                display_more_screen(fd, topline);
            }
            break;
        case KEY_LEFT:
            if (cursormode)
            {    
                more_x --;
                cursorchanged = 1;
            }
            else
            {
                if (!is_shortpost)
                    ch = 0;
                return leave_more(fd, ch);
            }
            break;
        case KEY_RIGHT:
            if (cursormode)
            {    
                more_x ++;
                cursorchanged = 1;
            }
            else
            {
                if (bottomline >= maxline)
                    return leave_more(fd, ch);
                linknum = -1;
                topline += t_lines - 2;
                if (topline > maxline - t_lines + 2)
                    topline = maxline - t_lines + 2;
                display_more_screen(fd, topline);
            }
            break;
        case KEY_PGUP:
        case Ctrl('B'):
            if (cursormode)
            {    
                if (is_shortpost)
                    more_y = 0;
                else
                {
                    topline -= t_lines - 2;
                    if (topline < 1)
                    {    
                        topline = 1;
                        more_y = 0;
                    }
                    linknum = -1;
                    display_more_screen(fd, topline);
                }
            }
            else
            {
                if (topline <= 1)
                    return leave_more(fd, KEY_PGUP);
                topline -= t_lines - 2;
                if (topline < 1)
                {    
                    topline = 1;
                    more_y = 1;
                }
                linknum = -1;
                display_more_screen(fd, topline);
            }
            break;
        case KEY_PGDN:
        case Ctrl('F'):
        case ' ':
        case 'a':
            if (cursormode)
            {    
                if (is_shortpost)
                    more_y = maxline - 1;
                else
                {
                    topline += t_lines - 2;
                    if (topline > maxline - t_lines + 2)
                    {    
                        topline = maxline - t_lines + 2;
                        more_y = t_lines - 2;
                    }
                    linknum = -1;
                    display_more_screen(fd, topline);
                }
            }
            else
            {
                if (bottomline >= maxline)
                    return leave_more(fd, KEY_RIGHT);
                topline += t_lines - 2;
                if (topline > maxline - t_lines + 2)
                    topline = maxline - t_lines + 2;
                linknum = -1;
                display_more_screen(fd, topline);
            }
            break;
        case KEY_HOME:
            if (cursormode)
                more_x = 0;
            else
            {
                if (!is_shortpost || topline != 1)
                {
                    topline = 1;
                    linknum = -1;
                    display_more_screen(fd, topline);
                }
            }
            break;
        case KEY_END:
            if (cursormode)
                more_x = MORE_SCREEN_WIDTH - 1;
            else
            {
                if (is_shortpost)
                    return leave_more(fd, KEY_LEFT);
                if (!is_shortpost && bottomline != maxline)
                {
                    topline = maxline - t_lines + 2;
                    linknum = -1;
                    display_more_screen(fd, topline);
                }
            }
            break;
        case 'h':
        case 'H':
        case '?':
            show_help("help/morehelp");
            display_more_screen(fd, topline);
            break;
        case '/':
            if (strlen(search_string) > 0)
                sprintf(genbuf, "ËÑÑ°×Ö´® [%s]: ", search_string);
            else
                sprintf(genbuf, "ËÑÑ°×Ö´® : ");
            getdata(t_lines - 1, 0, genbuf, input_string, 40, DOECHO, YEA);
            if (strlen(input_string) > 1)
                strcpy(search_string, input_string);
            if (strlen(search_string) > 0)
            {    
                search_mode = 1;
                if (search_result(fd, topline, more_x, more_y, &new_topline, &new_x, &new_y))
                {
                    topline = new_topline;
                    more_y = new_y;
                    more_x = new_x;
                    display_more_screen(fd, topline);

                    if (!cursormode)
                    {
                        move(new_y, new_x);
                    }
                }
                else
                    display_more_screen(fd, topline);
            }
            else
            {
                display_more_screen(fd, topline);
            }
            break;
        case '\\':
            if (more_links_count > 0)
            {
                show_fastjump = 1;
                display_more_screen(fd,topline);
                show_fastjump = 0;
                getdata(t_lines - 1, 0, "¿ìËÙÌø×ªµ½µÚ¼¸¸öÁ´½Ó£º", str_jumpto, 6, DOECHO, YEA);
                jumpto = atoi(str_jumpto)-1;
                if(jumpto >= 0&&jumpto < more_links_count)
                {
                    linknum = jumpto;
                    more_x = more_links[linknum].start_x - 1;
                    if (recalc_more_ypos(&more_y, &topline, more_links[linknum].y - 1))
                        display_more_screen(fd, topline);
                    move(more_y, more_x);
            
                }
                else
                    display_more_screen(fd, topline);
            }
            break;
        case 'g':
            if (!is_shortpost || cursormode)
            {
                getdata(t_lines - 1, 0, "Ìø×ªµ½µÚ¼¸ÐÐ£º", str_jumpto, 6, DOECHO, YEA);
                jumpto = atoi(str_jumpto);
                if (jumpto > 0)
                {
                    if (jumpto > maxline - 1)
                        jumpto = maxline - 1;
                    topline = jumpto;
                    if (topline > maxline - t_lines + 2)
                        topline = maxline - t_lines + 2;
                    if (topline < 1)
                        topline = 1;
                    more_y = jumpto - topline;
                    linknum = -1;
                    display_more_screen(fd, topline);
                    move(more_y, more_x);
                }
                else
                    linknum = -1;
                    display_more_screen(fd, topline);
            }
            break;
        case KEY_TAB:
        case 'n':
            if (more_links_count > 0)
            {
                linknum = (linknum + 1) % more_links_count;
                more_x = more_links[linknum].start_x - 1;
                if (recalc_more_ypos(&more_y, &topline, more_links[linknum].y - 1))
                    display_more_screen(fd, topline);
                move(more_y, more_x);
            }
            break;
        case 'b':
        case 'N':
            if (more_links_count > 0)
            {
                linknum = (linknum + more_links_count - 1) % more_links_count;
                more_x = more_links[linknum].start_x - 1;
                if (recalc_more_ypos(&more_y, &topline, more_links[linknum].y - 1))
                    display_more_screen(fd, topline);
                move(more_y, more_x);
            }
            break;
        case '.':
            if (cursormode_def)
            {
                cursormode_def = 0;
                cursormode = 0;
                move(t_lines - 1, MORE_SCREEN_WIDTH);
            }
            else
            {
                cursormode_def = 1;
                cursormode = is_reading;
            }
            break;
        case 'Y':
        case 'R':
        case 'x':
        case 'X':
        case 'y':
        case 'r':
        case Ctrl('R'):
        case Ctrl('U'):
        case Ctrl('N'):
        case Ctrl('S'):
        case Ctrl('X'):
        case 'p':
        case 'P':
        case Ctrl('A'):
        case 'S':
        case '[':
        case ']':
        case 'd':
        case 'D':
            return leave_more(fd, ch);
            break;
        case 'q':
        case 'Q':
        case EOF:
            if (!is_shortpost)
                return leave_more(fd, 0);
            else
                return leave_more(fd, KEY_LEFT);
            break;
        }
        // deal with cursor
        if (cursormode)
        {
            if (cursorchanged)
            {
                if (more_x > MORE_SCREEN_WIDTH - 1)
                {
                    if ((!is_shortpost && more_y == t_lines - 2 && bottomline == maxline)
                        || (is_shortpost && more_y == maxline - 1))
                        more_x --;
                    else
                    {
                        more_x = 0;
                        more_y ++;
                    }
                }
                else if (more_x < 0)
                {
                    if (more_y == 0 && topline == 1)
                        more_x ++;
                    else
                    {
                        more_x = MORE_SCREEN_WIDTH - 1;
                        more_y --;
                    }
                }

                if (!is_shortpost)
                {
                    if (more_y >= t_lines - 2)
                    {
                        more_y = t_lines - 2;
                        if (topline < maxline - t_lines + 2)
                            topline ++;
                        display_more_screen(fd, topline);
                    }
                    else if (more_y < 0)
                    {
                        more_y = 0;
                        if (topline > 1)
                            topline --;
                        display_more_screen(fd, topline);
                    }
                }
                else
                {
                    if (more_y > maxline - 1)
                        more_y = maxline - 1;
                    else if (more_y < 0)
                        more_y = 0;
                }
            }

            move(more_y, more_x);
        }
        ch = egetch();
    }
    return ch;
}

int ansimore_withmode(char *filename, int promptend_in, int more_mode_in)
{
    int ch;

    more_mode = more_mode_in;
    promptend = promptend_in;
    clear();
    ch = rawmore(filename, 0, 0);
    move(t_lines - 1, 0);
    clrtoeol();
    refresh();
    return ch;
}

int ansimore(char *filename, int promptend_in)
{
    return ansimore_withmode(filename, promptend_in, MORE_MODE_COMMON);
}

int ansimore_partial(char *filename, int promptend_in, int row, int numlines)
{
    int ch;
    
    more_mode = MORE_MODE_COMMON;
    promptend = promptend_in;
    ch = rawmore(filename, row, numlines);
    move(t_lines - 1, 0);
    clrtoeol();
    refresh();
    return ch;
}

int mesgmore(char *filename, int promptend)
{
    int ch = ansimore_withmode(filename, 0, MORE_MODE_MESSAGE);

    if (promptend == YEA || maxline < t_lines)
        msgmorebar(filename);
    return ch;
}

