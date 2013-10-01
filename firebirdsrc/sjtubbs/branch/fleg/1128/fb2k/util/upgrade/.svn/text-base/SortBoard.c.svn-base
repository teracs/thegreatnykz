/*
	这个文件可以用于整理 BBSHOME/.BOARDS 文件
	例如本文件的作用是清除每个版的斑竹名单 ;pp
*/
/* gcc -O2 -I../../include -o SortBoard  SortBoard.c ../../src/record.c  */

#include "bbs.h"
struct boardheader fh;
void report()
{}
;
int main(void)
{
    FILE *fp;
    int i = 1;
    char src[STRLEN], des[STRLEN];

    sprintf(src, "%s/.BOARDS", BBSHOME);
    strcpy(des, ".BOARDS.new");

    while (1)
    {
        if (get_record(src, &fh, sizeof(fh), i) == -1)
            break;
        if (fh.filename[0] == '\0')
        {
            printf("No.%2d invalidation, ship...\n", i);
        }
        else
        {
            fh.BM[0] = '\0';
            append_record(des, &fh, sizeof(fh));
        }
        i++;
    }
    printf("Total boards %d .\n", i - 1);
}
