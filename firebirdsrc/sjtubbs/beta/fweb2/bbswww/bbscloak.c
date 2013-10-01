#include "www.h"

int
bbscloak_main()
{
	init_all();
	if (!loginok)
		http_quit_print("匆匆过客不能进行此操作, 请先登录");
	if (!(currentuser.userlevel & PERM_CLOAK))
		http_quit_print("错误的参数");
	if (u_info->invisible)
	{
		u_info->invisible = 0;
		printf("隐身状态已经停止了.");
	}
	else
	{
		u_info->invisible = 1;
		printf("隐身状态已经开始了.");
	}
	return 0;
}
