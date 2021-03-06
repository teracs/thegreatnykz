#include "bbs.h"

struct registdetail alldetail[MAX_REGIST_NUM];
int isopen = 0, max_allow_num = 0;

void open_juhuireg(void)
{
    int fd, i;
    struct registdetail detail;
    FILE *fp;

    clear();
    if (askyn("开启聚会报名?", YEA, YEA) == YEA)
    {
        if (!dashd(REGIST_DIR))
            mkdir(REGIST_DIR, 0755);

        fp = fopen(REGIST_OPENCHECK_FILE, "w+");
        fprintf(fp, "%d", 1);
        fclose(fp);

        if (!dashf(REGIST_DATA_FILE))
        {
            fd = open(REGIST_DATA_FILE, O_WRONLY | O_CREAT, S_IRWXU);
            detail.bus = 0;
            bzero(detail.email, sizeof(detail.email));
            detail.gender = 0;
            bzero(detail.userid, sizeof(detail.userid));
            for (i = 0; i < MAX_REGIST_NUM; i++)
            {
                write(fd, &detail, sizeof(struct registdetail));
            }
            close(fd);
        }

        prints("聚会报名开启了!");
        pressanykey();
    }
}

void close_juhuireg(void)
{
    FILE *fp;

    clear();
    if (askyn("关闭聚会报名?", YEA, YEA) == YEA)
    {
        fp = fopen(REGIST_OPENCHECK_FILE, "w+");
        fprintf(fp, "%d", 0);
        fclose(fp);
        prints("聚会报名结束了!");
        pressanykey();
    }
}

void clear_juhuireg_data(void)
{
    clear();
    if (askyn("清空当前聚会报名名单?", YEA, YEA) == YEA)
    {
        remove(REGIST_DATA_FILE);
        remove(REGIST_DATAVIEW_FILE);
        remove(REGIST_LOG);
        prints("聚会报名名单已清空...");
        pressanykey();
    }
}

void view_juhuireg_log(void)
{
    ansimore_withmode(REGIST_LOG, 0, MORE_MODE_READING);
}

