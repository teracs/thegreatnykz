#include "www.h"

extern char *get_title(char *title);

extern char *setmodulefile(char *buf, char *boardname, char *filename);
extern int ismoduleexist(char *board);

extern int updateqzdenyuser(int district, char userid[IDLEN]);

extern int seek_in_qzdenyfile(struct userec *user);

int bbswappst_main()
{
    FILE *fp;
    int i;
    char userid[80], dir[80], buf[512], path[512], file[512], board[512],
        reid_str[16], title[80] = "";
    int check = 0;
    char ch[3];
    struct fileheader f;
    struct shortfile *bp;

    time_t now = time(0);
    struct tm *tm;

    tm = localtime(&now);

    init_all();

    if (cgiFormSuccess == cgiFormString("check", ch, 2))
    {
        check = 1;
    }
    if (check == 0)
    {
        strsncpy(board, getparm("board"), 20);
    }
    else
    {
        cgiFormString("board", board, 20);
    }
    if (!loginok)
        http_quit_print("匆匆过客不能发表文章，请先登录");
    seek_in_qzdenyfile(&currentuser);
    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("错误的讨论区或者您无权在此讨论区发表文章");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
    if (shm_bcache->all_no_post_end >= shm_bcache->all_no_post_begin)
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {
            if ((unsigned int) tm->tm_hour >= shm_bcache->all_no_post_begin
                && (unsigned int) tm->tm_hour <
                shm_bcache->all_no_post_end)
            {
                printf("本站于每日 %d 时至 %d 时，全站只读。",
                       shm_bcache->all_no_post_begin,
                       shm_bcache->all_no_post_end);
                http_quit();
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {

            if ((unsigned int) tm->tm_hour >= shm_bcache->no_post_begin
                && (unsigned int) tm->tm_hour < shm_bcache->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {

                    printf
                        ("本站于每日%d时至%d时暂停校外用户的发文权限。\n\n          如果有疑问请在 BBSHelp 或者 sysop 板查阅相关通知与说明。",
                         shm_bcache->no_post_begin,
                         shm_bcache->no_post_end);
                    http_quit();

                }
            }
        }
    }
    else
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {
            if ((unsigned int) tm->tm_hour >= shm_bcache->all_no_post_begin
                || (unsigned int) tm->tm_hour <
                shm_bcache->all_no_post_end)
            {
                printf("本站于每日 %d 时至次日 %d 时，全站只读。",
                       shm_bcache->all_no_post_begin,
                       shm_bcache->all_no_post_end);
                http_quit();
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {

            if ((unsigned int) tm->tm_hour >= shm_bcache->no_post_begin
                || (unsigned int) tm->tm_hour < shm_bcache->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {

                    printf
                        ("本站于每日%d时至次日%d时暂停校外用户的发文权限。\n\n          如果有疑问请在 BBSHelp 或者 sysop 板查阅相关通知与说明。",
                         shm_bcache->no_post_begin,
                         shm_bcache->no_post_end);
                    http_quit();

                }
            }
        }

    }

    if (check == 0)
    {
        strsncpy(file, getparm("file"), 20);
    }
    else
    {
        cgiFormString("file", file, 20);
    }

    bzero(reid_str, 16);

    if (file[0])
    {
        if (file[0] != 'M')
            http_quit_print("错误的文件名");
        sprintf(dir, "boards/%s/.DIR", getbcache(board)->filename);
        fp = fopen(dir, "r");
        if (fp == 0)
            http_quit_print("错误的目录");
        while (1)
        {
            if (fread(&f, sizeof(f), 1, fp) <= 0)
            {
                break;
            }

            if (!strcmp(f.filename, file))
            {
                char *ptr;

                sprintf(reid_str, "%d", f.reid);

                if ((f.accessed[0] & FILE_NOREPLY)
                    || (getbcache(board)->flag & NOREPLY_FLAG))
                {
                    fclose(fp);
                    http_quit_print("本文为不可回复");
                }
                ptr = f.title;
                if (!strncmp(ptr, "Re: ", 4))
                    ptr += 4;
                sprintf(title, "Re: %s", ptr);
                title[64] = 0;
                strsncpy(userid, f.owner, 60);
                fclose(fp);
                goto L2;
            }
        }
        fclose(fp);
        http_quit_print("错误的文件名");
    }
  L2:
    bp = getbcache(board);

    printf
        ("<font class=\"title\">%s - 发表文章　　</font>[使用者: %s] 讨论区: [%s]<hr>\n",
         BBSNAME, currentuser.userid, board);
    printf("<body onkeydown=\"ctlent()\">");
    printf
        ("<form name=\"form1\" method=post action=bbswapsnd enctype='multipart/form-data'>\n");
    printf
        ("<input type=hidden name='board' value='%s'><input type=hidden name='file' value='%s'><input type=hidden name='reidstr' value='%s'>",
         board, file, reid_str);
    printf
        ("标题: <input type=text name=\"title\" size=20 maxlength=100 tabindex=\"1\" value='%s' id=\"inputCH\">",
         get_title(title));
    printf("<br>");
    if (bp->flag & ANONY_FLAG)
    {
    	printf(" 设置本文属性 ");
    
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 checked>匿名");
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0>不匿名<br>\n");
    }
    if (bp->flag & POSITIVE_ANONY_FLAG)
    {
    	printf(" 设置本文属性 ");
    
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 >匿名");
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0 checked>不匿名<br>\n");
    }

    printf("<input type=hidden name=signature value=1>");
    printf("<input type=hidden name=autocr value=1>");
    printf(
        "\n<script language=\"JavaScript\" type=\"text/javascript\">\n"
        "setInterval(\"checkLength()\", 100);\n"
        "function checkLength() {\n"
        "var maxChars = 44, i = 0, len = 0;\n"
        "var which = document.getElementById(\"inputCH\");\n"
        "for (; i<which.value.length; i++) {\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len += 2;\n"
        "else\n"
        "len++;\n"
        "if (len > maxChars) break;\n"
        "}\n"
        "if (len > maxChars){\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len -= 2;\n"
        "else\n"
        "len--;\n"
        "which.value = which.value.substring(0,i);\n"
        "}\n"
        "var curr = maxChars - len;\n"
        "}\n"
        "</script>");

    printf
        ("\n<textarea id=text name=text rows=8 cols=20 wrap=physicle tabindex=\"2\" onkeydown=\"ctlent()\">");

    printf("%s", getparm("text"));
    if (check == 1)
    {
        char mpath[STRLEN];
        char module[90];

        cgiFormString("level", module, 80);
        strcat(module, "_M");
        sprintf(mpath, "vote/%s/%s", board, module);
        fp = fopen(mpath, "r");
        printf("\n");
        if (fp == NULL)
            http_quit_print("无法打开文件");
        while (fgets(buf, 500, fp) != NULL)
        {
            printf("%s", buf);
        }
        fclose(fp);
    }
    if (file[0])
    {
        int lines = 0;

        printf("\n\n【 在 %s 的大作中提到: 】\n", userid);
        sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
        fp = fopen(path, "r");
        if (fp == NULL)
            http_quit_print("无法打开文件2");
        if (fp)
        {
            for (i = 0; i < 3; i++)
                if (fgets(buf, 500, fp) == 0)
                    break;
            while (1)
            {
                if (fgets(buf, 500, fp) == 0)
                    break;
                if (!strncmp(buf, ": 【", 4))
                    continue;
                if (!strncmp(buf, ": : ", 4))
                    continue;
                if (!strncmp(buf, "--\n", 3))
                    break;
                if (buf[0] == '\n')
                    continue;
                if (lines++ >= 20)
                {

                    printf(": (以下引言省略...)\n");
                    break;
                }
                if (!strcasestr(buf, "</textarea>"))

                    printf(": %s", buf);
            }
            fclose(fp);
        }
    }

    printf("</textarea>\n");

    printf("<br><input type=submit value=发表文章> ");
    printf("<input type=reset value=内容复原>\n");
	printf("<br><font color=green>发文注意事项: <br>\n");
    printf
        ("本站规定同样内容的文章严禁在 4 个或 4 个以上讨论区内重复发表。详细内容参见有关站规。<br>\n");
    printf
        ("发文时应慎重考虑文章内容是否适合公开场合发表，请勿肆意灌水。谢谢您的合作。<br>\n</font>");
    printf("<hr><br>\n");

    printf("上载附件: <input type=file name=up>");
    printf("<input type=hidden name=MAX_FILE_SIZE value=1048577>");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<input type=hidden name=level value=0>\n");
    printf("<input name=live type=hidden value=180>\n");
    printf("<br>附件的简要说明: <input name=exp size=20 maxlength=72>\n");

    printf("<br><input type=submit value='开始上传文件'> \n");


    printf("</body>");

    printf("</form>\n");

    printf("\n<script language=\"javascript\">\n<!--\n");
    printf("function ctlent()\n");
    printf("{\n");
    printf("k = event.keyCode;	\n");
    printf("if((event.ctrlKey&&event.keyCode==13))\n");
    printf("{document.form1.submit();}\n");

    printf("if(event.ctrlKey&&k==87)\n");
    printf("{alert(\"为防止您不小心关闭当前窗口，故弹出此对话框\");}}\n");
    printf("-->\n");
    printf("</script>\n");
    printf("</html>");
    return 0;
}
