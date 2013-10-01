#include "www.h"

int bbsudel2_main()
{
    FILE *fp;
    struct dir x;
    struct shortfile *brd;
    char file[40], filename[256], dirbuf[256], board[40];
    int fid;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客无法删除文件, 请先登录");
    strsncpy(board, getparm("board"), 40);
    strsncpy(file, getparm("file"), 40);
    fid = atoi(getparm("fid"));
    sprintf(dirbuf, UPLOAD_PATH2 "/%s", board);
    if (fid < 0 || fid >= (file_size(dirbuf) / (int) sizeof(struct dir)))
        http_quit_print("文件不存在");
    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_quit_print("空目录");
    fseek(fp, fid * sizeof(struct dir), SEEK_SET);
    if (fread(&x, sizeof(x), 1, fp) <= 0)
    {
        fclose(fp);
        http_quit_print("内部错误1");
    }
    fclose(fp);
    if (!strncmp(x.showname, file, 40))
    {
        brd = getbcache(x.board);
        if (brd == 0)
            http_quit_print("内部错误3");
        if (!has_BM_perm(&currentuser, x.board) &&
            strcmp(x.userid, currentuser.userid) &&
            !(currentuser.userlevel & PERM_OBOARDS))
        {
            http_quit_print("你无权删除本文件");
        }
        sprintf(filename, UPLOAD_FILE_PATH2 "/%s/%s", x.board, x.showname);
        unlink(filename);

        del_record(dirbuf, sizeof(struct dir), fid);
        sprintf(filename, "bbsfdoc2?board=%s&start=%d", x.board,
                (fid - 10) > 0 ? (fid - 10) : 0);
        redirect(filename);
        http_quit();
    }
    else
    {
        http_quit_print("内部错误2,请重试");
    }
    return 0;
}
