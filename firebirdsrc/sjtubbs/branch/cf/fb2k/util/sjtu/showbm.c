/*
 * File showbm.c - 板主表现统计表
 */
/* $Id: showbm.c,v 1.2 2004/06/29 14:59:23 rwzmm Exp $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "../../include/bbs.h"
#include "showbm.h"

void fprintchar(FILE * fp0, int num, char ch)
{
    int i;

    for (i = 0; i < num; i++)
        fprintf(fp0, "%c", ch);
}

/*
 * getpath - 取得精华区所在目录
 * 返回值不为零表示精华区操作严重出错
 */
int getpath(char *filename, char *pathname)
{
    FILE *fp;
    char lpathname[256];        /* local pathname */

    sprintf(lpathname, "%s/0Announce/.Search", BBSHOME);
    fp = fopen(lpathname, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open .Search file.\n");
        return 1;
    }
    do
    {
        fscanf(fp, "%s", filename);
        filename[strlen(filename) - 1] = '\0';
    }
    while (strcmp(filename, pathname) && !feof(fp));
    fscanf(fp, "%s", filename);
    fclose(fp);

    return 0;
}

static int articles0 = -1;      /* 精华区文章总数 */
static int articles = -1;       /* 本板板主文章数 */
static int lastNarticles = -1;  /* 过去N天中本板文章数 */
static time_t lastmodified = 0; /* 精华区最后一次整理时间 */
static time_t lastbmpost = 0;   /* 板主在本板最后一次发表文章时间 */
static time_t lastpost = 0;     /* 本板最后一次发表文章时间 */

/*
 * afilecount - 精华区文章遍历 
 * 取得精华区文章总数和精华区最后整理时间
 * 返回值不为零表示本次遍历出错
 */
int afilecount(char *pathname)
{
    DIR *dp;
    struct dirent *link;

    dp = opendir(pathname);
    if (dp == NULL)
    {
        fprintf(stderr, "Can't open dir %s.\n", pathname);
        return 1;
    }
    while ((link = readdir(dp)) != 0)
    {
        struct stat st;
        char filename[256];

        if (*link->d_name != '.')
            articles0++;
        else
            continue;
        sprintf(filename, "%s", pathname);
        strcat(filename, "/");
        strcat(filename, link->d_name);
        stat(filename, &st);
        if (st.st_mtime > lastmodified)
            lastmodified = st.st_mtime;

        if ((st.st_mode & S_IFMT) == S_IFDIR)
            afilecount(filename);
    }
    closedir(dp);

    return 0;
}

/*
 * lastbmpostime - 板主本板最后发表文章时间
 * 由bmfilecount调用
 */
void lastbmpostime(char *pathname, char *filename)
{
    char lpathname[256];
    struct stat st;

    sprintf(lpathname, "%s", pathname);
    strcat(lpathname, filename);
    stat(lpathname, &st);
    if (st.st_mtime > lastbmpost)
        lastbmpost = st.st_mtime;
}

/*
 * lastpostime - 本板最后发表文章时间
 *             - 并且统计过去N天中本板文章数
 * 由bmfilecount调用
 */
void lastpostime(char *pathname, char *filename)
{
    char lpathname[256];
    struct stat st;

    sprintf(lpathname, "%s", pathname);
    strcat(lpathname, filename);
    stat(lpathname, &st);
    if (st.st_mtime > lastpost)
        lastpost = st.st_mtime;
    /* 统计过去N天中本板文章数 */
    if (st.st_mtime > time(0) - 60 * 60 * 24 * DAYSN)
        lastNarticles++;
}

/*
 * bmfilecount - 本板板主文章总数
 * 返回值不为零的情况一般不会出现, 因为前面主循环有过检测
 */
