#include <stdlib.h>
#include "nt.h"

extern int ntp_bbslogin(void);
extern int ntp_bbslogout(void);


static gboolean www_login_user(struct userec *user)
{
	char buf[256];
	struct user_info *u;
	int n;

	n = nt_new_uinfo(user->userid, &u);
	if(n<0)	return FALSE;

	u->mode = 10001;
	if (HAS_PERM(user, PERM_LOGINCLOAK) &&
	    (user->flags[0] & CLOAK_FLAG))
		u->invisible = YEA;
	u->pager = 0;
	if (user->userdefine & DEF_FRIENDCALL)
		u->pager |= FRIEND_PAGER;
	if (user->flags[0] & PAGER_FLAG)
	{
		u->pager |= ALL_PAGER;
		u->pager |= FRIEND_PAGER;
	}
	if (user->userdefine & DEF_FRIENDMSG)
		u->pager |= FRIENDMSG_PAGER;
	if (user->userdefine & DEF_ALLMSG)
	{
		u->pager |= ALLMSG_PAGER;
		u->pager |= FRIENDMSG_PAGER;
	}
	if (!HAS_PERM(user,PERM_LOGINOK))
	{

		nt_sethomefile(buf,sizeof(buf),user->userid,"register");
		if (g_file_test(buf,G_FILE_TEST_IS_REGULAR))
		{
			SET_PERM(user,PERM_DEFAULT);
			nt_set_userec_with_uid(user, u->uid);
		}
	}
	g_strlcpy(u->from, cgiRemoteAddr, 60);
	u->logintime = ntw_now;
	u->idle_time = ntw_now;
	g_strlcpy(u->username, user->username, 20);
	u->utmpkey  = rand();
	cgiHeaderCookieSetInteger(COOKIE_UTMPNUM, n, 0, "/", NULL);
	cgiHeaderCookieSetInteger(COOKIE_UTMPKEY, u->utmpkey, 0, "/", NULL);
	cgiHeaderCookieSetString(COOKIE_UTMPUSERID, u->userid, 0, "/", NULL);
	g_message("user %s@%s login from www",u->userid, cgiRemoteAddr);
	return TRUE;
}


static void www_check_multi(struct userec *puser)
{
	
	struct user_info *puinfos[USER_LOGIN_LIMIT_NUM];
	int total;

	if (HAS_PERM_SYSOP(puser))		return;
	
	total = nt_get_active_uinfo(puser->userid, puinfos, USER_LOGIN_LIMIT_NUM);
	if (total == USER_LOGIN_LIMIT_NUM)
	{
		ntw_logout_user(puinfos[0]);
	}
	return;

}

static gboolean www_check_multi_guest(char *fromhost)
{
	int i, n = 0;
	for (i = 0; i < MAXACTIVE; i++)
	{
		if (shm_utmp->uinfo[i].active == 0)
			continue;
		if (!strcasecmp("guest", shm_utmp->uinfo[i].userid)
		    && !strcmp(shm_utmp->uinfo[i].from, fromhost))
		{
			++n;
			if (n > 100)
				return FALSE;
		}
	}
	return TRUE;
}


int ntp_bbslogin(void)
{
	int  t;
	char buf[256], id[20], pw[20];
	struct userec user;
	char *errmsg;


	nt_inc_login_count();

	cgiFormString("id", id, 20);
	cgiFormString("pw", pw, 20);


    if (ntw_haslogin )
    {
		if(strcasecmp(id, ntw_curuser.userid)) 
		{
			/*注销已经登录的用户*/
			ntw_logout_user(ntw_puinfo);
		}
		else
	    {
			/*相同用户已经在本机登录*/
	        cgiHeaderLocation("/frame2.html");
			return 0;
	    }
    }

	if(!nt_get_userec(id, &user))
	{
		errmsg = "无此用户";
		goto ERROR;
	}

	if (strcasecmp(id, "guest"))
	{
		if (!nt_check_passwd(user.passwd, pw))
		{
			char path2[80];

			if (pw[0] != '\0')
			{
				
				sprintf(buf, "%s %s %s WWW\n",
					nt_ctime(ntw_now) + 4, id, cgiRemoteAddr);
				nt_file_append("badlogin.www", buf);
				nt_sethomefile(path2,80,user.userid, "logins.bad");
				nt_file_append(path2, buf);
				nt_sleep(2);
			}
			errmsg = "密码错误";
			goto ERROR;
		}		

		if (!HAS_PERM_LOGIN(&user))
		{
			errmsg = "此帐号已被停机, 若有疑问, 请用其他帐号在sysop板询问.";
			goto ERROR;
		}
		
		if (nt_is_bad_host(cgiRemoteAddr))
		{
			errmsg = "对不起, 本站不欢迎来自此IP的登录";
			goto ERROR;
		}
		
		t = user.lastlogin;
		user.lastlogin = ntw_now;

		if (abs(t - user.lastlogin) < 5)
		{
			errmsg = "两次登录间隔过密!";
			goto ERROR;
		}
		
		if( !nt_check_login_limit(&user))
		{
			errmsg = "登录次数过多，请勿使用上站机";
			goto ERROR;
		}
		www_check_multi(&user);
		if(ntw_haslogin) ntw_logout_user(ntw_puinfo);
		user.numlogins++;
		g_strlcpy(user.lasthost, cgiRemoteAddr, 60);
		if (user.passwd[0] < ' ' || user.passwd[0] > 'z'
		    || user.passwd[1] < ' ' || user.passwd[1] > 'z')
		{
			g_strlcpy(user.passwd, nt_crypt_passwd(pw), 14);
		}
		nt_set_userec(&user);
	}
	else
	{
		if (!www_check_multi_guest(cgiRemoteAddr))
		{
			errmsg = "web匿名用户数目超限制,请使用注册id登陆";
			goto ERROR;
		}
		user.lastlogin = ntw_now;
		user.numlogins++;
		g_strlcpy(user.lasthost, cgiRemoteAddr, 60);
		nt_set_userec(&user);
	}
	

	if(!www_login_user(&user))
	{
		errmsg = "已经达到系统登录上限，或者系统繁忙，请稍候再试";
		goto ERROR;
	}
	
	if (HAS_PERM_REALNAME(&user))
		cgiHeaderLocation("/frame2.html");
	else
		cgiHeaderLocation("/bbsrlmail");
		
	return 0;
	
ERROR:
	ntw_normal_headers();
	tpl_print_err(errmsg);
	return 0;
}

int ntp_bbslogout(void)
{

	if (ntw_haslogin)
	{
		ntw_logout_user(ntw_puinfo);
		cgiHeaderCookieSetString(COOKIE_UTMPKEY, "", -1000000000, "/", NULL);
		cgiHeaderCookieSetString(COOKIE_UTMPNUM, "", -1000000000, "/", NULL);
		cgiHeaderCookieSetString(COOKIE_UTMPUSERID, "", -1000000000, "/", NULL);
	}
	cgiHeaderLocation("/");
	return 0;
}

