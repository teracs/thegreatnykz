#include "www.h"

static int
search(char *id, char *pat, char *pat2, char *pat3, int dt, int dt2, int pagesize, int page, int theme)
{
    FILE *fp = 0;
    char board[256], dir[256];
    int total, now = time(0), i, sum = 0, n, t;
	int board_flag = 0;// if certain board contains article this would be 1
    struct fileheader x;

    printf("\n<script language=\"javascript\">\n");
    printf("function gotopage(page){\n");
    printf("document.getElementsByName('page')[0].value = page;\n");
	printf("document.getElementById(\"findform\").submit();\n");
    printf("}</script>\n");
    printf("<form id=\"findform\" action=bbsfind method=post>\n");
    printf("<input type=\"hidden\" name=\"title\" value=\"%s\"/>\n", pat);
    printf("<input type=\"hidden\" name=\"title2\" value=\"%s\"/>\n", pat2);
    printf("<input type=\"hidden\" name=\"title3\" value=\"%s\"/>\n", pat3);
    printf("<input type=\"hidden\" name=\"page\" value=\"%d\"/>\n", page);
    printf("<input type=\"hidden\" name=\"pagesize\" value=\"%d\"/>\n", pagesize);
    printf("<input type=\"hidden\" name=\"flag\" value=\"1\"/>\n");
    printf("<input type=\"hidden\" name=\"day\" value=\"%d\"/>", dt / 86400);
    printf("<input type=\"hidden\" name=\"day2\" value=\"%d\"/>", dt2 / 86400);
    printf("<input type=\"hidden\" name=\"user\" value=\"%s\"/>", id);
    
    printf
        ("<center><font class=\"title\">%s - 站内文章查询结果</font></center> <br>\n",
         BBSNAME);
    printf("作者: %s ", id);
    printf("标题含有: '%s' ", nohtml(pat));
    if (pat2[0])
        printf("和 '%s' ", nohtml(pat2));
    if (pat3[0])
        printf("不含 '%s'", nohtml(pat3));
    if (dt > dt2)
    {
        i = dt;
        dt = dt2;
        dt2 = i;
    }    
    printf("时间: 据今 %d-%d 天<br><hr>\n", dt / 86400, dt2 / 86400);
    for (i = 0; i < MAXBOARD; i++)
    {
        total = 0;
        strcpy(board, shm_bcache->bcache[i].filename);
        if (!has_read_perm(&currentuser, board))
            continue;
        sprintf(dir, "boards/%s/.DIR", board);
        if (fp != 0)
            fclose(fp);
        fp = fopen(dir, "r");
        if (fp == 0)
            continue;
        n = 0;
        while (1)
        {
            n++;
            if (fread(&x, sizeof(x), 1, fp) <= 0)
                break;
            t = atoi(x.filename + 2);
            if (id[0] != 0 && strcasecmp(x.owner, id))
                continue;
            if (pat[0] && !strcasestr(x.title, pat))
                continue;
            if (abs(now - t) < dt)
                continue;
            if (abs(now - t) > dt2)
                continue;
            if (pat2[0] && !strcasestr(x.title, pat2))
                continue;
            if (pat3[0] && strcasestr(x.title, pat3))
                continue;
            if ((sum >= (pagesize * page)) && (sum < (pagesize * (page + 1)))) {
                if (board_flag == 0) {
                    printf("<table class=show_border width=610>\n");
                    board_flag = 1;
                }
                if (!theme) {
                    printf("<tr><td>%d<td><a href=bbsqry?userid=%s>%s</a>", n,
                           x.owner, x.owner);
                    printf("<td>%6.6s", Ctime(atoi(x.filename + 2)) + 4);
                    printf
                          ("<td><a href=\"bbscon?board=%s&file=%s&num=%d\" target=\"_blank\">%s</a>\n",
                           board, x.filename, n - 1, nohtml(x.title));
                }    
                else {
                    if (x.id != x.reid)
                        continue;
                    printf("<tr><td>%d<td><a href=bbsqry?userid=%s>%s</a>", n,
                           x.owner, x.owner);
                    printf("<td>%6.6s", Ctime(atoi(x.filename + 2)) + 4);
                    printf
                          ("<td><a href=\"bbstcon,board,%s,reid,%d.html\" target=\"_blank\">%s</a>\n",
                           board, x.reid, nohtml(x.title));
                }
                total++;
            }	
            sum++;
            if (sum > 1999) {
               printf("</table> ....");
               fclose(fp);
               return 0;
            }
        }
        if (total == 0)
            continue;
        printf("</table>\n");
        board_flag = 0;
        printf("<br>以上%d篇来自 <a href=bbsdoc?board=%s>%s</a><br><br>\n",
               total, board, board);
    }
    if (fp != 0)
        fclose(fp);
    printf("一共找到%d篇文章符合查找条件<br>\n", sum);
    if ( sum > pagesize )
    {
	for (i = 0; i < ((sum - 1) / pagesize + 1); i++)
	{
	    if (page == i)
	    {
		printf("%d&nbsp;", (i + 1));
	    }
	    else
	    {
		printf("<a href=\"javascript:void(0)\" onclick=\"gotopage(%d)\">%d<a/>&nbsp;", i, (i + 1));
	    }
	}
    }
    printf("</form>");
    return 0;
}