int bmfilecount(char *pathname, char *bm)
{
    int fd, index, total;
    char lpathname[256];
    struct fileheader *buffer;
    struct stat st;

    sprintf(lpathname, "%s", pathname);
    lpathname[strlen(lpathname) - 4] = '\0';
    fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Can not open .DIR file!\n");
        return 1;
    }
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct fileheader);
    buffer =
        (struct fileheader *) calloc(total, sizeof(struct fileheader));
    if (buffer == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(-1);
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        free(buffer);
        fprintf(stderr, "Can not read .DIR file!\n");
        return 1;
    }
    (void) close(fd);

    for (index = 0; index < total; index++)
    {
        lastpostime(lpathname, buffer[index].filename);
        if (!bm)
            continue;
        if (!strcmp(buffer[index].owner, bm))
        {
            articles++;
            lastbmpostime(lpathname, buffer[index].filename);
        }
    }
    free(buffer);

    return 0;
}

/*
 * 从.PASSWD取板主个人资料
 * 返回值不为零表示不能读.PASSWDS, 属严重出错.
 */
int getbmrec(struct userec *bmrec, char *currbm)
{
    FILE *fp;
    char pathname[256];

    sprintf(pathname, "%s/.PASSWDS", BBSHOME);
    fp = fopen(pathname, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error open .PASSWDS file.\n");
        return 1;
    }

    while (1)
    {
        if (fread(bmrec, sizeof(struct userec), 1, fp) <= 0)
            break;
        if (!strcmp(bmrec->userid, currbm))
            break;
    }
    fclose(fp);

    return 0;
}

/*
 * 多关键字起泡排序
 */
void bubblesort(struct boardheader *buffer, int *bsort, int total)
{
    int change, i, j;

    for (i = total - 1, change = 1; i > 1 && change; --i)
    {
        change = 0;
        for (j = 0; j < i; ++j)
            if (strcmp(buffer[bsort[j]].filename,
                       buffer[bsort[j + 1]].filename) > 0)
            {
                int temp;

                temp = bsort[j];
                bsort[j] = bsort[j + 1];
                bsort[j + 1] = temp;
                change = 1;
            }
    }

    for (i = total - 1, change = 1; i > 1 && change; --i)
    {
        change = 0;
        for (j = 0; j < i; ++j)
            if (buffer[bsort[j]].title[0] > buffer[bsort[j + 1]].title[0])
            {
                int temp;

                temp = bsort[j];
                bsort[j] = bsort[j + 1];
                bsort[j + 1] = temp;
                change = 1;
            }
    }
}

int main(void)
{
    char pathname0[256];        /* 精华区bmstat路径名 */
    char filename0[20];         /* bmstat/文件名 */
    FILE *fp0;                  /* 用于自动发文到精华区 */

    int fd, index, total;       /* 初使化及主循环用 */
    char pathname[256];         /* 路径名 */
    struct boardheader *buffer;
    struct stat st;
    struct userec bmrec;
    int idxsort, *bsort;        /* 用于排序 */
    static int board_num, bm_num;       /* 总计 */

    sprintf(pathname, "%s/.BOARDS", BBSHOME);
    fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Error open .BOARDS file!\n");
        exit(-1);
    }
    fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer = (struct boardheader *)
             calloc((size_t) total, sizeof(struct boardheader));
    if (buffer == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(-1);
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        fprintf(stderr, "Can not read .BOARDS file!\n");
        exit(-1);
    }
    close(fd);

    board_num = 0;
    bm_num = 0;

    /* 打开精华区待写文件 */
    sprintf(pathname0, "%s/0Announce/bmstat/", BBSHOME);
    sprintf(filename0, "M.%ld.A", time(0));
    strcat(pathname0, filename0);
#ifdef DEBUG

    printf("pathname = %s\n", pathname0);
#endif

    fp0 = fopen(pathname0, "w");
    if (fp0 == NULL)
    {
        fprintf(stderr, "Error open file %s for write.\n", filename0);
        exit(-1);
    }
#ifndef PRINTOFF
    fprintf(fp0, "%s", head);
