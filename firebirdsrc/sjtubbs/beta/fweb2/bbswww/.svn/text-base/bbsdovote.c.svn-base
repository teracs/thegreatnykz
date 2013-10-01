#include "www.h"



static int
append_ip(char *filename,char *ip)
{
	int fd;
	struct voteip tmp;
	int size=sizeof(struct voteip);
	if((fd=open(filename,O_RDWR|O_CREAT,0644))==-1)
	{
		return -1;
	}
	while(read(fd,&tmp,size)==size)
	{
		if(!strcmp(ip,tmp.ip))
		{
			flock(fd, LOCK_EX);
			lseek(fd,-sizeof(unsigned int),SEEK_CUR);
			tmp.ipnum++;
			write(fd,&tmp.ipnum,sizeof(unsigned int));
			flock(fd, LOCK_UN);
			close(fd);
			return 0;
		}
	}
	flock(fd, LOCK_EX);
	lseek(fd,0,SEEK_END);
	sprintf(tmp.ip,"%s",ip);
	tmp.ipnum=1;
	write(fd,&tmp,size);
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}


int
append_record(filename, record, size)
char   *filename;
char   *record;
int     size;
{

    int     fd;
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
    {
        return -1;
    }
    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);
    safewrite(fd, record, size);
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

static int 
search_ip(char *filename,char *userip)
{
	int fd;
	int size=sizeof(struct voteip);
	struct voteip tmp;
	if((fd=open(filename,O_RDONLY,0))==-1)
		return 0;
	while(read(fd,&tmp,size)==size)
	{
		if(!strcmp(userip,tmp.ip))
		{
			close(fd);
			return tmp.ipnum;
		}
	}
	close(fd);
	return 0;
		
	
}



int
bbsdovote_main()
{
	char board[80], text[512], valuestring[10];
	char flagfile[STRLEN],ipfile[STRLEN], uipfile[STRLEN];
	char *p;
	struct shortfile *x1;
	struct ballot uservote,tmpvote;
	struct voter_info v_info;
	int type,totalitems,value;
	int width = 720,  i,fd;
	time_t opendate;
	int voted_flag;

	voted_flag=NA;
	init_all();
	strsncpy(board, getparm("board"), 32);
	opendate=atoi(getparm("opendate"));
	totalitems=atoi(getparm("totalitems"));
	type=atoi(getparm("type"));
	if (!has_post_perm(&currentuser, board))
	{
		http_quit_print("错误的讨论区");
	}
	x1 = getbcache(board);
	strcpy(board, x1->filename);

	if (my_font_mode == 0)
		width = 670;


	printf("<body>");
	printf("<form name=form1 action='bbsdovote' method=post enctype='multipart/form-data'>\n<input type=hidden name=board value='%s'>", board);
	printf("<input type=hidden name=opendate value='%lu'>",opendate); 
	printf("<center>\n");
	printf("<table><tr><td>");
	printf("<td><font class=title><b>%s(%s)</b></font></table>\n",
	       x1->filename, x1->title + 11);

	printf("<nobr>\n");
	printf("<table width=%d><tr style='height:14px'><td align=left width=40%%> 板主: %s", width, userid_str(x1->BM));
	printf("<td align=right width=10%%><font style='font-size:12px; color:#f00000'>投票中</font></td>");
	printf("</table><hr>\n");
	sprintf(ipfile,"vote/%s/ip.%ld",board,opendate);
	sprintf (uipfile, "vote/%s/uip.%ld", board, opendate);	// 设定ip信息文件
	sprintf(flagfile,"vote/%s/flag.%ld",board,opendate);
	if ((fd = open(flagfile, O_RDWR|O_CREAT,0644)) == -1)
	{
		printf("<tr><td>打开记录文件错误，请重试</td>");
		goto out;
	}
	while(read(fd,&tmpvote,sizeof(struct ballot))==sizeof(struct ballot))
	{
		if(!strcmp(tmpvote.uid,currentuser.userid))
		{
			voted_flag=YEA;
			break;
		}
	}
	
	if(voted_flag==YEA)
	{
		lseek(fd,-sizeof(struct ballot),SEEK_CUR);
	}
	else lseek(fd,0,SEEK_END);
	if(voted_flag==NA&&search_ip(ipfile,currentuser.lasthost)>=MAXVOTEPERIP)
	{
		printf("<tr><td>由于您的ip已经超过投票次数,您将不能再投票.</td>");
		close(fd);
		goto out;
		
	}
	sprintf(uservote.msg[0],"%s","");
	sprintf(uservote.msg[1],"%s","");
	sprintf(uservote.msg[2],"%s","");
	if(type==1||type==2)
	{
		if(type==1)sprintf(valuestring,"%s","SF");
		else if(type==2)sprintf(valuestring,"%s","DX");
		value=atoi(getparm(valuestring));
		uservote.voted=0;
		uservote.voted^=(1<<value);
	}
	else if(type==3)
	{
		uservote.voted=0;
		for(i=0;i<=totalitems-1;i++)
		{
			sprintf(valuestring,"FX%d",i);
			if(strlen(getparm(valuestring))==0)continue;
			value=atoi(getparm(valuestring));
			uservote.voted^=(1<<value);
		}
	}
	else if(type==4)
	{
		sprintf(valuestring,"%s","SZ");
		uservote.voted=atoi(getparm(valuestring));
	}
	else if(type==5)
	{
		sprintf(valuestring,"%s","WD");
		sprintf(text,"%s",getparm(valuestring));
		if((p=strtok(text,"\n")))sprintf(uservote.msg[0],"%s",p);
		if((p=strtok(NULL,"\n")))sprintf(uservote.msg[1],"%s",p);
		if((p=strtok(NULL,"\n")))sprintf(uservote.msg[2],"%s",p);
	}
	if(type!=5)
	{
		sprintf(valuestring,"%s","YJ");
		sprintf(text,"%s",getparm(valuestring));
		if((p=strtok(text,"\n")))sprintf(uservote.msg[0],"%s",p);
		if((p=strtok(NULL,"\n")))sprintf(uservote.msg[1],"%s",p);
		if((p=strtok(NULL,"\n")))sprintf(uservote.msg[2],"%s",p);
	}
	sprintf(uservote.uid,"%s",currentuser.userid);
	sprintf (v_info.uid,"%s", currentuser.userid);
	sprintf (v_info.uip,"%s", currentuser.lasthost);
	v_info.vote_time = time (0);
	flock(fd, LOCK_EX);
	write(fd,&uservote,sizeof(uservote));
	flock(fd, LOCK_UN);
	append_record(uipfile, &v_info, sizeof(v_info));
	append_ip(ipfile,currentuser.lasthost);
	close(fd);
	
	printf("您的投票已记入投票箱中，谢谢您的参与！");
out:
	printf("<hr><center><a href=bbsdoc?board=%s>返回</a></center> ", board);
	printf("</form>\n");
	printf("</body>\n");
	return 0;
}

