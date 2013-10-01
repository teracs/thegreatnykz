#include "www.h"

int bbsnick_main()
{
    int i;
    char nick[80];

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客无法改变昵称");
    strsncpy(nick, getparm("nick"), 30);
    if (nick[0] == 0)
    {
        printf
            ("<center><font class=\"title\">%s - 临时改变昵称(环顾四方有效)　　</font>[使用者: %s]</center><hr>\n",
             BBSNAME, currentuser.userid);
        printf
            ("<form action=bbsnick>新昵称<input name=nick size=24 maxlength=24 type=text value='%s'> \n",
             void1(u_info->username));
        printf("<input type=submit value=确定>");
        printf("</form>");
        http_quit();
    }
    for (i = 0; nick[i]; i++)
        if ((nick[i] > 0 && nick[i] < '\x20') || nick[i] == '\xFF')
            nick[i] = ' ';
    strsncpy(u_info->username, nick, 32);
    printf("临时变更昵称成功");
    printf("</html>");
    return 0;
}
