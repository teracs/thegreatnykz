#include "www.h"
static struct user_info user[MAXACTIVE];

static int cmpuser(a, b)
struct user_info *a, *b;
{
    char id1[80], id2[80];

    sprintf(id1, "%d%s", !isfriend(a->userid), a->userid);
    sprintf(id2, "%d%s", !isfriend(b->userid), b->userid);
    return strcasecmp(id1, id2);
}

int bbsfriend_main()
{
    int i, total = 0;
    struct user_info *x;

    init_all();
    printf("<center>\n");
    printf("<br>\n");
    printf("<div align=right>使用者: %s</div><hr color=green>\n",
           currentuser.userid);
    for (i = 0; i < MAXACTIVE; i++)
    {
        x = &(shm_utmp->uinfo[i]);
        if (x->active == 0)
            continue;
        if (x->invisible && !HAS_PERM(PERM_SEECLOAK))
            continue;
        memcpy(&user[total], x, sizeof(struct user_info));
        total++;
    }
    printf("<table width=610>\n");
    printf
    ("<tr bgcolor=e8e8e8><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
    qsort(user, total, sizeof(struct user_info), cmpuser);
    for (i = 0; i < total; i++)
    {
        int dt = (time(0) - user[i].idle_time) / 60;

        if (!isfriend(user[i].userid))
            continue;
        printf("<tr><td>%d", i + 1);
        printf("<td>%s", "√");
        printf("%s",
               user[i].invisible ? "<font color=green>C</font>" : " ");
        printf("<td><a href=bbsqry?userid=%s>%s</a>", user[i].userid,
               user[i].userid);
        printf("<td><a href=bbsqry?userid=%s>%24.24s </a>", user[i].userid,
               nohtml(user[i].username));
        printf("<td>%20.20s ", user[i].from);
        printf("<td>%s",
               user[i].invisible ? "隐身中..." : ModeType(user[i].mode));
        if (dt == 0)
        {
            printf("<td> \n");
        }
        else
        {
            printf("<td>%d\n", dt);
        }
    }
    printf("</table>\n");
    if (total == 0)
        printf("目前没有好友在线");
    printf("<hr color=green>");
    printf("[<a href=bbsfall>全部好友名册</a>]");
    printf("</center>\n");
    http_quit();
}
