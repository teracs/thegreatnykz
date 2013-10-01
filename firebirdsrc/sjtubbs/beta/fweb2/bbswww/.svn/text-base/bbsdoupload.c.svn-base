#include <glib.h>
#include "www.h"


/*
construct a file name for upload file
the format is time+pid+internal counter+postfix
it seems impossible that two files get the same name
name:	the origin file name
note:	this funtion will overwrite the memory where name point to
		it means, you will get the new file name from where name point to
*/
static int
get_unique_file_name(char *name)
{
	static int counter = 0;
	char buf[40];
	char *postfix, *p;
	postfix = strrchr(name, '.');
	if (NULL != postfix)
	{
		for (p = postfix + 1; *p != '\0'; p++)
			if (!isalnum(*p))
			{
				*postfix = '\0';
				break;
			}
		snprintf(buf, 40, "%ld%d%d%s", time(0), getpid(), counter,
			 postfix);
	}
	else
	{
		snprintf(buf, 40, "%ld%d%d", time(0), getpid(), counter);
	}
	strsncpy(name, buf, 40);
	counter++;
	return 0;
}


int
bbsdoupload_main()
{
	char path[1024], path2[1024], buf[4048],filename[1024];
	cgiFilePtr file;
	struct dir x;
	int got,flag;
	FILE *upfile;
	init_all();
	if (!loginok)
		http_quit_print("匆匆过客无法执行本操作，请先登录");


	if (!(cgiFormSuccess == cgiFormFileSize("up", &(x.size))
	    && 0 < x.size
	    && (1024*1024+1) > x.size
	    && cgiFormSuccess == cgiFormFileName("up", filename, 1024)
	    && cgiFormSuccess == cgiFormFileOpen("up", &file)))
	{
		printf("未上传文件,<a href='javascript:history.go(-1)'>返回</a>\n");
		return 0;
	}

	cgiFormFileContentType("up", x.type, 30);
	x.active = 1;
	strsncpy(x.userid, currentuser.userid, 13);
	cgiFormInteger("level", &(x.level), 0);
	cgiFormInteger("live", &(x.live), 180);
	cgiFormString("board", x.board, 20);
	cgiFormString("exp", x.exp, 80);
	cgiFormInteger("flag",&flag,0);
#ifdef	POST_AFTER_ONE_DAY
	if (too_young(&currentuser, x.board))
	{
		cgiFormFileClose(file);
		http_quit_print
			("您注册时间还不满24小时，暂时不能在本板上传附件.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
	}
#endif
	if (!has_post_perm(&currentuser, x.board))
	{
		cgiFormFileClose(file);
		http_quit_print("错误的讨论区或无权上载至本讨论区");
	}
	get_unique_file_name(filename);
	x.date = time(0);
	strncpy(x.showname, filename, 40);

	g_message("%s upload file %s at board %s from %s, file size: %d",
		  currentuser.userid, x.showname, x.board, cgiRemoteAddr,
		  x.size);

	snprintf(path, sizeof(path), UPLOAD_FILE_PATH2 "/%s", x.board);
	if (!file_exist(path))
		mkdir(path, 0755);
	snprintf(path2, sizeof(path), "%s/%s", path, x.showname);
	upfile = fopen(path2, "w+");
	if (!upfile)
	{
		cgiFormFileClose(file);
		http_quit_print("不能写文件");
	}
	while (cgiFormFileRead(file, buf, sizeof(buf), &got) ==
	       cgiFormSuccess)
	{
		fwrite(buf, got, 1, upfile);
	}
	fclose(upfile);
	cgiFormFileClose(file);

	sprintf(path, UPLOAD_PATH2 "/%s", x.board);
	upfile = fopen(path, "a");
	if (!upfile)
		http_quit_print("不能写文件");
	fwrite(&x, sizeof(struct dir), 1, upfile);
	fclose(upfile);
	if(flag == 1 || flag == 2 ) {
		if(flag == 1) {
		printf("<script src=\"/xml/index.js\" language=\"javascript\" type=\"text/javascript\"></script>"); 
		} else {
			printf("<script src=\"/js/attach.js\" language=\"javascript\" type=\"text/javascript\"></script>"); 
		}
		
		printf("<script language=\"javascript\">\n");
		printf("var attachurl = '"UPLOAD_FILE_URL_HEADER"/%s/%s';\n", x.board, x.showname);
		printf("applyattach(attachurl);");
		printf("</script>");


		} else {
	printf("附件上载成功, ");
	printf("URL为 :<P><font color=green>" UPLOAD_FILE_URL_HEADER
	       "/%s/%s </font><br>\n", x.board, x.showname);
	printf("(以上URL(绿色部分)可拷贝至文章中直接引用, 系统会判断类型自动生成超链接.)<br>\n");
	printf("如不拷贝, 则只能在板面的文章上载区看到本附件.<hr color=green>\n");
	printf("评定等级: %d星<br>\n", x.level + 1);
	printf("保留时间: %d天<br>\n", x.live);
	printf("文件大小: %d字节<br>\n", x.size);
	printf("文件类型: %s<br>\n", x.type);
	printf("文件名称: %s<br>\n", x.showname);
	printf("上载人ID: %s<br>\n", x.userid);
	printf("上载时间: %s<br>\n", Ctime(time(0)) + 4);
	printf("上载板面: %s<br>\n", x.board);
	printf("文件说明: %s<br>\n", nohtml(x.exp));
	printf("<hr color=green><a href='javascript:history.go(-1)'>返回</a>\n");
		}
	printf("\n</html>");
	return 0;
}
