#define _XOPEN_SOURCE 500       /* for pwrite/pread */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"

static int passwd_fd = -1;

gboolean ntcuser_init(void)
{
    passwd_fd = open(PASSFILE, O_RDWR);
    if (passwd_fd < 0)
    {
        g_warning("can't not open passwd file at" PASSFILE);
        return FALSE;
    }

    return TRUE;
}

void ntcuser_final(void)
{
    if (passwd_fd > 0)
    {
        close(passwd_fd);
        passwd_fd = -1;
    }
    return;
}

void ntcuser_reset(void)
{
    return;
}

int nt_get_passwd_pos(const char *userid)
{
    register int i;
    char (*shmuserid)[IDLEN + 1] = shm_ucache->userid;

    for (i = 0; i < MAXUSERS; i++)
    {
        if (!strcasecmp(shmuserid[i], userid))
            return i;
    }
    return -1;
}

gboolean nt_get_userec_with_uid(int uid, struct userec * puser)
{
    /* uid stored in shm_utmp is starting from 1 */
    int pos;

    pos = uid - 1;
    if (pos < 0)
        return FALSE;
    if (sizeof(struct userec) !=
        pread(passwd_fd, puser,
              sizeof(struct userec), pos * sizeof(struct userec)))
    {
        g_warning("can't pread from password file");
        return FALSE;
    }
    return TRUE;
}

gboolean nt_get_userec(const char *userid, struct userec * puser)
{
    int pos;

    pos = nt_get_passwd_pos(userid);
    if (pos < 0)
        return FALSE;
    return nt_get_userec_with_uid(pos + 1, puser);
}

gboolean nt_set_userec(struct userec * puser)
{
    int pos;

    pos = nt_get_passwd_pos(puser->userid);
    if (pos < 0)
        return FALSE;
    return nt_set_userec_with_uid(puser, pos + 1);
}

gboolean nt_set_userec_with_uid(struct userec * puser, int uid)
{
    /* uid stored in shm_utmp is starting from 1 */
    if (uid < 1)
        return FALSE;
    if (sizeof(struct userec) !=
        pwrite(passwd_fd, puser,
               sizeof(struct userec), (uid - 1) * sizeof(struct userec)))
    {
        g_warning("can't pwrite to password file");
        return FALSE;
    }
    return TRUE;
}

gboolean nt_logout_user(struct user_info * puinfo)
{
    int stay = 0;
    struct userec u;

    if (!puinfo->active)
    {
        g_warning("tried to logout an unactive user");
        return FALSE;
    }
    if ((!nt_is_web_user(puinfo)) && puinfo->pid != getpid())
    {
        kill(SIGHUP, puinfo->pid);
        return TRUE;
    }
    stay = abs(time(0) - puinfo->logintime);
    if (stay > 3600 * 24 * 3)
        stay = 3600 * 24 * 3;
    else if (stay < 0)
        stay = 0;

    if ((!nt_get_userec_with_uid(puinfo->uid, &u))
        || strcasecmp(u.userid, puinfo->userid))
    {
        g_warning("user_info and userec are not matching");
        return FALSE;
    }
    u.stay += stay;
    u.lastlogout = time(0);
    nt_set_userec_with_uid(&u, puinfo->uid);
    memset(puinfo, 0, sizeof(struct user_info));
    return TRUE;
}

extern char *crypt1(char *buf, char *salt);

gboolean nt_check_passwd(char *pw_crypted, char *pw_try)
{
    return !strcmp(crypt1(pw_try, pw_crypted), pw_crypted);
}

char *nt_crypt_passwd(char *pw)
{
    char salt[3];

    sprintf(salt, "%c%c", 65 + rand() % 26, 65 + rand() % 26);
    return crypt1(pw, salt);
}

