#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <setjmp.h>
#include <glib.h>

#define FWEB_STDIO_DEFINES
#include "fwebio.h"
#undef FWEB_STDIO_DEFINES

#include "bbs.h"
#include "shm.h"
#include "kernel.h"
#include "const.h"
#include "cgic.h"
#include "mempool.h"
#include "fweb.h"
#include "module.h"

char seccode[SECNUM][5] = {
    "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw", "x"
};

#define MAX_MSG_SIZE 1024

char secname[SECNUM][2][20] = {
    {"BBS ÏµÍ³", "[±¾Õ¾][Õ¾ÄÚ][BBS]"},
    {"ÉÏº£½»´ó", "[±¾Ð£]"},
    {"Ñ§×ÓÔºÐ£", "[¸ßÐ£][µØÇø]"},
    {"µçÄÔ¼¼Êõ", "[µçÄÔ]"},
    {"Ñ§Êõ¿ÆÑ§", "[¿ÆÑ§]"},
    {"ÒÕÊõÎÄ»¯", "[ÎÄÒÕ][ÓïÑÔ]"},
    {"ÌåÓýÔË¶¯", "[ÌåÓý][ÔË¶¯]"},
    {"ÐÝÏÐÓéÀÖ", "[ÐÝÏÐ][ÒôÀÖ][ÓéÀÖ]"},
    {"ÖªÐÔ¸ÐÐÔ", "[ÖªÐÔ] [¸ÐÐÔ]"},
    {"Éç»áÐÅÏ¢", "[Éç»á][ÐÅÏ¢][¿¼ÊÔ]"},
    {"ÉçÍÅÈºÌå", "[ÉçÍÅ][ÈºÌå]"},
    {"ÓÎÏ·×¨Çø", "[ÓÎÏ·]"}
};

int loginok = 0;
int pic_mod;

struct userec currentuser;
struct user_info *u_info;

char fromhost[256];
char datestring[30];

int my_t_lines = 20;
int my_link_mode = 0;
int my_def_mode = 0;
int my_font_mode = 0;
int my_fontn_mode = 1;
int my_ansisize_mode = 1;
int my_ansifont_mode = 0;
int my_theme_mode = 1;
int silence = 0;
int font_tag_started = 0;

