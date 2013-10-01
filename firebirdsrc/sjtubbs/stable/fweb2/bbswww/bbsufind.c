#include "www.h"

static int cmpuser(a, b)
struct user_info *a, *b;
{
    char id1[80], id2[80];

    sprintf(id1, "%d%s", !isfriend(a->userid), a->userid);
    sprintf(id2, "%d%s", !isfriend(b->userid), b->userid);
    return strcasecmp(id1, id2);
}

int bbsufind_main()
{
    int i, total = 0, total2 = 0;
    struct user_info *x;
    struct user_info *user;
    char search;

    cache_age = 60 * 5;
    init_all();
    user = (void *) mp_alloc(MAXACTIVE * sizeof(struct user_info));
    printf("<center>\n");
    printf("<br>");
    printf("<table width=640><tr><td align=right>目前在线: %d人</table>\n",
           count_online());
    printf("<hr>\n");
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
    search = toupper(getparm("search")[0]);
    if (!strcmp(getparm("search"), "数字"))
        search = '0';
    if (search != '*' && search != '0' && (search < 'A' || search > 'Z'))
    {
        http_quit_print("错误的参数");
    }
    if (search == '0')
    {
        printf("列出以数字开头的在线使用者.<br>\n");
    }
    else if (search == '*')
    {
        printf("列出所有在线使用者.<br>\n");
    }
    else
    {
        printf("列出以字母'%c'开头的在线使用者.<br>\n", search);
    }

    printf("<table width=610>\n");
    printf
        ("<tr bgcolor=f0f0f0><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
    qsort(user, total, sizeof(struct user_info), cmpuser);
    for (i = 0; i < total; i++)
    {
        int dt = (time(0) - user[i].idle_time) / 60;

        if (strchr("0123456789", user[i].userid[0]) && search == '0')
        {
            ;
        }
        else
        {
            if (toupper(user[i].userid[0]) != search && search != '*')
                continue;
        }
        printf("<tr><td>%d", i + 1);
        printf("<td>%s", isfriend(user[i].userid) ? "√" : "  ");
        printf("%s",
               user[i].invisible ? "<font color=green>C</font>" : " ");
        printf("<td><a href=bbsqry?userid=%s>%s</a>", user[i].userid,
               user[i].userid);
        printf("<td><a href=bbsqry?userid=%s>%24.24s </a>",
               user[i].userid, nohtml(user[i].username));
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
        total2++;
    }
    printf("</table><br>\n");
    printf("本项在线: %d人, ", total2);
    printf("<a href='javascript:history.go(-1)'>返回上一页</a>");
    printf("</center>\n");
    http_quit();
    return 0;
}
