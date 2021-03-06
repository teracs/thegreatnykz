#include "www.h"

static int update_form(char *board, char *file)
{
    FILE *fp;
    char *buf, path[80];
    char filehead[3][250];
    int i;
    char *newtitle, *temp;
    struct fileheader fileheader;
    int id;
    int changetitle = NA;

    sprintf(path, "boards/%s/%s", board, ".DIR");
    id = search_record(path, (char *) &fileheader, sizeof(fileheader),
                       cmpvfilename, file);
    newtitle = getparm("title");
    if (newtitle[0] == 0)
        http_quit_print("文章必须要有标题");

    if (strcmp(newtitle, fileheader.title))
    {

        changetitle = YEA;
        fp = fopen(path, "r");
        if (fp == 0)
        {
            http_quit_print("无法存盘");
        }
        fclose(fp);
        strncpy(fileheader.title, newtitle, STRLEN);
        fileheader.title[STRLEN-1] = '\0';
        substitute_record(path, &fileheader, sizeof(fileheader), id);
        sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
        change_title(path, fileheader.title);
    }
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_quit_print("无法存盘");
    for (i = 0; i < 3; i++)
        if (fgets(filehead[i], 250, fp) == 0)
            break;
    fclose(fp);
    fp = fopen(path, "w+");
    if (fp == 0)
        http_quit_print("无法存盘");
    for (i = 0; i < 3; i++)
        fprintf(fp, "%s", filehead[i]);
    fprintf(fp, "\n");
    buf = getparm("text");
    fprintf(fp, "%s", buf);
    getdatestring(time(0), NA);

    if (strstr(datestring, "星期"))
    {
        temp = &datestring[strlen(datestring) - 1];
        while (*temp && *temp != ' ')
            temp--;
        *temp = '\0';
    }    
    fprintf(fp,
        "\n[1;36m※ 修改:·%s 于 %s 修改本文·[FROM: %-.46s][m\n",
        currentuser.userid, datestring, currentuser.lasthost);

    fclose(fp);
    printf("修改文章成功.<br><a href=bbsdoc?board=%s>返回本讨论区</a>",
           board);
    return 0;
}

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
        http_quit_print("匆匆过客不能修改文章，请先登录");
    strsncpy(board, getparm("board"), 20);
    type = atoi(getparm("type"));
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("错误的讨论区");
    strcpy(board, brd->filename);
    strsncpy(file, getparm("file"), 20);
    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("错误的讨论区或者您无权在此讨论区发表文章");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
    x = get_file_ent(board, file, 1);
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数");
    if (x == 0)
        http_quit_print("错误的参数");

    if (strcmp(x->owner, currentuser.userid)
        && !has_BM_perm(&currentuser, board))
        http_quit_print("你无权修改此文章");

    printf
        ("<center><font class=\"title\">%s - 修改文章　　</font>[使用者: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    if (type != 0)
        return update_form(board, file);
    printf("<table border=1>\n");
    printf("<tr><td>");
    printf("<tr><td><form name=\"form1\" method=post action=bbsedit>\n");
    printf
        ("使用标题: <input type=text name=title size=40 maxlength=100 tabindex=\"1\" value='%s'>讨论区: %s<br>\n",
         nohtml(x->title), board);
    printf("本文作者：%s<br>\n", currentuser.userid);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_quit_print("文件丢失");
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

    printf("\n<script language=\"javascript\">\n<!--\n");
    printf("document.form1.text.focus();\n-->\n</script>\n");
    printf("\n</html>");
    return 0;
}
