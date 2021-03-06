#include "www.h"

char *defines[] = {
	"呼叫器关闭时可让好友呼叫",	/* DEF_FRIENDCALL */
	"接受所有人的讯息",	/* DEF_ALLMSG */
	"接受好友的讯息",	/* DEF_FRIENDMSG */
	"收到讯息发出声音",	/* DEF_SOUNDMSG */
	"使用彩色",		/* DEF_COLOR */
	"显示活动看板",		/* DEF_ACBOARD */
	"显示选单的讯息栏",	/* DEF_ENDLINE */
	"编辑时显示状态栏",	/* DEF_EDITMSG */
	"讯息栏采用一般/精简模式",	/* DEF_NOTMSGFRIEND */
	"选单采用一般/精简模式",	/* DEF_NORMALSCR */
	"分类讨论区以 New 显示",	/* DEF_NEWPOST */
	"阅读文章是否使用绕卷选择",	/* DEF_CIRCLE */
	"阅读文章游标停於第一篇未读",	/* DEF_FIRSTNEW */
	"进站时显示好友名单",	/* DEF_LOGFRIEND */
	"好友上站通知",		/* DEF_LOGINFROM */
	"观看留言板",		/* DEF_NOTEPAD */
	"不要送出上站通知给好友",	/* DEF_NOLOGINSEND */
	"主题式看板",		/* DEF_THESIS */
	"收到讯息等候回应或清除",	/* DEF_MSGGETKEY */
	"进站时观看上站人次图",	/* DEF_GRAPH */
	"进站时观看十大排行板",	/* DEF_TOP10 */
	"使用乱数签名档",	/* DEF_RANDSIGN */
	"显示星座",		/* DEF_S_HOROSCOPE */
	"星座使用颜色来显示性别",	/* DEF_COLOREDSEX */
	"缺省转寄信箱为站内信箱",	/* DEF_FMAIL */
	"使用\'+\'标记未读文章",	/* DEF_NOT_N_MASK */
	"汉字整字删除",		/* DEF_DELDBLCHAR */
	"自动排板宽度预设为 78 列",	/* DEF_AUTOWRAP */
	"使用GB码阅读",		/* DEF_USEGB KCN 99.09.03 */
	NULL
};
static int
read_form()
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
	printf("个人参数设置成功.<br><a href=bbsparm>返回个人参数设置选单</a>");
	return 0;
}
int
bbsparm_main()
{
	int i, perm = 1, type;

	init_all();
	type = atoi(getparm("type"));
	printf("<center><font class=\"title\">%s - 修改个人参数　　</font>[使用者: %s]<hr>\n", BBSNAME, currentuser.userid);
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
	printf("<input type=submit value=确定></form>以上参数大多仅在telnet方式下才有作用\n");
	printf("</html>");
	return 0;
}


