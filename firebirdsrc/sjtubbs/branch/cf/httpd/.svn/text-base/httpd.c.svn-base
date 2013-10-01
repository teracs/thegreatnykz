/*
*  httpd.c [HTTP server]
*
*  Lily BBS v1.0
*
*  Copyright (C) 2002 zhch.bbs@bbs.nju.edu.cn, 南大百合科技
*/

#include "www.h"

int has_http_head;

/**
 * used for cgi_head(), plz set before call init_all()
 * if cache_age >0, will print http header "Cache-control: public,max-age=cache_age"
 * if cache_age == HEADER_NO_STORE, will print http header "Cache-control: no-store"
 * if cache_age == HEADER_NO_CACHE, will print http header "Cache-control: no-cache"
 * if cache_age == HEADER_PRIVATE, will print http header "Cache-control: private"
 **/
int cache_age = HEADER_NO_STORE;

/** 
 * default http header Last-Modified is time(0)
 * if u want to send a different one, plz set this before call init_all()
 **/
time_t last_modified_time = 0;

int main(int n, char *cmd[])
{
    int httpd_main();
    int i;
    int port = 80;
	int is_local = 0;

	if(3 == n)
	{
		/* cmdline should like "sjtuwebbsd --port 8888" */
        if (!strcmp(cmd[1], "--port") )
        {
            port = atoi(cmd[2]);
			if(port<=0 || port>65523)
				goto ERROR;
        }
        else
			goto ERROR;
	}
	else if(2 == n)
	{
		/* cmdline should like "sjtuwebbsd --localhost", means listening on 127.0.0.1 */
		if(!strcmp(cmd[1],"--localhost"))
			is_local = 1;
		else
			goto ERROR;
	}
	else if(4 == n)
	{
		/* cmdline should like "sjtuwebbsd --port 8888 --localhost" */
        if (!strcmp(cmd[1], "--port") )
        {
            port = atoi(cmd[2]);
			if(port<=0 || port>65523)
				goto ERROR;
        }
        else
			goto ERROR;
		if(!strcmp(cmd[3],"--localhost"))
			is_local = 1;
		else
			goto ERROR;		
	}
	else
		goto ERROR;
    tcp_bind(is_local,port, (void *) httpd_main, BBSUID);
	exit(0);
	ERROR:
	    printf("usage: sjtuwebbsd [--port port][--localhost]\n");
        exit(1);
}

int do_log(char *fmt, ...)
{
    FILE *fp;
    char buf[512];

    FORMAT(fmt, buf, 500);
    fp = fopen(LOG_FILE, "a");
    if (fp)
    {
        fprintf(fp, "%20.20s %-16.16s %s", Ctime(time(0)),
                getsenv("REMOTE_ADDR"), buf);
        fclose(fp);
    }
}

void timeout()
{
    exit(0);
}

