#include "www.h"

static int cmpboard(b1, b2)
struct boardheader *b1, *b2;
{
    return strcasecmp(b1->filename, b2->filename);
}

int bbsboa_main()
{
    struct shortfile data[MAXBOARD], *x;
    int i, total = 0, sec1;
    char *cgi = "bbsdoc", *ptr;

    init_all();
    printf("<link rel=stylesheet type=text/css href=http://bbs.sjtu.edu.cn:8000/TechGroup/1100423301201530.css>\n");
    printf("<body topmargin=0 leftmargin=0>");
    //printf("<style type=text/css>A {color: #0000f0}</style>");
    sec1 = atoi(getparm("sec"));
    if (sec1 < 0 || sec1 >= SECNUM)
        http_fatal("错误的参数");
    if (my_def_mode != 0)
        cgi = "bbstdoc";
    for (i = 0; i < MAXBOARD; i++)
    {
        x = &(shm_bcache->bcache[i]);
        if (x->filename[0] <= 32 || x->filename[0] > 'z')
            continue;
        if (!has_read_perm(&currentuser, x->filename))
            continue;
        if (!strchr(seccode[sec1], x->title[0]))
            continue;
        memcpy(&data[total], x, sizeof(struct shortfile));
        total++;
    }
    qsort(data, total, sizeof(struct shortfile), cmpboard);
    printf("<center>\n");
    printf("<font class=\"title\">%s - 分类讨论区 [%s区]</font>\n
		    <hr>", BBSNAME, secname[sec1]);
    printf("<table width=700>\n");
    printf
    ("<tr bgcolor=e8e8e8><td>序号<td>未<td>讨论区名称<td>更新时间<td>类别<td>中文描述<td>板主<td>文章数\n");
    for (i = 0; i < total; i++)
    {
        char buf[100], buf2[80];

        sprintf(buf2, "boards/%s", data[i].filename);
        sprintf(buf, "boards/%s/.DIR", data[i].filename);
        printf("<tr><td>%d<td>%s",
               i + 1, board_read(data[i].filename) ? "◇" : "◆");
        printf("<td><a href=%s?board=%s>%s</a>", cgi, data[i].filename,
               data[i].filename);
        printf("<td>%12.12s", 4 + Ctime(file_time(buf2)));
        printf("<td>%6.6s", data[i].title + 1);
        printf("<td><a href=%s?board=%s>%s</a>", cgi, data[i].filename,
               data[i].title + 7);
        ptr = strtok(data[i].BM, " ,;");
        if (ptr == 0)
            ptr = "诚征板主中";
        printf("<td><a href=bbsqry?userid=%s>%s</a>", ptr, ptr);
        printf("<td>%d\n", filenum(data[i].filename));
    }
    printf("</table></center><hr><br><br>\n");
    http_quit();
}

int filenum(char *board)
{
    char file[256];

    sprintf(file, "boards/%s/.DIR", board);
    return file_size(file) / sizeof(struct fileheader);
}

int board_read(char *board)
{
    char buf[256];
    FILE *fp;
    struct fileheader x;
    int total;

    if (!loginok)
        return 1;
    bzero(&x, sizeof(x));
    sprintf(buf, "boards/%s/.DIR", board);
    total = file_size(buf) / sizeof(struct fileheader);
    if (total <= 0)
        return 1;
    fp = fopen(buf, "r+");
    fseek(fp, (total - 1) * sizeof(struct fileheader), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    fclose(fp);
    brc_init(currentuser.userid, board);
    return brc_has_read(x.filename);
}