int loadregistdetail(int *isopen, int *max_allow_num)
{
    int fd;
    int i, bufread;
    struct registdetail detail;
    FILE *fp;

    if ((fp = fopen(REGIST_OPENCHECK_FILE, "r")) != NULL)
    {
        fscanf(fp, "%d", isopen);
        fclose(fp);
    }
    if (fp == NULL)
    {
        return -1;
    }

    if ((fp = fopen(REGIST_MAXALLOWNUM_FILE, "r")) != NULL)
    {
        fscanf(fp, "%d", max_allow_num);
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
    if (*isopen == 0)
    {
        return -1;
    }    
    close(fd);
    return 0;
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

void show_juhuireg_note(int num)
{
    clear();
    ansimore_partial(REGIST_NOTE, NA, 0, 16);

    if (HAS_PERM(PERM_SYSOP))
    {
        move(t_lines - 2, 0);
        prints("[1;44;31m[站庆管理] [33m 开启报名 + | 关闭报名 - | 清空报名 d | 查看日志 l [m");
    }

    move(t_lines - 1, 0);
    prints("[1;44;31m[站庆报名] [33m查看报名情况 v ");
    if (num == -1)
        prints("| 我要报名 Enter                       ");
    else
        prints("| 已选号码: %-4d 修改 Enter 取消报名 c ", num);
    prints("│结束 q,←  [m");
}

void view_juhuireg_data()
{
    loadregistdetail(&isopen, &max_allow_num);
    if (!isopen || max_allow_num <= 0)
        return;

    ansimore_withmode(REGIST_DATAVIEW_FILE, 0, MORE_MODE_READING);
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

int doapply(int originalnum, int num, int bus)
{
    int i;
    char email[REAL_NAME_EMAIL_LEN];

    if (bus < 0 || bus > 3)
    {
        prints("选号失败：乘坐校车选项有误\n");
        return 0;
    }

    if (num < 1 || num > max_allow_num)
    {
        prints("选号失败：该号码不在允许号码范围内\n");
        return 0;
    }

    if (originalnum == num && bus == alldetail[originalnum].bus)
    {
        prints("您的选择没有变化。\n");
        return 0;
    }

    loadregistdetail(&isopen, &max_allow_num);
    for (i = 1; i <= max_allow_num; i++)
    {
        if (originalnum != i && !strcmp(currentuser.userid, alldetail[i].userid))
        {
            prints("选号失败：你已选择过号码\n");
            return 0;
        }
    }

    if (originalnum != num && alldetail[num].userid[0] != '\0')
    {
        prints("选号失败：该号码已被选择\n");
        return 0;
    }
    
    get_real_name_email(email);
    if (originalnum == -1 && email[0] != '\0')
    {
        for (i = 1; i <= max_allow_num; i++)
        {
            if (!strcmp(email, alldetail[i].email))
            {                
                prints("选号失败：该id绑定的邮箱已经选择过号码\n");
                prints("使用该绑定邮箱的id为 %s\n",alldetail[i].userid);
                return 0;
            }
        }
    }

    if (originalnum != -1)
        writedetail("", originalnum, 0, "", 0);
    writedetail(currentuser.userid, num, bus, email, currentuser.gender);
    log_regist(currentuser.userid, num, bus, currentuser.gender, originalnum == -1 ? 1 : 2);
    rebuild_juhuireg_dataview();
    if (originalnum == -1)
        prints("号码选择成功。\n");
    else
        prints("号码修改成功。\n");
    prints("您选择的号码是: %d\n", num);
    return 1;
}

int juhuireg_apply(int originalnum)
{
    int i, bus, preset_bus, num, preset_num;
    char askbuf[STRLEN], buf[10];

    clear();

    if (originalnum == -1)
    {
        for (i = 1; i <= max_allow_num; i++)
            if (alldetail[i].userid[0] == '\0')
                break;

        if (i == max_allow_num + 1)
        {
            move(0, 0);
            prints("很抱歉，目前号码已全部用完。");
            pressanykey();
            return -1;
        }
        preset_num = i;
        preset_bus = 0;
    }
    else
    {
        preset_num = originalnum;
        preset_bus = alldetail[originalnum].bus;
    }

    while (1)
    {
        clear();
        prints("%s", originalnum == -1 ? "我要报名" : "修改报名");
        buf[0] = '\0';
        sprintf(askbuf, "是否乘坐校车(0:否;1:乘校车来;2:乘校车回;3:均乘坐校车，已选[1;31m%d[m): ", preset_bus);
        getdata(1, 0, askbuf, buf, 5, DOECHO, NA);
        if (!isdigit(buf[0]))
            bus = preset_bus;
        else
            bus = atoi(buf);

        buf[0] = '\0';
        sprintf(askbuf, "输入您想选择的号码(直接回车选[1;31m%d[m, 输入0退出): ", preset_num);
        getdata(2, 0, askbuf, buf, 5, DOECHO, NA);
        if (!isdigit(buf[0]))
        {
            num = preset_num;
            if (doapply(originalnum, num, bus))
            {
                pressanykey();
                return num;
            }
            else
            {
                pressanykey();
                return originalnum;
            }
        }
        else
        {
            num = atoi(buf);
            if (num == 0)
                return originalnum;

            if (doapply(originalnum, num, bus))
            {
                pressanykey();
                return num;
            }
            else
            {
                pressanykey();
                return originalnum;
            }
        }
        pressanykey();
    }

    return originalnum;
}

int juhuireg_cancel(int num, int bus)
{
    clear();
    move(0, 0);
    if (askyn("确定要取消报名吗?", NA, NA) == YEA)
    {
        writedetail("", num, 0, "", 0);
        log_regist(currentuser.userid, num, bus, currentuser.gender, 3);
        rebuild_juhuireg_dataview();
        prints("\n已取消报名...");
        pressanykey();
        return -1;
    }
    return num;
}

void m_RegJuhui()
{
    int num;
    int ch;

    clear();
    move(0, 0);
    loadregistdetail(&isopen, &max_allow_num);

    modify_user_mode(READNEW);

    num = get_originalnum(currentuser.userid);
    if ((!isopen || max_allow_num <= 0) && !HAS_PERM(PERM_SYSOP))
    {
        prints("站庆报名尚未开启或已经关闭\n");
        if (num == -1)
            prints("您未参与已结束的站庆报名");
        else
            prints("您的已选号码: %-4d", num);
        pressanykey();
        return;
    }

    show_juhuireg_note(num);
    ch = igetkey();
    while (1)
    {
        switch (ch)
        {
            case 'q':
            case KEY_LEFT:
                return;
            case 'v':
                view_juhuireg_data();               
                break;
            case '\r':
            case '\n':
                juhuireg_apply(num);
                num = get_originalnum(currentuser.userid);
                break;
            case 'c':
                if (num != -1)
                {
                    juhuireg_cancel(num, alldetail[num].bus);
                    num = get_originalnum(currentuser.userid);
                }
                break;
            case 'l':
                if (HAS_PERM(PERM_SYSOP))
                    view_juhuireg_log();
                break;
            case '+':
                if (HAS_PERM(PERM_SYSOP))
                    open_juhuireg();
                break;
            case '-':
                if (HAS_PERM(PERM_SYSOP))
                    close_juhuireg();
                break;
            case 'd':
                if (HAS_PERM(PERM_SYSOP))
                    clear_juhuireg_data();
                break;
        }
        show_juhuireg_note(num);
        ch = igetkey();
    }
}

