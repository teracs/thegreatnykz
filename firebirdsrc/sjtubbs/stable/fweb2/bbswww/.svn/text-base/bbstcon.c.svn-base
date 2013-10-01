#include "www.h"
#define TOPIC_MAX 2000
#define TPAGE_SIZE 20
#define TPAGE_SIZE_ALL  (TOPIC_MAX + 1)

static void show_file(char *board, struct fileheader *x)
{
    FILE *fp;
    char path[80], buf[512], cachepath[80];
    struct stat filestat, cachestat;
    int cachestatus;

    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(x->filename),
            x->filename);
    if (stat(path, &filestat) == -1)
        return;
    sprintf(cachepath, "cache/%s/%s/%s", board, getfolderstr(x->filename),
            x->filename);
    if (stat(cachepath, &cachestat) == -1)
        cachestatus = 0;
    else
    {
        if (cachestat.st_mtime > filestat.st_mtime)
            cachestatus = 1;
        else
            cachestatus = 0;
    }
    if (0 == cachestatus)
    {
        do_cache(path, cachepath);
        cachestatus = 1;
    }
    fp = fopen(cachepath, "r");
    if (fp == 0)
        return;
    printf("<table class=show_border width=610><tr><td><pre>\n");
    printf("[<a href='bbspst?board=%s&file=%s'>回复本文</a>][<a href='bbscon?board=%s&file=%s'>原帖</a>] ", board,
           x->filename, board, x->filename);
    while (1)
    {
        int r;

        if ((r = fread(buf, 1, 512, fp)) <= 0)
            break;
        fwrite(buf, 1, r, stdout);
    }
    fclose(fp);
    printf("</pre></table><br>");
}

