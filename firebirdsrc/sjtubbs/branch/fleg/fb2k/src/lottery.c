#include "bbs.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
//#include <curses.h>
//#include <setjmp.h>
#define ASCFILEPATH "0Announce/groups/GROUP_0/JuhuiAdmin/asc/"

//static jmp_buf env_alrm;

void SelectMOde (char *ch);
void Asc_show ();
void Asc_more (int i);
void GetFid1 ();
void GetFid2 ();
int WaitInput (long usec);
void Show_sleep (long usec);

void
lottery ()
{
    char ch[3];

    modify_user_mode (SYSINFO);
    SelectMOde (ch);		//开头提示,选择抽奖模式
    if (ch[0] != '1' && ch[0] != '2')
        return;
    Asc_show ();			//抽奖中间动画
    if (ch[0] == '1')
        GetFid1 ();			//现场抽奖结果
    else
        GetFid2 ();			//在线抽奖结果
    while (ch[0] != 'q')
        read (0, ch, 1);
}

void
SelectMOde (char *ch)
{
    clear ();
    refresh ();
    getdata (0, 0, "切请选择抽奖模式 (1)现场 (2)在线 (按空格结束):", ch, 2,
             DOECHO, YEA);
}

void
Asc_show ()
{
    int i, n;
    char buf;

    alarm (0);

    for (n = 15; n > 0; n--)
    {
        Asc_more ((40 - n) % 25);
        Show_sleep (80000 + n * 8000);
    }

    while (buf != ' ')
    {
        Asc_more (rand () % 25);
        n++;
        i = WaitInput (80000);
        if (i == 0)
            continue;
        if (i == -1)
            break;
        read (0, &buf, 1);
    }

    for (n = 20; n > 0; n--)
    {
        Asc_more (rand () % 25);
        Show_sleep (200000 - n * 6000);

    }
}

void
Asc_more (int i)
{
    char file[80];

    sprintf (file, "%s%d", ASCFILEPATH, i);
    ansimore (file, NA);
}

void
GetFid1 ()
{
    int size, sum, num;
    char filename[STRLEN];
    struct stat st;
    struct override fboy;
    FILE *fp;

    size = sizeof (struct override);
    setuserfile (filename, "friends");
    if (stat (filename, &st) == -1)
        return;
    sum = st.st_size / size;

    if (sum == 0)
    {
        clear ();
        move (1, 24);
        prints ("[1;31m庆祝饮水思源BBS七周岁生日![m");
        move (9, 24);
        prints ("[1;41m见鬼阿, 现场一个人都没有![m");
        move (10, 38);
        prints ("请先把现场 ID 加入好友名单");
        move (20, 50);
        prints ("按 q 键退出");
        refresh ();
        return;
    }

    srand (time (NULL));
    num = rand () % sum;

    fp = fopen (filename, "r");
    fseek (fp, num * size, SEEK_SET);
    fread (&fboy, size, 1, fp);
    fclose (fp);

    clear ();
    move (1, 24);
    prints ("[1;31m庆祝饮水思源BBS七周岁生日![m");
    move (9, 24);
    prints ("本日现场获奖者为: [1;41m%s[m", fboy.id);
    move (10, 38);
    prints ("祝贺ta!");
    move (20, 50);
    prints ("按 q 键退出");
    refresh ();
}

void
GetFid2 ()
{
    int num;
    char buf[STRLEN];

    srand (time (NULL));
    num = rand () % count_users;
    fill_userlist ();
    getoluserid (buf, num);
    while (strcmp (buf, "guest") == 0)
        getoluserid (buf, num);

    clear ();
    move (1, 24);
    prints ("[1;31m庆祝饮水思源BBS七周岁生日![m");
    move (9, 24);
    //prints("本日BBS在线获奖者为: [1;41m%s %d %d[m",buf,num,count_users);
    prints ("本日BBS在线获奖者为: [1;41m%s[m", buf);
    //do_query(0,num);
    move (10, 38);
    prints ("祝贺ta!");
    move (20, 50);
    prints ("按 q 键退出");
    refresh ();
}

int
WaitInput (long usec)
{
    fd_set rfds;
    struct timeval tv;
    char retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO (&rfds);
    FD_SET (0, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = usec;

    retval = select (1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    return retval;
}

void
Asc_alarm (long usec)
{
    struct itimerval ascitimerval;

    ascitimerval.it_interval.tv_sec = 0;
    ascitimerval.it_interval.tv_usec = 0;
    ascitimerval.it_value.tv_sec = 0;
    ascitimerval.it_value.tv_usec = usec;
    setitimer (ITIMER_REAL, &ascitimerval, NULL);
}

static void
sig_alrm (int signo)
{
    // longjmp(env_alrm, 1);
}

void
Show_sleep (long usec)
{
    if (signal (SIGALRM, sig_alrm) == SIG_ERR)
        return;
    //if (setjmp(env_alrm)==0)
    {
        Asc_alarm (usec);
        pause ();
    }
    return;
}