int bbsfind_main()
{
    char user[32], title3[80], title[80], title2[80];
    int day, day2, flag, theme;
    int pagesize, page;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客不能查询网友大作");
    if (get_load(0) > 20.0)
        http_quit_print
            ("抱歉，因系统负荷过高目前无法使用本功能，请稍后再试");
    strsncpy(user, getparm("user"), 13);
    strsncpy(title, getparm("title"), 50);
    strsncpy(title2, getparm("title2"), 50);
    strsncpy(title3, getparm("title3"), 50);
    day = atoi(getparm("day"));
    day2 = atoi(getparm("day2"));
    flag = atoi(getparm("flag"));
    theme = atoi(getparm("theme"));
    pagesize = atoi(getparm("pagesize"));
    page = atoi(getparm("page"));

    if (pagesize == 0)
    {
	pagesize = 100;
    }
    if (page < 0)
    {
	page = 0;
    }
	    
    if (!flag)
    {
        printf("<center>\n");
        printf("<hr>\n");
        printf("<table>\n");
        printf("<form action=bbsfind method=post>\n");
        printf("<input type=hidden name=flag value=1>\n");
        printf("<input type=hidden name=theme value=0>\n");
        printf
            ("<tr><td>文章作者: </td><td><input maxlength=12 size=12 type=text name=user> (不填查找所有作者)</td></tr>\n");
        printf
            ("<tr><td>标题含有: </td><td><input maxlength=60 size=20 type=text name=title>");
        printf
            ("&nbsp;AND <input maxlength=60 size=20 type=text name=title2></td></tr>\n");
        printf
            ("<tr><td>标题不含: </td><td><input maxlength=60 size=20 type=text name=title3></td></tr>\n");
        printf
            ("<tr><td>查找据今: </td><td><input maxlength=5 size=5 type=text name=day value=0> 至\n");
        printf
            ("<input maxlength=5 size=5 type=text name=day2 value=7> 天以内的文章</td></tr>\n");
	printf
	    ("<tr><td>每页显示: </td><td><input maxlength=5 size=4 type=text name=\"pagesize\" value=\"%d\"/>篇</td></tr>\n", pagesize);
        printf("</table><hr>\n");
        printf("<input type=submit value=提交查询>\n");
    }
    else
    {
        if (day > 9999)
            day = 9999;
        if (day2 > 9999)
            day2 = 9999;
        search(user, title, title2, title3, day * 86400, day2 * 86400, pagesize, page, theme);
    }
    printf("</html>");
    return 0;
}
