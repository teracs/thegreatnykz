#ifndef	_H_LOG

#define _H_LOG

#include <glib.h>

/**
 * This file provides logging capabiltiy for all
 * Logging is use GLIB interface (the format is the same as printf):
 *
 * g_error(char *fmt, ...)  for unrecoverable error
 * g_warning(char *fmt, ...) for recoverable error
 * g_message(char *fmt, ...) for general message, accouting log
 * g_debug(char *fmt, ...) for debug log
 *
 * You should #include <glib.h> before using it. 
 */
G_BEGIN_DECLS
/**
 * initialize the log subsystem
 *
 * @return TRUE if successfully, FALSE if failed
 */
extern gboolean log_init(void);

G_END_DECLS
#endif
