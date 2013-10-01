/*********************************************************
   write by: deardragon             date: 2000.11.09      
   fb2000.dhs.org                   [精华区索引]
 *********************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define HIDE(str) (strstr(str+43,"(BM: SYSOPS)")||strstr(str+43,"(BM: BMS)")||!memcmp(str+5,"<HIDE>",6))

char dotNames[] = ".Names";
char dotIndex[] = ".index";
char dotIndexNew[] = ".index.new";

/* visitnumFilename 是精华区计数器文件的名字，如果没有则用 "" */
//char visitnumFilename[] = "";
char visitnumFilename[] = "counter.person";

FILE *fpIndexNew;
int visited;
int ndir;
int nfile;
int dironly = 0;
char currPath[1024], currFile[1024];
char visitnumstr[20];
char printstr[1024];
char prefix[80];
char prefixstr[5][3] = { "├", "→", "└", "┬", "│" };

int Isdir(char *fname)
{
    struct stat st;
    char tempfilename[1024];

    sprintf(tempfilename, "%s/%s", currPath, fname);
    return (stat(tempfilename, &st) == 0 && S_ISDIR(st.st_mode));
}

void visitlog(char *fname)
{
    FILE *logfp;
    char tempfilename[1024], counter[9];

    if (visitnumFilename[0] == '\0')
    {
        visitnumstr[0] = '\0';
        return;
    }
    sprintf(tempfilename, "%s/%s/%s", currPath, fname, visitnumFilename);
    logfp = fopen(tempfilename, "r");
    if (logfp)
    {
        fscanf(logfp, "%s", counter);
        fclose(logfp);
    }
    else
        counter[0] = '\0';
    if (counter[0] != '\0')
        sprintf(visitnumstr, "\033[33m[%s]\033[m", counter);
}

