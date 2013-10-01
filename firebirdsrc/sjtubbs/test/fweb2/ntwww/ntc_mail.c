#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"

unsigned int nt_mail_get_num(const char *userid)
{
    char path[1024];

    nt_setmailfile(path, sizeof(path), userid, ".DIR");
    return nt_stat_size(path) / sizeof(struct fileheader);
}

unsigned int nt_mail_get_unread_num(const char *userid)
{
    char path[1024];
    unsigned int num;
    GMappedFile *map_dir;
    struct fileheader *fheader, *fheader_end;

    nt_setmailfile(path, sizeof(path), userid, ".DIR");
    map_dir = g_mapped_file_new(path, FALSE, NULL);

    if (map_dir == NULL)
        return 0;

    fheader = (struct fileheader *) g_mapped_file_get_contents(map_dir);
    fheader_end =
        fheader +
        g_mapped_file_get_length(map_dir) / sizeof(struct fileheader);
    for (num = 0; fheader != fheader_end; fheader++)
    {
        if (!(fheader->accessed[0] & FILE_READ))
            num++;
    }
    g_mapped_file_free(map_dir);
    return num;
}

int
nt_post_imail(char *userid, char *title, char *file, char *id,
           char *nickname, int autocr, char *ip, int sig)
{

    return 0;
}

int
nt_post_mail(char *userid, char *title, char *file, char *id,
          char *nickname, int autocr, char *ip, int sig)
{
    FILE *fp, *fp2;
    char buf3[256], dir[256];
    struct fileheader header;
    int t, i;
	time_t now;
	char currtime[30];

    if (strstr(userid, "@"))
        return nt_post_imail(userid, title, file, id, nickname, autocr, ip, sig);
    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    for (i = 0; i < 100; i++)
    {
        t = time(0) + i;
        sprintf(buf3, "mail/%c/%c/%s/M.%d.A", toupper(userid[0]),
                toupper(userid[1]), userid, (int) (i + time(0)));
        if (!nt_file_exist(buf3))
            break;
    }
    if (i >= 99)
        return -1;
    sprintf(header.filename, "M.%d.A", t);
    nt_strsncpy(header.title, title, 60);
    fp = fopen(buf3, "w");
    if (fp == 0)
        return -2;
    fp2 = fopen(file, "r");
	now = time(0);
	sprintf(currtime, "%s", nt_ctime_cn(now));
    fprintf(fp, "¼ÄÐÅÈË: %s (%s)\n", id, nickname);
    fprintf(fp, "±ê  Ìâ: %s\n", title);
    fprintf(fp, "·¢ÐÅÕ¾: %s (%s)\n", BBSNAME, currtime);
    fprintf(fp, "À´  Ô´: %s\n\n", ip);
    if (fp2)
    {
        while (1)
        {
            if (fgets(buf3, 256, fp2) == NULL)
                break;
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
    fprintf(fp, "\n\n[1;%dm¡ù À´Ô´:¡¤%s http://%s¡¤[FROM: %.46s][m\n",
            31 + rand() % 7, BBSNAME, BBSHOST, ip);
    fclose(fp);
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(userid[0]),
            toupper(userid[1]), userid);
    fp = fopen(dir, "a");
    if (fp == 0)
        return -1;
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return 0;
}

