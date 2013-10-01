
#include "www.h"
int totalij = 0;
char showboard[80];

static int f_get_file(char *file, struct boardcollection *sstruct, int n)
{
    int sd;
    char currentfilename[80];

    snprintf(currentfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, file);
    if ((sd = open(currentfilename, O_RDONLY)) == -1)
        return -1;
    if (lseek(sd, (n - 1) * sizeof(struct boardcollection), SEEK_CUR) ==
        -1)
    {
        close(sd);
        return -3;
    }
    if (read(sd, sstruct, sizeof(struct boardcollection)) !=
        sizeof(struct boardcollection))
    {
        close(sd);
        return -2;
    }
    close(sd);
    return 0;
}

static int getsubdirnum(char *file)
{
    struct boardcollection tmp;
    int i, num = 0;

    if (f_get_file(file, &tmp, 1) == -1)
        return -1;
    for (i = 0; i <= tmp.subnum - 1; i++)
        num += getsubdirnum(tmp.subdirsfile[i]);
    if (tmp.extends[0] != 0)
        num += getsubdirnum(tmp.extends);
    return num + tmp.subnum;
}
static int getdirnumber()
{
    char bufnum[80];
    int num;
    char cnum[10];
    FILE *fp;

    snprintf(bufnum, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             ".dirnum");
    if ((fp = fopen(bufnum, "r")) == NULL)
    {
        if ((fp = fopen(bufnum, "w")) == NULL)
            return -1;
        num = getsubdirnum("main.hck");
        snprintf(cnum, 10, "%d", num);
        fputs(cnum, fp);
        fclose(fp);
        return num;
    }
    fgets(cnum, 10, fp);

    fclose(fp);
    return atoi(cnum);
}
static int freshdirnum(int n)
{
    char bufnum[80];
    FILE *fp;
    char num[10];

    snprintf(bufnum, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             ".dirnum");
    if ((fp = fopen(bufnum, "w")) == NULL)
        return -1;
    snprintf(num, 10, "%d", n);
    fputs(num, fp);
    fclose(fp);
    return 0;

}
static int getfilename()
{
    char buf3[80];
    time_t t;
    int filesd, count = 0;;
    t = time(0);
    snprintf(buf3, 80, "home/%c/%c/%s/%ld", toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, t);
    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        snprintf(buf3, 80, "home/%c/%c/%s/%ld",
                 toupper(currentuser.userid[0]),
                 toupper(currentuser.userid[1]), currentuser.userid, t);

        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);
    return t;

}

static int f_replace_file(struct boardcollection sstruct)
{
    int sd;
    char file[80];

    snprintf(file, 80, "home/%c/%c/%s/%s", toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             sstruct.dirname);

    if ((sd = open(file, O_RDWR | O_CREAT, 0644)) == -1)
        return -1;
    write(sd, &sstruct, sizeof(struct boardcollection));
    close(sd);
    return 0;
}
static void init_dir(struct boardcollection *dirr, char *dirname,
                     char *updir)
{
    int i = 0;

    dirr->subnum = 0;
    dirr->boardnum = 0;
    dirr->isextends = 0;
    snprintf(dirr->dirname, 80, dirname, toupper(currentuser.userid[0]),
             currentuser.userid);
    snprintf(dirr->updir, 80, updir, toupper(currentuser.userid[0]),
             currentuser.userid);
    for (i = 0; i <= 19; i++)
    {
        dirr->subdirname[i][0] = 0;
        dirr->subdirsfile[i][0] = 0;
        dirr->boards[i][0] = 0;
    }
    dirr->extends[0] = 0;
}

static int do_adddir(char *boardfile, char *newdir)
{
    char boardfilename[80];
    struct boardcollection currentdir;
    struct boardcollection newstruct;
    time_t now;

    snprintf(boardfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);
    if (f_get_file(boardfile, &currentdir, 1) != 0)
        return -1;
    if (currentdir.subnum < 20)
    {
        now = getfilename();
        currentdir.subnum++;
        snprintf(currentdir.subdirsfile[currentdir.subnum - 1], 80, "%ld",
                 now);
        snprintf(currentdir.subdirname[currentdir.subnum - 1], 80, "%s",
                 newdir);
        f_replace_file(currentdir);
        init_dir(&newstruct, currentdir.subdirsfile[currentdir.subnum - 1],
                 boardfile);
        f_replace_file(newstruct);
    }
    else
    {
        if (currentdir.extends[0] == 0)
        {
            now = getfilename();
            snprintf(currentdir.extends, 80, "%ld", now);
            f_replace_file(currentdir);
            init_dir(&newstruct, currentdir.extends, boardfile);
            newstruct.isextends = 1;
            f_replace_file(newstruct);
            do_adddir(currentdir.extends, newdir);
        }
        else
        {
            do_adddir(currentdir.extends, newdir);
        }
    }
    return 0;

}

