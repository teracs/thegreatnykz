#include "www.h"

int bbsedit_main()
{
    FILE *fp;
    int type = 0;
    int i;
    char buf[512], path[512], file[512], board[512];
    struct shortfile *brd;
    struct fileheader *x;

    init_all();
    if (!loginok)
        http_fatal("匆匆过客不能修改文章，请先登录");
    strsncpy(board, getparm("board"), 20);
    type = atoi(getparm("type"));
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("错误的讨论区");
    strcpy(board, brd->filename);
    strsncpy(file, getparm("file"), 20);
    if (!has_post_perm(&currentuser, board))
        http_fatal("错误的讨论区或者您无权在此讨论区发表文章");
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, board))
		http_fatal("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif	
    x = get_file_ent(board, file);
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("错误的参数");
    if (x == 0)
        http_fatal("错误的参数");

    //Bug
    //Bug-description:	the boardmaster cannot modify forum
    //Bug-solution:	let the bm modify it :)
    //Bug-fixer:	hongliang
    //Date:		Nov. 21th, 2003
    //start-hongliang
    if (strcmp(x->owner, currentuser.userid) && !has_BM_perm(&currentuser, board))
        http_fatal("你无权修改此文章");
    //end-hongliang

    printf("<center>%s -- 修改文章 [使用者: %s]<hr color=green>\n",
           BBSNAME, currentuser.userid);
    if (type != 0)
        return update_form(board, file);
    printf("<table border=1>\n");
    printf("<tr><td>");
    printf("<tr><td><form name=\"form1\" method=post action=bbsedit>\n");
    printf("使用标题: %s 讨论区: %s<br>\n", nohtml(x->title), board);
    printf("本文作者：%s<br>\n", currentuser.userid);
    sprintf(path, "boards/%s/%s", board, file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("文件丢失");
    for (i = 0; i < 4; i++)
        if (fgets(buf, 500, fp) == 0)
            break;
    printf
    ("<textarea name=text rows=20 cols=80 wrap=physicle type=\"hidden\">");
    while (1)
    {
        if (fgets(buf, 500, fp) == 0)
            break;
        if (!strcasestr(buf, "</textarea>"))
            printf("%s", buf);
    }
    fclose(fp);
    printf("</textarea>\n");
    printf("<tr><td class=post align=center>\n");
    printf("<input type=hidden name=type value=1>\n");
    printf("<input type=hidden name=board value=%s>\n", board);
    printf("<input type=hidden name=file value=%s>\n", file);
    printf("<input type=submit value=存盘> \n");
    printf("<input type=reset value=重置></form>\n");
    printf("</table>");

    //added by hongliang on Nov. 21th, 2003 12:20
    printf("\n<script language=\"javascript\">\n<!--\n");
    printf("document.form1.text.focus();\n-->\n</script>\n");

    http_quit();
}

int update_form(char *board, char *file)
{
    FILE *fp;
    char *buf, path[80];
    char filehead[3][250];
    int i;

    sprintf(path, "boards/%s/%s", board, file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("无法存盘");
    for (i = 0; i < 3; i++)
        if (fgets(filehead[i], 250, fp) == 0)
            break;
    fclose(fp);
    fp = fopen(path, "w");
    if (fp == 0)
        http_fatal("无法存盘");
    for (i = 0; i < 3; i++)
        fprintf(fp, "%s", filehead[i]);
    fprintf(fp, "\n");
    buf = getparm("text");
    fprintf(fp, "%s", buf);
    getdatestring(time(0), NA);
    fprintf(fp,
            "[1;36m※ 修改:·%s 於 %16.16s 修改本文·[FROM: %-.20s][m\n",
            currentuser.userid, datestring + 6, fromhost);
    fclose(fp);
    printf("修改文章成功.<br><a href=bbsdoc?board=%s>返回本讨论区</a>",
           board);
}
