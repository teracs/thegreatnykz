#ifndef _H_NTC_BOARD
#define _H_NTC_BOARD

#include <stdio.h>
#include <ctype.h>

G_BEGIN_DECLS
    inline static int nt_setboardfile(char *buf, size_t buflen,
                                      const char *board,
                                      const char *filename)
{
    return snprintf(buf, buflen, "board/%s/%s", board, filename);
}

/**
 * get random tips from board
 *
 * @param board OUT: the board name, NULL-terminated string
 * @param boardlen how many space available in board, in bytes
 * @param tip OUT: the tip, NULL-terminated string
 * @param tiplen how many space available in board, in bytes, >=512 is appropriate
 *
 * @return TRUE if we get a tip, FALSE if we are not
 */
extern gboolean nt_board_get_tip(char *board, unsigned int boardlen,
                                 char *tip, unsigned int tiplen);

extern struct shortfile *nt_getbcache(char *board);

extern int
nt_post_article(char *board, char *title, char *file, char *id,
             char *nickname, char *ip, int sig, int autocr,
             unsigned int currid, unsigned int reid);

G_END_DECLS
#endif                          /* _H_NTC_BOARD */
