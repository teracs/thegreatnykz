#include "www.h"
static struct deny
{
    char id[80];
    char exp[80];
    //added by hongliang on Nov. 23th, 2003 for whether auto undeny
    char autoundeny[10];
    char free_time[30];
}
denyuser[256];
static int denynum = 0;

static int loaddenyuser(char *board)
{
    FILE *fp;
    char path[80], buf[256];

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "r");
    if (fp == 0)
        return;
    while (denynum < 100)
    {
        if (fgets(buf, 80, fp) == 0)
            break;
        sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
               denyuser[denynum].exp, denyuser[denynum].autoundeny, denyuser[denynum].free_time);
        denynum++;
    }
    fclose(fp);
}

static int savedenyuser(char *board)
{
    FILE *fp;
    int i;
    char path[80], *exp;

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "w");
    if (fp == 0)
        return;
    for (i = 0; i < denynum; i++)
    {
        int m;

        exp = denyuser[i].exp;
        if (denyuser[i].id[0] == 0)
            continue;
        for (m = 0; exp[m]; m++)
        {
            if (exp[m] <= 32 && exp[m] > 0)
                exp[m] = '.';
        }
        //fprintf(fp, "%-12s %s %d\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
        fprintf(fp, "%-12s %-40s %s %16s\n", denyuser[i].id,
                denyuser[i].exp, denyuser[i].autoundeny, denyuser[i].free_time);
        denynum++;
    }
    fclose(fp);
}

int bbsdenydel_main()
{
    int i;
    char board[80], *userid;

    init_all();
    if (!loginok)
        http_fatal("您尚未登录, 请先登录");
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    if (!has_BM_perm(&currentuser, board))
        http_fatal("你无权进行本操作");
    loaddenyuser(board);
    userid = getparm("userid");
    for (i = 0; i < denynum; i++)
    {
        if (!strcasecmp(denyuser[i].id, userid))
        {
            denyuser[i].id[0] = 0;
            savedenyuser(board);
            printf("已经给 %s 解封. <br>\n", userid);
            inform(board, userid);
            printf("[<a href=bbsdenyall?board=%s>返回被封名单</a>]",
                   board);
            http_quit();
        }
    }
    http_fatal("这个用户不在被封名单中");
    http_quit();
}

int inform(char *board, char *user)
{
    FILE *fp;
    char path[80], title[80];
	time_t now = time(0);

    sprintf(title, "[公告] 恢复%s在%s板的POST权", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "【本公告由自动发信系统自动张贴】\n\n");
    fprintf(fp, "%s被板务人员[%s]恢复了%s板POST权.\n", user,
            currentuser.userid, board);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "自动发信系统",
                 "自动发信系统", -1, 0, now, now);

    //BUG
    //Bug-description:	web can not auto post a article to DENY_POST_BOARDNAME('penalty' in SJTUBBS)
    //Bug-solution:		post it :)
    //Bug-fixer:		hongliang
    //Date:			Nov. 21th, 2003 14:14
    //start-hongliang
#ifdef DENY_POST_BOARDNAME

	if (strncmp(board, "BMTraining", 10) == 0)
	{
		post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS", "自动发信系统",
			"自动发信系统", -1, 0, now, now);
	}
	else
	{
		post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS", "自动发信系统",
			"自动发信系统", -1, 0, now, now);
	}
#endif
    //end-hongliang

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
    printf("系统已经发信通知了%s.<br>\n", user);
}

//this function is added by hongliang on Nov. 23th, 2003 13:00 for auto save deny users
void bbsautoundeny(char board[80], char *userid)
{
    int i;

    loaddenyuser(board);
    for (i = 0; i < denynum; i++)
    {
        if (!strcasecmp(denyuser[i].id, userid))
        {
            denyuser[i].id[0] = 0;
            savedenyuser(board);
            inform_autoundeny(board, userid);
        }
    }
}

void inform_autoundeny(char *board, char *user)
{
    FILE *fp;
    char path[80], title[80];
	time_t now = time(0);

    sprintf(title, "[公告] 恢复%s在%s板的POST权", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "【本公告由自动发信系统自动张贴】\n\n");
    fprintf(fp, "\n  %s 网友：\n\n\t因封禁时间已过，现恢复您在 [%s] 板的『发表文章』权力。\n\n"
            "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS",
            user, board);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "自动发信系统",
                 "自动发信系统", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME
	if (strncmp(board, "BMTraining", 10) == 0)
	{
		post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS", "自动发信系统",
			"自动发信系统", -1, 0, now, now);
	}
	else
	{
		post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS", "自动发信系统",
                 "自动发信系统", -1, 0, now, now);
	}
#endif

    post_mail(user, title, path, currentuser.userid, currentuser.username, fromhost, -1);
    unlink(path);
}
