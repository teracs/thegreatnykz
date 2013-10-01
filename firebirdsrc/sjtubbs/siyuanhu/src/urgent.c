#include "bbs.h"

/*紧急状态开启*/
int x_urgent_start();
/*紧急状态关闭*/
int x_urgent_stop();

int x_urgent_start()
{
	char buf[80];
    char bname[STRLEN];
	char mailbuf[256];
    int pos = 0;
    struct boardheader fh;
	FILE *fp;


	clear();
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    stand_title ("板面紧急状态开启\n");
    if (!gettheboardname (3, "请输入板名: ", &pos, &fh, bname))
	{
		prints("板面不存在！\n");
		pressanykey();
		return;
	}

    setbfile(buf, bname, "board.allow");
    if(!dashf(buf))
    {
		setbfile(buf, bname, "board.reject");
		if(!dashf(buf))
		{
			move(5,10);
			prints("板面不属于团体板面");
	        pressanykey();
		    return;
		}
    }

	setbfile(buf, bname, "board.urgent");

	if(dashf(buf))
	{
        move(5,10);
		prints("对不起，紧急功能已经开启。\n");
		pressanykey();
		return;
	} 


	if(askyn ("确认开启？", YEA, NA) == YEA)
	{
		fp = fopen(buf,"w+");
		fclose(fp);

		sprintf(mailbuf,"板面紧急状态开启 %s 开启人 %s",bname,currentuser.userid);
		securityreport(mailbuf);
		pressanykey();
	}

	return 1;
}



int x_urgent_stop()
{
	char buf[80];
	char mailbuf[256];
    char bname[STRLEN];
    int  pos = 0;
    struct boardheader fh;
	FILE *fp;


	clear();
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("板面紧急状态终止\n");
    if (!gettheboardname (3, "请输入板名: ", &pos, &fh, bname))
	{
		prints("板面不存在！\n");
		pressanykey();
		return;
	}
	
	setbfile(buf, bname, "board.urgent");
	if(!dashf(buf))
	{
        move(5,10);
		prints("对不起，紧急功能没有开启。\n");
		pressanykey();
		return;
	} 

	if(askyn ("确认终止？", YEA, NA) == YEA)
	{
		setbfile(buf, bname, "board.urgent");
		unlink(buf);

		sprintf(mailbuf,"板面紧急状态终止 %s 开启人 %s",bname,currentuser.userid);
		securityreport(mailbuf);
		pressanykey();
	}
	return 1;
}

