#include "www.h"
static struct dir x;


int bbsfupdate2_main()
{

    FILE * fp;

    struct shortfile *brd;

    char file[80], board[80];



    init_all();

    strsncpy(board, getparm("board"), 30);


    if (!loginok)
        http_fatal("匆匆过客无法更新文件, 请先登录");

    strsncpy(file, getparm("file"), 40);

    fp = fopen(UPLOAD_PATH "/.DIR", "r+");     //? w+ ?
    flock(fileno(fp), LOCK_EX);


    if (fp == 0)
        http_fatal("空目录");

    while (1)
    {

        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;

        if (!strncmp(x.showname, file, 36))
        {

            brd = getbcache(x.board);

            if (brd == 0)
                http_fatal("内部错误10002");

            if (!has_BM_perm(&currentuser, x.board)
                    &&
                    strcmp(x.userid, currentuser.userid)
                    &&
                    !(currentuser.userlevel & PERM_OBOARDS))
            {

                http_fatal("你无权更新本文件");

            }


            x.level = atoi(getparm("level"));

            strsncpy(x.exp, getparm("exp"), 72);

            x.live = atoi(getparm("live"));


            fseek(fp, -sizeof(x), SEEK_CUR);

            fwrite(&x, sizeof(x), 1, fp);


            {

                char buf[256];


                sprintf(buf, "%16.16s %s %d %s %s %s\n",

                        Ctime(time(0)) + 4, currentuser.userid, x.size,
                        fromhost, x.board, x.showname);

                f_append("update.log", buf);

            }

            printf("附件更新成功, ");

            printf
            ("URL为 :<P><font color=green>http://bbs.sjtu.edu.cn/showfile?name=%s </font><br>\n",
             x.showname);

            printf
            ("(以上URL(绿色部分)可拷贝至文章中直接引用, 系统会判断类型自动生成超链接.)<br>\n");

            printf
            ("如不拷贝, 则只能在板面的文章上载区看到本附件.<hr color=green>\n");

            printf("评定等级: %d星<br>\n", x.level + 1);

            printf("保留时间: %d天<br>\n", x.live);

            printf("文件大小: %d字节<br>\n", x.size);

            printf("文件类型: %s<br>\n", x.type);

            printf("文件名称: %s<br>\n", x.showname);

            printf("上载人ID: %s<br>\n", x.userid);

            printf("上载时间: %s<br>\n", Ctime(time(0)) + 4);

            printf("上载板面: %s<br>\n", x.board);

            printf("文件说明: %s<br>\n", nohtml(x.exp));

            printf("更新人ID: %s<br>\n", currentuser.userid);

            printf
            ("<hr color=green><a href='javascript:history.go(-2)'>返回</a>\n");

            http_quit();


            flock(fileno(fp), LOCK_UN);

            fclose(fp);

            return;

        }

    }

    fclose(fp);

    http_fatal("错误的文件名");

}


