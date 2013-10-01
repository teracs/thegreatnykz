#include "www.h"

/*	bbsupload2 version 2
	author: 		jawahh
	description:	Change upload file to somewhere can be accessed from 
					another staic file httpd server. And every boards have 
					it's own uploading space
					dir file is placed in UPLOAD_PATH2/board_name
					uploading file is placed in UPLOAD_FILE_PATH2/board_name/file_name
*/

static struct dir x;

int bbsupload2_main()
{
    char file[80], name[80];
    int  level, pid;

    init_all();
    if (!loginok)
        http_fatal("匆匆过客无法执行本操作，请先登录");
    x.active = 1;
    x.level = atoi(getparm("level"));
    strsncpy(x.board, getparm("board"), 20);
    strsncpy(x.exp, getparm("exp"), 72);
    strsncpy(x.userid, currentuser.userid, 13);
    strsncpy(x.showname, getparm("name"), 36);
    get_unique_file_name(x.showname);
	x.date = time(0);
    strsncpy(x.type, getparm("type"), 30);
    x.live = atoi(getparm("live"));
    level = atoi(getparm("level"));
    pid = atoi(getparm("file"));
    sprintf(file, "tmp/%d.upload", pid);
    strsncpy(name, getparm("name"), 30);	
    if (!file_exist(file))
        http_fatal("错误的文件名");
    if (!has_post_perm(&currentuser, x.board))
    {
		unlink(file);
        http_fatal("错误的讨论区或无权上载至本讨论区");
    }
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, x.board))
	{
		unlink(file);
		http_fatal("您注册时间还不满24小时，暂时不能在本板上传附件.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
	}
#endif	
    x.size = file_size(file);
    {
        char buf[256];

        sprintf(buf, "%16.16s %s %d %s %s %s\n",
                Ctime(time(0)) + 4, currentuser.userid, x.size, fromhost,
                x.board, x.showname);
        f_append("upload.log", buf);
    }
    if (x.size > 5000000)
    {
        unlink(file);
        http_fatal("文件太大(请不要超过5M)");
    }
    upload(file, &x);
    unlink(file);
    printf("附件上载成功, ");
    printf
    ("URL为 :<P><font color=green>http://"BBSHOST":"UPLOAD_SERVER_PORT"/%s/%s </font><br>\n",
     x.board,x.showname);
    printf
    ("(以上URL(绿色部分)可拷贝至文章中直接引用, 系统会判断类型自动生成超链接.)<br>\n");
    printf
    ("如不拷贝, 则只能在板面的文章上载区看到本附件.<hr color=green>\n");
    printf("评定等级: %d星<br>\n", x.level + 1);
    printf("保留时间: %d天<br>\n", x.live);
    printf("文件大小: %d字节<br>\n", x.size);
    printf("文件类型: %s<br>\n", x.type);
    printf("文件名称: %s<br>\n", x.showname);
    printf("上载人ID: %s<br>\n", x.userid);
    printf("上载时间: %s<br>\n", Ctime(time(0)) + 4);
    printf("上载板面: %s<br>\n", x.board);
    printf("文件说明: %s<br>\n", nohtml(x.exp));
    printf
    ("<hr color=green><a href='javascript:history.go(-2)'>返回</a>\n");
    http_quit();
}

/* int badfile(char *filename)
{
    int i;

    for (i = 0; filename[i]; i++)
        if (!strchr
                ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                 filename[i]))
            return 1;
    return 0;
}*/

int upload(char *file, struct dir *x)
{
    FILE *fp;
    int t = times(0);
    char buf[256], dir[256];

    x->filename = t;
	sprintf(dir,UPLOAD_FILE_PATH2"/%s",x->board);
	if(!file_exist(dir))
		mkdir(dir,0711);
    sprintf(buf, "%s/%s",dir,x->showname);
    link(file, buf);
    sprintf(dir, UPLOAD_PATH2"/%s",x->board);
    fp = fopen(dir, "a");
    fwrite(x, sizeof(struct dir), 1, fp);
    fclose(fp);
    return 0;
}

/*
construct a file name for upload file
the format is time+pid+internal counter+postfix
it seems impossible that two files get the same name
name:	the origin file name
note:	this funtion will overwrite the memory where name point to
		it means, you will get the new file name from where name point to
*/
int get_unique_file_name(char *name)
{
	static int counter = 0;
	char buf[40];
	char *postfix, *p;
	postfix=strrchr(name,'.');
	if( NULL != postfix)
	{
		for(p=postfix+1; *p != '\0'; p++)
			if( ! isalnum(*p))
			{
				*postfix='\0';
				break;
			}
		snprintf(buf,40,"%d%d%d%s",time(0),getpid(),counter,postfix);
	}
	else
	{
		snprintf(buf,40,"%d%d%d",time(0),getpid(),counter);
	}
	strsncpy(name,buf,40 );
	counter ++;
	return 0;
}

/* int do_check(char *show)
{
    FILE *fp;
    struct dir x;

    show[30] = 0;
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        return;
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        if (!strcmp(x.showname, show))
        {
            sprintf(show, "%d%s", time(0), x.showname);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}*/
