#include "www.h"

int
bbsmail_main()
{
	FILE *fp;
	int filetime, i, start, total;
	int ret, max = 0, size = 0;
	char *ptr, buf[512], dir[80];
	struct fileheader *data;

	init_all();
	if (!loginok)
		http_quit_print("您尚未登录, 请先登录");
	strsncpy(buf, getparm("start"), 10);
	start = atoi(buf);
	if (buf[0] == 0)
		start = 999999;
	printf("<center>\n");
	printf("<hr>\n");
	sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
		currentuser.userid);
	total = file_size(dir) / sizeof(struct fileheader);
	if (total < 0 || total > 30000)
		http_quit_print("too many mails");
	data = (struct fileheader *) calloc(total, sizeof(struct fileheader));
	if (data == 0)
		http_quit_print("memory overflow");
	if (!file_exist(dir))
	{
		char buf[80];

		sprintf(buf, "mail/%c/%c/%s", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
			currentuser.userid);
		if (!file_exist(buf))
			mkdir(buf, 0770);
		fp = fopen(dir, "a+");
		fclose(fp);
	}
	fp = fopen(dir, "r");
	if (fp == 0)
		http_quit_print("dir error");
	total = fread(data, sizeof(struct fileheader), total, fp);
	fclose(fp);
	if (start > total - 19)
		start = total - 19;
	if (start < 0)
		start = 0;
	printf("<table width=650>\n");
	printf("<form action=bbsman2 method=post>\n");
	printf("<tr bgcolor=e8e8e8><td>序号<td>删<td>状态<td>发信人<td>日期<td>信件标题\n");
	for (i = start; i < start + 19 && i < total; i++)
	{
		//              int type='N', type2=' ';
		int type = 'N';
		char filename[256];

		printf("<tr><td>%d", i + 1);
		printf("<td><input style='height:14px' type=checkbox name=box%s>", data[i].filename);
		if (data[i].accessed[0] & FILE_READ)
			type = ' ';
		if (data[i].accessed[0] & FILE_MARKED)
			type = (type == 'N') ? 'M' : 'm';
		/*
		   if(type!='N') {
		   printf("<td>%c%c", type, type2);
		   } else {
		   printf("<td>");
		   }
		 */
		printf("<td>%c", type);
		ptr = strtok(data[i].owner, " (");
		if (ptr == 0)
			ptr = " ";
		ptr = nohtml(ptr);
		printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
		filetime = atoi(data[i].filename + 2);
		printf("<td>%12.12s", Ctime(filetime) + 4);
		printf("<td><a href=bbsmailcon?file=%s&num=%d>",
		       data[i].filename, i);
		if (strncmp("Re: ", data[i].title, 4))
			printf("★ ");
		hprintf("%42.42s", void1(data[i].title));
		sprintf(filename, "mail/%c/%c/%s/%s",
			toupper(currentuser.userid[0]), toupper(currentuser.userid[1]), currentuser.userid,
			data[i].filename);
		printf("</a>%s\n", eff_size(filename));
	}
	free(data);
	printf("</table><hr>\n");
	ret = check_maxmail2(&max, &size);
	printf("<input style='height:20px' onclick='return confirm(\"确实要删除所有标记的信件吗\")' type=submit value='删除标记的信件'>\n");
	printf("[您的信箱容量为: %dK, 目前已使用: %dK, 信件总数: %d封] ", max,
	       size, total);
	printf("[<a href=bbspstmail>发送新信件</a>]\n");
	printf("</form>\n");
	if (start > 0)
	{
		i = start - 19;
		if (i < 0)
			i = 0;
		printf("[<a href=bbsmail?start=0>第一页</a>] ");
		printf("[<a href=bbsmail?start=%d>上一页</a>] ", i);
	}
	if (start < total - 19)
	{
		i = start + 19;
		if (i > total - 1)
			i = total - 1;
		printf("[<a href=bbsmail?start=%d>下一页</a>] ", i);
		printf("[<a href=bbsmail>最后一页</a>]");
	}
	printf("<form><input type=submit value=跳转到> 第 <input style='height:20px' type=text name=start size=3> 封</form>");
	if (size > max)
		printf("<script>您的信件过量, 请清理过期信件!</script>\n");
	http_quit();
	return 0;
}
