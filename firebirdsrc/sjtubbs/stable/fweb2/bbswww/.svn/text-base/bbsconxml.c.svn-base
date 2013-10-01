#include "www.h"
#include "bbs.h"

int bbsconxml_main()
{
    char buf[512], board[80], file[80], filename[80];

    struct fileheader *pfh;
    int num;
    FILE *fpcache;
    struct stat originfile, cachefile;
    char cachefilename[80];
    int cachestatus;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();

        http_quit_print("错误的讨论区");
    }
    printf("Connection: close\n");
    printf("Content-type: text/xml;charset=gb2312\n\n");
    strcpy(board, getbcache(board)->filename);

    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");
    if (map_board_dir(board, ".DIR") == 0)
        http_quit_print("此讨论区不存在或者为空");
    pfh = find_in_map_dir(file);
    num = get_offset_of_mapdir();
    if (NULL == pfh)
    {
        unmap_board_dir();
        http_quit_print("此文不存在");
    }

    brc_init(currentuser.userid, board);
    brc_add_read(pfh->filename);
    brc_update(currentuser.userid, board);

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
    printf
        ("<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n<!--  generator=\"fweb/0.1\" -->\n");
    printf("<post><![CDATA[ \n<pre>\n");
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
    printf("\n</pre>\n]]></post>\n");

    unmap_board_dir();
    return 0;
}
