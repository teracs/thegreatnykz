#ifndef _H_NTCORE
#define _H_NTCORE

#include <glib.h>

#include "bbs.h"

/* include ntcore library headers */
#include "ntc_user.h"
#include "ntc_board.h"
#include "ntc_mail.h"
#include "ntc_utils.h"

G_BEGIN_DECLS
/*initialize and finalize library */
/**
 * Initialize the ntcore library
 * 
 * @return TRUE if successfully
 */
extern gboolean nt_init(void);

/**
 * Finalize the ntcore library
 */
extern void nt_final(void);

/**
 * Reset the state of ntcore library, e.g. used before every request of WWW
 */
extern void nt_reset(void);

/**
 * Increase the whole system login count from bbs being setuped
 * 
 * @return TRUE if successfully
 */
extern gboolean nt_inc_login_count(void);

/**
 * Check if the ip 'remoteaddr' is a bad host (not welcome to login)
 * 
 * @param remoteaddr the ip address
 * 
 * @return TRUE if it's a bad host
 */
extern gboolean nt_is_bad_host(const char *remoteaddr);

#define NT_RANDOM_SIGNATURE 0
#define NT_NO_SIGNATURE -1

extern gboolean nt_mail_post(char *userid, char *nickname, char *fromip,
                             int signature, char *toid, char *title,
                             char *content);

extern gboolean nt_board_post(char *userid, char *nickname, char *fromip,
                              int signature, char *board,
                              unsigned int reid, char *title,
                              char *content);

extern gboolean nt_board_replace(char *fromip, char *board,
                                 unsigned int id, char *title,
                                 char *content);

extern gboolean nt_board_del(char *board, unsigned int id);

extern gboolean nt_board_del_some(char *board, unsigned int *id_arr,
                                  unsigned int num, unsigned int *delnum);

extern gboolean nt_board_del_range(char *board, unsigned int start_id,
                                   unsigned int end_id,
                                   unsigned int *delnum);

extern gboolean nt_board_del_subject(char *board, unsigned int reid,
                                     unsigned int start_id,
                                     unsigned int end_id,
                                     unsigned int *delnum);

extern gboolean nt_board_undel(char *board, unsigned int id);

G_END_DECLS
#endif                          /* ifndef _H_NTCORE */
