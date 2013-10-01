#include "www.h"

static int isimage(char *name)
{
    char *ptr;

    ptr = strrchr(name, '.');
    if (ptr != NULL)
    {
        ptr++;
        if (!strcasecmp(ptr, "jpg"))
            return 1;
        else if (!strcasecmp(ptr, "gif"))
            return 1;
        else if (!strcasecmp(ptr, "bmp"))
            return 1;
        else if (!strcasecmp(ptr, "png"))
            return 1;
        else if (!strcasecmp(ptr, "jpeg"))
            return 1;
    }
    return 0;

}

/*show detailed uploading file,only for bm*/
int bbsfddoc_main()
{
    FILE *fp;
    char *ptr, board[30], dirbuf[256];
    int dirsize, n_read, i, start;
    struct dir x[20];
    struct shortfile *brd;

    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("´íÎóµÄÌÖÂÛÇø");
    ptr = getparm("start");
    sprintf(dirbuf, UPLOAD_PATH2 "/%s", board);
    dirsize = file_size(dirbuf) / sizeof(struct dir);
    if (0 == dirsize)
    {
        char buf[256];

        snprintf(buf, 256,
                 "ÉÏÔØÇøÎª¿Õ£¬<a href=bbsupload?board=%s>ÉÏÔØÎÄ¼ş</a>",
                 board);
        http_quit_print(buf);
    }

    if (ptr[0] == 0)
    {
        start = dirsize - 20;
    }
    else
    {
        start = atoi(ptr);
        if (start >= dirsize)
            start = dirsize - 20;
    }
    if (start < 0)
        start = 0;
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("´íÎóµÄÌÖÂÛÇø");
    if (!has_BM_perm(&currentuser, board))
    {
        http_quit_print("ÄãÎŞÈ¨½øÈë¹ÜÀíÒ³Ãæ");
    }

    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_quit_print("Ã»ÓĞÕÒµ½ÎÄ¼ş");
    fseek(fp, start * sizeof(struct dir), SEEK_SET);
    n_read = fread(x, sizeof(struct dir), 20, fp);
    fclose(fp);

    printf
        ("<center><font class=\"title\">%s - °åÄÚÈí¼şÏÂÔØ¡¡¡¡</font>[ÌÖÂÛÇø: %s]<hr>\n",
         BBSNAME, board);
    printf("<form method=post action=bbsurdel>\n");
    printf("<input type=hidden name=board value=%s>\n", board);
    printf
        ("<table width=800 border=1><tr><td>ĞòºÅ<td>É¾<td>ÎÄ¼ş<td>ÉÏÔØÕß<td>Ê±¼ä<td>´óĞ¡<td>ËµÃ÷<td>±¸×¢\n");
    for (i = 0; i < n_read; i++)
    {
        printf("<tr><td>%d", start + 1 + i);
        printf
            ("<td><input style='height:14px' type=checkbox name=del%06d%s>",
             start + i, x[i].showname);
        printf("<td><a href=" UPLOAD_FILE_URL_HEADER
               "/%s/%s  target=_blank>", x[i].board, x[i].showname);
        if (isimage(x[i].showname))
            printf("<img src=" UPLOAD_FILE_URL_HEADER
                   "/%s/%s alt=%s onload=\"if(this.width>150) this.width = 150; if(this.height>150) this.height = 150\" ></a>",
                   x[i].board, x[i].showname, x[i].showname);
        else
            printf(" %s </a>", x[i].showname);
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td><a href=bbsfexp2?fid=%d&board=%s", start + i,
               x[i].board);
        printf(">more </a>\n");
        printf("<td width='33%%'>%s ", nohtml(x[i].exp));
    }
    printf("</table><hr>\n");
    printf
        ("<input type=submit value=\"É¾³ı\" onclick='return confirm(\"É¾³ıÑ¡Ôñ¸½¼şÂğå?\")'><br>");
    printf("</form>");
    printf
        ("[<a href=bbsfddoc?board=%s&start=%d>Ë¢ĞÂ</a>] [<a href=bbsdoc?board=%s>±¾ÌÖÂÛÇø</a>] [<a href=bbsupload?board=%s>ÉÏÔØÎÄ¼ş</a>]",
         board, start, board, board);
    printf("[<a href=bbsfddoc?board=%s&start=0>Ê×Ò³</a>] ", board);
    if (0 != start)
        printf("[<a href=bbsfddoc?board=%s&start=%d>ÉÏÒ»Ò³</a>] ",
               board, ((start - 20) < 0) ? 0 : (start - 20));
    if (start + 20 < dirsize)
        printf("[<a href=bbsfddoc?board=%s&start=%d>ÏÂÒ»Ò³</a>] ",
               board, start + 20);
    printf("[<a href=bbsfddoc?board=%s>Ä©Ò³</a>] ", board);
    printf
        ("<form name=form1 action=bbsfddoc method=post onsubmit='this.start.value-=1' >\n");
    printf("<input type=hidden name=board value=%s>\n", board);
    printf
        ("<input type=submit value=Ìø×ªµ½> µÚ <input type=text name=start size=4 onkeydown=\"return true;\" >Ìõ ");
    printf("</form>\n");
    printf("</html>");
    return 0;
}
