#include <stdlib.h>
#include <time.h>

#include "bbs.h"
#include "ntwww.h"
#include "ntcore.h"
#include "fwebtpl.h"
#include "fwebio.h"
#include "cgic.h"
#include "shm.h"
#include "fweb.h"
#include "mempool.h"
#include "const.h"

gboolean ntw_haslogin;
gboolean ntw_notguest;
struct userec ntw_curuser;
struct user_info *ntw_puinfo;
time_t ntw_now;

static inline void www_user_init(void)
{
    struct userec user;
    struct user_info *uinfo;

    char id[IDLEN + 2];
    int num, key;

    cgiCookieString("utmpuserid", id, IDLEN + 2);
    cgiCookieInteger("utmpnum", &num, -1);
    cgiCookieInteger("utmpkey", &key, -1);

    if (num < 0 || num >= MAXACTIVE)
        return;
    uinfo = &(shm_utmp->uinfo[num]);

    if (strcmp(uinfo->from, cgiRemoteAddr)
        || (uinfo->utmpkey != key)
        || (uinfo->active == 0)
        || (uinfo->userid[0] == 0)
        || (!nt_is_web_user(uinfo))
        || (!strcasecmp(uinfo->userid, "new"))
        || (!nt_get_userec(uinfo->userid, &user))
        || strcmp(user.userid, uinfo->userid))
    {
        return;
    }
    ntw_puinfo = uinfo;
    memcpy(&ntw_curuser, &user, sizeof(user));
    ntw_haslogin = TRUE;
    if (strcasecmp(uinfo->userid, "guest"))
        ntw_notguest = TRUE;

    uinfo->idle_time = ntw_now;

    return;
}
static inline void www_reset_global_var(void)
{
    ntw_haslogin = FALSE;
    ntw_notguest = FALSE;
    memset(&ntw_curuser, 0, sizeof(ntw_curuser));
    ntw_puinfo = NULL;
    ntw_now_update();
    return;
}
static inline void www_log_action(void)
{
    if (ntw_haslogin)
        g_message("%s %s %s?%s", cgiRemoteAddr, ntw_curuser.userid,
                  cgiScriptName, cgiQueryString);
    else
        g_message("%s guest %s?%s", cgiRemoteAddr, cgiScriptName,
                  cgiQueryString);
    return;

}
static CALLBACK_RETURN www_init_request(CALLBACK_ID callback_id,
                                        void *data, void *userdata)
{
    www_reset_global_var();
    nt_reset();
    www_user_init();
    www_log_action();
    return CALLBACK_CONTINUE;
}

#define SESSION_LOCK_FILE ".CLEAN_SESSION.LOCK"
#define SESSION_IDLE_TIME 1800
#define SESSION_CHECK_INTERVAL 300

static gboolean check_session(struct user_info *puinfo, void *userdata)
{

    if ((puinfo->idle_time < ntw_now - SESSION_IDLE_TIME)
        && nt_is_web_user(puinfo))
    {
        ntw_logout_user(puinfo);
    }
    return TRUE;
}
static void clean_session(void)
{
    time_t last_time;
    int fd;

    last_time = nt_stat_mtime(SESSION_LOCK_FILE);

    /* need a cleaning now? */
    if (abs(ntw_now - last_time) > SESSION_CHECK_INTERVAL)
    {
        fd = nt_open_lock_file_nb(SESSION_LOCK_FILE, O_RDWR | O_CREAT);
        if (fd < 0)
        {
            /* another process has locked the file and are in action */
            return;
        }
        /* double check to insure we need a cleaning */
        last_time = nt_stat_mtime(SESSION_LOCK_FILE);
        if (abs(ntw_now - last_time) > SESSION_CHECK_INTERVAL)
        {
            g_debug("start cleaning session");
            nt_foreach_active_uinfo(check_session, NULL);
            nt_touch_file(SESSION_LOCK_FILE);
            g_debug("stop cleaning session");
        }
        nt_close_lock_file(fd);
    }
    return;
}
static CALLBACK_RETURN www_clean_session(CALLBACK_ID callback_id,
                                         void *data, void *userdata)
{
    static int count = 50;
    static time_t last_check_time = 0;

    count--;
    if (!count)
    {
        count = 50;
        ntw_now_update();
        if (abs(ntw_now - last_check_time) > 60)
        {
            last_check_time = ntw_now;
            clean_session();
        }
    }
    return CALLBACK_CONTINUE;
}
static CALLBACK_ID www_init_request_cid;
static CALLBACK_ID www_clean_session_cid;
static CALLBACK_RETURN ntwww_final(CALLBACK_ID callback_id, void *data,
                                   void *userdata)
{
    remove_callback(www_init_request_cid);
    remove_callback(www_clean_session_cid);
    nt_final();
    return CALLBACK_CONTINUE;
}
static EVENT_ID ntw_enter_event;
static EVENT_ID ntw_exit_event;
gboolean ntwww_init(EVENT_ID unload_event, EVENT_ID enter_event,
                    EVENT_ID exit_event)
{
    if (!nt_init())
    {
        return FALSE;
    }

    www_init_request_cid =
        register_callback(enter_event, www_init_request, NULL);
    www_clean_session_cid =
        register_callback(event_new_request, www_clean_session, NULL);
    register_callback(unload_event, ntwww_final, NULL);
    ntw_enter_event = enter_event;
    ntw_exit_event = exit_event;
    return TRUE;
}

