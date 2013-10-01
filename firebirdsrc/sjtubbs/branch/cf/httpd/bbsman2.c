#include "www.h"

int bbsman2_main()
{
    int i, total = 0;

    init_all();
    if (!loginok)
        http_fatal("请先登录");
    for (i = 0; i < parm_num && i < 40; i++)
    {
        if (!strncmp(parm_name[i], "box", 3))
        {
            total++;
            do_del3(parm_name[i] + 3);
        }
    }
    if (total <= 0)
        printf("请先选定信件<br>\n");
    redirect("bbsmail");
    http_quit();
}

int do_del3(char *file)
{
    FILE *fp;
    int num = 0;
    char path[256], dir[256];
    struct fileheader f;
    struct userec *u;

    sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]),
            currentuser.userid);
    sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
            currentuser.userid, file);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("错误的参数");
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {
            del_record(dir, sizeof(struct fileheader), num);
            unlink(path);
            u = getuser(f.owner);
            fclose(fp);
            return;
        }
        num++;
    }
    fclose(fp);
    printf("<tr><td><td>%s<td>文件不存在.\n", file);
}
