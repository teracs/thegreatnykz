#include "www.h"

int bbslogout_main()
{
    int pid;

    init_all();
    if (!loginok)
        http_fatal("你没有登录");
    if (u_info->mode == 20000)
        http_fatal("ICQ方式请用客户端注销");
    pid = u_info->pid;
    if (pid > 0)
    {
		if(is_web_user(u_info))
			kick_web_user(u_info);
		else
	        kill(pid, SIGHUP);
    }
    setcookie("utmpkey", "");
    setcookie("utmpnum", "");
    setcookie("utmpuserid", "");
    redirect("/");
}
