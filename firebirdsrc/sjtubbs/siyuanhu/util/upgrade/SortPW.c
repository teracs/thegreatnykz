/*
	这个文件可以整理 .PASSWDS 文件。
 
  编译方法：
  gcc -O2 -I../../include -o SortPW SortPW.c 
  编译无错后，把 .PASSWDS 文件cp 到当前目录，执行 ./SortPW 
*/

#include "bbs.h"

struct userec old, new;

int main(void)
{
    int i, j;
    FILE *src, *dst;

    src = fopen(".PASSWDS", "rb");
    if (!src)
    {
        printf("you must put your old .PASSWDS into this directory.\n");
        exit(3);
    }
    dst = fopen(".PASSWDS.new", "wb");
    j = 0;
    for (i = 0;; i++)
    {
        if (fread(&old, sizeof(old), 1, src) <= 0)
            break;
        if (strlen(old.userid) <= 0)
            continue;
        printf("Processing #%d userid: %s\n", i + 1, old.userid);
        memset(&new, 0, sizeof(new));
        memcpy(&new, &old, sizeof(old));

        new.userid[IDLEN + 1] = '\0';
        new.lasthost[15] = '\0';
        new.passwd[PASSLEN - 1] = '\0';
        new.username[NAMELEN - 1] = '\0';
        new.ident[NAMELEN - 1] = '\0';
        new.termtype[15] = '\0';
        new.reginfo[STRLEN - 17] = '\0';
        new.realname[NAMELEN - 1] = '\0';
        new.address[STRLEN - 1] = '\0';
        new.email[STRLEN - 13] = '\0';

        printf("  writing, sizeof(old)=%d sizeof(new)=%d ..",
               sizeof(old), sizeof(new));
        if (!fwrite(&new, sizeof(new), 1, dst))
        {
            printf("failed!!\n");
            exit(0);
        }
        else
        {
            printf("done.\n");
        }
        j++;
    }
    fclose(src);
    fclose(dst);
    printf("done. total %d of %d users transfeerd\n", j, i + 1);
    printf("remember clean shm before replace new PASSWDS file!\n");
    return 0;
}
