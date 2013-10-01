#include "www.h"

int bbsdel_main()
{
    FILE *fp;
    struct shortfile *brd;
    struct fileheader f;
    struct userec *u;
    char buf[80], dir[80], path[80], board[80], file[80], *id;
    int num = 0;
    int num1, start;

    init_all();
    if (!loginok)
        http_quit_print("请先登录");
    id = currentuser.userid;
    strsncpy(board, getparm("board"), 60);
    strsncpy(file, getparm("file"), 20);

    start = atoi(getparm("start"));
    num1 = atoi(getparm("num"));
    if (start < 0)
    {
        start = 0;
    }
    if (num1 < 0)
    {
        num1 = 0;
    }
    if (!strcmp(board, "deleted"))
        http_quit_print("本板不能删文");
    brd = getbcache(board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("错误的参数");
    if (strstr(file, ".."))
        http_quit_print("错误的参数");
    if (brd == 0)
        http_quit_print("板面错误");
    if (!has_post_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board))
        http_quit_print
            ("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
    sprintf(dir, "boards/%s/.DIR", board);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("错误的参数");
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {
            if (strcasecmp(id, f.owner)
                && !has_BM_perm(&currentuser, board))
            {
                fclose(fp);
                http_quit_print("你无权删除该文");
            }
            //debug by rwzmm @ sjtubbs
            //不能删除有G,M,#标记的文章
            if (f.accessed[0] & FILE_DIGEST)
            {
                fclose(fp);
                http_quit_print("不能删除收入文摘区(G)的文章");
            }
            if (f.accessed[0] & FILE_MARKED)
            {
                fclose(fp);
                http_quit_print("不能删除有Mark标记的文章");
            }
            if (f.accessed[0] & FILE_TOP1)
            {
                fclose(fp);
                http_quit_print("不能删除被置底/顶的文章");
            }
            del_record(dir, sizeof(struct fileheader), num);
            post_article2(board, f.title, file, f.owner, f.id, f.reid);
            /*
               sprintf(buf, "\n※ %s 于 %s 删除[FROM: %s]", currentuser.userid, Ctime(time(0))+4, fromhost);
               f_append(path, buf);
               if(!strcasecmp(currentuser.userid, f.owner)) {
               post_article("junk", f.title, path, f.owner, "", fromhost, -1, 0);
               } else {
               post_article("deleted", f.title, path, f.owner, "", fromhost, -1, 0);
               }
               unlink(path);
             */
            printf("删除成功.<br>");

            printf("<input type=\"button\" name=\"a\" ");
            printf
                ("onclick=\"location.href('bbsdoc?board=%s&start=%d&currentpoint=%d')\"",
                 board, start, (num1 - start));
            printf(" value=\"[返回本讨论区]\">\n");
            printf("<script>document.all(\"a\").focus();</script>");

            u = getuser(f.owner);
            if (!junkboard(brd->filename) && u)
            {
                if (u->numposts > 0)
                    u->numposts--;
                save_user_data(u);
            }
            sprintf(buf, "%s %-12s bbsdel %s\n",
                    Ctime(time(0)) + 4, id, board);
            f_append("trace", buf);
            {
                char buf[256];

                sprintf(buf, "%24.24s %s %s Do_Web_D\n",
                        Ctime(time(0)), currentuser.userid, board);
                f_append("bm2.log", buf);
            }
            fclose(fp);
            http_quit();
        }
        num++;
    }
    fclose(fp);
    printf("文件不存在, 删除失败.<br>\n");

    printf("<input type=\"button\" name=\"a\" ");
    printf
        ("onclick=\"location.href('bbsdoc?board=%s&start=%d&currentpoint=%d')",
         board, start, (num1 - start));
    printf(" value=\"[返回本讨论区]\">\n");
    printf("<script>document.all(\"a\").focus();</script>");

    printf("\n</html>");
    return 0;
}
