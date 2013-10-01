#include "www.h"

#define BAD_HOST                BBSHOME"/.bad_host"

int getcounts14()
{
    int fd, c = 0;
    FILE *fp;

    fd = open(".dellock", O_RDWR | O_CREAT | O_APPEND, 0644);
    flock(fd, LOCK_EX);
    if ((fp = fopen("counts.dat", "r")) == NULL)
    {
        c = 0;
    }
    else
    {
        fscanf(fp, "%d", &c);
        c++;
        fclose(fp);
    }
    fp = fopen("counts.dat", "w");
    fprintf(fp, "%d", c);
    fclose(fp);
    flock(fd, LOCK_UN);
    close(fd);
    return c;
}

int
web_bad_host (char *name)
{
    FILE *list;
    char buf[40], *ptr;
	
    if (list = fopen (BAD_HOST, "r"))
    {
        while (fgets (buf, 40, list))
        {
            ptr = strtok (buf, " \n\t\r");
            if (ptr != NULL && *ptr != '#')
            {
                if (!strcmp (ptr, name))
                    return 1;
                if (ptr[0] == '#')
                    continue;
                if (ptr[0] == '-' && !strcmp (name, &ptr[1]))
                    return 0;
                if (ptr[strlen (ptr) - 1] == '.'
					&& !strncmp (ptr, name, strlen (ptr) - 1))
                    return 1;
                if (ptr[0] == '.' && strlen (ptr) < strlen (name) &&
					!strcmp (ptr, name + strlen (name) - strlen (ptr)))
                    return 1;
            }
        }
        fclose (list);
    }
    return 0;
}

int bbslogin_main()
{
    int n, t;
    char buf[256], id[20], pw[20];
    struct userec *x;

    init_all();
    getcounts14();
    strsncpy(id, getparm("id"), 13);
    strsncpy(pw, getparm("pw"), 13);
/*
    if (loginok && strcasecmp(id, currentuser.userid))
    {
        http_fatal
        ("系统检测到目前你的计算机上已经登录有一个帐号 %s, 请先退出, 或者 <a href=/>使用 %s 帐号进入</a>.",
         currentuser.userid, currentuser.userid);
    }
*/
    x = getuser(id);
    if (x == 0)
        http_fatal("错误的使用者帐号");
    if (strcasecmp(id, "guest"))
    {
        if (!checkpasswd(x->passwd, pw))
        {
            char path2[80];

            if (pw[0] != 0)
            {
                sleep(2);
                sprintf(buf, "%s %s %s WWW\n", Ctime(time(0)) + 4, id,
                        fromhost);
                f_append("badlogin.www", buf);
                sethomefile(path2, x->userid, "logins.bad");
                f_append(path2, buf);
            }
            http_fatal("密码错误");
        }
        if (x->passwd[0] < ' ' || x->passwd[0] > 'z' || x->passwd[1] < ' '
                || x->passwd[1] > 'z')
            strsncpy(x->passwd, crypt_passwd(pw), 14);
        check_multi(x->userid);
        if (!user_perm(x, PERM_BASIC))
            http_fatal
            ("此帐号已被停机, 若有疑问, 请用其他帐号在sysop板询问.");
		//	debug by rwzmm @ sjtubbs, ban bad host
		//  if (file_has_word(".bad_host", fromhost))
		if(web_bad_host(fromhost))
        {
            http_fatal("对不起, 本站不欢迎来自 [%s] 的登录. <br>",
                       fromhost);
        }
        t = x->lastlogin;
        x->lastlogin = time(0);
        save_user_data(x);
        if (abs(t - time(0)) < 5)
            http_fatal("两次登录间隔过密!");
        check_login_limit(x);
        x->numlogins++;
        strsncpy(x->lasthost, fromhost, 17);
        save_user_data(x);
        currentuser = *x;
    }
	else
	{
		if(check_multi_guest(fromhost))
			http_fatal("web匿名用户数目超限制,请使用注册id登陆");
		x->lastlogin = time(0);
		x->numlogins++;
		strsncpy(x->lasthost,fromhost,17);
		save_user_data(x);
		currentuser = *x;	
	}
    //      else redirect("bbsmain");
    sprintf(buf, "%s %s %s\n", Ctime(time(0)), x->userid, fromhost);
    f_append("www.log", buf);
    //      sprintf(buf, "%s ENTER %-12s @%s [www]\n", Ctime(time(0))+4, x->userid, fromhost);
    sprintf(buf, "%s@%s", "www", fromhost);
    log_usies("ENTER", buf);
    //      f_append("usies", buf);
    //n = 0;
    if (!loginok || !strcasecmp("guest",currentuser.userid))
        wwwlogin(x);
    redirect("/frame");
}

