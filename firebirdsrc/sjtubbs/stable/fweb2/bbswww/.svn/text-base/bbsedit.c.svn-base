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
        http_quit_print("ÎÄÕÂ±ØÐëÒªÓÐ±êÌâ");

    if (strcmp(newtitle, fileheader.title))
    {

        changetitle = YEA;
        fp = fopen(path, "r");
        if (fp == 0)
        {
            http_quit_print("ÎÞ·¨´æÅÌ");
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
        http_quit_print("ÎÞ·¨´æÅÌ");
    for (i = 0; i < 3; i++)
        if (fgets(filehead[i], 250, fp) == 0)
            break;
    fclose(fp);
    fp = fopen(path, "w+");
    if (fp == 0)
        http_quit_print("ÎÞ·¨´æÅÌ");
    for (i = 0; i < 3; i++)
        fprintf(fp, "%s", filehead[i]);
    fprintf(fp, "\n");
    buf = getparm("text");
    fprintf(fp, "%s", buf);
    getdatestring(time(0), NA);

    if (strstr(datestring, "ÐÇÆÚ"))
    {
        temp = &datestring[strlen(datestring) - 1];
        while (*temp && *temp != ' ')
            temp--;
        *temp = '\0';
    }    
    fprintf(fp,
            "\n[1;36m¡ù ÐÞ¸Ä:¡¤%s ÓÚ %s ÐÞ¸Ä±¾ÎÄ¡¤[FROM: %-.46s][m\n",
            currentuser.userid, datestring, currentuser.lasthost);

    fclose(fp);
    printf("ÐÞ¸ÄÎÄÕÂ³É¹¦.<br><a href=bbsdoc?board=%s>·µ»Ø±¾ÌÖÂÛÇø</a>",
           board);
    return 0;
}

int bbsedit_main()
{
    FILE *fp;
    int type = 0;
    int i,posttime;
    char buf[512], path[512], file[512], board[512];
    struct shortfile *brd;
    struct fileheader *x;

    init_all();
    if (!loginok)
        http_quit_print("´Ò´Ò¹ý¿Í²»ÄÜÐÞ¸ÄÎÄÕÂ£¬ÇëÏÈµÇÂ¼");
    strsncpy(board, getparm("board"), 20);
    type = atoi(getparm("type"));
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("´íÎóµÄÌÖÂÛÇø");
    strcpy(board, brd->filename);
    strsncpy(file, getparm("file"), 20);
    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("´íÎóµÄÌÖÂÛÇø»òÕßÄúÎÞÈ¨ÔÚ´ËÌÖÂÛÇø·¢±íÎÄÕÂ");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("Äú×¢²áÊ±¼ä»¹²»Âú24Ð¡Ê±£¬ÔÝÊ±²»ÄÜÔÚ±¾°å·¢±íÎÄÕÂ.Èç¹ûÓÐÒÉÎÊÇëÔÚ BBSHelp »òÕß sysop °å·¢ÎÄ×ÉÑ¯¡£");
#endif
    x = get_file_ent(board, file, 1);
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("´íÎóµÄ²ÎÊý");
    if (x == 0)
        http_quit_print("´íÎóµÄ²ÎÊý");

    if (strcmp(x->owner, currentuser.userid)
        && !has_BM_perm(&currentuser, board))
        http_quit_print("ÄãÎÞÈ¨ÐÞ¸Ä´ËÎÄÕÂ");
    else if(!strcmp(x->owner, currentuser.userid)){
        strcpy(buf, file);
        buf[strlen(buf) - 2] = '\0';
        posttime = atoi(&buf[2]);
        if (posttime < currentuser.firstlogin)
            http_quit_print("ÄãÎÞÈ¨ÐÞ¸Ä´ËÎÄÕÂ");
    } 
    printf
        ("<center><font class=\"title\">%s - ÐÞ¸ÄÎÄÕÂ¡¡¡¡</font>[Ê¹ÓÃÕß: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    if (type != 0)
        return update_form(board, file);
    printf("<table border=1>\n");
    printf("<tr><td>");
    printf("<tr><td><form name=\"form1\" method=post action=bbsedit>\n");
    printf("<br />±êÌâ×î´ó³¤¶È: 44¡£»¹Ê£: <span id=\"chLeft\">44</span>¡£<br>");
    printf
        ("Ê¹ÓÃ±êÌâ: <input type=text name=title size=40 maxlength=100 tabindex=\"1\" value='%s' id=\"inputCH\">ÌÖÂÛÇø: %s<br>\n",
         nohtml(x->title), board);
    printf(
        "<script language=\"JavaScript\" type=\"text/javascript\">\n"
        "setInterval(\"checkLength()\", 100);\n"
        "function checkLength() {\n"
        "var maxChars = 44, i = 0, len = 0;\n"
        "var which = document.getElementById(\"inputCH\");\n"
        "for (; i<which.value.length; i++) {\n"
        "if (which.value.charCodeAt(i) == 27) {\n"
        "if (len == 0) which.value = \"error\";\n"
        "else which.value = which.value.substring(0,i);\n"
        "}\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len += 2;\n"
        "else\n"
        "len++;\n"
        "if (len > maxChars) break;\n"
        "}\n"
        "if (len > maxChars){\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len -= 2;\n"
        "else\n"
        "len--;\n"
        "which.value = which.value.substring(0,i);\n"
        "}\n"
        "var curr = maxChars - len;\n"
        "document.getElementById(\"chLeft\").innerHTML = curr.toString();\n"
        "}\n"
        "</script>\n");
    printf("±¾ÎÄ×÷Õß£º%s<br>\n", currentuser.userid);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_quit_print("ÎÄ¼þ¶ªÊ§");
    for (i = 0; i < 4; i++)
        if (fgets(buf, 500, fp) == 0)
            break;
    printf
        ("<textarea name=text rows=20 cols=76 wrap=physicle type=\"hidden\">");
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

    printf("\n<script language=\"javascript\">\n<!--\n");
    printf("document.form1.text.focus();\n-->\n</script>\n");
    printf("\n</html>");
    return 0;
}
