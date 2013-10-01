
#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"

struct binfo
{
    char boardname[18];
    char expname[28];
    int times;
    int sum;
}
st[MAXBOARD];

int numboards = 0;

#define RECORD_FILE BBSHOME"/reclog/brdstat.rec"

int brd_cmp(struct binfo *b, struct binfo *a)
{
    if (a->times != b->times)
        return (a->times - b->times);
    return a->sum - b->sum;
}

int report()
{
    return 0;
}

char *Ctime(time_t * date)
{
    static char buf[80];

    strcpy(buf, (char *) ctime(date));
    buf[strlen(buf) - 1] = '\0';
    return buf;
}

void record_data(char *board, int sec, int times)
{
    int i;

    for (i = 0; i < numboards; i++)
    {
        if (!strcmp(st[i].boardname, board))
        {
            st[i].times += times;
            st[i].sum += sec;
            return;
        }
    }
}

int fillbcache(struct boardheader *fptr)
{

    if (numboards >= MAXBOARD)
        return 0;
    if (((fptr->level != 0)
            && !(fptr->level & PERM_NOZAP || fptr->level & PERM_POSTMASK))
            || strlen(fptr->filename) == 0)
        return 0;
    strcpy(st[numboards].boardname, fptr->filename);
    strcpy(st[numboards].expname, fptr->title + 8);
    /*    printf("%s %s\n",st[numboards].boardname,st[numboards].expname); */
    st[numboards].times = 0;
    st[numboards].sum = 0;
    numboards++;
    return 0;
}

int fillboard()
{
    apply_record(BBSHOME "/.BOARDS", fillbcache,
                 sizeof(struct boardheader));
    return 0;
}

char *timetostr(int i)
{
    static char str[30];
    int minute, sec, hour;

    minute = (i / 60);
    hour = minute / 60;
    minute = minute % 60;
    sec = i & 60;
    sprintf(str, "%2d:%2d:%2d", hour, minute, sec);
    return str;
}

void load_record()
{
    int fd;
    int rcd;
    char filename[256];
    struct binfo bi;

    snprintf(filename, sizeof(filename), RECORD_FILE ".lock");
    if ((fd = open(filename, O_CREAT | O_RDONLY, 0644)) < 0)
    {
        perror("open");
        exit(-1);
    }
    if (flock(fd, LOCK_EX) < 0)
        exit(-2);
    if ((rcd = open(RECORD_FILE, O_RDONLY, 0644)) < 0)
    {
        perror("open " RECORD_FILE);
        flock(fd, LOCK_UN);
        close(fd);
        exit(-1);
    }
    // ...
    while (read(rcd, &bi, sizeof(bi)) == sizeof(bi))
    {
        record_data(bi.boardname, bi.sum, bi.times);
    }
    close(rcd);
    flock(fd, LOCK_UN);
    close(fd);
}

void save_record()
{
    int i;
    int fd;
    int rcd;
    char filename[256];

    snprintf(filename, sizeof(filename), RECORD_FILE ".lock");
    if ((fd = open(filename, O_CREAT | O_RDONLY, 0644)) < 0)
    {
        perror("open");
        exit(-1);
    }
    if (flock(fd, LOCK_EX) < 0)
        exit(-2);
    snprintf(filename, sizeof(filename), RECORD_FILE ".OLD");
    if (rename(RECORD_FILE, filename) < 0)
    {
        perror("rename");
        flock(fd, LOCK_UN);
        close(fd);
        exit(-1);
    }
    if ((rcd = open(RECORD_FILE, O_CREAT | O_WRONLY, 0644)) < 0)
    {
        perror("open " RECORD_FILE);
        flock(fd, LOCK_UN);
        close(fd);
        exit(-1);
    }
    // ...
    for (i = 0; i < numboards; i++)
    {
        safewrite(rcd, &st[i], sizeof(struct binfo));
    }
    close(rcd);
    flock(fd, LOCK_UN);
    close(fd);
}

