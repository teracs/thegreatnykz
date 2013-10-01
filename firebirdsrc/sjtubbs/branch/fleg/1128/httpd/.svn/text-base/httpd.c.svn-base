/*
*  httpd.c [HTTP server]
*
*  Lily BBS v1.0
*
*  Copyright (C) 2002 zhch.bbs@bbs.nju.edu.cn, 南大百合科技
*/

#include "www.h"

int has_http_head;

int main(int n, char *cmd[])
{
    int httpd_main();
    int i;
    int port = 80;

    for (i = 1; i < n; i++)
    {
        if (!strcmp(cmd[i], "--port") && i < n - 1)
        {
            port = atoi(cmd[i + 1]);
            i++;
        }
        else
        {
            printf("usage: sjtuwebbsd [--port port]\n");
            exit(0);
        }
    }
    tcp_bind(port, (void *) httpd_main, BBSUID);
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

int http_deal(char *file)
{
    char tmp[2560];

    if (has_http_head)
    {
        printf("HTTP/1.1 200 OK\n");
        printf("Date: %s\n", Gtime(time(0)));
        printf("Server: Sjtuwebbsd/0.95 (test version)\n");
    }
    if (!strncmp(file, "/file/", 6))
    {
        file[256] = 0;
        snprintf(tmp, 200, "name=%s", file + 6);
        setenv("QUERY_STRING", tmp, 1);
        return showfile_main();
    }
    if (!strcmp(file, "/bbs0an"))
        return bbs0an_main();
    if (!strcmp(file, "/bbsafind"))
        return bbsafind_main();
    if (!strcmp(file, "/bbsall"))
        return bbsall_main();
    if (!strcmp(file, "/bbsallf"))
        return bbsallf_main();
    if (!strcmp(file, "/bbsalluser"))
        return bbsalluser_main();
    if (!strcmp(file, "/bbsanc"))
        return bbsanc_main();
    if (!strcmp(file, "/bbsbfind"))
        return bbsbfind_main();
    if (!strcmp(file, "/bbsblockmail"))
        return bbsblockmail_main();
    if (!strcmp(file, "/bbsboa"))
        return bbsboa_main();
    if (!strcmp(file, "/bbsbrdadd"))
        return bbsbrdadd_main();
    if (!strcmp(file, "/bbsbrddel"))
        return bbsbrddel_main();
    if (!strcmp(file, "/bbsccc"))
        return bbsccc_main();
    if (!strcmp(file, "/bbsclear"))
        return bbsclear_main();
    if (!strcmp(file, "/bbscloak"))
        return bbscloak_main();
    if (!strcmp(file, "/bbscon"))
        return bbscon_main();
    if (!strcmp(file, "/bbstopcon"))
        return bbstopcon_main();
    if (!strcmp(file, "/bbsdel"))
        return bbsdel_main();
	if (!strcmp(file,"/bbsfexp2"))
		return bbsfexp2_main();
	if (!strcmp(file,"/bbsfdoc2"))
		return bbsfdoc2_main();
	if (!strcmp(file,"/bbsudel2"))
		return bbsudel2_main();
	if (!strcmp(file,"/bbsfddoc"))
		return bbsfddoc_main();
	if (!strcmp(file,"/bbsurdel"))
		return bbsurdel_main();
    //added by hongliang for recycle bin returning on Nov.21th, 2003 20:30
    if (!strcmp(file, "/bbsdocdel"))
        return bbsdocdel_main();
    if (!strcmp(file, "/bbscondel"))
        return bbscondel_main();
    if (!strcmp(file, "/bbsdelreturn"))
        return bbsdelreturn_main();
    if (!strcmp(file, "/bbsdocnore"))
        return bbsdocnore_main();

    if (!strcmp(file, "/bbsdelmail"))
        return bbsdelmail_main();
    if (!strcmp(file, "/bbsdelmsg"))
        return bbsdelmsg_main();
    if (!strcmp(file, "/bbsdenyadd"))
        return bbsdenyadd_main();
    if (!strcmp(file, "/bbsdenyall"))
        return bbsdenyall_main();
    if (!strcmp(file, "/bbsdenydel"))
        return bbsdenydel_main();
    if (!strcmp(file, "/bbsdoc"))
        return bbsdoc_main();
    if (!strcmp(file, "/bbsdoreg"))
        return bbsdoreg_main();
    if (!strcmp(file, "/bbsdoupload"))
        return bbsdoupload_main();
    if (!strcmp(file, "/bbsedit"))
        return bbsedit_main();
    if (!strcmp(file, "/bbsfadd"))
        return bbsfadd_main();
    if (!strcmp(file, "/bbsfall"))
        return bbsfall_main();
    if (!strcmp(file, "/bbsfdel"))
        return bbsfdel_main();
    if (!strcmp(file, "/bbsfdoc"))
        return bbsfdoc_main();
    if (!strcmp(file, "/bbsfexp"))
        return bbsfexp_main();
    if (!strcmp(file, "/bbsfind"))
        return bbsfind_main();
    if (!strcmp(file, "/bbsfind2"))
        return bbsfind2_main();
    if (!strcmp(file, "/bbsfoot"))
        return bbsfoot_main();
    if (!strcmp(file, "/bbsform"))
        return bbsform_main();
    if (!strcmp(file, "/bbsfriend"))
        return bbsfriend_main();
    if (!strcmp(file, "/bbsfwd"))
        return bbsfwd_main();
    if (!strcmp(file, "/bbsgcon"))
        return bbsgcon_main();
    if (!strcmp(file, "/bbsgdoc"))
        return bbsgdoc_main();
    if (!strcmp(file, "/bbsgetmsg"))
        return bbsgetmsg_main();
    if (!strcmp(file, "/bbsinfo"))
        return bbsinfo_main();
    if (!strcmp(file, "/bbsleft"))
        return bbsleft_main();
    if (!strcmp(file, "/bbslogin"))
        return bbslogin_main();
    if (!strcmp(file, "/bbslogout"))
        return bbslogout_main();
    if (!strcmp(file, "/bbsmail"))
        return bbsmail_main();
    if (!strcmp(file, "/bbsmailcon"))
        return bbsmailcon_main();
    if (!strcmp(file, "/bbsmailmsg"))
        return bbsmailmsg_main();
    if (!strcmp(file, "/bbsman"))
        return bbsman_main();
    if (!strcmp(file, "/bbsman2"))
        return bbsman2_main();
    if (!strcmp(file, "/bbsmdoc"))
        return bbsmdoc_main();
    if (!strcmp(file, "/bbsmnote"))
        return bbsmnote_main();
    if (!strcmp(file, "/bbsmnote3"))
        return bbsmnote3_main();
    if (!strcmp(file, "/bbsmovie"))
        return bbsmovie_main();
    if (!strcmp(file, "/bbsmsg"))
        return bbsmsg_main();
    if (!strcmp(file, "/bbsmybrd"))
        return bbsmybrd_main();
    if (!strcmp(file, "/bbsmywww"))
        return bbsmywww_main();
    if (!strcmp(file, "/bbsnewmail"))
        return bbsnewmail_main();
    if (!strcmp(file, "/bbsnick"))
        return bbsnick_main();
    if (!strcmp(file, "/bbsnot"))
        return bbsnot_main();
    if (!strcmp(file, "/bbsnotepad"))
        return bbsnotepad_main();
    if (!strcmp(file, "/bbsparm"))
        return bbsparm_main();
    if (!strcmp(file, "/bbsplan"))
        return bbsplan_main();
    if (!strcmp(file, "/bbspst"))
        return bbspst_main();
    if (!strcmp(file, "/bbspstmail"))
        return bbspstmail_main();
    if (!strcmp(file, "/bbspwd"))
        return bbspwd_main();
    if (!strcmp(file, "/bbsqry"))
        return bbsqry_main();
    if (!strcmp(file, "/bbsreg"))
        return bbsreg_main();
    if (!strcmp(file, "/bbssec"))
        return bbssec_main();
    if (!strcmp(file, "/bbssel"))
        return bbssel_main();
    if (!strcmp(file, "/bbssendmsg"))
        return bbssendmsg_main();
    if (!strcmp(file, "/bbssig"))
        return bbssig_main();
    if (!strcmp(file, "/bbssnd"))
        return bbssnd_main();
    if (!strcmp(file, "/bbssndmail"))
        return bbssndmail_main();
    if (!strcmp(file, "/bbsstat"))
        return bbsstat_main();
    if (!strcmp(file, "/bbstcon"))
        return bbstcon_main();
    if (!strcmp(file, "/bbstdoc"))
        return bbstdoc_main();
    if (!strcmp(file, "/bbstfind"))
        return bbstfind_main();
    if (!strcmp(file, "/bbstfind0"))
        return bbstfind0_main();
    if (!strcmp(file, "/bbstop10"))
        return bbstop10_main();
    if (!strcmp(file, "/bbstopb10"))
        return bbstopb10_main();
    if (!strcmp(file, "/bbsudel"))
        return bbsudel_main();
    if (!strcmp(file, "/bbsufind"))
        return bbsufind_main();
    if (!strcmp(file, "/bbsupload"))
        return bbsupload_main();
    if (!strcmp(file, "/bbsupload2"))
        return bbsupload2_main();
    if (!strcmp(file, "/bbsusr"))
        return bbsusr_main();
    if (!strcmp(file, "/showfile"))
        return showfile_main();
    if (!strcmp(file, "/showinfo"))
        return showinfo_main();
    if (!strcmp(file, "/top10"))
        return top10_main();
    if (!strcmp(file, "/bbsmain"))
        return bbsmain_main();
    if (!strcmp(file, "/bbsrss"))
        return bbsrss_main();
	if (!strcmp(file, "/bbsvcode"))
		return bbsvcode_main();
    if (!strcmp(file, "/"))
        return main_main();
	if (!strcmp(file,"/bbsindex"))
		return main2_main();
    /*
       if (!strcmp(file, "/bbsfupdate"))
           return bbsfupdate_main();
       if (!strcmp(file, "/bbsfupdate2"))
           return bbsfupdate2_main();
    	*/
/*
    if (!strcmp(file, "/bbs.css"))
        return catf(HTML_PATH "/bbs.css");

    //added by hongliang for some js
    if (!strcmp(file, "/key_hl_forumlist.js"))
        return catf(HTML_PATH "/key_hl_forumlist.js");
    if (!strcmp(file, "/key_hl_viewforum.js"))
        return catf(HTML_PATH "/key_hl_viewforum.js");
    if (!strcmp(file, "/key_hl_delforumlist.js"))
        return catf(HTML_PATH "/key_hl_delforumlist.js");
    if (!strcmp(file, "/key_hl_viewdelforum.js"))
        return catf(HTML_PATH "/key_hl_viewdelforum.js");

    if (!strcmp(file, "/bei.gif"))
        return catf(HTML_PATH "/bei.gif");

    if (!strcmp(file, "/link0.gif"))
        return catf(HTML_PATH "/link0.gif");
    if (!strcmp(file, "/link.gif"))
        return catf(HTML_PATH "/link.gif");
    if (!strcmp(file, "/folder.gif"))
        return catf(HTML_PATH "/folder.gif");
    if (!strcmp(file, "/folder2.gif"))
        return catf(HTML_PATH "/folder2.gif");
    if (!strcmp(file, "/telnet.gif"))
        return catf(HTML_PATH "/telnet.gif");
    if (!strcmp(file, "/sjtubg.gif"))
        return catf(HTML_PATH "/sjtubg.gif");
    if (!strcmp(file, "/x.gif"))
        return catf(HTML_PATH "/x.gif");
    if (!strcmp(file, "/bbsweb-1.jpg"))
        return catf(HTML_PATH "/bbsweb-1.jpg");
    if (!strcmp(file, "/bbsweb-2.jpg"))
        return catf(HTML_PATH "/bbsweb-2.jpg");
    if (!strcmp(file, "/bbsweb-3.jpg"))
        return catf(HTML_PATH "/bbsweb-3.jpg");
    if (!strcmp(file, "/bbsweb-4.jpg"))
        return catf(HTML_PATH "/bbsweb-4.jpg");
    if (!strcmp(file, "/bbsweb-5.jpg"))
        return catf(HTML_PATH "/bbsweb-5.jpg");
    if (!strcmp(file, "/bbsweb-6.gif"))
        return catf(HTML_PATH "/bbsweb-6.gif");
    if (!strcmp(file, "/bbsweb-7.jpg"))
        return catf(HTML_PATH "/bbsweb-7.jpg");
    if (!strcmp(file, "/bbsweb-8.jpg"))
        return catf(HTML_PATH "/bbsweb-8.jpg");
    if (!strcmp(file, "/bbsweb-9.jpg"))
        return catf(HTML_PATH "/bbsweb-9.jpg");
    if (!strcmp(file, "/bbsweb-a.gif"))
        return catf(HTML_PATH "/bbsweb-a.gif");
    if (!strcmp(file, "/bbsweb-b.jpg"))
        return catf(HTML_PATH "/bbsweb-b.jpg");
    if (!strcmp(file, "/bbsweb-c.jpg"))
        return catf(HTML_PATH "/bbsweb-c.jpg");

    if (!strcmp(file, "/bbsweb1.jpg"))
        return catf(HTML_PATH "/bbsweb1.jpg");
    if (!strcmp(file, "/bbsweb2.jpg"))
        return catf(HTML_PATH "/bbsweb2.jpg");
    if (!strcmp(file, "/bbsweb3.gif"))
        return catf(HTML_PATH "/bbsweb3.gif");
*/
    cgi_head();
    hprintf("File '%s' not exist!\n", file);
    exit(0);
}