int httpd_main()
{
    char buf[512] = "";
    char file[512] = "";
    char qstring[512] = "";
    char method[512] = "";
    char url[512] = "";
    char proto[512] = "";
    char *p;
    int n;
    int ret;
    void timeout();

    chdir(BBSHOME);
    signal(SIGALRM, timeout);
    alarm(1000);
    has_http_head = 0;
    buf[0] = 0;
    file[0] = 0;
    qstring[0] = 0;
    method[0] = 0;
    url[0] = 0;
    proto[0] = 0;
    if (fgets(buf, 512, stdin) == 0)
        exit(0);
    do_log("(%d)%s", getpid(), buf);
    n = sscanf(buf, "%s %s %s", method, url, proto);
    if (n <= 1)
    {
        printf("Error head\n");
        exit(0);
    }
    if (!strcmp(method, "GET"))
    {
        setenv("REQUEST_METHOD", "GET", 1);
    }
    else if (!strcmp(method, "POST"))
    {
        setenv("REQUEST_METHOD", "POST", 1);
    }
    else
    {
        printf("Error head\n");
        exit(0);
    }
    if (url[0] != '/')
    {
        printf("Error URL\n");
        exit(0);
    }
    if (!strncmp(url, "/vd", 3))
    {
        char buf1[512], *p1;

        strsncpy(buf1, url, 500);
        p1 = strchr(buf1 + 4, '/');
        if (p1)
            strsncpy(url, p1, 500);
    }
    strsncpy(file, url, 512);
    p = strchr(file, '?');
    if (p)
    {
        strsncpy(qstring, p + 1, 500);
        p[0] = 0;
    }
    setenv("QUERY_STRING", qstring, 1);
    setenv("REQUEST_URI", url, 1);
    setenv("SCRIPT_NAME", file, 1);
    if (proto[0])
    {
        has_http_head = 1;
        while (1)
        {
            if (fgets(buf, 256, stdin) == 0)
                break;
            buf[256] = 0;
            if (!strncasecmp(buf, "cookie: ", 8))
            {
                int i;

                for (i = 0; buf[i]; i++)
                    if (buf[i] < 32 && buf[i] > 0)
                        buf[i] = 0;
                setenv("HTTP_COOKIE", buf + 8, 1);
            }
            if (!strncasecmp(buf, "Content-Length: ", 15))
                setenv("CONTENT_LENGTH", buf + 16, 1);
            if (!strncasecmp(buf, "X-FORWARDED-FOR: ", 16))
            {
                char buf3[256] = "";

                sscanf(buf + 16, "%s", buf3);
                if (buf3[0])
                    setenv("HTTP_X_FORWARDED_FOR", buf3, 1);
            }
            if (buf[0] == 10 || buf[0] == 13)
                break;
        }
        ret = http_deal(file);
    }
    else
    {
        http_deal(file);
        exit(0);
    }
}
/*
int catf(char *file)
{
    FILE *fp;
    char buf[1024];
    int ret;

    fp = fopen(file, "r");
    if (!fp)
    {
        printf("\nFile does not exist.");
        exit(0);
    }
    if (has_http_head)
    {
        printf("Last-Modified: %s\n", Gtime(file_time(file)));
        printf("Accept-Ranges: bytes\n");
        printf("Content-Length: %d\n", file_size(file));
        printf("Connection: %s\n", "close");
        printf("Content-type: text/html\n\n");
    }
    while (1)
    {
        ret = fread(buf, 1, 1024, fp);
        if (ret <= 0)
            break;
        fwrite(buf, 1, ret, stdout);
    }
    fclose(fp);
    fflush(stdout);
    return;
}
*/
int http_deal(char *file)
{
    char tmp[2560];

    if (has_http_head)
    {
        printf("HTTP/1.1 200 OK\n");
        printf("Date: %s\n", Gtime(time(0)));
        printf("Server: Sjtuwebbsd/0.96 \n");
    }
    if (!strcmp(file, "/bbs0an")) /*精华区浏览,列出目录内容*/
        return bbs0an_main(); 
    if (!strcmp(file, "/bbsafind")) /*精华区搜索，没有使用*/
        return bbsafind_main();
    if (!strcmp(file, "/bbsall")) /*列出所有板面*/
        return bbsall_main();
    if (!strcmp(file, "/bbsallf")) /*旧上载全部区文件列表*/
        return bbsallf_main();
    if (!strcmp(file, "/bbsalluser")) /*列出bbs用户*/
        return bbsalluser_main();
    if (!strcmp(file, "/bbsanc")) /*精华区浏览，列出文章内容*/
        return bbsanc_main();
    if (!strcmp(file, "/bbsbfind")) /*板面文章搜索*/
        return bbsbfind_main();
    if (!strcmp(file, "/bbsblockmail")) /*邮件过滤，没有使用*/
        return bbsblockmail_main();
    if (!strcmp(file, "/bbsboa")) /*列出各区版板面*/
        return bbsboa_main();
    if (!strcmp(file, "/bbsbrdadd")) /*增加收藏版面*/
        return bbsbrdadd_main();
    if (!strcmp(file, "/bbsbrddel"))/*删除收藏版面*/
        return bbsbrddel_main();
    if (!strcmp(file, "/bbsccc"))/*转载文章*/
        return bbsccc_main();
    if (!strcmp(file, "/bbsclear"))/*清除未读标记*/
        return bbsclear_main();
    if (!strcmp(file, "/bbscloak"))/*隐身*/
        return bbscloak_main();
    if (!strcmp(file, "/bbscon"))/*阅读帖子*/
        return bbscon_main(); 
    if (!strcmp(file, "/bbstopcon"))/*阅读置底*/
        return bbstopcon_main();  
    if (!strcmp(file, "/bbsdel"))/*删除帖子*/
        return bbsdel_main(); 
	if (!strcmp(file,"/bbsfexp2"))/*新上载区文件详细资料*/
		return bbsfexp2_main();
	if (!strcmp(file,"/bbsfdoc2"))/*新上载区文件列表*/
		return bbsfdoc2_main();
	if (!strcmp(file,"/bbsudel2"))/*新上载区单个文件删除*/
		return bbsudel2_main();
	if (!strcmp(file,"/bbsfddoc"))/*新上载区管理模式*/
		return bbsfddoc_main();
	if (!strcmp(file,"/bbsurdel"))/*新上载多文件删除*/
		return bbsurdel_main();
    if (!strcmp(file, "/bbsdocdel"))/*回收站帖子阅读*/
        return bbsdocdel_main(); 
    if (!strcmp(file, "/bbscondel"))/*回收站文章列表*/
        return bbscondel_main();
    if (!strcmp(file, "/bbsdelreturn"))/*回收站文章恢复*/
        return bbsdelreturn_main(); 
    if (!strcmp(file, "/bbsdocnore")) /*设置帖子可re属性*/
        return bbsdocnore_main();
    if (!strcmp(file, "/bbsdelmail")) /*删除邮件*/
        return bbsdelmail_main();
    if (!strcmp(file, "/bbsdelmsg")) /*删除消息*/
        return bbsdelmsg_main();
    if (!strcmp(file, "/bbsdenyadd"))/*版面封人*/
        return bbsdenyadd_main();
    if (!strcmp(file, "/bbsdenyall"))/*被封名单列表*/
        return bbsdenyall_main();
    if (!strcmp(file, "/bbsdenydel"))/*版面解封*/
        return bbsdenydel_main();
    if (!strcmp(file, "/bbsdoc"))/*板面帖子列表*/
        return bbsdoc_main();
    if (!strcmp(file, "/bbsdoreg"))/*注册*/
        return bbsdoreg_main();
    if (!strcmp(file, "/bbsdoupload"))/*上传文件*/
        return bbsdoupload_main();
    if (!strcmp(file, "/bbsedit"))/*修改帖子*/
        return bbsedit_main();
    if (!strcmp(file, "/bbsfadd"))/*增加好友*/
        return bbsfadd_main();
    if (!strcmp(file, "/bbsfall"))/*列出好友*/
        return bbsfall_main();
    if (!strcmp(file, "/bbsfdel"))/*删除好友*/
        return bbsfdel_main();
    if (!strcmp(file, "/bbsfdoc"))/*旧上载区板面文件列表*/
        return bbsfdoc_main(); 
    if (!strcmp(file, "/bbsfexp"))/*旧上载区文件详细信息*/
        return bbsfexp_main();
    if (!strcmp(file, "/bbsfind"))/*全站文章搜索*/
        return bbsfind_main();
/*    if (!strcmp(file, "/bbsfind2")) 
        return bbsfind2_main(); */ /*全站旧上载区文件搜索，废弃不用 */
    if (!strcmp(file, "/bbsfoot")) /*frame中的foot，显示在线时间，邮件等等*/
        return bbsfoot_main();
    if (!strcmp(file, "/bbsform")) /*注册单填写*/
        return bbsform_main();
    if (!strcmp(file, "/bbsfriend")) /*在线好友*/
        return bbsfriend_main();
    if (!strcmp(file, "/bbsfwd")) /*帖子转寄*/
        return bbsfwd_main();
    if (!strcmp(file, "/bbsgcon"))/*文摘区浏览*/
        return bbsgcon_main();
    if (!strcmp(file, "/bbsgdoc"))/*文摘区帖子浏览*/
        return bbsgdoc_main();
    if (!strcmp(file, "/bbsgetmsg"))
        return bbsgetmsg_main();/* 接受消息*/
    if (!strcmp(file, "/bbsinfo"))/*个人资料*/
        return bbsinfo_main();
    if (!strcmp(file, "/bbsleft"))/*frame中的左栏*/
        return bbsleft_main();
    if (!strcmp(file, "/bbslogin"))/*登入*/
        return bbslogin_main();
    if (!strcmp(file, "/bbslogout"))/*登出*/
        return bbslogout_main();
    if (!strcmp(file, "/bbsmail"))/*邮件列表*/
        return bbsmail_main();
    if (!strcmp(file, "/bbsmailcon"))/*邮件阅读*/
        return bbsmailcon_main();
    if (!strcmp(file, "/bbsmailmsg"))/*信息备份*/
        return bbsmailmsg_main();
    if (!strcmp(file, "/bbsman"))/*板面管理*/
        return bbsman_main();
    if (!strcmp(file, "/bbsman2"))/*邮件删除*/
        return bbsman2_main();
    if (!strcmp(file, "/bbsmdoc"))/*版面管理界面*/
        return bbsmdoc_main();
    if (!strcmp(file, "/bbsmnote"))/*进板画面*/
        return bbsmnote_main();
    if (!strcmp(file, "/bbsmnote3"))/*板主寄语*/
        return bbsmnote3_main();
    if (!strcmp(file, "/bbsmovie"))/*活动看板,未使用*/
        return bbsmovie_main();
    if (!strcmp(file, "/bbsmsg")) /*察看信息*/
        return bbsmsg_main();
    if (!strcmp(file, "/bbsmybrd"))/*设定收藏夹*/
        return bbsmybrd_main();
    if (!strcmp(file, "/bbsmywww"))/*WWW个人定制 */
        return bbsmywww_main();
    if (!strcmp(file, "/bbsnewmail"))/*编写新邮件*/
        return bbsnewmail_main();
    if (!strcmp(file, "/bbsnick"))/*更改nick*/
        return bbsnick_main();
    if (!strcmp(file, "/bbsnot"))/*进板画面*/
        return bbsnot_main();
    if (!strcmp(file, "/bbsnotepad"))/*留言版*/
        return bbsnotepad_main();
    if (!strcmp(file, "/bbsparm"))/*设定个人参数*/
        return bbsparm_main();
    if (!strcmp(file, "/bbsplan"))/*设定个人qmd*/
        return bbsplan_main();
    if (!strcmp(file, "/bbspst"))/*发表新文章*/
        return bbspst_main();
    if (!strcmp(file, "/bbspstmail"))/*发送邮件*/
        return bbspstmail_main();
    if (!strcmp(file, "/bbspwd"))/*修改密码*/
        return bbspwd_main();
    if (!strcmp(file, "/bbsqry"))/*查询用户*/
        return bbsqry_main();
    if (!strcmp(file, "/bbsreg"))/*注册表单*/
        return bbsreg_main();
    if (!strcmp(file, "/bbssec"))/*区列表*/
        return bbssec_main();
    if (!strcmp(file, "/bbssel"))/*选择版面*/
        return bbssel_main();
    if (!strcmp(file, "/bbssendmsg"))/*发送信息*/
        return bbssendmsg_main();
    if (!strcmp(file, "/bbssig"))/*设置qmd*/
        return bbssig_main();
    if (!strcmp(file, "/bbssnd"))/*发表帖子*/
        return bbssnd_main();
    if (!strcmp(file, "/bbssndmail"))/*发送信件*/
        return bbssndmail_main();
    if (!strcmp(file, "/bbsstat"))/*个人排名信息统计*/
        return bbsstat_main();
    if (!strcmp(file, "/bbstcon"))/*同主题阅读*/
        return bbstcon_main();
    if (!strcmp(file, "/bbstdoc"))/*同主题列表*/
        return bbstdoc_main();/* non-cachable wait for modifying */
    if (!strcmp(file, "/bbstfind"))/*同主题阅读,根据board和title*/
        return bbstfind_main();
    if (!strcmp(file, "/bbstfind0"))/*同主题列表*/
        return bbstfind0_main();
    if (!strcmp(file, "/bbstop10"))/*十大话题*/
        return bbstop10_main();
    if (!strcmp(file, "/bbstopb10"))/*十大版面*/
        return bbstopb10_main();
    if (!strcmp(file, "/bbsudel"))/*旧上载区文件删除*/
        return bbsudel_main();
    if (!strcmp(file, "/bbsufind"))/*寻找特定字母开头的在线用户*/
        return bbsufind_main();
    if (!strcmp(file, "/bbsupload"))/*上载表单*/
        return bbsupload_main(); 
    if (!strcmp(file, "/bbsupload2"))/*上载第二步*/
        return bbsupload2_main();
    if (!strcmp(file, "/bbsusr"))/*在线用户列表*/
        return bbsusr_main();
    if (!strcmp(file, "/showfile"))/*旧上载区文件显示*/
        return showfile_main();
    if (!strcmp(file, "/showinfo"))/*在线情况分析*/
        return showinfo_main();
/*    if (!strcmp(file, "/top10"))
        return top10_main();*/ /*未知用途，删除*/
    if (!strcmp(file, "/bbsmain"))/*frame*/
        return bbsmain_main();
    if (!strcmp(file, "/bbsrss"))/*板面rss feed*/
        return bbsrss_main();
	if (!strcmp(file, "/bbsvcode"))/*注册的验证码*/
		return bbsvcode_main();
    if (!strcmp(file, "/")) /*首页*/
        return main_main();
	if (!strcmp(file,"/frame")) /*frame*/
		return frame_main();
	if (!strcmp(file,"/bbsindex"))/*首页效果测试*/
		return main2_main();

	
	cache_age = 5*60;
    cgi_head();
    hprintf("File '%s' not exist!\n", file);
    exit(0);
}
