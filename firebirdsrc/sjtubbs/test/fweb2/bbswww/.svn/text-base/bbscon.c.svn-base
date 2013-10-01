#include "www.h"
#include "bbs.h"

int bbscon_main()
{

    char buf[512], board[80], file[80], filename[80];

    struct fileheader *pfh, *pre, *next;
    int num;
    FILE *fpcache;
    struct stat originfile, cachefile;
    char cachefilename[80];
    int cachestatus, no_re;

	struct anony_item item;
    char *userid = NULL;

    no_re = 0;
    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("错误的讨论区");
    }
    else
    {
        if (is_normal_board)
            cache_age = 5 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    printf("<body>");
    printf("<center>\n");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    printf
        ("<font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]<hr>",
         BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");
    if (map_board_dir(board, DOT_DIR) == 0)
        http_quit_print("此讨论区不存在或者为空");
    pfh = find_in_map_dir(file);
    num = get_offset_of_mapdir();
    if (pfh == NULL)
    {        
        unmap_board_dir();
        if (map_board_dir(board, RETURN_DIR) == 0)
            http_quit_print("此文不存在");
        pfh = find_in_map_dir(file);
        if (pfh == NULL)
        {
            unmap_board_dir();
            http_quit_print("此文不存在");
        }
        if (!strncmp(pfh->owner, "Anony.", 6))
            if (search_in_anonyindex(NULL, pfh->owner + 6, &item) == -1)
                userid = NULL;
            else
                userid = item.userid;
        else
            userid = pfh->owner;
        if (strcmp(userid, currentuser.userid) && !has_BM_perm(&currentuser, board))
        {
            unmap_board_dir();
            http_quit_print("此文不存在");
        }
        num = get_offset_of_mapdir();
    }
	if (loginok)
		user_enter_board(board);

    printf("[<a href=bbsfwd?board=%s&file=%s>转寄/推荐</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>转贴</a>]", board, file);
    printf
        ("[<a onclick='return confirm(\"你真的要删除本文吗?\")' href=bbsdel?board=%s&file=%s>删除</a>]",
         board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>修改</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&num=%d>设置可RE属性</a>]",
           board, file, num);
    pre = pre_in_map_dir();
#ifdef STATIC_URL
    if (pre != NULL)
        printf("[<a href=bbscon,board,%s,file,%s.html>上一篇</a>]",
               board, pre->filename);
    printf("[<a href=\"bbsdoc,board,%s,page,%d.html\">返回讨论区</a>]",
           board, num / PAGE_SIZE);
    printf("[<a onclick='javascript:switch_style();return false;' href=\"#\">切换布局</a>]");
    next = next_in_map_dir();
    if (NULL != next)
        printf("[<a href=bbscon,board,%s,file,%s.html>下一篇</a>]",
               board, next->filename);
#else
    if (pre != NULL)
        printf("[<a href=bbscon?board=%s&file=%s>上一篇</a>]",
               board, pre->filename);
    printf("[<a href=\"bbsdoc?board=%s&page=%d\">返回讨论区</a>]", board,
           board, get_offset_of_mapdir() / PAGE_SIZE);
    next = next_in_map_dir();
    printf("[<a onclick='javascript:switch_style();return false;' href=\"#\">切换布局</a>]");
    if (NULL != next)
        printf("[<a href=bbscon?board=%s&file=%s>下一篇</a>]",
               board, next->filename);
#endif
    brc_init(currentuser.userid, board);
    brc_add_read(pfh->filename);
    brc_update(currentuser.userid, board);

    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>回文章</a>]",
               board, file, pfh->reid);
    printf("[<a href='bbstfind0?board=%s&reid=%d'>同主题列表</a>]", board,
           pfh->reid);
#ifdef STATIC_URL
    printf("[<a href='bbstcon,board,%s,reid,%d.html'>同主题阅读</a>]",
           board, pfh->reid);
    printf
        ("[<a href='bbstcon,board,%s,reid,%d,file,%s.html'>从这里展开</a>]",
         board, pfh->reid, pfh->filename);
