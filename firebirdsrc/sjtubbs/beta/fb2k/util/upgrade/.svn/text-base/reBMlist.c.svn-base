/*
   Added by deardragon 1999.12.22 配合板主自动任免功能的板主列表转化程序 
   如果不对以前已经任命的板主进行转化的话，用板主离职功能，会提示该板主
   不是该板板主的说法，用它来修正。 
 
	请注意，本文件仅用于其他版本 BBS 转化成本站代码时的板主列表转化。
 		已经是本站代码的朋友，不要使用! 
 
编译本程序前，请把 #define BBSHOME "/home/bbs" 修改成你正确的路径。
 
编译方法： gcc -o reBMlist reBMlist.c 
使用方法： ./reBMlist 
 
*/

#define BBSHOME "/home/bbs"

#define MAX_BMS	3

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#define STRLEN  80
#define BM_LEN	60
struct boardheader
{            /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char owner[STRLEN - BM_LEN];
    char BM[BM_LEN - 4];
    char flag;
    char title[STRLEN];
    unsigned level;
    unsigned char accessed[12];
}
fh;

int get_record(char *filename, char *rptr, int size, int id)
{
    int fd;

    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return -1;
    if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        close(fd);
        return -1;
    }
    if (read(fd, rptr, size) != size)
    {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int main(void)
{
    FILE *fp;
    int bid = 1, bmnum, i, j;
    char usernames[MAX_BMS][STRLEN];
    char bmfile[STRLEN], BOARDS[STRLEN], ans[STRLEN];

    sprintf(BOARDS, "%s/.BOARDS", BBSHOME);
    while (1)
    {
        if (get_record(BOARDS, (char *) &fh, sizeof(fh), bid) == -1)
            break;
        bmnum = 0;
        for (i = 0, j = 0; fh.BM[i] != '\0'; i++)
        {
            if (fh.BM[i] == ' ')
            {
                if (j == 0)
                    continue;
                usernames[bmnum][j] = '\0';
                bmnum++;
                if (bmnum >= MAX_BMS)
                    break;
                j = 0;
            }
            else
            {
                usernames[bmnum][j++] = fh.BM[i];
            }
        }
        if (bmnum < MAX_BMS && j != 0)
            usernames[bmnum++][j] = '\0';
        if (fh.filename[0] == '\0')
        {
            printf("No.%2d invalidation, ship...\n", bid++);
        }
        else if (fh.BM[0] == '\0')
        {
            printf("No.%2d %16s   : [no boardmanager] ship...\n",
                   bid++, fh.filename);
        }
        else
        {
            printf("No.%2d %16s[%d]:%-40s append...\n",
                   bid++, fh.filename, bmnum, fh.BM);
            for (i = 0; i < bmnum; i++)
            {
                sprintf(bmfile, "%s/home/%c/%c/%s/.bmfile",
                        BBSHOME, toupper(usernames[i][0]), toupper(usernames[i][1]), usernames[i]);
                fp = fopen(bmfile, "a+");
                if (fp != NULL)
                {
                    fprintf(fp, "%s\n", fh.filename);
                    fclose(fp);
                }
            }
        }
    }
    printf("Total boards %d .\n", bid - 1);
}
