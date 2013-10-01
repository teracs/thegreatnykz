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

#include "bbs.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/telnet.h>
#include <arpa/inet.h>

#ifdef SYSV
#include <sys/termios.h>
#else
#include <termios.h>
#endif

#ifdef LINUX
#include <sys/ioctl.h>
#endif

#ifdef LINUX

#endif

#define SOCKET_QLEN     4

#define TH_LOW          10
#define TH_HIGH         15

#define PID_FILE		BBSHOME"/reclog/bbsd.pid"
#define LOG_FILE		BBSHOME"/reclog/bbsd.log"
#define BAD_HOST		BBSHOME"/.bad_host"
#define NOLOGIN			BBSHOME"/NOLOGIN"

#ifdef  HAVE_CHKLOAD
#define BANNER  "\r\nª∂”≠π‚¡Ÿ[1;33m"BBSNAME"[m[ [1;32m"BBSHOST"[m ] [1;33m"BBSVERSION"[m «Î…‘∫Ú...\r\n[1;36m◊ÓΩ¸ [33m(1,10,15)[36m ∑÷÷”∆Ωæ˘∏∫∫…Œ™[33m %s [36m(…œœﬁ = %d) [%s][0m\r\n"
#else
#define BANNER  "\r\nª∂”≠π‚¡Ÿ[1;33m"BBSNAME"[m[ [1;32m"BBSHOST"[m] [1;33m"BBSVERSION"[m [bbsd ready]\r\n"
#endif

jmp_buf byebye;

extern char fromhost[60];
char genbuf[1024];
enum login_proto_t login_proto;

#ifdef REBOOT_CHKLOAD
char loadstr[1024];
#endif
char status[64];

#ifdef FOR_DOMAIN_NAME
char h_str[2000][32];
int ip[2000], h_total;

void loadhosts()
{
    FILE *fp;
    char tmphost[80], tmp[80], tmpip[80];

    h_total = 0;
    fp = fopen("etc/hosts", "r");
    while (1)
    {
        bzero(tmphost, 80);
        if (fscanf(fp, "%s %s %s %s", tmphost, tmp, tmp, tmpip) < 0)
            break;
        strncpy(h_str[h_total], tmphost, 28);
        ip[h_total] = inet_addr(tmpip);
        if (h_total++ >= 1999)
            break;
    }
    fclose(fp);
}

char *gethost(int x)
{
    int i;

    for (i = 0; i < 2000; i++)
    {
        if (ip[i] == 0)
            break;
        if (ip[i] == x)
            return h_str[i];
    }
    return (char *) inet_ntoa(x);
}
#endif
static void telnet_init(void)
{
    static char svr[] = {
        IAC, WILL, TELOPT_ECHO,
        IAC, WILL, TELOPT_SGA
    };
    struct timeval to;
    fd_set rset;
    char buf[256];

    FD_ZERO(&rset);
    FD_SET(0, &rset);
    send(0, svr, 6, 0);
    to.tv_sec = 3;
    to.tv_usec = 0;
    if (select(1, &rset, NULL, NULL, &to) > 0)
        recv(0, buf, sizeof(buf), 0);

}

#include <setjmp.h>

#define STRN_CPY(d,s,l) { strncpy((d),(s),(l)); (d)[(l)-1] = 0; }
#define RFC931_TIMEOUT   10
#define RFC931_PORT     113
#define ANY_PORT        0

static void getremotename_4(char *remotehost)
{
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    char rhost[50];

    if (getpeername(0, (struct sockaddr *) &from, &len))
    {
        char err[] = "can't get address\n";

        write(1, err, sizeof(err) - 1);
        exit(1);
    }
#ifdef FOR_DOMAIN_NAME
    (void) strcpy(remotehost,
                  gethost(inet_addr
                          (inet_ntop
                           (AF_INET, &from.sin_addr, rhost, 50))));
#else
    (void) strcpy(remotehost,
                  (char *) inet_ntop(AF_INET, &from.sin_addr, rhost, 50));
#endif
}

static void getremotename_6(char *remotehost)
{
    struct sockaddr_in6 from;
    socklen_t len = sizeof(from);
    char rhost[50];

    if (getpeername(0, (struct sockaddr *) &from, &len))
    {
        char err[] = "can't get address\n";

        write(1, err, sizeof(err) - 1);
        exit(1);
    }
    (void) strcpy(remotehost,
                  (char *) inet_ntop(AF_INET6, &from.sin6_addr, rhost,
                                     50));
}
static void getremotename_shell(char *remotehost)
{
    strcpy(remotehost, "127.0.0.1");
}

