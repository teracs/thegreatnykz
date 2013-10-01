#include "www.h"

static int is_voting(char *board)
{
    int num_of_vote = 0;
    char controlfile[STRLEN];

    sprintf(controlfile, "vote/%s/control", board);
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    return num_of_vote;
}

static int cmpboard(b1, b2)
struct boardheader *b1, *b2;
{
    if ((b1->flag2[0] & BOARD_DIR_FLAG)
        && (!(b2->flag2[0] & BOARD_DIR_FLAG)))
        return -1;
    if (!(b1->flag2[0] & BOARD_DIR_FLAG)
        && ((b2->flag2[0] & BOARD_DIR_FLAG)))
        return 1;
    return strcasecmp(b1->filename, b2->filename);
}
static int filenum(char *board)
{
    char file[256];

    sprintf(file, "boards/%s/.DIR", board);
    return file_size(file) / sizeof(struct fileheader);
}

static int board_read(char *board)
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

int bbssubboard_main()
{
    struct shortfile data[MAXBOARD], *x;
    int i, total = 0;
    int boardnum = 0;
    char *cgi = "bbsdoc", *ptr;
    char boardname[20] = { 0 };

    cache_age = 60 * 60;
    init_all();
    printf("<body topmargin=0 leftmargin=0>");
    strsncpy(boardname, getparm("name"), 16);
    if (strlen(boardname) == 0)
        http_quit_print("错误的参数");

    boardnum = getbnum(boardname);
    if (boardnum <= 0)
        http_quit_print("错误的目录");

    if (my_def_mode != 0)
        cgi = "bbstdoc";

    for (i = 0; i < MAXBOARD; i++)
    {
        x = &(shm_bcache->bcache[i]);
        if (x->filename[0] <= 32 || x->filename[0] > 'z')
            continue;
        if (x->group != boardnum)
            continue;
        if (!has_read_perm(&currentuser, x->filename))
            continue;
        memcpy(&data[total], x, sizeof(struct shortfile));
        total++;
    }

    qsort(data, total, sizeof(struct shortfile), cmpboard);
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - 子板面列表 [%s]</font>\n\n	   <hr>",
         BBSNAME, boardname);
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
#ifdef STATIC_URL
        printf("<td><a href=%s,board,%s.html>%s</a>", cgi,
               data[i].filename, data[i].filename);
        printf("<td>%12.12s", 4 + Ctime(file_time(buf2)));
        printf("<td>%6.6s", data[i].title + 1);
        printf("<td><a href=%s,board,%s.html>%s%s</a>", cgi,
               data[i].filename, data[i].title + 7,
               is_voting(data[i].
                         filename) ?
               "<font style='font-size:12px; color:#f00000'> V</font>" :
               "");
#else
        printf("<td><a href=%s?board=%s>%s</a>", cgi,
               data[i].filename, data[i].filename);
        printf("<td>%12.12s", 4 + Ctime(file_time(buf2)));
        printf("<td>%6.6s", data[i].title + 1);
        printf("<td><a href=%s?board=%s>%s%s</a>", cgi,
               data[i].filename, data[i].title + 7,
               is_voting(data[i].
                         filename) ?
               "<font style='font-size:12px; color:#f00000'> V</font>" :
               "");
#endif
        ptr = strtok(data[i].BM, " ,;");
        if (ptr == 0)
            ptr = "诚征板主中";
        printf("<td><a href=bbsqry?userid=%s>%s</a>", ptr, ptr);
        printf("<td>%d\n", filenum(data[i].filename));
    }
    printf("</table></center><hr><br><br>\n");
    printf("</html>");
    return 0;
}
