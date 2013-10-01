#include "www.h"

int bbsmailcon_main()
{
    FILE *fp;
    char buf[512], dir[80], file[80], path[80], *ptr, *id;
    char prefilename[STRLEN], nextfilename[STRLEN], refilename[STRLEN];
    char title2[80], owner[STRLEN];
    struct fileheader x;
    int num, total;

    init_all();
    strsncpy(file, getparm("file"), 32);
    num = atoi(getparm("num"));
    printf
        ("<script>if(!top.get_html) top.f4.location=top.f4.location</script>\n");
    printf("<center>\n");
    id = currentuser.userid;
    printf
        ("<font class=\"title\">%s - 阅读信件　　</font>[使用者: %s]<hr>\n",
         BBSNAME, id);
    if (strncmp(file, "M.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(id[0]), toupper(id[1]), id);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_quit_print("错误的参数3");
    printf
        ("[<a onclick='return confirm(\"你真的要删除这封信吗?\")' href=bbsdelmail?file=%s> 删除</a>]",
         file);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_quit_print("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbsmailcon?file=%s&num=%d>上一篇</a>]",
               prefilename, num - 1);
    }
    printf("[<a href=bbsmail>返回信件列表</a>]");
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbsmailcon?file=%s&num=%d>下一篇</a>]",
               nextfilename, num + 1);
    }
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        if (fread(&x, sizeof(x), 1, fp) > 0)
        {
            x.accessed[0] |= FILE_READ;
            fseek(fp, sizeof(x) * num, SEEK_SET);
            fwrite(&x, sizeof(x), 1, fp);
            strcpy(owner, x.owner);
            ptr = strtok(owner, " (");
            if (ptr == 0)
                ptr = " ";
            strcpy(title2, x.title);
            if (strncmp(x.title, "Re:", 3))
                sprintf(title2, "Re: %s", x.title);
            title2[60] = 0;
            strcpy(refilename, x.filename);
            printf
                ("[<a href='bbspstmail?userid=%s&file=%s&title=%s'>回信</a>]",
                 ptr, refilename, title2);
        }
    }
    if (num >= 0 && num < total)
    {
        ptr = strtok(owner, " (");
        if (ptr == 0)
            ptr = " ";
        printf("[<a href='bbsmwd?file=%s'>转寄</a>]", x.filename);
    }

    fclose(fp);
    printf("<table class=show_border width=610>\n");

    printf("<tr><td>\n<pre>");
    sprintf(path, "mail/%c/%c/%s/%s", toupper(id[0]), toupper(id[1]), id,
            file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_quit_print("本文不存在或者已被删除");
    while (1)
    {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "寄信人: ", 8))
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
                printf("寄信人: %s%s", userid_str(id), s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, ": ", 2))
            printf("<font color=808080>");
        hhprintf("%s", void1(buf));
        if (!strncmp(buf, ": ", 2))
            printf("</font>");
    }
    fclose(fp);
    printf("</pre>\n</table><hr>\n");
    printf
        ("[<a onclick='return confirm(\"你真的要删除这封信吗?\")' href=bbsdelmail?file=%s> 删除</a>]",
         file);
    if (num > 0)
        printf("[<a href=bbsmailcon?file=%s&num=%d>上一篇</a>]",
               prefilename, num - 1);
    printf("[<a href=bbsmail>返回信件列表</a>]");
    if (num < total - 1)
        printf("[<a href=bbsmailcon?file=%s&num=%d>下一篇</a>]",
               nextfilename, num + 1);
    if (num >= 0 && num < total)
    {
        ptr = strtok(owner, " (");
        if (ptr == 0)
            ptr = " ";
        printf
            ("[<a href='bbspstmail?userid=%s&file=%s&title=%s'>回信</a>]",
             ptr, refilename, title2);
    }
    if (num >= 0 && num < total)
    {
        ptr = strtok(owner, " (");
        if (ptr == 0)
            ptr = " ";
        printf("[<a href=bbsmwd?file=%s>转寄</a>]", x.filename);
    }
    printf("</center>\n");
    printf("</html>");
    return 0;
}
