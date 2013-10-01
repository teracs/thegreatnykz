#include "www.h"
#define TOPIC_MAX 1000
extern int do_cache(char *filename,char *cachefilename);

static int no_re = 0;
static int nums = 0;

int bbstcon_main()
{
    FILE *fp;
    char userid[80], board[80], reid_str[16], file[32];
    struct fileheader *fh, *fhs[1000];
    int num = 0, offset;
	unsigned int reid = 0;

	silence = 1;
	init_all();
	strsncpy(board, getparm("board"), 32);
	strsncpy(reid_str, getparm("reid"), 16);
	strsncpy(file,getparm("file"),31);
	if (!has_read_perm(&currentuser, board))
	{
		cgi_head();
		html_head();
		http_fatal("错误的讨论区");
	}
	else
	{
		if(is_normal_board)
			cache_age = 3*60;
		cgi_head();
		html_head();
	}
	if(strlen(reid_str)!=0)
		reid = atoi(reid_str);
	else
		http_fatal("错误参数reid");	
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
	fh=(struct fileheader *)map_board_dir(board, ".DIR");
	if(fh==NULL)
        http_fatal("目录错误");
	offset = get_size_of_mapdir()-1;
	for(;offset>=0;--offset)
		if(fh[offset].reid == reid)
		{
			fhs[num++] = &fh[offset];
			if(fh[offset].id == reid||(!strcmp(fh[offset].filename,file))||num==TOPIC_MAX)
				break;
		}
	if(num == 0)
		http_fatal("该主题不存在");
    printf("<center>\n");

    printf("%s -- 主题文章阅读 [讨论区: %s]<hr color=green>", BBSNAME,
           board);
	for(offset = num-1;offset >= 0; --offset)
		show_file(board, fhs[offset]);	
    printf("本主题共有 %d 篇文章.<br>\n", num);
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s'>回文章</a>] ",
               board, fhs[num-1]->filename);
    printf("[<a href='javascript:history.go(-1)'>返回上一页</a>]");
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    printf("</center>\n");
    http_quit();
}

int show_file(char *board, struct fileheader *x)
{
    FILE *fp;
    char path[80], buf[512],cachepath[80];
	struct stat filestat,cachestat;
    int is_sig, cachestatus ;

	sprintf(path, "boards/%s/%s", board, x->filename);
	if(stat(path,&filestat)==-1)
		return;
	sprintf(cachepath,"cache/%s",board);
	if(stat(cachepath,&cachestat)==-1)
		mkdir(cachepath,0755);
	sprintf(cachepath,"cache/%s/%s",board,x->filename);
	if(stat(cachepath,&cachestat)==-1)
		cachestatus = 0;
	else
	{
		if(cachestat.st_mtime > filestat.st_mtime)
            cachestatus=1;
        else
            cachestatus=0;
	}
	if(0==cachestatus)
	{
		do_cache(path,cachepath);
		cachestatus = 1;
	}
    fp = fopen(cachepath, "r");
    if (fp == 0)
        return;
    printf
    ("<table width=610 style='BORDER: 2px solid; BORDER-COLOR: D0F0C0;'><tr><td><pre>\n");
    printf("[<a href='bbspst?board=%s&file=%s'>回复本文</a>] ", board,
           x->filename);
    nums++;
    while(1)
    {
		int r;
        if( (r=fread(buf, 1, 512, fp)) <= 0)
            break;
        fwrite(buf, 1, r, stdout);	
    }
    fclose(fp);
    printf("</pre></table><hr color=white>");
}
