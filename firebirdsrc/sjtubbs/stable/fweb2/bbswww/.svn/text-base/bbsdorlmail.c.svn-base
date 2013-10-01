#include "www.h"

static int
bbs_sendmail(const char *fname, const char *title, const char *receiver)
{
    char genbuf[1024];
    FILE *fin, *fout;

    sprintf(genbuf, "%s -f %s.bbs@%s %s", SENDMAIL,
            currentuser.userid, BBSHOST, receiver);
    fout = popen(genbuf, "w");
    fin = fopen(fname, "r");
    if (fin == NULL || fout == NULL)
        return -1;

    fprintf(fout, "Return-Path: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "Reply-To: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "From: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "To: %s\n", receiver);
    fprintf(fout, "Subject: %s\n", title);
    fprintf(fout, "X-Forwarded-By: %s (%s)\n", currentuser.userid,
            currentuser.username);

    fprintf(fout, "X-Disclaimer: %s 对本信内容恕不负责。\n", BBSNAME);
    
    fprintf(fout, "MIME-Version: 1.0\n");
    fprintf(fout, "Content-Type: text/plain; charset=GB2312\n");
    fprintf(fout, "Content-Transfer-Encoding: 8bit\n");
    
    fprintf(fout, "Precedence: junk\n\n");

    while (fgets(genbuf, 255, fin) != NULL)
    {
        if (genbuf[0] == '.' && genbuf[1] == '\n')
            fputs(". \n", fout);
        else
            fputs(genbuf, fout);
    }

    fprintf(fout, ".\n");

    fclose(fin);
    pclose(fout);
    return 0;
}

#ifdef REAL_NAME_EMAIL
static int check_email_id(const char *id)
{
    int i;

    for (i = 0; id[i]; i++)
        if (!
            (isalnum(id[i]) || id[i] == '.' || id[i] == '_'
             || id[i] == '-'))
            return 1;
    return 0;
}

inline static int is_strcase_with_prefix(const char *prefix, int len,
                                         const char *str)
{
    return !strncasecmp(prefix, str, len);
}
inline static int is_strcase_with_suffix(const char *suffix, int len,
                                         const char *str)
{
    int slen;

    slen = strlen(str);
    if (slen < len)
        return 0;
    return !strncasecmp(suffix, str + slen - len, len);
}
static int check_email_domain(const char *domain)
{
    FILE *fp;
    char line[512];
    int len;
    int i;

    if (!(isalnum(domain[0]) && isalpha(domain[strlen(domain) - 1])))
        return 1;
    for (i = 0; domain[i]; i++)
        if (!(isalnum(domain[i]) || domain[i] == '.' || domain[i] == '-'))
            return 1;

    if (!(fp = fopen(REAL_NAME_EMAIL_RULE, "r")))
        return 1;
    while (fgets(line, sizeof(line), fp))
    {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            len--;
        }
        if (len < 2)
            continue;

        switch (line[0])
        {
        case 'P':
            if (is_strcase_with_prefix(&line[1], len - 1, domain))
                return 1;
            else
                break;
        case 'p':
            if (is_strcase_with_prefix(&line[1], len - 1, domain))
                return 0;
            else
                break;
        case 'S':
            if (is_strcase_with_suffix(&line[1], len - 1, domain))
                return 1;
            else
                break;
        case 's':
            if (is_strcase_with_suffix(&line[1], len - 1, domain))
                return 0;
            else
                break;
        case 'A':
            if (!strcasecmp(&line[1], domain))
                return 1;
            else
                break;
        case 'a':
            if (!strcasecmp(&line[1], domain))
                return 0;
            else
                break;
        default:
            break;
        }
    }
    fclose(fp);
    return 1;
}

