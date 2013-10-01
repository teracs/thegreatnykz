#include "www.h"
static int do_del(char *board, char *file)
{
    FILE *fp;
    int num = 0;
    char path[256], buf[256], dir[256], *id = currentuser.userid;
    struct fileheader f;
    struct userec *u;
    struct shortfile *brd = getbcache(board);

    sprintf(dir, "boards/%s/.DIR", board);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(dir, "r");
    if (fp == 0)
    {
        printf("<tr><td><td>%s<td>文件不存在.\n", board);
        return -1;
    }
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {
            del_record(dir, sizeof(struct fileheader), num);
            //change by mudboy 4 回收站
            post_article2(board, f.title, file, f.owner, f.id, f.reid);
            /*
               sprintf(buf, "※ %s 于%s 删除[FROM: %s]", currentuser.userid, 4+Ctime(time(0)), fromhost);
               f_append(path, buf);
               if(!strcasecmp(currentuser.userid, f.owner)) {
               post_article("junk", f.title, path, f.owner, "", fromhost, -1, 0);
               } else {
               post_article("deleted", f.title, path, f.owner, "", fromhost, -1, 0);
               }
               unlink(path);
             */
            printf("<tr><td>%s  <td>标题:%s <td>删除成功.\n",
                   f.owner, nohtml(f.title));
            u = getuser(f.owner);
            if (brd && !junkboard(brd->filename) && u)
            {
                if (u->numposts > 0)
                    u->numposts--;
                save_user_data(u);
            }
            sprintf(buf, "%s %-12s bbsmdel %s\n",
                    Ctime(time(0)) + 4, id, board);
            f_append("trace", buf);
            fclose(fp);
            return 0;
        }
        num++;
    }
    fclose(fp);
    printf("<tr><td><td>%s<td>文件不存在.\n", file);
    return -1;
}

static int log_bm_ops(char *boardname, char *posname, char *operation,
                      char *content)
{
    char logfile[STRLEN];
    struct stat statbuf;
    FILE *fp;
    time_t now;
    struct tm *tm;
    char snippet[STRLEN];
    int district;

    now = time(0);
    tm = localtime(&now);

    district = board_in_which_dist(boardname);

    sprintf(logfile, "reclog/bmlog%c",
            district <= 9 ? district + '0' : district - 10 + 'A');
    if (stat(logfile, &statbuf) == -1)
    {
        fp = fopen(logfile, "a");
        fprintf(fp, "%4d年%02d月%02d日板面操作记录\n", tm->tm_year + 1900,
                tm->tm_mon + 1, tm->tm_mday);
        fprintf(fp, "%-6s%-16s%-13s%-4s%-7s%-5s%-29s\n", "时间", "板面",
                "操作者", "cli", "位置", "操作", "内容");
    }
    else
        fp = fopen(logfile, "a");

    strcpy(snippet, content);

    if (strlen(snippet) > 29)
    {
        snippet[26] = '.';
        snippet[27] = '.';
        snippet[28] = '.';
        snippet[29] = '\0';
    }

    fprintf(fp, "%02d:%02d %-16s%-13s%-4s%-7s%-5s%-29s\n",
            tm->tm_hour, tm->tm_min, boardname, currentuser.userid, "web",
            posname, operation, snippet);

    fclose(fp);
    return 1;
}

static int do_set(char *board, char *file, int flag)
{
    FILE *fp;
    char path[256], dir[256];
    char *action = "";
    struct fileheader f;

    sprintf(dir, "boards/%s/.DIR", board);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(dir, "r+");
    if (fp == 0)
    {
        printf("<tr><td><td>%s<td>文件不存在.\n", board);
        return -1;
    }
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {
            if (flag == 0)
            {
                f.accessed[0] &= ~(FILE_MARKED | FILE_DIGEST);
                action = "清除MG";
            }
            else if (flag == FILE_NOREPLY)
            {
                f.accessed[1] ^= FILE_NOREPLY2;
                action = "切换不可Re";
            }
            else
            {
                f.accessed[0] |= flag;
                action = (flag == FILE_MARKED ? "加M" : "加G");
            }
            fseek(fp, -1 * sizeof(struct fileheader), SEEK_CUR);
            fwrite(&f, sizeof(struct fileheader), 1, fp);
            fclose(fp);
            if (flag == FILE_DIGEST)
            {
                FILE *fp2;
                char dir2[256];
                char file2[256], path2[256];
                struct fileheader f2 = f;

                sprintf(file2, "G.%s", file + 2);
                sprintf(path2, "boards/%s/%s/%s", board,
                        getfolderstr(file2), file2);
                if (!file_exist(path2))
                {
                    link(path, path2);
                    sprintf(dir2, "boards/%s/.DIGEST", board);
                    strsncpy(f2.filename, file2, 20);
                    fp2 = fopen(dir2, "a");
                    fwrite(&f2, sizeof(struct fileheader), 1, fp2);
                    fclose(fp2);
                }
            }
            printf("<tr><td>%s<td>标题:%s<td>%s成功.\n",
                   f.owner, nohtml(f.title), action);
            if (flag & (FILE_MARKED | FILE_DIGEST))
                log_bm_ops(board, "板面",
                           flag == 0 ? "!mg" : (flag ==
                                                FILE_MARKED ? "m" : "g"),
                           f.title);
            return 0;
        }
    }
    fclose(fp);
    printf("<td><td><td>%s<td>文件不存在.\n", file);
    return -1;
}

int bbsman_main()
{
    int i, total = 0, mode;
    char board[80];
    struct shortfile *brd;
    char **parm_name;

    init_all();
    if (!loginok)
        http_quit_print("请先登录");
    strsncpy(board, getparm("board"), 60);
    mode = atoi(getparm("mode"));
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("错误的讨论区");
    strcpy(board, brd->filename);
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("你无权访问本页");
    if (mode <= 0 || mode > 5)
        http_quit_print("错误的参数1");
    if (cgiFormSuccess != cgiFormEntries(&parm_name))
        http_quit_print("错误的参数2");
    printf("<table>");
    for (i = 0; parm_name[i]; i++)
    {
        if (!strncmp(parm_name[i], "box", 3))
        {
            total++;
            if (mode == 1)
                do_del(board, parm_name[i] + 3);
            if (mode == 2)
                do_set(board, parm_name[i] + 3, FILE_MARKED);
            if (mode == 3)
                do_set(board, parm_name[i] + 3, FILE_DIGEST);
            if (mode == 4)
                do_set(board, parm_name[i] + 3, FILE_NOREPLY2);
            if (mode == 5)
                do_set(board, parm_name[i] + 3, 0);
        }
    }
    cgiStringArrayFree(parm_name);
    printf("</table>");
    if (total <= 0)
        printf("请先选定文章<br>\n");
    printf("<br><a href=bbsmdoc?board=%s&start=%s>返回管理模式</a>",
           board, getparm("start"));
    printf("</html>");
    return 0;
}
