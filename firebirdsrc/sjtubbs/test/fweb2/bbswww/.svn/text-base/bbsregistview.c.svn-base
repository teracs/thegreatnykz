#include "www.h"

struct registdetail alldetail[MAX_REGIST_NUM];

int cmpruid(userid, rd)
char *userid;
struct registdetail *rd;
{
    return !strncmp(userid, rd->userid, IDLEN);
}

int loadregistdetail(int *isopen, int *max_allow_num)
{
    int fd;
    int i, bufread;
    struct registdetail detail;
    FILE *fp;

    if ((fp = fopen(REGIST_OPENCHECK_FILE, "r")) != NULL)
    {
        fscanf(FCGI_ToFILE(fp), "%d", isopen);
        fclose(fp);
    }
    if (fp == NULL || *isopen == 0)
        return -1;

    if ((fp = fopen(REGIST_MAXALLOWNUM_FILE, "r")) != NULL)
    {
        fscanf(FCGI_ToFILE(fp), "%d", max_allow_num);
        fclose(fp);
    }
    if (fp == NULL || *max_allow_num == 0)
        return -1;

    if ((fd = open(REGIST_DATA_FILE, O_RDONLY, 0)) == -1)
        return -1;
    for (i = 0; i <= *max_allow_num; i++)
    {
        bufread = read(fd, &detail, sizeof(struct registdetail));
        if (bufread != sizeof(struct registdetail))
            break;
        alldetail[i].bus = detail.bus;
        alldetail[i].gender = detail.gender;
        strcpy(alldetail[i].email, detail.email);
        strcpy(alldetail[i].userid, detail.userid);
    }
    close(fd);
    return 0;
}

int bbsregistview_main()
{
    struct registdetail myregist;
    int i;
    FILE *fp;
    char buf[256];
    int isopen = 0, max_allow_num = 0;
    int mynum = -1;

    myregist.userid[0] = '\0';
    init_all();
    loadregistdetail(&isopen, &max_allow_num);
    for (i = 1; i <= max_allow_num; i++)
    {
        if (!strcmp(alldetail[i].userid, currentuser.userid))
        {
            myregist.bus = alldetail[i].bus;
            strcpy(myregist.email, alldetail[i].email);
            myregist.gender = alldetail[i].gender;
            strcpy(myregist.userid, alldetail[i].userid);
            mynum = i;
        }
    }
    printf("<body>\n");
    printf
        ("<table align=center><tr><td><font class=title><b>站庆报名</b></font></td></tr></table><hr>\n");

    printf("<table align=center class=show_border width=610 border=1>\n");
    printf("<tr><td><pre>\n");
    fp = fopen(REGIST_NOTE, "r");
    if (fp != 0)
    {
        while (1)
        {
            if (fgets(buf, 256, fp) == 0)
                break;
            hhprintf("%s", void1(buf));
        }
        fclose(fp);
    }
    printf("</pre></td></tr>\n");
    printf("</table>\n");

    printf("<center>\n");
    printf("<br/>[<a href=bbsregist?type=%d>%s</a>]\n",
           mynum == -1 ? 1 : 2,
           mynum == -1 ? "我要报名" : "修改报名");
    if (mynum != -1)
    {
        printf
            ("<script language='javascript'>					    \n");
        printf
            ("<!--													\n");
        printf
            ("	function cancelreg(){								\n");
        printf
            ("		warning = confirm('确定取消报名吗？')			\n");
        printf
            ("		if (warning != '0'){							\n");
        printf
            ("			window.open('bbsdoregist?type=3', '_self')	\n");
        printf
            ("		}												\n");
        printf
            ("	}													\n");
        printf
            ("-->													\n");
        printf
            ("</script>												\n");
        printf
            ("[<a href='javascript:cancelreg()'>取消报名</a>]		\n");
    }
    printf("[<a href=\"/bbsdoc?board=juhui\">去Juhui板</a>]\n");
    printf("</center><br><br>\n\n");

    printf("<table align=\"center\" class=\"show_border\" width=610>");
    printf("<tr><td align=\"center\"><pre>");
    fp = fopen(REGIST_DATAVIEW_FILE, "r");
    if (fp == 0)
        http_quit_print("错误的文件名");
    while (1)
    {
        if (fgets(buf, 256, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre></td></tr></table><hr>\n");

    printf("</body>\n");
    return 0;
}

