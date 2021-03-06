#include "www.h"

extern int loadregistdetail();

extern struct registdetail alldetail[MAX_REGIST_NUM];

int isopen = 0, max_allow_num = 0;

static void get_real_name_email(char *buf)
{
    char path[128];
    int fd;
    ssize_t readnum;

    sethomefile(path, currentuser.userid, REAL_NAME_FILE);
    if ((fd = open(path, O_RDONLY)) == -1)
    {
        buf[0] = '\0';
        return;
    }
    readnum = read(fd, buf, REAL_NAME_EMAIL_LEN);
    close(fd);
    buf[readnum] = '\0';
    return;
}

int writedetail(char *userid, int num, int bus, char *email, int gender)
{
    int fd;
    struct registdetail detail;

    strncpy(detail.userid, userid, IDLEN + 1);
    strncpy(detail.email, email, REAL_NAME_EMAIL_LEN);
    detail.bus = bus;
    detail.gender = gender;
    if ((fd = open(REGIST_DATA_FILE, O_WRONLY)) == -1)
        return 0;
    lseek(fd, num * sizeof(struct registdetail), SEEK_SET);
    flock(fd, LOCK_EX);
    write(fd, &detail, sizeof(struct registdetail));
    flock(fd, LOCK_UN);
    close(fd);
    return 1;
}

int get_originalnum(char *userid)
{
    int i;
    for (i = 1; i <= max_allow_num; i++)
        if (strcmp(alldetail[i].userid, userid) == 0)
            return i;
    return -1;
}

int check_legal(char *userid, int num, int bus, char *email, int type, int originalnum)
{
    int i;
    if (bus < 0 || bus > 3)
    {
        printf("选号失败：乘坐校车选项有误");
        return 0;
    }

    if (num <= 0 || num > max_allow_num)
    {
        printf("选号失败：该号码不在允许号码范围内");
        return 0;
    }

    loadregistdetail(&isopen, &max_allow_num);
    for (i = 1; i <= max_allow_num; i++)
    {
        if (originalnum != i && !strcmp(currentuser.userid, alldetail[i].userid))
        {
            printf("选号失败：你已选择过号码\n");
            return 0;
        }
    }

    if (originalnum != num && alldetail[num].userid[0] != '\0')
    {
        printf("选号失败：该号码已被选择");
        return 0;
    }

    if (type == 1 && email[0] != '\0')
    {
        for (i = 1; i <= max_allow_num; i++)
        {
            if (!strcmp(email, alldetail[i].email))
            {                
                printf("选号失败：该id绑定的邮箱已经选择过号码,\n");
                printf("使用该绑定邮箱的id为<a href=\"bbsqry?userid=%s\">%s</a>\n",alldetail[i].userid,alldetail[i].userid);
                return 0;
            }
        }
    }

    return 1;
}

void log_regist(char *userid, int num, int bus, char gender, int type)
{
    FILE *fp;
    time_t now;

    now = time(0);
    getdatestring(now, NA);
    fp = fopen(REGIST_LOG, "at");
    fprintf(fp, "%-3c%-36s%-15s%-5d%-3c%3d\n", (type == 1)? '+' : ((type == 2) ? '@' : '-'),
            datestring, userid, num, gender, bus);
    fclose(fp);
}

