#include "www.h"
static int cloak = 0, /*pk = 0,*/ total = 0, guest = 0, www = 0, bbsnet =
                       0, /*dorm = 0,*/ people = 0, ip = 0, idle = 0;
static char id[MAXACTIVE][20], from[MAXACTIVE][40];

int showinfo_main()
{
    int i, j;

    init_all();
    printf("<center>\n");
    printf("<br><font color=green>BBS详细在线情况分析</font><br><br>\n");
    for (i = 0; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active)
        {
            if (abs(time(0) - shm_utmp->uinfo[i].idle_time) > 60)
                idle++;
            strcpy(id[total], shm_utmp->uinfo[i].userid);
            strcpy(from[total], shm_utmp->uinfo[i].from);
            if (shm_utmp->uinfo[i].mode == BBSNET)
                bbsnet++;
            if (is_web_user(&(shm_utmp->uinfo[i])))
                www++;
            if (shm_utmp->uinfo[i].invisible)
                cloak++;
            total++;
        }
    people = total;
    ip = total;
    for (i = 0; i < total; i++)
    {
        if (!strcmp(id[i], "guest"))
        {
            guest++;
            continue;
        }
        for (j = i + 1; j < total; j++)
        {
            if (id[i][0] == 0)
                continue;
            if (!strcmp(id[i], id[j]))
            {
                id[j][0] = 0;
                people--;
            }
        }
    }
    for (i = 0; i < total; i++)
    {
        if (from[i][0] == 0)
        {
            ip--;
            continue;
        }
        for (j = i + 1; j < total; j++)
            if (!strcmp(from[i], from[j]))
                from[j][0] = 0;
    }
    printf("总在线数 %d <br><br>\n", total);
    printf("登录用户分析<br>\n");
    printf
    ("<table width=400 style='BORDER: 2px solid; BORDER-COLOR: e8e8e8;'><tr><td>%20.20s   <td>%20s <td>%s%%\r\n",
     "项目", "数目", "所占百分比");
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "在线窗口", total,
           100);
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "隐身人数", cloak,
           100 * cloak / total);
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "在线ID数", people,
           100 * people / total);
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "Guests数", guest,
           100 * guest / total);
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "BBSNET数", bbsnet,
           100 * bbsnet / total);
    printf("<tr><td>%20.20s <td>%20d   <td>%2d%%\r\n", "WWW 人数", www,
           100 * www / total);
    printf("<table>\n");
}