int boardisexist(struct boardcollection tmp, char *newboard)
{
    int i = 0;

    while (i < tmp.boardnum)
    {
        if (strcmp(tmp.boards[i], newboard) == 0)
            return 1;
        i++;
    }
    return 0;
}
static int do_addboard(char *boardfile, char *newboard)
{
    char boardfilename[80];
    struct boardcollection currentdir;
    struct boardcollection newstruct;
    time_t now;

    snprintf(boardfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);
    if (f_get_file(boardfile, &currentdir, 1) != 0)
        return -1;
    if (boardisexist(currentdir, newboard))
    {
        http_quit_print("该板已经存在于该目录,无需再次添加.");
    }
    if (currentdir.boardnum < 20)
    {
        currentdir.boardnum++;
        snprintf(currentdir.boards[currentdir.boardnum - 1], 80, "%s",
                 newboard);
        f_replace_file(currentdir);
    }
    else
    {
        if (currentdir.extends[0] == 0)
        {
            now = getfilename();
            snprintf(currentdir.extends, 80, "%ld", now);
            f_replace_file(currentdir);
            init_dir(&newstruct, currentdir.extends, boardfile);
            newstruct.isextends = 1;
            f_replace_file(newstruct);
            do_addboard(currentdir.extends, newboard);
        }
        else
        {
            do_addboard(currentdir.extends, newboard);
        }
    }
    return 0;

}
static void print_dir(struct boardcollection currentdir)
{
    int i;

    for (i = 0; i < currentdir.subnum; i++)
    {
        totalij++;
        printf
            ("<tr><td align=\"center\"><font color=blue style='font-size:14px'>%d</font></td>",
             totalij);
        printf
            ("<td align=center><img src=/gif/folder.gif name=img1>&nbsp;</td>");
        printf
            ("<td><font style='font-size:14px'><a href=\"bbsshowhome?boardfile=%s\">%s</a></font></td>",
             currentdir.subdirsfile[i], currentdir.subdirname[i]);
        printf
            ("<td align=\"center\"><font style='font-size:14px'>目录</font></td>");
        printf
            ("<td align=center><font style='font-size:14px'><a href=\"bbsshowhome?boardfile=%s&targetfile=%s&deldir=%s\">删除</a></font></td></tr>",
             showboard, currentdir.dirname, currentdir.subdirsfile[i]);
    }
    if (currentdir.extends[0] != 0)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return;
        print_dir(currentdir);
    }
}
static void print_board(struct boardcollection currentdir)
{
    int j;

    for (j = 0; j < currentdir.boardnum; j++)
    {
        totalij++;
        printf
            ("<tr><td align=\"center\"><font color=blue style='font-size:14px'>%d</font></td>",
             totalij);
        printf("<td align=center><img src=/gif/file.gif></td>");
        printf
            ("<td><font style='font-size:14px'><a href=\"bbsdoc?board=%s\">%s</a></font></td>",
             currentdir.boards[j], currentdir.boards[j]);
        printf
            ("<td align=\"center\"><font style='font-size:14px'>讨论区</font></td>");
        printf
            ("<td align=center><font style='font-size:14px'><a href=\"bbsshowhome?boardfile=%s&targetfile=%s&delboard=%s\">删除</a></font></td></tr>",
             showboard, currentdir.dirname, currentdir.boards[j]);
    }
    if (currentdir.extends[0] != 0)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return;
        print_board(currentdir);
    }
}
static int removefile(char *file)
{
    char filename[80];

    snprintf(filename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid, file);
    if (remove(filename) == 0)
        return 0;
    return -1;

}
static int struct_del_dir(struct boardcollection *dirr, char *deldir)
{
    int i;

    for (i = 0; i < dirr->subnum; i++)
    {
        if (!strcasecmp(dirr->subdirsfile[i], deldir))
        {
            snprintf(dirr->subdirsfile[i], 80, "%s",
                     dirr->subdirsfile[dirr->subnum - 1]);
            snprintf(dirr->subdirname[i], 80, "%s",
                     dirr->subdirname[dirr->subnum - 1]);
            dirr->subdirsfile[dirr->subnum - 1][0] = 0;

            dirr->subnum--;

            break;
        }
    }

    return 0;
}
static int struct_del_board(struct boardcollection *dirr, char *delboard)
{
    int i;

    for (i = 0; i < dirr->boardnum; i++)
    {
        if (!strcasecmp(dirr->boards[i], delboard))
        {
            snprintf(dirr->boards[i], 80, "%s",
                     dirr->boards[dirr->boardnum - 1]);
            dirr->boards[dirr->boardnum - 1][0] = 0;
            dirr->boardnum--;
            break;
        }
    }
    return 0;
}
static int do_del_nocare(struct boardcollection dirr)
{
    int i;
    struct boardcollection temp;

    for (i = 0; i < dirr.subnum; i++)
    {
        if (f_get_file(dirr.subdirsfile[i], &temp, 1) != 0)
            return -1;;
        do_del_nocare(temp);
    }
    if (dirr.extends[0] != 0)
    {
        if (f_get_file(dirr.extends, &temp, 1) != 0)
            return -1;
        do_del_nocare(temp);
    }
    removefile(dirr.dirname);
    return 0;
}
static int do_deldir(struct boardcollection updir, char *deldir)
{
    int num, subnum;
    struct boardcollection dirr;
    struct boardcollection upupdir;

    if (f_get_file(deldir, &dirr, 1) != 0)
        return -1;
    num = getdirnumber();
    subnum = getsubdirnum(deldir) + 1;
    do_del_nocare(dirr);
    struct_del_dir(&updir, deldir);
    if (updir.isextends == 1 && updir.boardnum == 0 && updir.subnum == 0)
    {
        if (updir.extends[0] == 0)
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            upupdir.extends[0] = 0;
            f_replace_file(upupdir);
        }
        else
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            snprintf(upupdir.extends, 80, "%s", updir.extends);
            f_replace_file(upupdir);
        }
        removefile(updir.dirname);
    }
    else
    {
        f_replace_file(updir);
    }
    freshdirnum(num - subnum);
    return 0;

}
static int do_delboard(struct boardcollection updir, char *delboard)
{
    struct boardcollection upupdir;

    struct_del_board(&updir, delboard);
    if (updir.isextends == 1 && updir.boardnum == 0 && updir.subnum == 0)
    {
        if (updir.extends[0] == 0)
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            upupdir.extends[0] = 0;
            f_replace_file(upupdir);
        }
        else
        {
            if (f_get_file(updir.updir, &upupdir, 1) != 0)
                return -1;
            snprintf(upupdir.extends, 80, "%s", updir.extends);
            f_replace_file(upupdir);
        }
        removefile(updir.dirname);
    }
    else
    {
        f_replace_file(updir);
    }
    return 0;

}

