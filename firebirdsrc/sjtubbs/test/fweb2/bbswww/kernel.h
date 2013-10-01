#ifndef _H_KERNEL
#define _H_KERNEL

#include <glib.h>
#include "dispatcher.h"
#include "bbs.h"
#include "const.h"

G_BEGIN_DECLS
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
struct deny
{
    char id[80];
    char exp[80];
    //added by hongliang on Nov. 23th, 2003 for whether auto undeny
    char autoundeny[10];
    char free_time[30];
};

/*  LISTMASK    投票受投票名单限制 */
#define LISTMASK        PERM_POSTMASK
/*  VOTEMASK    投票受上站时间等条件限制 */
#define VOTEMASK        PERM_NOZAP
/* 如果票箱的 level 属性还含有其他权限限制，亦起作用 */

struct ballot
{
    char uid[IDLEN + 1];        /* 投票人       */
    unsigned int voted;         /* 投票的内容   */
    char msg[3][STRLEN];        /* 建议事项     */
};

struct voter_info
{
    char uid[IDLEN + 1];        // 投票人
    time_t vote_time;           // 投票时间
    char uip[48];               // 投票ip
};

struct votebal
{
    char userid[15];
    char showip;
    char title[STRLEN];
    char type;
    char items[32][40];
    int maxdays;
    int maxtkt;
    int totalitems;
    time_t opendate;
    unsigned int level;
    unsigned int x_logins;      //上站次数
    unsigned int x_posts;       //发表文章数
    unsigned int x_stay;        //上站的实际小时数
    unsigned int x_live;        //注册的物理时间
};

extern char seccode[SECNUM][5];
extern char secname[SECNUM][2][20];
extern int loginok;
extern int pic_mod;
extern int silence;
extern struct userec currentuser;
extern struct user_info *u_info;
extern char fromhost[256];
extern int my_t_lines;
extern int my_link_mode;
extern int my_def_mode;
extern int my_font_mode;
extern int my_fontn_mode;
extern int my_ansisize_mode;
extern int my_ansifont_mode;
extern int my_theme_mode;
extern int friendnum;
extern struct override fff[200];
extern int friendnum;
extern int friend_inited;
extern struct override bbb[32];
extern int badnum;
extern int is_normal_board;
extern struct deny denyuser[256];
extern int denynum;

extern char *eff_size(char *file);
extern char *crypt1(char *pw, char *salt);
extern char *brc_getrecord(char *ptr, char *name, int *pnum, int *list);
extern char *brc_putrecord(char *ptr, char *name, int num, int *list);
extern void brc_update(char *userid, char *board);
extern int brc_init(char *userid, char *boardname);
extern int brc_add_read(char *filename);
extern int brc_un_read(char *file);
extern int brc_clear(void);
extern int brc_has_read(char *file);
extern int file_has_word(char *file, char *word);
extern int f_append(char *file, char *buf);
extern struct stat *f_stat(char *file);
extern int file_time(char *file);
extern int get_record(void *buf, int size, int num, char *file);
extern int put_record(void *buf, int size, int num, char *file);
extern int del_record(char *file, int size, int num);
extern char *Ctime(time_t t);
extern char *noansi(char *s);
extern char *nohtml(char *s);
extern char *strright(char *s, int len);
extern char *strcasestr(const char *s1, const char *s2);
extern int strsncpy(char *s1, char *s2, int n);
extern char *ltrim(char *s);
extern char *rtrim(char *s);
extern char *trim(char *s);
extern char *getsenv(char *s);
extern int strnncpy(char *s, int *l, char *s2);
extern int hsprintf(char *s, char *fmt, ...);
extern int hprintf(char *fmt, ...);
extern int change1(char *buf);
extern int hhprintf(char *fmt, ...);
extern int bbs_http_init(void);
extern char *getparm(char *var);
extern int user_init(struct userec *x, struct user_info **y);
extern int post_mail(char *userid, char *title, char *file, char *id,
                     char *nickname, char *ip, int sig);
extern int post_imail(char *userid, char *title, char *file, char *id,
                      char *nickname, char *ip, int sig);
extern int sig_append(FILE * fp, char *id, int sig);
extern char *anno_path_of(char *board);
extern int has_BM_perm(struct userec *user, char *board);
extern int has_badmin_perm(struct userec *user, struct shortfile *x);
extern int has_read_perm(struct userec *user, char *board);
extern int has_post_perm(struct userec *user, char *board);
extern int too_young(struct userec *user, char *board);
extern struct shortfile *getbcache(char *board);
extern int getbnum(char *board);
extern int count_on_board(char *board);
extern int count_mails(char *id, int *total, int *unread);
extern int findnextutmp(char *id, int from);
extern int sethomefile(char *buf, char *id, char *file);
extern int send_msg(char *myuserid, int mypid, char *touserid, int topid,
                    char msg[256], int isWeb);