static int check_real_name_email(const char *email)
{
    char *pat, *domain, buf[REAL_NAME_EMAIL_LEN + 1];

    strcpy(buf, email);
    pat = strchr(buf, '@');
    if (!pat)
        return 1;
    *pat = '\0';
    domain = pat + 1;
    if (check_email_id(buf) || check_email_domain(domain))
        return 1;
    else
        return 0;
}

static int check_real_name_email_blocked(const char *email)
{
    char buf[REAL_NAME_EMAIL_LEN + 1];
    char namelist[] = BBSHOME "/etc/bad_remail";
    FILE *fp;

    fp = fopen(namelist, "r");
    if (fp == NULL)
        return 1;
    while (fgets(buf, 255, fp) != NULL)
    {
        if (strncasecmp(email, buf, strlen(email)))
        {
            continue;
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1;
}

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

static int write_real_name_email(const char *email)
{
    char path[128];
    FILE *file;

    sethomefile(path, currentuser.userid, REAL_NAME_FILE);
    if ((file = fopen(path, "w")) == NULL)
    {
        return 1;
    }
    fprintf(file, email);
    fclose(file);
    return 0;
}
static int get_random(unsigned int buf[], int size);
static int mailto_real_name_mail(char *userid)
{
    char email[REAL_NAME_EMAIL_LEN + 1], fname[256], code_fname[256];
    const char title[] = BBSNAME "注册确认信";
    unsigned int rand[4], return_no;
    FILE *content, *code;

    get_real_name_email(email);
    if (!email[0])
        return 1;
    get_random(rand, 4);
    sprintf(fname, "tmp/check_mail.%s", userid);
    content = fopen(fname, "w");
    if (content == NULL)
        return 2;
    fprintf(content, "\n\
    你好！这是%s(%s)注册确认信。\n\
    如果你曾在%s注册ID %s,请用这个的链接完成注册：\n\
    http://%s/bbsdochkmail?id=%s&code=%08x%08x%08x%08x\n\
    如果你没有注册过此ID，请删除本信。我们为此信的打扰感到歉意。\n\n\n\
                                                  %s站务组\n", BBSNAME, BBSHOST, BBSNAME, userid, BBSHOST, userid, rand[0], rand[1], rand[2], rand[3], BBSNAME);
    fclose(content);
    sethomefile(code_fname, userid, REAL_NAME_CHK_CODE);
    code = fopen(code_fname, "w");
    if (code == NULL)
    {
        unlink(fname);
        return 3;
    }
    fprintf(code, "%08x%08x%08x%08x", rand[0], rand[1], rand[2], rand[3]);
    fclose(code);
    return_no = bbs_sendmail(fname, title, email);
    unlink(fname);
    return return_no;
}
#endif

static int get_random(unsigned int buf[], int size)
{
    int fd;

    if ((fd = open("/dev/urandom", O_RDONLY)) != -1)
    {
        read(fd, buf, sizeof(unsigned int) * size);
        close(fd);
    }
    else
    {
        int i;

        for (i = 0; i < size; i++)
            buf[i] = random();
    }
    return 0;
}

int bbsdorlmail_main()
{
    char email[REAL_NAME_EMAIL_LEN + 1];

    init_all();
    if (!loginok)
    {
        redirect("/");
        http_quit();
    }

    if (HAS_PERM(PERM_PERSONAL))
    {
        redirect("/frame2.html");
        http_quit();
    }
    strncpy(email, getparm("email"), REAL_NAME_EMAIL_LEN);
    if (check_real_name_email(email))
    {
        printf("邮箱不正确,请重新输入");
        http_quit();
    }
    if (!check_real_name_email_blocked(email))
    {

        http_quit();
    }
    write_real_name_email(email);
    if (mailto_real_name_mail(currentuser.userid))
    {
        printf("发送确认信件失败,请重新输入");

    }
    else
    {
        refreshto("/frame2.html", 3);
        printf("已发送确认信,请查收. 3秒后自动转入bbs界面");
    }
    printf("\n</html>");
    return 0;

}
