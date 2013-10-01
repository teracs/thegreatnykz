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
        http_fatal("´Ò´Ò¹ı¿Í²»ÄÜĞŞ¸ÄÎÄÕÂ£¬ÇëÏÈµÇÂ¼");
    strsncpy(board, getparm("board"), 20);
    type = atoi(getparm("type"));
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("´íÎóµÄÌÖÂÛÇø");
    strcpy(board, brd->filename);
    strsncpy(file, getparm("file"), 20);
    if (!has_post_perm(&currentuser, board))
        http_fatal("´íÎóµÄÌÖÂÛÇø»òÕßÄúÎŞÈ¨ÔÚ´ËÌÖÂÛÇø·¢±íÎÄÕÂ");
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, board))
		http_fatal("Äú×¢²áÊ±¼ä»¹²»Âú24Ğ¡Ê±£¬ÔİÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ.Èç¹ûÓĞÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
#endif	
    x = get_file_ent(board, file);
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("´íÎóµÄ²ÎÊı");
    if (x == 0)
        http_fatal("´íÎóµÄ²ÎÊı");

    //Bug
    //Bug-description:	the boardmaster cannot modify forum
    //Bug-solution:	let the bm modify it :)
    //Bug-fixer:	hongliang
    //Date:		Nov. 21th, 2003
    //start-hongliang
    if (strcmp(x->owner, currentuser.userid) && !has_BM_perm(&currentuser, board))
        http_fatal("ÄãÎŞÈ¨ĞŞ¸Ä´ËÎÄÕÂ");
    //end-hongliang

    printf("<center>%s -- ĞŞ¸ÄÎÄÕÂ [Ê¹ÓÃÕß: %s]<hr color=green>\n",
           BBSNAME, currentuser.userid);
    if (type != 0)
        return update_form(board, file);
    printf("<table border=1>\n");
    printf("<tr><td>");
    printf("<tr><td><form name=\"form1\" method=post action=bbsedit>\n");
    printf("Ê¹ÓÃ±êÌâ: %s ÌÖÂÛÇø: %s<br>\n", nohtml(x->title), board);
    printf("±¾ÎÄ×÷Õß£º%s<br>\n", currentuser.userid);
    sprintf(path, "boards/%s/%s", board, file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("ÎÄ¼ş¶ªÊ§");
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
    printf("<input type=submit value=´æÅÌ> \n");
    printf("<input type=reset value=ÖØÖÃ></form>\n");
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
        http_fatal("ÎŞ·¨´æÅÌ");
    for (i = 0; i < 3; i++)
        if (fgets(filehead[i], 250, fp) == 0)
            break;
    fclose(fp);
    fp = fopen(path, "w");
    if (fp == 0)
        http_fatal("ÎŞ·¨´æÅÌ");
    for (i = 0; i < 3; i++)
        fprintf(fp, "%s", filehead[i]);
    fprintf(fp, "\n");
    buf = getparm("text");
    fprintf(fp, "%s", buf);
    getdatestring(time(0), NA);
    fprintf(fp,
            "[1;36m¡ù ĞŞ¸Ä:¡¤%s ÓÚ %16.16s ĞŞ¸Ä±¾ÎÄ¡¤[FROM: %-.20s][m\n",
            currentuser.userid, datestring + 6, fromhost);
    fclose(fp);
    printf("ĞŞ¸ÄÎÄÕÂ³É¹¦.<br><a href=bbsdoc?board=%s>·µ»Ø±¾ÌÖÂÛÇø</a>",
           board);
}