gboolean ntw_logout_user(struct user_info * puinfo)
{
    char buf[128];

    snprintf(buf, sizeof(buf), "user %s@%s logout from www",
             puinfo->userid, puinfo->from);
    if (nt_logout_user(puinfo))
    {
        g_message("%s", buf);
        return TRUE;
    }
    return FALSE;
}

void ntw_now_update(void)
{
    ntw_now = time(NULL);
}

void ntw_normal_headers(void)
{
    cgiHeaderString("Cache-control: no-cache\r\n");
    cgiHeaderContentType("text/html; charset=" CHARSET);
    return;
}

void ntw_error_headers(void)
{
    cgiHeaderStatus(500, "INTERNAL ERROR");
    fwebio_printf("500 INTERNAL ERROR");
    return;
}
char *ntw_safe_clear_htmltag(char *buf)
{
    char *buf2 = mp_copy0(buf, strlen(buf));

    return nt_clear_htmltag(buf2);
}

char *ntw_safe_clear_ansi(char *buf)
{
    char *buf2 = mp_copy0(buf, strlen(buf));

    return nt_clear_ansi(buf2);
}

#define HTML_SPAN_START "<span>"
#define HTML_SPAN_END "</span>"
#define HTML_SPAN_RESET "</span><span>"
void ntw_html_start(void)
{
    mp_grow(HTML_SPAN_START, strlen(HTML_SPAN_START));
    return;
}

char *ntw_html_end(int *plen)
{
    mp_grow0(HTML_SPAN_END, strlen(HTML_SPAN_END));
    if (plen)
    {
        *plen = mp_grow_size() - 1;
    }
    return (char *) mp_grow_finish();
}

#define HTML_HTTP_STR1	"<img class=\"original\" src=\""
#define HTML_HTTP_STR2	"\"/>"
#define HTML_HTTP_STR3	"<img class=\"small\" src=\""
#define HTML_HTTP_STR4	"<object classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" class=\"original\"><param name=\"AllowScriptAccess\" value=\"never\"/><param name=\"movie\" value=\""
#define HTML_HTTP_STR5	"\"/></object>"
#define HTML_HTTP_STR6	"<object classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" class=\"small\"><param name=\"AllowScriptAccess\" value=\"never\"/><param name=\"movie\" value=\""
#define HTML_HTTP_STR7	"<a target=\"_blank\" href=\""
#define HTML_HTTP_STR8	"\">"
#define HTML_HTTP_STR9	"</a>"

