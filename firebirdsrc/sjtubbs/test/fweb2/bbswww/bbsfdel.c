#include "www.h"

int bbsfdel_main()
{
    FILE *fp;
    int i, total = 0;
    char path[80], userid[80];
    struct override f[200];

    init_all();
    if (!loginok)
        http_quit_print("您尚未登录，请先登录");
    sprintf(path, "home/%c/%c/%s/friends", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    printf
        ("<center><font class=\"title\">%s - 好友名单　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    strsncpy(userid, getparm("userid"), 13);
    if (userid[0] == 0)
    {
        printf("<form action=bbsfdel>\n");
        printf("请输入欲删除的好友帐号: <input type=text><br>\n");
        printf("<input type=submit>\n");
        printf("</form>");
        http_quit();
    }
    loadfriend(currentuser.userid);
    if (friendnum <= 0)
        http_quit_print("您没有设定任何好友");
    if (!isfriend(userid))
        http_quit_print("此人本来就不在你的好友名单里");
    for (i = 0; i < friendnum; i++)
    {
        if (strcasecmp(fff[i].id, userid))
        {
            memcpy(&f[total], &fff[i], sizeof(struct override));
            total++;
        }
    }
    fp = fopen(path, "w");
    fwrite(f, sizeof(struct override), total, fp);
    fclose(fp);
    printf
        ("[%s]已从您的好友名单中删除.<br>\n <a href=bbsfall>返回好友名单</a>",
         userid);
    printf("</html>");
    return 0;
}
