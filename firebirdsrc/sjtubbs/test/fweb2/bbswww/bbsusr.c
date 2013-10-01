#include "www.h"

static int cmpuser(a, b)
struct user_info *a, *b;
{
    char id1[80], id2[80];

    sprintf(id1, "%d%s", !isfriend(a->userid), a->userid);
    sprintf(id2, "%d%s", !isfriend(b->userid), b->userid);
    return strcasecmp(id1, id2);
}

int bbsusr_main()
{
    int i, start, total = 0;
    struct user_info *x;
    struct user_info *user;

    cache_age = 60 * 3;
    init_all();
    user = (void *) mp_alloc(MAXACTIVE * sizeof(struct user_info));
    printf("<center>\n");
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
    start = atoi(getparm("start"));
    if (my_t_lines < 10 || my_t_lines > 40)
        my_t_lines = 20;
    printf("<table width=610>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
    qsort(user, total, sizeof(struct user_info), cmpuser);
    if (start > total - 5)
        start = total - 5;
    if (start < 0)
        start = 0;
    for (i = start; i < start + my_t_lines && i < total; i++)
    {
        int dt = (time(0) - user[i].idle_time) / 60;

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
    }
    printf("</table>\n");
    printf("<table width=610><tr><td align=left>");
    printf("<a href='bbsufind?search=*'>全部</a> ");
    if (start > 0)
        printf("<a href=bbsusr?start=%d>上一页</a> ", start - 20);
    if (start < total - my_t_lines)
        printf("<a href=bbsusr?start=%d>下一页</a>", start + my_t_lines);
    printf("<td align=right>");
    for (i = 'A'; i <= 'Z'; i++)
    {
        printf("<a href=bbsufind?search=%c>%c</a>,", i, i);
    }
    printf("<a href=bbsufind?search=数字>数字</a>\n");
    printf("</table>\n");
    printf("<br><form action=bbsusr>\n");
    printf("<input type=submit value=跳转到第> ");
    printf("<input type=input size=4 name=start> 个使用者</form>");
    printf("</center>\n");
    printf("</html>");
    return 0;
}
