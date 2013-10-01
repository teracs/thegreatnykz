#include "www.h"

int bbsdelmail_main()
{
    FILE *fp;
    struct fileheader f;
    char path[80], file[80], *id;
    int num = 0;

    init_all();
    if (loginok == 0)
        http_quit_print("您尚未登录");
    id = currentuser.userid;
    strsncpy(file, getparm("file"), 20);
    if (strncmp(file, "M.", 2) || strstr(file, ".."))
        http_quit_print("错误的参数");
    sprintf(path, "mail/%c/%c/%s/.DIR", toupper(id[0]), toupper(id[1]),
            id);
    fp = fopen(path, "r");
    if (fp == 0)
        http_quit_print("错误的参数2");
    while (1)
    {
        if (fread(&f, sizeof(f), 1, fp) <= 0)
            break;
        num++;
        if (!strcmp(f.filename, file))
        {

            if (f.accessed[0] & FILE_MARKED)
            {
                fclose(fp);
                http_quit_print("不能删除有Mark标记的文章");
            }

            fclose(fp);
            del_record(path, sizeof(struct fileheader), num - 1);
            sprintf(path, "mail/%c/%c/%s/%s", toupper(id[0]),
                    toupper(id[1]), id, f.filename);
            unlink(path);

            redirect("bbsmail");
            http_quit();
        }
    }
    fclose(fp);
    http_quit_print("信件不存在, 无法删除");
    return 0;
}