#ifdef REBOOT_CHKLOAD
int fkmem;

int chkload(limit)
int limit;
{
    double cpu_load[3];
    register int i;

#ifdef BSD44

    getloadavg(cpu_load, 3);
#elif defined(LINUX)

    FILE *fp;

    fp = fopen("/proc/loadavg", "r");

    if (!fp)
        cpu_load[0] = cpu_load[1] = cpu_load[2] = 0;
    else
    {
        float av[3];

        fscanf(fp, "%g %g %g", av, av + 1, av + 2);
        fclose(fp);
        cpu_load[0] = av[0];
        cpu_load[1] = av[1];
        cpu_load[2] = av[2];
    }
#else

#include <nlist.h>

#ifdef SOLARIS

#define VMUNIX  "/dev/ksyms"
#define KMEM    "/dev/kmem"

    static struct nlist nlst[] = {
        {"avenrun"},
        {0}
    };
#else

#define VMUNIX  "/vmunix"
#define KMEM    "/dev/kmem"

    static struct nlist nlst[] = {
        {"_avenrun"},
        {0}
    };
#endif

    long avenrun[3];
    static long offset = -1;
    int kmem;

    if ((kmem = open(KMEM, O_RDONLY)) == -1)
        return (1);

    if (offset < 0)
    {
        (void) nlist(VMUNIX, nlst);
        if (nlst[0].n_type == 0)
            return (1);
        offset = (long) nlst[0].n_value;
    }
    if (lseek(kmem, offset, L_SET) == -1)
    {
        close(kmem);
        return (1);
    }
    if (read(kmem, (char *) avenrun, sizeof(avenrun)) == -1)
    {
        close(kmem);
        return (1);
    }
    close(kmem);
#define loaddouble(la) ((double)(la) / (1 << 8))

    for (i = 0; i < 3; i++)
        cpu_load[i] = loaddouble(avenrun[i]);
#endif

    i = cpu_load[0];
    if (i < limit)
        i = 0;
    if (i)
    {
        strcpy(status, "≥¨∏∫∫…£¨«Î…‘∫Û‘Ÿ¿¥");
    }
    else if (cpu_load[0] >= (float) 0 && cpu_load[0] < (float) 1)
    {
        strcpy(status, "∏∫∫…’˝≥£");
    }
    else if (cpu_load[0] >= 1 && cpu_load[0] < (float) 10)
    {
        strcpy(status, "∏∫∫…∆´∏ﬂ");
    }
    else
        strcpy(status, "∏∫∫…π˝÷ÿ");
    sprintf(loadstr, "%.2f %.2f %.2f", cpu_load[0], cpu_load[1],
            cpu_load[2]);

    return i;
}
#endif

static void reapchild(int num)
{

    while ((waitpid(-1, NULL, WNOHANG | WUNTRACED)) > 0);
}

static void start_daemon(void)
{
    int n;
    unsigned int r;
    struct timeval tv;

#if 0
    char buf[80];

    time_t dummy = time(NULL);
    struct tm *dummy_time = localtime(&dummy);

    (void) strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

    n = getdtablesize();
    if (fork())
        exit(0);

    if (setsid() == -1)
        exit(0);
    if (fork())
        exit(0);

    sprintf(genbuf, "%d\t%s", getpid(), buf);

    while (n)
        (void) close(--n);

#endif
    for (n = 1; n <= NSIG; n++)
        (void) signal(n, SIG_IGN);
    gettimeofday(&tv, NULL);
    r = (unsigned int) (tv.tv_sec);
    r ^= ((unsigned int) tv.tv_usec) << 16;
    r ^= ((unsigned int) getppid()) << 8;
    r ^= ((unsigned int) getpid());
    srand(r);
}

static void close_daemon(int num)
{
    exit(0);
}

