#include "www.h"
static int do_del3(char *file)
{
    FILE *fp;
    int num = 0;
    char path[256], dir[256];
    struct fileheader f;
    struct userec *u;

    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    sprintf(path, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, file);
    fp = fopen(dir, "r");
    if (fp == 0)
    {
        printf("错误的参数");
        return -1;
    }
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {

            if (!(f.accessed[0] & FILE_MARKED))
            {
                del_record(dir, sizeof(struct fileheader), num);
                unlink(path);
                u = getuser(f.owner);
                fclose(fp);
                return 0;

            }
            else
            {
                printf("不能删除有Mark标记的文章");
                u = getuser(f.owner);
                fclose(fp);
                return -1;

            }

        }
        num++;
    }
    fclose(fp);
    printf("<tr><td><td>%s<td>文件不存在.\n", file);
    return -1;
}

int bbsman2_main()
{
    int i, total = 0;
    char **parm_name;

    init_all();
    if (!loginok)
        http_quit_print("请先登录");
    if (cgiFormSuccess != cgiFormEntries(&parm_name))
        http_quit_print("错误的参数2");
    for (i = 0; parm_name[i]; i++)
    {
        if (!strncmp(parm_name[i], "box", 3))
        {
            total++;
            if (-1 == do_del3(parm_name[i] + 3))
            {
                cgiStringArrayFree(parm_name);
                return 0;
            }
        }
    }
    cgiStringArrayFree(parm_name);
    if (total <= 0)
        printf("请先选定信件<br>\n");
    redirect("bbsmail");
    printf("</html>");
    return 0;
}
