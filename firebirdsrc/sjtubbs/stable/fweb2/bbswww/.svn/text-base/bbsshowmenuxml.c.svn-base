
#include "www.h"

static void init_dir(struct boardcollection *dirr, char *dirname,
                     char *updir)
{
    int i = 0;

    dirr->subnum = 0;
    dirr->boardnum = 0;
    dirr->isextends = 0;
    sprintf(dirr->dirname,  dirname, toupper(currentuser.userid[0]),
             currentuser.userid);
    sprintf(dirr->updir,  updir, toupper(currentuser.userid[0]),
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
    else {
        if(write(sd, &sstruct, sizeof(struct boardcollection))) {
            close(sd);
    	    return 0;
	} else 
	    return -1;
    }
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
    int i;

    for (i = 0; i < currentdir.subnum; i++)
    {
        printf("<dentry>\n");
	printf("<dboardfile>%s</dboardfile>\n",currentdir.subdirsfile[i]);
	printf("<dlevel>%d</dlevel>\n",level + 1);
	printf("<dname><![CDATA[ %s ]]></dname>\n",currentdir.subdirname[i]);
	printf("</dentry>\n");
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
    int i;

    for (i = 0; i < currentdir.boardnum; i++)
    {
        printf("<bentry>");
	printf("<bname>%s</bname>",currentdir.boards[i]);
	printf("</bentry>\n");
    }
    if (currentdir.extends[0] != 0 && currentdir.boardnum >= 20)
    {
        if (f_get_file(currentdir.extends, &currentdir, 1) != 0)
            return;
        print_board(currentdir, level);
    }
}

int bbsshowmenuxml_main()
{
    char boardfile[80], currentfilename[80], goodboard[80], bboard[80];
    int level;
    struct boardcollection currentdir;
    FILE *fp;
    silence = 1;
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
    printf("Connection: close\n");
    printf("Content-type: text/xml;charset=gb2312\n\n");	
    printf
        ("<?xml version=\"1.0\" encoding=\"gb2312\"?>\n<!-- generator=\"fweb/xml mod 0.2\" -->\n");

    printf("<favourite>\n<boardfile>%s</boardfile>\n",boardfile);
    printf("<level>%d</level>\n",level);
    snprintf(currentfilename, 80, "home/%c/%c/%s/%s",
             toupper(currentuser.userid[0]),
             toupper(currentuser.userid[1]), currentuser.userid,
             boardfile);

    if (f_get_file(boardfile, &currentdir, 1) == -1)
    {
        if (!strcmp(boardfile, "main.hck"))
        {
            init_dir(&currentdir, "main.hck", "main.hck");
            sprintf(goodboard,  "home/%c/%c/%s/.goodbrd",
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
    printf("<favs>\n");
    print_dir(currentdir, level);
    print_board(currentdir, level);
    printf("</favs>\n</favourite>\n");

  end:
    return 0;
}
