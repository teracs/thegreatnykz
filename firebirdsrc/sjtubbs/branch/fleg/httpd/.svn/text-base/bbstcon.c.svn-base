#include "www.h"

extern int do_cache(char *filename,char *cachefilename);

static int no_re = 0;
static int nums = 0;

int bbstcon_main()
{
    FILE *fp;
    char title[256], userid[80], board[80], dir[80], file[80], *ptr;
    struct fileheader x;
    int num = 0, found = 0;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    if (loginok)
        brc_init(currentuser.userid, board);
    printf("%s -- 主题文章阅读 [讨论区: %s]<hr color=green>", BBSNAME,
           board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数2");
    sprintf(dir, "boards/%s/.DIR", board);
    if (!strcasecmp(board, "notice"))
        no_re = 1;
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("目录错误");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        num++;
        if (!strcmp(x.filename, file))
        {
			ptr = x.title;
            if (!strncmp(ptr, "Re:", 3))
                ptr += 4;
            strsncpy(title, ptr, 40);
            (*(int *) (x.title + 73))++;
            fseek(fp, -1 * sizeof(x), SEEK_CUR);
            fwrite(&x, sizeof(x), 1, fp);
            found = 1;
            strcpy(userid, x.owner);
            show_file(board, &x, num - 1);
            while (1)
            {
                if (fread(&x, sizeof(x), 1, fp) <= 0)
                    break;
                num++;
                if (!strncmp(x.title + 4, title, 39)
                        && !strncmp(x.title, "Re: ", 4))
				{
					show_file(board, &x, num - 1);
				}
                    
            }
        }
    }
    fclose(fp);
    if (found == 0)
        http_fatal("错误的文件名");
    printf("本主题共有 %d 篇文章.<br>\n", nums);
    printf("<script>\n");
    printf
    ("if(top==self) document.writeln('[<a href=url?url=bbstcon?board=%s@file=%s>添加边框</a>]');",
     board, file);
    printf("</script>\n");
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s'>回文章R</a>] ",
               board, file);
    printf("[<a href='javascript:history.go(-1)'>返回上一页←</a>]");
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    ptr = x.title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("</center>\n");
    //added by hongliang on Nov. 23th, 2003 19:58 for keyboard shortcut
    printf("<script language=\"javascript\">\n");
    printf("<!--\nfunction killErrors() { return true; } \n");
    printf("window.onerror = killErrors;\nself.focus();");
    printf("document.onkeydown = kd;\n\n");
    printf("function kd()\n{\n\t");
    printf("if ( window.event.keyCode == 37 )\n\t{\n\t\thistory.go(-1);\n\t}\n");
    printf("else if ( window.event.keyCode == 82 )\n\t{\n\t\t");
    printf("location.href('bbspst?board=%s&file=%s');\n\t}\n}\n", 
		board, file);
    printf("-->\n</script>\n");

    if (loginok)
        brc_update(currentuser.userid, board);
    http_quit();
}

int show_file(char *board, struct fileheader *x, int n)
{
    FILE *fp;
    char path[80], buf[512],cachepath[80];
	struct stat filestat,cachestat;
    int is_sig, cachestatus ;

    if (loginok)
        brc_add_read(x->filename);
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
    printf("[<a href=bbscon?board=%s&file=%s&num=%d>本篇全文</a>] ", board,
           x->filename, n);
    printf("[<a href='bbspst?board=%s&file=%s'>回复本文</a>] ", board,
           x->filename);
    printf("[本篇作者: %s] ", userid_str(x->owner));
    printf("[本篇人气: %d]\n", *(int *) (x->title + 73));
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