int wwwlogin(struct userec *user)
{
    FILE *fp;
    char buf[80];
    int pid, n, tmp;
    struct user_info *u;

    fp = fopen(".UTMP.lock", "a");
    flock(fileno(fp), LOCK_EX);
    for (n = 0; n < MAXACTIVE; n++)
    {
        if (shm_utmp->uinfo[n].active == 0)
        {
            u = &(shm_utmp->uinfo[n]);
            u_info = u;
			/* deleted for httpsession watcher by jawahh
            pid = fork();
            if (pid < 0)
                http_fatal("can't fork");
            if (pid == 0)
            {
                wwwagent();
                exit(0);
            }*/

            if(shm_utmp->sess.magic != SESS_MAGIC_NUMBER || 0 == shm_utmp->sess.pid)
	    {
		http_fatal("系统尚未完成初始化(session watcher)");
	    }
            bzero(u, sizeof(struct user_info));
            u->active = 1;
            u->uid = getusernum(user->userid) + 1;
            u->pid = shm_utmp->sess.pid;
            u->mode = 10001;
            if (user_perm(&currentuser, PERM_LOGINCLOAK) &&
                    (currentuser.flags[0] & CLOAK_FLAG))
                u->invisible = YEA;
            u->pager = 0;
            if (currentuser.userdefine & DEF_FRIENDCALL)
                u->pager |= FRIEND_PAGER;
            if (currentuser.flags[0] & PAGER_FLAG)
            {
                u->pager |= ALL_PAGER;
                u->pager |= FRIEND_PAGER;
            }
            if (currentuser.userdefine & DEF_FRIENDMSG)
                u->pager |= FRIENDMSG_PAGER;
            if (currentuser.userdefine & DEF_ALLMSG)
            {
                u->pager |= ALLMSG_PAGER;
                u->pager |= FRIENDMSG_PAGER;
            }
            if (!(currentuser.userlevel & PERM_LOGINOK))
            {
                char file[256];

                sprintf(file, "home/%c/%s/register",
                        toupper(currentuser.userid[0]),
                        currentuser.userid);
                if (file_exist(file))
                {
                    currentuser.userlevel |= PERM_DEFAULT;
                    save_user_data(&currentuser);
                    printf
                    ("<script>alert('你己通过身份认证!')</script>\n");
                }
            }
            strsncpy(u->from, fromhost, 24);
            *(int *) (u->from + 32) = time(0);
            u->from[40] = currentuser.termtype[0];
            u->idle_time = time(0);
            strsncpy(u->username, user->username, 20);
            strsncpy(u->userid, user->userid, 13);
            tmp = rand() % 100000000;
            u->utmpkey = tmp;
            sprintf(buf, "%d", n);
            setcookie("utmpnum", buf);
            sprintf(buf, "%d", tmp);
            setcookie("utmpkey", buf);
            setcookie("utmpuserid", currentuser.userid);
            flock(fileno(fp), LOCK_UN);
            fclose(fp);
            return 0;
        }
    }
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    http_fatal("抱歉，目前在线用户数已达上限，无法登录。请稍后再来。");
}
/****** not used ****
void add_msg()
{
    int i;
    FILE *fp;
    char buf[1048], file[256], *id = currentuser.userid;
    //	char bufwww[256];
    //	struct msgdir md;

    memset(buf, 0x20, 1048);
    buf[1047] = 0x00;

    //      if(!loginok) return;
    //sprintf(file, "touch home/%c/%s/wwwmsg.flush", toupper(id[0]), id);
    //system(file);

    //modified by rwzmm @ sjtubbs
    //read msg info from .msgdir instead of msgfile

    //	sprintf(file, "home/%c/%s/msgfile", toupper(id[0]), id);
    //    i = file_size(file) / 129;
    //    if (get_record(&buf, 129, i - 1, file) <= 0)
    //        return;
    //    sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
    //    fp = fopen(file, "a");
    //    fwrite(buf, 129, 1, fp);
    //    fclose(fp);
    sprintf(file, "home/%c/%s/.msgdir", toupper(id[0]), id);
    i = file_size(file) / sizeof(struct msgdir);
    if (get_record(&buf, sizeof(struct msgdir), i - 1, file) <= 0)
        return;
    for(i = 0; i<1048; i ++)
    {
        if(buf[i] == 0x00)
            buf[i] = 0x20;
    }
    sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
    fp = fopen(file, "a");
    fwrite(buf, sizeof(struct msgdir), 1, fp);
    fclose(fp);


    //for rwzmm debug only
    //	sprintf(file, "/home/rwzmm/msgbak");
    //	fp = fopen(file, "a");
    //	fwrite(buf, sizeof(struct msgdir), 1, fp);
    //	fclose(fp);


}

void abort_program()
{
    int stay = 0;
    struct userec *x;

	if (!strcmp(u_info->userid, currentuser.userid))
    {
        stay = abs(time(0) - *(int *) (u_info->from + 32));
        bzero(u_info, sizeof(struct user_info));
    }
    if (stay > 3600 * 24 * 3)
        stay = 3600 * 24 * 3;
    if (stay < 0)
        stay = 0;
    x = getuser(currentuser.userid);
    if (x)
    {
        x->stay += stay;
        x->lastlogout = time(0);
        save_user_data(x);
    }
    exit(0);
}

int wwwagent()
{
    int i;
    for (i = 0; i < 1024; i++)
        close(i);
    for (i = 0; i < NSIG; i++)
        signal(i, SIG_IGN);

    //signal(SIGUSR2, add_msg);
    signal(SIGHUP, abort_program);
    nice(3);
    while (1)
    {
        sleep(600);
        if (abs(time(0) - u_info->idle_time) > 60 * 30)
        {
            FILE *fp;

            fp = fopen("err", "a");
            fprintf(fp, "%s %s timeout\n", Ctime(time(0)),
                    currentuser.userid);
            fclose(fp);
            abort_program();
        }
    }
    exit(0);
}
********/