int main(int argc, char **argv)
{
    //char *progmode;
    FILE *fp;
    FILE *op;
    char buf[256], *p, bname[20];
    char date[80];
    int mode;
    int c[3];
    int max[3];
    unsigned int ave[3];
    int now, sec;
    int i, j, k;
    char *blk[10] = {
                        "£ß", "£ß", "¨x", "¨y", "¨z",
                        "¨{", "¨|", "¨}", "¨~", "¨€",
                    };

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s mode\n", argv[0]);
        return -1;
    }
    mode = atoi(argv[1]);
    if (mode == 1)
        strcpy(buf, BBSHOME "/0Announce/bbslist/board2");
    else if (mode == 0)
        strcpy(buf, BBSHOME "/0Announce/bbslist/board1");
    else
        strcpy(buf, RECORD_FILE);

    rename(BBSHOME "/use_board", BBSHOME "/use_board.OLD");
    fp = fopen(BBSHOME "/use_board.OLD", "r");
    //if ((fp = fopen(BBSHOME"/use_board.OLD", "r")) == NULL)
    //{
    //  printf("cann't open use_board\n");
    //  return 1;
    //}

    if ((op = fopen(buf, "w")) == NULL)
    {
        printf("Can't Write file\n");
        return 1;
    }

    fillboard();
    // ¶ÁÈëÉÏ´ÎµÄÍ³¼ÆÊý¾Ý
    load_record();
    now = time(0);
    sprintf(date, "%6.6s", Ctime(&now) + 4);
    // ¶ÁÈë±¾´ÎµÄÔ­Ê¼Êý¾Ý£¬²¢Éú³ÉÍ³¼Æ½á¹û
    while (fp != NULL && fgets(buf, 256, fp))
    {
        if (strlen(buf) < 57)
            continue;
        if (!strncmp(buf + 23, "USE", 3))
        {
            p = strstr(buf, "USE");
            p += 4;
            p = strtok(p, " ");
            strcpy(bname, p);
            if (p = (char *) strstr(buf + 48, "Stay: "))
            {
                sec = atoi(p + 6);
            }
            else
                sec = 0;
            record_data(bname, sec, 1);
        }
    }
    fclose(fp);
    // ±£´æ±¾´ÎµÄÍ³¼Æ½á¹û
    save_record();
    if (mode != 1 && mode != 0)
        return 0;
    // Éú³É±¨±í
    qsort(st, numboards, sizeof(st[0]), brd_cmp);
    ave[0] = 0;
    ave[1] = 0;
    ave[2] = 0;
    max[1] = 0;
    max[0] = 0;
    max[2] = 0;
    for (i = 0; i < numboards; i++)
    {
        ave[0] += st[i].times;
        ave[1] += st[i].sum;
        ave[2] += st[i].times == 0 ? 0 : st[i].sum / st[i].times;
        if (max[0] < st[i].times)
        {
            max[0] = st[i].times;
        }
        if (max[1] < st[i].sum)
        {
            max[1] = st[i].sum;
        }
        if (max[2] < (st[i].times == 0 ? 0 : st[i].sum / st[i].times))
        {
            max[2] = (st[i].times == 0 ? 0 : st[i].sum / st[i].times);
        }
    }
    c[0] = max[0] / 30 + 1;
    c[1] = max[1] / 30 + 1;
    c[2] = max[2] / 30 + 1;
    numboards++;
    st[numboards - 1].times = ave[0] / numboards;
    st[numboards - 1].sum = ave[1] / numboards;
    strcpy(st[numboards - 1].boardname, "Average");
    strcpy(st[numboards - 1].expname, "×ÜÆ½¾ù");
    if (mode == 1)
    {
        fprintf(op, "[1;37mÃû´Î %-15.15s%-28.28s %5s %8s %10s[m\n",
                "ÌÖÂÛÇøÃû³Æ", "ÖÐÎÄÐðÊö", "ÈË´Î", "ÀÛ»ýÊ±¼ä", "Æ½¾ùÊ±¼ä");
    }
    else
    {
        fprintf(op,
                "      [1;37m1 [m[34m%2s[1;37m= %d (×ÜÈË´Î) [1;37m1 [m[32m%2s[1;37m= %s (ÀÛ»ý×ÜÊ±Êý) [1;37m1 [m[31m%2s[1;37m= %d Ãë(Æ½¾ùÊ±Êý)\n\n",
                blk[9], c[0], blk[9], timetostr(c[1]), blk[9], c[2]);
    }

    for (i = 0; i < numboards; i++)
    {
        if (mode == 1)
            fprintf(op, "[1m%4d[m %-15.15s%-28.28s %5d %-.8s %10d\n",
                    i + 1, st[i].boardname, st[i].expname, st[i].times,
                    timetostr(st[i].sum),
                    st[i].times == 0 ? 0 : st[i].sum / st[i].times);
        else
        {
            fprintf(op,
                    "      [1;37mµÚ[31m%3d [37mÃû ÌÖÂÛÇøÃû³Æ£º[31m%s [35m%s[m\n",
                    i + 1, st[i].boardname, st[i].expname);
            fprintf(op,
                    "[1;37m    ©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª\n");
            fprintf(op, "[1;37mÈË´Î©¦[m[34m");
            for (j = 0; j < st[i].times / c[0]; j++)
            {
                fprintf(op, "%2s", blk[9]);
            }
            fprintf(op, "%2s [1;37m%d[m\n",
                    blk[(st[i].times % c[0]) * 10 / c[0]], st[i].times);
            fprintf(op, "[1;37mÊ±¼ä©¦[m[32m");
            for (j = 0; j < st[i].sum / c[1]; j++)
            {
                fprintf(op, "%2s", blk[9]);
            }
            fprintf(op, "%2s [1;37m%s[m\n",
                    blk[(st[i].sum % c[1]) * 10 / c[1]],
                    timetostr(st[i].sum));
            j = st[i].times == 0 ? 0 : st[i].sum / st[i].times;
            fprintf(op, "[1;37mÆ½¾ù©¦[m[31m");
            for (k = 0; k < j / c[2]; k++)
            {
                fprintf(op, "%2s", blk[9]);
            }
            fprintf(op, "%2s [1;37m%s[m\n", blk[(j % c[2]) * 10 / c[2]],
                    timetostr(j));
            fprintf(op,
                    "[1;37m    ©¸¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª[m\n\n");
        }
    }
    fclose(op);
    return 0;
}
