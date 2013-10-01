#include "www.h"

static void b_usage(char *board, int stay)
{
    char buf[256];

    sprintf(buf, "%s USE %-20.20s Stay: %5d (%s)\n", Ctime(time(0)) + 4,
            board, stay, currentuser.userid);
    f_append("use_board", buf);
    sprintf(buf, "%24.24s %s %s W_Enter\n", Ctime(time(0)),
            currentuser.userid, board);
    if (has_BM_perm(&currentuser, board))
        f_append("bm2.log", buf);
}

int bbsdocdel_main()
{
    FILE *fp;
    char board[80], dir[80];
    struct shortfile *x1;
    struct fileheader x;
    int i, start, total, width = 720;
    int currentpoint;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客无此权限");
    if (my_font_mode == 0)
        width = 670;
    strsncpy(board, getparm("board"), 32);

    currentpoint = atoi(getparm("currentpoint"));
    if (currentpoint < 0)
    {
        currentpoint = 0;
    }

    x1 = getbcache(board);
    if (x1 == 0)
        http_quit_print("错误的讨论区");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("您没有权限访问本页");

    printf("<body>");
    printf
        ("<form name=form1 action=bbsdocdel?board=%s method=post onsubmit='this.start.value-=1'>\n",
         board);
    printf("<center>\n");
    printf("<table><tr><td>");
    printf
        ("<td><font style=\"font-size:18px;\"><b>%s(%s)</b></font></table>\n",
         x1->filename, x1->title + 11);
    sprintf(dir, "boards/%s/.RETURNDIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("错误的讨论区目录");
    total = file_size(dir) / sizeof(struct fileheader);

    start = atoi(getparm("start"));
    if (my_t_lines < 10 || my_t_lines > 40)
        my_t_lines = 20;

    if (currentpoint < 0)
    {
        currentpoint = my_t_lines + start;
    }
    if (currentpoint >= my_t_lines)
    {
        currentpoint = my_t_lines - 1;
    }

    if (strlen(getparm("start")) == 0 || start > total - my_t_lines)
        start = total - my_t_lines;
    if (start < 0)
        start = 0;
    if (strlen(getparm("start")) == 0)
        b_usage(board, 33);
    brc_init(currentuser.userid, board);
    printf("<nobr>\n");

    printf
        ("<script language=\"javascript\" src=\"/key_hl_delforumlist.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", currentpoint);
    printf("var totalid = %d;\n", my_t_lines - 1);
    printf("var toptotal = 0;\n");
    printf("var arrayid = new Array(totalid);\n");
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var total = %d;\n", total);
    printf("if ( total < totalid ) \n\ttotalid = total - 1;\n");
    printf("\n-->\n</script>\n");

    printf
        ("<table width=%d><tr style='height:14px'><td align=left width=50%%> 板主: %s",
         width, userid_str(x1->BM));
    printf("<td align=right width=25%%>");
    if (start > 0)
        printf("<a href=bbsdocdel?board=%s&start=%d>↑</a>,", board,
               start - my_t_lines);
    if (start < total - my_t_lines)
        printf("<a href=bbsdocdel?board=%s&start=%d>↓</a>,", board,
               start + my_t_lines);

    printf("<td align=right width=25%%>");
    printf
        ("<input type=submit value=跳转到> 第 <input type=text name=start size=4> 篇");
    printf("</table>\n");
    show_bm_words(board, width);
    printf("<hr>\n");
    if (total <= 0)
    {
        fclose(fp);
        http_quit_print("本讨论区回收站目前没有文章");
    }
    printf("<table width=%d>\n", width);

    printf
        ("<tr bgcolor='e8e8e8'><td>序号<td>状态<td>作者<td>日期<td width=12>&nbsp;<td>标题<td>人气\n");
    fseek(fp, start * sizeof(struct fileheader), SEEK_SET);
    for (i = 0; i < my_t_lines; i++)
    {
        char filename[80];
        char *ptr, *font1 = "", *font2 = "";

        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        ptr = flag_str2(x.accessed[0]);
        if (ptr[0] == 'N')
        {
            font1 = "<font color=#909090>";
            font2 = "</font>";
        }
        sprintf(filename, "boards/%s/%s/%s", board,
                getfolderstr(x.filename), x.filename);
        printf("<tr><td>%d<td>%s%s%s<td>%s", start + i + 1, font1, ptr,
               font2, userid_str(x.owner));
        printf("<td>%12.12s", Ctime(atoi(x.filename + 2)) + 4);

        printf("<td><span id=\"arrow%d\">", i);
        if (i == currentpoint)
        {
            printf("&gt;");
        }
        printf("</span>");

        printf
            ("<td><a href=bbscondel?board=%s&file=%s&num=%d&start=%d>%s%s%s </a>%s",
             board, x.filename, start + i, start,
             ((x.accessed[0] & FILE_NOREPLY) || (x.accessed[1] & FILE_NOREPLY2)) ?
             "<font color = red><u>x</u></font>" : "", strncmp(x.title,
                                                               "Re: ",
                                                               4) ? "○ " :
             "", void1(nohtml2(x.title)), eff_size(filename));

        printf("<script language=\"javascript\">\n");
        printf("arrayid[%d] = \"board=%s&file=%s&num=%d&start=%d\";\n", i,
               board, x.filename, start + i, start);
        printf("</script>\n");

        printf("<td><font color=%s>%d</font>\n",
               *(int *) (x.title + 73) > 99 ? "red" : "black",
               *(int *) (x.title + 73));
    }

    printf("</table><hr>\n");

    printf("<a href='bbsmdoc?board=%s'>返回管理模式</a> ", board);
    printf("<a href='javascript:location=location'>刷新</a> ");
    if (start > 0)
    {
        printf("<a href=bbsdocdel?board=%s&start=0>首页[Home]</a> ",
               board);
        printf("<a href=bbsdocdel?board=%s&start=%d>上一页[PageUp]</a> ",
               board, start - my_t_lines);
    }
    if (start < total - my_t_lines)
    {
        printf("<a href=bbsdocdel?board=%s&start=%d>下一页[PageDown]</a> ",
               board, start + my_t_lines);
        printf("<a href=bbsdocdel?board=%s>末页[End]</a> ", board);
    }
    fclose(fp);

    printf("<br>\n");
    printf("键盘操作: [K] 上移/上翻页 &nbsp; [J] 下移/下翻页 &nbsp; ");
    printf("[→]/[Enter] 看帖子 &nbsp; ");
    printf("[PageUp] 上翻页 &nbsp; [PageDown] 下翻页 ");
    printf("[O]恢复文章 &nbsp; ");

    printf("</form>\n</body>\n</html>\n");
    return 0;
}
