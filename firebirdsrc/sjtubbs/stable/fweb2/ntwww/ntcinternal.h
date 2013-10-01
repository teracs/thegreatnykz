/**
 * This header file contains nt library internal functions which are exported
 * to nt library only. Don't use it outside nt library.
 */

#ifndef _H_NTCINTERNAL
#define _H_NTCINTERNAL

#include <glib.h>
#include "bbs.h"
#include "shm.h"

G_BEGIN_DECLS
/*============= start functions in ntc_user.c ================*/
extern gboolean ntcuser_init(void);
extern void ntcuser_reset(void);
extern void ntcuser_final(void);

/*------------- end functions in ntc_user.c ------------------*/

/*============= start functions in ntc_board.c ================*/

extern gboolean ntcboard_init(void);
extern void ntcboard_reset(void);
extern void ntcboard_final(void);

/*------------- end functions in ntc_board.c ------------------*/
G_END_DECLS
#endif                          /* _H_NTCINTERNAL */
