
#include "www.h"

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
static void print_dir(struct boardcollection currentdir, int level)
{
    int i, j;

    for (i = 0; i < currentdir.subnum; i++)
    {
        printf("<div>");
        for (j = 1; j <= level; j++)
            printf("<img src='/gif/blank.gif'>");
        printf
            ("<a href=javascript:submenu('%s',%d) target=_self><img id='img_%s' src='/gif/folderclose.gif' class='pm'></a>",
             currentdir.subdirsfile[i], level + 1,
             currentdir.subdirsfile[i]);
        printf
            ("<a class=left href='bbsshowhome?boardfile=%s' target='f3'>%s</a></div>",
             currentdir.subdirsfile[i], currentdir.subdirname[i]);
        printf("<div id='%s' style='display:none'></div>",
               currentdir.subdirsfile[i]);
    }
    if (currentdir.extends[0] != 0 && currentdir.subnum >= 20)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return;
        print_dir(currentdir, level);
    }
}
static void print_board(struct boardcollection currentdir, int level)
{
    int i, j;

    for (i = 0; i < currentdir.boardnum; i++)
    {
        printf("<div>");
        for (j = 1; j <= level; j++)
            printf("<img src='/gif/blank.gif'>");
        printf("<img src='/gif/board.gif'>");
        printf
            ("<a class=left href='bbsdoc?board=%s' target='f3'>%s</a></div>",
             currentdir.boards[i], currentdir.boards[i]);
    }
    if (currentdir.extends[0] != 0 && currentdir.boardnum >= 20)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return;
        print_board(currentdir, level);
    }
}

int bbsshowmenu_main()
{
    char boardfile[80], currentfilename[80], goodboard[80], bboard[80];
    int level;
    struct boardcollection currentdir;
    FILE *fp;

    init_all();
    if (!loginok)
    {
        http_quit();
    }
    cgiFormInteger("level", &level, 180);
    cgiFormString("boardfile", boardfile, 80);
    if (strchr(boardfile, '/'))
    {
        printf("´íÎó²ÎÊý");
        http_quit();
    }
    printf
        ("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">");
    printf("<link rel=stylesheet type=text/css href='/css/bbs.css'>");
    printf("<link rel=stylesheet type=text/css href='/css/blue.css'>");
    printf("</head>");
    printf("<body class=left>");
    printf("<script language=\"javascript\">");
    printf("parent.document.getElementById(\"%s\").innerHTML=\"",
           boardfile);
    printf("<input type=hidden name='temp' id='temp' value=''>");
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
    printf("<div id='%s'>", boardfile);
    print_dir(currentdir, level);
    print_board(currentdir, level);
    printf("</div>");

  end:
    printf("\";</script>");
    printf("</body>");
    printf("</html>");
    return 0;
}
