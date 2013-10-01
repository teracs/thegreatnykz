#include "www.h"

#define MAXREGIST (2500)

struct registdetail
{
	char id[IDLEN + 1];
	char email[REAL_NAME_EMAIL_LEN];
	int num;
	int gender;
	int bus;	// 0: 否; 1: 来, 2: 回; 3: both
};

struct registdetail alldetail[MAXREGIST];

int
cmpruid (userid, rd)
char *userid;
struct registdetail *rd;
{
    return !strncmp (userid, rd->id, IDLEN);
}

int
loadregistdetail()
{
	char filename[] = "vote/regist";
	int fd;
	int i, bufread;
	struct registdetail detail;
	if ((fd = open(filename, O_RDONLY, 0)) == -1)
	{
		g_message("vote/regist not exist, now will create it");
		fd = open(filename, O_WRONLY|O_CREAT, S_IRWXU);
		detail.bus = 0;
		bzero(detail.email, sizeof(detail.email));
		detail.gender = 0;
		bzero(detail.id, sizeof(detail.id));
		detail.num = 0;
		for(i = 0; i < MAXREGIST; i++)
		{
			write(fd, &detail, sizeof(struct registdetail));
		}
		close(fd);
	}
	if((fd = open(filename, O_RDONLY, 0)) == -1)
	{
		return -1;
	}
	for(i = 0; i < MAXREGIST; i++)
	{
		bufread = read(fd, &detail, sizeof(struct registdetail));
		if(bufread  != sizeof(struct registdetail))
			break;
		alldetail[i].num = detail.num;
		alldetail[i].bus = detail.bus;
		alldetail[i].gender = detail.gender;
		strcpy(alldetail[i].email, detail.email);
		strcpy(alldetail[i].id, detail.id);
	}
	close(fd);
	return 0;
}

int
bbsregistview_main()
{
	char filename[STRLEN];
	struct registdetail myregist;
	int i, j;
	int totalnum = 0, male = 0, female = 0, buscome = 0, busgo = 0;
	FILE *fp;
	char buf[256];

	//initialize
	myregist.num = 0;
	myregist.id[0] = '\0';
	sprintf(filename, "vote/regist");
	init_all();
	loadregistdetail();
	for(i = 0; i < MAXREGIST; i++)
	{
		if(!strcmp(alldetail[i].id, currentuser.userid))
		{
			myregist.bus = alldetail[i].bus;
			strcpy(myregist.email, alldetail[i].email);
			myregist.gender = alldetail[i].gender;
			strcpy(myregist.id, alldetail[i].id);
			myregist.num = alldetail[i].num;
		}
		if(alldetail[i].id[0] != '\0')
		{
			totalnum++;
			if(alldetail[i].bus == 1)
			{
				buscome++;
			}
			else if(alldetail[i].bus == 2)
			{
				busgo++;
			}
			else if(alldetail[i].bus == 3)
			{
				buscome++;
				busgo++;
			}
			if(alldetail[i].gender == 1)
			{
				male++;
			}
			else if(alldetail[i].gender == 2)
			{
				female++;
			}
		}
	}
	g_message("i = 0, id is %s, num is %d", alldetail[0].id, alldetail[0].num);
	g_message("i = 1, id is %s, num is %d", alldetail[1].id, alldetail[1].num);
	g_message("i = 2, id is %s, num is %d", alldetail[2].id, alldetail[2].num);
	printf("<body>\n");
	printf("<table align=center><tr><td><font class=title><b>站庆报名</b></font></td></tr></table><hr>\n");
	
	printf("<table align=center class=show_border width=610 border=1>\n");
	printf("<tr><td><pre>\n");
	fp = fopen("vote/regist_note", "r");
	if (fp != 0)
	{
		while (1)
		{
			if (fgets(buf, 256, fp) == 0)
				break;
			hhprintf("%s", void1(buf));
		}
		fclose(fp);
	}
	printf("</pre></td></tr>\n");
	printf("</table>\n");

	//type=1: 增加; 2: 修改
	printf("<center>\n");
	printf("<br/>[<a href=bbsregist?type=%d>%s</a>]\n",myregist.num == 0 ? 1 : 2, myregist.num == 0 ? "我要报名" : "修改报名");
	if(myregist.num > 0)
	{
		//type=3 means delete user's regist
		//printf("[<a href=bbsdoregist?type=3>取消报名</a>]");
		printf("<script language='javascript'>							\n");
		printf("<!--													\n");
		printf("	function cancelreg(){								\n");
		printf("		warning = confirm('确定取消报名吗？')			\n");
		printf("		if (warning != '0'){							\n");
		printf("			window.open('bbsdoregist?type=3', '_self')	\n");
		printf("		}												\n");
		printf("	}													\n");
		printf("-->														\n");
		printf("</script>												\n");
		printf("[<a href='javascript:cancelreg()'>取消报名</a>]			\n");

	}
	printf("[<a href=bbsdoc?board=juhui>去Juhui板</a>]\n");
	printf("</center><br><br>\n\n");

	if(totalnum > 0)
	{
		printf("<table align=center width=610>\n");
		printf("	<tr>\n<td width=150 align=center><font class=c32>当前报名人数: %d人</font></td>\n", totalnum);
		printf("	<td width=100 align=center><font class=c36>男性: %d人</font></td>\n", male);
		printf("	<td width=100 align=center><font class=c35>女性: %d人</font></td>\n", female);
		printf("	<td width=50 align=center>&nbsp;</td>\n");
		printf("	<td width=100 align=center>乘校车来: %d人</td>\n", buscome);
		printf("	<td width=100 align=center>乘校车回: %d人</td>\n</tr>\n", busgo);
		printf("</table>\n");

		printf("<table align=center class=show_border width=610 border=1>\n");
		printf("<tr>");
		for (i = 0; i< 3; i++)
			printf("<td width=100>报名ID</td><td width=40>号码</td>");
		printf("</tr>\n");
		for(i = 0, j = 0; i < MAXREGIST; i++)
		{
			if(alldetail[i].id[0] != '\0')
			{
				if(j % 3 == 0)
				{
					printf("<tr>\n");
				}
				printf("<td width=100><font class=c%s>%s</font></td>", alldetail[i].gender == 1 ? "36" : "35", alldetail[i].id);
				printf("<td width=40>%d</td>\n", alldetail[i].num);
				if(j % 3 == 2)
				{
					printf("</tr>\n");
				}
				j++;
			}
		}
		j %= 3;
		if (j != 0)
		{
			for(i = 0; i < 3 - j; i++)
			{
				printf("<td>&nbsp;</td><td>&nbsp;</td>");
			}
			printf("</tr>\n");
		}
		printf("</table>\n");
	}
	else
	{
		printf("<center>当前还没有用户报名</center><br/>");
	}

	printf("</body>\n");
	return 0;
}