int check_multi(char *id)
{
    int i,j, total = 0, pid = 0;

    if (currentuser.userlevel & PERM_SYSOP)
        return;
    for (i = 0; i < MAXACTIVE; i++)
    {
        if (shm_utmp->uinfo[i].active == 0)
            continue;
        if (!strcasecmp(shm_utmp->uinfo[i].userid, id))
        {
            pid = shm_utmp->uinfo[i].pid;
			j=i;
            total++;
        }
    }
    if (total >= 3 && pid > 0)
    {
		if(is_web_user( &(shm_utmp->uinfo[j]) ))
			kick_web_user( &(shm_utmp->uinfo[j]));
		else
	        kill(pid, SIGHUP);
    }
    //http_fatal("您已经登录了3个窗口。为了保证他人利益，此次连线将被取消。");
}
int check_multi_guest(char *fromhost)
{
	int i,n=0;
	for(i = 0;i<MAXACTIVE;i++)
	{
		if(shm_utmp->uinfo[i].active == 0)
			continue;
		if(!strcasecmp("guest",shm_utmp->uinfo[i].userid) 
			&& !strcmp(shm_utmp->uinfo[i].from,fromhost))
		{
			++n;
			if(n>100)
				return 1;
		}	
	}
	return 0;
}
int check_login_limit(struct userec *x)
{
    FILE *fp;
    char filename[100];
    int num = 0;

    if (x->userlevel & PERM_SYSOP)
        return;
    sethomefile(filename, x->userid, "logincheck");
    if (!file_exist(filename)
            || abs(file_time(filename) - time(0)) > 86400)
    {
        fp = fopen(filename, "w");
        fprintf(fp, "%d", x->numlogins);
        fclose(fp);
        return;
    }
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("内部错误");
    fscanf(fp, "%d", &num);
    fclose(fp);
    if (abs(num - x->numlogins) > 100)
    {
        char buf[256];

        sprintf(buf, "%24.24s %s\n", Ctime(time(0)), x->userid);
        f_append("login.log", buf);
        http_fatal
        ("此帐号本日login次数过多, 请过几个小时再试, 请勿使用上站机.\n");
    }
}