#else
    printf("[<a href='bbstcon?board=%s&reid=%d'>同主题阅读</a>]", board,
           pfh->reid);
    printf("[<a href='bbstcon?board=%s&reid=%d&file=%s'>从这里展开</a>]",
           board, pfh->reid, pfh->filename);
#endif

    printf("<table class=show_border width=610>\n");

    printf("<tr><td>\n<pre>");
    sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(file), file);
    sprintf(cachefilename, "cache/%s/%s/%s", board, getfolderstr(file),
            file);
    if (stat(filename, &originfile) == -1)
    {
        unmap_board_dir();
        http_quit_print("本文不存在或者已被删除");
    }
    if (stat(cachefilename, &cachefile) == -1)
        cachestatus = 0;
    else
    {
        if (cachefile.st_mtime > originfile.st_mtime)
            cachestatus = 1;
        else
            cachestatus = 0;
    }

    if (cachestatus == 0)
    {
        do_cache(filename, cachefilename);
        cachestatus = 1;
    }
    fpcache = fopen(cachefilename, "r");

    while (1)
    {
        int r;

        r = fread(buf, 1, 512, fpcache);
        if (r <= 0)
            break;
        fwrite(buf, 1, r, stdout);
    }
    fclose(fpcache);

    printf("</pre>\n</table><hr>\n");
    printf("[<a href=bbsfwd?board=%s&file=%s>转寄/推荐</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>转贴</a>]", board, file);
    printf
        ("[<a onclick='return confirm(\"你真的要删除本文吗?\")' href=bbsdel?board=%s&file=%s>删除</a>]",
         board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>修改</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&num=%d>设置可RE属性</a>]",
           board, file, num);
#ifdef STATIC_URL
    if (NULL != pre)
        printf("[<a href=bbscon,board,%s,file,%s.html>上一篇</a>]",
               board, pre->filename);

    printf("[<a href=\"bbsdoc,board,%s,page,%d.html\">返回讨论区</a>]",
           board, get_offset_of_mapdir() / PAGE_SIZE);
    printf("[<a onclick='javascript:switch_style();return false;' href=\"#\">切换布局</a>]");

    if (NULL != next)
        printf("[<a href=bbscon,board,%s,file,%s.html>下一篇</a>]",
               board, next->filename);
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>回文章</a>]",
               board, file, pfh->reid);
    printf("[<a href='bbstfind0?board=%s&reid=%d'>同主题列表</a>]", board,
           pfh->reid);
    printf("[<a href='bbstcon,board,%s,reid,%d.html'>同主题阅读</a>]",
           board, pfh->reid);
    printf
        ("[<a href='bbstcon,board,%s,reid,%d,file,%s.html'>从这里展开</a>]",
         board, pfh->reid, pfh->filename);
#else
    if (NULL != pre)
        printf("[<a href=bbscon?board=%s&file=%s>上一篇</a>]",
               board, pre->filename);

    printf("[<a href=\"bbsdoc?board=%s&page=%d\">返回讨论区</a>]", board,
           get_offset_of_mapdir() / PAGE_SIZE);
    printf("[<a onclick='javascript:switch_style();return false;' href=\"#\">切换布局</a>]");

    if (NULL != next)
        printf("[<a href=bbscon?board=%s&file=%s>下一篇</a>]",
               board, next->filename);
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>回文章</a>]",
               board, file, pfh->reid);
    printf("[<a href='bbstfind0?board=%s&reid=%d'>同主题列表</a>]", board,
           pfh->reid);
    printf("[<a href='bbstcon?board=%s&reid=%d'>同主题阅读</a>]", board,
           pfh->reid);
    printf("[<a href='bbstcon?board=%s&reid=%d&file=%s'>从这里展开</a>]",
           board, pfh->reid, pfh->filename);
#endif

    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    unmap_board_dir();
    printf("\n</html>");
    return 0;

}
