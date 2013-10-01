#ifndef _H_NTC_UTILS
#define _H_NTC_UTILS

#include <sys/types.h>

G_BEGIN_DECLS
/**
 * sleep implemented by select()
 * 
 * @param sec    how many secends to sleep
 */
extern void nt_sleep(int sec);

/**
 * open a file, and get a exclusive lock
 * 
 * @param path   path to the file
 * @param flags  flags passed to open()
 * 
 * @return the file descriptor if successfully. -1 if failed
 * @see nt_open_lock_file_nb
 */
extern int nt_open_lock_file(const char *path, int flags);

/**
 * As nt_open_lock_file, but doesn't block on geting lock, just give a try
 * 
 * @param path   path to the file
 * @param flags  flags passed to open()
 * 
 * @return the file descriptor if successfully. -1 if failed
 * @see nt_open_lock_file
 */
extern int nt_open_lock_file_nb(const char *path, int flags);

/**
 * close the file and release the lock
 * 
 * @param fd     the file descriptor
 * 
 * @see nt_open_lock_file
 * @see nt_open_lock_file_nb
 */
extern void nt_close_lock_file(int fd);

/**
 * append string in 'buf' to file 'file'
 * 
 * @param file   path to the file
 * @param buf    the string written to the file
 * 
 * @return TRUE if successfully
 */
extern gboolean nt_file_append(char *file, char *buf);

/**
 * get the mtime of file
 * 
 * @param path   path to file
 * 
 * @return the mtime if successfully, 0 if failed
 */
extern time_t nt_stat_mtime(const char *path);

/**
 * get the size of the file
 *
 * @param path path to file
 *
 * @return the size in bytes if successfully, 0 if no such path, or path is not a regular file
 */
extern off_t nt_stat_size(const char *path);

/**
 * update mtime of file to current time, if the file doesn't 
 * exist, create it 
 * 
 * @param path   path to file
 * 
 * @return TRUE if successfully
 */
extern gboolean nt_touch_file(const char *path);

/**
 * Check if 'file' has 'word' in its content. File must be one word per line, max 255 char per line
 * 
 * @param file   the file path
 * @param word   the word to be checked
 * 
 * @return TRUE if has such word, FALSE if can't open file or no such word
 */
extern gboolean nt_file_has_word(const char *file, const char *word);

/**
 * Clear ANSI color control char in the buf. Because the result is always less than input 'buf', the result is written back to buf. Be careful
 * 
 * @param buf    The input buf to be cleared 
 *  
 * @return the input buf 
 */
extern char *nt_clear_ansi(char *buf);

/**
 * Clear(not escape!) html tag from input buf. the result is written back to buf. be carebuf
 * 
 * @param buf    the input buf
 * 
 * @return the input buf
 */
extern char *nt_clear_htmltag(char *buf);

/**
 * convert time 't' to string of GMT time format
 *
 * @param t		time to used
 *
 * @return	string of GMT time, it will be invalid at the next call
 */
extern char *nt_gtime(time_t t);

/**
 * convert time 't' to string of local time
 *
 * @param t		time to used
 * 
 * @return string of local time, it will be invalid at the next call
 */
extern char *nt_ctime(time_t t);

/**
 * convert time 't' to gb2312 chinese string of local time
 *
 * @param t		time to used
 * 
 * @return string of local time, it will be invalid at the next call
 */
extern char *nt_ctime_cn(time_t t);

/**
 * copy from Kernel.c by Ellias, July 9th, 2011
 */
extern int nt_ci_strcmp(register char *s1, register char *s2);

extern char *nt_getfolderstr(char *filename);

extern int nt_strsncpy(char *s1, char *s2, int n);

extern int nt_sig_append(FILE * fp, char *id, int sig);

extern int nt_fprintf2(FILE * fp, char *s);

extern int nt_file_exist(char *file);

extern int nt_file_time(char *file);

extern struct stat *nt_f_stat(char *file);

extern int nt_char_in_url(char c);

G_END_DECLS
#endif
