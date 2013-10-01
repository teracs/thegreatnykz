#include "www.h"

int bbspwd_main()
{
    int type;
    char pw1[20], pw2[20], pw3[20];

    init_all();
    if (!loginok)
        http_quit_print("您尚未登录, 请先登录");
    type = atoi(getparm("type"));
    if (type == 0)
    {
        printf
            ("<center><font class=\"title\">%s - 修改密码　　</font>[使用者: %s]<hr>\n",
             BBSNAME, currentuser.userid);
        printf("<form action=bbspwd method=post>\n");
        printf("<input type=hidden name=type value=1>\n");
        printf("<table>\n");
        printf("<tr><td>使用者帐号: <td>%s\n", currentuser.userid);
        printf
            ("<tr><td>你的旧密码: <td><input maxlength=12 size=12 type=password name=pw1>\n");
        printf
            ("<tr><td>你的新密码: <td><input maxlength=12 size=12 type=password name=pw2>\n");
        printf
            ("<tr><td>再输入一次: <td><input maxlength=12 size=12 type=password name=pw3>\n");
        printf("</table><hr>\n");
        printf("<input type=submit value=确定>\n");
        http_quit();
    }
    strsncpy(pw1, getparm("pw1"), 13);
    strsncpy(pw2, getparm("pw2"), 13);
    strsncpy(pw3, getparm("pw3"), 13);
    if (strcmp(pw2, pw3))
        http_quit_print("两次密码不相同");
    if (strlen(pw2) < 2)
        http_quit_print("新密码太短");
    if (!checkpasswd(currentuser.passwd, pw1))
        http_quit_print("密码不正确");
    strsncpy(currentuser.passwd, crypt_passwd(pw2), 14);
    save_user_data(&currentuser);
    printf("[%s] 密码修改成功.", currentuser.userid);
    printf("</html>");
    return 0;
}
