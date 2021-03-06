#include "www.h"

int bbsgcon_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];
    struct fileheader x;
    int num, total;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    num = atoi(getparm("num"));
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    printf("<center><font class=\"title\">%s - 文章阅读　　</font>[讨论区: %s]</center><hr>", BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数2");
    sprintf(dir, "boards/%s/.DIGEST", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("此讨论区不存在或者为空");
    printf("[<a href=bbssec>分类讨论区</a>]");
    printf("[<a href=bbsall>全部讨论区</a>]");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbsgcon?board=%s&file=%s&num=%d>上一篇</a>]",
               board, prefilename, num - 1);
    }
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbsgcon?board=%s&file=%s&num=%d>下一篇</a>]",
               board, nextfilename, num + 1);
    }
    if (num > 0 && num <= total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        (*(int *) (x.title + 73))++;
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
    }
    fclose(fp);
    strcpy(title, x.title);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("[<a href='bbstfind?board=%s&title=%s'>同主题阅读</a>]\n",
           board, void1(ptr));
    printf
	    ("<table class=show_border width=610>\n");
    //printf("<tr><td>\n<pre style='font-size:14px'>");
    printf("<tr><td>\n<pre>");
    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("本文不存在或者已被删除");
    while (1)
    {
        if (fgets(buf, 512, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre>\n</table><hr>\n");
    printf("[<a href=bbssec>分类讨论区</a>]");
    printf("[<a href=bbsall>全部讨论区</a>]");
    if (num > 0)
        printf("[<a href=bbsgcon?board=%s&file=%s&num=%d>上一篇</a>]",
               board, prefilename, num - 1);
    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
        printf("[<a href=bbsgcon?board=%s&file=%s&num=%d>下一篇</a>]",
               board, nextfilename, num + 1);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("[<a href='bbstfind?board=%s&title=%s'>同主题阅读</a>]\n",
           board, void1(ptr));
    printf("</center>\n");
    http_quit();
}
