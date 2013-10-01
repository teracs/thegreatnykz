#include "www.h"

char seccode[SECNUM][5] = {
                              "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw", "x"
                          };

//add by rwzmm@sjtubbs

#define MAX_MSG_SIZE 1024

//struct msgdir {
//    char userid[IDLEN + 2];
//    time_t tt;
//    pid_t pid;
//    char msg[MAX_MSG_SIZE];
//};


//add end

char secname[SECNUM][2][20] = {
                                  "BBS ÏµÍ³", "[±¾Õ¾][Õ¾ÄÚ][BBS]",
                                  "ÉÏº£½»´ó", "[±¾Ð£]",
                                  "Ñ§×ÓÔºÐ£", "[¸ßÐ£][µØÇø]",
                                  "µçÄÔ¼¼Êõ", "[µçÄÔ]",
                                  "Ñ§Êõ¿ÆÑ§", "[¿ÆÑ§]",
                                  "ÒÕÊõÎÄ»¯", "[ÎÄÒÕ][ÓïÑÔ]",
                                  "ÌåÓýÔË¶¯", "[ÌåÓý][ÔË¶¯]",
                                  "ÐÝÏÐÓéÀÖ", "[ÐÝÏÐ][ÒôÀÖ][ÓéÀÖ]",
                                  "ÖªÐÔ¸ÐÐÔ", "[ÖªÐÔ] [¸ÐÐÔ]",
                                  "Éç»áÐÅÏ¢", "[Éç»á][ÐÅÏ¢][¿¼ÊÔ]",
                                  "ÉçÍÅÈºÌå", "[ÉçÍÅ][ÈºÌå]",
                                  "ÓÎÏ·×¨Çø", "[ÓÎÏ·]"
                              };

int loginok = 0;
int pic_mod;

struct userec currentuser;
struct user_info *u_info;
struct UTMPFILE *shm_utmp;
struct BCACHE *shm_bcache;
struct UCACHE *shm_ucache;
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
        return;
    fprintf(fp, "%s", buf);
    fclose(fp);
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
    char *buf = calloc(strlen(s) + 1, 1);
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

char *strcasestr(char *s1, char *s2)
{
    int l;

    l = strlen(s2);
    while (s1[0])
    {
        if (!strncasecmp(s1, s2, l))
            return s1;
        s1++;
    }
    return 0;
}

int strsncpy(char *s1, char *s2, int n)
{
    int l = strlen(s2);

    if (n < 0)
        return;
    if (n > l + 1)
        n = l + 1;
    strncpy(s1, s2, n - 1);
    s1[n - 1] = 0;
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

char *get_new_shm(int key, int size)
{
    int id;

    id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0640);
    if (id < 0)
        return 0;
    return shmat(id, NULL, 0);
}

char *get_old_shm(int key, int size)
{
    int id;

    id = shmget(key, size, 0);
    if (id < 0)
        return 0;
    return shmat(id, NULL, 0);
}

char *get_shm(int key, int size)
{
    int id;

    id = shmget(key, size, IPC_CREAT | 0640);
    if (id < 0)
        return 0;
    return shmat(id, NULL, 0);
}

int setcookie(char *s, char *b)
{
    printf("<script>document.cookie='%s=%s'</script>\n", s, b);
}

int redirect(char *x)
{
    printf("<meta http-equiv='Refresh' content='0; url=%s'>\n", x);
}

int refreshto(char *x, int t)
{
    printf("<meta http-equiv='Refresh' content='%d; url=%s'>\n", t, x);
}

int cgi_head()
{
    if (!has_http_head)
        return;
    printf("Connection: close\n");
    printf("Content-type: text/html; charset=%s\n\n", CHARSET);
}

char *getsenv(char *s)
{
    char *t = getenv(s);

    if (t)
        return t;
    return "";
}

int http_quit()
{
    printf("\n</html>\n");
    exit(0);
}

int http_fatal(char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, 1023, fmt, ap);
    va_end(ap);
    buf[1023] = 0;
    printf("´íÎó! %s! <br><br>\n", buf);
    printf("<a href=\"javascript:history.go(-1)\">¿ìËÙ·µ»Ø</a>");
    http_quit();
}

