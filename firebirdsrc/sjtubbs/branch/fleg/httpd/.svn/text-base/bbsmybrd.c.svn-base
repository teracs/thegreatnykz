#include "www.h"
static struct shortfile data[MAXBOARD];

//static char mybrd[32][80];
static char mybrd[GOOD_BRC_NUM][80];
static int mybrdnum = 0;

static int cmpboard(b1, b2)
struct boardheader *b1, *b2;
{
    return strcasecmp(b1->filename, b2->filename);
}

int bbsmybrd_main()
{
    int total = 0, i, type;
    /*char path[200];*/

    //      FILE *fp;
    init_all();
    if (!loginok)
        http_fatal("尚未登录或者超时");
    if (!(currentuser.userlevel & PERM_LOGINOK))
        http_fatal("未注册用户不能设定收藏夹");
    type = atoi(getparm("type"));
    if (type != 0)
    {
        read_submit();
        http_quit();
    }
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    printf("<center>\n");
    printf("<br>\n");
    printf("<style type=text/css>A {color: 000080} </style>\n");
    printf("您目前已经预定了%d个讨论区，最多可预定20个<hr color=green>\n",
           mybrdnum);
    printf("<form action=bbsmybrd?type=1&confirm1=1 method=post>\n");
    printf("<input type=hidden name=confirm1 value=1>\n");
    printf("<table>\n");
    for (i = 0; i < MAXBOARD; i++)
    {
        if (has_read_perm(&currentuser, shm_bcache->bcache[i].filename))
        {
            memcpy(&data[total], &(shm_bcache->bcache[i]),
                   sizeof(struct shortfile));
            total++;
        }
    }
    qsort(data, total, sizeof(struct shortfile), cmpboard);
    for (i = 0; i < total; i++)
    {
        char *buf3 = "";

        if (ismyboard(data[i].filename))
            buf3 = " checked";
        if (i % 3 == 0)
            printf("\n<tr>");
        printf
        ("<td><input type=checkbox name=%s %s><a href=bbsdoc?board=%s>%s(%s)</a>",
         data[i].filename, buf3, data[i].filename, data[i].filename,
         data[i].title + 11);
    }
    printf("</table><hr>\n");
    printf
    ("<input type=submit value=确认预定> <input type=reset value=复原>\n");
    printf("</form>\n");
    http_quit();
}

int ismyboard(char *board)
{
    int i;

    for (i = 0; i < mybrdnum; i++)
        if (!strcasecmp(board, mybrd[i]))
            return 1;
    return 0;
}

int read_submit()
{
    int i;
    /*char buf1[200];
    FILE *fp;*/
    int mybrdnum = 0;

    if (!strcmp(getparm("confirm1"), ""))
        http_fatal("参数错误");
    for (i = 0; i < parm_num; i++)
    {
        if (!strcasecmp(parm_val[i], "on"))
        {
            if (mybrdnum >= 20)
                http_fatal("您试图预定超过20个讨论区");
            if (!has_read_perm(&currentuser, parm_name[i]))
            {
                printf("警告: 无法预定'%s'讨论区<br>\n",
                       nohtml(parm_name[i]));
                continue;
            }
            strsncpy(mybrd[mybrdnum], parm_name[i], 80);
            mybrdnum++;
        }
    }
    SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum);
    printf
    ("<script>top.f2.location='bbsleft'</script>修改预定讨论区成功，您现在一共预定了%d个讨论区:<hr>\n",
     mybrdnum);
    printf("[<a href='javascript:history.go(-2)'>返回</a>]");
}
