#include "www.h"

int bbsfexp2_main()
{
    FILE *fp;
    struct dir x;
    int fid, dirsize;
    char board[40], url[256], dirbuf[256];

    cache_age = 60 * 60 * 24;
    init_all();
    strsncpy(board, getparm("board"), 40);
    fid = atoi(getparm("fid"));
    sprintf(dirbuf, UPLOAD_PATH2 "/%s", board);
    dirsize = file_size(dirbuf) / sizeof(struct dir);
    if (fid < 0 || fid >= dirsize)
        http_quit_print("文件不存在");
    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_quit_print("文件不存在1");
    fseek(fp, fid * sizeof(struct dir), SEEK_SET);
    if (fread(&x, sizeof(x), 1, fp) == 0)
    {
        fclose(fp);
        http_quit_print("系统错误,稍后再试");
    }
    fclose(fp);
    printf("<center><font class=\"title\">%s - 文件资料</font><hr>\n",
           BBSNAME);
    printf("<table border=1 width=600>\n");
    printf("<tr><td>个人推荐等级<td>%d星\n", x.level + 1);
    printf("<tr><td>希望保留时间<td>%d天\n", x.live);
    printf("<tr><td>文件大小<td>%d字节\n", x.size);
    printf("<tr><td>文件类型<td>%s\n", x.type);
    printf("<tr><td>文件名称<td>%s\n", x.showname);
    printf("<tr><td>上载人<td>%s\n", userid_str(x.userid));
    printf("<tr><td>上载时间<td>%s\n", Ctime(x.date));
    printf("<tr><td>上载板面<td><a href=bbsdoc?board=%s>%s</a>\n",
           x.board, x.board);
    printf("<tr><td>文件说明<td>%s\n", nohtml(x.exp));
    sprintf(url, UPLOAD_FILE_URL_HEADER "/%s/%s", x.board, x.showname);
    printf("<tr><td>URL地址<td><a href='%s' target=_blank>%s </a>\n",
           url, url);
    printf("</table><br>\n");
    printf("文件资料的URL地址可在本站文章中直接引用，系统会自动展开。");
    return 0;
}