static inline int html_insert_http_url(char *strptr, int len,
                                       NTW_IMG_MODE mode)
{
    int urllen;

    urllen = 7;
    while (urllen < len)
    {
        char c = strptr[urllen];

        /*see RFC2397 for uri standard */
        if (c > '\x20' /*space */  && c < '{'
            && c != '\"' && c != '\\'
            && c != '<' && c != '>'
            && c != '[' && c != ']' && c != '^' && c != '`')
        {
            urllen++;
        }
        else
        {
            break;
        }
    }
    if (urllen < 8)
    {
        mp_grow(strptr, 7);
        return 7;
    }

    if (NTW_IMG_URL == mode)
    {
        goto INSERT_URL;
    }
    else if (!(strncasecmp(".jpg", &(strptr[urllen - 4]), 4)
               && strncasecmp(".gif", &(strptr[urllen - 4]), 4)
               && strncasecmp(".png", &(strptr[urllen - 4]), 4)
               && strncasecmp(".jpeg", &(strptr[urllen - 5]), 5)
               && strncasecmp(".bmp", &(strptr[urllen - 4]), 4)))
    {
        switch (mode)
        {
        case NTW_IMG_ORIGINAL:
            mp_grow(HTML_HTTP_STR1, strlen(HTML_HTTP_STR1));
            mp_grow(strptr, urllen);
            mp_grow(HTML_HTTP_STR2, strlen(HTML_HTTP_STR2));
            break;
        case NTW_IMG_SMALL:
            mp_grow(HTML_HTTP_STR3, strlen(HTML_HTTP_STR3));
            mp_grow(strptr, urllen);
            mp_grow(HTML_HTTP_STR2, strlen(HTML_HTTP_STR2));
            break;
        case NTW_IMG_URL:
            goto INSERT_URL;
        default:
            return 0;
        }
    }
    else if (!strncasecmp(".swf", &(strptr[urllen - 4]), 4))
    {
        switch (mode)
        {
        case NTW_IMG_ORIGINAL:
            mp_grow(HTML_HTTP_STR4, strlen(HTML_HTTP_STR4));
            mp_grow(strptr, urllen);
            mp_grow(HTML_HTTP_STR5, strlen(HTML_HTTP_STR5));
            break;
        case NTW_IMG_SMALL:
            mp_grow(HTML_HTTP_STR6, strlen(HTML_HTTP_STR6));
            mp_grow(strptr, urllen);
            mp_grow(HTML_HTTP_STR5, strlen(HTML_HTTP_STR5));
            break;
        case NTW_IMG_URL:
            goto INSERT_URL;
        default:
            return 0;
        }
    }
    else
    {
      INSERT_URL:
        mp_grow(HTML_HTTP_STR7, strlen(HTML_HTTP_STR7));
        mp_grow(strptr, urllen);
        mp_grow(HTML_HTTP_STR8, strlen(HTML_HTTP_STR8));
        mp_grow(strptr, urllen);
        mp_grow(HTML_HTTP_STR9, strlen(HTML_HTTP_STR9));
    }
    return urllen;
}

static inline int html_insert_ftp_url(char *strptr, int len,
                                      NTW_IMG_MODE mode)
{
    int urllen;

    urllen = 6;
    while (urllen < len)
    {
        char c = strptr[urllen];

        /*see RFC2397 for uri standard */
        if (c > '\x20' /*space */  && c < '{'
            && c != '\"' && c != '\\'
            && c != '<' && c != '>'
            && c != '[' && c != ']' && c != '^' && c != '`')
        {
            urllen++;
        }
        else
        {
            break;
        }
    }
    if (urllen < 7)
    {
        mp_grow(strptr, 6);
        return 6;
    }
    mp_grow(HTML_HTTP_STR7, strlen(HTML_HTTP_STR7));
    mp_grow(strptr, urllen);
    mp_grow(HTML_HTTP_STR8, strlen(HTML_HTTP_STR8));
    mp_grow(strptr, urllen);
    mp_grow(HTML_HTTP_STR9, strlen(HTML_HTTP_STR9));
    return urllen;
}