extern char *horoscope(int month, int day);
extern char *ModeType(int mode);
extern char *cexp0(int exp, int type);
extern char *cexp2(int exp);
extern char *cexp3(int exp);
extern char *cperf(int perf);
extern int count_life_value(struct userec *urec);
extern int countexp(struct userec *x);
extern int countperf(struct userec *x);
extern int modify_mode(struct user_info *x, int newmode);
extern int save_user_data(struct userec *x);
extern int is_bansite(char *ip);
extern int user_perm(struct userec *x, int level);
extern int getusernum(char *id);
extern int getuser_like(char *userid);
extern int str_is_like(char *userid, char *userid2);
extern struct userec *getuser(char *id);
extern int checkpasswd(char *pw_crypted, char *pw_try);
extern char *crypt_passwd(char *pw);
extern int checkuser(char *id, char *pw);
extern int count_id_num(char *id);
extern int count_online(void);
extern int count_online2(void);
extern int loadfriend(char *id);
extern int isfriend(char *id);
extern int loadbad(char *id);
extern int isbad(char *id1, char *id2);
extern int init_all(void);
extern char *void1(char *s);
extern char *sec(char c);
extern char *flag_str(int access);
extern char *flag_str2(int access);
extern char *userid_str(char *s);
extern int fprintf2(FILE * fp, char *s);
extern struct fileheader *get_file_ent(char *board, char *file,
                                       int isPost);
extern char *getbfroma(char *path);
extern int load_my_www(void);
extern char *bg(char *board);
extern char *nohtml2(char *s);
extern char *void3(char *s);
extern float get_load(int i);
extern void *file_map(char *file, int len);
extern int in_pw_lock(char *userid, char *ip);
extern int add_pw_lock(char *userid, char *ip);
extern int del_pw_lock(char *userid, char *ip);
extern int check_pw_lock();
extern int check_maxmail2(int *maxmail, int *size);
extern int tcp_open(char *host, int port);
extern char *nohtml3(char *s);
extern char *nohtml5(char *s);
extern char *nohtml4(char *s);
extern char *Gtime(time_t t);
extern int count_wwwguest();
extern int check_post_limit();
extern struct tm *Ltime(time_t t);
extern int Day(time_t t);
extern int today(void);
extern int check_today();
extern int start_serv(int is_local, int port, uid_t uid, gid_t gid);
extern int file_size(char *file);
extern int file_rtime(char *file);
extern int file_exist(char *file);
extern int file_isdir(char *file);
extern int file_isfile(char *file);
extern int base64(char c);
extern char *unbase64(char *buf);
extern void *map_board_dir(char *board, char *file);
extern int unmap_board_dir(void);
extern struct fileheader *find_in_map_dir(char *filename);
extern struct fileheader *pre_in_map_dir(void);
extern struct fileheader *next_in_map_dir(void);
extern int get_offset_of_mapdir(void);
extern int set_offset_of_mapdir(int offset);
extern int get_size_of_mapdir(void);
extern int print_style(void);
extern int seek_in_denyfile(char *board, char *seekstr);
extern void bbsautoundeny(char board[80], char *userid);
extern int ci_strcmp(register char *s1, register char *s2);
extern int inform(char *board, char *user);
extern void inform_autoundeny(char *board, char *user);
extern void bbsautoundeny(char board[80], char *userid);
extern int show_bm_words(char *board, int width);
extern int do_cache(char *filename, char *cachefilename);
extern int insjtunetwork(char *ip);
extern int noregist(void);

/*extern void *Malloc(size_t size);*/
#define Malloc malloc
extern char *url_quote(char *url);
extern int is_bad_id(char *userid);
extern int LoadMyBrdsFromGoodBrd(char *userid,
                                 char mybrd[GOOD_BRC_NUM][80]);
extern void SaveMyBrdsToGoodBrd(char *userid,
                                char mybrd[GOOD_BRC_NUM][80],
                                int mybrdnum);
extern int post_article(char *board, char *title, char *file, char *id,
                        char *nickname, char *ip, int sig, int autocr,
                        unsigned int currid, unsigned int reid);
extern void post_article2(char *board, char *title, char *file, char *id,
                          unsigned int currid, unsigned int reid);
extern void html_head(void);
extern int junkboard(char *board);
extern int return_del_article(char *file, int size, int num);
extern int loaddenyuser(char *board);
extern int savedenyuser(char *board);
extern int getdatestring(time_t now, int Chang);
extern void log_usies(char *mode, char *mesg);
extern int has_fill_form(void);
extern int is_web_user(struct user_info *user);
extern int kick_web_user(struct user_info *user);
extern void show_bm(char firstchar, char *userid);
extern void show_dm(char *userid);
extern gboolean bbswww_init(EVENT_ID unload_event, EVENT_ID enter_event,
                            EVENT_ID exit_event);
extern long get_num_records(char *filename, int size);
extern void cgi_head(void);
extern void http_quit(void);
extern void http_quit_print(char *str);
extern void http_fatal(char *fmt, ...);
extern void setcookie(char *s, char *b);
extern void redirect(char *x);
extern void refreshto(char *x, int t);
extern char *getfolderstr(char *filename);
extern int safewrite(int fd, char *buf, int size);
extern int substitute_record(char *filename, void *rptr, int size, int id);
extern int cmpvfilename(char *filename, void *vfn);
extern int search_record(char *filename, char *rptr, int size,
                         int (*fptr) (), char *farg);
extern int change_title(char *fname, char *title);
extern int board_in_which_dist(char *boardname);
extern int user_enter_board(char *boardname);
extern int user_exit_previous_board();
extern int canmsg(struct user_info *uin);
extern int canpage(int friend, int pager);
extern char *strnzhcpy(char *dest, const char *src, int n);
extern int search_in_anonyindex(char* userid, char* ranstr, struct anony_item *item_p);
extern char* get_anonyid(char* userid, char* boardname);
extern char *setmdir(char *buf, char *userid);

#define HEADER_NO_STORE 0
#define HEADER_NO_CACHE -1
#define HEADER_PRIVATE -2
extern int cache_age;
extern time_t last_modified_time;

G_END_DECLS
#endif
