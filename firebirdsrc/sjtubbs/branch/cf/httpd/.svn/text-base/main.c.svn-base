#include "www.h"

int showindex()
{
	char buf[16*1024];
	FILE *fp = fopen("0Announce/web/index","r");
	if(fp == NULL)
		return;
	while (fgets(buf,sizeof(buf),fp)!=NULL)
		printf("%s",buf);
	fclose(fp);
	return 0;
}
int showindex2()
	
{
	char buf[16*1024];
	FILE *fp = fopen("0Announce/web/testindex","r");
	if(fp == NULL)
		return;
	while (fgets(buf,sizeof(buf),fp)!=NULL)
		printf("%s",buf);
	fclose(fp);
	return 0;
}

int main_main()
{
	silence = 1;
	cache_age = 60*10;
	last_modified_time = file_time("0Announce/web/index");
	cgi_head();
	showindex();
}
int main2_main()
{
	silence = 1;
	cgi_head();
	showindex2();
}
int frame_main()
{
	cache_age = 60*10000;
	last_modified_time = file_time("0Announce/web/index");
	cgi_head();
	  printf("
<html>
<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">
<link rel=stylesheet type=text/css href='%s'>
<head>
<title>%s</title>
</head>
<frameset border=0 frameborder=0 framespacing=2 framemargin=0 cols=\"150,*\">
    <frame name=f2 framespcing=2 src=\"bbsleft\">
    <frameset rows=\"20, *, 20\">
        <frame scrolling=no marginwidth=4 marginheight=0 framespacing=0 name=fmsg src=\"bbsgetmsg\">
        <frame framespacing=2 name=f3 src=\"bbssec\">
        <frame scrolling=no marginwidth=4 marginheight=1 framespacing=1 name=f4 src=\"bbsfoot\">
    </frameset>
</frameset>
</html>
	",CSS_FILE,BBSNAME);
}
