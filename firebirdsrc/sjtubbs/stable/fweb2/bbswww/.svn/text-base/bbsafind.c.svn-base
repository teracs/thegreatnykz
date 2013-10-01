#include "www.h"

static void show_form2(char *board);

int bbsafind_main()
{
    FILE *fp;
    int total = 0, type;
    char dir[80], title[80], title2[80], title3[80], board[80], buf[256];
    struct shortfile *brd;

    init_all();
    printf
        ("<center><font class=\"title\">%s - 精华区文章搜索</font><hr><br>\n",
         BBSNAME);
    type = atoi(getparm("type"));
    strsncpy(board, getparm("board"), 30);
    if (type == 0)
    {
        show_form2(board);
        return 0;
    }
    strsncpy(title, getparm("title"), 60);
    strsncpy(title2, getparm("title2"), 60);
    strsncpy(title3, getparm("title3"), 60);
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("错误的讨论区");
    strcpy(board, brd->filename);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区名称");
    sprintf(dir, "%s/0Announce%s/_index2.txt", BBSHOME,
            anno_path_of(board));
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("讨论区错误或没有目前文章");
    printf("查找'%s'板精华区内, 标题含: '%s' ", board, nohtml(title));
    if (title2[0])
        printf("和 '%s' ", nohtml(title2));
    if (title3[0])
        printf("不含 '%s' ", nohtml(title3));
    printf(", 最后更新: %s\n", Ctime(file_time(dir)) + 4);
    printf("<table width=610>\n");
    printf("<tr bgcolor=e8e8e8><td>类型<td>标题\n");
    while (1)
    {
        int type = 0;
        char *t, path3[80] = "", tmp[80];

        if (fgets(buf, 256, fp) == 0)
            break;
        if (sscanf(buf, "%d %s %s", &type, path3, tmp) != 3)
            continue;
        strtok(buf, " \r\n");
        strtok(0, " \r\n");
        t = strtok(0, " \r\n");
        if (title[0] && !strcasestr(t, title))
            continue;
        if (title2[0] && !strcasestr(t, title2))
            continue;
        if (title3[0] && strcasestr(t, title3))
            continue;
        total++;
        printf("<tr><td>[%s]", type == 0 ? "文件" : "目录");
        printf("<td><a href=%s?board=%s&path=%s%s>%40.40s </a>\n",
               type == 0 ? "bbsanc" : "bbs0an",
               board, anno_path_of(board), path3 + 1, t);
        if (total >= 999)
            break;
    }
    fclose(fp);
    printf("</table>\n");
    printf("<br>共找到 %d 篇文章符合条件", total);
    if (total > 999)
        printf("(匹配结果过多, 省略第1000以后的查询结果)");
    printf("<br>\n");
    printf
        ("[<a href=bbsdoc?board=%s>返回本讨论区</a>] [<a href='javascript:history.go(-1)'>返回上一页</a>]",
         board);
    printf("\n</html>");
    return 0;
}

static void show_form2(char *board)
{
    printf("<table><form action=bbsafind method=post>\n");
    printf("<inpurt type=hidden name=type value=1>\n");
    printf
        ("<tr><td>板面名称: <input type=text maxlength=24 size=24 name=board value='%s'><br>\n",
         board);
    printf
        ("<tr><td>标题含有: <input type=text maxlength=50 size=20 name=title> AND ");
    printf("<input type=text maxlength=50 size=20 name=title2>\n");
    printf
        ("<tr><td>标题不含: <input type=text maxlength=50 size=20 name=title3>\n");
    printf
        ("<tr><td align=center><input type=submit value=递交查询结果>\n");
    printf("</form></table>");
    printf("[<a href='bbsdoc?board=%s'>返回本讨论区</a>]", board);
    printf("\n</heml>");
    return;
}
