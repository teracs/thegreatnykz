#include "www.h"

static int getmsg_IsNumber(char *pchar)
{
    if (pchar == NULL)
        return 0;

    if ((*pchar >= '0') && (*pchar <= '9'))
        return 1;

    return 0;
}

int bbsgetmsg_main()
{
    char buf[256], buf2[1200] = ".";
    char buf3[256];
    char *p3 = NULL;
    char toid[13];

    init_all();
    memset(buf3, 0x00, 256);
    p3 = buf3;
    printf("<style type=text/css>\n");
    printf("A {color: #0000FF}\n");
    printf("</style>\n");
    if (loginok == 0)
        http_quit();

    sethomefile(buf, currentuser.userid, "wwwmsg");
    if (file_size(buf) > 0)
    {
        int total;

        printf
            ("<body onkeypress='checkrmsg(event.keyCode)' style='BACKGROUND-COLOR: #f0ffd0'>");

        total = file_size(buf) / sizeof(struct msgdir);
        get_record(buf2, sizeof(struct msgdir), 0, buf);
        del_record(buf, sizeof(struct msgdir), 0);

        printf("<div id='getmsg'><table width=100%%>\n");
        printf("<tr><td>");

        buf2[IDLEN + 2 + sizeof(time_t) + sizeof(pid_t) + 1024] = 0;

        memcpy(p3, buf2 + IDLEN + 2 + sizeof(time_t) + sizeof(pid_t), 24);

        p3 += 24;
        memcpy(p3, ": ", 2);

        p3 += 2;

        if (!getmsg_IsNumber
            (buf2 + IDLEN + 2 + sizeof(time_t) + sizeof(pid_t) + 24 + 13))

        {
            memcpy(p3,
                   buf2 + IDLEN + 2 + sizeof(time_t) +
                   sizeof(pid_t) + 24 + 95, 84);

        }

        else

        {
            memcpy(p3,
                   buf2 + IDLEN + 2 + sizeof(time_t) +
                   sizeof(pid_t) + 24 + 30, 84);

        }

        buf3[110] = 0;

        hprintf("%s", buf3);

        sscanf(buf2, "%s", toid);

        printf
            ("<td align=right><a target=f3 href=bbssendmsg?destid=%s>[»ØÑ¶Ï¢]</a> \
<a href='javascript:if(top.refresh_msg)top.refresh_msg();else document.location=\"bbsgetmsg\";'>[ºöÂÔ]</a>\n",
             toid);
        printf("<script language=\"javascript\">\n\
			if(!gmsg){\n\
			var rframe = top.document.getElementById(\"rframe\");\n\
			if(!rframe.rows) rframe = top.rightframe;\n\
			rframe.rows = \"54,*,20\";}\n\
			</script>\n\
			</table><form><input type='hidden' name='gmsg' id='gmsg' value='y'/></form></div></body>\n");
    }
    else
    {
        printf("<body><div id='getmsg'>\n\
			<script language=\"javascript\"> \n\
			if(!gmsg){\n\
			var rframe = top.document.getElementById(\"rframe\");\n\
			if(!rframe.rows) rframe = top.rightframe;\n\
			rframe.rows = \"0,*,20\";\n\
			setTimeout(\"document.location='bbsgetmsg'\",120000);}\n\
			</script>\n\
			<form><input type='hidden' name='gmsg' id='gmsg' value='n'/></form></div></body>\n");
    }
    printf("</html>\n");
    return 0;
}
