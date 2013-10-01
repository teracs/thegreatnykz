#include "www.h"

int bbsudel_main()
{
    FILE *fp;
    struct dir x;
    struct shortfile *brd;
    char file[80], filename[80];
    int num = 0;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客无法删除文件, 请先登录");
    strsncpy(file, getparm("file"), 40);
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_quit_print("空目录");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        if (!strncmp(x.showname, file, 36))
        {
            brd = getbcache(x.board);
            if (brd == 0)
                http_quit_print("内部错误10002");
            if (!has_BM_perm(&currentuser, x.board) &&
                strcmp(x.userid, currentuser.userid) &&
                !(currentuser.userlevel & PERM_OBOARDS))
            {
                http_quit_print("你无权删除本文件");
            }
            sprintf(filename, UPLOAD_PATH "/%d", x.filename);
            unlink(filename);
            del_record(UPLOAD_PATH "/.DIR", sizeof(struct dir), num);
            sprintf(filename, "bbsfdoc?board=%s", x.board);
            redirect(filename);
            http_quit();
        }
        num++;
    }
    fclose(fp);
    http_quit_print("错误的文件名");
    return 0;
}
