/*********************************************************
   write by: deardragon@163.net     date: 2000.11.26      
   fb2000.dhs.org                   [投票记录的升级程序]
   编译方法： gcc -o revote revote.c
 *********************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#define  IDLEN 	12
#define  STRLEN 80

struct old_votebal
{
    char userid[IDLEN + 1];
    char title[STRLEN];
    char type;
    char items[32][38];
    int maxdays;
    int maxtkt;
    int totalitems;
    time_t opendate;
}
oldvote;

struct votebal
{
    char userid[16];
    char title[STRLEN];
    char type;
    char items[32][40];
    int maxdays;
    int maxtkt;
    int totalitems;
    time_t opendate;
    unsigned int level;
    unsigned int x_logins;      //上站次数
    unsigned int x_posts;       //发表文章数
    unsigned int x_stay;        //上站的实际小时数
    unsigned int x_live;        //注册的物理时间
}
newvote;

char genbuf[1024];

int Isdir(char *fname)
{
    struct stat st;

    return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}

void update_vote(char *fname)
{
    FILE *old, *new;
    struct stat st;
    int i;
    char Cfile[80], newCfile[80];

    sprintf(Cfile, "%s/control", fname);
    sprintf(newCfile, "%s/control.new", fname);
    if (stat(Cfile, &st) == -1)
    {
        printf("%-12s: no VOTE.\n", fname);
        return;
    }
    old = fopen(Cfile, "r");
    if (old == NULL)
    {
        printf("%-12s: open Error.\n", fname);
        return;
    }
    new = fopen(newCfile, "w+");
    if (new == NULL)
    {
        fclose(old);
        printf("%-12s: creat Error.\n", fname);
        return;
    }
    printf("%-12s: Update now...\n", fname);
    i = 1;
    while (fread(&oldvote, sizeof(oldvote), 1, old) > 0)
    {
        printf("    vote[%d]: %-50s ", i++, oldvote.title);
        memset(&newvote, 0, sizeof(newvote));
        memcpy(newvote.userid, oldvote.userid, sizeof(oldvote.userid));
        memcpy(newvote.title, oldvote.title, sizeof(oldvote.title));
        newvote.type = oldvote.type;
        memcpy(newvote.items, oldvote.items, sizeof(oldvote.items));
        newvote.maxdays = oldvote.maxdays;
        newvote.maxtkt = oldvote.maxtkt;
        newvote.totalitems = oldvote.totalitems;
        newvote.opendate = oldvote.opendate;
        newvote.level = 0;
        newvote.x_logins = 0;
        newvote.x_posts = 0;
        newvote.x_stay = 0;
        newvote.x_live = 0;
        if (!fwrite(&newvote, sizeof(newvote), 1, new))
        {
            printf("write failed!\n");
            fclose(old);
            fclose(new);
            return;
        }
        printf("done.\n");
    }
    fclose(new);
    fclose(old);
    rename(newCfile, Cfile);
}

int main(int argc, char *argv[])
{
    DIR *dirp;
    struct dirent *de;
    int ch;
    char *fname, currPath[80];

    if (argc != 2)
    {
        printf("  Useage: %s BBSHOME\n", argv[0]);
        return 1;
    }
    sprintf(currPath, "%s/vote", argv[1]);
    if (chdir(currPath) == -1)
    {
        printf("目录 [%s] 不正确。\n", currPath);
        return -1;
    }
    strcpy(currPath, ".");
    if (!(dirp = opendir(currPath)))
    {
        printf("Error! Unable to enter [vote]\n");
        return -1;
    }
    while (de = readdir(dirp))
    {
        fname = de->d_name;
        ch = fname[0];
        if (ch != '.')
        {
            if (Isdir(fname))
                update_vote(fname);
        }
    }
    closedir(dirp);
    exit(0);
}
