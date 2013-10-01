#ifndef _H_HTTP

#define _H_HTTP
#include <glib.h>

G_BEGIN_DECLS
/**
 * set default response code in http, 404 or 500
 */
extern int set_default_response_code(int num);

/**
 * output default response code header
 */
extern int output_default_header(void);

/**
 * check if the request method is GET or POST
 */
extern int check_method(void);

/**
 * initialize http subsystem
 */
extern gboolean fwebhttp_init(void);

G_END_DECLS
#endif
