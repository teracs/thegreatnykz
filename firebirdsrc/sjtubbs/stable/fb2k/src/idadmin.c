/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STRLEN		80
#define ADMIN_NS	15
#define BBSHOME		"/home/bbs/bbshome"
#define FILE_PATH	"0Announce/reserve/IDAdmin/idadmin"
#define BLOCK_SIZE	50

struct id_statistic
{
    int accept;
    int reject;
    int del;
};

int cal_date(char *datestr, time_t now)
{
    struct tm *tm;

    tm = localtime(&now);
    sprintf(datestr, "%4d年%02d月%02d日%02d:%02d:%02d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    return (tm->tm_sec % 10);
}

int get_file_size(char *fullpath)
{
    int size = 0;
    FILE *fp = NULL;

    fp = fopen(fullpath, "r");
    if (fp == NULL)
        return 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

void draw_bar(char *buf, int number)
{
    int count;
    int i;

    char block[2] = { "■" };
    count = (number + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (count > 120)
        count = 120;
    for (i = 0; i < count; i++)
    {
        memcpy(buf + i * 2, block, 2);
    }
    return;
}

int main(void)
{
    char mode[3][10] = {
        "accept",
        "reject",
        "del"
    };
    FILE *fpz = NULL;
    int nCount = 0;
    char id_table[ADMIN_NS][16];
    struct id_statistic data[ADMIN_NS];
    int i = 0;
    char path[STRLEN];
    FILE *fp = NULL;
    int time_value = 0;
    char date_str1[STRLEN];
    char date_str2[STRLEN];
    char bar[255];

    chdir(BBSHOME);

    fpz = fopen("./vote/IDAdmin/secnotes", "rb");
    if (fpz == NULL)
        return 0;
    memset(id_table, 0, sizeof(id_table));
    while (nCount < ADMIN_NS)
    {
        fgets(id_table[nCount], 16, fpz);
        id_table[nCount][strlen(id_table[nCount]) - 1] = '\0';
        if (strlen(id_table[nCount]) <= 1)
            break;
        nCount++;
    }
    printf("Load Userid Ns = %d\n", nCount);
    memset(data, 0x00, ADMIN_NS * sizeof(struct id_statistic));
    for (i = 0; i < nCount; i++)
    {
        sprintf(path, "./etc/idadmin.%s.%s", id_table[i], mode[0]);
        printf("%s\n", path);
        data[i].accept = (get_file_size(path)) / 2;
        if (data[i].accept > 0)
        {
            unlink(path);
        }
        printf("%s\n", path);
        sprintf(path, "./etc/idadmin.%s.%s", id_table[i], mode[1]);
        data[i].reject = (get_file_size(path)) / 2;
        if (data[i].reject > 0)
        {
            unlink(path);
        }
        printf("%s\n", path);
        sprintf(path, "./etc/idadmin.%s.%s", id_table[i], mode[2]);
        data[i].del = (get_file_size(path)) / 2;
        if (data[i].del > 0)
        {
            unlink(path);
        }
    }

    fp = fopen(FILE_PATH, "w");
    if (fp == NULL)
        return 0;

    time_value = time(0);
    cal_date(date_str1, time_value - 86400 * 7);
    cal_date(date_str2, time_value);

    fprintf(fp, "\t\t\t\t\t\t帐号管理员工作情况统计\n\n");
    fprintf(fp, "\t\t\t%s - %s\n\n", date_str1, date_str2);
    for (i = 0; i < nCount; i++)
    {
        fprintf(fp,
                "\033[1;35m☆──────────────────────────────────────☆\033[m\n\n");
        fprintf(fp, "%s:\n\n", id_table[i]);
        fprintf(fp, "通过：\t");
        memset(bar, 0x00, 255);
        draw_bar(bar, data[i].accept);
        fprintf(fp, "\033[1;32m%s\033[m %d人\n", bar, data[i].accept);

        fprintf(fp, "拒绝：\t");
        memset(bar, 0x00, 255);
        draw_bar(bar, data[i].reject);
        fprintf(fp, "\033[1;31m%s\033[m %d人\n", bar, data[i].reject);

        fprintf(fp, "删除：\t");
        memset(bar, 0x00, 255);
        draw_bar(bar, data[i].del);
        fprintf(fp, "\033[1;33m%s\033[m %d人\n\n", bar, data[i].del);
    }

    fclose(fp);
    return 1;

}
