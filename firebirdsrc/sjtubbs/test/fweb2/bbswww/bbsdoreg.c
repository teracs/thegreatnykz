#include "www.h"

static int badymd(int y, int m, int d)
{
    int max[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
        max[2] = 29;
    if (y < 10 || y > 100 || m < 1 || m > 12)
        return 1;
    if (d < 0 || d > max[m])
        return 1;
    return 0;
}

static int badstr(char *s);
static void newcomer(struct userec *x, char *words);
static void adduser(struct userec *x);

int bbsdoreg_main()
{
    FILE *fp;
    struct userec x;
    int i, xz, vfd;
    unsigned int vcode = 0, real_vcode;
    char buf[80], filename[80], pass1[80], pass2[80], dept[80], phone[80],
        words[1024], vcodefile[32], vcodestr[8], vcodebuf[1024];
    struct regform record;

    memset(&record, 0, sizeof(record));
    init_all();
    if (noregist())
        http_quit_print("对不起，本站暂停注册新用户");
    bzero(&x, sizeof(x));
    xz = atoi(getparm("xz"));
    strsncpy(x.userid, getparm("userid"), 13);
    strsncpy(pass1, getparm("pass1"), 13);
    strsncpy(pass2, getparm("pass2"), 13);
    strsncpy(x.username, getparm("username"), 32);
    strsncpy(x.realname, getparm("realname"), 32);
    strsncpy(dept, getparm("dept"), 64);
    strsncpy(x.address, getparm("address"), 64);
    strsncpy(x.email, getparm("email"), 64);
    strsncpy(phone, getparm("phone"), 64);
    strsncpy(words, getparm("words"), 1000);
    strsncpy(vcodefile, getparm("vcodefile"), 30);
    strsncpy(vcodestr, getparm("vcode"), 7);
    vcode = atoi(vcodestr);
    if (0 == vcode || 0 == strlen(vcodefile))
        http_quit_print("验证码不正确");
    snprintf(vcodebuf, 1000, "httpd/vcode-tmp/%s", vcodefile);
    vfd = open(vcodebuf, O_RDONLY);
    if (-1 == vfd)
        http_quit_print("打开文件错误");
    read(vfd, &real_vcode, sizeof(real_vcode));
    close(vfd);
    if (vcode != real_vcode)
        http_quit_print("验证码不正确2");
    unlink(vcodebuf);
    x.gender = 'M';
    if (atoi(getparm("gender")))
        x.gender = 'F';
    x.birthyear = atoi(getparm("year")) - 1900;
    x.birthmonth = atoi(getparm("month"));
    x.birthday = atoi(getparm("day"));
    for (i = 0; x.userid[i]; i++)
        if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", toupper(x.userid[i])))
            http_quit_print("帐号只能由英文字母组成");
    if (strlen(x.userid) < 2)
        http_quit_print("帐号长度太短(2-12字符)");
    if (strlen(pass1) < 4)
        http_quit_print("密码太短(至少4字符)");
    if (strcmp(pass1, pass2))
        http_quit_print("两次输入的密码不一致, 请确认密码");
    if (strlen(x.username) < 2)
        http_quit_print("请输入昵称(昵称长度至少2个字符)");
    if (strlen(x.realname) < 4)
        http_quit_print("请输入真实姓名(请用中文, 至少2个字)");
    if (strlen(dept) < 6)
        http_quit_print("工作单位的名称长度至少要6个字符(或3个汉字)");
    if (strlen(x.address) < 10)
        http_quit_print("通讯地址长度至少要10个字符(或5个汉字)");
    if (badstr(x.passwd) || badstr(x.username) || badstr(x.realname))
        http_quit_print("您的注册单中含有非法字符");
    if (badstr(dept) || badstr(x.address) || badstr(x.email)
        || badstr(phone))
        http_quit_print("您的注册单中含有非法字符");
    if (badymd(x.birthyear, x.birthmonth, x.birthday))
        http_quit_print("请输入您的出生年月");
    if (is_bad_id(x.userid))
        http_quit_print("不雅帐号或禁止注册的id, 请重新选择");
    if (getuser(x.userid))
        http_quit_print("此帐号已经有人使用,请重新选择。");
    if (getuser_like(x.userid))
        http_quit_print("此帐号与已有帐号过于相似，请重新选择。");
    strsncpy(x.passwd, crypt_passwd(pass1), 14);
    strcpy(x.termtype, "vt100");
    strcpy(x.lasthost, fromhost);
    x.userlevel = PERM_BASIC;
    x.firstlogin = time(0);
    x.lastlogin = time(0);
    x.lastlogout = time(0);
    x.numlogins = 1;
    x.userdefine = -1;
    x.flags[0] = CURSOR_FLAG | PAGER_FLAG;
    if (xz == 1)
        x.userdefine ^= DEF_COLOREDSEX;
    if (xz == 2)
        x.userdefine ^= DEF_S_HOROSCOPE;
    adduser(&x);

    record.usernum = getusernum(x.userid) + 1;
    strncpy(record.date, Ctime(time(0)), 30);
    strncpy(record.userid, x.userid, IDLEN + 2);
    strncpy(record.realname, x.realname, NAMELEN);
    strncpy(record.dept, dept, STRLEN);
    strncpy(record.addr, x.address, STRLEN);
    strncpy(record.phone, phone, STRLEN);
    record.status = 0;
    fp = fopen("register_sjtubbs", "a");
    if (fp)
    {
        fwrite(&record, sizeof(struct regform), 1, fp);
        fclose(fp);
    }

    f_append("trace.post", "G");
    sprintf(filename, "home/%c/%c/%s", toupper(x.userid[0]),
            toupper(x.userid[1]), x.userid);
    mkdir(filename, 0755);
    printf("<center><table><td><td><pre>\n");
    printf("亲爱的新使用者，您好！\n\n");
    printf("欢迎光临 %s, 您的新帐号已经成功被登记了。\n", BBSNAME);
    printf("本站地址是 %s, 支持WWW, telnet\n", BBSHOST);
    printf("等多种登录方式.\n");
    printf("您目前拥有本站基本的权限, 包括阅读文章、环顾四方、接收私人\n");
    printf("信件、接收他人的消息、进入聊天室等等。当您通过本站的身份确\n");
    printf("认手续之后，您还会获得更多的权限。目前您的注册单已经被提交\n");
    printf("等待审阅。一般情况24小时以内就会有答复，请耐心等待。同时请\n");
    printf("留意您的站内信箱。\n");
    printf
        ("如果您有任何疑问，可以去BBSHelp(新手求助)板发文求助。\n\n</pre></table>");
    printf("<hr><br>您的基本资料如下:<br>\n");
    printf("<table border=1 width=400>");
    printf("<tr><td>帐号位置: <td>%d\n", getusernum(x.userid));
    printf("<tr><td>使用者代号: <td>%s (%s)\n", x.userid, x.username);
    printf("<tr><td>姓  名: <td>%s<br>\n", x.realname);
    printf("<tr><td>昵  称: <td>%s<br>\n", x.username);
    printf("<tr><td>上站位置: <td>%s<br>\n", x.lasthost);
    printf("<tr><td>电子邮件: <td>%s<br></table><br>\n", x.email);
    printf
        ("<center><input type=button onclick='window.close()' value=关闭本窗口></center>\n");
    newcomer(&x, words);
    sprintf(buf, "%s %-12s %d\n", Ctime(time(0)) + 4, x.userid,
            getusernum(x.userid));
    f_append("wwwreg.log", buf);
    sprintf(buf, "uid %d from %s", getusernum(x.userid), fromhost);
    log_usies("APPLY", buf);
    printf("\n</html>");
    return 0;

}

static int badstr(char *s)
{
    int i;

    for (i = 0; s[i]; i++)
        if (s[i] != '\x09'
            && ((s[i] < '\x20' && s[i] > 0) || s[i] == '\xFF'))
            return 1;
    return 0;
}

static void newcomer(struct userec *x, char *words)
{
    FILE *fp;
    char filename[80];

    char nickname[80];

    time_t now = time(0);

    sprintf(nickname, "WWW新手上路: %s", x->username);

    sprintf(filename, "tmp/%d.tmp", getpid());
    fp = fopen(filename, "w");
    fprintf(fp, "大家好, \n\n");
    fprintf(fp, "我是 %s(%s), 来自 %s\n", x->userid, x->username,
            fromhost);
    fprintf(fp, "今天初来此地报到, 请大家多多指教.\n\n");
    fprintf(fp, "自我介绍:\n\n");
    fprintf(fp, "%s", words);
    fclose(fp);
    post_article("message", nickname, filename, x->userid,
                 x->username, fromhost, -1, 0, now, now);
    unlink(filename);
}

static void adduser(struct userec *x)
{
    int i;
    FILE *fp;

    fp = fopen(".PASSWDS", "r+");
    flock(fileno(fp), LOCK_EX);
    for (i = 0; i < MAXUSERS; i++)
    {
        if (shm_ucache->userid[i][0] == 0)
        {
            strncpy(shm_ucache->userid[i], x->userid, 13);
            save_user_data(x);
            if (shm_ucache->number < i + 1)
                shm_ucache->number = i + 1;
            break;
        }
    }
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}
