#include "www.h"

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
    struct user_info *x, *puser;
    time_t curtime;

    init_all();
    if (!loginok)
        http_quit_print("匆匆游客无此权限");
    puser =
        (struct user_info *) mp_alloc(MAXACTIVE *
                                      sizeof(struct user_info));
    bzero(puser, MAXACTIVE * sizeof(struct user_info));
    printf("<center>\n");
    printf("<br>\n");
    printf("<div align=right>使用者: %s</div><hr>\n", currentuser.userid);
    for (i = 0; i < MAXACTIVE; i++)
    {
        x = &(shm_utmp->uinfo[i]);
        if (x->active == 0)
            continue;
        if (x->invisible && !HAS_PERM(PERM_SEECLOAK))
            continue;
        memcpy(&puser[total], x, sizeof(struct user_info));
        total++;
    }
    printf("<table width=610>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
    qsort(puser, total, sizeof(struct user_info), cmpuser);
    curtime = time(0);
    for (i = 0; i < total; i++)
    {
        int dt = (curtime - puser[i].idle_time) / 60;

        if (!isfriend(puser[i].userid))
            continue;
        printf("<tr><td>%d", i + 1);
        printf("<td>%s", "√");
        printf("%s",
               puser[i].invisible ? "<font color=green>C</font>" : " ");
        printf("<td><a href=bbsqry?userid=%s>%s</a>", puser[i].userid,
               puser[i].userid);
        printf("<td><a href=bbsqry?userid=%s>%24.24s </a>",
               puser[i].userid, nohtml(puser[i].username));
        printf("<td>%20.20s ", puser[i].from);
        printf("<td>%s",
               puser[i].invisible ? "隐身中..." : ModeType(puser[i].mode));
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
    printf("<hr>");
    printf("[<a href=bbsfall>全部好友名册</a>]");
    printf("</center>\n");
    printf("</html>");
    return 0;
}