int file_has_word(char *file, char *word)
{
    FILE *fp;
    char buf[256], buf2[256];

    fp = fopen(file, "r");
    if (fp == 0)
        return 0;
    while (1)
    {
        bzero(buf, 256);
        if (fgets(buf, 255, fp) == 0)
            break;
        sscanf(buf, "%s", buf2);
        if (!strcasecmp(buf2, word))
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int f_append(char *file, char *buf)
{
    FILE *fp;

    fp = fopen(file, "a");
    if (fp == 0)
        return 1;
    fprintf(fp, "%s", buf);
    fclose(fp);
    return 0;
}

struct stat *f_stat(char *file)
{
    static struct stat buf;

    bzero(&buf, sizeof(buf));
    if (stat(file, &buf) == -1)
        bzero(&buf, sizeof(buf));
    return &buf;
}

int file_size(char *file)
{
    return f_stat(file)->st_size;
}

int file_rtime(char *file)
{
    return f_stat(file)->st_atime;
}

int file_exist(char *file)
{
    return file_time(file) != 0;
}

int file_isdir(char *file)
{
    return (f_stat(file)->st_mode & S_IFDIR) != 0;
}

int file_isfile(char *file)
{
    return (f_stat(file)->st_mode & S_IFREG) != 0;
}

int file_time(char *file)
{
    return f_stat(file)->st_mtime;
}

int get_record(void *buf, int size, int num, char *file)
{
    FILE *fp;

    if (size < 1 || size > 4096)
        return 0;
    if (num < 0 || num > 1000000)
        return 0;
    bzero(buf, size);
    fp = fopen(file, "r");
    if (fp == 0)
        return 0;
    fseek(fp, num * size, SEEK_SET);
    if (fread(buf, size, 1, fp) <= 0)
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

int put_record(void *buf, int size, int num, char *file)
{
    FILE *fp;

    if (size < 1 || size > 4096)
        return 0;
    if (num < 0 || num > 1000000)
        return 0;
    fp = fopen(file, "r+");
    if (fp == 0)
        return 0;
    fseek(fp, num * size, SEEK_SET);
    fwrite(buf, size, 1, fp);
    fclose(fp);
    return 1;
}

int del_record(char *file, int size, int num)
{
    FILE *fp;
    int total, i;
    char buf[4096];

    if (size < 1 || size > 4096)
        return 0;
    total = file_size(file) / size;
    if (total < 1 || total > 1000000)
        return 0;
    fp = fopen(file, "r+");
    if (fp == 0)
        return 0;
    flock(fileno(fp), LOCK_EX);
    for (i = num + 1; i <= total - 1; i++)
    {
        fseek(fp, i * size, SEEK_SET);
        if (fread(buf, size, 1, fp) <= 0)
            break;
        fseek(fp, (i - 1) * size, SEEK_SET);
        fwrite(buf, size, 1, fp);
    }
    ftruncate(fileno(fp), (total - 1) * size);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    return 1;
}

char *Ctime(time_t t)
{
    static char s[80];

    sprintf(s, "%24.24s", ctime(&t));
    return s;
}

char *noansi(char *s)
{
    static char buf[1024];
    int i = 0, mode = 0;

    while (s[0] && i < 1023)
    {
        if (mode == 0)
        {
            if (s[0] == 27)
            {
                mode = 1;
            }
            else
            {
                buf[i] = s[0];
                i++;
            }
        }
        else
        {
            if (!strchr(";[0123456789", s[0]))
                mode = 0;
        }
        s++;
    }
    buf[i] = 0;
    return buf;
}

char *nohtml(char *s)
{
    static char buf[1024];
    int i = 0, mode = 0;

    while (s[0] && i < 1023)
    {
        if (mode == 0)
        {
            if (s[0] == '<')
            {
                mode = 1;
            }
            else
            {
                buf[i] = s[0];
                i++;
            }
        }
        else
        {
            if (s[0] == '>')
                mode = 0;
        }
        s++;
    }
    buf[i] = 0;
    return buf;
}

char *strright(char *s, int len)
{
    int l = strlen(s);

    if (len <= 0)
        return "";
    if (len >= l)
        return s;
    return s + (l - len);
}

char *strcasestr(const char *s1, const char *s2)
{
    int l;

    l = strlen(s2);
    while (s1[0])
    {
        if (!strncasecmp(s1, s2, l))
            return (char *) s1;
        s1++;
    }
    return 0;
}

int strsncpy(char *s1, char *s2, int n)
{
    int l = strlen(s2);

    if (n < 0)
        return 1;
    if (n > l + 1)
        n = l + 1;
    strncpy(s1, s2, n - 1);
    s1[n - 1] = 0;
    return 0;
}

char *ltrim(char *s)
{
    char *s2 = s;

    if (s[0] == 0)
        return s;
    while (s2[0] && strchr(" \t\r\n", s2[0]))
        s2++;
    return s2;
}

char *rtrim(char *s)
{
    static char t[1024], *t2;

    if (s[0] == 0)
        return s;
    strsncpy(t, s, 1024);
    t2 = t + strlen(s) - 1;
    while (strchr(" \t\r\n", t2[0]) && t2 > t)
        t2--;
    t2[1] = 0;
    return t;
}

char *trim(char *s)
{
    return ltrim(rtrim(s));
}

char *getsenv(char *s)
{
    char *t = getenv(s);

    if (t)
        return t;
    return "";
}

int strnncpy(char *s, int *l, char *s2)
{
    strncpy(s + (*l), s2, strlen(s2));
    (*l) += strlen(s2);
    return 0;
}

int hsprintf(char *s, char *fmt, ...)
{
    char buf[1024], ansibuf[80], buf2[80];
    char *tmp;
    int c, bold, m, i, l, len;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, 1023, fmt, ap);
    va_end(ap);
    s[0] = 0;
    l = strlen(buf);
    len = 0;
    bold = 0;
    for (i = 0; i < l; i++)
    {
        c = buf[i];
        if (c == '&')
        {
            if (i < l-1 && buf[i+1] == '#' ){
                    strnncpy(s, &len, "&");
            }
            else{
                    strnncpy(s, &len, "&amp;");
            }
        }
        else if (c == '<')
        {
            strnncpy(s, &len, "&lt;");
        }
        else if (c == '>')
        {
            strnncpy(s, &len, "&gt;");
        }
        else if (c == 27)
        {
            if (buf[i + 1] != '[')
                continue;
            for (m = i + 2; m < l && m < i + 24; m++)
                if (strchr("0123456789;", buf[m]) == 0)
                    break;
            strsncpy(ansibuf, &buf[i + 2], m - (i + 2) + 1);
            i = m;
            if (buf[i] != 'm')
                continue;
            if (strlen(ansibuf) == 0)
            {
                bold = 0;
                if (font_tag_started)
                    strnncpy(s, &len, "</font><font class='c37'>");
                else
                {    
                    strnncpy(s, &len, "<font class='c37'>");
                    font_tag_started = 1;
                }
            }
            tmp = strtok(ansibuf, ";");
            while (tmp)
            {
                c = atoi(tmp);
                tmp = strtok(0, ";");
                if (c == 0)
                {
                    if (font_tag_started)
                        strnncpy(s, &len, "</font><font class='c37'>");
                    else
                    {    
                        strnncpy(s, &len, "<font class='c37'>");
                        font_tag_started = 1;
                    }
                    bold = 0;
                }
                if (c >= 30 && c <= 37)
                {
                    if (bold == 1)
                    {
                        if (font_tag_started)
                            sprintf(buf2, "</font><font class='d%d'>", c);
                        else
                        {
                            sprintf(buf2, "<font class='d%d'>", c);
                            font_tag_started = 1;
                        }
                    }
                    if (bold == 0)
                    {    
                        if (font_tag_started)
                            sprintf(buf2, "</font><font class='c%d'>", c);
                        else
                        {
                            sprintf(buf2, "<font class='c%d'>", c);
                            font_tag_started = 1;
                        }
                    }
                    strnncpy(s, &len, buf2);
                }
            }
        }
        else
        {
            s[len] = c;
            len++;
        }
    }
    s[len] = 0;
    return 0;
}

int hprintf(char *fmt, ...)
{
    char buf[8096], buf2[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf2, 1023, fmt, ap);
    va_end(ap);
    hsprintf(buf, "%s", buf2);
    printf("%s", buf);
    return 0;
}

int hfprintf(FILE * stream, char *fmt, ...)
{
    char buf[8096], buf2[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf2, 1023, fmt, ap);
    va_end(ap);
    hsprintf(buf, "%s", buf2);
    fprintf(stream, "%s", buf);
    return 0;
}

int hhprintf(char *fmt, ...)
{
    char buf0[1024], buf[1024], *s, *getparm(char *var);
    int len = 0;
    int my_link_mode;
    va_list ap;


    va_start(ap, fmt);
    vsnprintf(buf, 1023, fmt, ap);
    va_end(ap);
    buf[1023] = 0;
    s = buf;
    my_link_mode = atoi(getparm("my_link_mode"));
    if (my_link_mode == 1)
        return hprintf("%s", buf);
    if (!strcasestr(s, "http://") && !strcasestr(s, "https://") &&!strcasestr(s, "ftp://")
        && !strcasestr(s, "mailto:") && !strstr(s, "[["))
        return hprintf("%s", buf);
    while (s[0])
    {
        if (!strncasecmp(s, "https://", 8)
            || !strncasecmp(s, "http://", 7)
            || !strncasecmp(s, "mailto:", 7)
            || !strncasecmp(s, "ftp://", 6))
        {
            char *tmp, *url;

            if (len > 0)
            {
                buf0[len] = 0;
                hprintf("%s", buf0);
                len = 0;
            }
            tmp = strtok(s, "\'\" \r\t)(;\n");
            if (tmp == 0)
            {
                return hprintf("%s",s);
            }
            url = tmp;
            if (!strncmp
                (UPLOAD_FILE_URL_HEADER, tmp,
                 strlen(UPLOAD_FILE_URL_HEADER)))
            {
                url += strlen("http://" BBSHOST);
            }
            if (pic_mod == 2
                &&
                (strcasestr(tmp, ".gif")
                 || strcasestr(tmp, ".jpg")
                 || strcasestr(tmp, ".png") || strcasestr(tmp, ".jfif")))
            {

                printf
                    ("<IMG SRC=\"%s\" onload=\"if(this.width > screen.width - 200){this.width = screen.width - 200}\"><br/>\n",
                     nohtml(url));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return 0;
                return hhprintf(tmp);
            }

            if (pic_mod == 2 && strcasestr(tmp, ".swf"))
            {
                printf
                    ("<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400 AllowScriptAccess=\"never\"></EMBED><br/>\n",
                     nohtml(url));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return 0;
                return hhprintf("%s", tmp);
            }
            if (pic_mod == 1
                &&
                (strcasestr(tmp, ".gif")
                 || strcasestr(tmp, ".jpg")
                 || strcasestr(tmp, ".png") || strcasestr(tmp, ".jfif")))
            {

                printf
                    ("<div style=\"height: 100px\"><IMG class=\"QmdImg\" SRC=\"%s\" style=\"max-height: 100%%; max-width: 100%%\"></div>\n",
                     nohtml(url));
                    tmp = strtok(0, "");
                    pic_mod = 0;
                    if (tmp == 0)
                        return 0;
                    return hhprintf(tmp);
	    }
            printf("<a target=_blank href='%s'>", nohtml(url));
            printf("%s</a>", nohtml4(tmp));
            tmp = strtok(0, "");
            if (tmp == 0)
                return printf("\n");
            return hhprintf("%s", tmp);
        }
        else if (s[0] == '[' && s[1] == '[')
        {
            char *linktype, *endpos, *pipepos, *dest, *display, *filename;

            if (len>0 && buf0[len-1]=='#')
            {
                buf0[len - 1] = '[';
                buf0[len] = '[';
                buf0[len + 1] = '\0';
                hprintf("%s", buf0);
                len = 0;
                return hhprintf("%s", s + 2);
            }

            if (len > 0)
            {
                buf0[len] = 0;
                hprintf("%s", buf0);
                len = 0;
            }

            linktype = s + 2;
            endpos = strstr(linktype, "]]");
            if (endpos == NULL || endpos - linktype > STRLEN * 2 - 2)
            {
                /* no ending ']]', print it */
                return hprintf("%s",s);
            }

            *endpos++ = '\0';

            dest = strchr(linktype, ':');
            if (dest == NULL || dest[1] == '\0')
            {
                /* format error, print it  */
                endpos[-1] = ']';
                return hprintf("%s", s);
            }

            *dest++ = '\0';            

            pipepos = strchr(dest, '|');
            if (pipepos != NULL)
            {
                *pipepos = '\0';
                display = pipepos + 1;        
                if(dest[0]=='\0' || display[0]=='\0')
                {
                    goto FMT_ERR;
                }
            }
            else
            {
                display = dest;
            }
		
            if (!strcmp(linktype, "User"))
            {    
                printf("<a target='_self' href='/bbsqry?userid=%s'>", url_quote(dest)); 
                hprintf("%s", nohtml4(display));
                printf("</a>");
            }
            else if (!strcmp(linktype, "Post"))
            {
                filename = strchr(dest, ',');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    printf("<a target='_self' href='/bbscon?board=%s&file=", url_quote(dest));
                    printf("%s'>", url_quote(filename));
                    hprintf("%s", nohtml4(display));
                    printf("</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }
            else if (!strcmp(linktype, "Digest"))
            {
                filename = strchr(dest, '/');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    printf("<a href=\"bbsanc,path,%s", anno_path_of(dest));
                    printf("%%2F%s.html\">", url_quote(filename));
                    hprintf("%s", nohtml4(display));
                    printf("</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }
            else if (!strcmp(linktype, "Digdir"))
            {	
                filename = strchr(dest, '/');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    printf("<a href=\"bbs0an,path,%s", anno_path_of(dest));
                    printf("%%2F%s.html\">", url_quote(filename));
                    hprintf("%s", nohtml4(display));
                    printf("</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }            
            else if (!strcmp(linktype, "Wiki"))
            {
                printf("<a target='_self' href='/wiki/%s'>", url_quote(dest));
                hprintf("%s", nohtml4(display));
                printf("</a>");
            }
            else
            {
                goto FMT_ERR;
            }

            endpos ++;
            if (*endpos == '\0')
            {
                return printf("\n");
            }
            return hhprintf("%s", endpos);

        FMT_ERR:
            /* revert changes */
            endpos[-1] = ']';
            dest[-1] = ':';
            if(pipepos)
            {
                pipepos[0] = '|';
            }
            return hprintf("%s", s);
        }
        else
        {
            buf0[len++] = *s++;
            if (len > 1000)
            {
                buf0[len] = '\0';
                hprintf("%s",buf0);
                len = 0;
            }
        }
    }

    return 0;
}

int hhfprintf(FILE * stream, char *fmt, ...)
{
    char buf0[1024], buf[1024], *s, *getparm(char *var);
    int len = 0;
    int my_link_mode;
    va_list ap;


    va_start(ap, fmt);
    vsnprintf(buf, 1023, fmt, ap);
    va_end(ap);
    buf[1023] = 0;
    s = buf;
    my_link_mode = atoi(getparm("my_link_mode"));
    if (my_link_mode == 1)
        return hfprintf(stream, "%s", buf);
    if (!strcasestr(s, "http://") && !strcasestr(s, "https://") && !strcasestr(s, "ftp://")
        && !strcasestr(s, "mailto:") && !strstr(s, "[["))
        return hfprintf(stream, "%s", buf);
    while (s[0])
    {
        if (!strncasecmp(s, "https://", 8)
            || !strncasecmp(s, "http://", 7)
            || !strncasecmp(s, "mailto:", 7)
            || !strncasecmp(s, "ftp://", 6))
        {
            char *tmp, *url;

            if (len > 0)
            {
                buf0[len] = 0;
                hfprintf(stream, "%s", buf0);
                len = 0;
            }
            tmp = strtok(s, "\'\" \r\t)(;\n");
            if (tmp == 0)
            {
                return hfprintf(stream, "%s",s);
            }
            url = tmp;
            if (!strncmp
                (UPLOAD_FILE_URL_HEADER, tmp,
                 strlen(UPLOAD_FILE_URL_HEADER)))
            {
                url += strlen("http://" BBSHOST);
            }
            if (pic_mod == 2
                &&
                (strcasestr(tmp, ".gif")
                 || strcasestr(tmp, ".jpg")
                 || strcasestr(tmp, ".png") || strcasestr(tmp, ".jfif")))
            {
                fprintf(stream,
                        "<IMG SRC=\"%s\" onload=\"if(this.width > screen.width - 200){this.width = screen.width - 200}\"><br/>\n",
                        nohtml(url));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return 0;
                return hhfprintf(stream, tmp);
            }
            if (pic_mod == 2 && strcasestr(tmp, ".swf"))
            {
                char *url = tmp;

                if (!strncmp
                    (UPLOAD_FILE_URL_HEADER, tmp,
                     strlen(UPLOAD_FILE_URL_HEADER)))
                {
                    url += strlen("http://" BBSHOST);
                }
                fprintf(stream,
                        "<EMBED SRC=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400 AllowScriptAccess=\"never\"></EMBED><br/>\n",
                        nohtml(url));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return 0;
                return hhfprintf(stream, "%s", tmp);
            }
            if (pic_mod == 1
                &&
                (strcasestr(tmp, ".gif")
                 || strcasestr(tmp, ".jpg")
                 || strcasestr(tmp, ".png") || strcasestr(tmp, ".jfif")))
            {

                fprintf
                    (stream, 
                     "<div style=\"height: 100px\"><IMG class=\"QmdImg\" SRC=\"%s\" style=\"max-height: 100%%; max-width: 100%%\"></div>\n",
                     nohtml(url));
                tmp = strtok(0, "");
                pic_mod = 0;
                if (tmp == 0)
                    return 0;
                return hhfprintf(stream, tmp);
	    }
            fprintf(stream, "<a target=_blank href='%s'>", nohtml(url));
            fprintf(stream, "%s</a>", nohtml4(tmp));
            tmp = strtok(0, "");
            if (tmp == 0)
                return fprintf(stream, "%s", "\n");
            return hhfprintf(stream, "%s", tmp);
        }
        else if (s[0] == '[' && s[1] == '[')
        {
            char *linktype, *endpos, *pipepos, *dest, *display, *filename;

            if (len>0 && buf0[len-1] == '#')
            {
                buf0[len - 1] = '[';
                buf0[len] = '[';
                buf0[len + 1] = '\0';
                hfprintf(stream, "%s", buf0);
                len = 0;
                return hhfprintf(stream, "%s", s + 2);
            }

            if (len > 0)
            {
                buf0[len] = 0;
                hfprintf(stream, "%s", buf0);
                len = 0;
            }

            linktype = s + 2;
            endpos = strstr(linktype, "]]");
            if (endpos == NULL || endpos - linktype > STRLEN * 2 - 2)
            {
                return hfprintf(stream, "%s",s);
            }

            *endpos++ = '\0';
            dest = strchr(linktype, ':');
            if (dest == NULL || dest[1] == '\0')
            {
                /* format error, print it  */
                endpos[-1] = ']';
                return hfprintf(stream, "%s", s);
            }
            *dest++ = '\0';            

            pipepos = strchr(dest, '|');
            if (pipepos != NULL)
            {
                *pipepos = '\0';
                display = pipepos + 1;        
                if(dest[0]=='\0' || display[0]=='\0')
                {
                    goto FMT_ERR;
                }
            }
            else
            {
                display = dest;
            }

            if (!strcmp(linktype, "User"))
            {
                fprintf(stream, "<a target='_self' href='/bbsqry?userid=%s'>", url_quote(dest));
                hfprintf(stream, "%s", nohtml4(display));
                fprintf(stream, "</a>");
            }
            else if (!strcmp(linktype, "Post"))
            {
                filename = strchr(dest, ',');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    fprintf(stream, "<a target='_self' href='/bbscon?board=%s&file=", url_quote(dest));
                    fprintf(stream, "%s'>", url_quote(filename));
                    hfprintf(stream, "%s", nohtml4(display));
                    fprintf(stream, "</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }
            else if (!strcmp(linktype, "Digest"))
            {
                filename = strchr(dest, '/');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    fprintf(stream, "<a href=\"bbsanc,path,%s", anno_path_of(dest));
                    fprintf(stream, "%%2F%s.html\">", url_quote(filename));
                    hfprintf(stream,"%s", nohtml(display));
                    fprintf(stream, "</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }
            else if (!strcmp(linktype, "Digdir"))
            {
                filename = strchr(dest, '/');
                if (filename != NULL)
                {
                    *filename++ = '\0';
                    fprintf(stream,"<a href=\"bbs0an,path,%s", anno_path_of(dest));
                    fprintf(stream,"%%2F%s.html\">", url_quote(filename));
                    hfprintf(stream, "%s", nohtml4(display));
                    fprintf(stream, "</a>");
                }
                else
                {
                    goto FMT_ERR;
                }
            }            
            else if (!strcmp(linktype, "Wiki"))
            {
                fprintf(stream, "<a target='_self' href='/wiki/%s'>", url_quote(dest));
                hfprintf(stream, "%s", nohtml(display));
                fprintf(stream, "</a>");
            }
            else if (!strcmp(linktype, "Url")) 
            {
                fprintf(stream, "<a target='_blank' href='%s'>", url_quote(dest));
                hfprintf(stream, "%s", nohtml(display));
                fprintf(stream, "</a>");
            }
            else
            {
                goto FMT_ERR;
            }
                
            endpos ++;
            if (*endpos == '\0')
            {
                return fprintf(stream, "%s", "\n");
            }
            return hhfprintf(stream, "%s", endpos);

        FMT_ERR:
            /* revert changes */
            endpos[-1] = ']';
            dest[-1] = ':';
            if(pipepos)
            {
                pipepos[0] = '|';
            }
            return hfprintf(stream, "%s", s);
        }
        else
        {
            buf0[len++] = *s++;
            if (len > 1000)
            {
                buf0[len] = '\0';
                hfprintf(stream, "%s",buf0);
                len = 0;
            }
        }
    }

    return 0;
}

void html_head(void)
{
    printf("<html>\n");
    printf
        ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\n");
    printf("<link rel=stylesheet type=text/css href='%s'>\n", CSS_FILE);
    printf
        ("<script type=\"text/javascript\" src=\"/check.js\"></script>\n");
}
void js_head(void)
{
    printf
        ("<link rel=\"stylesheet\" href=\"/app/css/jquery-ui.css\"></script>\n");
    printf
        ("<script type=\"text/javascript\" src=\"/app/lib/jquery-1.9.1.min.js\"></script>\n");
    printf
        ("<script type=\"text/javascript\" src=\"/app/lib/jquery-ui.js\"></script>\n");
}

void html_title_head(char *title)
{
	char temp[256];
    printf("<html>\n");
    printf
        ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\n");
    printf("<link rel=stylesheet type=text/css href='%s'>\n", CSS_FILE);
	html_normal(title, temp);
	printf("<title>%s - ÒûË®Ë¼Ô´</title>", temp);
    printf
        ("<script type=\"text/javascript\" src=\"/check.js\"></script>\n");
    printf("<script type=\"text/javascript\">\n");
    printf("function jumpUrl(x){\n");
    printf("	var url = '', e = encodeURIComponent, s = screen, d = document, title = d.title, u = d.location;\n");
    printf("	if(x=='douban'){\n");
    printf("		url =\"http://www.douban.com/recommend/?url=\"+e(u.href)+\"&title=\"+e(title);\n");
    printf("	}else if(x=='renren'){\n");
    printf("		url =\"http://share.renren.com/share/buttonshare?link=\"+e(u.href)+\"&title=\"+e(title);\n");
    printf("	}else if(x=='kaixin'){\n");
    printf("		url =\"http://www.kaixin001.com/repaste/share.php?rurl=\"+e(u.href)+\"&rtitle=\"+e(title);\n");
    printf("	}else if(x=='qzone'){\n");
    printf("		url =\"http://sns.qzone.qq.com/cgi-bin/qzshare/cgi_qzshare_onekey?to=xiaoyou&url=\"+e(u.href);\n");
    printf("	}else if(x=='sina'){\n");
    printf("		url =\"http://v.t.sina.com.cn/share/share.php?title=\"+e(title)+\"&url=\"+e(u.href);\n");
    printf("	}\n");
    printf("	function a(){\n");
    printf("		if (!window.open(url))\n");
    printf("			u.href = [url].join('');\n");
    printf("	}\n");
    printf("	if (/Firefox/.test(navigator.userAgent)){\n");
    printf("		setTimeout(a, 0);\n");
    printf("	}else{\n");
    printf("		a();\n");
    printf("	}\n");
    printf("}\n");
    printf("</script>\n");
}

void html_normal(char* source, char* replace){
    char* src = source;
    char* temp = replace;
    while (*src != 0){
        if (*src == '<'){
            sprintf(temp, "[");
            temp ++;
        }    
        else if (*src == '>'){
            sprintf(temp, "]");
            temp ++;
        }    
        else if (*src == '&'){
            sprintf(temp, "&amp;");
            temp += 5;
        }    
        else{
            *temp = *src;
            temp++;
        }
        src++;    
    }    
    *temp = 0;
}    

void set_fromhost(void)
{

    strsncpy(fromhost, getsenv("REMOTE_ADDR"), 60);
}

int bbs_http_init(void)
{
    set_fromhost();
    if (!silence)
    {
        cgi_head();
        html_head();
    }
    return 0;
}

int __to16(char c)
{
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= '0' && c <= '9')
        return c - '0';
    return 0;
}

int __unhcode(char *s)
{
    int m, n;

    for (m = 0, n = 0; s[m] != 0; m++, n++)
    {
        if (s[m] == '+')
        {
            s[n] = ' ';
            continue;
        }
        if (s[m] == '%')
        {
            s[n] = __to16(s[m + 1]) * 16 + __to16(s[m + 2]);
            m += 2;
            continue;
        }
        s[n] = s[m];
    }
    s[n] = 0;
    return 0;
}

char *getparm(char *var)
{
    int n;
    char *ptr;

    if (cgiFormStringSpaceNeeded(var, &n) != cgiFormSuccess)
        return "";
    ptr = mp_alloc(n);
    cgiFormString(var, ptr, n);
    return ptr;
}

int is_web_user(struct user_info *user)
{
    return (user->mode == 10001);
}

int user_init(struct userec *x, struct user_info **y)
{
    struct userec *x2;
    char id[20];
    int i, key;

    cgiCookieString("utmpuserid", id, 20);
    cgiCookieInteger("utmpnum", &i, -1);
    cgiCookieInteger("utmpkey", &key, -1);

    if (i < 0 || i >= MAXACTIVE)
        return 0;
    (*y) = &(shm_utmp->uinfo[i]);
    if (strncmp((*y)->from, fromhost, 60))
        return 0;
    if ((*y)->utmpkey != key)
        return 0;
    if ((*y)->active == 0)
        return 0;
    if ((*y)->userid[0] == 0)
        return 0;
    if (!is_web_user(*y))
        return 0;
    if (!strcasecmp((*y)->userid, "new"))
        return 0;
    x2 = getuser((*y)->userid);
    if (x2 == 0)
        return 0;
    if (strcmp(x2->userid, id))
        return 0;
    memcpy(x, x2, sizeof(*x));
    if (!strcasecmp((*y)->userid, "guest"))
        return 0;
    return 1;
}

int getdatestring(time_t now, int Chang)
{
    struct tm *tm;
    char weeknum[7][3] = { "Ìì", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù" };

    tm = localtime(&now);
    sprintf(datestring, "%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
    return (tm->tm_sec % 10);
}

int
post_mail(char *userid, char *title, char *file, char *id,
          char *nickname, char *ip, int sig)
{
    FILE *fp, *fp2;
    char buf3[256], dir[256];
    struct fileheader header;
    int t, i;

    if (strstr(userid, "@"))
        return post_imail(userid, title, file, id, nickname, ip, sig);
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    for (i = 0; i < 100; i++)
    {
        t = time(0) + i;
        sprintf(buf3, "mail/%c/%c/%s/M.%d.A", toupper(userid[0]),
                toupper(userid[1]), userid, (int) (i + time(0)));
        if (!file_exist(buf3))
            break;
    }
    if (i >= 99)
        return -1;
    sprintf(header.filename, "M.%d.A", t);
    strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    if (fp == 0)
        return -2;
    fp2 = fopen(file, "r");
    getdatestring(time(0), NA);
    fprintf(fp, "¼ÄÐÅÈË: %s (%s)\n", id, nickname);
    fprintf(fp, "±ê  Ìâ: %s\n", title);
    fprintf(fp, "·¢ÐÅÕ¾: %s (%s)\n", BBSNAME, datestring);
    fprintf(fp, "À´  Ô´: %s\n\n", ip);
    if (fp2)
    {
        int tail = 0, sum = 0, in_link = 0;
        while (1)
        {
            if (fgets(buf3, 256, fp2) == NULL)
                break;
            fprintf2(fp, buf3, &tail, &sum, &in_link);
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--\n");
    sig_append(fp, id, sig);
    fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(userid[0]),
            toupper(userid[1]), userid);
    fp = fopen(dir, "a");
    if (fp == 0)
        return -1;
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return 0;
}

int
cross_mail(char *userid, char *title, char *file, char *id,
          char *nickname, char *ip)
{
    FILE *fp, *fp2;
    char buf3[256], dir[256];
    struct fileheader header;
    int t, i;

    if (strstr(userid, "@"))
        return post_imail(userid, title, file, id, nickname, ip, -1);
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    for (i = 0; i < 100; i++)
    {
        t = time(0) + i;
        sprintf(buf3, "mail/%c/%c/%s/M.%d.A", toupper(userid[0]),
                toupper(userid[1]), userid, (int) (i + time(0)));
        if (!file_exist(buf3))
            break;
    }
    if (i >= 99)
        return -1;
    sprintf(header.filename, "M.%d.A", t);
    strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    if (fp == 0)
        return -2;
    fp2 = fopen(file, "r");
    getdatestring(time(0), NA);
    fprintf(fp, "¼ÄÐÅÈË: %s (%s)\n", id, nickname);
    fprintf(fp, "±ê  Ìâ: %s\n", title);
    fprintf(fp, "·¢ÐÅÕ¾: %s (%s)\n", BBSNAME, datestring);
    fprintf(fp, "À´  Ô´: %s\n\n", ip);
    if (fp2)
    {
        int tail = 0, sum = 0, in_link = 0;
        while (1)
        {
            if (fgets(buf3, 256, fp2) == NULL)
                break;
            fprintf2(fp, buf3, &tail, &sum, &in_link);
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--");
    fprintf(fp, "\n[1;%dm¡ù ×ª¼Ä:¡¤%s %s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(userid[0]),
            toupper(userid[1]), userid);
    fp = fopen(dir, "a");
    if (fp == 0)
        return -1;
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return 0;
}

int
post_imail(char *userid, char *title, char *file, char *id,
           char *nickname, char *ip, int sig)
{

    return 0;
}

int
post_article_ex(char *board, char *title, char *file, char *id,
             char *nickname, char *ip, int sig, int autocr,
             unsigned int currid, unsigned int reid, unsigned int flag)
{
    FILE *fp, *fp2;
    char buf3[1024];

    char dir [80];
    struct fileheader f;
    int noreply;
    struct shortfile *brd;
    struct fileheader header;
    int t = 0, count = 0, filesd;

    brd = getbcache(board);
    if (brd == 0)
        return -1;
    board = brd->filename;
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    if (flag & FILE_NOREPLY)
		header.accessed[0] |= FILE_NOREPLY;

    noreply = 0;
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("´íÎóµÄÄ¿Â¼");
    while (1)
    {
        if (fread(&f, sizeof(f), 1, fp) <= 0)
        {
            break;
        }
        if (f.reid == reid)
        {
            if ((f.accessed[0] & FILE_NOREPLY))
            {
                noreply = 1;
            }
            else
            {
                noreply = 0;
                break;
            }
        }
    }
    fclose(fp);
    if(noreply)
        http_quit_print("±¾ÎÄÎª²»¿É»Ø¸´");


    t = time(0);
    char fulltitle[20];

    sprintf(fulltitle, "M.%d.A", t);
    sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
            fulltitle);

    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        sprintf(fulltitle, "M.%d.A", t);
        sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
                fulltitle);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);

    if (0 == currid)
    {
        header.id = t;
        header.reid = reid;
    }
    else
    {
        header.id = t;
        header.reid = t;
    }

    sprintf(header.filename, "M.%d.A", t);
    strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    fp2 = fopen(file, "r");
    getdatestring(time(0), NA);
    fprintf(fp,
            "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
            id, nickname, board, title, BBSNAME, datestring);
    if (fp2 != 0)
    {
        int tail = 0, sum = 0, in_link = 0;
        while (1)
        {
            if (fgets(buf3, 1000, fp2) == NULL)
                break;
            if (autocr)
            {
                fprintf2(fp, buf3, &tail, &sum, &in_link);
            }
            else
            {
                fprintf(fp, "%s", buf3);
            }
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--\n");
    sig_append(fp, id, sig);
    fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(buf3, "boards/%s/.DIR", board);
    fp = fopen(buf3, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return t;
}

int
post_article(char *board, char *title, char *file, char *id,
             char *nickname, char *ip, int sig, int autocr,
             unsigned int currid, unsigned int reid)
{
    FILE *fp, *fp2;
    char buf3[1024];

    char dir [80];
    struct fileheader f;
    int noreply;
    struct shortfile *brd;
    struct fileheader header;
    int t = 0, count = 0, filesd;

    brd = getbcache(board);
    if (brd == 0)
        return -1;
    board = brd->filename;
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);

    noreply = 0;
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("´íÎóµÄÄ¿Â¼");
    while (1)
    {
        if (fread(&f, sizeof(f), 1, fp) <= 0)
        {
            break;
        }
        if (f.reid == reid)
        {
            if ((f.accessed[0] & FILE_NOREPLY))
            {
                noreply = 1;
            }
            else
            {
                noreply = 0;
                break;
            }
        }
    }
    fclose(fp);
    if(noreply)
        http_quit_print("±¾ÎÄÎª²»¿É»Ø¸´");


    t = time(0);
    char fulltitle[20];

    sprintf(fulltitle, "M.%d.A", t);
    sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
            fulltitle);

    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        sprintf(fulltitle, "M.%d.A", t);
        sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
                fulltitle);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);

    if (0 == currid)
    {
        header.id = t;
        header.reid = reid;
    }
    else
    {
        header.id = t;
        header.reid = t;
    }

    sprintf(header.filename, "M.%d.A", t);
    strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    fp2 = fopen(file, "r");
    getdatestring(time(0), NA);
    fprintf(fp,
            "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
            id, nickname, board, title, BBSNAME, datestring);
    if (fp2 != 0)
    {
        int tail = 0, sum = 0, in_link = 0;
        while (1)
        {
            if (fgets(buf3, 1000, fp2) == NULL)
                break;
            if (autocr)
            {
                fprintf2(fp, buf3, &tail, &sum, &in_link);
            }
            else
            {
                fprintf(fp, "%s", buf3);
            }
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--\n");
    sig_append(fp, id, sig);
    fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(buf3, "boards/%s/.DIR", board);
    fp = fopen(buf3, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return t;
}

int
cross_article(char *board, char *title, char *file, char *id,
             char *nickname, char *ip)
{
    FILE *fp, *fp2;
    char buf3[1024];

    struct shortfile *brd;
    struct fileheader header;
    int t = 0, count = 0, filesd;

    brd = getbcache(board);
    if (brd == 0)
        return -1;
    board = brd->filename;
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);

    t = time(0);
    char fulltitle[20];

    sprintf(fulltitle, "M.%d.A", t);
    sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
            fulltitle);

    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        sprintf(fulltitle, "M.%d.A", t);
        sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle),
                fulltitle);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);

    header.id = t;
    header.reid = t;

    sprintf(header.filename, "M.%d.A", t);
    strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    fp2 = fopen(file, "r");
    getdatestring(time(0), NA);
    fprintf(fp,
            "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
            id, nickname, board, title, BBSNAME, datestring);
    if (fp2 != 0)
    {
        while (1)
        {
            if (fgets(buf3, 1000, fp2) == NULL)
                break;
            fprintf(fp, "%s", buf3);
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--");
    fprintf(fp, "\n[1;%dm¡ù ×ªÔØ:¡¤%s %s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(buf3, "boards/%s/.DIR", board);
    fp = fopen(buf3, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return t;
}

int sig_append(FILE * fp, char *id, int sig)
{
    FILE *fp2;
    char path[256];
    char buf[100][256];
    int i, total;
    struct userec *x;

    if (sig < 0 || sig > 10)
        return 0;
    x = getuser(id);
    if (x == 0)
        return 0;
    sprintf(path, "home/%c/%c/%s/signatures", toupper(id[0]),
            toupper(id[1]), id);
    fp2 = fopen(path, "r");
    if (fp2 == 0)
        return 0;
    for (total = 0; total < 100; total++)
        if (fgets(buf[total], 255, fp2) == 0)
            break;
    fclose(fp2);
    for (i = sig * 6; i < sig * 6 + 6; i++)
    {
        if (i >= total)
            break;
        fprintf(fp, "%s", buf[i]);
    }
    return 0;
}

char *anno_path_of(char *board)
{
    FILE *fp;
    static char buf[256], buf1[80], buf2[80];

    fp = fopen("0Announce/.Search", "r");
    if (fp == 0)
        return "";
    snprintf(buf, 255, "%s%c", board, ':');
    while (1)
    {
        if (fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2) <= 0)
            break;
        if (!strcasecmp(buf1, buf))
        {
            sprintf(buf, "/%s", buf2);
            fclose(fp);
            return url_quote(buf);
        }
    }
    fclose(fp);
    return "";
}

int WHICHDIST(char *userid)
{
    char datafile[80] = BBSHOME "/etc/DISTmaster.ini";
    FILE *fp;
    int counts = 1;
    char t[256], *t1, *t2, *t3;

    fp = fopen(datafile, "r");
    if (fp == NULL)
        return 12;
    while (fgets(t, 255, fp) && counts <= 11)
    {
        t1 = strtok(t, " \t\n");
        if (t1 == NULL || t1[0] == '#')
            continue;
        if (!isalpha(t1[0]))
        {
            counts++;
            continue;
        }
        t2 = strtok(NULL, " \t\n");
        if (t2 != NULL)
            t3 = strtok(NULL, " \t\n");
        else
            t3 = NULL;
        if ((t1 != NULL && !strcasecmp(t1, userid))
            || (t2 != NULL && !strcasecmp(t2, userid)) || (t3 != NULL
                                                           &&
                                                           !strcasecmp
                                                           (t3, userid)))
        {
            fclose(fp);
            return (counts);
        }
        counts++;
    }
    fclose(fp);
    return (counts);
}

int has_DM_perm(struct userec *user, char *board)
{
    char BoardsFile[256], category;
    int fd, total, index, Dindex, DMLevel;
    struct boardheader *buffer;
    struct stat st;

    char CateInSect[SectNum][5] =
        { "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw",
"x" };

    sprintf(BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open(BoardsFile, O_RDONLY);
    if (fd == -1)
        return NA;
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer =
        (struct boardheader *) calloc((size_t) total,
                                      sizeof(struct boardheader));
    if (buffer == NULL)
    {
        close(fd);
        return NA;
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close(fd);
        return NA;
    }
    close(fd);

    for (index = 0; index < total; index++)
    {
        if (!strcmp(buffer[index].filename, board))
            break;
    }
    if (index == total)
        return NA;

    category = buffer[index].title[0];
    free(buffer);

    for (Dindex = 0; Dindex < SectNum; Dindex++)
    {
        if (strchr(CateInSect[Dindex], category) != NULL)
            break;
    }

    if (Dindex == SectNum)
        return NA;

    DMLevel = WHICHDIST(user->userid);
    if (Dindex == DMLevel)
        return YEA;
    else
        return NA;
}

int has_parent_perm(struct userec *user, char *board)
{
    struct shortfile *x;
    char buf[256], *bm;
    int parent = 0;

    x = getbcache(board);
    parent = x->group - 1;
    if (parent < 0)
        return 0;
    strncpy(buf, shm_bcache->bcache[parent].BM, BM_LEN - 4);
    bm = strtok(buf, ",: ;&()\n");
    while (bm)
    {
        if (!strcasecmp(bm, user->userid))
            return 1;
        bm = strtok(0, ",: ;&()\n");
    }
    return 0;
}

int has_BM_perm(struct userec *user, char *board)
{
    struct shortfile *x;
    char buf[256], *bm;

    x = getbcache(board);
    if (x == 0)
        return 0;
    if (user_perm(user, PERM_BLEVELS))
        return 1;

    if (user_perm(user, PERM_SPECIAL7) && has_DM_perm(user, board))
        return 1;

    if (!user_perm(user, PERM_BOARDS))
        return 0;
    if (has_parent_perm(user, board))
        return 1;
    strcpy(buf, x->BM);
    bm = strtok(buf, ",: ;&()\n");
    while (bm)
    {
        if (!strcasecmp(bm, user->userid))
            return 1;
        bm = strtok(0, ",: ;&()\n");
    }
    if (has_badmin_perm(user, x))
        return 1;
    return 0;
}

int has_badmin_perm(struct userec *user, struct shortfile *x)
{
    FILE *fp;
    char buf[256], code[80], userid[80];
    int ret = 0;

    fp = fopen("etc/admins", "r");
    if (!fp)
        return 0;
    while (1)
    {
        if (fgets(buf, 255, fp) == 0)
            break;
        if (buf[0] == '#')
            continue;
        userid[0] = 0;
        sscanf(buf, "%s %s", code, userid);
        if (!strcmp(userid, currentuser.userid))
        {
            if (strchr(code, x->title[0]))
                ret = 1;
            break;
        }
    }
    fclose(fp);
    return ret;
}

int is_normal_board = 0;
int has_read_perm(struct userec *user, char *board)
{
    struct shortfile *x;
    char pathbuf[80];
    FILE *fp;

    fp = NULL;

    if (board[0] <= 32)
        return 0;
    x = getbcache(board);
    if (x == 0)
        return 0;

    if (x->level == 0)
    {
        sprintf(pathbuf, "boards/%s/board.allow", x->filename);
        fp = fopen(pathbuf, "r");
        if (fp == NULL)
        {
            is_normal_board = 1;
            return 1;
        }
        else
        {
            fclose(fp);
            if (file_has_word(pathbuf, user->userid))
                return 1;
            else
                return 0;
        }
    }

    if (x->level & (PERM_POSTMASK | PERM_NOZAP))
    {
        sprintf(pathbuf, "boards/%s/board.allow", x->filename);
        fp = fopen(pathbuf, "r");
        if (fp == NULL)
            return 1;
        else
        {
            fclose(fp);
            if (file_has_word(pathbuf, user->userid))
                return 1;
            else
                return 0;
        }
    }

    if (!user_perm(user, PERM_BASIC))
        return 0;

    if (user_perm(user, x->level))
    {
        sprintf(pathbuf, "boards/%s/board.allow", x->filename);
        fp = fopen(pathbuf, "r");
        if (fp == NULL)
            return 1;
        else
        {
            fclose(fp);
            if (file_has_word(pathbuf, user->userid))
                return 1;
            else
                return 0;
        }
    }

    return 0;
}

int ci_strcmp(register char *s1, register char *s2)
{
    char c1, c2;

    while (1)
    {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 += 'A' - 'a';
        if (c2 >= 'a' && c2 <= 'z')
            c2 += 'A' - 'a';
        if (c1 != c2)
            return (c1 - c2);
        if (c1 == 0)
            return 0;
    }
}

struct deny denyuser[512];
int denynum = 0;

int loaddenyuser(char *board)
{
    FILE *fp;
    char path[80], buf[256];

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "r");
    if (fp == 0)
        return 0;
    while (denynum < 500)
    {
        if (fgets(buf, 80, fp) == 0)
            break;
        sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
               denyuser[denynum].exp, denyuser[denynum].autoundeny,
               denyuser[denynum].free_time);
        denynum++;
    }
    fclose(fp);
    return 0;
}

int inform(char *board, char *user)
{
    FILE *fp;
    char path[80], title[80];
    time_t now = time(0);

    sprintf(title, "[¹«¸æ] »Ö¸´%sÔÚ%s°åµÄPOSTÈ¨", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "¡¾±¾¹«¸æÓÉ×Ô¶¯·¢ÐÅÏµÍ³×Ô¶¯ÕÅÌù¡¿\n\n");
    fprintf(fp, "%s±»°åÎñÈËÔ±[%s]»Ö¸´ÁË%s°åPOSTÈ¨.\n", user,
            currentuser.userid, board);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "×Ô¶¯·¢ÐÅÏµÍ³",
                 "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME

    if (strncmp(board, "BMTraining", 10) == 0)
    {
        post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
                     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
    }
    else
    {
        post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
                     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
    }
#endif

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
    printf("ÏµÍ³ÒÑ¾­·¢ÐÅÍ¨ÖªÁË%s.<br>\n", user);
    return 0;
}

void inform_autoundeny(char *board, char *user)
{
    FILE *fp;
    char path[80], title[80];
    time_t now = time(0);

    sprintf(title, "[¹«¸æ] »Ö¸´%sÔÚ%s°åµÄPOSTÈ¨", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "¡¾±¾¹«¸æÓÉ×Ô¶¯·¢ÐÅÏµÍ³×Ô¶¯ÕÅÌù¡¿\n\n");
    fprintf(fp,
            "\n  %s ÍøÓÑ£º\n\n\tÒò·â½ûÊ±¼äÒÑ¹ý£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º·¢±íÎÄÕÂ¡»È¨Á¦¡£\n\n"
            "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS", user, board);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "×Ô¶¯·¢ÐÅÏµÍ³",
                 "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME
    if (strncmp(board, "BMTraining", 10) == 0)
    {
        post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
                     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
    }
    else
    {
        post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
                     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
    }
#endif

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
}

int savedenyuser(char *board)
{
    FILE *fp;
    int i;
    char path[80], *exp;

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "w");
    if (fp == 0)
        return 0;
    for (i = 0; i < denynum; i++)
    {
        int m;

        exp = denyuser[i].exp;
        if (denyuser[i].id[0] == 0)
            continue;
        for (m = 0; exp[m]; m++)
        {
            if (exp[m] <= 32 && exp[m] > 0)
                exp[m] = '.';
        }

        fprintf(fp, "%-12s %-40s %s %16s\n", denyuser[i].id,
                denyuser[i].exp, denyuser[i].autoundeny,
                denyuser[i].free_time);
//        denynum++;
    }
    fclose(fp);
    return 0;
}

void bbsautoundeny(char board[80], char *userid)
{
    int i;

    loaddenyuser(board);
    for (i = 0; i < denynum; i++)
    {
        if (!strcasecmp(denyuser[i].id, userid))
        {
            denyuser[i].id[0] = 0;
            savedenyuser(board);
            inform_autoundeny(board, userid);
        }
    }
}

int seek_in_denyfile(char *board, char *seekstr)
{
    FILE *fp;
    char buf[STRLEN], buff[STRLEN];
    char *namep, *date;
    char flag;
    time_t nowtime;

    sprintf(buff, "boards/%s/deny_users", board);
    if ((fp = fopen(buff, "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, ": \n\r\t");
        if (namep != NULL && ci_strcmp(namep, seekstr) == 0)
        {
            fclose(fp);
            nowtime = time(0);
            getdatestring(nowtime, NA);
            flag = buf[55];
            date = (char *) (buf + 58);
            if (flag == 'a' && strncmp(datestring, date, 16) >= 0)
            {
                bbsautoundeny(board, seekstr);
                return 0;
            }
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int has_post_perm(struct userec *user, char *board)
{
    char buf3[256];
    struct shortfile *x;
	struct anony_item item;
	char anonyid[IDLEN + 2];

    x = getbcache(board);

	if (x == 0)
		return 0;
	strcpy(board, x->filename);
	if (!strcasecmp(board, "Forum") && !loginok)
	{
		sprintf(buf3, "boards/%s/deny_users", x->filename);
		if (file_has_word(buf3, "guest"))
			return 0;
		return 1;
	}
	if (!has_read_perm(user, board))
		return 0;
	if (x == 0)
		return 0;
	if (!loginok)
		return 0;
	sprintf(buf3, "boards/%s/deny_users", x->filename);
	if (file_has_word(buf3, user->userid))
	{
		if (seek_in_denyfile(board, user->userid))
			return 0;
		else
			return 1;
	}

	if ((x->flag & ANONY_FLAG || x->flag & POSITIVE_ANONY_FLAG))
	{
		if (search_in_anonyindex(user->userid, NULL, &item) != -1)
		{
			strncpy(anonyid, "Anony.", 6);
			strncpy(anonyid + 6, item.ranstr, 7);
			if (file_has_word(buf3, anonyid))
			{
				if (seek_in_denyfile(board, anonyid))
					return 0;
				else
					return 1;
			}
		}
	}

	if (!HAS_PERM(PERM_BASIC))
		return 0;
	if (!strcasecmp(board, "sysop"))
		return 1;
	if (!strcasecmp(board, "BBSHelp"))
		return 1;
	if (user_perm(user, PERM_SYSOP))
		return 1;
	if (!user_perm(user, PERM_BASIC))
		return 0;
	if (!user_perm(user, PERM_POST))
		return 0;
#ifdef REAL_NAME_EMAIL
    if (!user_perm(user, PERM_REALNAME))
        return 0;
#endif
    if (!(x->level & PERM_NOZAP) && x->level && !user_perm(user, x->level))
        return 0;
    return 1;
}

struct shortfile *getbcache(char *board)
{
    int i;

    if (board[0] == 0)
        return 0;
    for (i = 0; i < MAXBOARD; i++)
        if (!strcasecmp(board, shm_bcache->bcache[i].filename))
            return &shm_bcache->bcache[i];
    return 0;
}

int getbnum(char *board)
{
    int i;

    if (board[0] == 0)
        return 0;
    for (i = 0; i < MAXBOARD; i++)
        if (!strcasecmp(board, shm_bcache->bcache[i].filename))
            return i + 1;
    return 0;
}

int count_on_board(char *board)
{
    int i, j, s = 0;

    i = getbnum(board);
    if (i <= 0 || i > MAXBOARD)
        return 0;
    for (j = 0; j < MAXACTIVE; j++)
    {
        if (shm_utmp->uinfo[j].userid[0] < ' '
            || !shm_utmp->uinfo[j].active || !shm_utmp->uinfo[j].pid)
            continue;

    }
    return s;
}

int count_mails(char *id, int *total, int *unread)
{
    struct fileheader x1;
    char buf[256];
    FILE *fp;

    *total = 0;
    *unread = 0;
    if (getuser(id) == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%c/%s/.DIR", BBSHOME, toupper(id[0]),
            toupper(id[1]), id);
    fp = fopen(buf, "r");
    if (fp == 0)
        return 0;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
    {
        (*total)++;
        if (!(x1.accessed[0] & FILE_READ))
            (*unread)++;
    }
    fclose(fp);
    return 0;
}

int findnextutmp(char *id, int from)
{
    int i;

    if (from < 0)
        from = 0;
    for (i = from; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active)
            if (!strcasecmp(shm_utmp->uinfo[i].userid, id))
                return i;
    return -1;
}

int sethomefile(char *buf, char *id, char *file)
{
    sprintf(buf, "home/%c/%c/%s/%s", toupper(id[0]), toupper(id[1]), id,
            file);
    return 0;
}

int add_web_msg(char *id)
{
    int i;
    FILE *fp;
    char buf[1048], file[256];

    memset(buf, 0x20, 1048);
    buf[1047] = 0x00;

    sprintf(file, "home/%c/%c/%s/.msgdir", toupper(id[0]), toupper(id[1]),
            id);
    i = file_size(file) / sizeof(struct msgdir);
    if (get_record(buf, sizeof(struct msgdir), i - 1, file) <= 0)
        return 0;
    for (i = 0; i < 1048; i++)
    {
        if (buf[i] == 0x00)
            buf[i] = 0x20;
    }
    sprintf(file, "home/%c/%c/%s/wwwmsg", toupper(id[0]), toupper(id[1]),
            id);
    fp = fopen(file, "a");
    fwrite(buf, sizeof(struct msgdir), 1, fp);
    fclose(fp);
    return 0;

}

int safewrite(int fd, char *buf, int size)
{
    int cc, sz = size, origsz = size;
    char *bp = buf;

    do
    {
        cc = write(fd, bp, sz);
        if ((cc < 0) && (errno != EINTR))
        {

            return -1;
        }
        if (cc > 0)
        {
            bp += cc;
            sz -= cc;
        }
    }
    while (sz > 0);
    return origsz;
}

int f_append_msg(char *path, char *uid, char *msg)
{
    struct msgdir m;
    int fd;

    if ((fd = open(path, O_WRONLY | O_CREAT, 0644)) == -1)
    {

        return -1;
    }
    memset(&m, 0, sizeof(m));
    strncpy(m.userid, uid, IDLEN + 2);
    m.pid = getpid();
    m.tt = time(NULL);
    strncpy(m.msg, msg, MAX_MSG_SIZE);
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    if (safewrite(fd, (char *) &m, sizeof(m)) == -1)

        flock(fd, LOCK_UN);
    close(fd);

    return 0;

}

int
send_msg(char *myuserid, int mypid, char *touserid, int topid,
         char msg[256], int isWeb)
{
    char msgbuf[256], msgbuf2[256], buf3[256], *timestr;
    unsigned int i;
    time_t now;

    now = time(0);
    timestr = ctime(&now);
    *(timestr + 24) = '\0';
    for (i = 0; i < strlen(msg); i++)
        if ((0 < msg[i] && msg[i] <= 27) || msg[i] == -1)
            msg[i] = 32;
    if (mypid <= 0)
        return -1;
    sprintf(msgbuf,
            "[0;1;44;36m%-12.12s[33m([36m%-25s[33m):%50s\n[37m%-54.54s[m[%05dm\n",
            myuserid, timestr, "[1;31m^Z»Ø¸´[0m", msg, mypid);
    sprintf(msgbuf2, "[1;32;40m·¢ËÍ£º[1;33;40m%s[m (%-25s):[m\n%s\n",
            touserid, timestr, msg);

    sethomefile(buf3, touserid, "msgfile");
    f_append(buf3, msgbuf);

    sethomefile(buf3, touserid, "msgfile.me");
    f_append(buf3, msgbuf);

    sethomefile(buf3, touserid, ".msgdir");

    f_append_msg(buf3, myuserid, msgbuf);

    sethomefile(buf3, myuserid, "msgfile.me");
    f_append(buf3, msgbuf2);
    if (isWeb)
    {
        add_web_msg(touserid);
        return 0;
    }

    if (topid <= 0)
        return -1;

    kill(topid, SIGUSR2);
    return 0;
}

char *horoscope(int month, int day)
{
    int date = month * 100 + day;

    if (month < 1 || month > 12 || day < 1 || day > 31)
        return "²»Ïê";
    if (date < 121 || date >= 1222)
        return "Ä¦ôÉ×ù";
    if (date < 219)
        return "Ë®Æ¿×ù";
    if (date < 321)
        return "Ë«Óã×ù";
    if (date < 421)
        return "ÄµÑò×ù";
    if (date < 521)
        return "½ðÅ£×ù";
    if (date < 622)
        return "Ë«×Ó×ù";
    if (date < 723)
        return "¾ÞÐ·×ù";
    if (date < 823)
        return "Ê¨×Ó×ù";
    if (date < 923)
        return "´¦Å®×ù";
    if (date < 1024)
        return "Ìì³Ó×ù";
    if (date < 1123)
        return "ÌìÐ«×ù";
    if (date < 1222)
        return "ÉäÊÖ×ù";
    return "";
}

char *ModeType(int mode)
{
    switch (mode)
    {
    case IDLE:
        return "";
    case NEW:
        return "ÐÂÕ¾ÓÑ×¢²á";
    case LOGIN:
        return "½øÈë±¾Õ¾";
    case DIGEST:
        return "ä¯ÀÀ¾«»ªÇø";
    case MMENU:
        return "×ª»»ÕóµØÖÐ";
    case ADMIN:
        return "¹ÜÀíÕßÑ¡µ¥";
    case SELECT:
        return "Ñ¡ÔñÌÖÂÛÇø";
    case READBRD:
        return "Ò»ÀÀÖÚÉ½Ð¡";
    case READNEW:
        return "¿´¿´ÐÂÎÄÕÂ";
    case READING:
        return "Æ·Î¶ÎÄÕÂ";
    case POSTING:
        return "ÎÄºÀ»Ó±Ê";
    case MAIL:
        return "´¦ÀíÐÅ¼ã";
    case SMAIL:
        return "¼ÄÓïÐÅ¸ë";
    case RMAIL:
        return "ÔÄÀÀÐÅ¼ã";
    case TMENU:
        return "ÁÄÌìÑ¡µ¥";
    case LUSERS:
        return "¶«ÕÅÎ÷Íû:)";
    case FRIEND:
        return "Ñ°ÕÒºÃÅóÓÑ";
    case MONITOR:
        return "Ì½ÊÓÃñÇé";
    case QUERY:
        return "²éÑ¯ÍøÓÑ";
    case TALK:
        return "ÁÄÌì";
    case PAGE:
        return "ºô½Ð";
    case CHAT1:
        return "¹ú¼Ê»áÒéÌü";
    case CHAT2:
        return "ÄÏÑó¿ÍÕ»";
    case LAUSERS:
        return "Ì½ÊÓÍøÓÑ";
    case XMENU:
        return "ÏµÍ³×ÊÑ¶";
    case VOTING:
        return "Í¶Æ±ÖÐ...";
    case BBSNET:
        return "BBSNET";
    case EDITWELC:
        return "±à¼­Welc";
    case EDITUFILE:
        return "±à¼­¸öÈËµµ";
    case EDITSFILE:
        return "±àÐÞÏµÍ³µµ";
    case ZAP:
        return "¶©ÔÄÌÖÂÛÇø";
    case SYSINFO:
        return "ÐÒÔË³é½±";
    case DICT:
        return "·­²é×Öµä";
    case LOCKSCREEN:
        return "Ó©Ä»Ëø¶¨";
    case NOTEPAD:
        return "ÁôÑÔ°å";
    case GMENU:
        return "¹¤¾ßÏä";
    case MSG:
        return "ËÍÑ¶Ï¢";
    case USERDEF:
        return "×Ô¶©²ÎÊý";
    case EDIT:
        return "ÐÞ¸ÄÎÄÕÂ";
    case OFFLINE:
        return "×ÔÉ±ÖÐ..";
    case EDITANN:
        return "±àÐÞ¾«»ª";
    case LOOKMSGS:
        return "²ì¿´Ñ¶Ï¢";
    case WNOTEPAD:
        return "Óû×ß»¹Áô";
    case GOODWISH:
        return "¸øÅóÓÑ×£¸£";
    case FRIENDTEST:
        return "ÓÑÒê²âÊÔ";
    case FAVOURITE:
        return "¸öÈËÊÕ²Ø¼Ð";
    case QAUTHOR:
        return "²éÑ¯×÷Õß";
    case WATCHINBOARD:
        return "²é¿´°åÃæ";
    case 10001:
        return "Web ä¯ÀÀ";
    default:
        return "È¥ÁËÄÇÀï!?";
    }
}

char *cexp0(int exp, int type)
{
    char *cexp2(), *cexp3();

    if (type == 1)
        return cexp2(exp);
    if (type == 2)
        return cexp3(exp);
    if (type == 3)
        return "";
    if (exp == -9999)
        return "Ã»µÈ¼¶";
    if (exp <= 100)
        return "ÐÂÊÖÉÏÂ·";
    if (exp <= 450)
        return "Ò»°ãÕ¾ÓÑ";
    if (exp <= 850)
        return "ÖÐ¼¶Õ¾ÓÑ";
    if (exp <= 1500)
        return "¸ß¼¶Õ¾ÓÑ";
    if (exp <= 2500)
        return "ÀÏÕ¾ÓÑ";
    if (exp <= 3000)
        return "³¤ÀÏ¼¶";
    if (exp <= 5000)
        return "±¾Õ¾ÔªÀÏ";
    if (exp <= 10000)
        return "¿ª¹ú´óÀÏ";
    if (exp <= 50000)
        return "³¬¼¶´óÀÏ";
    return "·µè±¹éÕæ";
}

char *cexp2(int exp)
{
    if (exp <= 100)
        return "ÁÐ±ø";
    if (exp <= 300)
        return "ÉÏµÈ±ø";
    if (exp <= 500)
        return "ÏÂÊ¿";
    if (exp <= 700)
        return "ÖÐÊ¿";
    if (exp <= 1000)
        return "ÉÏÊ¿";
    if (exp <= 1500)
        return "¾üÊ¿³¤";
    if (exp <= 2000)
        return "ÉÙÎ¾";
    if (exp <= 3000)
        return "ÖÐÎ¾";
    if (exp <= 4000)
        return "ÉÏÎ¾";
    if (exp <= 5000)
        return "ÉÙÐ£";
    if (exp <= 6000)
        return "ÖÐÐ£";
    if (exp <= 7000)
        return "ÉÏÐ£";
    if (exp <= 8000)
        return "×¼½«";
    if (exp <= 9000)
        return "ÉÙ½«";
    if (exp <= 10000)
        return "ÖÐ½«";
    if (exp <= 12000)
        return "ÉÏ½«";
    return "ÔªË§";
}

char *cexp3(int exp)
{
    if (exp <= 150)
        return "»¨×Ñ";
    if (exp <= 500)
        return "ÎÞÃûÐ¡»¨";
    if (exp <= 2000)
        return "¿Õ¹ÈÓÄÀ¼";
    if (exp <= 3000)
        return "ÓÀºãÃµ¹å";
    if (exp <= 10000)
        return "°ÙºÏÌìÏÉ";
    return "»¨Éñ";
}

char *cperf(int perf)
{
    if (perf == -9999)
        return "Ã»µÈ¼¶";
    if (perf <= 5)
        return "¸Ï¿ì¼ÓÓÍ";
    if (perf <= 12)
        return "Å¬Á¦ÖÐ";
    if (perf <= 35)
        return "»¹²»´í";
    if (perf <= 50)
        return "ºÜºÃ";
    if (perf <= 90)
        return "ÓÅµÈÉú";
    if (perf <= 140)
        return "Ì«ÓÅÐãÁË";
    if (perf <= 200)
        return "±¾Õ¾Ö§Öù";
    if (perf <= 500)
        return "Éñ¡«¡«";
    if (perf <= 2500)
        return "»úÆ÷ÈË!";
    return "³¬¼¶»úÆ÷ÈË!";
}

int count_life_value(struct userec *urec)
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
        return (666 * 1440 - value) / 1440;
    }
    if (urec->numlogins <= 3)
        return (15 * 1440 - value) / 1440;
    if (!(urec->userlevel & PERM_LOGINOK))
        return (30 * 1440 - value) / 1440;
    if (urec->stay > 1000000)
        return (365 * 1440 - value) / 1440;
    return (120 * 1440 - value) / 1440;
}

int countexp(struct userec *x)
{
    int tmp;

    if (!strcasecmp(x->userid, "guest"))
        return -9999;
    tmp = x->numposts + x->numlogins / 3 + (time(0) -
                                            x->firstlogin) / 86400 +
        x->stay / 3600;
    if (tmp < 0)
        tmp = 0;
    return tmp;
}

int countperf(struct userec *x)
{
    int day, logins, posts;

    if (!strcasecmp(x->userid, "guest"))
        return -9999;
    day = (time(0) - x->firstlogin) / 86400 + 1;
    logins = x->numlogins;
    posts = x->numposts;
    if (day <= 0 || logins <= 0 || posts < 0)
        return 0;
    return (10 * posts / logins + 10 * logins / day);
}

int modify_mode(struct user_info *x, int newmode)
{
    if (x == 0)
        return -1;
    x->mode = newmode;
    return 0;
}

int save_user_data(struct userec *x)
{
    FILE *fp;
    int n;

    n = getusernum(x->userid);
    if (n < 0 || n > 1000000)
        return 0;
    fp = fopen(".PASSWDS", "r+");
    if (fp == 0)
        return 0;
    fseek(fp, n * sizeof(struct userec), SEEK_SET);
    fwrite(x, sizeof(struct userec), 1, fp);
    fclose(fp);
    return 1;
}

int is_bansite(char *ip)
{
    FILE *fp;
    char buf3[256];

    fp = fopen(".bansite", "r");
    if (fp == 0)
        return 0;
    while (fscanf(FCGI_ToFILE(fp), "%s", buf3) > 0)
        if (!strcasecmp(buf3, ip))
            return 1;
    fclose(fp);
    return 0;
}

int user_perm(struct userec *x, int level)
{
    return (x->userlevel & level);
}

int getusernum(char *id)
{
    int i;

    if (id[0] == 0)
        return -1;
    for (i = 0; i < MAXUSERS; i++)
    {
        if (!strcasecmp(shm_ucache->userid[i], id))
            return i;
    }
    return -1;
}

int getuser_like(char *userid)
{
    int i;

    for (i = 0; i < MAXUSERS; i++)
    {
        if (str_is_like(userid, shm_ucache->userid[i]))
            return i + 1;
    }
    return 0;
}

int str_is_like(char *userid, char *userid2)
{
    int i;

    if (strlen(userid) != strlen(userid2))
        return 0;
    for (i = 0; userid[i] && i < 14; i++)
    {
        if (toupper(userid[i]) == toupper(userid2[i]))
            continue;
        if (strchr("0Oo", userid[i]) && strchr("0Oo", userid2[i]))
            continue;
        if (strchr("Il1", userid[i]) && strchr("Il1", userid2[i]))
            continue;
        return 0;
    }
    return 1;
}

struct userec *getuser(char *id)
{
    static struct userec userec1;
    int uid;
    FILE *fp;

    if (id == NULL || id[0] == 0)
        return 0;
    uid = getusernum(id);
    if (uid < 0)
        return 0;
    fp = fopen(".PASSWDS", "r");
    fseek(fp, uid * sizeof(userec1), SEEK_SET);
    bzero(&userec1, sizeof(userec1));
    fread(&userec1, sizeof(userec1), 1, fp);
    fclose(fp);
    return &userec1;
}

int checkpasswd(char *pw_crypted, char *pw_try)
{
    return !strcmp(crypt1(pw_try, pw_crypted), pw_crypted);
}

char *crypt_passwd(char *pw)
{
    char salt[3];

    sprintf(salt, "%c%c", 65 + rand() % 26, 65 + rand() % 26);
    return crypt1(pw, salt);
}

int checkuser(char *id, char *pw)
{
    struct userec *x;

    x = getuser(id);
    if (x == 0)
        return 0;
    return checkpasswd(x->passwd, pw);
}

int count_id_num(char *id)
{
    int i, total = 0;

    for (i = 0; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active
            && !strcasecmp(shm_utmp->uinfo[i].userid, id))
            total++;
    return total;
}

int count_online()
{
    int i, total = 0;

    for (i = 0; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active)
            total++;
    return total;
}

int count_online2()
{
    int i, total = 0;

    for (i = 0; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active && shm_utmp->uinfo[i].invisible == 0)
            total++;
    return total;
}

struct override fff[200];
int friendnum = 0;
int friend_inited = 0;

int loadfriend(char *id)
{
    FILE *fp;
    char file[256];

    friend_inited = 1;
    if (!loginok)
        return -1;
    sprintf(file, "home/%c/%c/%s/friends", toupper(id[0]), toupper(id[1]),
            id);
    fp = fopen(file, "r");
    if (fp)
    {
        friendnum = fread(fff, sizeof(fff[0]), 200, fp);
        fclose(fp);
    }
    return 0;
}

int isfriend(char *id)
{
    int n;

    if (!friend_inited)
        loadfriend(currentuser.userid);
    for (n = 0; n < friendnum; n++)
        if (!strcasecmp(id, fff[n].id))
            return 1;
    return 0;
}

struct override bbb[32];
int badnum = 0;

int loadbad(char *id)
{
    FILE *fp;
    char file[256];

    if (!loginok)
        return -1;
    sprintf(file, "home/%c/%c/%s/rejects", toupper(id[0]), toupper(id[1]),
            id);
    fp = fopen(file, "r");
    if (fp)
    {
        badnum = fread(bbb, sizeof(bbb[0]), 30, fp);
        fclose(fp);
    }
    return 0;
}

int isbad(char *id1, char *id2)
{
    int n;

    loadbad(id1);
    for (n = 0; n < badnum; n++)
        if (!strcasecmp(id2, bbb[n].id))
            return 1;
    return 0;
}
int user_json() {
    if (loginok) {
        printf("<script type=\"JavaScript\">");
        printf("myself={");
        printf("  userid : \"%s\","
               "  nick   : \"%s\","
               "  ip     : \"%s\","
               "  level  : %u    ,"
               "  gender : \"%c\","
               "  unreadMail : %d "
               , currentuser.userid, currentuser.username, fromhost, currentuser.userlevel, currentuser.gender, 0
              );
        printf("};");
        printf("</script>");
    }
    else if (!strcasecmp("guest", currentuser.userid)) {
        printf("<script type=\"JavaScript\">");
        printf("myself={");
        printf("  userid : \"guest\","
               "  nick   : \"guest\","
               "  ip     : \"%s\","
               "  level  : 0    ,"
               "  gender : \"M\","
               "  unreadMail : 0"
	       , fromhost
              );
        printf("};");
        printf("</script>");
    }

}
int init_all()
{
    bbs_http_init();

    loginok = user_init(&currentuser, &u_info);
    g_message("%s %s %s?%s", cgiRemoteAddr, currentuser.userid,
              cgiScriptName, cgiQueryString);
    pic_mod = 2;
    if (loginok || !strcasecmp("guest", currentuser.userid))
        u_info->idle_time = time(0);
    user_json();
    load_my_www();
    if (!silence && my_font_mode)
    {
        printf("<style type='text/css'>\n");
        printf("TABLE,TH,TD,P,INPUT,BODY,SELECT {FONT-SIZE: 14px;}");
        printf("</style>\n");
    }
    if (!silence && !my_fontn_mode)
    {
        printf("<style type='text/css'>\n");
        printf
            ("TABLE,TH,TD,P,INPUT,BODY,SELECT {FONT-FAMILY: \"ËÎÌå\";}</style>\n");
        printf("</style>\n");
    }
    if (!silence && !my_ansisize_mode)
    {
        printf("<style type='text/css'>\n");
        printf
            ("PRE, TEXTAREA {FONT-SIZE: 9pt; line-height: 9pt}</style>\n");
        printf("</style>\n");
    }
    if (!silence && my_ansifont_mode)
    {
        printf("<style type='text/css'>\n");
        printf
            ("PRE, TEXTAREA {FONT-FAMILY: \"Verdana\", Tahoma, sans-serif, ËÎÌå;}</style>\n");
        printf("</style>\n");
    }
    if (!silence)
    {
        if (my_theme_mode == 1)
            printf("<link rel=stylesheet type=text/css href='"
                   CSS_HEADER "blue.css'>\n");
        else if (my_theme_mode == 2)
            printf("<link rel=stylesheet type=text/css href='"
                   CSS_HEADER "red.css'>\n");
    }
    return 0;
}

char *void1(char *s)
{
    int i;
    int flag = 0;

    for (i = 0; s[i]; i++)
    {
        if (flag == 0)
        {
            if (s[i] > '\x80' && s[i] < '\xFF')
                flag = 1;
        }
        else
        {
            flag = 0;
            if (s[i] < '\x20' && s[i] > '\0')
                s[i - 1] = '\x20';
        }
    }
    if (flag)
        s[i - 1] = '\0';
    return s;
}

char *sec(char c)
{
    int i;

    for (i = 0; i < SECNUM; i++)
    {
        if (strchr(seccode[i], c))
            return secname[i][0];
    }
    return "(unknown.)";
}

char *flag_str(int access)
{
    static char buf[80];
    char *flag1 = "", *flag2 = "";

    strcpy(buf, "  ");
    if (access & FILE_DIGEST)
        flag2 = "G";
    if (access & FILE_MARKED)
        flag2 = "M";
    if ((access & FILE_MARKED) && (access & FILE_DIGEST))
        flag2 = "B";
    sprintf(buf, "%s%s", flag1, flag2);
    return buf;
}

char *flag_str2(int access)
{
    static char buf[80];

    strcpy(buf, "   ");
    if (access & FILE_DIGEST)
        buf[0] = 'G';
    if (access & FILE_MARKED)
        buf[0] = 'M';
    if ((access & FILE_MARKED) && (access & FILE_DIGEST))
        buf[0] = 'B';
    return buf;
}

char *userid_str(char *s)
{
    static char buf[512];
    char buf2[256], tmp[256], *ptr;
//    char *ptr2;
    int len;

    strsncpy(tmp, s, 255);
    buf[0] = 0;
    ptr = strtok(tmp, " ,();\r\n\t");
    while (ptr && strlen(buf) < 400)
    {
        ptr = nohtml(ptr);
        if (strncmp(ptr, "Anony.", 6) == 0)
            sprintf(buf2, "<a href=\"bbsqry?userid='comment'\">comment</a> ");
        else
            sprintf(buf2, "<a href=\"bbsqry?userid=%s\">%s</a> ", ptr, ptr);
        strcat(buf, buf2);
        ptr = strtok(0, " ,();\r\n\t");
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == ' ')
        buf[len - 1] = 0;
    return buf;
}

int char_in_url(char c)
{
    char *invalid_char = "\'\" \r\t)(;\n|\\^~[]`";
    int i;

    if (c < 0)
        return 0;
    for (i = 0; invalid_char[i]; ++i)
    {
        if (c == invalid_char[i])
            return 0;
    }
    return 1;
}

int fprintf2(FILE * fp, char *s, int *tail, int *sum, int *in_link)
{
    int i;

    if (s[0] == ':' && s[1] == ' ' && strlen(s) > 79)
    {
        sprintf(s + 76, "..\n");
        fprintf(fp, "%s", s);
        *tail = *sum = *in_link = 0;
        return 0;
    }
    for (i = 0; s[i]; i++)
    {
        if (*in_link)
        {
            if (char_in_url(s[i]))
            {
                (*in_link)++;
            }
            else
            {
                *in_link = 0;
            }
        }
        else if (strncasecmp(s + i, "http://", 7) == 0 ||
                 strncasecmp(s + i, "ftp://", 6) == 0 ||
                 strncasecmp(s + i, "mailto:", 7) == 0)
        {
            *in_link = 1;
        }
        if (*sum >= 78 && *tail == 0 && (!(*in_link) || *in_link > 128))
        {
            fprintf(fp, "\n");
            *sum = 0;
            *in_link = 0;
            if (s[i] == '\n')
            {
                continue;
            }    
        }
        fprintf(fp, "%c", s[i]);
        (*sum)++;
        if (*tail)
        {
            *tail = 0;
        }
        else if (s[i] < 0)
        {
            *tail = s[i];
        }
    }
    if (strlen(s) < 999)
    {
        *sum = *tail = *in_link = 0;
    }    
    return 0;
}

struct fileheader *get_file_ent(char *board, char *file, int isPost)
{
    FILE *fp;
    char dir[80];
    static struct fileheader x;
    int num = 0;

    if (1 == isPost)
    {
        sprintf(dir, "boards/%s/.DIR", board);
    }
    else
    {
        sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid);
    }
    fp = fopen(dir, "r");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        if (!strcmp(x.filename, file))
        {
            fclose(fp);
            return &x;
        }
        num++;
    }
    fclose(fp);
    return 0;
}

char *getbfroma(char *path)
{
    FILE *fp;
    static char buf1[180], buf2[180];

    if (path[0] == 0)
        return "";
    path++;
    fp = fopen("0Announce/.Search", "r");
    if (fp == 0)
        return "";
    while (1)
    {
        bzero(buf1, sizeof(buf1));
        bzero(buf2, sizeof(buf2));
        if (fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2) <= 0)
            break;
        if (buf1[0])
            buf1[strlen(buf1) - 1] = 0;
        if (buf1[0] == '*')
            continue;
        if (!strncmp(buf2, path, strlen(buf2))
            && path[strlen(buf2)] == '/')
        {
            fclose(fp);
            return buf1;
        }
    }
    fclose(fp);
    return "";
}

int load_my_www()
{
    FILE *fp;
    char path[256], buf[256], buf1[256], buf2[256];

    if (!loginok)
        return -1;
    sprintf(path, "home/%c/%c/%s/.mywww", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    fp = fopen(path, "r");
    if (fp)
    {
        while (1)
        {
            if (fgets(buf, 80, fp) == 0)
                break;
            if (sscanf(buf, "%80s %80s", buf1, buf2) != 2)
                continue;
            if (!strcmp(buf1, "t_lines"))
                my_t_lines = atoi(buf2);
            if (!strcmp(buf1, "link_mode"))
                my_link_mode = atoi(buf2);
            if (!strcmp(buf1, "def_mode"))
                my_def_mode = atoi(buf2);
            if (!strcmp(buf1, "font_mode"))
                my_font_mode = atoi(buf2);
            if (!strcmp(buf1, "fontn_mode"))
                my_fontn_mode = atoi(buf2);
            if (!strcmp(buf1, "ansisize_mode"))
                my_ansisize_mode = atoi(buf2);
            if (!strcmp(buf1, "ansifont_mode"))
                my_ansifont_mode = atoi(buf2);
            if (!strcmp(buf1, "theme_mode"))
                my_theme_mode = atoi(buf2);
        }
        fclose(fp);
    }
    return 0;
}

int has_fill_form()
{
    FILE *fp;

    struct regform form;

    memset(&form, 0, sizeof(form));
    fp = fopen("register_sjtubbs", "r");
    if (fp == NULL)
        return 0;
    while (fread(&form, sizeof(struct regform), 1, fp) > 0)
    {
        if (strncmp(form.userid, currentuser.userid, IDLEN) == 0)
        {
            fclose(fp);
            return 1;
        }
        memset(&form, 0x0, sizeof(struct regform));
    }
    fclose(fp);
    return 0;

}

char *nohtml2(char *s)
{
    static char buf[1024];

    hsprintf(buf, "%s", s);
    return buf;
}

char *void3(char *s)
{
    static char buf[1024];
    int i = 0, j = 0;

    for (i = 0; s[i] && j < 1000; i++)
    {
        if (s[i] == 34)
        {
            sprintf(buf + j, "&#34;");
            j += 5;
        }
        else if (s[i] == 39)
        {
            sprintf(buf + j, "&#39;");
            j += 5;
        }
        else
        {
            buf[j] = s[i];
            j++;
        }
    }
    buf[j] = 0;
    return buf;
}

float get_load(int i)
{
    static float x[3];
    static int last_atime = 0;
    FILE *fp;

    if (i < 0 || i > 2)
        return -1;
    if (abs(time(0) - last_atime) <= 5)
        return x[i];
    last_atime = time(0);
    fp = fopen("/proc/loadavg", "r");
    if (fp == 0)
        return -1;
    fscanf(FCGI_ToFILE(fp), "%g %g %g", &x[0], &x[1], &x[2]);
    fclose(fp);
    return x[i];
}

void *file_map(char *file, int len)
{
    FILE *fp;
    void *buf;

    fp = fopen(file, file_exist(file) ? "r+" : "w+");
    if (fp == 0)
        return 0;
    if (file_size(file) != len)
        ftruncate(fileno(fp), len);
    buf = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fp), 0);
    fclose(fp);
    if (buf == MAP_FAILED)
        buf = 0;
    if (buf == 0)
        return 0;
    return buf;
}

int getbmns(char *userid)
{
    FILE *bmfp = NULL;
    char bmfilename[256], tmp[20];
    int i = 0;

    sethomefile(bmfilename, userid, ".bmfile");
    bmfp = fopen(bmfilename, "r");
    if (!bmfp)
        return 0;
    for (i = 0; i < 3; i++)
    {
        fscanf(FCGI_ToFILE(bmfp), "%s\n", tmp);
        if (feof(bmfp))
            break;
    }

    fclose(bmfp);
    return (i + 1);
}

int check_maxmail2(int *maxmail, int *size)
{
    char filename[256], cmd[256], mailpath[256], maildir[256], tmpf[256];
    FILE *tempfile, *fp;
    int i;

    *size = 0;
    *maxmail = 5000;
    if (!loginok)
        return 0;
    if (HAS_PERM(PERM_BASIC))
        *maxmail = 2000;
    if (HAS_PERM(PERM_BOARDS))
    {
        i = getbmns(currentuser.userid);
        *maxmail = 4000 + 600 * (i - 1);
    }
    if (HAS_PERM(PERM_CHATCLOAK))
        *maxmail = 6000;
    sprintf(mailpath, "mail/%c/%c/%s/", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    sprintf(tmpf, "%s.size", mailpath);
    sprintf(maildir, "%s.DIR", mailpath);
    if (file_time(maildir) == file_time(tmpf))
    {
        fp = fopen(tmpf, "r");
        if (fp)
        {
            fscanf(FCGI_ToFILE(fp), "%d %d", maxmail, size);
            fclose(fp);
        }
        return 1;
    }
    sprintf(filename, "/tmp/size.%s.%d", currentuser.userid, u_info->pid);
    sprintf(cmd, "/usr/bin/du -bk %s > %s", mailpath, filename);
    system(cmd);
    if ((tempfile = fopen(filename, "r")) == NULL)
        return 0;
    if ((fscanf(FCGI_ToFILE(tempfile), "%d", size)) == 0)
        return 0;
    fclose(tempfile);
    unlink(filename);
    fp = fopen(tmpf, "w");
    fprintf(fp, "%d %d\n", *maxmail, *size);
    fclose(fp);
    currentuser.nummails = file_size(maildir) / sizeof(struct fileheader);
    save_user_data(&currentuser);
    return 2;
}

char *nohtml3(char *s)
{
    static char buf[512];
    int i, j = 0;

    bzero(buf, 512);
    for (i = 0; s[i] && j < 500; i++)
    {
        if (s[i] == '\'')
        {
            strcat(buf, "%27");
            j += 3;
        }
        else if (s[i] == '\"')
        {
            strcat(buf, "%22");
            j += 3;
        }
        else
        {
            buf[j] = s[i];
            j++;
        }
    }
    return buf;
}

char *nohtml5(char *s)
{

    static unsigned char hexchars[] = "0123456789ABCDEF";
    static char buf[512];
    int i, j = 0;

    bzero(buf, 512);

    for (i = 0, j = 0; s[i] && j < 500; ++i, ++j)
    {
        buf[j] = (unsigned char) s[i];
        if ((buf[j] < '/' && buf[i] != '-' && buf[i] != '.')
            || (buf[i] < 'A' && buf[i] > '9') || (buf[i] > 'Z'
                                                  && buf[i] < 'a'
                                                  && buf[i] != '_')
            || (buf[i] > 'z'))
        {
            buf[j++] = '%';
            buf[j++] = hexchars[(unsigned char) s[i] >> 4];
            buf[j] = hexchars[(unsigned char) s[i] & 15];
        }
    }
    buf[j] = '\0';

    return buf;

}

char *nohtml4(char *s)
{
    static char buf[2048];
    int i, j = 0;

    bzero(buf, 2048);
    for (i = 0; s[i] && i < 2030; i++)
    {
        if (s[i] == '&')
        {
            strcat(buf, "&amp;");
            j += 5;
        }
        else if (s[i] == '<')
        {
            strcat(buf, "&lt;");
            j += 4;
        }
        else if (s[i] == '>')
        {
            strcat(buf, "&gt;");
            j += 4;
        }
        else
        {
            buf[j] = s[i];
            j++;
        }
    }
    return buf;
}

static char *days[] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

static char *months[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

char *Gtime(time_t t)
{
    static char buf[64];
    struct tm *gt;

    gt = gmtime(&t);
    sprintf(buf,
            "%s, %02d-%s-%04d %02d:%02d:%02d GMT",
            days[gt->tm_wday],
            gt->tm_mday,
            months[gt->tm_mon],
            gt->tm_year + 1900, gt->tm_hour, gt->tm_min, gt->tm_sec);
    return buf;
}

struct tm *Ltime(time_t t)
{
    return localtime(&t);
}

int Day(time_t t)
{
    struct tm *p;

    p = Ltime(t);
    return (p->tm_year + 1900) * 10000 + (p->tm_mon + 1) * 100 +
        p->tm_mday;
}

int today(void)
{
    return Day(time(0));
}

char *eff_size(char *file)
{
    static char buf[256];
    int size;

    size = (file_size(file) - 200) / 2;

    if (size < 0)
        size = 0;
    if (size < 2048)
    {
        sprintf(buf,
                "(<font style='font-size:12px; color:#008080'>%d×Ö</font>)",
                size);
    }
    else
    {
        sprintf(buf,
                "(<font style='font-size:12px; color:#f00000'>%d.%dK</font>)",
                size / 1000, (size / 100) % 10);
    }
    return buf;
}

int base64(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    if (c >= '0' && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return 0;
}

char *unbase64(char *buf)
{
    static char buf2[256];
    int len = strlen(buf), i, j = 0;

    for (i = 0; i < len; i += 4)
    {
        if (buf[i] == '=' || buf[i] < ' ')
            break;
        buf2[j] = 4 * base64(buf[i]) + base64(buf[i + 1]) / 16;
        j++;
        if (buf[i + 2] == '=' || buf[i + 2] < ' ')
            break;
        buf2[j] = 16 * (base64(buf[i + 1]) % 16) + base64(buf[i + 2]) / 4;
        j++;
        if (buf[i + 3] == '=' || buf[i + 3] < ' ')
            break;
        buf2[j] = 64 * (base64(buf[i + 2]) % 4) + base64(buf[i + 3]);
        j++;
    }
    buf2[j] = 0;
    return buf2;
}

int junkboard(char *board)
{
    if (strstr(JUNKBOARDS, board))
        return 1;
    else
        return file_has_word("etc/junkboards", board);

}

int LoadMyBrdsFromGoodBrd(char *userid, char mybrd[GOOD_BRC_NUM][80])
{
    FILE *fp;
    char fname[STRLEN];
    int i = 0;

    sprintf(fname, "home/%c/%c/%s/.goodbrd", toupper(userid[0]),
            toupper(userid[1]), userid);
    if ((fp = fopen(fname, "r")))
    {
        for (i = 0; i < GOOD_BRC_NUM; i++)
        {
            if (EOF == fscanf(FCGI_ToFILE(fp), "%s\n", mybrd[i]))
                break;
        }
        fclose(fp);
    }
    return i;
}

void
SaveMyBrdsToGoodBrd(char *userid,
                    char mybrd[GOOD_BRC_NUM][80], int mybrdnum)
{
    FILE *fp;
    char fname[STRLEN];
    int i;

    sprintf(fname, "home/%c/%c/%s/.goodbrd", toupper(userid[0]),
            toupper(userid[1]), userid);
    if ((fp = fopen(fname, "w")))
    {
        for (i = 0; i < mybrdnum; i++)
            fprintf(fp, "%s\n", mybrd[i]);
        fclose(fp);
    }

}

void
post_article2(char *board, char *title, char *file, char *id,
              unsigned int currid, unsigned int reid)
{
    FILE *fp;
    char buf[1024];
    struct fileheader header;

    bzero(&header, sizeof(header));

    strcpy(header.owner, id);
    header.id = currid;
    header.reid = reid;

    sprintf(header.filename, "%s", file);

    sprintf(header.title, "%-32.32s - %s", title, currentuser.userid);

    header.accessed[0] = FILE_DELETED;
    sprintf(buf, "boards/%s/.RETURNDIR", board);
    fp = fopen(buf, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
}

void strtolower(char *dst, char *src)
{
    for (; *src; src++)
        *dst++ = tolower(*src);
    *dst = '\0';
}

void log_usies(char *mode, char *mesg)
{

    g_message("%s %s %s", currentuser.userid, mode, mesg);
}

int return_del_article(char *file, int size, int num)
{
    FILE *fp;
    int total, i;
    char buf[4096];

    if (size < 1 || size > 4096)
        return 0;
    total = file_size(file) / size;
    if (total < 1 || total > 1000000)
        return 0;
    fp = fopen(file, "r+");
    if (fp == 0)
        return 0;
    flock(fileno(fp), LOCK_EX);
    for (i = num + 1; i <= total - 1; i++)
    {
        fseek(fp, i * size, SEEK_SET);
        if (fread(buf, size, 1, fp) <= 0)
            break;
        fseek(fp, (i - 1) * size, SEEK_SET);
        fwrite(buf, size, 1, fp);
    }
    ftruncate(fileno(fp), (total - 1) * size);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    return 1;
}

int too_young(struct userec *user, char *board)
{
    if (strcmp(board, "BBSHelp") == 0)
        return 0;

    if (strcmp(board, "sysop") == 0)
        return 0;

    if ((time(0) - user->firstlogin) > 86400)
        return 0;

    return 1;
}

int kick_web_user(struct user_info *user)
{
    int stay = 0;
    FILE *fp;
    struct userec u;

    if (!user->active)
        return -1;
    stay = abs(time(0) - user->logintime);
    if (stay > 3600 * 24 * 3)
        stay = 3600 * 24 * 3;
    else if (stay < 0)
        stay = 0;

    fp = fopen(".PASSWDS", "r+");
    fseek(fp, (user->uid - 1) * sizeof(struct userec), SEEK_SET);
    fread(&u, sizeof(struct userec), 1, fp);
    if (strcasecmp(u.userid, user->userid))
    {
        fclose(fp);
        memset(user, 0, sizeof(struct user_info));
        return -2;
    }
    u.stay += stay;
    u.lastlogout = time(0);
    fseek(fp, -sizeof(struct userec), SEEK_CUR);
    fwrite(&u, sizeof(struct userec), 1, fp);
    fclose(fp);
    memset(user, 0, sizeof(struct user_info));
    return 0;
}

void show_dm(char *userid)
{
    int dist = 12;

    dist = WHICHDIST(userid);
    if ((dist < 12) && (dist > 0))
    {
        if (dist < 10)
            printf(" [<a href=bbsdoc?board=District%c><font class=c31>%d <font class=c32>ÇøÇø³¤</font></a><font class=c37>]", '0' + dist, dist); 
        else
        {
            if (dist == 10)
                printf(" [<a href=bbsdoc?board=DistrictA><font class=c31>A <font class=c32>ÇøÇø³¤</font></a><font class=c37>]"); 
            if (dist == 11)
                printf(" [<a href=bbsdoc?board=DistrictB><font class=c31>B <font class=c32>ÇøÇø³¤</font></a><font class=c37>]"); 
        }
    }
    else
        printf(" [<font class=c32>±¾Õ¾Çø³¤</font><font class=c37>]");
    return;
}

void show_bm(char firstchar, char *userid)
{
    FILE *stream;
    int i = 0;
    int f = 0;
    char bm_file_path[64], buffer[64], *ch;

    sprintf(bm_file_path, "home/%c/%c/%s/.bmfile", firstchar,
            toupper(userid[1]), userid);

    if ((stream = fopen(bm_file_path, "r")) == NULL)
        printf(" [<font class=c32>±¾Õ¾°åÖ÷</font><font class=c37>]");
    else
    {

        while (fgets(buffer, 32, stream) && i < 4)
        {
            if ((ch = strchr(buffer, 10)))
                *ch = '\0';
            if (strncmp(buffer, "District", 8))
            {
                if (f == 0)
                    printf(" [");
                f = 1;
                printf("<a href=bbsdoc?board=%s><font class=c31>%s</a> ", buffer, buffer); 
            }
            i++;
        }
        fclose(stream);
        if (f)
            printf("<font class=c32>°å°åÖ÷</font><font class=c37>]");
    }
    return;
}

static struct
{
    struct fileheader *add;
    size_t len;
    int size;
    int offset;
} _map_dir =
{
0, 0, -1, 0};
void *map_board_dir(char *board, char *file)
{
    char buf[1024];
    int fd;

    sprintf(buf, "boards/%s/%s", board, file);
    _map_dir.len = file_size(buf);
    if (_map_dir.len == 0)
        return 0;
    fd = open(buf, O_RDONLY);
    if (fd == -1)
        return 0;
    _map_dir.add =
        (struct fileheader *) mmap(NULL, _map_dir.len, PROT_READ,
                                   MAP_PRIVATE, fd, 0);
    _map_dir.size = _map_dir.len / sizeof(struct fileheader);
    if (_map_dir.add == MAP_FAILED)
    {
        _map_dir.add = 0;
        _map_dir.len = 0;
    }
    close(fd);
    return _map_dir.add;
}

int unmap_board_dir(void)
{
    if (_map_dir.add == 0)
        return 0;
    if (munmap(_map_dir.add, _map_dir.len) != -1)
    {
        _map_dir.len = 0;
        _map_dir.add = 0;
        _map_dir.size = 0;
        _map_dir.offset = -1;
        return 1;
    }
    else
        return 0;
}
struct fileheader *find_in_map_dir(char *filename)
{
    int size, i;
    struct fileheader *fh;

    if (0 == _map_dir.add)
        return NULL;
    fh = _map_dir.add;
    size = _map_dir.size;
    for (i = size - 1; i >= 0; --i)
    {
        if (!strcmp(fh[i].filename, filename))
        {
            _map_dir.offset = i;
            break;
        }
    }
    if (i >= 0)
        return &fh[i];
    else
    {
        _map_dir.offset = -1;
        return NULL;
    }
}
struct fileheader *pre_in_map_dir(void)
{
    if (0 == _map_dir.add || _map_dir.offset <= 0)
        return NULL;
    else
        return &_map_dir.add[_map_dir.offset - 1];
}
struct fileheader *next_in_map_dir(void)
{
    if (0 == _map_dir.add || -1 == _map_dir.offset
        || _map_dir.size == _map_dir.offset + 1)
        return NULL;
    else
        return &_map_dir.add[_map_dir.offset + 1];
}

int get_offset_of_mapdir(void)
{
    return _map_dir.offset;
}

int set_offset_of_mapdir(int offset)
{
    if (0 == _map_dir.add || _map_dir.size <= offset || 0 > offset)
        return 0;
    _map_dir.offset = offset;
    return 1;
}

int get_size_of_mapdir(void)
{
    return _map_dir.size;
}

int print_style(void)
{
    if (my_theme_mode == 1)
        printf("<link rel=stylesheet type=text/css href='"
               CSS_HEADER "blue.css'>\n");
    else
        printf("<link rel=stylesheet type=text/css href='"
               CSS_HEADER "red.css'>\n");
    return 0;
}

static int is_head_printed = 0;

static CALLBACK_RETURN reset_kernel(CALLBACK_ID callback_id, void *data,
                                    void *userdata)
{
    loginok = 0;
    silence = 0;
    bzero(&currentuser, sizeof(currentuser));
    u_info = NULL;
    bzero(fromhost, sizeof(fromhost));
    my_link_mode = 0;
    my_def_mode = 0;
    my_font_mode = 0;
    my_fontn_mode = 1;
    my_ansisize_mode = 1;
    my_ansifont_mode = 0;
    my_t_lines = 20;
    my_theme_mode = 1;
    friendnum = 0;
    friend_inited = 0;
    bzero(fff, sizeof(fff));
    badnum = 0;
    bzero(bbb, sizeof(bbb));
    is_normal_board = 0;
    bzero(denyuser, sizeof(denyuser));
    denynum = 0;
    is_head_printed = 0;
    last_modified_time = 0;
    cache_age = HEADER_NO_CACHE;
    return CALLBACK_CONTINUE;
}
static CALLBACK_RETURN bbswww_final(CALLBACK_ID callback_id, void *data,
                                    void *userdata)
{
    CALLBACK_ID *cid = (CALLBACK_ID *) userdata;

    remove_callback(*cid);
    g_free(cid);
    return CALLBACK_CONTINUE;
}

gboolean bbswww_init(EVENT_ID unload_event, EVENT_ID enter_event,
                     EVENT_ID exit_event)
{
    CALLBACK_ID *cid = g_malloc(sizeof(CALLBACK_ID *));

    *cid = register_callback(enter_event, reset_kernel, NULL);
    register_callback(unload_event, bbswww_final, (void *) cid);
    return TRUE;
}

int show_bm_words(char *board, int width)
{
    FILE *fp;
    char buf[256], buf2[256] = "";
    char *t;

    sprintf(buf, "vote/%s/note2", board);
    fp = fopen(buf, "r");
    if (!fp)
        return -1;
    fgets(buf2, 254, fp);
    buf2[254] = 0;
    t = strchr(buf2, '\n');
    if (t)
        *t = '\0';
    fclose(fp);
    if (strlen(buf2) == 0)
        return -1;
    if (loginok && has_BM_perm(&currentuser, board))
    {
        printf
            ("<table width=%d><tr style='height:14px'><td><b><a href=bbsmnote3?board=%s>°åÖ÷¼ÄÓï</a></b>: <font color='#308080'>",
             width, board);
        hprintf("%s", nohtml(buf2));
        printf("</font></table>");
    }
    else
    {
        printf
            ("<table width=%d><tr style='height:14px'><td><b>°åÖ÷¼ÄÓï</b>: <font color='#308080'>",
             width);
        hprintf("%s", nohtml(buf2));
        printf("</font></table>");
    }
    return 0;
}

#define BRC_MAXSIZE     70000
#define BRC_MAXNUM      60
#define BRC_STRLEN      15
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
static char brc_buf[BRC_MAXSIZE];
static int brc_size;
static char brc_name[BRC_STRLEN];
static int brc_list[BRC_MAXNUM], brc_num;

char *brc_getrecord(char *ptr, char *name, int *pnum, int *list)
{
    int num;
    char *tmp;

    strncpy(name, ptr, BRC_STRLEN);
    ptr += BRC_STRLEN;
    num = (*ptr++) & 0xff;
    tmp = ptr + num * sizeof(int);
    if (num > BRC_MAXNUM)
        num = BRC_MAXNUM;
    *pnum = num;
    memcpy(list, ptr, num * sizeof(int));
    return tmp;
}

char *brc_putrecord(char *ptr, char *name, int num, int *list)
{
    if (num > 0)
    {
        if (num > BRC_MAXNUM)
        {
            num = BRC_MAXNUM;
        }
        strncpy(ptr, name, BRC_STRLEN);
        ptr += BRC_STRLEN;
        *ptr++ = num;
        memcpy(ptr, list, num * sizeof(int));
        ptr += num * sizeof(int);
    }
    return ptr;
}

void brc_update(char *userid, char *board)
{
    char dirfile[STRLEN], *ptr;
    char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
    char tmp_name[BRC_STRLEN];
    int tmp_list[BRC_MAXNUM], tmp_num;
    int fd, tmp_size;

    ptr = brc_buf;
    if (brc_num > 0)
    {
        ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
    }
    if (1)
    {
        sethomefile(dirfile, userid, ".boardrc");
        if ((fd = open(dirfile, O_RDONLY)) != -1)
        {
            tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
            close(fd);
        }
        else
        {
            tmp_size = 0;
        }
    }
    tmp = tmp_buf;
    while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
    {
        tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
        if (strncmp(tmp_name, board, BRC_STRLEN) != 0)
        {
            ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
        }
    }
    brc_size = (int) (ptr - brc_buf);
    if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
    {
        ftruncate(fd, 0);
        write(fd, brc_buf, brc_size);
        close(fd);
    }
}

int brc_init(char *userid, char *boardname)
{
    FILE *fp;
    char dirfile[STRLEN], *ptr;

    sethomefile(dirfile, userid, ".boardrc");
    if ((fp = fopen(dirfile, "r")))
    {
        brc_size = fread(brc_buf, 1, BRC_MAXSIZE, fp);
        fclose(fp);
    }
    else
    {
        brc_size = 0;
    }
    ptr = brc_buf;
    while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
    {
        ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
        if (!strncasecmp(brc_name, boardname, BRC_STRLEN))
            return brc_num;
    }
    strncpy(brc_name, boardname, BRC_STRLEN);
    brc_list[0] = 1;
    brc_num = 1;
    return 0;
}

int brc_add_read(char *filename)
{
    int ftime, n, i;

    if (!loginok)
        return -1;
    if (filename[0] != 'M' && filename[0] != 'G')
        return -1;
    ftime = atoi(&filename[2]);
    if (brc_num <= 0)
    {
        brc_list[brc_num++] = ftime;
        return -1;
    }
    for (n = 0; n < brc_num; n++)
    {
        if (ftime == brc_list[n])
        {
            return -1;
        }
        else if (ftime > brc_list[n])
        {
            if (brc_num < BRC_MAXNUM)
                brc_num++;
            for (i = brc_num - 1; i > n; i--)
            {
                brc_list[i] = brc_list[i - 1];
            }
            brc_list[n] = ftime;
            return -1;
        }
    }
    if (brc_num < BRC_MAXNUM)
        brc_list[brc_num++] = ftime;
    return 0;
}

int brc_un_read(char *file)
{
    int ftime, n;

    if (file[0] != 'M' && file[0] != 'G')
        return 0;
    ftime = atoi(file + 2);
    if (brc_num <= 0)
        return 1;
    for (n = 0; n < brc_num; n++)
    {
        if (ftime > brc_list[n])
            return 1;
        if (ftime == brc_list[n])
            return 0;
    }
    return 0;
}

int brc_clear(void)
{
    int i;
    char filename[20];

    if (!loginok)
        return -1;
    for (i = 0; i < 60; i++)
    {
        sprintf(filename, "M.%d.A", (int) (time(0) - i));
        brc_add_read(filename);
    }
    return 0;
}

int brc_has_read(char *file)
{
    return !brc_un_read(file);
}

int do_cache(char *filename, char *cachefilename)
{
    FILE *fp, *fpcache;
    char buf[512], *ptr;
    int swap = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;
    fpcache = fopen(cachefilename, "w");
    if (fpcache == NULL)
    {
        strcpy(buf, cachefilename);
        ptr = strrchr(buf,'/');
        *ptr = '\0';
        g_mkdir_with_parents(buf,0700);
        if(NULL==(fpcache=fopen(cachefilename,"w")))
        {
            fclose(fp);
            return -2;
        }
    }
    while (1)
    {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "·¢ÐÅÈË: ", 8))
        {
            ptr = strdup(buf);
            id = strtok(ptr + 8, " ");
            s = strtok(0, "");
            if (id == 0)
                id = " ";
            if (s == 0)
                s = "\n";
            if (strlen(id) < 13 && getuser(id))
            {
                fprintf(fpcache, "·¢ÐÅÈË: %s", userid_str(id));
                hfprintf(fpcache, "%s", s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, "--", 2))
            pic_mod = 1;
        if (!strncmp(buf, ": ", 2))
        {
            fprintf(fpcache, "<font color=\"808080\">");
            swap = pic_mod;
            pic_mod = 0;
        }
        hhfprintf(fpcache, "%s", void1(buf));
        if (!strncmp(buf, ": ", 2))
        {
            fprintf(fpcache, "</font>");
            pic_mod = swap;
        }
    }
    fclose(fp);
    fclose(fpcache);
    return 0;
}

unsigned long ipintonum(char *ip)
{
    union ip_str
    {
        unsigned char ip_s[4];
        unsigned long ip_num;
    } ip_u;
    char buf[128];
    char *p;
    int i;

    sprintf(buf, "%s", ip);
    if (!strchr(buf, '.'))
        return 0;
    p = strtok(buf, ".");
    for (i = 3; i >= 0; i--)
    {
        ip_u.ip_s[i] = atoi(p);
        if ((p = strtok(NULL, ".")) == NULL)
            break;
    }
    return ip_u.ip_num;
}

int
ipinrange(unsigned long ipaddr, unsigned long network, unsigned long mask)
{
    if ((ipaddr & mask) == network)
        return 1;
    else
        return 0;
}

static int insjtunetwork_v6(char *ip)
{
    /* currently,  2001:da8:8000::/48 is used in sjtu */
    return strncmp(ip, "2001:da8:8000:", strlen("2001:da8:8000:"))==0;
}
int insjtunetwork(char *ip)
{
    struct ip_range
    {
        unsigned long network;
        unsigned long mask;
    } ip_range[10];
    int i;

    if (strchr(ip,':')!=NULL) /* if this is a ipv6 address */
    {
        return insjtunetwork_v6(ip);
    }
    ip_range[0].network = 0xca780000UL;
    ip_range[0].mask = 0xffffc000UL;
    ip_range[1].network = 0xd3502000UL;
    ip_range[1].mask = 0xffffe000UL;
    ip_range[2].network = 0xd3505000UL;
    ip_range[2].mask = 0xfffff000UL;
    ip_range[3].network = 0xca79b000UL;
    ip_range[3].mask = 0xfffff800UL;
    ip_range[4].network = 0xdac1b000UL;
    ip_range[4].mask = 0xfffff000UL;
    ip_range[5].network = 0xdbe46000UL;
    ip_range[5].mask = 0xffffe000UL;
    ip_range[6].network = 0xca701a00UL;
    ip_range[6].mask = 0xffffff00UL;
    ip_range[7].network = 0x3b4e0000UL;
    ip_range[7].mask = 0xffffc000UL;
    ip_range[8].network = 0xca788000UL;
    ip_range[8].mask = 0xfffff000UL;
    ip_range[9].network = 0x3ac48000UL;
    ip_range[9].mask = 0xffffc000UL;

    for (i = 0; i <= 10; i++)
    {
        if (ipinrange
            (ipintonum(ip), ip_range[i].network, ip_range[i].mask))
        {
            return 1;
        }

    }
    return 0;
}

int noregist(void)
{
    FILE *fp = NULL;

    fp = fopen("NOREGISTER", "r");
    if (fp)
    {
        fclose(fp);
        return 1;
    }
    else
        return 0;
}

#ifdef LOGHANDLER

static int loghandler_fd = -1;

static int loghandler_timeindex = -1;
static int loghandler_open_new(void)
{
    int randomindex, timeindex;
    char path[1024];

    randomindex = getpid() % LOGHANDLER_RANDOMINDEX_NUM;
    timeindex =
        time(NULL) / LOGHANDLER_ROTATING_TIME % LOGHANDLER_TIMEINDEX_NUM;
    snprintf(path, 1024, LOGHANDLER_PATH_FORMT, timeindex, randomindex);
    if (loghandler_fd > -1)
    {
        close(loghandler_fd);
    }
    loghandler_fd = open(path, O_CREAT | O_WRONLY | O_APPEND);
    if (loghandler_fd > -1)
    {
        loghandler_timeindex = timeindex;
    }
    return loghandler_fd;
}
int loghandler_get_fd(void)
{
    int timeindex;

    if (loghandler_fd < 0)
    {
        return loghandler_open_new();
    }
    timeindex =
        time(NULL) / LOGHANDLER_ROTATING_TIME % LOGHANDLER_TIMEINDEX_NUM;
    if (timeindex != loghandler_timeindex)
    {
        return loghandler_open_new();
    }
    return loghandler_fd;
}
int loghander_write(char *line, int len)
{
    int fd, ret;

    if ('\n' != line[len - 1])
        return -2;
    fd = loghandler_get_fd();
    if (fd < 0)
        return -1;
    else
    {
        ret = write(fd, line, len);
        return ret;
    }
}

int loghandler_writev(int linenum, ...)
{
    va_list ap;
    struct iovec *pvec;
    int i, ret = 0, fd, len;
    char *line;

    fd = loghandler_get_fd();
    if (fd < 0)
        return -1;
    pvec = (struct iovec *) malloc(linenum * sizeof(struct iovec));
    if (!pvec)
        return -1;
    va_start(ap, linenum);
    for (i = 0; i < linenum; ++i)
    {
        line = va_arg(ap, char *);
        len = va_arg(ap, int);

        if (line[len - 1] != '\n')
        {
            ret = -1;
            break;
        }
        pvec[i].iov_base = line;
        pvec[i].iov_len = len;
    }
    va_end(ap);
    if (ret < 0)
    {
        free(pvec);
        return ret;
    }
    ret = writev(fd, pvec, linenum);
    free(pvec);
    return ret;
}

int mysql_escape_string(char *dst, char *src, int len)
{
    int i;
    char *tmpdst = dst;

    for (i = 0; i < len; i++)
    {
        switch (src[i])
        {
        case '\n':
            *tmpdst++ = '\\';
            *tmpdst++ = 'n';
            break;
        case '\\':
            *tmpdst++ = '\\';
            *tmpdst++ = '\\';
            break;
        case '\r':
            *tmpdst++ = '\\';
            *tmpdst++ = 'r';
            break;
        case '\0':
            *tmpdst++ = '\\';
            *tmpdst++ = '0';
            break;
        case '\'':
            *tmpdst++ = '\\';
            *tmpdst++ = '\'';
            break;
        case '\"':
            *tmpdst++ = '\\';
            *tmpdst++ = '\"';
            break;
        case '\b':
            *tmpdst++ = '\\';
            *tmpdst++ = 'b';
            break;
        case '\t':
            *tmpdst++ = '\\';
            *tmpdst++ = 't';
            break;
        case 0x1A:
            *tmpdst++ = '\\';
            *tmpdst++ = 'Z';
            break;
        case '%':
            *tmpdst++ = '\\';
            *tmpdst++ = '%';
            break;
        case '_':
            *tmpdst++ = '\\';
            *tmpdst++ = '_';
            break;
        default:
            *tmpdst++ = src[i];
            break;
        }
    }
    *tmpdst++ = '\0';
    return tmpdst - dst - 1;
}
#endif

char *url_quote(char *url)
{
    static char qurl[1024];
    int i = 0;

    while (i < 1020 && (*url))
    {
        if (isalnum(*url))
        {
            qurl[i++] = *url;
        }
        else
        {
            i += sprintf(qurl + i, "%%%02X", *url);
        }
        ++url;
    }
    qurl[i] = 0;
    return qurl;
}

int is_bad_id(char *userid)
{
    FILE *fp;
    char buf[STRLEN], ptr2[IDLEN + 2], *ptr;

    if ((fp = fopen(".badname", "r")) != NULL)
    {
        strtolower(ptr2, userid);
        while (fgets(buf, STRLEN, fp) != NULL)
        {
            ptr = strtok(buf, " \n\t\r");
            if (ptr != NULL && *ptr != '#')
            {
                if ((ptr[0] == '*'
                     && strstr(ptr2, &ptr[1]) != NULL)
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

long get_num_records(char *filename, int size)
{
    char *s, buf[64];
    int num;
    struct stat st;

    if (stat(filename, &st) == -1)
        return 0;
    num = st.st_size;
    strcpy(buf, filename);
    s = strrchr(buf, '/') + 1;
    strcpy(s, TOPFILE_DIR);
    if (stat(buf, &st) == -1)
        return num / size;
    else
        return (num + st.st_size) / size;
}

int cache_age = HEADER_NO_STORE;

time_t last_modified_time = 0;

void cgi_head(void)
{
    char buf[256];

    if (is_head_printed)
        return;
    if (last_modified_time == 0)
    {
        snprintf(buf, 256, "Last-Modified: %s\r\n", Gtime(time(0)));
        cgiHeaderString(buf);
    }
    else
    {
        snprintf(buf, 256, "Last-Modified: %s\r\n",
                 Gtime(last_modified_time));
        cgiHeaderString(buf);
    }
    switch (cache_age)
    {
    case HEADER_NO_CACHE:
        cgiHeaderString("Cache-control: no-cache\r\n");
        break;
    case HEADER_NO_STORE:
        cgiHeaderString("Cache-control: no-store\r\n");
        break;
    case HEADER_PRIVATE:
        cgiHeaderString("Cache-control: private\r\n");
        break;
    default:
        if (cache_age <= 0)
            break;
        snprintf(buf, 256, "Cache-control: max-age=%d\r\n", cache_age);
        cgiHeaderString(buf);
    }
    cgiHeaderContentType("text/html; charset=" CHARSET);
    is_head_printed = 1;
}

void http_quit(void)
{
    printf("\n</html>\n");
    longjmp(QUIT_JMP, 1);
    return;
}
void http_quit_print(char *str)
{
    printf
        ("<br>ERROR: %s<br><br>\n<a href=\"javascript:history.go(-1)\">¿ìËÙ·µ»Ø</a></html>\n",
         str);
    longjmp(QUIT_JMP, 1);
    return;
}
void http_fatal(char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    cgi_head();
    va_start(ap, fmt);
    vsnprintf(buf, 1023, fmt, ap);
    va_end(ap);
    buf[1023] = 0;
    printf("´íÎó! %s! <br><br>\n", buf);
    printf("<a href=\"javascript:history.go(-1)\">¿ìËÙ·µ»Ø</a>");
    printf("</html>\n");
    fflush(stdout);
    longjmp(fatalJmp, 1);
    return;
}

void setcookie(char *s, char *b)
{

    printf("<script>document.cookie='%s=%s'</script>\n", s, b);
}

void redirect(char *x)
{

    printf("<meta http-equiv='Refresh' content='0; url=%s'>\n", x);
}

void refreshto(char *x, int t)
{
    printf("<meta http-equiv='Refresh' content='%d; url=%s'>\n", t, x);
}

char *getfolderstr(char *filename)
{
    static char str[4] = { '\0', '\0', '\0', '\0' };
    int len = strlen(filename);

    str[0] =
        filename[len - 5] >
        '4' ? filename[len - 5] - 5 : filename[len - 5];
    str[1] = filename[len - 4];
    str[2] = filename[len - 3];
    return str;
}

int substitute_record(char *filename, void *rptr, int size, int id)
{
    int fd;

    struct stat st;

    if (stat(filename, &st) == -1 || st.st_size / size < id)
        return -1;

    if (id < 1)
    {
        g_message("substitue_record(...) id <= 0");
        return -1;
    }
    if (id > 120000)
    {
        g_message("substitue_record(...) id > 120000");
        return -1;
    }
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
        return -1;
    flock(fd, LOCK_EX);
    if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        g_message("subrec seek err");
    }
    else
    {
        if (safewrite(fd, rptr, size) != size)
            g_message("subrec write err");
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int cmpvfilename(char *filename, void *vfn)
{
    struct fileheader *fn = (struct fileheader *) vfn;

    return !strcmp(filename, fn->filename);
}

int
search_record(char *filename, char *rptr, int size, int (*fptr) (),
              char *farg)
{
    int fd;
    int id = 1;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return 0;
    while (read(fd, rptr, size) == size)
    {
        if ((*fptr) (farg, rptr))
        {
            close(fd);
            return id;
        }
        id++;
    }
    close(fd);
    return 0;
}

int change_title(char *fname, char *title)
{
    FILE *fp, *out;
    char buf[256], outname[STRLEN];
    int newtitle = 0;

    if (strlen(title) > 64)
        title[64] = 0;

    g_message("%s", fname);
    if ((fp = fopen(fname, "r")) == NULL)
        return 0;

    sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid,
            u_info->pid);
    g_message("%s", outname);
    if ((out = fopen(outname, "w")) == NULL)
        return 0;
    while ((fgets(buf, 256, fp)) != NULL)
    {
        g_message("equal: %d", !strncmp(buf, "±ê  Ìâ: ", 8));
        if (!strncmp(buf, "±ê  Ìâ: ", 8) && newtitle == 0)
        {
            fprintf(out, "±ê  Ìâ: %s\n", title);
            newtitle = 1;
            continue;
        }
        fputs(buf, out);
    }
    fclose(fp);
    fclose(out);
    rename(outname, fname);
    chmod(fname, 0644);
    return 1;
}

int board_in_which_dist(char *boardname)
{
    char BoardsFile[256] = { 0 };
    char category;
    struct boardheader *buffer = NULL;
    int Dindex, fd, index, total;
    struct stat st;
    char CateInSect[SectNum][5] =
        { "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw",
        "x"
    };

    sprintf(BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open(BoardsFile, O_RDONLY);
    if (fd == -1)
        return -1;
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer =
        (struct boardheader *) calloc((size_t) total,
                                      sizeof(struct boardheader));
    if (buffer == NULL)
    {
        close(fd);
        return -1;
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close(fd);
        return -1;
    }
    close(fd);

    for (index = 0; index < total; index++)
    {
        if (!strcmp(buffer[index].filename, boardname))
            break;
    }
    if (index == total)
        return -1;

    category = buffer[index].title[0];
    free(buffer);

    for (Dindex = 0; Dindex < SectNum; Dindex++)
    {
        if (strchr(CateInSect[Dindex], category) != NULL)
            break;
    }

    if (Dindex == SectNum)
        return -1;

    return Dindex;
}

int add_to_file(char *filename, char *str)
{
    FILE *fp;
    int rc;

    if ((fp = fopen(filename, "a")) == NULL)
        return -1;
    flock(fileno(fp), LOCK_EX);
    rc = fprintf(fp, "%s\n", str);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    return (rc == EOF ? -1 : 1);
}

int del_from_file(char *filename, char *str)
{
    FILE *fp, *nfp;
    int deleted = NA;
    char tmpbuf[1024], fnnew[STRLEN];

    if ((fp = fopen(filename, "r")) == NULL)
        return -1;
    sprintf(fnnew, "%s.%d", filename, getuid());
    if ((nfp = fopen(fnnew, "w")) == NULL)
        return -1;
    while (fgets(tmpbuf, 1024, fp) != NULL)
    {
        if (strncmp(tmpbuf, str, strlen(str)) == 0
            && (tmpbuf[strlen(str)] == '\0' || tmpbuf[strlen(str)] == ' '
                || tmpbuf[strlen(str)] == '\n'))
            deleted = YEA;
        else if (*tmpbuf > ' ')
            fputs(tmpbuf, nfp);
    }
    fclose(fp);
    fclose(nfp);
    if (!deleted)
        return -1;
    return (rename(fnnew, filename) + 1);
}

int exist_in_inboardfile(char *inboard_file)
{
    FILE *fp;
    char buf[STRLEN];
    char *namep;

    if ((fp = fopen(inboard_file, "r")) == NULL)
        return 0;

    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, "\n");
        if (namep != NULL && strcasecmp(namep, currentuser.userid) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int user_exit_board(char *boardname)
{
    char inboard_file[STRLEN];

    sprintf(inboard_file, "boards/%s/%s", boardname, INBOARD_FILE);
    if (exist_in_inboardfile(inboard_file))
        del_from_file(inboard_file, currentuser.userid);

    return 1;
}

void board_usage(char* mode, time_t usetime)
{
    time_t now;
    char buf[256];

    now = time(0);
    getdatestring(now, NA);
    sprintf(buf, "%.22s use %-20.20s stay: %5ld (%s)\n", datestring, mode, usetime, currentuser.userid);
    f_append("use_board", buf);
}

int user_enter_board(char *boardname)
{
    char path[STRLEN], inboard_file[STRLEN], previous_boardname[STRLEN];
    FILE *fp;

    board_usage(boardname, 10);
    sethomefile(path, currentuser.userid, "inboard.www");
    if ((fp = fopen(path, "r")) != NULL) {
        fgets(previous_boardname, STRLEN, fp);
        fclose(fp);

        if (!strcmp(previous_boardname, boardname))
            return 0;

        user_exit_board(previous_boardname);
    }
    if ((fp = fopen(path, "w")) == NULL)
        return 0;
    fprintf(fp, "%s", boardname);
    fclose(fp);

    sprintf(inboard_file, "boards/%s/%s", boardname, INBOARD_FILE);
    if (!exist_in_inboardfile(inboard_file))
        add_to_file(inboard_file, currentuser.userid);

    return 1;
}

int user_exit_previous_board()
{
    char path[STRLEN], boardname[STRLEN];
    FILE *fp;

    sethomefile(path, currentuser.userid, "inboard.www");
    if ((fp = fopen(path, "r")) == NULL)
        return 0;
    fgets(boardname, STRLEN, fp);
    fclose(fp);
    unlink(path);

    user_exit_board(boardname);

    return 1;
}

int canmsg(struct user_info *uin)
{
    if (isbad(currentuser.userid, uin->userid)
        || isbad(uin->userid, currentuser.userid))
        return NA;
    if ((uin->pager & ALLMSG_PAGER) || (currentuser.userlevel & PERM_SYSOP)
        || (currentuser.userlevel & PERM_FORCEPAGE))
        return YEA;
    if ((uin->pager & FRIENDMSG_PAGER) && isfriend(currentuser.userid))
        return YEA;
    return NA;
}

int canpage(int friend, int pager)
{
    if ((pager & ALL_PAGER) || (currentuser.userlevel & PERM_SYSOP)
        || (currentuser.userlevel & PERM_FORCEPAGE))
        return YEA;
    if ((pager & FRIEND_PAGER))
    {
        if (friend)
            return YEA;
    }
    return NA;
}

char *strnzhcpy(char *dest, const char *src, int n)
{
    register int c = 0;
    register char *dst = dest;

    if (n == 0)
        return dest;
    n--;
    while (n > 0 && *src != '\0')
    {
        c = (((*src) & 0x80) & (c ^ 0x80));
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *(dest - (c >> 7)) = '\0';
    return dst;
}

int search_in_anonyindex(char* userid, char* ranstr, struct anony_item *item_p)
{
#ifndef ANONYMOUS_MODULE
    return -1;
#endif

    int fd;
    int aisize = sizeof(struct anony_item);
    int pos;

    if ((fd = open(ANONYFILE, O_RDONLY, 0)) != -1)
    {
        pos = 0;
        if (userid != NULL)
        {
            while (read(fd, item_p, aisize) == aisize)
            {
                if (strcmp(item_p->userid, userid) == 0)
                {
                    close(fd);
                    return pos;
                }
                pos ++;
            }
        }
        else if (ranstr != NULL)
        {
            while (read(fd, item_p, aisize) == aisize)
            {
                if (strcmp(item_p->ranstr, ranstr) == 0)
                {
                    close(fd);
                    return pos;
                }
                pos ++;
            }
        }
        close(fd);
    }
    return -1;
}

void set_random_string(char* ranstr)
{
    struct anony_item item2;
    int i;

    while (1)
    {
        for (i=0; i<ANONY_RANSTR_SIZE; i++)
            ranstr[i] = rand()%26 + 'A';
        ranstr[ANONY_RANSTR_SIZE] = '\0';
        if (search_in_anonyindex(NULL, ranstr, &item2) == -1)
            break;
    }
}

char* get_anonyid(char* userid, char* boardname)
{
#ifndef ANONYMOUS_MODULE
    return boardname;
#endif

	static char anony_id[IDLEN + 2];

    int pos, fd, fdd;
    struct anony_item item;
    char buf[STRLEN];
    time_t now;

    pos = search_in_anonyindex(userid, NULL, &item);

    if (pos == -1)
    {
        set_random_string(item.ranstr);
        strcpy(item.userid, userid);
        item.lastpost_time = time(0);
               
        if ((fd = open(ANONYFILE, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
        {
            flock(fd, LOCK_EX);
            write(fd, &item, sizeof(struct anony_item));
            flock(fd, LOCK_UN);
            close(fd);
        }
        
        strncpy(anony_id, "Anony.", 6);
        strncpy(anony_id + 6, item.ranstr, 7);
    }
    else
    {
        strncpy(anony_id, "Anony.", 6);
        strncpy(anony_id + 6, item.ranstr, 7);
        sprintf(buf, "boards/%s/deny_users", boardname);
        if (!seek_in_denyfile(buf, anony_id))
        {
            now = time(0);
            if (now - item.lastpost_time > 86400)
            {
                set_random_string(item.ranstr);
            }
            item.lastpost_time = now;

            if ((fdd = open(ANONYFILE, O_WRONLY)) == -1)
                return boardname;
            lseek(fdd, pos * sizeof(struct anony_item), SEEK_SET);
            flock(fdd, LOCK_EX);
            write(fdd, &item, sizeof(struct anony_item));
            flock(fdd, LOCK_UN);
            close(fdd);

            strncpy(anony_id, "Anony.", 6);
            strncpy(anony_id + 6, item.ranstr, 7);
        }
    }
    
    return anony_id;
}

char *setmdir(char *buf, char *userid)
{
    sprintf(buf, "mail/%c/%c/%s/.DIR", toupper(userid[0]),
            toupper(userid[1]), userid);
    return buf;
}

// reply log handler
#ifdef SERVICE_HANDLER
int send_msg_to_service(const char* msg, buf_callback callback) {
    int sockfd;
    int len;
    struct sockaddr_in serveraddr;
    struct timeval timeout;
    char buf[4096];
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(struct timeval)) < 0)
        return 2;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval)) < 0)
        return 3;

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(REPLY_SERVICE_PORT);
    inet_pton(AF_INET, REPLY_SERVICE_HOST, &serveraddr.sin_addr);

    if (connect(sockfd , (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        return 4;
    }

    if (send(sockfd, msg, strlen(msg), 0) < 0) {
        return 5;
    }
    while ((len = recv(sockfd, buf, 4096, 0)) > 0) {
        if (callback)
            callback(buf, len);
    }

        close(sockfd);
        return 0;
}
#endif
#ifdef REPLY_SERVICE_HANDLER
int b64_ntop(const char*src, size_t srclength, char *target, size_t targsize);
int reply_service_handle(const char* board, const char* title, const char* from_user, const char* to_user, int post) {
    char buf[4096];
    char b64_title[1024];
    if (b64_ntop(title, strlen(title), b64_title, 1024) == -1)
        sprintf(b64_title, "error");
    sprintf(buf,
            "{ \"type\" : \"reply\","
            "  \"board\" : \"%s\"  ,"
            "  \"title\" : \"%s\"  ,"
            "  \"from_user\" : \"%s\"   ,"
            "  \"to_user\" : \"%s\"     ,"
            "  \"post\" : %d       "
            " }"
            , board, b64_title, from_user, to_user, post);

    if (send_msg_to_service(buf, NULL) == 0)
        return 1;
    else
        return 0;
}
void print_reply(char* buf, int len) {
    buf[len] = 0;
    printf("%s",buf);
}
int get_reply_handle(const char* user) {
    char buf[4096];
    sprintf(buf,
            "{ \"type\" : \"get_reply\","
            "  \"user\" : \"%s\"   }"
            , user);

    if (send_msg_to_service(buf, print_reply) == 0)
        return 1;
    else
        return 0;
}
#endif
#ifdef WWW_LOGIN_SERVICE_HANDLER
int www_login_service_handle(const char* user, const int num, const int key) {
    char buf[1024];
    sprintf(buf,
            "{ \"type\" : \"login\","
            "  \"user\" : \"%s\"   ,"
            "  \"num\" :  %d       ,"
            "  \"key\" :  %d       }"
            , user, num, key);

	if (send_msg_to_service(buf, NULL) == 0)
        return 1;
    else
        return 0;
}
int www_logout_service_handle(const char* user) {
    char buf[1024];
    sprintf(buf,
            "{ \"type\" : \"logout\","
            "  \"user\" : \"%s\"   },"
            , user);

	if (send_msg_to_service(buf, NULL) == 0)
        return 1;
    else
        return 0;
}
#endif
