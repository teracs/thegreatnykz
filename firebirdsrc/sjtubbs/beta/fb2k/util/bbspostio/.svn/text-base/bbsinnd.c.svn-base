
#include "../../include/bbs.h"
#include "sys/socket.h"
#include "netdb.h"
#include "netinet/in.h"
#include "stdarg.h"

#define PORT	10111
char *servernow = "";


/* iconf 的格式: 转信站地址	对方转信版面	本地转入版面 */

char *iconf[] = {
                    "bbs.nju.edu.cn		BBSDev		installbbs",
                    "bbs.nju.edu.cn		LinuxUnix	UNIXpost",
                    "bbs.nju.edu.cn		VB		post",
                    "bbs.nju.edu.cn		Aboard		post",
                    "bbs.nju.edu.cn		GRE_TOEFL	post",
                    "ytht.net		BBSDev		installbbs",
                    "ytht.net		Linux		UNIXpost",
                    "ytht.net		Unix		UNIXpost",
                    "ytht.net		AdvancedEdu	post",
                    "sbbs.seu.edu.cn	BBSDev		installbbs",
                    "sbbs.seu.edu.cn	Linux		UNIXpost",
                    "sbbs.seu.edu.cn	NewSoftware	post",
                    "bbs.xidian.edu.cn	XIDIAN		XDU",
                    "bbs.xidian.edu.cn	OuterNews	XDU",
                    "bbs.pku.edu.cn		HK_MC_TW	Hongkong",
                    NULL
                };


/* 转信黑名单: 不转入这些人的文章 */

char *black_user[] = {
                         "deliver",
                         "anonymous",
                         "guest",
                         "SYSOP",
                         "",
                         NULL
                     };

int valid_user(char *user)
{
    int i;
    char s2[80], *p;

    strcpy(s2, user);
    p = strchr(s2, '.');
    if (p)
        p[0] = 0;
    for (i = 0; black_user[i] != NULL && i < 999; i++)
        if (!strcasecmp(s2, black_user[i]))
            return 0;
    if (!strstr(user, "@"))
        return 0;
    return 1;
}

int do_log(char *fmt, ...)
{
    FILE *fp;
    va_list ap;
    char cmd[256];
    time_t t = time(0);

    va_start(ap, fmt);
    vsnprintf(cmd, 255, fmt, ap);
    va_end(ap);
    fp = fopen("bbshome/reclog/bbsinnd.log", "a");
    fprintf(fp, "%24.24s %-24s %s\n", ctime(&t), servernow, cmd);
    fclose(fp);
}

int last;

int main()
{
    int i;
    FILE *fp;

    if (fork())
        exit(0);
    chdir(BBSHOME);
    for (i = 0; i <= getdtablesize(); i++)
        close(i);
    for (i = 1; i <= NSIG; i++)
        signal(i, SIG_IGN);
    last = time(0) - 86400;     //首次运行的缺省值: 1天以内的文章。
    fp = fopen("etc/bbsinnd.last", "r");
    if (fp)
    {
        fscanf(fp, "%d", &last);
        fclose(fp);
    }
    while (1)
    {
        int r;
        char host[80], oboard[80], iboard[80];

        for (i = 0; iconf[i] != 0; i++)
        {
            r = sscanf(iconf[i], "%s %s %s", host, oboard, iboard);
            if (r < 3)
                continue;
            get_mail(host, oboard, iboard);
        }
        last = time(0);
        fp = fopen("etc/bbsinnd.last", "w");
        fprintf(fp, "%d\n", last);
        fclose(fp);
        sleep(3600);
    }
}

int get_mail(char *host, char *oboard, char *iboard)
{
    int fd;
    int i;
    struct sockaddr_in xs;
    struct hostent *he;
    char dir[80], file[80], buf[256], brk[80];
    char filename[20];

    //      file: 本地文件名, buf: 临时变量, brk: 分隔符.

    servernow = host;

    bzero((char *) &xs, sizeof(xs));
    xs.sin_family = AF_INET;
    if ((he = gethostbyname(host)) != NULL)
        bcopy(he->h_addr, (char *) &xs.sin_addr, he->h_length);
    else
        xs.sin_addr.s_addr = inet_addr(host);
    xs.sin_port = htons(PORT);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr *) &xs, sizeof(xs)) < 0)
    {
        close(fd);
        return do_log("can't conenct to %s", host);
    }
    dup2(fd, 0);
    if (fd > 0)
        close(fd);
    dup2(0, 1);
    if (fgets(brk, 80, stdin) == 0)
        goto E;
    if (strlen(brk) < 10)
        goto E;
    printf("select * from %s where dt < %d\n", oboard, time(0) - last);
    fflush(stdout);
    do_log("SEND request %s %d", oboard, time(0) - last);
    while (1)
    {
        FILE *fp;
        int t;
        struct fileheader x;

        bzero(&x, sizeof(x));
        if (fgets(x.title, 79, stdin) == 0)
            break;
        if (fgets(x.owner, 60, stdin) == 0)
            break;
        x.title[60] = 0;
        check_str(x.title);
        check_str(x.owner);
        do_log(x.title);
        for (i = 0; i < 100; i++)
        {
            t = time(0) + i;
            sprintf(filename, "M.%d.A", t);
            sprintf(file, "boards/%s/%s/%s", iboard, getfolderstr(filename), filename);
            sleep(1);
            if (!f_exist(file))
                break;
        }
        if (i >= 99)
            break;
        sprintf(x.filename, "M.%d.A", t);
        do_log(file);
        do_log(x.filename);
        fp = fopen(file, "w");
        while (1)
        {
            if (fgets(buf, 255, stdin) == 0)
                break;
            if (!strcmp(buf, brk))
                break;
            fprintf(fp, "%s", buf);
        }
        fclose(fp);
        if (!valid_user(x.owner))
        {
            unlink(file);
            do_log("bad user: %s", x.owner);
            continue;
        }
        sprintf(dir, "boards/%s/.DIR", iboard);
        fp = fopen(dir, "a");
        fwrite(&x, sizeof(x), 1, fp);
        fclose(fp);
    };
E:
    close(0);
    close(1);
    do_log("done");
}

int f_exist(char *file)
{
    struct stat buf;

    if (stat(file, &buf) == -1)
        return 0;
    return 1;
}

int check_str(unsigned char *s)
{
    int i;

    for (i = 0; i < strlen(s); i++)
        if (s[i] < 32 || s[i] == 255)
            s[i] = 0;
}
