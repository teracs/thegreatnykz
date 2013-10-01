#include <stdlib.h>
#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"

static char seccode[SectNum][5] = {
    "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw", "x"
};

static char secname[SectNum][2][20] = {
    {"BBS œµÕ≥", "[±æ’æ][’æƒ⁄][BBS]"},
    {"…œ∫£Ωª¥Û", "[±æ–£]"},
    {"—ß◊”‘∫–£", "[∏ﬂ–£][µÿ«¯]"},
    {"µÁƒ‘ºº ı", "[µÁƒ‘]"},
    {"—ß ıø∆—ß", "[ø∆—ß]"},
    {"“’ ıŒƒªØ", "[Œƒ“’][”Ô—‘]"},
    {"ÃÂ”˝‘À∂Ø", "[ÃÂ”˝][‘À∂Ø]"},
    {"–›œ–”È¿÷", "[–›œ–][“Ù¿÷][”È¿÷]"},
    {"÷™–‘∏––‘", "[÷™–‘] [∏––‘]"},
    {"…Áª·–≈œ¢", "[…Áª·][–≈œ¢][øº ‘]"},
    {"…ÁÕ≈»∫ÃÂ", "[…ÁÕ≈][»∫ÃÂ]"},
    {"”Œœ∑◊®«¯", "[”Œœ∑]"}
};

#define _INFO_BRD_NUM (256)
#define _INFO_BRD_FILE "etc/infobrds"
static struct
{
    char district[_INFO_BRD_NUM];
    char board[_INFO_BRD_NUM][32];
    time_t mtime;
    unsigned int num;
} info_board;

static gboolean board_check_info_brd_file(void)
{
    time_t mtime;
    unsigned int i;
    FILE *fp;

    mtime = nt_stat_mtime(_INFO_BRD_FILE);
    if (mtime == 0)
        return FALSE;
    if (mtime == info_board.mtime)
        return TRUE;
    fp = fopen(_INFO_BRD_FILE, "r");
    if (fp == NULL)
        return FALSE;
    for (i = 0; i < _INFO_BRD_NUM; i++)
    {
        if (2 !=
            fscanf(fp, "%c %31s\n", &(info_board.district[i]),
                   info_board.board[i]))
            break;
    }
    fclose(fp);
    info_board.mtime = mtime;
    info_board.num = i;
    return TRUE;
}

gboolean nt_board_get_tip(char *board, unsigned int boardlen, char *tip,
                          unsigned int tiplen)
{
    char tips[3][512], path[1024];
    int rbrd, rtip;
    unsigned int i;
    FILE *fp;

    if (!board_check_info_brd_file())
        return FALSE;
    if (info_board.num == 0)
        return FALSE;
    rbrd = rand() % info_board.num;
    snprintf(path, sizeof(path), "0Announce/groups/GROUP_%c/%s/info",
             info_board.district[rbrd], info_board.board[rbrd]);
    fp = fopen(path, "r");
    if (fp == NULL)
        return FALSE;
    for (i = 0; i < 3; i++)
    {
        if (NULL == fgets(tips[i], 512, fp))
            break;
    }
    fclose(fp);
    if (i == 0)
        return FALSE;
    rtip = rand() % i;
    g_strlcpy(board, info_board.board[rbrd], boardlen);
    g_strlcpy(tip, tips[rtip], tiplen);
    return TRUE;
}

gboolean ntcboard_init(void)
{
    memset(&info_board, 0, sizeof(info_board));
    return TRUE;
}

void ntcboard_final(void)
{
    return;
}

void ntcboard_reset(void)
{
    return;
}

struct shortfile *nt_getbcache(char *board)
{
    int i;

    if (board[0] == 0)
        return 0;
    for (i = 0; i < MAXBOARD; i++)
        if (!strcasecmp(board, shm_bcache->bcache[i].filename))
            return &shm_bcache->bcache[i];
    return 0;
}

int
nt_post_article(char *board, char *title, char *file, char *id,
             char *nickname, char *ip, int sig, int autocr,
             unsigned int currid, unsigned int reid)
{
    FILE *fp, *fp2;
    char buf3[1024];
    struct shortfile *brd;
    struct fileheader header;
    int t = 0, count = 0, filesd;
	time_t now;
	char currtime[30];

    brd = nt_getbcache(board);
    if (brd == 0)
        return -1;
    board = brd->filename;
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);

    t = time(0);
    char fulltitle[20];

    sprintf(fulltitle, "M.%d.A", t);
    sprintf(buf3, "boards/%s/%s/%s", board, nt_getfolderstr(fulltitle),
            fulltitle);

    while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        t++;
        sprintf(fulltitle, "M.%d.A", t);
        sprintf(buf3, "boards/%s/%s/%s", board, nt_getfolderstr(fulltitle),
                fulltitle);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    fchmod(filesd, 0644);
    close(filesd);

    if (0 == currid)
    {
        header.id = t;
        header.reid = reid;
    }
    else
    {
        header.id = t;
        header.reid = t;
    }

    sprintf(header.filename, "M.%d.A", t);
    nt_strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    fp2 = fopen(file, "r");
	now = time(0);
	sprintf(currtime, "%s", nt_ctime_cn(now));
    fprintf(fp,
            "∑¢–≈»À: %s (%s), –≈«¯: %s\n±Í  Ã‚: %s\n∑¢–≈’æ: %s (%s)\n\n",
            id, nickname, board, title, BBSNAME, currtime);
    if (fp2 != 0)
    {
        while (1)
        {
            if (fgets(buf3, 1000, fp2) == NULL)
                break;
            if (autocr)
            {
                nt_fprintf2(fp, buf3);
            }
            else
            {
                fprintf(fp, "%s", buf3);
            }
        }
        fclose(fp2);
    }
    fprintf(fp, "\n--\n");
    nt_sig_append(fp, id, sig);
    fprintf(fp, "\n[1;%dm°˘ ¿¥‘¥:°§%s %s°§[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(buf3, "boards/%s/.DIR", board);
    fp = fopen(buf3, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return t;
}

