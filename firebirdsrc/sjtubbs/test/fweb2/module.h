#ifndef _H_MODULE

#define _H_MODULE
#include <setjmp.h>
#include <glib.h>

#include "dispatcher.h"         /*for EVENT_ID */

/**
 * fweb dynamicly loads two type modules: global module and page module
 *
 * global module:
 *		Global module is used to extend fweb: connect to database
 *		with a long live connection, or session handler, any thing you like.
 *		The exported name in global module will be exported to fweb global symbol 
 *		table too. Be careful to select the names to avoid name conflict.
 *		Global module name is defined in config file [fweb]->global_module
 *		(splited by semicolon; no 'lib' prefix and '.so' suffix).
 *		The module will be loaded in order and module initialize function
 *		(module name)_init will be called with appropriate parameters.
 * 
 * page module:
 *		Page module is used to provide page processor to fweb.
 *		The exported name in page module will NOT be exported to fweb global
 *		symbol table. The relation between URL and page processor function
 *		is defined in config file group [pages]. For example
 *		[pages]
 *		/test=abc:page_test
 *		The config above will define page processor of URL /test as 
 *		function page_test in page module abc. Request accessing URL
 *		/test will cause page_test to be called.
 *		Page module name is defined in config file [fweb]->page_module
 *		(splited by semicolon; no 'lib' prefix and '.so' suffix).
 *		The module will be loaded in order and module initialize function
 *		(module name)_init will be called with appropriate parameters.
 *
 * see prototype LIB_INIT_F and PAGE_PROCESS_F for initialize and page 
 * processor function details
 */

G_BEGIN_DECLS
/**
 * LIBrary INITialize Function prototype
 * 
 * This function will be called just after loading this module.
 * The function name should be (module name)_init. For example,
 * the module 'abc' must contain a exported function abc_init.
 *
 * @param unload_event the event will be sent just before the module to be unloaded,
 *    you can use it for cleaning up
 * @param enter_event the event will be sent just before calling page processor in this 
 *    page module. If this is a global module, it will be set to NULL
 * @param exit_event as enter_event, but be sent just after calling page processor.
 *    If this is a global module, it will be set to NULL
 * 
 * @return TRUE on success, FALSE on error
 */
typedef gboolean(*LIB_INIT_F) (EVENT_ID unload_event, EVENT_ID enter_event,
                               EVENT_ID exit_event);

/**
 * PAGE PROCESSor Function prototype
 *
 * This function will be called when invoke_page_processor find the page processor
 * associated with 'url'. The relation of processor and url is defined in config 
 * file group [pages]
 *
 * @return 0 if success, others on error
 */
typedef int (*PAGE_PROCESS_F) (void);

/**
 * for compatibility only, don't use it any more
 */
extern jmp_buf QUIT_JMP;

/**
 * initialize module subsystem
 *
 * @return TRUE on success, FALSE on error
 */
extern gboolean module_init(void);

/**
 * invoke page processor associated with url
 * you can associate processor and url in config file 
 * in group [pages]
 *
 * @param the url
 *
 * @return if the page processor is found and execute successfully, return TRUE
 */
extern gboolean invoke_page_processor(const char *url);

G_END_DECLS
#endif
/* end _H_MODULE */