int AnnounceIndex(int level)
{
    FILE *fpNames;
    int isdir, count;
    char NamesFile[1024], IndexFile[1024], IndexNewFile[1024];
    char buf[256], *bufptr, *ptr, currentpath[1024], title[80];

    //   char save_numstr[80];
    char save_prefix[80];
    char *bool;

    strcpy(currentpath, currPath);
    strcpy(NamesFile, currFile);
    if ((fpNames = fopen(NamesFile, "r")) == NULL)
        return -1;
    if (!level)
    {
        struct tm *tm;
        char weeknum[7][3] = { "天", "一", "二", "三", "四", "五", "六" };
        time_t now = time(0);

        tm = localtime(&now);
        sprintf(IndexNewFile, "%s/%s", currentpath, dotIndexNew);
        if ((fpIndexNew = fopen(IndexNewFile, "w")) == NULL)
        {
            fclose(fpNames);
            return -1;
        }
        sprintf(printstr,
                "\033[0;1;41;33m ---====== ※精华区索引※ [更新时间："
                "%4d年%02d月%02d日%02d:%02d:%02d 星期%2s] ======--- \033[m\n",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
        visited = ndir = nfile = 0;
        prefix[0] = prefixstr[0][0];
        prefix[1] = prefixstr[0][1];
        prefix[2] = prefixstr[1][0];
        prefix[3] = prefixstr[1][1];
        prefix[4] = 0;
        strcpy(save_prefix, prefix);
    }
    else
    {

        if (ptr = strstr(prefix, prefixstr[0]))
        {
            *ptr++ = prefixstr[4][0];
            *ptr = prefixstr[4][1];
        }
        if (ptr = strstr(prefix, prefixstr[2]))
        {
            *ptr++ = ' ';
            *ptr = ' ';
        }
        if (ptr = strstr(prefix, prefixstr[1]))
        {
            *ptr++ = prefixstr[0][0];
            *ptr = prefixstr[0][1];
        }
        strcat(prefix, prefixstr[1]);
        strcpy(save_prefix, prefix);
    }

    isdir = count = 0;
    bool = buf;
    while ((memcmp(buf, "Name=", 5) || HIDE(buf)) && bool)
        bool = fgets(buf, sizeof(buf), fpNames);
    while (bool)
    {
        bufptr = buf + 5;
        strcpy(title, bufptr);
        ptr = strchr(title, '\n');
        *ptr = '\0';
        ptr--;
        while (ptr && *ptr == ' ')
            ptr--;
        *(ptr + 1) = '\0';
        if (strlen(title) > 34)
            title[34] = '\0';
        if (!count)
        {
            ptr = strstr(printstr, prefixstr[1]);
            if (ptr && !dironly)
            {
                *ptr++ = prefixstr[3][0];
                *ptr = prefixstr[3][1];
            }
        }
        fprintf(fpIndexNew, "%s", printstr);
        bool = fgets(buf, sizeof(buf), fpNames);
        while (memcmp(buf, "Path=", 5) && bool)
            bool = fgets(buf, sizeof(buf), fpNames);
        bufptr = bufptr + 2;
        if (ptr = strchr(bufptr, '\n'))
            *ptr = '\0';
        if (strlen(bufptr) == 0)
            continue;
        count++;
        if (Isdir(bufptr))
        {
            ndir++;
            isdir = 1;
        }
        else
        {
            nfile++;
        }
        if (!level && !memcmp(bufptr, dotIndex, 6))
        {
            visited = 1;
        }
        if (isdir)
        {
            visitlog(bufptr);
            sprintf(currPath, "%s/%s", currentpath, bufptr);
            sprintf(currFile, "%s/%s", currPath, dotNames);
        }

        while ((memcmp(buf, "Name=", 5) || HIDE(buf)) && bool)
            bool = fgets(buf, sizeof(buf), fpNames);
        if (!bool)
        {
            ptr = strstr(prefix, prefixstr[0]);
            if (ptr)
            {
                *ptr++ = prefixstr[2][0];
                *ptr = prefixstr[2][1];
            }
        }
        if (isdir)
        {
            sprintf(printstr, "%s\033[1;32m%s %s\033[m\n",
                    prefix, title, visitnumstr);
        }
        else if (!dironly)
        {
            sprintf(printstr, "%s%s \n", prefix, title);
        }
        else
            sprintf(printstr, "");

        if (isdir)
        {
            AnnounceIndex(level + 1);
            sprintf(prefix, "%s", save_prefix);
            strcpy(currPath, currentpath);
            strcpy(currFile, NamesFile);
            isdir = 0;
        }

    }
    if (!level)
    {
        fprintf(fpIndexNew, "%s", printstr);
        fclose(fpIndexNew);
        sprintf(IndexFile, "%s/%s", currPath, dotIndex);
        rename(IndexNewFile, IndexFile);
        if (!visited)
        {
            sprintf(IndexNewFile, "%s/%s.new", currPath, dotNames);
            if (fpIndexNew = fopen(IndexNewFile, "w"))
            {
                fseek(fpNames, (off_t) 0, SEEK_SET);
                while (fgets(buf, sizeof(buf), fpNames))
                {
                    if (buf[0] != '#')
                        break;
                    fputs(buf, fpIndexNew);
                }
                fprintf(fpIndexNew,
                        "Name=【本板精华区索引】\nPath=~/%s\nNumb=0\n#\n",
                        dotIndex);
                fputs(buf, fpIndexNew);
                while (fgets(buf, sizeof(buf), fpNames))
                {
                    fputs(buf, fpIndexNew);
                }
                fclose(fpNames);
                fclose(fpIndexNew);
                rename(IndexNewFile, NamesFile);
                return 0;
            }
        }
    }
    fclose(fpNames);
}

int main(int argc, char *argv[])
{
    DIR *dirp, *bdirp;
    struct dirent *de, *bde;
    int ch;
    char *fname, *bname;

    if (argc < 3)
    {
        printf("  Useage: %s BBSHOME GROUPnum BoardName [-d]\n", argv[0]);
        printf("          %s BBSHOME all [-d]\n", argv[0]);
        printf("\n(1)例: %s /home/bbs 0 sysop\n", argv[0]);
        printf("       即：对位于 GROUP_0 区的 sysop 版进行索引");
        printf("\n(2)例: %s /home/bbs all -d\n", argv[0]);
        printf("       即：对所有版面进行纯目录索引");
        printf("\n(3)参数 [-d] 的含义：仅仅进行目录索引\n");
        printf("       注：-d 参数如果有，则必须在最后！\n\n");
        return 1;
    }
    sprintf(currPath, "%s/0Announce/groups", argv[1]);
    if (chdir(currPath) == -1)
    {
        printf("目录 [%s] 不正确。\n", currPath);
        return -1;
    }
    if (argv[argc - 1][0] == '-' && argv[argc - 1][1] == 'd')
        dironly = 1;
    if (strcasecmp(argv[2], "all"))
    {
        sprintf(currPath, "GROUP_%s/%s", argv[2], argv[3]);
        sprintf(currFile, "%s/%s", currPath, dotNames);
        AnnounceIndex(0);
        return 0;
    }
    strcpy(currPath, ".");
    if (!(dirp = opendir(currPath)))
    {
        printf("Error! Unable to enter [Announce]\n");
        return -1;
    }
    while (de = readdir(dirp))
    {
        strcpy(currPath, ".");
        fname = de->d_name;
        ch = fname[0];
        if (ch != '.')
        {
            if (!Isdir(fname))
                continue;
            if (!(bdirp = opendir(fname)))
            {
                printf("Error! Unable to enter [%s]\n", fname);
                continue;
            }
            while (bde = readdir(bdirp))
            {
                strcpy(currPath, fname);
                bname = bde->d_name;
                ch = bname[0];
                if (ch != '.')
                {
                    if (strstr(bname, "PersonalCorp"))
                        continue;
                    if (Isdir(bname))
                    {
                        sprintf(currPath, "%s/%s", fname, bname);
                        sprintf(currFile, "%s/%s", currPath, dotNames);
                        printf("%s/%-14sd: %d\tf: %d\n", fname, bname,
                               ndir, nfile);
                        AnnounceIndex(0);
                    }
                }
            }
            closedir(bdirp);
        }
    }
    closedir(dirp);
    exit(0);
}