#endif

    /* 排序 */
    bsort = (int *) malloc(total * sizeof(int));
    for (index = 0; index < total; index++)
        bsort[index] = index;
    bubblesort(buffer, bsort, total);

    /* 主循环 */
    for (idxsort = 0; idxsort < total; idxsort++, index = bsort[idxsort])
    {
        int count;              /* 本板文章总数 */
        char pathname[256];
        char filename[80];      /* pathname = pathname + filename */
        char boardtitles[STRLEN];       /* 用于中文板名预处理 */
        char boardtitle[STRLEN];        /* 提取并截断太长的中文板名 */

        char bms[BM_LEN - 1];   /* 给strtok()用 */
        char *bm;               /* 当前板主(副)指针 */
        int bmflag = 0;         /* 板副标志, 0-板副1 , 1-板副2 */

        if (buffer[index].level > 0)
            continue;           /* Read/Post限制 */
        if ((buffer[index].flag & ANONY_FLAG) == ANONY_FLAG)
            continue;           /* 匿名板 */
        sprintf(pathname, "%s/boards/%s/.DIR", BBSHOME,
                buffer[index].filename);
        fd = open(pathname, O_RDONLY);
        if (fd == -1)
            continue;           /* 滤去新建后未发表文章的板 */
        (void) fstat(fd, &st);
        /* 板上文章总数 */
        count = st.st_size / sizeof(struct fileheader);
        (void) close(fd);

        /* 中文板名 */
        strcpy(boardtitles, buffer[index].title);
        {
            char *p, *q;

            boardtitles[STRLEN - 1] = '\0';
            boardtitle[0] = '\0';
            // 取中文版名: 最后一个空格字符后面的字符串
            p = q = boardtitles;
            while (*q != '\0')
            {
                q++;
                if (*q == ' ')
                    p = q + 1;
            }
            if (*p == '\0')
                continue;
            /*
               p = strtok (boardtitles, " ");
               p = strtok (NULL, " ");
               p = strtok (NULL, "\0");
               while (*p == ' ')
               p++;
             */
            strcat(boardtitle, p);
        }

        if (strlen(boardtitle) >= 35)
        {
            boardtitle[32] = '.';
            boardtitle[33] = '.';
            boardtitle[34] = '.';
            boardtitle[35] = ')';
            boardtitle[36] = '\0';
        }
        else
        {
            strcat(boardtitle, ")");
            boardtitle[35] = '\0';
        }

        /* 本板无新文章天数 */
        lastpost = 0;

        /* 精华区文章总数 */
        sprintf(pathname, "%s/0Announce/", BBSHOME);
        if (!getpath(filename, buffer[index].filename))
        {
            strcat(pathname, filename);
            articles0 = 0;      /* init */
            lastmodified = 0;
            if (afilecount(pathname))
                articles0 = -1; /* 本次精华区遍历出错 */
        }
        /* else 找不到精华区的.Search file, 精华区数据无效. */

        sprintf(pathname, "%s/boards/", BBSHOME);
        strcat(pathname, buffer[index].filename);
        strcat(pathname, "/.DIR");

        /* 板主 */
        sprintf(bms, "%s", buffer[index].BM);
        bm = strtok(bms, " ");  /* strtok()是一个分解字串的ANSI函数 */

        /* 板上板主文章数 */
        articles = 0;
        lastbmpost = 0;
        lastNarticles = 0;
        bmfilecount(pathname, bm);      /* 允许 bm == NULL */

        if (bm)
        {
            if (getbmrec(&bmrec, bm))
                continue;

            /* 滤去板主为SYSOP的板 */
            if (!strcmp(bm, "SYSOP"))
                continue;

            bm_num++;

        }
        /* 滤去无板主的板 */
        /*
           else
           continue;            
         */

#ifndef PRINTOFF
        fprintf(fp0, "%-17s", buffer[index].filename);
        /*
           fprintf (fp0, " %5d", count);
         */
        fprintf(fp0, " %5d", lastNarticles);
        fprintf(fp0, " ");
        if ((time(0) - lastpost) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastpost) / 60 / 60 / 24 > NOFILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastpost) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastpost) / 60 / 60 / 24);
        fprintf(fp0, " ");
        fprintf(fp0, "%6d", articles0);
        fprintf(fp0, " ");
        if ((time(0) - lastmodified) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastmodified) / 60 / 60 / 24 > NO0FILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastmodified) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastmodified) / 60 / 60 / 24);
        fprintf(fp0, " ");
