#include "www.h"

static int is_rej_user(char *user1, char *user2)
{
    char buf[256];
    FILE *fp;
    struct override data;

    sethomefile(buf, user1, "rejects");
    fp = fopen(buf, "r");
    if (fp == 0)
        return 0;
    while (1)
    {
        if (fread(&data, sizeof(struct override), 1, fp) <= 0)
            break;
        if (!strcasecmp(data.id, user2))
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int bbssndmail_main()
{
    char userid[80], filename[80], title[80], title2[80], *content;
    int sig, backup;
    unsigned int i, len;
    struct userec *u;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客不能写信，请先登录");
    if (!(currentuser.userlevel & PERM_POST))
        http_quit_print("你目前没有发信的权限");
    if (!(currentuser.userlevel & PERM_PERSONAL))
        http_quit_print("你目前没有发信的权限");
    strsncpy(userid, getparm("userid"), 40);
    strsncpy(title, getparm("title"), 50);
    backup = strlen(getparm("backup"));
    if (!strstr(userid, "@"))
    {
        u = getuser(userid);
        if (u == 0)
            http_quit_print("错误的收信人帐号");
        if (currentuser.userlevel & PERM_FORCEPAGE)
            goto L;
        strcpy(userid, u->userid);
        if (!(u->userlevel & PERM_READMAIL))
            http_quit_print("此帐号不能收信");
        if (is_rej_user(currentuser.userid, userid)
            || is_rej_user(userid, currentuser.userid))
            http_quit_print("你无法给这个帐号发信");
    }
  L:
    len = strlen(title);
    for (i = 0; i < len; i++)
        if (title[i] < 27 && title[i] >= -1)
            title[i] = ' ';
    sig = atoi(getparm("signature"));
    content = getparm("text");
    if (title[0] == 0)
        strcpy(title, "没主题");
    sprintf(filename, "tmp/%d.tmp", getpid());
    unlink(filename);
    f_append(filename, content);
    sprintf(title2, "备份{%s} %s", userid, title);
    title2[72] = 0;
    post_mail(userid, title, filename, currentuser.userid,
              currentuser.username, fromhost, sig - 1);
    if (backup)
        post_mail(currentuser.userid, title2, filename,
                  currentuser.userid, currentuser.username, fromhost,
                  sig - 1);
    printf("信件已寄给%s.<br>\n", userid);
    if (backup)
        printf("信件已经备份.<br>\n");
    printf("<a href='javascript:history.go(-2)'>返回</a>");
    printf("</html>");
    return 0;
}