static int check_parm(char *name)
{
    return (int) strchr(name, '/');
}

int bbsshowhome_main()
{
    char goodboard[80], currentfilename[80], bboard[80], boardfile[80],
        newdir[80], newboard[80];
    char redr[80], targetfile[80], deldir[80], delboard[80];
    char *board1, *title;
    int i = 0, find = 0, ndir = 0;
    struct boardcollection currentdir;
    FILE *fp;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客无法执行本操作，请先登录");
    totalij = 0;
    cgiFormString("boardfile", boardfile, 80);
    cgiFormString("targetfile", targetfile, 80);
    cgiFormString("deldir", deldir, 80);
    cgiFormString("delboard", delboard, 80);
    cgiFormString("boardfile", showboard, 80);
    cgiFormString("newdir", newdir, 80);
    cgiFormString("newboard", newboard, 80);
    if (check_parm(boardfile)
        || check_parm(targetfile)
        || check_parm(deldir)
        || check_parm(delboard)
        || check_parm(boardfile)
        || check_parm(newdir) || check_parm(newboard))
    {
        printf("错误参数");
        http_quit();
    }

    if (newdir[0] != 0)
    {
        if ((ndir = getdirnumber()) == -1)
            goto end;
        if (ndir >= MAX_DIRNUM)
        {
            printf
                ("<html><meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\n");
            printf
                ("<link rel=stylesheet type=text/css href='/css/bbs.css'>\n");
            printf
                ("<link rel=stylesheet type=text/css href='/css/blue.css'>\n");
            printf
                ("<font class=\"title\">饮水思源 -添加目录</font><hr>\n");
            printf
                ("您已超过最大目录数%d的限制,请按<a href=bbsshowhome?boardfile=%s>这里</a>返回\n",
                 MAX_DIRNUM, boardfile);
            http_quit();
        }
        if (do_adddir(boardfile, newdir) == 0)
        {
            ndir++;
            freshdirnum(ndir);
        }
        snprintf(redr, 80, "bbsshowhome?boardfile=%s", boardfile);
        redirect(redr);
        return 0;
    }
    if (newboard[0] != 0)
    {
        for (i = 0; i < MAXBOARD; i++)
        {
            board1 = shm_bcache->bcache[i].filename;
            title = shm_bcache->bcache[i].title;
            if (!has_read_perm(&currentuser, board1))
                continue;
            if (!strcasecmp(newboard, board1))
            {
                find++;
                snprintf(newboard, 80, "%s", board1);
                break;
            }
        }
        if (find == 0)
        {
            printf
                ("<html><meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">");
            printf
                ("<link rel=stylesheet type=text/css href='/css/bbs.css'>");
            printf
                ("<link rel=stylesheet type=text/css href='/css/blue.css'>");
            printf
                ("<font class=\"title\">饮水思源 -添加讨论区</font><hr>");
            printf
                ("您所要添加的讨论区不存在,请按<a href=bbsshowhome?boardfile=%s>这里</a>返回",
                 boardfile);
            http_quit();
        }
        do_addboard(boardfile, newboard);
        snprintf(redr, 80, "bbsshowhome?boardfile=%s", boardfile);
        redirect(redr);
        return 0;
    }
    if (deldir[0] != 0)
    {
        if (f_get_file(targetfile, &currentdir, 1) != 0)
            goto end;
        do_deldir(currentdir, deldir);
        snprintf(redr, 80, "bbsshowhome?boardfile=%s", boardfile);
        redirect(redr);
        return 0;
    }
    if (delboard[0] != 0)
    {
        if (f_get_file(targetfile, &currentdir, 1) != 0)
            goto end;
        do_delboard(currentdir, delboard);
        snprintf(redr, 80, "bbsshowhome?boardfile=%s", boardfile);
        redirect(redr);
        return 0;
    }
    printf
        ("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">");
    printf("<link rel=stylesheet type=text/css href='/css/bbs.css'>");
    printf("<link rel=stylesheet type=text/css href='/css/blue.css'>");
    printf
        ("<body topmargin=0 leftmargin=0><br><center><font color=blue style='font-size:18px'><b>饮水思源BBS-个人收藏夹</b></font><hr><br>");
    printf("<table border=\"1\" align=\"center\">");
    printf
        ("<tr><td>序号</td><td>&nbsp;</td><td align=center>讨论区名称</td><td align=center>类别</td><td align=center>删除<td>");

    snprintf(currentfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);

    if (f_get_file(boardfile, &currentdir, 1) == -1)
    {
        if (!strcmp(boardfile, "main.hck"))
        {
            init_dir(&currentdir, "main.hck", "main.hck");
            snprintf(goodboard, 80, "home/%c/%c/%s/.goodbrd",
                     toupper(currentuser.userid[0]),
                     toupper(currentuser.userid[1]), currentuser.userid);
            fp = fopen(goodboard, "r+");
            if (fp == NULL)
            {
                f_replace_file(currentdir);
                goto end;
            }
            while (fgets(bboard, 80, fp) != NULL)
            {
                char *ptr;

                ptr = strstr(bboard, "\n");
                if (ptr)
                    *ptr = '\0';
                snprintf(currentdir.boards[currentdir.boardnum++], 80,
                         "%s", bboard);
            }
            fclose(fp);
            f_replace_file(currentdir);
        }
        else
        {
            goto end;
        }
    }
    if (strcmp(boardfile, "main.hck"))
    {
        printf("<tr><td align=\"center\"> &nbsp;</td>");
        printf("<td align=center><img src=/gif/dir.gif> </td>");
        printf
            ("<td align=\"center\" ><font style='font-size:14px'><a href=\"bbsshowhome?boardfile=%s\">返回上一级</a></td>",
             currentdir.updir);
        printf("<td align=\"center\" ><font style='font-size:14px'></td>");
        printf("<td align=\"center\" ></td></tr>");
    }
    print_dir(currentdir);
    print_board(currentdir);
    totalij = 0;

  end:printf("</table><hr>");
    printf("<center>");
    printf
        ("<form action=\"bbsshowhome?boardfile=%s\">增加目录名<input name=newdir size=24 maxlength=20 type=text value="
         "><input type=submit value=确定><input type=hidden name=select value=0>",
         currentdir.dirname);
    printf("<input type=hidden name=boardfile value=\"%s\"></form>",
           boardfile);
    printf
        ("<form action=\"bbsshowhome?boardfile=%s\">增加讨论区<input name=newboard size=24 maxlength=20 type=text value="
         "><input type=submit value=确定><input type=hidden name=select value=0>",
         currentdir.dirname);
    printf("<input type=hidden name=boardfile value=\"%s\"></form>",
           boardfile);
    printf("</center>");
    printf("</form>");
    printf("</body>");
    printf("</html>");
    return 0;
}
