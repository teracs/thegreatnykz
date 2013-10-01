#include "www.h"

extern int loadregistdetail();

extern struct registdetail alldetail[MAX_REGIST_NUM];

int bbsregist_main()
{
    struct registdetail detail;
    int type;
    int i = 0;
    int isopen = 0, max_allow_num = 0;
    int mynum = -1;

    init_all();
    loadregistdetail(&isopen, &max_allow_num);

    if (!isopen)
    {
        printf("<body>");
        printf("当前报名尚未开启或已经关闭。<br/>");
        printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
        printf("</body>");
    }
    else if (!loginok)
    {
        printf("<body>");
        printf("匆匆过客不能报名，请先登录。<br/>");
        printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
        printf("</body>");
    }
    else if (!(currentuser.userlevel & PERM_LOGINOK))
    {
        printf("<body>");
        printf("您尚未通过注册，不能报名。<br/>");
        printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
        printf("</body>");
    }
    else
    {
        type = atoi(getparm("type"));
        if (type == 2)
        {
            for (i = 1; i <= max_allow_num; i++)
            {
                if (strcmp(alldetail[i].userid, currentuser.userid) == 0)
                {
                    detail.bus = alldetail[i].bus;
                    strcpy(detail.email, alldetail[i].email);
                    detail.gender = alldetail[i].gender;
                    strcpy(detail.userid, alldetail[i].userid);
                    mynum = i;
                    break;
                }
            }
        }
        else if (type == 1)
        {
            detail.userid[0] = '\0';
            mynum = -1;
        }
        printf("<body>");
        printf
            ("<form action='bbsdoregist' method=get enctype='multipart/form-data'>\n");

        printf("<input type=\"hidden\" name=\"type\" value=\"%d\"/>",
               mynum == -1 ? 1 : 2);
        printf("<table>");
        printf("<tr>");
        printf("<td>");
        printf("您选择的号码:");
        printf("</td>");
        printf("<td>");

        if (mynum == -1)
        {
            for (i = 1; i <= max_allow_num; i++)
                if (alldetail[i].userid[0] == '\0')
                    break;

            if (i == max_allow_num + 1)
            {
                printf("<body>");
                printf("很抱歉，目前号码已全部用完。<br/>");
                printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
                printf("</body>");
                return 0;
            }
        }

        printf("<input name='number' maxlength='4' size='4' value='%d'/>",
               mynum == -1 ? i : mynum);

        printf("</td>");
        printf("</tr>");
        printf("<tr>");
        printf("<td>");
        printf("是否乘坐校车:");
        printf("</td>");
        printf("<td>");
        printf
            ("<input type=radio name=\"bus\" value=\"0\"%s/>不乘坐校车<br/>",
             detail.bus == 0 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"1\"%s/>乘校车来<br/>",
             detail.bus == 1 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"2\"%s/>乘校车回<br/>",
             detail.bus == 2 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"3\"%s/>均乘坐校车<br/>",
             detail.bus == 3 ? " checked" : "");
        printf("</td>");
        printf("</tr>");
        printf("</table>");
        printf
            ("<input type=\"button\" value='提交' onclick=\"verify();\"/>");
        printf("&nbsp;&nbsp;");
        printf
            ("<input type=\"button\" value='取消' onclick=\"confirmback();\"/>");
        printf("</form>");
        printf("</body>");
        printf("<script language=\"javascript\">\n");
        printf("function verify()\n");
        printf("{");
        printf("var num = document.getElementsByName('number')[0].value;");
        printf(" if ( isNaN(num) ) { \n");
        printf("alert('号码必须为数字');return false;}\n");
        printf("else if(num <= 0 || num > %d) { \n", max_allow_num);
        printf("alert('号码必须在1-%d之间');return false;}\n", max_allow_num);
        printf("else if(!(parseInt(num) == num)) {\n");
        printf("alert('号码必须为整数');return false;}\n");
        printf("if(confirm(\"确定提交吗?\"))");
        printf("document.forms[0].submit();}");
        printf("function confirmback(){");
        printf("if(confirm('确定要取消吗?'))");
        printf("window.open('bbsregistview','_self');}");
        printf("</script>\n");
    }
    return 0;
}