void rebuild_juhuireg_dataview()
{
    FILE *fp;
    char filename[STRLEN];
    struct registdetail *detail;
    int i, j;
    int blank, leftblank, k;
    int totalnum, male, female, buscome, busgo;

    loadregistdetail(&isopen, &max_allow_num);
    if (!isopen || max_allow_num <= 0)
        return;

    totalnum = male = female = buscome = busgo = 0;

    sprintf(filename, "%s.%d", REGIST_DATAVIEW_FILE, getuid());
    fp = fopen(filename, "w");

    for (i = 1; i <= max_allow_num; i++)
    {
        if (alldetail[i].userid[0] != '\0')
        {
            totalnum++;
            if (alldetail[i].bus == 1)
                buscome++;
            else if (alldetail[i].bus == 2)
                busgo++;
            else if (alldetail[i].bus == 3)
            {
                buscome++;
                busgo++;
            }
            if (alldetail[i].gender == 'M')
                male++;
            else
                female++;
        }
    }

    fprintf(fp, "                           [1;32m饮水思源BBS站 站庆报名名单[m\n");
    fprintf(fp, "  [1;32m报名人数:%4d人  [36m男性:%4d人  [35m女性:%4d人  [33m乘校车来:%4d人  乘校车回:%4d人\n\n[m",
            totalnum, male, female, buscome, busgo);
    fprintf(fp, "[1;42;37m┌───┬────────┬───┬────────┬───┬────────┐[m\n");
    fprintf(fp, "[1;42;37m│[40;33m 号码 [42;37m│[40m    报名ID      [1;42;37m│[40;33m 号码 [42;37m│[40m    报名ID      [1;42;37m│[40;33m 号码 [42;37m│[40m    报名ID      [1;42;37m│\n");

    fprintf(fp, "[1;42;37m├───┼────────┼───┼────────┼───┼────────┤[m\n");
    j = 0;
    for (i = 1; i<= max_allow_num; i++)
    {
        detail = &alldetail[i];
        if (detail->userid[0] != '\0')
        {
            fprintf(fp, "[1;42;37m│[40;33m %04d [42;37m│[40m", i);
            blank = 16 - strlen(detail->userid);
            leftblank = blank / 2;
            for (k = 0; k < leftblank; k ++)
                fprintf(fp, " ");
            fprintf(fp, "[%dm%s[m", detail->gender == 'F' ? 35 : 36, detail->userid);
            for (k = 0; k < blank - leftblank; k ++)
                fprintf(fp, " ");
            j ++;
            if (j == 3)
            {
                j = 0;
                fprintf(fp, "[1;42;37m│[m\n");
            }
        }
    }
    if (j > 0)
    {
        for (k = 0; k < 3 - j; k ++)
            fprintf(fp, "[1;42;37m│[m      [1;42;37m│[m                ");
        fprintf(fp, "[1;42;37m│[m\n");
    }
    fprintf(fp, "[1;42;37m└───┴────────┴───┴────────┴───┴────────┘[m\n");
    fclose(fp);
    rename(filename, REGIST_DATAVIEW_FILE);
}

int bbsdoregist_main()
{
    int num, bus, originalnum = 0;
    char gender, email[REAL_NAME_EMAIL_LEN];
    int type;
    int success;

    init_all();
    loadregistdetail(&isopen, &max_allow_num);

    printf("<body>\n  ");
    if (!isopen)
        printf("当前报名尚未开启或已经关闭。<br/>");
    else if (!loginok)
        printf("匆匆过客不能报名，请先登录。<br/>");
    else if (!(currentuser.userlevel & PERM_LOGINOK))
        printf("您尚未通过注册，不能报名。<br/>");
    else
    {
        type = atoi(getparm("type"));
        bus = atoi(getparm("bus"));
        num = atoi(getparm("number"));
        success = 0;
        get_real_name_email(email);
        gender = currentuser.gender;

        if (type == 1)
        {
            if (check_legal(currentuser.userid, num, bus, email, type, -1))
            {
                writedetail(currentuser.userid, num, bus, email, gender);
                printf("号码选择成功。<br/>\n");
                printf("您选择的号码是: %d</br>\n", num);
                success = 1;
            }
        }
        else if (type == 2)
        {
            if (strcmp(currentuser.userid, alldetail[num].userid) == 0 && alldetail[num].bus == bus)
            {
                printf("您的选择没有变化。<br/>\n");
            }
            else
            {
                originalnum = get_originalnum(currentuser.userid);
                if (originalnum > 0 && check_legal(currentuser.userid, num, bus, email, type, originalnum))
                {
                    writedetail("", originalnum, 0, "", 0);
                    writedetail(currentuser.userid, num, bus, email, gender);
                    printf("号码修改成功。<br/>\n");
                    success = 1;
                }
            }
        }
        else if (type == 3)
        {
            num = get_originalnum(currentuser.userid);
            if (num > 0)
            {
                writedetail("", num, 0, "", 0);
                printf("删除选号成功。<br/>\n");
                success = 1;
            }
        }
        if (success)
        {
            log_regist(currentuser.userid, num, bus, gender, type);
            rebuild_juhuireg_dataview();
        }
    }
    printf
        ("[<a href=\"bbsregistview\">查看选号情况</a>]");
    printf("</body>");

    return 0;
}

