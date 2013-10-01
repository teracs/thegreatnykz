#include "www.h"
#include "bbs.h"

void post_article3(char *board, char *title, char *file, char *id,
                   unsigned int currid, unsigned int reid);
int bbsdelreturn_main()
{
    FILE *fp;
    struct shortfile *brd;
    struct fileheader f;
    struct userec *u;
    char buf[80], dir[80], path[80], board[80], file[80], *id, *p,
        UTitle[128];
    int num = 0;
    int i;

    init_all();
    if (!loginok)
        http_quit_print("请先登录");
    id = currentuser.userid;
    strsncpy(board, getparm("board"), 60);
    strsncpy(file, getparm("file"), 20);
    if (!strcmp(board, "deleted"))
        http_quit_print("本板不能恢复回收文章");
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
    sprintf(dir, "boards/%s/.RETURNDIR", board);
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
                http_quit_print("你无权恢复该文");
            }

            strcpy(UTitle, f.title);
            if ((p = strrchr(UTitle, '-')))
            {
                *p = 0;
                for (i = strlen(UTitle) - 1; i >= 0; i--)
                {
                    if (UTitle[i] != ' ')
                        break;
                    else
                        UTitle[i] = 0;
                }
            }
            strncpy(f.title, UTitle, STRLEN);
            f.accessed[0] = 0;

            return_del_article(dir, sizeof(struct fileheader), num);
            post_article3(board, f.title, file, f.owner, f.id, f.reid);

            printf
                ("恢复成功.<br><a href='bbsdocdel?board=%s'>返回本讨论区回收站</a>",
                 board);
            u = getuser(f.owner);
            if (!junkboard(brd->filename) && u)
            {
                if (u->numposts > 0)
                    u->numposts++;
                save_user_data(u);
            }
            sprintf(buf, "%s %-12s bbsdel %s\n",
                    Ctime(time(0)) + 4, id, board);
            f_append("trace", buf);
            {
                char buf[256];

                sprintf(buf,
                        "%24.24s %s %s Do_Web_D_RETURN\n",
                        Ctime(time(0)), currentuser.userid, board);
                f_append("bm2.log", buf);
            }
            http_quit();
        }
        num++;
    }
    fclose(fp);
    printf("文件不存在, 恢复失败.<br>\n");
    printf("<a href='bbsdocdel?board=%s'>返回本讨论区回收站</a>", board);
    printf("\n</html>");
    return 0;
}

void
post_article3(char *board, char *title, char *file, char *id,
              unsigned int currid, unsigned int reid)
{
    FILE *fp;
    char buf[1024];
    struct fileheader header;

    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    header.id = currid;
    header.reid = reid;
    sprintf(header.filename, "%s", file);
    strsncpy(header.title, title, 60);
    header.accessed[0] = 0;
    sprintf(buf, "boards/%s/.DIR", board);
    fp = fopen(buf, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
}