#endif

#ifndef PRINTOFF

        if (bm == NULL)
        {
            fprintf(fp0, "%s%s%s\n", "[1;32m", "无", "[m");
            board_num++;
#ifdef DEBUG

            fprintf(fp0, "(%-43s\n", buffer[index].title);
#else

            fprintf(fp0, "(%-43s\n", boardtitle);
#endif

            continue;           /* 处理无板主的板 */
        }
        fprintf(fp0, "%-12s", bm);
        fprintf(fp0, " ");

        /*
           fprintf (fp0, "%5d", bmrec.numposts);
           fprintf (fp0, " ");
         */
        fprintf(fp0, "%6d", articles);
        fprintf(fp0, " ");
        if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > BMNOLOGIN)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - bmrec.lastlogin) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld",
                    (time(0) - bmrec.lastlogin) / 60 / 60 / 24);
        fprintf(fp0, " ");
        if ((time(0) - lastbmpost) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastbmpost) / 60 / 60 / 24 > BMNOFILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastbmpost) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastbmpost) / 60 / 60 / 24);

        fprintf(fp0, "\n");
#endif

#ifndef PRINTOFF
#ifdef DEBUG

        fprintf(fp0, "(%-43s", buffer[index].title);
#else

        fprintf(fp0, "(%-43s", boardtitle);
#endif
#endif

        /* 板副 */
        while ((bm = strtok(NULL, " ")) != NULL)
        {
            if (getbmrec(&bmrec, bm))
            {
                fprintf(fp0, "\n");
                continue;
            }
            /* 板上板副文章数 */
            articles = 0;
            lastbmpost = 0;
            bmfilecount(pathname, bm);

#ifndef PRINTOFF

            if (bmflag)
                fprintchar(fp0, 44, ' ');
            fprintf(fp0, " ");
            fprintf(fp0, "%-12s", bm);
            fprintf(fp0, " ");

            /*
               fprintf (fp0, "%5d", bmrec.numposts);
               fprintf (fp0, " ");
             */
            fprintf(fp0, "%6d", articles);
            fprintf(fp0, " ");
            if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > 999)
                fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
            else if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 >
                     BMNOLOGIN)
                fprintf(fp0, "%s%6ld%s", "[1;31m",
                        (time(0) - bmrec.lastlogin) / 60 / 60 / 24, "[m");
            else
                fprintf(fp0, "%6ld",
                        (time(0) - bmrec.lastlogin) / 60 / 60 / 24);
            fprintf(fp0, " ");
            if ((time(0) - lastbmpost) / 60 / 60 / 24 > 999)
                fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
            else if ((time(0) - lastbmpost) / 60 / 60 / 24 > BMNOFILEDAYS)
                fprintf(fp0, "%s%6ld%s", "[1;31m",
                        (time(0) - lastbmpost) / 60 / 60 / 24, "[m");
            else
                fprintf(fp0, "%6ld",
                        (time(0) - lastbmpost) / 60 / 60 / 24);

            fprintf(fp0, "\n");
#endif

            bmflag++;
            bm_num++;
        }

#ifndef PRINTOFF
        if (!bmflag)
            fprintf(fp0, "\n");
#endif

        board_num++;
    }

    free(buffer);
    free(bsort);

#ifndef PRINTOFF

    fprintf(fp0, "---------------------------------------\n");
    fprintf(fp0, "总共 %d 个板, %d 个板主(有重复计)在表中.\n",
            board_num, bm_num);
#endif

    fclose(fp0);

    /* 写精华区0Announce/bmstat/.Names文件 */
    sprintf(pathname0, "%s/0Announce/bmstat/.Names", BBSHOME);
    fp0 = fopen(pathname0, "a");
    if (fp0 == NULL)
    {
        fprintf(stderr, "Error open file %s for write.\n", ".Names");
        exit(-1);
    }
    fprintf(fp0, "%-44s%s\n", "Name=板主表现统计", "deliver");
    fprintf(fp0, "Path=~/%s\n", filename0);
    fprintf(fp0, "Numb=1\n#\n");
    fclose(fp0);

    return 0;
}