int strnncpy(char *s, int *l, char *s2)
{
    strncpy(s + (*l), s2, strlen(s2));
    (*l) += strlen(s2);
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
            strnncpy(s, &len, "&amp;");
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
                strnncpy(s, &len, "<font class=c37>");
            }
            tmp = strtok(ansibuf, ";");
            while (tmp)
            {
                c = atoi(tmp);
                tmp = strtok(0, ";");
                if (c == 0)
                {
                    strnncpy(s, &len, "<font class=c37>");
                    bold = 0;
                }
                if (c >= 30 && c <= 37)
                {
                    if (bold == 1)
                        sprintf(buf2, "<font class=d%d>", c);
                    if (bold == 0)
                        sprintf(buf2, "<font class=c%d>", c);
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
}

//add by cf
int hfprintf(FILE *stream,char *fmt, ...)
{
    char buf[8096], buf2[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf2, 1023, fmt, ap);
    va_end(ap);
    hsprintf(buf, "%s", buf2);
    fprintf(stream,"%s", buf);
}

int hhprintf(char *fmt, ...)
{
    char buf0[1024], buf[1024], *s, *getparm();
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
    if (!strcasestr(s, "http://") && !strcasestr(s, "ftp://")
            && !strcasestr(s, "mailto:"))
        return hprintf("%s", buf);
    while (s[0])
    {
        if (!strncasecmp(s, "http://", 7) || !strncasecmp(s, "mailto:", 7)
                || !strncasecmp(s, "ftp://", 6))
        {
            char *tmp;

            if (len > 0)
            {
                buf0[len] = 0;
                hprintf("%s", buf0);
                len = 0;
            }
            tmp = strtok(s, "\'\" \r\t)(,;\n");
            if (tmp == 0)
                break;
            if ((strcasestr(tmp, ".gif") || strcasestr(tmp, ".jpg")
                    || strcasestr(tmp, ".bmp")) && pic_mod)
            {
                if (pic_mod == 1)
                    printf("<IMG SRC=\"%s\" onload=\"if (this.height>%d) this.height=%d\" ALT=\"%s\">",
                           nohtml(tmp), IMG_HEIGHT, IMG_HEIGHT,nohtml(tmp));
                if (pic_mod == 2)
                    printf("<IMG SRC=\"%s\" ALT=\"%s\">", nohtml(tmp),
                           nohtml(tmp));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return;
                return hhprintf(tmp);
            }

            if (strcasestr(tmp, ".swf") && pic_mod)
            {
                if (pic_mod == 1)
                    printf
                    ("<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=%d></EMBED>",
                     nohtml(tmp), IMG_HEIGHT);
                if (pic_mod == 2)
                    printf
                    ("<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400 ><param name=\"play\" value=\"false\"></param></EMBED>",
                     nohtml(tmp));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return;
                return hhprintf("%s",tmp);
            }
            printf("<a target=_blank href='%s'>%s</a>", nohtml(tmp),
                   nohtml(tmp));
            tmp = strtok(0, "");
            if (tmp == 0)
                return printf("\n");
            return hhprintf("%s",tmp);
        }
        else
        {
            buf0[len] = s[0];
            if (len < 1000)
                len++;
            s++;
        }
    }
}

int hhfprintf(FILE *stream,char *fmt, ...)
{
    char buf0[1024], buf[1024], *s, *getparm();
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
        return hfprintf(stream,"%s", buf);
    if (!strcasestr(s, "http://") && !strcasestr(s, "ftp://")
            && !strcasestr(s, "mailto:"))
        return hfprintf(stream,"%s", buf);
    while (s[0])
    {
        if (!strncasecmp(s, "http://", 7) || !strncasecmp(s, "mailto:", 7)
                || !strncasecmp(s, "ftp://", 6))
        {
            char *tmp;

            if (len > 0)
            {
                buf0[len] = 0;
                hfprintf(stream,"%s", buf0);
                len = 0;
            }
            tmp = strtok(s, "\'\" \r\t)(,;\n");
            if (tmp == 0)
                break;
            if ((strcasestr(tmp, ".gif") || strcasestr(tmp, ".jpg")
                    || strcasestr(tmp, ".bmp")) && pic_mod)
            {
                if (pic_mod == 1)
                    fprintf(stream,"<IMG SRC=\"%s\" onload=\"if (this.height>%d) this.height=%d\" ALT=\"%s\">",
                           nohtml(tmp), IMG_HEIGHT, IMG_HEIGHT,nohtml(tmp));
                if (pic_mod == 2)
                    fprintf(stream,"<IMG SRC=\"%s\" ALT=\"%s\">", nohtml(tmp),
                           nohtml(tmp));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return;
                return hhfprintf(stream,tmp);
            }
            if (strcasestr(tmp, ".swf") && pic_mod)
            {
                if (pic_mod == 1)
                    fprintf
                    (stream,"<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=%d></EMBED>",
                     nohtml(tmp), IMG_HEIGHT);
                if (pic_mod == 2)
                    fprintf
                    (stream,"<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400><param name=\"play\" value=\"false\"></param></EMBED>",
                     nohtml(tmp));
                tmp = strtok(0, "");
                if (tmp == 0)
                    return;
                return hhfprintf(stream,"%s",tmp);
            }
            fprintf(stream,"<a target=_blank href='%s'>%s</a>", nohtml(tmp),
                   nohtml(tmp));
            tmp = strtok(0, "");
            if (tmp == 0)
                return fprintf(stream,"%s","\n");
            return hhfprintf(stream,"%s",tmp);
        }
        else
        {
            buf0[len] = s[0];
            if (len < 1000)
                len++;
            s++;
        }
    }
}

char parm_name[256][80], *parm_val[256];
int parm_num = 0;

int parm_add(char *name, char *val)
{
    int len = strlen(val);

    if (parm_num >= 255)
        http_fatal("too many parms.");
    parm_val[parm_num] = calloc(len + 1, 1);
    if (parm_val[parm_num] == 0)
        http_fatal("memory overflow2 %d %d", len, parm_num);
    strsncpy(parm_name[parm_num], name, 78);
    strsncpy(parm_val[parm_num], val, len + 1);
    parm_num++;
}

int http_init()
{
    char *buf, buf2[1024], *t2, *t3;
    int n;

    if (!silence)
    {
        cgi_head();
        printf("<html>\n");
        printf
        ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\n");
        printf("<link rel=stylesheet type=text/css href='%s'>\n",
               CSS_FILE);
    }
    n = atoi(getsenv("CONTENT_LENGTH"));
    if (n > 5000000)
        n = 5000000;
    buf = calloc(n + 1, 1);
    if (buf == 0)
        http_fatal("memory overflow");
    fread(buf, 1, n, stdin);
    buf[n] = 0;
    t2 = strtok(buf, "&");
    while (t2)
    {
        t3 = strchr(t2, '=');
        if (t3 != 0)
        {
            t3[0] = 0;
            t3++;
            __unhcode(t3);
            parm_add(trim(t2), t3);
        }
        t2 = strtok(0, "&");
    }
    strsncpy(buf2, getsenv("QUERY_STRING"), 1024);
    t2 = strtok(buf2, "&");
    while (t2)
    {
        t3 = strchr(t2, '=');
        if (t3 != 0)
        {
            t3[0] = 0;
            t3++;
            __unhcode(t3);
            parm_add(trim(t2), t3);
        }
        t2 = strtok(0, "&");
    }
    strsncpy(buf2, getsenv("HTTP_COOKIE"), 1024);
    t2 = strtok(buf2, ";");
    while (t2)
    {
        t3 = strchr(t2, '=');
        if (t3 != 0)
        {
            t3[0] = 0;
            t3++;
            parm_add(trim(t2), t3);
        }
        t2 = strtok(0, ";");
    }
    strsncpy(fromhost, getsenv("REMOTE_ADDR"), 32);
    if (!strcmp(fromhost, "202.119.32.8") ||
            !strcmp(fromhost, "202.119.32.32") ||
            !strcmp(fromhost, "202.119.32.49"))
    {
        char s[80];

        strsncpy(s, getsenv("HTTP_X_FORWARDED_FOR"), 20);
        if (s[0])
            sprintf(fromhost, "%s@", s);
    }
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
}

char *getparm(char *var)
{
    int n;

    for (n = 0; n < parm_num; n++)
        if (!strcasecmp(parm_name[n], var))
            return parm_val[n];
    return "";
}

int get_shmkey(char *s)
{
    int n = 0;

    while (shmkeys[n].key != 0)
    {
        if (!strcasecmp(shmkeys[n].key, s))
            return shmkeys[n].value;
        n++;
    }
    return 0;
}

int shm_init()
{
    shm_utmp =
        (struct UTMPFILE *) get_old_shm(UTMP_SHMKEY,
                                        sizeof(struct UTMPFILE));
    shm_bcache =
        (struct BCACHE *) get_old_shm(BCACHE_SHMKEY,
                                      sizeof(struct BCACHE));
    shm_ucache =
        (struct UCACHE *) get_old_shm(UCACHE_SHMKEY,
                                      sizeof(struct UCACHE));
    if (!shm_utmp)
        http_fatal("shm_utmp error");
    if (!shm_bcache)
        http_fatal("shm_bcache error");
    if (!shm_ucache)
        http_fatal("shm_ucache error");
}

int user_init(struct userec *x, struct user_info **y)
{
    struct userec *x2;
    char id[20], num[20];
    int i, key;

    strsncpy(id, getparm("utmpuserid"), 13);
    strsncpy(num, getparm("utmpnum"), 12);
    key = atoi(getparm("utmpkey"));
    i = atoi(num);
    if (i < 0 || i >= MAXACTIVE)
        return 0;
    (*y) = &(shm_utmp->uinfo[i]);
    if (strncmp((*y)->from, fromhost, 24))
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

int post_mail(char *userid, char *title, char *file, char *id,
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
        sprintf(buf3, "mail/%c/%s/M.%d.A", toupper(userid[0]), userid,
                i + time(0));
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
        while (1)
        {
            if (fgets(buf3, 256, fp2) <= 0)
                break;
            fprintf2(fp, buf3);
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--\n");
    sig_append(fp, id, sig);
    fprintf(fp, "\n\n[1;%dm¡ù À´Ô´:¡¤%s http://%s¡¤[FROM: %.20s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(dir, "mail/%c/%s/.DIR", toupper(userid[0]), userid);
    fp = fopen(dir, "a");
    if (fp == 0)
        return -1;
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
}

int post_imail(char *userid, char *title, char *file, char *id,
               char *nickname, char *ip, int sig)
{
    //All the variables have no use, so I deleted them.  --hongliang
    //Nov. 20th, 2003 2:25
    /*
       FILE *fp1, *fp2;
       char buf[256];
       int i;
    */

    return 0;
}

int post_article(char *board, char *title, char *file, char *id,
                 char *nickname, char *ip, int sig, int autocr, 
				 unsigned int currid, unsigned int reid)
{
    FILE *fp, *fp2;
    char buf3[1024];
    struct shortfile *brd;
    struct fileheader header;
    int t, i;

    brd = getbcache(board);
    if (brd == 0)
        return -1;
    board = brd->filename;
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
	
	header.id = currid;
	header.reid = reid;

    for (i = 0; i < 100; i++)
    {
        t = time(0) + i;
        sprintf(buf3, "boards/%s/M.%d.A", board, t);
        if (!file_exist(buf3))
            break;
    }
    if (i >= 99)
        return -1;
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
            if (fgets(buf3, 1000, fp2) <= 0)
                break;
            if (autocr)
            {
                fprintf2(fp, buf3);
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
    fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s¡¤[FROM: %.20s][m\n",
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
        return;
    x = getuser(id);
    if (x == 0)
        return;
    sprintf(path, "home/%c/%s/signatures", toupper(id[0]), id);
    fp2 = fopen(path, "r");
    if (fp2 == 0)
        return;
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
}

char *anno_path_of(char *board)
{
    FILE *fp;
    static char buf[256], buf1[80], buf2[80];

    fp = fopen("0Announce/.Search", "r");
    if (fp == 0)
        return "";
    while (1)
    {
        if (fscanf(fp, "%s %s", buf1, buf2) <= 0)
            break;
        buf1[strlen(buf1) - 1] = 0;
        if (!strcasecmp(buf1, board))
        {
            sprintf(buf, "/%s", buf2);
            return buf;
        }
    }
    fclose(fp);
    return "";
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
    //added by hongliang on Nov. 23th, 2003 for District Master
    if ( user_perm(user, PERM_SPECIAL7) && has_DM_perm(user, board) )
        return 1;

    if (!user_perm(user, PERM_BOARDS))
        return 0;
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

//The following code is add by hongliang on Nov. 23th, 2003 07:25
//For District Master
//start-hongliang
/* Added by old@sjtu.edu to access master of district 2001.2.1 */
/*added by aegis@010921 to modify chk_currDM()*/
//mudboy modify to WHICHDIST(char *userid) from WHICHDIST()
int WHICHDIST(char *userid)
{
    char datafile[80]= BBSHOME"/etc/DISTmaster.ini";
    FILE *fp;
    int counts=1;
    char t[256],*t1, *t2, *t3;
    fp= fopen(datafile, "r");
    if(fp== NULL)
        return 10;
    while(fgets(t, 255, fp)&& counts <= 9)
    {
        t1= strtok(t, " \t\n");
        if(t1== NULL||t1[0]== '#')
            continue;
        if (!isalpha(t1[0]))
        {
            counts++;
            continue;
        }
        t2= strtok(NULL, " \t\n");
        if (t2!=NULL)
            t3= strtok(NULL, " \t\n");
        else
            t3=NULL;
        if ((t1!=NULL&&!strcasecmp(t1,userid))||(t2!=NULL&&!strcasecmp(t2,userid))||(t3!=NULL&&!strcasecmp(t3,userid)))
        {
            fclose(fp);
            return(counts);
        }
        counts++;
    }
    fclose(fp);
    return(counts);
}

int has_DM_perm(struct userec *user, char *board)
{
    char BoardsFile[256], category;
    int fd, total, index, Dindex, DMLevel;
    struct boardheader *buffer;
    struct stat st;

    char CateInSect[SectNum][5] =
        {"a", "bc", "d", "efg", "hij", "kl", "o", "p","qrs","tu","vw", "x"};
    /* {"a", "bcd", "efg", "hij", "kl", "mn", "op", "qr","s"};*/
    /****************************************************************************/
    /*  ²ÉÓÃÀàËÆbbs2wwwµÄ°ì·¨£¬Èç¹ûÐÞ¸ÄÁËÌÖÂÛÇøµÄ·ÖÀà£¬ÇëÊÓÇé¿öÐÞ¸ÄÒÔÏÂ¶¨Òå     */
    /*                                                                          */
    /*  ·ÖÀàÂëºÍ·ÖÀà²Ëµ¥ÔÚmenu.iniÖÐÉè¶¨,Firebird BBS 2.52ÖÐÈ±Ê¡µÄ·ÖÀàÓÐ:       */
    /*  0¡¾Õ¾ÄÚ¡¿¡¢C¡¾±¾Ð£¡¿¡¢A¡¾Çó¾È¡¿¡¢E¡¾×ÊÑ¶¡¿¡¢I¡¾Ð£Ô°¡¿¡¢N¡¾µçÄÔ¡¿¡¢R¡¾   */
    /*  ÏµÍ³¡¿¡¢a¡¾ÐÝÏÐ¡¿¡¢e¡¾ÎÄÒÕ¡¿¡¢i¡¾Ñ§¿Æ¡¿¡¢m¡¾ÒôÀÖ¡¿¡¢n¡¾Éç½»¡¿¡¢r¡¾ÔË¶¯  */
    /*  ¡¿¡¢s¡¾Ö°°ô¡¿¡¢x¡¾Ì¸Ìì¡¿¡¢f¡¾ÐÂÎÅ¡¿µÈÀà                                 */
    /****************************************************************************/

    if (!user_perm(user, PERM_SPECIAL7))
        return 0;

    sprintf (BoardsFile, "%s/.BOARDS", BBSHOME);
    fd = open (BoardsFile, O_RDONLY);
    if (fd == -1)
        return 0;
    (void) fstat (fd, &st);
    total = st.st_size / sizeof (struct boardheader);
    buffer = (struct boardheader *) calloc ((size_t) total, sizeof (struct boardheader));
    if (buffer == NULL)
    {
        close (fd);
        return 0;
    }
    if (read (fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        close (fd);
        return 0;
    }
    close (fd);

    for (index = 0; index < total; index++)
    {
        if(!strcmp(buffer[index].filename,board))
            break;
    }
    if(index == total)
        return 0;

    category = buffer[index].title[0];

    for ( Dindex = 0; Dindex < 10; Dindex++ )
    {
        if (strchr (CateInSect[Dindex], category) != NULL)
            break;
    }

    if ( Dindex == 10 )
        return 0;

    DMLevel = WHICHDIST(currentuser.userid);//mudboy modify
    if ( Dindex == DMLevel )
        return 1;
    else
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

int has_read_perm(struct userec *user, char *board)
{
    struct shortfile *x;        /* °åÃæ²»´æÔÚ·µ»Ø0, pºÍz°åÃæ·µ»Ø1, ÓÐÈ¨ÏÞ°åÃæ·µ»Ø1. */
	char pathbuf[80];
	FILE* fp;

	fp = NULL;

    if (board[0] <= 32)
        return 0;
    x = getbcache(board);
    if (x == 0)
        return 0;
	
    
	if (x->level == 0) 
	{
		sprintf(pathbuf, "boards/%s/board.allow", board);
		fp = fopen(pathbuf, "r");
		if(fp == NULL)
			return 1;
		else 
		{
			fclose(fp);
			if(file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}


	
	if(x->level & (PERM_POSTMASK | PERM_NOZAP)) 
	{
		sprintf(pathbuf, "boards/%s/board.allow", board);
		fp = fopen(pathbuf, "r");
		if(fp == NULL)
			return 1;
		else 
		{
			fclose(fp);
			if(file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}

	if (!user_perm(user, PERM_BASIC))
        return 0;

	if(user_perm(user, x->level))
	{
		sprintf(pathbuf, "boards/%s/board.allow", board);
		fp = fopen(pathbuf, "r");
		if(fp == NULL)
			return 1;
		else 
		{
			fclose(fp);
			if(file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}

    return 0;
}

int has_post_perm(struct userec *user, char *board)
{
    char buf3[256];
    struct shortfile *x;

    x = getbcache(board);
    strcpy(board, x->filename);

    if (x == 0)
        return 0;
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
        //added by hongliang on Nov. 23th, 2003 for auto undeny
        if ( seek_in_denyfile(board, user->userid) )
        {
            //still denying
            return 0;
        }
        else
        {
            //auto undeny
            return 1;
        }
    }
    if (!HAS_PERM(PERM_BASIC))
        return 0;
    if (!strcasecmp(board, "sysop")/* && user_perm(user, PERM_LOGINOK)*/)
        return 1;
    if (!strcasecmp(board, "BBSHelp") /*&& !user_perm(user, PERM_LOGINOK)*/)
        return 1;
    //next two lines deleted,bu acdsee
    //if (!strcasecmp(board, "Appeal") && user_perm(user, PERM_LOGINOK))
    //    return 1;
    if (user_perm(user, PERM_SYSOP))
        return 1;
    if (!user_perm(user, PERM_BASIC))
        return 0;
    if (!user_perm(user, PERM_POST))
        return 0;
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
        //        if(shm_utmp->uinfo[j].board==i) s++;
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
    sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(id[0]), id);
    fp = fopen(buf, "r");
    if (fp == 0)
        return;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
    {
        (*total)++;
        if (!(x1.accessed[0] & FILE_READ))
            (*unread)++;
    }
    fclose(fp);
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
    sprintf(buf, "home/%c/%s/%s", toupper(id[0]), id, file);
}
int add_web_msg(char* id)
{
    int i;
    FILE *fp;
    char buf[1048], file[256];

    memset(buf, 0x20, 1048);
    buf[1047] = 0x00;

    sprintf(file, "home/%c/%s/.msgdir", toupper(id[0]), id);
    i = file_size(file) / sizeof(struct msgdir);
    if (get_record(buf, sizeof(struct msgdir), i - 1, file) <= 0)
        return;
    for(i = 0; i<1048; i ++)
    {
        if(buf[i] == 0x00)
            buf[i] = 0x20;
    }
    sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
    fp = fopen(file, "a");
    fwrite(buf, sizeof(struct msgdir), 1, fp);
    fclose(fp);
	return 0;

}

int send_msg(char *myuserid, int mypid, char *touserid, int topid,
             char msg[256],int isWeb)
{
    char msgbuf[256], msgbuf2[256], buf3[256];
    int i;

    for (i = 0; i < strlen(msg); i++)
        if ((0 < msg[i] && msg[i] <= 27) || msg[i] == -1)
            msg[i] = 32;
    if (mypid <= 0)
        return -1;
    sprintf(msgbuf,
            "[0;1;44;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z»Ø)[m[%05dm\n",
            myuserid, Ctime(time(0)) + 11, msg, mypid);
    sprintf(msgbuf2, "[1;32;40m·¢ËÍ£º[1;33;40m%s[m (%5.5s):%s\n",
            touserid, Ctime(time(0)) + 11, msg);

    sethomefile(buf3, touserid, "msgfile");
    f_append(buf3, msgbuf);

    sethomefile(buf3, touserid, "msgfile.me");
    f_append(buf3, msgbuf);

    //add by rwzmm@sjtubbs
    //write msg to targetuser's .msgdir
    sethomefile(buf3, touserid, ".msgdir");
    // 	tracerecord("msgdir is open!\n");
    f_append_msg(buf3, myuserid, msgbuf);	//rwzmm add this function
    // tracerecord("msgdir is colsed!\n");

    sethomefile(buf3, myuserid, "msgfile.me");
    f_append(buf3, msgbuf2);
	if(isWeb)/* ·¢ËÍ¸øwebÓÃ»§ */
	{
		add_web_msg(touserid);
		return 0;
	}
	/*·¢ËÍ¸øtelnet */
    if (topid <= 0)
        return -1;
//    kill(topid, SIGTTOU); //this signal isn't used, why here?
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
        return "Ö÷Ñ¡µ¥";
    case ADMIN:
        return "¹ÜÀíÕßÑ¡µ¥";
    case SELECT:
        return "Ñ¡ÔñÌÖÂÛÇø";
    case READBRD:
        return "ÀÀ±éÌìÏÂ";
    case READNEW:
        return "ÀÀÐÂÎÄÕÂ";
    case READING:
        return "Æ·Î¶ÎÄÕÂ";
    case POSTING:
        return "·¢±íÎÄÕÂ";
    case MAIL:
        return "´¦ÀíÐÅ¼ã";
    case SMAIL:
        return "¼ÄÓïÐÅ¸ë";
    case RMAIL:
        return "ÔÄÀÀÐÅ¼ã";
    case TMENU:
        return "ÁÄÌìÑ¡µ¥";
    case LUSERS:
        return "»·¹ËËÄ·½";
    case FRIEND:
        return "Ñ°ÕÒºÃÓÑ";
    case MONITOR:
        return "Ì½ÊÓÃñÇé";
    case QUERY:
        return "²éÑ¯ÍøÓÑ";
    case TALK:
        return "ÁÄÌì";
    case PAGE:
        return "ºô½Ð";
    case CHAT1:
        return "Chat1";
    case CHAT2:
        return "Chat2";
    case CHAT3:
        return "Chat3";
    case CHAT4:
        return "Chat4";
    case LAUSERS:
        return "Ì½ÊÓÍøÓÑ";
    case XMENU:
        return "ÏµÍ³×ÊÑ¶";
    case VOTING:
        return "Í¶Æ±";
    case EDITWELC:
        return "±à¼­Welc";
    case EDITUFILE:
        return "±à¼­¸öÈËµµ";
    case EDITSFILE:
        return "±àÐÞÏµÍ³µµ";
    case ZAP:
        return "¶©ÔÄÌÖÂÛÇø";
    case LOCKSCREEN:
        return "ÆÁÄ»Ëø¶¨";
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
        //case WFRIEND:   return "Ñ°ÈËÃû²á";
    case WNOTEPAD:
        return "Óû×ß»¹Áô";
    case BBSNET:
        return "BBSNET..";
    case 10001:
        return "Webä¯ÀÀ";
    default:
        return "È¥ÁËÄÄ¶ù??";
    }
}

/*
char   *cexp(int exp) {
        if(exp==-9999) 	return "Ã»µÈ¼¶";
        if(exp<=100)  	return "ÐÂÊÖÉÏÂ·";
        if(exp<=450)	return "Ò»°ãÕ¾ÓÑ";
        if(exp<=850)	return "ÖÐ¼¶Õ¾ÓÑ";
        if(exp<=1500)	return "¸ß¼¶Õ¾ÓÑ";
        if(exp<=2500)	return "ÀÏÕ¾ÓÑ";
        if(exp<=3000)	return "³¤ÀÏ¼¶";
        if(exp<=5000)	return "±¾Õ¾ÔªÀÏ";
        if(exp<=10000)	return "¿ª¹ú´óÀÏ";
        if(exp<=50000)  return "³¬¼¶´óÀÏ";
	return "·µÆÓ¹éÕæ";
}
*/
char *cexp(int exp, int type)
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
/*
	if(urec->userlevel & PERM_666LIFE)
		return 666;*/

    value = (time(0) - urec->lastlogin) / 60;
    /* new user should register in 30 mins */
    if (strcmp(urec->userid, "new") == 0)
    {
        return (30 - value) * 60;
    }
	if(urec->userlevel & PERM_666LIFE)
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
    tmp =
        x->numposts + x->numlogins / 3 + (time(0) -
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
        return;
    x->mode = newmode;
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
    while (fscanf(fp, "%s", buf3) > 0)
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
        return;
    sprintf(file, "home/%c/%s/friends", toupper(id[0]), id);
    fp = fopen(file, "r");
    if (fp)
    {
        friendnum = fread(fff, sizeof(fff[0]), 200, fp);
        fclose(fp);
    }
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
        return;
    sprintf(file, "home/%c/%s/rejects", toupper(id[0]), id);
    fp = fopen(file, "r");
    if (fp)
    {
        badnum = fread(bbb, sizeof(bbb[0]), 30, fp);
        fclose(fp);
    }
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

int init_all()
{
    srand(time(0) + getpid());
    chdir(BBSHOME);
    http_init();
    seteuid(BBSUID);
    if (geteuid() != BBSUID)
        http_fatal("uid error.");
    shm_init();
    loginok = user_init(&currentuser, &u_info);
    pic_mod = 2;
    if (loginok || !strcasecmp("guest",currentuser.userid) )
        u_info->idle_time = time(0);
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
        printf("TABLE,TH,TD,P,INPUT,BODY,SELECT {FONT-FAMILY: \"ËÎÌå\";}</style>\n");
        printf("</style>\n");
    }
    if (!silence && !my_ansisize_mode)
    {
        printf("<style type='text/css'>\n");
        printf("PRE, TEXTAREA {FONT-SIZE: 9pt; line-height: 9pt}</style>\n");
        printf("</style>\n");
    }
    if (!silence && my_ansifont_mode)
    {
        printf("<style type='text/css'>\n");
        printf("PRE, TEXTAREA {FONT-FAMILY: \"Verdana\", Tahoma, sans-serif, ËÎÌå;}</style>\n");
        printf("</style>\n");
    }
    if (!silence)
    {
	    if (my_theme_mode == 1)
		    printf("<link rel=stylesheet type=text/css href='"UPLOAD_SERVER_HEADER"/blue.css'>\n");
    	    else if (my_theme_mode == 2)
		    printf("<link rel=stylesheet type=text/css href='"UPLOAD_SERVER_HEADER"/red.css'>\n");
    }
}

int init_no_http()
{
    srand(time(0) + getpid());
    chdir(BBSHOME);
    shm_init();
}

char *void1(unsigned char *s)
{
    int i;
    int flag = 0;

    for (i = 0; s[i]; i++)
    {
        if (flag == 0)
        {
            if (s[i] >= 128)
                flag = 1;
            continue;
        }
        flag = 0;
        if (s[i] < 32)
            s[i - 1] = 32;
    }
    if (flag)
        s[strlen(s) - 1] = 0;
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

char *flag_str2(int access, int has_read)
{
    static char buf[80];

    strcpy(buf, "   ");
    if (loginok)
        strcpy(buf, "N  ");
    if (access & FILE_DIGEST)
        buf[0] = 'G';
    if (access & FILE_MARKED)
        buf[0] = 'M';
    if ((access & FILE_MARKED) && (access & FILE_DIGEST))
        buf[0] = 'B';
    if (has_read)
        buf[0] = tolower(buf[0]);
    if (buf[0] == 'n')
        buf[0] = ' ';
    return buf;
}

char *userid_str(char *s)
{
    static char buf[512];
    char buf2[256], tmp[256], *ptr, *ptr2;
    int len;

    strsncpy(tmp, s, 255);
    buf[0] = 0;
    ptr = strtok(tmp, " ,();\r\n\t");
    while (ptr && strlen(buf) < 400)
    {
        if (ptr2 = strchr(ptr, '.'))
        {
            ptr2[1] = 0;
            ptr2 = strchr(ptr, '<');
            if (ptr2)
                ptr2[0] = ' ';
            strcat(buf, ptr);
            strcat(buf, " ");
        }
        else
        {
            ptr = nohtml(ptr);
            sprintf(buf2, "<a href=bbsqry?userid=%s>%s</a> ", ptr, ptr);
            strcat(buf, buf2);
        }
        ptr = strtok(0, " ,();\r\n\t");
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == ' ')
        buf[len - 1] = 0;
    return buf;
}

int fprintf2(FILE * fp, char *s)
{
    int i, tail = 0, sum = 0;

    if (s[0] == ':' && s[1] == ' ' && strlen(s) > 79)
    {
        sprintf(s + 76, "..\n");
        fprintf(fp, "%s", s);
        return;
    }
    for (i = 0; s[i]; i++)
    {
        fprintf(fp, "%c", s[i]);
        sum++;
        if (tail)
        {
            tail = 0;
        }
        else if (s[i] < 0)
        {
            tail = s[i];
        }
        if (sum >= 78 && tail == 0)
        {
            fprintf(fp, "\n");
            sum = 0;
        }
    }
}

struct fileheader *get_file_ent(char *board, char *file)
{
    FILE *fp;
    char dir[80];
    static struct fileheader x;
    int num = 0;

    sprintf(dir, "boards/%s/.DIR", board);
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
        bzero(buf1, 80);
        bzero(buf2, 80);
        if (fscanf(fp, "%s %s", buf1, buf2) <= 0)
            break;
        if (buf1[0])
            buf1[strlen(buf1) - 1] = 0;
        if (buf1[0] == '*')
            continue;
        if (!strncmp(buf2, path, strlen(buf2))
                && !strncmp(buf2, path, strlen(path)))
            return buf1;
    }
    fclose(fp);
    return "";
}

int load_my_www()
{
    FILE *fp;
    char path[256], buf[256], buf1[256], buf2[256];

    if (!loginok)
        return;
    sprintf(path, "home/%c/%s/.mywww", toupper(currentuser.userid[0]),
            currentuser.userid);
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
    }
}

int has_fill_form()
{
    FILE *fp;
    int r;
    char userid[256], tmp[256], buf[256];

//modified by rwzmm @ sjtubbs, new register form!
//the following code is for old register modula 

//	fp = fopen("new_register", "r");
//    if (fp == 0)
//        return 0;
//    while (1)
//    {
//        if (fgets(buf, 100, fp) == 0)
//            break;
//        r = sscanf(buf, "%s %s", tmp, userid);
//        if (r == 2)
//        {
//            if (!strcasecmp(tmp, "userid:")
//                    && !strcasecmp(userid, currentuser.userid))
//            {
//                fclose(fp);
//                return 1;
//            }
//        }
//    }
//    fclose(fp);
//    return 0;
    
//end
	
////the following code is for new register modula

	struct regform form = {0};
	fp=fopen("register_sjtubbs", "r");
	if(fp == NULL)
		return 0;
	while(fread(&form, sizeof(struct regform), 1, fp) > 0)
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
	
////end
	

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
    fscanf(fp, "%g %g %g", &x[0], &x[1], &x[2]);
    fclose(fp);
    return x[i];
}

#include "sys/mman.h"

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

int check_maxmail2(int *maxmail, int *size)
{
    char filename[256], cmd[256], mailpath[256], maildir[256], tmpf[256];
    FILE *tempfile, *fp;

    *size = 0;
    *maxmail = 5000;
    if (!loginok)
        return 0;
    if (HAS_PERM(PERM_BASIC))
        *maxmail = 600;
    if (HAS_PERM(PERM_BOARDS))
        *maxmail = 1500;
    if (HAS_PERM(PERM_CHATCLOAK))
        *maxmail = 2500;
    sprintf(mailpath, "mail/%c/%s/", toupper(currentuser.userid[0]),
            currentuser.userid);
    sprintf(tmpf, "%s.size", mailpath);
    sprintf(maildir, "%s.DIR", mailpath);
    if (file_time(maildir) == file_time(tmpf))
    {
        fp = fopen(tmpf, "r");
        if (fp)
        {
            fscanf(fp, "%d %d", maxmail, size);
            fclose(fp);
        }
        return 1;
    }
    sprintf(filename, "/tmp/size.%s.%d", currentuser.userid, u_info->pid);
    sprintf(cmd, "/usr/bin/du %s > %s", mailpath, filename);
    system(cmd);
    if ((tempfile = fopen(filename, "r")) == NULL)
        return 0;
    if ((fscanf(tempfile, "%d", size)) == 0)
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
    //BUG
    //Bug-description:	if the subject of the topic contains '+', '/', '-', it would crash.
    //Bug-solution:		change this function like rawurlencode() in php
    //Bug-fixer:		hongliang
    //Date:			Nov. 19th, 2003 17:15
    static unsigned char hexchars[] = "0123456789ABCDEF";	//added by hongliang
    static char buf[512];
    int i, j = 0;

    bzero(buf, 512);
    //start-hongliang
    for (i = 0, j = 0; s[i] && j < 500; ++i, ++j)
    {
        buf[j] = (unsigned char) s[i];
        if ( (buf[j] < '/' && buf[i] != '-' && buf[i] != '.') || (buf[i] < 'A' && buf[i] > '9') ||
                (buf[i] > 'Z' && buf[i] < 'a' && buf[i] != '_') || (buf[i] > 'z') )
        {
            buf[j++] = '%';
            buf[j++] = hexchars[(unsigned char) s[i] >> 4];
            buf[j] = hexchars[(unsigned char) s[i] & 15];
        }
    }
    buf[j]	= '\0';

    return buf;

    //the following code was deleted by hongliang
    /*
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
    else if (s[i] == '%')
    {
    	strcat(buf, "%25");
    	j += 3;
    }
    else if (s[i] == '&')
    {
    	strcat(buf, "%26");
    	j += 3;
    }
    else
    {
    	buf[j] = s[i];
    	j++;
    }

       return buf;
       */
}

char *nohtml4(char *s)
{
    static char buf[512];
    int i, j = 0;

    bzero(buf, 512);
    for (i = 0; s[i] && i < 500; i++)
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

int tcp_open(char *host, int port)
{
    struct sockaddr_in x;
    int fd;
    bzero(&x, sizeof(struct sockaddr_in));
    x.sin_family = AF_INET;
    x.sin_addr.s_addr = inet_addr(host);
    x.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr *) &x, sizeof(struct sockaddr)) != -1)
        return fd;
    close(fd);
    return -1;
}

char *Gtime(time_t t)
{
    static char buf[40];
    char *s;

    if (!t)
        return "";
    t = t - 3600 * 8;
    s = Ctime(t);
    sprintf(buf, "%3.3s, %02d %3.3s %4.4s %8.8s GMT", s, atoi(s + 8),
            s + 4, s + 20, s + 11);
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

int today()
{
    return Day(time(0));
}

int tcp_bind(int port, void func(), int pid)
{
    static struct sockaddr_in xs;
    int i;
    int o;
    int fd;
    int fd2;

    o = 1;
    if (port <= 0)
        port = 80;
    if (fork())
        exit(0);
    for (i = getdtablesize(); i >= 0; i--)
        close(i);
    for (i = NSIG; i >= 1; i--)
        signal(i, SIG_IGN);
    xs.sin_family = AF_INET;
    xs.sin_port = htons(port);
    fd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &o, sizeof(o));
    if (bind(fd, (struct sockaddr *) &xs, sizeof xs) < 0)
        exit(1);
    if (listen(fd, 4) < 0)
        exit(1);
    setreuid(pid, pid);
    umask((mode_t) 022);
    while (1)
    {
        int n = sizeof(xs);

        fd2 = accept(fd, (struct sockaddr *) &xs, &n);
        if (fd2 <= 0)
            continue;
#ifdef DEBUG
	break;
#else
        if (fork() == 0)
            break;
#endif
        close(fd2);
    }
    dup2(fd2, 0);
    dup2(0, 1);
    dup2(1, 2);
    if (fd2 >= 3)
        close(fd2);
    nice(3);
    setenv("REMOTE_ADDR", (char *) inet_ntoa(*(int *) &xs.sin_addr), 1);
    func();
}

char *eff_size(char *file)
{
    FILE *fp;
    static char buf[256];
    int i, size, size2 = 0;

    size = file_size(file);
    if (size > 3000 || size == 0)
        goto E;
    size = 0;
    fp = fopen(file, "r");
    if (fp == 0)
        return "-";
    for (i = 0; i < 3; i++)
        if (fgets(buf, 255, fp) == 0)
            break;
    while (1)
    {
        if (fgets(buf, 255, fp) == 0)
            break;
        if (!strcmp(buf, "--\n"))
            break;
        if (!strncmp(buf, ": ", 2))
            continue;
        if (!strncmp(buf, "¡¾ ÔÚ ", 4))
            continue;
        if (strstr(buf, "¡ù À´Ô´:¡¤"))
            continue;
        for (i = 0; buf[i]; i++)
            if (buf[i] < 0)
                size2++;
        size += strlen(trim(buf));
    }
    fclose(fp);
E:
    if (size < 2048)
    {
        sprintf(buf,
                "(<font style='font-size:12px; color:#008080'>%d×Ö</font>)",
                size - size2 / 2);
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
    // add your code here;
}

int LoadMyBrdsFromGoodBrd(char userid[IDLEN + 2],
                          char mybrd[GOOD_BRC_NUM][80])
{
    FILE *fp;
    char fname[STRLEN];
    int i = 0;

    sprintf(fname, "home/%c/%s/.goodbrd", toupper(userid[0]), userid);
    if (fp = fopen(fname, "r"))
    {
        for (i = 0; i < GOOD_BRC_NUM; i++)
        {
            if (EOF == fscanf(fp, "%s\n", mybrd[i]))
                break;
        }
        fclose(fp);
    }
    return i;
}

void SaveMyBrdsToGoodBrd(char userid[IDLEN + 2],
                         char mybrd[GOOD_BRC_NUM][80], int mybrdnum)
{
    FILE *fp;
    char fname[STRLEN];
    int i;

    sprintf(fname, "home/%c/%s/.goodbrd", toupper(userid[0]), userid);
    if (fp = fopen(fname, "w"))
    {
        for (i = 0; i < mybrdnum; i++)
            fprintf(fp, "%s\n", mybrd[i]);
        fclose(fp);
    }

}

//add by mudboy 4 »ØÊÕÕ¾
void post_article2(char *board, char *title, char *file, char *id, 
				   unsigned int currid, unsigned int reid)
{
    FILE *fp;
    char buf[1024];
    struct fileheader header;
//	char temptitle[STRLEN];

//	memset(temptitle, 0x00, STRLEN);
    //strcpy(temptitle, header.title)
	bzero(&header, sizeof(header));

    strcpy(header.owner, id);
	header.id = currid;
	header.reid = reid;

    sprintf(header.filename, "%s", file);

//debug by rwzmm @ sjtubbs
//    strsncpy(header.title, title, 60);
	sprintf(header.title, "%-32.32s - %s", title, currentuser.userid);
//	strncpy(header.title, temptitle, STRLEN);
	
    header.accessed[0] = FILE_DELETED;
    sprintf(buf, "boards/%s/.RETURNDIR", board);
    fp = fopen(buf, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
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

void strtolower(dst, src)
char *dst, *src;
{
    for (; *src; src++)
        *dst++ = tolower(*src);
    *dst = '\0';
}

void log_usies(mode, mesg)
char *mode, *mesg;
{
    time_t now;
    char buf[256], *fmt;

    now = time(0);
    fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
    getdatestring(now, NA);
    sprintf(buf, fmt, datestring, mode, currentuser.userid, mesg);
    f_append("usies", buf);
}

//added by hongliang on Nov.21th, 2003 18:10 for
//Return a post from recycle bin
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


          //add by rwzmm@sjtubbs
          //write msg to targetuser's .msgdir
          int f_append_msg(char *path, char *uid, char *msg)
          {
              struct msgdir m;
              int fd;
              // tracerecord("begin append!\n");
              if ((fd=open(path, O_WRONLY | O_CREAT, 0644)) == -1)
              {
                  //		report("open file error in append_msg()");
                  // tracerecord("open file error in append_msg()\n");
                  return -1;
              }
              memset(&m,0,sizeof(m));
              strncpy(m.userid,uid,IDLEN+2);
              m.pid=getpid();	//here may cause a bug. worned by rwzmm@sjtubbs
              m.tt=time(NULL);
              strncpy(m.msg,msg,MAX_MSG_SIZE);
              flock(fd, LOCK_EX);
              lseek(fd, 0, SEEK_END);
              if (safewrite(fd, &m, sizeof(m)) == -1)
                  // tracerecord("apprec write err!\n");
                  //		report("apprec write err!");
                  flock(fd, LOCK_UN);
              close(fd);

              // tracerecord("end append!\n");
              return 0;

          }


          int safewrite(int fd, char *buf, int size)
          {
              int     cc, sz = size, origsz = size;
              char   *bp = buf;

              do
              {
                  cc = write(fd, bp, sz);
                  if ((cc < 0) && (errno != EINTR))
                  {
                      //			report("safewrite err!");
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


//created by rwzmm @ sjtubbs. ÐÂ×¢²áID 24Ð¡Ê±ÄÚ²»ÄÜPOST
int	too_young(struct userec *user, char *board)
{
  if(strcmp(board, "BBSHelp") == 0)
	  return 0;
  
  if(strcmp(board, "sysop") == 0)
	  return 0;
  
  if((time (0) - user->firstlogin) > 86400)
	  return 0;
  
  
  return 1;
}
		  


          // this function is for debug only, by rwzmm @ sjtubbs
          // it must be annotated in release version.

          //void tracerecord(char* buf, char* filename)
          //{
          //	FILE* fp;
          //	char path[256];
          //
          //	sprintf(path, "/home/rwzmm/%s", filename);
          //	fp = fopen(path, "w");
          //	fwrite(buf, 256, 1, fp);
          //	fclose(fp);
          //}

          //add end
int is_web_user(struct user_info *user)
{
	return (user->mode == 10001);
}
int kick_web_user(struct user_info *user)
{
	  int stay = 0;
	  FILE *fp;
	  struct userec u;
	  if (!user->active)
		  return -1;
	  stay =abs(time(0)-*(int *) (user->from + 32));
	  if(stay > 3600*24*3)
		  stay = 3600*24*3;
	  else if(stay < 0)
		  stay = 0;
	  
	  fp = fopen(".PASSWDS","r+");
	  fseek(fp,(user->uid-1)*sizeof(struct userec),SEEK_SET);
	  fread(&u,sizeof(struct userec),1,fp);
	  if( strcasecmp(u.userid,user->userid) )
	  {
		  fclose(fp);
		  memset(user,0,sizeof(struct user_info));
		  return -2;
	  }
	  u.stay += stay;
	  u.lastlogout = time(0);
	  fseek(fp,-sizeof(struct userec),SEEK_CUR);
	  fwrite(&u,sizeof(struct userec),1,fp);
	  fclose(fp);
	  memset(user,0,sizeof(struct user_info));
	  return 0;
}


//added by littlesword@SJTUbbs
//show dm and bm

void show_dm(char* userid)
{
	int dist = 12;

	dist = WHICHDIST( userid );
    if (( dist < 12 ) && ( dist > 0 ))
	{        
		printf(" [<font class=c31>");
		if (dist <10)
			printf("%d" , dist );
		else
			printf("%c" , 'A' + dist - 10);
			printf(" <font class=c32>ÇøÇø³¤<font class=c37>]");
	}
	else
		printf (" [<font class=c32>±¾Õ¾Çø³¤<font class=c37>]");
	return;
}


void show_bm(char firstchar,char* userid)
{
    FILE *stream;
    int i=0;
	int f = 0;
    char bm_file_path[28],buffer[51],*ch;
    sprintf(bm_file_path,"home/%c/%s/.bmfile",firstchar,userid);

    if((stream=fopen(bm_file_path,"r"))==NULL)
        printf (" [<font class=c32>±¾Õ¾°åÖ÷<font class=c37>]");
    else
    {
        
        while(fgets(buffer,32,stream)&& i<4)
        {
            if(ch = strchr(buffer,10))
                *ch='\0';
            if (strncmp(buffer , "District" , 8) ) 
			{
				if (f == 0)
					printf(" [<font class=c31>");
				printf ("%s ",buffer);
				f = 1;
			}
            i++;
        }
        fclose(stream);
        if (f) printf("<font class=c32>°å°åÖ÷<font class=c37>]");
    }
	return;
}
