#ifndef _H_MAIN

#define _H_MAIN

#include <setjmp.h>
#include <glib.h>
#include "dispatcher.h"

G_BEGIN_DECLS
#define FWEB_ID "fweb2"
/**
 * don't use fatalJump anymore!
 * it exists only for compatibility
 */
extern jmp_buf fatalJmp;

/**
 * Order of event (see dispatcher.h for what is event):
 *
 * [a new request (fweb.c)]
 *       |
 * send (event_new_request)
 *       |
 * [processing http header and body (cgic.c)]
 *       |
 * send (event_before_processing)
 *       |
 * [invoke page processing (module.c)]
 *       |
 * send (event_after_processing)
 *       |
 * [free cgic resourese (cgic.c)]
 *       |
 * send (event_finish_request)
 *       |
 * [if we should exist? no: go back to 'a new request' (fweb.c)]
 *       |
 * send (event_program_exiting)
 *       |
 * [main program exits]
 *
 * CAUTION: In general, page module should not use events above.
 *      See module.h for details if you want to capture events
 *      relating to page preocessor and module
 */

/**
 * this event will occur when the programe is preparing for exiting
 * register to it if you need some clean up
 * you can use it in main program or global module
 * don't use it in page module
 * the flag is set to EVENT_FLAG_REVERSE_ORDER|EVENT_FLAG_IGNORE_BREAK|EVENT_FLAG_IGNORE_BREAK.
 */
extern EVENT_ID event_program_exiting;

/**
 * this event will occur when a new request is coming up, but before
 * parsing the http header and body. cgiXXXXX function is unavailable.
 * no flag is set. callback failed will cause a fatal error and stop the program
 */
extern EVENT_ID event_new_request;

/**
 * this event will occur when a request is going to finish. cgiXXX function
 * is unavailable.
 * the flag is set to EVENT_FLAG_REVERSE_ORDER
 * callback failed will cause a fatal error and stop the program
 */
extern EVENT_ID event_finish_request;

/**
 * this event will occur just before going to enter a page module and call page processor
 * cgiXXX function is available now.
 * no flag is set.
 * callback failed will cause a fatal error and stop the program
 */
extern EVENT_ID event_before_processing;

/**
 * this event will occur just after leaving page module
 * cgiXXX function is available now.
 * flag is set to EVENT_FLAG_REVERSE_ORDER
 * callback  failed will cause a fatal error and stop the program
 */
extern EVENT_ID event_after_processing;

G_END_DECLS
#endif