#if 0
static void bbsd_log(char *str)
{
    char buf[256];
    time_t mytime;
    struct tm *tm;

    mytime = time(0);
    tm = localtime(&mytime);
    sprintf(buf, "%.2d/%.2d/%.2d %.2d:%.2d:%.2d bbsd[%d]: %s",
            tm->tm_year % 100, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
            tm->tm_min, tm->tm_sec, getpid(), str);
    file_append(LOG_FILE, buf);
}
static int bind_port(port)
int port;
{
    int sock, on = 1;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

    xsin.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *) &xsin, sizeof xsin) < 0
        || listen(sock, SOCKET_QLEN) < 0)
    {
        exit(1);
    }

    sprintf(genbuf, "started on port %d\n", port);
    bbsd_log(genbuf);

    return sock;
}
#endif
static int bad_host(char *name)
{
    FILE *list;
    char buf[40], *ptr;

    if ((list = fopen(BAD_HOST, "r")))
    {
        while (fgets(buf, 40, list))
        {
            ptr = strtok(buf, " \n\t\r");
            if (ptr != NULL && *ptr != '#')
            {
                if (!strcmp(ptr, name))
                    return 1;
                if (ptr[0] == '#')
                    continue;
                if (ptr[0] == '-' && !strcmp(name, &ptr[1]))
                    return 0;
                if (ptr[strlen(ptr) - 1] == '.'
                    && !strncmp(ptr, name, strlen(ptr) - 1))
                    return 1;
                if (ptr[0] == '.' && strlen(ptr) < strlen(name) &&
                    !strcmp(ptr, name + strlen(name) - strlen(ptr)))
                    return 1;
            }
        }
        fclose(list);
    }
    return 0;
}
extern int get_tty(void);
extern void init_tty(void);
extern void reset_tty(void);

static pid_t bbs_shell_pid;
static void shell_reset_tty(void)
{
    if (getpid() == bbs_shell_pid)
        reset_tty();
}
int main(int argc, char *argv[])
{
    extern int errno;
    uid_t uid;

#ifdef HAVE_CHKLOAD
    int overload;
#endif

    uid = getuid();
    if (uid != BBSUID)
    {
        const char msg[] = "uid error, please run with user bbs\n";

        write(2, msg, sizeof(msg) - 1);
        exit(1);
    }
    if (setjmp(byebye))
    {
        exit(0);
    }
    start_daemon();

    (void) signal(SIGCHLD, reapchild);
    (void) signal(SIGTERM, close_daemon);

    (void) chdir(BBSHOME);
    umask((mode_t) 022);

#ifdef FOR_DOMAIN_NAME
    loadhosts();
#endif

#ifdef  HAVE_CHKLOAD
    overload = 0;
    overload = chkload(overload ? TH_LOW : TH_HIGH);
#endif

#ifdef HAVE_CHKLOAD
    sprintf(genbuf, BANNER, loadstr, TH_LOW, status);
#else

    sprintf(genbuf, BANNER);
#endif

    (void) write(2, genbuf, strlen(genbuf));

#ifdef NOLOGIN
    {
        FILE *fp;
        char buf[256];

#define MYBANNER "\r\nFB2000 [bbsd NOLOGIN] œµÕ≥¥¶”⁄[1;33m‘›Õ£µ«¬Ω[m◊¥Ã¨\r\n[1;32m[±æ’æ≥Ã–ÚŒ¨ª§ø…“‘…æ≥˝ \'[36m~bbs/NOLOGIN[32m\' ∫ÛΩ‚≥˝∏√◊¥Ã¨][m\r\n\r\n£Ω£Ω£Ω£Ω£Ω£Ωπÿ”⁄œµÕ≥Ω¯»Î‘›Õ£µ«¬Ω◊¥Ã¨µƒ°æπ´∏Ê°ø£Ω£Ω£Ω£Ω£Ω£Ω\r\n"

        if ((fp = fopen(NOLOGIN, "r")) != NULL)
        {
            (void) write(1, MYBANNER, strlen(MYBANNER));
            while (fgets(buf, 255, fp) != 0)
            {
                strcat(buf, "\r");
                (void) write(1, buf, strlen(buf));
            }
            fclose(fp);
            sleep(5);
            exit(0);
        }
    }
#endif

#ifdef  HAVE_CHKLOAD
    (void) close(fkmem);
#endif
    if (argc == 2)
    {
        if (!strcmp(argv[1], "--tcp4"))
        {
            getremotename_4(fromhost);
            login_proto = LOGIN_PROTO_TCP4;
            telnet_init();
        }
        else if (!strcmp(argv[1], "--tcp6"))
        {
            getremotename_6(fromhost);
            login_proto = LOGIN_PROTO_TCP6;
            telnet_init();
        }
        else if (!strcmp(argv[1], "--shell"))
        {
            getremotename_shell(fromhost);
            login_proto = LOGIN_PROTO_SHELL;
            get_tty();
            init_tty();
            bbs_shell_pid = getpid();
            atexit(shell_reset_tty);
        }
        else
        {
            goto WRONG_ARG;
        }
    }
    else
    {
      WRONG_ARG:
        {
            const char msg[] =
                "unknown argument, usage: bbsd --tcp4|--tcp6|--shell\n";
            write(2, msg, sizeof(msg) - 1);
            exit(1);
        }
    }
    close(2);

    if (bad_host(fromhost))
        exit(1);

    bbssetenv("REMOTEHOST", fromhost);

    start_client();
    return 1;
}
