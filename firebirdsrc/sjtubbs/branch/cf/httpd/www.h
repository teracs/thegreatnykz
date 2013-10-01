#include "bbs.h"
#include "stdarg.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/socket.h"
#include "netdb.h"
#include "netinet/in.h"
#include "const.h"
#include "types.h"
#include "gdbm.h"


#ifdef CTRL
#undef CTRL
#endif
#define CTRL(x)         (x & 31)

#define FORMAT(fmt, buf, len) \
	va_list ap; \
	va_start(ap, fmt); \
	vsnprintf(buf, len, fmt, ap); \
	va_end(ap); \
	if(len>=1) buf[len-1]=0;

struct post_log
{
    char author[IDLEN + 1];
    char board[18];
    char title[66];
    time_t date;
    int number;
};

struct dir
{
    char board[20];
    char userid[14];
    char showname[40];
    char exp[80];
    char type[30];
    int filename;
    int date;
    int level;
    int size;
    int live;
    int click;
    int active;
    int accessed;
};

extern struct PW_LOCK_FILE *pw_lock_file;
extern char seccode[SECNUM][5];
extern char secname[SECNUM][2][20];
extern int loginok;
extern int pic_mod;
extern int silence;
extern struct userec currentuser;
extern struct user_info *u_info;
extern struct UTMPFILE *shm_utmp;
extern struct BCACHE *shm_bcache;
extern struct UCACHE *shm_ucache;
extern struct GUEST_INFO *shm_guest;
extern struct UEXT *uext;
extern char fromhost[256];
extern int my_t_lines;
extern int my_link_mode;
extern int my_def_mode;
extern int my_font_mode;
extern int my_fontn_mode;
extern int my_ansisize_mode;
extern int my_ansifont_mode;
extern int friendnum;
extern struct override fff[200];
extern int friendnum;
extern int friend_inited;
extern struct override bbb[32];
extern int badnum;
extern char parm_name[256][80];
extern char *parm_val[256];
extern int parm_num;
extern int keep_alive;
extern int has_http_head;
extern char **environ;

#define HEADER_NO_STORE 0
#define HEADER_NO_CACHE -1
#define HEADER_PRIVATE -2
extern int cache_age;
extern time_t last_modified_time;
extern int is_normal_board;

