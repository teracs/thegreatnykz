#include "www.h"

#define REG_GOOD_ID 0
#define REG_EXIST_ID 1
#define REG_SIMILAR_ID 2
#define REG_BAD_ID 3
#define REG_SHORT_ID 4
#define REG_NOTENGLISH_ID 5
#define REG_OTHERS 6

static int check_id_type(char *myid)
{
    int i = 0;

    if (strlen(myid) < 2)
        return REG_SHORT_ID;

    for (i = 0; myid[i]; i++)
        if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", toupper(myid[i])))
            return REG_NOTENGLISH_ID;

    if (is_bad_id(myid))
        return REG_BAD_ID;
    if (getuser(myid))
        return REG_EXIST_ID;
    if (getuser_like(myid))
        return REG_SIMILAR_ID;
    return REG_GOOD_ID;
}

int bbschkid_main()
{
    char myid[14] = { 0 };
    int id_type = 0;

    init_all();
    strsncpy(myid, getparm("userid"), 12);

    id_type = check_id_type(myid);

    switch (id_type)
    {
    case REG_NOTENGLISH_ID:
        printf
            ("<font color=red>对不起，帐号只能由英文字母组成:) </font>\n\n");
        break;
    case REG_SHORT_ID:
        printf
            ("<font color=red>对不起，ID长度至少需要两个字符:) </font>\n\n");
        break;
    case REG_BAD_ID:
        printf
            ("<font color=red>不雅帐号或禁止注册的id, 请重新选择:) </font>\n\n");
        break;
    case REG_EXIST_ID:
        printf
            ("<font color=red>对不起，此帐号已经有人使用,请重新选择:) </font>\n\n");
        break;
    case REG_SIMILAR_ID:
        printf
            ("<font color=red>对不起，此帐号与已有帐号过于相似，请重新选择:) </font>\n\n");
        break;
    case REG_GOOD_ID:
        printf
            ("<font color=green>恭喜您，您可以注册这个ID! :) </font>\n\n");
        break;
    default:
        printf
            ("<font color=red>未知错误，反正您就是不能注册这个ID啦:) </font>\n\n");
        break;
    }
    printf("\n</html>");
    return 0;
}
