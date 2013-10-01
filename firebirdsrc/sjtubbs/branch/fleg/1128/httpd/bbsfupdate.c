#include "www.h"

int bbsfupdate_main()
{

    const char checked[] = "checked";

    const char unchecked[] = "";



    FILE * fp;

    struct dir x;

    struct shortfile *brd;

    char file[80], board[80];



    init_all();

    strsncpy(board, getparm("board"), 30);


    if (!loginok)
        http_fatal("匆匆过客无法更新文件, 请先登录");

    strsncpy(file, getparm("file"), 40);

    fp = fopen(UPLOAD_PATH "/.DIR", "r");

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


            printf
            ("<form method=post action=bbsfupdate2 enctype='multipart/form-data'>\n");

            printf("<table>\n");

            printf("<tr><td>更新附件: <td>");

            printf("<input type=hidden name=board value='%s'>", board);

            printf("<input type=hidden name=file value='%s'>", file);

            printf("<tr><td>个人推荐评价: <td>");


            printf("<input type=radio name=level value=0 %s>1星 ",
                   x.level == 0 ? checked : unchecked);

            printf("<input type=radio name=level value=1 %s>2星 ",
                   x.level == 1 ? checked : unchecked);

            printf("<input type=radio name=level value=2 %s>3星 ",
                   x.level == 2 ? checked : unchecked);

            printf("<input type=radio name=level value=3 %s>4星 ",
                   x.level == 3 ? checked : unchecked);

            printf("<input type=radio name=level value=4 %s>5星 ",
                   x.level == 4 ? checked : unchecked);

            printf
            ("<tr><td>希望保留的时间: <td><input name=live size=4 maxlength=4 value=%d>(1-9999)天<br>\n",
             x.live);

            printf
            ("<tr><td>附件的简要说明: <td><input name=exp size=60 maxlength=72 value=%72s>\n",
             x.exp);

            printf("</table>\n");

            printf("<hr><br>\n");

            printf("<input type=submit value='更新文件'> \n");

            printf
            ("<input type=button onclick='javascript:history.go(-1)' value='返回上一页'> \n");

            printf("</form>");


            fclose(fp);

            return;

        }

    }

    fclose(fp);

    http_fatal("错误的文件名");

}