static inline int html_insert_ansi(char *strptr, int len,
                                   gboolean output_color)
{
        /**
	 * ANSI code is a sequence of 
	 * CSI number [;number]* command_char 
	 * CSI is "\x1b[" 
     * the command_char of color code is 'm' 
	 */
    int ansilen;
    gboolean ansi_color;
    long int color, tmpcolor;
    char *endptr, *ptr;

    if (strptr[2] == 'm')
    {
        /* 'CSI m' means reset all color */
        if (output_color)
            mp_grow(HTML_SPAN_RESET, strlen(HTML_SPAN_RESET));
        return 3;
    }
    else if (!isdigit(strptr[2]))
    {
        return 3;
    }
    ansilen = 3;
    ansi_color = FALSE;
    while (ansilen < len)
    {
        char c = strptr[ansilen++];

        if (isdigit(c) || c == ';')
        {
            continue;
        }
        else if (c != 'm')
        {
            return ansilen;
        }
        else
        {
            ansi_color = TRUE;
            break;
        }
    }
    if (!ansi_color)
    {
        /*should not be here, it means there is no command_char in ANSI code,
           so eat CSI  and return */
        return 2;
    }
    if (!output_color)
    {
        return ansilen;
    }
    ptr = strptr + 1;
    color = -1;
    while (*ptr != 'm')
    {
        ptr++;
        tmpcolor = strtol(ptr, &endptr, 10);
        if (ptr == endptr)
        {
            /*wrong color command, ignore it */
            return ansilen;
        }
        if (tmpcolor == 0 || (tmpcolor >= 30 && tmpcolor <= 37))
        {
            /*only intersting in 0, 30-37 color command */
            color = tmpcolor;
        }
        ptr = endptr;
    }
    if (color == 0)
    {
        mp_grow(HTML_SPAN_RESET, strlen(HTML_SPAN_RESET));
    }
    else if (color == -1)
    {
        return ansilen;
    }
    else
    {
        char buf[128];
        int l;

        l = sprintf(buf, "</span><span class=\"c%ld\">", color);
        mp_grow(buf, l);
    }
    return ansilen;

}

static inline int html_insert_normal(char *ptr, int len)
{
    char tmpc = *ptr;
    int normallen = 0;

    if (tmpc > '\x80' && tmpc < '\xFF' && len > 1)
        /** 
	 *  GBK char.
	 *  in GBK, 1st char range 0x81-0xFE, 2nd char range 0x40-0xFE
	 */
    {
        normallen += 2;
        len -= 2;
    }
    else if (isprint(tmpc))
        /*normal ascii char */
    {
        normallen++;
        len--;
    }
    else
        /* throw tmpc away, it looks like a garbage char */
    {
        return 1;
    }

    while (len)
    {
        tmpc = ptr[normallen];
        switch (tmpc)
        {
        case 'h':
        case 'H':
        case 'f':
        case 'F':
        case '\x1B':
        case '\x20':
        case '&':
        case '<':
        case '>':
        case '\n':
        case '\t':
            mp_grow(ptr, normallen);
            return normallen;
        default:
            if (tmpc > '\x80' && tmpc < '\xFF' && len > 1)
                        /** 
			 *  GBK char.
			 *  in GBK, 1st char range 0x81-0xFE, 2nd char range 0x40-0xFE
			 */
            {
                normallen += 2;
                len -= 2;
            }
            else if (isprint(tmpc))
                /*normal ascii char */
            {
                normallen++;
                len--;
            }
            else
                /* throw tmpc away, it looks like a garbage char */
            {
                mp_grow(ptr, normallen);
                return ++normallen;
            }

        }                       /*end switch(tmpc) */
    }                           /*end while(len) */
    mp_grow(ptr, normallen);
    return normallen;
}

#define HTML_SPAN_QUOTE "</span><span class=\"quote\">"
#define HTML_SPAN_QMD "</span><div class=\"qmd\"><span>"
#define HTML_SPAN_QMD_RESET "</span></div><span>"