int nt_new_uinfo(const char *userid, struct user_info **ppuinfo)
{
    int fd, n;
    struct user_info *shmuinfo = shm_utmp->uinfo;
    struct user_info *u;

    fd = nt_open_lock_file(".UTMP.lock", O_RDWR | O_CREAT);
    if (fd < 0)
        return -1;

    for (n = 0; n < MAXACTIVE; n++)
    {
        if (shmuinfo[n].active == 0)
        {
            u = &(shmuinfo[n]);
            memset(u, 0, sizeof(struct user_info));
            u->active = 1;
            g_strlcpy(u->userid, userid, 13);
            u->uid = nt_get_passwd_pos(userid) + 1;
            u->pid = getpid();
            nt_close_lock_file(fd);
            *ppuinfo = u;
            return n;
        }
    }
    nt_close_lock_file(fd);
    return -1;
}
int nt_get_active_uinfo(const char *userid, struct user_info *puinfos[],
                        int max)
{
    register int i = 0, num = 0;
    struct user_info *shmuinfos = shm_utmp->uinfo;

    if (max < 1)
        return 0;

    for (; i < MAXACTIVE; i++)
    {
        if (shmuinfos[i].active == 0)
            continue;
        if (!strcasecmp(shmuinfos[i].userid, userid))
        {
            puinfos[num] = &(shmuinfos[i]);
            num++;
            if (num == max)
                break;
        }
    }
    return num;
}

int nt_get_active_uinfo_condition(struct user_info *puinfos[], int max,
                                  gboolean(*condition) (struct user_info *,
                                                        void *),
                                  void *userdata)
{
    register int i = 0, num = 0;
    struct user_info *shmuinfos = shm_utmp->uinfo;

    if (max < 1)
        return 0;

    for (; i < MAXACTIVE; i++)
    {
        if (shmuinfos[i].active == 0)
            continue;
        if (condition(&(shmuinfos[i]), userdata))
        {
            puinfos[num] = &(shmuinfos[i]);
            num++;
            if (num == max)
                break;
        }
    }
    return num;
}

void
nt_foreach_active_uinfo(gboolean(*walk_func) (struct user_info *, void *),
                        void *userdata)
{
    register unsigned int i = 0;
    struct user_info *shmuinfos = shm_utmp->uinfo;

    for (; i < MAXACTIVE; i++)
    {
        if (shmuinfos[i].active == 0)
            continue;
        if (!walk_func(&(shmuinfos[i]), userdata))
            break;
    }
    return;
}

unsigned int nt_get_online_num(void)
{
    register unsigned int i = 0, num = 0;
    struct user_info *shmuinfos = shm_utmp->uinfo;

    for (; i < MAXACTIVE; i++)
    {
        if (shmuinfos[i].active)
            num++;
    }
    return num;
}

gboolean nt_check_login_limit(struct userec * puser)
{
    FILE *fp;
    char filename[100];
    int num = 0;

    if (HAS_PERM_SYSOP(puser))
        return TRUE;

    nt_sethomefile(filename, sizeof(filename), puser->userid,
                   "logincheck");
    if (!g_file_test(filename, G_FILE_TEST_EXISTS)
        || abs(nt_stat_mtime(filename) - time(0)) > 86400)
    {
        fp = fopen(filename, "w");
        fprintf(fp, "%d", puser->numlogins);
        fclose(fp);
        return TRUE;
    }
    fp = fopen(filename, "r");
    if (fp == 0)
    {
        g_warning("can't open logincheck file at '%s'", filename);
        return FALSE;
    }
    fscanf(fp, "%d", &num);
    fclose(fp);
    if (abs(num - puser->numlogins) > 100)
    {
        char buf[256];

        sprintf(buf, "%24.24s %s\n", nt_ctime(time(0)), puser->userid);
        nt_file_append("login.log", buf);
        return FALSE;
    }
    return TRUE;
}

gboolean nt_msg_www_has_new(const char *userid)
{
    char path[1024];

    nt_sethomefile(path, sizeof(path), userid, "wwwmsg");
    return nt_stat_size(path) > 0;
}
