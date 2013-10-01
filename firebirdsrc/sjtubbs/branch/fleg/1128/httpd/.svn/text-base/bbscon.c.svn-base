#include "www.h"
#include <sys/sendfile.h>
static int no_re = 0;

int do_log2()
{
    int i;
    FILE *fp;

    fp = fopen("wwwlog3.log", "a");
    for (i = 0; i < 100 && environ[i]; i++)
    {
        fprintf(fp, "%s\n", environ[i]);
    }
    fclose(fp);
}
int do_cache(char *filename,char *cachefilename)
{
	FILE *fp,*fpcache;
	char buf[512],*ptr;
	int swap;	
	fp = fopen(filename, "r");
    fpcache = fopen(cachefilename,"w");

        while (1)
        {
            char *id, *s;
            if (fgets(buf, 512, fp) == 0)
                break;
            if (!strncmp(buf, "发信人: ", 8))
            {
                ptr = strdup(buf);
                id = strtok(ptr + 8, " ");
                s = strtok(0, "");
                if (id == 0)
                    id = " ";
                if (s == 0)
                    s = "\n";
                if (strlen(id) < 13 && getuser(id))
                {
                    fprintf(fpcache,"发信人: %s", userid_str(id));
                    hfprintf(fpcache,"%s", s);
                    free(ptr);
                    continue;
                }
                free(ptr);
            }
            if (!strncmp(buf, "--", 2))
                pic_mod = 1;
            if (!strncmp(buf, ": ", 2))
            {
                fprintf(fpcache,"<font color=808080>");
                swap = pic_mod;
                pic_mod = 0;
            }
            hhfprintf(fpcache,"%s", void1(buf));
            if (!strncmp(buf, ": ", 2))
            {
                fprintf(fpcache,"</font>");
                pic_mod = swap;
            }
        }
    fclose(fp);
    fclose(fpcache);
}
int bbscon_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80],*ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];

    struct fileheader x;
    int num, total;
    unsigned int jsreid;
    int swap;

    //added by cf,caching web content
    FILE *fpcache;
    struct stat originfile,cachefile;
    char cachefilename[80];
    int cachestatus;
    off_t offset=0;
    //added by hongliang
    int	start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);

    //added by hongliang
    start	= atoi(getparm("start"));

    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    //u_info->board=getbnum(board);
    printf("<font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]<hr>", BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数2");
    sprintf(dir, "boards/%s/.DIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("此讨论区不存在或者为空");
    //{add by alt@sjtu for 屏蔽回收站文章

    printf("[<a href=bbsfwd?board=%s&file=%s>转寄/推荐</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>转贴</a>]", board, file);
    printf
    ("[<a onclick='return confirm(\"你真的要删除本文吗?\")' href=bbsdel?board=%s&file=%s>删除</a>]",
     board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>修改</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&start=%d&num=%d>设置可RE属性</a>]", board, file, start, num);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>上一篇</a>]",
               board, prefilename, num - 1, start);
    }

    //added by hongliang
    printf("[<a href=\"bbsdoc?board=%s&start=%d&currentpoint=%d\">返回讨论区</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>本讨论区末页</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>下一篇</a>]",
               board, nextfilename, num + 1, start);
    }
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        (*(int *) (x.title + 73))++;
        if (*(int *) (x.title + 73) > 1000000)
            (*(int *) (x.title + 73)) = 0;
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
        brc_init(currentuser.userid, board);
        brc_add_read(x.filename);
        brc_update(currentuser.userid, board);
    }
    fclose(fp);
    strcpy(title, x.title);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    ptr[60] = 0;
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>回文章</a>]",
               board, file, x.reid);
    printf("[<a href='bbstfind0?board=%s&title=%s'>同主题列表</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("[<a href='bbstfind?board=%s&title=%s'>同主题阅读</a>]", board,
           nohtml5(void3(void1(ptr))));


    //added by hongliang for javascript
    printf("<script language=\"javascript\" src=\""UPLOAD_SERVER_HEADER"/key_hl_viewforum.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", num - start);
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var filename = \"%s\";\n", file);
    printf("var title= \"%s\";\n", nohtml5(void3(void1(ptr))));
    printf("var prefilename = \"%s\";\n", prefilename);
    printf("var nextfilename= \"%s\";\n", nextfilename);
    printf("var jsreid = %d;\n", x.reid);
    printf("\n-->\n</script>\n");


    printf
	    ("<table class=\"show_border\" width=610>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    if (strcmp(x.filename, file) != 0)
        http_fatal("本文不存在或已被删除,或者己被删除在回收站中");
    fclose(fp);
    //}
    sprintf(filename, "boards/%s/%s", board, file);
    //add by cf,cache web content.2004.08.10
    sprintf(cachefilename,"cache/%s",board);
    if(stat(cachefilename,&cachefile) == -1)
        mkdir(cachefilename,0755);
    sprintf(cachefilename,"cache/%s/%s",board,file);
    if(stat(filename,&originfile) == -1)
        http_fatal("本文不存在或者已被删除");
    if(stat(cachefilename,&cachefile) == -1)
        cachestatus=0;
    else
    {
        if(cachefile.st_mtime > originfile.st_mtime)
            cachestatus=1;
        else
            cachestatus=0;
    }

    if(cachestatus == 0)
    {
	    do_cache(filename,cachefilename);
        cachestatus = 1;
    }
    if(cachestatus == 1)
    {
        fpcache=fopen(cachefilename,"r");

        while(1)
        {
            /*	if(sendfile(fileno(stdout),fileno(fpcache),&offset,512) == -1)
            		break;
            */
            int r;
            r = fread(buf, 1, 512, fpcache);
            if (r <= 0)
                break;
            fwrite(buf, 1, r, stdout);

        }
        fclose(fpcache);
    }

    printf("</pre>\n</table><hr>\n");
    printf("[<a href=bbsfwd?board=%s&file=%s>转寄/推荐</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>转贴</a>]", board, file);
    printf
    ("[<a onclick='return confirm(\"你真的要删除本文吗?\")' href=bbsdel?board=%s&file=%s>删除</a>]",
     board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>修改</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&start=%d&num=%d>设置可RE属性</a>]", board, file, start, num);
    if (num > 0)
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>上一篇</a>]",
               board, prefilename, num - 1, start);

    //added by hongliang
    printf("[<a href=\"bbsdoc?board=%s&start=%d&currentpoint=%d\">返回讨论区</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>本讨论区末页</a>]", board);
    if (num < total - 1)
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>下一篇</a>]",
               board, nextfilename, num + 1, start);
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>回文章</a>]",
               board, file, x.reid);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("[<a href='bbstfind0?board=%s&title=%s'>同主题列表</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("[<a href='bbstfind?board=%s&title=%s'>同主题阅读</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    http_quit();
}


int bbscondel_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];
    struct fileheader x;
    int num, total;
    int swap;

    //added by hongliang
    int	start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);

    //added by hongliang
    start	= atoi(getparm("start"));

    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    //u_info->board=getbnum(board);
    printf("<font class=\"title\">%s - 已被删除文章阅读 [讨论区回收站: %s]</font><hr>", BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数2");

    //added by hongliang
    if (!has_BM_perm(&currentuser, board))
        http_fatal("对不起，您没有恢复文章的权限");

    sprintf(dir, "boards/%s/.RETURNDIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("此讨论区不存在或者为空");

    printf("[<a href=bbsdelreturn?board=%s&file=%s>恢复O</a>]", board, file);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>上一篇K</a>]",
               board, prefilename, num - 1, start);
    }

    //added by hongliang
    printf("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">返回讨论区回收站←</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>下一篇J</a>]",
               board, nextfilename, num + 1, start);
    }
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        (*(int *) (x.title + 73))++;
        if (*(int *) (x.title + 73) > 1000000)
            (*(int *) (x.title + 73)) = 0;
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
        brc_init(currentuser.userid, board);
        brc_add_read(x.filename);
        brc_update(currentuser.userid, board);
    }
    fclose(fp);
    strcpy(title, x.title);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    ptr[60] = 0;

    //added by hongliang for javascript
    printf("<script language=\"javascript\" src=\""UPLOAD_SERVER_HEADER"/key_hl_viewdelforum.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", num - start);
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var filename = \"%s\";\n", file);
    printf("var title= \"%s\";\n", nohtml5(void3(void1(ptr))));
    printf("var prefilename = \"%s\";\n", prefilename);
    printf("var nextfilename= \"%s\";\n", nextfilename);
    printf("var jsreid = %d;\n", x.reid);
    printf("\n-->\n</script>\n");

    printf
    ("<table width=610 style='BORDER: 2px solid; BORDER-COLOR: D0F0C0;'>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    if (strcmp(x.filename, file) != 0)
        http_fatal("本文不存在或已被恢复");
    fclose(fp);

    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("本文不存在或者已被完全删除");
    while (1)
    {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "发信人: ", 8))
        {
            ptr = strdup(buf);
            id = strtok(ptr + 8, " ");
            s = strtok(0, "");
            if (id == 0)
                id = " ";
            if (s == 0)
                s = "\n";
            if (strlen(id) < 13 && getuser(id))
            {
                printf("发信人: %s", userid_str(id));
                hprintf("%s", s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, "--", 2))
            pic_mod = 1;
        if (!strncmp(buf, ": ", 2))
        {
            printf("<font color=808080>");
            swap = pic_mod;
            pic_mod = 0;
        }
        hhprintf("%s", void1(buf));
        if (!strncmp(buf, ": ", 2))
        {
            printf("</font>");
            pic_mod = swap;
        }
    }
    fclose(fp);

    printf("[<a href=bbsdelreturn?board=%s&file=%s>恢复O</a>]", board, file);
    if (num > 0)
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>上一篇K</a>]",
               board, prefilename, num - 1, start);

    //added by hongliang
    printf("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">返回讨论区回收站←</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>下一篇J</a>]",
               board, nextfilename, num + 1, start);
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    http_quit();
}


