#include "www.h"

char *defines[] = {
    "呼叫器关闭时可让好友呼叫",
    "接受所有人的讯息",
    "接受好友的讯息",
    "收到讯息发出声音",
    "使用彩色",
    "显示活动看板",
    "显示选单的讯息栏",
    "编辑时显示状态栏",
    "讯息栏采用一般/精简模式",
    "选单采用一般/精简模式",
    "分类讨论区以 New 显示",
    "阅读文章是否使用绕卷选择",
    "阅读文章游标停於第一篇未读",
    "进站时显示好友名单",
    "好友上站通知",
    "观看留言板",
    "不要送出上站通知给好友",
    "主题式看板",
    "收到讯息等候回应或清除",
    "进站时观看上站人次图",
    "进站时观看十大排行板",
    "使用乱数签名档",
    "显示星座",
    "星座使用颜色来显示性别",
    "使用\'+\'标记未读文章",
    "汉字整字删除",
    "自动排板宽度预设为 78 列",
    "使用GB码阅读",
    "拒收陌生人的群体信件", 
    "显示板主寄语", 
    "阅读文章不使用光标模式", 
    "扩展定义4",
    NULL
};
static int read_form()
{
    int i, perm = 1, def = 0;
    char var[100];

    for (i = 0; i < 32; i++)
    {
        sprintf(var, "perm%d", i);
        if (strlen(getparm(var)) == 2)
            def += perm;
        perm = perm * 2;
    }
    currentuser.userdefine = def;
    save_user_data(&currentuser);
    printf
        ("个人参数设置成功.<br><a href=bbsparm>返回个人参数设置选单</a>");
    return 0;
}

int bbsparm_main()
{
    int i, perm = 1, type;

    init_all();
    type = atoi(getparm("type"));
    printf
        ("<center><font class=\"title\">%s - 修改个人参数　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    if (!loginok)
        http_quit_print("匆匆过客不能设定参数");
    if (type)
        return read_form();
    printf("<form action=bbsparm method=post>\n");
    printf("<input type=hidden name=type value=1>\n");
    printf("<table width=610>\n");
    for (i = 0; defines[i]; i++)
    {
        char *ptr = "";

        if (i % 2 == 0)
            printf("<tr>\n");
        if (currentuser.userdefine & perm)
            ptr = " checked";
        printf("<td><input type=checkbox name=perm%d%s><td>%s", i,
               ptr, defines[i]);
        perm = perm * 2;
    }
    printf("</table><br><hr>");
    printf
        ("<input type=submit value=确定></form>以上参数大多仅在telnet方式下才有作用\n");
    printf("</html>");
    return 0;
}
