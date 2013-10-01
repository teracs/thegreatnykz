#include "www.h"
#include "bbs.h"

int bbscondel_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];
    struct fileheader x;
    int num, total;
    int swap = 0;
    int no_re = 0;

    int start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);

    start = atoi(getparm("start"));

    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;

    printf
        ("<font class=\"title\">%s - 已被删除文章阅读 [讨论区回收站: %s]</font><hr>",
         BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");

    if (!has_BM_perm(&currentuser, board))
        http_quit_print("对不起，您没有恢复文章的权限");

    sprintf(dir, "boards/%s/.RETURNDIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_quit_print("此讨论区不存在或者为空");

    printf("[<a href=bbsdelreturn?board=%s&file=%s>恢复O</a>]", board,
           file);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_quit_print("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf
            ("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>上一篇K</a>]",
             board, prefilename, num - 1, start);
    }

    printf
        ("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">返回讨论区回收站←</a>]",
         board, start, num - start);

    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf
            ("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>下一篇J</a>]",
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
    printf("<table class=show_border width=610>\n");

    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_quit_print("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    fclose(fp);

    if (strcmp(x.filename, file) != 0)
        http_quit_print("本文不存在或已被恢复");
    sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_quit_print("本文不存在或者已被完全删除");
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

    printf("[<a href=bbsdelreturn?board=%s&file=%s>恢复O</a>]", board,
           file);
    if (num > 0)
        printf
            ("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>上一篇K</a>]",
             board, prefilename, num - 1, start);

    printf
        ("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">返回讨论区回收站←</a>]",
         board, start, num - start);

    printf("[<a href=bbsdoc?board=%s>本讨论区</a>]", board);
    if (num < total - 1)
        printf
            ("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>下一篇J</a>]",
             board, nextfilename, num + 1, start);
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    printf("\n</html>");
    return 0;
}
