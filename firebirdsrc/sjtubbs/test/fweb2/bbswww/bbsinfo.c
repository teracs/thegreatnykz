#include "www.h"

static int check_info()
{
    unsigned int m;
    char buf[256];

    strsncpy(buf, getparm("nick"), 30);
    for (m = 0; m < strlen(buf); m++)
        if ((buf[m] < 32 && buf[m] > 0) || buf[m] == -1)
            buf[m] = ' ';
    if (strlen(buf) > 1)
    {
        strcpy(currentuser.username, buf);
    }
    else
    {
        printf("警告: 昵称太短!<br>\n");
    }
    strsncpy(buf, getparm("realname"), 9);
    if (strlen(buf) > 1)
    {
        strcpy(currentuser.realname, buf);
    }
    else
    {
        printf("警告: 真实姓名太短!<br>\n");
    }
    strsncpy(buf, getparm("address"), 40);
    if (strlen(buf) > 8)
    {
        strcpy(currentuser.address, buf);
    }
    else
    {
        printf("警告: 居住地址太短!<br>\n");
    }
    strsncpy(buf, getparm("email"), 32);
    if (strlen(buf) > 8 && strchr(buf, '@'))
    {
        strcpy(currentuser.email, buf);
    }
    else
    {
        printf("警告: email地址不合法!<br>\n");
    }
    strsncpy(buf, getparm("year"), 5);
    if (atoi(buf) > 1910 && atoi(buf) < 1998)
    {
        currentuser.birthyear = atoi(buf) - 1900;
    }
    else
    {
        printf("警告: 错误的出生年份!<br>\n");
    }
    strsncpy(buf, getparm("month"), 3);
    if (atoi(buf) > 0 && atoi(buf) <= 12)
    {
        currentuser.birthmonth = atoi(buf);
    }
    else
    {
        printf("警告: 错误的出生月份!<br>\n");
    }
    strsncpy(buf, getparm("day"), 3);
    if (atoi(buf) > 0 && atoi(buf) <= 31)
    {
        currentuser.birthday = atoi(buf);
    }
    else
    {
        printf("警告: 错误的出生日期!<br>\n");
    }
    strsncpy(buf, getparm("gender"), 2);
    if (!strcasecmp(buf, "F"))
        currentuser.gender = 'F';
    if (!strcasecmp(buf, "M"))
        currentuser.gender = 'M';
    strsncpy(buf, getparm("exptype"), 3);
    if (!strcasecmp(buf, "E1"))
        currentuser.user_exp_type = 0;
    if (!strcasecmp(buf, "E2"))
        currentuser.user_exp_type = 1;
    if (!strcasecmp(buf, "E3"))
        currentuser.user_exp_type = 2;
    if (!strcasecmp(buf, "E4"))
        currentuser.user_exp_type = 3;
    save_user_data(&currentuser);
    printf("[%s] 个人资料修改成功.", currentuser.userid);
    return 0;
}

int bbsinfo_main()
{
    int type;

    init_all();
    if (!loginok)
        http_quit_print("您尚未登录");
    type = atoi(getparm("type"));
    printf
        ("<center><font class=\"title\">%s - 设置个人资料　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);

    if (type != 0)
    {
        check_info();
        http_quit();
    }
    printf("<form action=bbsinfo method=post>");
    printf("<input type=hidden name=type value=1>\n");
    printf("<table>\n");
    printf("<tr><td>您的帐号： <td>%s\n", currentuser.userid);
    printf
        ("<tr><td>您的昵称： <td><input type=text name=nick value='%s' size=24 maxlength=30>\n",
         currentuser.username);
    printf("<tr><td>发表大作： <td>%d 篇<br>\n", currentuser.numposts);
    printf("<tr><td>信件数量： <td>%d 封<br>\n", currentuser.nummails);
    printf("<tr><td>上站次数： <td>%d 次<br>\n", currentuser.numlogins);
    printf("<tr><td>上站时间： <td>%ld 分钟<br>\n", currentuser.stay / 60);
    printf
        ("<tr><td>真实姓名： <td><input type=text name=realname value='%s' size=16 maxlength=16>\n",
         currentuser.realname);
    printf
        ("<tr><td>居住地址： <td><input type=text name=address value='%s' size=40 maxlength=40>\n",
         currentuser.address);
    printf("<tr><td>帐号建立： <td>%s\n", Ctime(currentuser.firstlogin));
    printf("<tr><td>最近光临： <td>%s\n", Ctime(currentuser.lastlogin));
    printf("<tr><td>来源地址： <td>%s\n", currentuser.lasthost);
    printf
        ("<tr><td>电子邮件： <td><input type=text name=email value='%s' size=32 maxlength=32>\n",
         currentuser.email);
    printf
        ("<tr><td>出生日期： <td><input type=text name=year value=%d size=4 maxlength=4>年",
         currentuser.birthyear + 1900);
    printf("<input type=text name=month value=%d size=2 maxlength=2>月",
           currentuser.birthmonth);
    printf("<input type=text name=day value=%d size=2 maxlength=2>日\n",
           currentuser.birthday);
    printf("<tr><td>您的性别： ");
    printf("<td>男<input type=radio value=M name=gender %s>, ",
           currentuser.gender == 'M' ? "checked" : "");
    printf("女<input type=radio value=F name=gender %s>\n",
           currentuser.gender == 'F' ? "checked" : "");

    printf("</table><hr>\n");
    printf
        ("<input type=submit value=确定> <input type=reset value=复原>\n");
    printf("</form>");
    printf("</center>\n");
    printf("</html>");
    return 0;
}
