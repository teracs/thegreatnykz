#include "www.h"
#include <time.h>

int bbsrss_main()
{
    FILE *fpcache, *fppost_cache;
    char board[80], dir[80], cachefilename[80];
    struct shortfile *x1;
    struct fileheader x;
    int pos, total, count, cache = 1;
    struct fileheader *fileheaders;

    char boardLink[128], boardDes[128], buf[1024], filename[80],
        cachename[80];
    struct stat latestfile, cachefile;
    char date[128];
    int cachestatus;
    char *p, *q;

    silence = 1;
    init_all();
    printf("Connection: close\n");
    printf("Content-type: text/xml;charset=GB2312\n\n");
    fflush(0);

    strsncpy(board, getparm("board"), 32);
    x1 = getbcache(board);
    if (x1 == 0)
        http_quit_print("错误的讨论区1");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区2");

    sprintf(dir, "boards/%s/.DIR", board);
    total = file_size(dir) / sizeof(x);
    if (total == 0)
        http_quit_print("讨论区暂无文章");

    sprintf(cachename, "boards/%s/rss2.xml", board);
    if (stat(cachename, &cachefile) == -1)
        cache = 0;
    else
    {
        if (stat(dir, &latestfile) == -1)
            http_quit_print("fatal error1");
        if (cachefile.st_mtime >= latestfile.st_mtime)
            cache = 1;
        else
            cache = 0;

    }

    if (cache == 0)
    {
        fpcache = fopen(cachename, "w");
        flock(fileno(fpcache), LOCK_EX);
        sprintf(boardLink, "https://" BBSHOST "/bbsdoc?board=%s", board);
        strcpy(boardDes, x1->title + 11);
        fprintf(fpcache,
                "<?xml version=\"1.0\" encoding=\"GB2312\"?>\n<!-- generator=\"fweb/0.1\" -->\n<rss version=\"2.0\">\n	<channel>\n		<title>%s</title>\n		<description> %s </description>\n		<link>%s</link>\n		<generator>%s</generator>\n",
                board, boardDes, boardLink, BBSHOST);

        fileheaders = (struct fileheader *) map_board_dir(board, ".DIR");
        total = get_size_of_mapdir();
        if (fileheaders == NULL)
            http_quit_print("目录错误");
        pos = total;
        count = 0;

        while (pos > 0)
        {
            pos--;
            x = fileheaders[pos];
            if (x.id != x.reid)
                continue;

            count++;
            if (count > 20)
                break;

            sprintf(filename, "boards/%s/%s/%s", board,
                    getfolderstr(x.filename), x.filename);
            if (stat(filename, &latestfile))
                continue;

            strftime(date, 128, "%a,%d %b %Y %H:%M:%S %z",
                     localtime(&latestfile.st_mtime));

            fprintf(fpcache, "		<item>\n");
            fprintf(fpcache, "			<title> %s </title>\n",
                    (void1(nohtml2(x.title))));
            fprintf(fpcache,
                    "			<link> <![CDATA[https://" BBSHOST
                    "/bbstcon,board,%s,reid,%d.html]]> </link>\n",
                    board, x.reid);
            fprintf(fpcache, "			<author> %s </author>\n",
                    x.owner);
            fprintf(fpcache, "			<pubDate> %s </pubDate>\n",
                    date);
            fprintf(fpcache, "			<description>\n");
            fprintf(fpcache, "				<![CDATA[<pre>\n");

            sprintf(cachefilename, "cache/%s/%s/%s", board,
                    getfolderstr(x.filename), x.filename);
            if (stat(cachefilename, &cachefile) == -1)
                cachestatus = 0;
            else
            {
                if (cachefile.st_mtime > latestfile.st_mtime)
                    cachestatus = 1;
                else
                    cachestatus = 0;
            }

            if (cachestatus == 0)
            {
                do_cache(filename, cachefilename);
                cachestatus = 1;
            }
            fppost_cache = fopen(cachefilename, "r");
            while (1)
            {
                if (fgets(buf, 512, fppost_cache) == 0)
                    break;
                q = p = buf;
                while (*p)
                {
                    if (!strncasecmp(p, "SRC=\"", 5))
                    {
                        *(p + 4) = '\0';
                        fprintf(fpcache, "%s\"https://%s", q, BBSHOST);
                        p += 5;
                        q = p;
                        while (*p && *p != '\"')
                            p++;
                    }
                    p++;
                }
                fprintf(fpcache, "%s", q);
            }
            fclose(fppost_cache);

            fprintf(fpcache, "				</pre>]]>\n");
            fprintf(fpcache, "			</description>\n");
            fprintf(fpcache, "		</item>\n");
        }
        fprintf(fpcache, "%s", "</channel></rss>\n");
        flock(fileno(fpcache), LOCK_UN);
        fclose(fpcache);
    }
    unmap_board_dir();

    fpcache = fopen(cachename, "r");
    while (1)
    {
        int r;

        r = fread(buf, 1, 512, fpcache);
        if (r <= 0)
            break;
        fwrite(buf, 1, r, stdout);

    }
    fclose(fpcache);

    fflush(0);
    return 0;
}
