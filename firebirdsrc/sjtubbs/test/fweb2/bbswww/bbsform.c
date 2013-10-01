#include "www.h"
static void check_if_ok()
{
    if (user_perm(&currentuser, PERM_LOGINOK))
        http_quit_print("您已经通过本站的身份认证, 无需再次填写注册单.");
    if (has_fill_form())
        http_quit_print("目前站长尚未处理您的注册单，请耐心等待.");
}

static int check_submit_form()
{
    FILE *fp = NULL;
    struct regform record;

    memset(&record, 0, sizeof(record));
    record.usernum = getusernum(currentuser.userid) + 1;
    strncpy(record.date, Ctime(time(0)), 30);
    strncpy(record.userid, currentuser.userid, IDLEN + 2);
    strncpy(record.realname, getparm("realname"), NAMELEN);
    strncpy(record.dept, getparm("dept"), STRLEN);
    strncpy(record.addr, getparm("address"), STRLEN);
    strncpy(record.phone, getparm("phone"), STRLEN);
    record.status = 0;

    fp = fopen("register_sjtubbs", "a");
    if (fp == 0)
        http_quit_print("注册文件错误，请通知SYSOP");

    fwrite(&record, sizeof(struct regform), 1, fp);
    fclose(fp);

    printf
        ("您的注册单已成功提交. 站长检验过后会给您发信, 请留意您的信箱.");
    return 0;
}

int bbsform_main()
{
    int type;

    init_all();
    type = atoi(getparm("type"));
    if (!loginok)
        http_quit_print("您尚未登录, 请重新登录。");
    printf("<font class=\"title\">%s - 填写注册单</font><hr>\n", BBSNAME);
    check_if_ok();
    if (type == 1)
    {
        check_submit_form();
        http_quit();
    }
    printf
        ("您好, %s, 请如实详细填写以下各项内容, 经审阅通过后即可获得本站注册用户的各项权限.<br><hr>\n",
         currentuser.userid);
    printf("<form method=post action=bbsform>\n");
    printf("<input type=hidden name=type value=1>\n");
    printf
        ("*真实姓名: <input name=realname type=text maxlength=8 size=8 value='%s'> (请用中文)<br>\n",
         nohtml(currentuser.realname));
    printf
        ("*学校系级: <input name=dept type=text maxlength=64 size=32 value='%s'> (在校者请填写学校、系别、年级, 已工作者请填写工作单位)<br>\n",
         nohtml(currentuser.reginfo));
    printf
        ("*居住地址: <input name=address type=text maxlength=64 size=32 value=''> (请详细填写您的居住地址, 包括寝室或门牌号码)<br>\n");
    printf
        ("*联络电话: <input name=phone type=text maxlength=32 size=32> (最好能填写, 没有可写'无')<br><hr><br>\n");
    printf("<input type=submit> <input type=reset>");
    printf("</html>");
    return 0;
}
