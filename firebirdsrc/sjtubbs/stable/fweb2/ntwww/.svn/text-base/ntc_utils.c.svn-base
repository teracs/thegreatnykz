#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"

static int open_lock_file(const char *path, int open_flags, int lock_flags)
{
    int fd;

    fd = open(path, open_flags);
    if (fd < 0)
    {
        g_debug("can't open file at '%s'", path);
        return -1;
    }
    if (-1 == flock(fd, lock_flags))
    {
        close(fd);
        return -1;
    }
    return fd;
}

int nt_open_lock_file(const char *path, int flags)
{
    return open_lock_file(path, flags, LOCK_EX);
}
int nt_open_lock_file_nb(const char *path, int flags)
{
    return open_lock_file(path, flags, LOCK_EX | LOCK_NB);
}

void nt_close_lock_file(int fd)
{
    flock(fd, LOCK_UN);
    close(fd);
}

void nt_sleep(int sec)
{
    struct timeval t;

    t.tv_sec = sec;
    t.tv_usec = 0;
    select(0, 0, 0, 0, &t);
}

gboolean nt_file_append(char *file, char *buf)
{
    int fd;

    fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        g_warning("can't not open file at '%s'", file);
        return FALSE;
    }
    write(fd, buf, strlen(buf));
    close(fd);
    return TRUE;
}

time_t nt_stat_mtime(const char *path)
{
    struct stat buf;

    if (stat(path, &buf) == -1)
        return 0;
    else
        return buf.st_mtime;

}

off_t nt_stat_size(const char *path)
{
    struct stat buf;

    if (stat(path, &buf) == -1)
        return 0;
    else if (!S_ISREG(buf.st_mode))
        return 0;
    else
        return buf.st_size;
}

gboolean nt_touch_file(const char *path)
{
    int fd;

    if (!g_file_test(path, G_FILE_TEST_EXISTS))
    {
        fd = open(path, O_CREAT | O_RDWR);
        if (fd < 0)
            return FALSE;
        close(fd);
    }
    return -1 != utime(path, NULL);
}

gboolean nt_file_has_word(const char *file, const char *word)
{
    FILE *fp;
    char buf[256], buf2[256];

    fp = fopen(file, "r");
    if (fp == 0)
    {
        g_warning("can't open file at '%s'", file);
        return FALSE;
    }
    while (1)
    {
        if (fgets(buf, sizeof(buf), fp) == 0)
            break;
        sscanf(buf, "%s", buf2);
        if (!strcasecmp(buf2, word))
        {
            fclose(fp);
            return TRUE;
        }
    }
    fclose(fp);
    return FALSE;
}

char *nt_clear_ansi(char *buf)
{
    int i = 0, j = 0, mode = 0;

    while (buf[i])
    {
        if (mode == 0)
        {
            if (buf[i] == 27)
            {
                mode = 1;
            }
            else
            {
                buf[j++] = buf[i];
            }
        }
        else
        {
            if (buf[i] == ';'
                || buf[i] == '[' || (buf[i] >= '0' && buf[i] <= '9'))
                mode = 0;
        }
        i++;
    }
    buf[j] = 0;
    return buf;
}

char *nt_clear_htmltag(char *buf)
{
    int i = 0, j = 0, mode = 0;

    while (buf[i])
    {
        if (mode == 0)
        {
            if (buf[i] == '<')
            {
                mode = 1;
            }
            else
            {
                buf[j++] = buf[i];
            }
        }
        else
        {
            if (buf[i] == '>')
                mode = 0;
        }
        i++;
    }
    buf[j] = '\0';
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

static char *weeknum[] =
    { "天", "一", "二", "三", "四", "五", "六" };

char *nt_gtime(time_t t)
{
    struct tm *gt;
    static char buf[32];

    gt = gmtime(&t);
    sprintf(buf,
            "%s, %02d-%s-%04d %02d:%02d:%02d GMT",
            days[gt->tm_wday],
            gt->tm_mday,
            months[gt->tm_mon],
            gt->tm_year + 1900, gt->tm_hour, gt->tm_min, gt->tm_sec);
    return buf;
}

char *nt_ctime(time_t t)
{
    static char buf[32];

    memcpy(buf, ctime(&t), 24);
    buf[24] = '\0';
    return buf;
}

char *nt_ctime_cn(time_t t)
{
    static char buf[64];
    struct tm *tm;

    tm = localtime(&t);
    sprintf(buf, "%4d年%02d月%02d日%02d:%02d:%02d 星期%2s",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
    return buf;
}

int nt_ci_strcmp(register char *s1, register char *s2)
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

char *nt_getfolderstr(char *filename)
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

int nt_strsncpy(char *s1, char *s2, int n)
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

int nt_sig_append(FILE * fp, char *id, int sig)
{
    FILE *fp2;
    char path[256];
    char buf[100][256];
    int i, total;
    struct userec x;

    if (sig < 0 || sig > 10)
        return 0;
//    x = getuser(id);
    if (!nt_get_userec(id, &x))
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

int nt_fprintf2(FILE * fp, char *s)
{
    int i;
    static int tail = 0, sum = 0;
    static int in_link = 0;

    if (s[0] == ':' && s[1] == ' ' && strlen(s) > 79)
    {
        sprintf(s + 76, "..\n");
        fprintf(fp, "%s", s);
        return 0;
    }
    for (i = 0; s[i]; i++)
    {
        if (in_link)
        {
            if (nt_char_in_url(s[i]))
            {
                ++in_link;
            }
            else
            {
                in_link = 0;
            }
        }
        else if (strncasecmp(s + i, "http://", 7) == 0 ||
                 strncasecmp(s + i, "ftp://", 6) == 0 ||
                 strncasecmp(s + i, "mailto:", 7) == 0)
        {
            in_link = 1;
        }
        if (sum >= 78 && tail == 0 && (!in_link || in_link > 128))
        {
            fprintf(fp, "\n");
            sum = 0;
            in_link = 0;
        }
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
    }
    return 0;
}

int nt_file_exist(char *file)
{
    return nt_file_time(file) != 0;
}

int nt_file_time(char *file)
{
    return nt_f_stat(file)->st_mtime;
}

struct stat *nt_f_stat(char *file)
{
    static struct stat buf;

    bzero(&buf, sizeof(buf));
    if (stat(file, &buf) == -1)
        bzero(&buf, sizeof(buf));
    return &buf;
}

int nt_char_in_url(char c)
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

