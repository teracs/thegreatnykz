#ifndef _H_NTWWW
#define _H_NTWWW

#include <sys/types.h>
#include <glib.h>
#include "dispatcher.h"
#include "fwebtpl.h"

#define COOKIE_UTMPKEY "utmpkey"
#define COOKIE_UTMPNUM "utmpnum"
#define COOKIE_UTMPUSERID "utmpuserid"

G_BEGIN_DECLS
/* init function for ntwww module */
extern gboolean ntwww_init(EVENT_ID unload_event, EVENT_ID enter_event,
                           EVENT_ID exit_event);
/* TRUE if user is not a guest */
extern gboolean ntw_haslogin;
extern gboolean ntw_notguest;
extern struct userec ntw_curuser;
extern struct user_info *ntw_puinfo;
extern time_t ntw_now;

extern gboolean ntw_logout_user(struct user_info *puinfo);
extern void ntw_now_update(void);
extern void ntw_normal_headers(void);
extern void ntw_error_headers(void);

/**
 * same as nt_clear_ansi, but use mempool to alloc memory instead of using input buf
 * 
 * @param buf    The input buf to be cleared
 * 
 * @return buf from mempool
 * @see nt_clear_ansi
 */
extern char *ntw_safe_clear_ansi(char *buf);

/**
 * same as nt_clear_htmltag, but use mempool to alloc memory
 * instead of using input buf
 * 
 * @param buf    the input buf
 * 
 * @return buf from mempool
 * @see nt_clear_htmltag
 */
extern char *ntw_safe_clear_htmltag(char *buf);

typedef enum
{
        /**
	 * output ansi color in html
	 */
    NTW_COLOR_ANSI,
        /**
	 * ignore all ansi color in html
	 */
    NTW_COLOR_IGNORE_ANSI,
        /**
	 * output html in quote mode and ignore ansi color
	 */
    NTW_COLOR_QUOTE,
        /**
	 * output html in qmd(signature) mode, include ansi color
	 */
    NTW_COLOR_QMD,
} NTW_COLOR_MODE;

typedef enum
{
        /**
	 * image url shows as  original size image in html
	 */
    NTW_IMG_ORIGINAL,
        /**
	 * image url shows as small size image in html
	 */
    NTW_IMG_SMALL,
        /**
	 * image url shows as  a link in html
	 */
    NTW_IMG_URL,
        /**
	 * don't parse image url and show the text
	 */
    NTW_IMG_TEXT,
} NTW_IMG_MODE;

/**
 * start encoding article to html
 * be careful, you can't start another encoding before call ntw_end_html
 * 
 * @see ntw_html_end
 * @see ntw_html_str
 */
extern void ntw_html_start(void);

/**
 * end encoding article and get the encoding result
 * 
 * @param plen   if not NULL, write the length of encoding 
 *               result to it, not including NULL byte in the
 *               end
 * 
 * @return the encoding result. the result is malloced from mempool and including and NULL byte in the end
 * @see ntw_html_end
 * @see ntw_html_str
 */
extern char *ntw_html_end(int *plen);

/**
 * encoding string 'ptr' to html
 * 
 * @param ptr    the string to be encoded
 * @param len    the length of string
 * @param mode   how will url links be handled in encoding
 * @param cmode  how will the ansi code be handled
 * 
 * @see ntw_html_start
 * @see ntw_html_end
 */
extern void ntw_html_str(char *ptr, int len, NTW_IMG_MODE mode,
                         NTW_COLOR_MODE cmode);

/**
 * get num of online user, the value is refreshed every 5 seconds
 *
 * @return the num
 */
extern unsigned int ntw_get_cached_online_num(void);

/**
 * get a new Tdict and set some default action, eg add header/footer
 *
 * @param name the Tdict name
 *
 * @return the new Tdict, don't final it with tdict_final, it will be finalized automatically
 *
 * @see tdict_init
 * @see tdict_final
 */
extern Tdict ntw_default_tdict_init(const char *name);

G_END_DECLS
#endif                          /* ifndef _H_WWW */