void ntw_html_str(char *ptr, int len, NTW_IMG_MODE mode,
                  NTW_COLOR_MODE cmode)
{
    register int tmplen;
    register char tmpc;

    if (len < 1)
    {
        return;
    }
    switch (cmode)
    {
    case NTW_COLOR_IGNORE_ANSI:
        mp_grow(HTML_SPAN_RESET, strlen(HTML_SPAN_RESET));
        break;
    case NTW_COLOR_QUOTE:
        mp_grow(HTML_SPAN_QUOTE, strlen(HTML_SPAN_QUOTE));
        break;
    case NTW_COLOR_QMD:
        mp_grow(HTML_SPAN_QMD, strlen(HTML_SPAN_QMD));
        break;
    default:
        break;
    }
    while (len)
    {
        tmplen = 1;
        tmpc = *ptr;
        switch (tmpc)
        {
        case 'h':
        case 'H':
            if (mode != NTW_IMG_TEXT && len > 7
                && (ptr[1] == 't' || ptr[1] == 'T')
                && (ptr[2] == 't' || ptr[2] == 'T')
                && (ptr[3] == 'p' || ptr[3] == 'P')
                && ptr[4] == ':' && ptr[5] == '/' && ptr[6] == '/'
                /* like && !strncasecmp("http://",ptr,7) */ )
            {
                tmplen = html_insert_http_url(ptr, len, mode);
            }
            else
            {
                tmplen = html_insert_normal(ptr, len);
            }
            break;

        case 'f':
        case 'F':
            if (mode != NTW_IMG_TEXT && len > 6
                && (ptr[1] == 't' || ptr[1] == 'T')
                && (ptr[2] == 'p' || ptr[2] == 'P')
                && ptr[3] == ':' && ptr[4] == '/' && ptr[5] == '/'
                /* like && !strncasecmp("ftp://",ptr,6) */ )
            {
                tmplen = html_insert_ftp_url(ptr, len, mode);
            }
            else
            {
                tmplen = html_insert_normal(ptr, len);
            }
            break;

        case '\x1B' /*ESCAPE char */ :
            if (len > 2 && ptr[1] == '[')
            {
                tmplen =
                    html_insert_ansi(ptr, len, (cmode == NTW_COLOR_ANSI)
                                     || (cmode == NTW_COLOR_QMD));
            }
            break;

        case '\x20' /* SPACE char */ :
            mp_grow("&nbsp;", 6);
            break;

        case '\t':
            mp_grow("&nbsp;&nbsp;&nbsp;&nbsp;", 24);
            break;

        case '<':
            mp_grow("&lt;", 4);
            break;

        case '>':
            mp_grow("&gt;", 4);
            break;

        case '&':
            mp_grow("&amp;", 5);
            break;

        case '\n':
            mp_grow("<br/>\n", 6);
            break;

        default:
            tmplen = html_insert_normal(ptr, len);
        }                       /*end switch(tmpc) */
        ptr += tmplen;
        len -= tmplen;
    }                           /*end while(len) */
    if (cmode == NTW_COLOR_QUOTE)
    {
        mp_grow(HTML_SPAN_RESET, strlen(HTML_SPAN_RESET));
    }
    if (cmode == NTW_COLOR_QMD)
    {
        mp_grow(HTML_SPAN_QMD_RESET, strlen(HTML_SPAN_QMD_RESET));
    }
    return;
}

/* debug ntw_html_xxxx	
int test_html(void)
{
	int size,fd;
	char *ptr;
	ptr = malloc(100000);
	fd = open("test.txt",O_RDONLY);
	size = read(fd,ptr,100000);
	close(fd);
	ntw_html_start();
	ntw_html_str(ptr,size,NTW_IMG_ORIGINAL,NTW_COLOR_ANSI);
	ptr = ntw_html_end(&size);
	printf("%s",ptr);
	return 0;
}*/

unsigned int ntw_get_cached_online_num(void)
{
    static unsigned int num = 0;
    static time_t update_time = 0;

    if (ntw_now - update_time > 5)      /* update every 5 seconds */
    {
        num = nt_get_online_num();
        update_time = ntw_now;
    }
    return num;
}
static CALLBACK_RETURN ntw_tdict_final(CALLBACK_ID callback_id, void *data,
                                       void *userdata)
{
    Tdict *pdict = (Tdict *) userdata;

    tdict_final(*pdict);
    remove_callback(callback_id);
    return CALLBACK_CONTINUE;
}

Tdict ntw_default_tdict_init(const char *name)
/* the default action is add "header" include to header.tpl, and more in future */
{
    Tdict dict;

    dict = tdict_init(name);
    tdict_add_include(dict, "header", "header.tpl");
    register_callback(ntw_exit_event, ntw_tdict_final,
                      mp_copy(&dict, sizeof(dict)));
    return dict;
}
