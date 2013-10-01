#include "www.h"

#define MAXREGIST (2500)

extern int loadregistdetail();

struct registdetail
{
	char id[IDLEN + 1];
	char email[REAL_NAME_EMAIL_LEN];
	int num;
	int gender;
	int bus;	// 0: 否; 1: 来, 2: 回; 3: both
};

extern struct registdetail alldetail[MAXREGIST];

int
bbsregist_main()
{
    FILE *stream, *fp;
    char bm_file_path[36],email[REAL_NAME_EMAIL_LEN];
	struct registdetail detail;
	int type;
	int i = 0;
	char *status_file = "vote/regist_status";
	int regIsOpen = 0;

	init_all();

	if ((fp = fopen(status_file, "r")) != NULL){
		fscanf(FCGI_ToFILE(fp), "%d", &regIsOpen);
		fclose(fp);
	}

	if (!regIsOpen){
		printf("<body>");
		printf("当前报名尚未开启或已经关闭。<br/>");
		printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
		printf("</body>");
	}
	else if (!loginok)	// guest 不能报名
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
		loadregistdetail();
		type = atoi(getparm("type"));
		if (type == 2)	// 修改
		{
			for (i = 0; i < MAXREGIST; i++)
			{
				if (strcmp(alldetail[i].id, currentuser.userid) == 0)
				{
					detail.bus = alldetail[i].bus;
					strcpy(detail.email, alldetail[i].email);
					detail.gender = alldetail[i].gender;
					strcpy(detail.id, alldetail[i].id);
					detail.num = alldetail[i].num;
					break;
				}
			}
		}
		else if (type == 1)	// 新增
		{
			detail.id[0] = '\0';
			detail.num = 0;
		}
		sprintf(bm_file_path,"home/%c/%c/%s/real_email",toupper(currentuser.userid[0]),toupper (currentuser.userid[1]), currentuser.userid);
		if ((stream=fopen(bm_file_path,"r"))==NULL)
			sprintf(email, "NO_REAL_EMAIL");
		else
		{
			fgets(email,32,stream);
			fclose(stream);
		}
		printf("<body>");
		printf("<form action='bbsdoregist' method=post enctype='multipart/form-data'>\n");
		//type: 1: 增; 2: 改; 3: 删
		printf("<input type=\"hidden\" name=\"type\" value=\"%d\"/>", detail.num > 0 ? 2 : 1);
		printf("<input type=\"hidden\" name=\"email\" value=\"%s\"/>", email);
		printf("<input type=\"hidden\" name=\"id\" value=\"%s\"/>", currentuser.userid);
		printf("<input type=\"hidden\" name=\"gender\" value=\"%c\"/>", currentuser.gender);
		printf("<table>");
		printf("<tr>");
		printf("<td>");
		printf("您选择的号码:");
		printf("</td>");
		printf("<td>");

		if (detail.num == 0)
		{
			for (i = 0; i < MAXREGIST; i++)
				if (alldetail[i].id[0] == '\0')
					break;
			
			if (i == MAXREGIST)
			{
				printf("<body>");
				printf("很抱歉，目前号码已全部用完。<br/>");
				printf("[<a href=\"bbsregistview\">查看选号情况</a>]");
				printf("</body>");
				return 0;
			}
		}

		printf("<input name='number' maxlength='4' size='4' value='%d'/>", detail.num == 0? (i+1) : detail.num);

		printf("</td>");
		printf("</tr>");
		printf("<tr>");
		printf("<td>");
		printf("是否乘坐校车:");
		printf("</td>");
		printf("<td>");
		printf("<input type=radio name=\"bus\" value=\"0\"%s/>不乘坐校车<br/>", detail.bus == 0 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"1\"%s/>乘校车来<br/>", detail.bus == 1 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"2\"%s/>乘校车回<br/>", detail.bus == 2 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"3\"%s/>均乘坐校车<br/>", detail.bus == 3 ? " checked" : "");
		printf("</td>");
		printf("</tr>");
		printf("</table>");
		printf("<input type=\"button\" value='提交' onclick=\"verify();\"/>");
		printf("&nbsp;&nbsp;");
		printf("<input type=\"button\" value='取消' onclick=\"confirmback();\"/>");
		printf("</form>");
		printf("</body>");
		printf("<script language=\"javascript\">\n");
		printf("function verify()\n");
		printf("{");
		printf("var num = document.getElementsByName('number')[0].value;");
		printf(" if ( isNaN(num) ) { \n");
		printf("alert('号码必须为数字');return false;}\n");
		printf("else if(num <= 0 || num > 2500) { \n");
		printf("alert('号码必须在1-2500之间');return false;}\n");
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

