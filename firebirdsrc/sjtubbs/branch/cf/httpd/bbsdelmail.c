#include "www.h"

int bbsdelmail_main()
{
    FILE *fp;
    struct fileheader f;
    char path[80], file[80], *id;
    int num = 0;

    init_all();
    if (loginok == 0)
        http_fatal("您尚未登录");
    id = currentuser.userid;
    strsncpy(file, getparm("file"), 20);
    if (strncmp(file, "M.", 2) || strstr(file, ".."))
        http_fatal("错误的参数");
    sprintf(path, "mail/%c/%s/.DIR", toupper(id[0]), id);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("错误的参数2");
    while (1)
    {
        if (fread(&f, sizeof(f), 1, fp) <= 0)
            break;
        num++;
        if (!strcmp(f.filename, file))
        {
            fclose(fp);
            del_record(path, sizeof(struct fileheader), num - 1);
            sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id, f.filename);
            unlink(path);
            //printf("信件已删除.<br><a href=bbsmail>返回所有信件列表</a>\n");
            redirect("bbsmail");
            http_quit();
        }
    }
    fclose(fp);
    http_fatal("信件不存在, 无法删除");
}
