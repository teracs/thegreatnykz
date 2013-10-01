#include "www.h"

int bbsdochkmail_main()
{
    char *id;
    char *code, rlcode[32];
    char path[1024];
    int fd;
    struct userec *user;

    init_all();
    id = getparm("id");
    code = getparm("code");
    sethomefile(path, id, REAL_NAME_CHK_CODE);
    fd = open(path, O_RDONLY);
    if (fd == -1)
        http_quit_print("无此用户或已经认证");
    read(fd, rlcode, 32);
    close(fd);
    if (strncmp(code, rlcode, 32))
    {
        printf("无此用户,请确认链接地址正确");
        return 0;
    }
    unlink(path);
    user = getuser(id);
    if (!user)
    {
        printf("无此用户,请确认链接地址正确");
        return 0;
    }
    user->userlevel = user->userlevel | PERM_REALNAME;
    save_user_data(user);
    refreshto("/", 5);
    printf("认证成功。5秒后转入首页");
    return 0;
}