int bbstcon_main()
{
    char board[80], reid_str[16], file[32], page_str[8], showall[8];
    struct fileheader *fh, *fhs[TOPIC_MAX];
    int num = 0, offset, no_re = 0, i = 0, count = TPAGE_SIZE;
    unsigned int reid = 0;

    int page_size = TPAGE_SIZE;
    int page = 1;
    int page_max = 0;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(reid_str, getparm("reid"), 16);
    strsncpy(file, getparm("file"), 31);
    strsncpy(page_str, getparm("page"), 8);
    strsncpy(showall, getparm("showall"), 8);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("错误的讨论区");
    }
    else
    {
        if (is_normal_board)
            cache_age = 3 * 60;
    }
    if (strlen(reid_str) != 0)
        reid = atoi(reid_str);
    else
	{
        cgi_head();
        html_head();
        print_style();
        http_quit_print("错误参数reid");
	}
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
	{
        cgi_head();
        html_head();
        print_style();
        http_quit_print("目录错误");
	}
    if (strlen(page_str) > 0)
    {
        page = atoi(page_str);
    }
    else
    {
        page = 1;
    }
    if (strlen(showall) > 0)
    {
        count = page_size = TPAGE_SIZE_ALL;
        page = 1;
    }
    offset = get_size_of_mapdir() - 1;
    for (; offset >= 0; --offset)
        if (fh[offset].reid == reid)
        {
            fhs[num++] = &fh[offset];
            if (fh[offset].id == reid
                || (!strcmp(fh[offset].filename, file))
                || num == TOPIC_MAX)
                break;
        }
    if (num == 0)
    {
        cgi_head();
        html_head();
        print_style();
        unmap_board_dir();
        http_quit_print("该主题不存在");
    }
    if (loginok)
        user_enter_board(board);
    page_max =
        (num % page_size) ? ((num / page_size) + 1) : (num / page_size);
    cgi_head();
    html_title_head(fhs[num - 1]->title);
    print_style();
    printf("<center>\n");

    printf
        ("<font class=\"title\">%s - 主题文章阅读　　</font>[讨论区: %s]<hr>",
         BBSNAME, board);
    printf
        ("本主题共有 %d 篇文章，分 %d 页, 当前显示第 %d 页 [<a href=\"/bbstdoc?board=%s\">返回讨论区</a>]<br />",
         num, page_max, page, board);
    if (num > page_size || page_size == TPAGE_SIZE_ALL)
    {
        if (page_size == TPAGE_SIZE_ALL)
        {
            printf
                ("<a href=\"/bbstcon?board=%s&reid=%s&file=%s&page=1\">返回分页模式</a>",
                 board, reid_str, fhs[num - 1]->filename);
        }
        else
        {
            if (page > 1)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>上一页</a>",
           			board, reid_str, fhs[num - 1]->filename, page - 1);
            }
            if( page_max > 9)
            {
                printf("<br>");
            }
            for (i = 1; i <= page_max; i++)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>%d</a>&nbsp;&nbsp;",
           			board, reid_str, fhs[num - 1]->filename, i, i);
                if(i%10==0 && i!=page_max)
                {
                    printf("<br>");
                }
            }
            if( page_max > 9)
            {
                printf("<br>");
            }
            if (page > 0 && page < page_max)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>下一页</a>",
           			board, reid_str, fhs[num - 1]->filename, page + 1);
            }

            printf
                ("<a href=\"/bbstcon?board=%s&reid=%s&file=%s&showall=true\">全部显示</a>",
                 board, reid_str, fhs[num - 1]->filename);
        }
    }
    for (offset = num - (page - 1) * page_size - 1;
         offset >= 0 && count > 0; --offset)
    {
        show_file(board, fhs[offset]);
        count--;
    }
    printf
        ("本主题共有 %d 篇文章，分 %d 页, 当前显示第 %d 页 [<a href=\"/bbstdoc?board=%s\">返回讨论区</a>]<br />",
         num, page_max, page, board);
    if (num > page_size || page_size == TPAGE_SIZE_ALL)
    {
        if (page_size == TPAGE_SIZE_ALL)
        {
            printf
                ("<a href=\"/bbstcon?board=%s&reid=%s&file=%s&page=1\">返回分页模式</a>",
                 board, reid_str, fhs[num - 1]->filename);
        }
        else
        {
            if (page > 1)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>上一页</a>",
           			board, reid_str, fhs[num - 1]->filename, page - 1);
            }
            if(page_max > 9)
            {
                printf("<br>");
            }
            for (i = 1; i <= page_max; i++)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>%d</a>&nbsp;&nbsp;",
           			board, reid_str, fhs[num - 1]->filename, i, i);
                if(i%10==0 && i!=page_max)
                {
                    printf("<br>");
                }
            }
            if(page_max > 9)
            {
                printf("<br>");
            }
            if (page > 0 && page < page_max)
            {
    			printf
                   ("<a href='bbstcon?board=%s&reid=%s&file=%s&page=%d'>下一页</a>",
           			board, reid_str, fhs[num - 1]->filename, page + 1);
            }

            printf
                ("<a href=\"/bbstcon?board=%s&reid=%s&file=%s&showall=true\">全部显示</a>",
                 board, reid_str, fhs[num - 1]->filename);
        }
    }

    printf("<br />[<a href='javascript:history.go(-1)'>返回上一页</a>]");
    printf("[<a href=bbsdoc?board=%s>本讨论区(一般模式)</a>]\n", board);
    printf("<br/>\n");
    printf("<a href=\"javascript:void(0)\" onclick=\"jumpUrl('kaixin')\"><img src=\"fav/kaixin001.ico\" height=16 width=16 title=\"分享到开心\" border=\"0\"></a>");
    printf("<a href=\"javascript:void(0)\" onclick=\"jumpUrl('renren')\"><img src=\"fav/renren.png\" height=16 width=16 title=\"分享到人人\" border=\"0\"></a>");
    printf("<a href=\"javascript:void(0)\" onclick=\"jumpUrl('douban')\"><img src=\"fav/douban.ico\" height=16 width=16 title=\"分享到豆瓣\" border=\"0\"></a>");
    printf("<a href=\"javascript:void(0)\" onclick=\"jumpUrl('sina')\"><img src=\"fav/sina.ico\" height=16 width=16 title=\"分享到新浪\" border=\"0\"></a>");
    printf("<a href=\"javascript:void(0)\" onclick=\"jumpUrl('qzone')\"><img src=\"fav/qq.png\" height=16 width=16 title=\"分享到QQ校友\" border=\"0\"></a>");
    printf("</center>\n");
    unmap_board_dir();
    printf("</html>");
    return 0;
}