int bbsdelreturn_main()
                {
                    FILE *fp;
                    struct shortfile *brd;
                    struct fileheader f;
                    struct userec *u;
                    char buf[80], dir[80], path[80], board[80], file[80], *id, *p, UTitle[128];
                    int num = 0;
                    int i;

                    init_all();
                    if (!loginok)
                        http_fatal("请先登录");
                    id = currentuser.userid;
                    strsncpy(board, getparm("board"), 60);
                    strsncpy(file, getparm("file"), 20);
                    if (!strcmp(board, "deleted"))
                        http_fatal("本板不能恢复回收文章");
                    brd = getbcache(board);
                    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                        http_fatal("错误的参数");
                    if (strstr(file, ".."))
                        http_fatal("错误的参数");
                    if (brd == 0)
                        http_fatal("板面错误");
                    if (!has_post_perm(&currentuser, board))
                        http_fatal("错误的讨论区");
#ifdef	POST_AFTER_ONE_DAY
                    if(too_young(&currentuser, board))
                        http_fatal("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
                    sprintf(dir, "boards/%s/.RETURNDIR", board);
                    sprintf(path, "boards/%s/%s", board, file);
                    fp = fopen(dir, "r");
                    if (fp == 0)
                        http_fatal("错误的参数");
                    while (1)
                    {
                        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
                            break;
                        if (!strcmp(f.filename, file))
                        {
                            if (strcasecmp(id, f.owner)
                                    && !has_BM_perm(&currentuser, board))
                                http_fatal("你无权恢复该文");

                            strcpy(UTitle, f.title);
                            if (p = strrchr(UTitle, '-'))
                            { /* create default article title */
                                *p = 0;
                                for (i = strlen(UTitle) - 1; i >= 0; i --)
                                {
                                    if (UTitle[i] != ' ')
                                        break;
                                    else
                                        UTitle[i] = 0;
                                }
                            }
                            strncpy(f.title,UTitle,STRLEN);
                            f.accessed[0] = 0;

                            return_del_article(dir, sizeof(struct fileheader), num);
                            post_article3(board, f.title, file, f.owner, f.id, f.reid);

                            printf
                            ("恢复成功.<br><a href='bbsdocdel?board=%s'>返回本讨论区回收站</a>",
                             board);
                            u = getuser(f.owner);
                            if (!junkboard(brd->filename) && u)
                            {
                                if (u->numposts > 0)
                                    u->numposts++;
                                save_user_data(u);
                            }
                            sprintf(buf, "%s %-12s bbsdel %s\n", Ctime(time(0)) + 4, id,
                                    board);
                            f_append("trace", buf);
                            {
                                char buf[256];

                                sprintf(buf, "%24.24s %s %s Do_Web_D_RETURN\n", Ctime(time(0)),
                                        currentuser.userid, board);
                                f_append("bm2.log", buf);
                            }
                            http_quit();
                        }
                        num++;
                    }
                    fclose(fp);
                    printf("文件不存在, 恢复失败.<br>\n");
                    printf("<a href='bbsdocdel?board=%s'>返回本讨论区回收站</a>", board);
                    http_quit();
                }

                void post_article3(char *board, char *title, char *file, char *id,
                                   unsigned int currid, unsigned int reid)
                {
                    FILE *fp;
                    char buf[1024];
                    struct fileheader header;

                    bzero(&header, sizeof(header));
                    strcpy(header.owner, id);
                    header.id = currid;
                    header.reid = reid;
                    sprintf(header.filename, "%s", file);
                    strsncpy(header.title, title, 60);
                    header.accessed[0] = 0;
                    sprintf(buf, "boards/%s/.DIR", board);
                    fp = fopen(buf, "a");
                    fwrite(&header, sizeof(header), 1, fp);
                    fclose(fp);
                }
