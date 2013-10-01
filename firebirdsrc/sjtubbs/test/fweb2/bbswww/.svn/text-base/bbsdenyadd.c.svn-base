#include "www.h"

static void inform2(char *board, char *user, char *exp, int dt);
static void show_form3(char *board);
int bbsdenyadd_main()
{
    int i;
    char exp[80], board[80], *userid;
    int dt;

    int fAutoUndeny;

    bzero(denyuser, sizeof(denyuser));
    denynum = 0;
    init_all();
    if (!loginok)
        http_quit_print("您尚未登录, 请先登录");
    strsncpy(board, getparm("board"), 30);
    strsncpy(exp, getparm("exp"), 30);
    dt = atoi(getparm("dt"));

    fAutoUndeny = atoi(getparm("autoundeny"));

    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("你无权进行本操作");
    loaddenyuser(board);
    userid = getparm("userid");
    if (userid[0] == 0)
    {
        show_form3(board);
        return 0;
    }
    if (getuser(userid) == 0)
        http_quit_print("错误的使用者帐号");
    strcpy(userid, getuser(userid)->userid);
    if (dt < 1 || dt > 30)
        http_quit_print("请输入被封天数(1-30)");
    if (exp[0] == 0)
        http_quit_print("请输入封人原因");
    for (i = 0; i < denynum; i++)
        if (!strcasecmp(denyuser[i].id, userid))
            http_quit_print("此用户已经被封");
    if (denynum > 40)
        http_quit_print("太多人被封了");
    strsncpy(denyuser[denynum].id, userid, 13);
    strsncpy(denyuser[denynum].exp, exp, 30);

    strsncpy(denyuser[denynum].autoundeny, fAutoUndeny ? "(a)" : "(n)", 4);
    getdatestring(time(0) + dt * 86400, NA);
    strsncpy(denyuser[denynum].free_time, datestring, 17);

    denynum++;
    savedenyuser(board);
    printf("封禁 %s 成功<br>\n", userid);
    inform2(board, userid, exp, dt);
    printf("[<a href=bbsdenyall?board=%s>返回被封帐号名单</a>]", board);
    printf("\n</html>");
    return 0;
}

static void show_form3(char *board)
{
    printf
        ("<center><font class=\"title\">%s - 板务管理　　</font>[讨论区: %s]<hr>\n",
         BBSNAME, board);
    printf
        ("<form action=bbsdenyadd><input type=hidden name=board value='%s'>",
         board);
    printf
        ("封禁使用者<input name=userid size=12> 本板POST权 <input name=dt size=2> 天, 原因<input name=exp size=20>\n");

    printf
        ("<input type=\"checkbox\" name=\"autoundeny\" value=\"1\" checked>自动解封 \n");
    printf("<input type=submit value=确认></form>");
}

static void inform2(char *board, char *user, char *exp, int dt)
{
    FILE *fp;
    char path[80], title[80];
    time_t now = time(0);

    sprintf(title, "[公告] %s被取消在%s板的发文权限POST权", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "【本公告由自动发信系统自动张贴】\n\n");
    fprintf(fp, "%s被板务人员[%s]封禁了%s板POST权[%d]天.\n", user,
            currentuser.userid, board, dt);
    fprintf(fp, "原因是: %s\n", exp);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "自动发信系统",
                 "自动发信系统", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME

    if (strncmp(board, "BMTraining", 10) == 0)
    {
        post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
                     "自动发信系统", "自动发信系统", -1, 0, now, now);
    }
    else
    {
        post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
                     "自动发信系统", "自动发信系统", -1, 0, now, now);
    }
#endif

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
    printf("系统已经发信通知了%s.<br>\n", user);
}
