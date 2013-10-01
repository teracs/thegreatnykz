#ifndef _H_DISPATCHER
#define _H_DISPATCHER
#include <glib.h>

/**
 * This file provides a simple event mechanism.
 * You can create an event (create_event), register a callback
 * to it (register_callback).
 * Then you can send event (it means call all callback
 * functions) as many times as you want (send_event).
 * Remove a callback from an event is also supported (remove_callback).
 * If you don't need the event any more, remember to destory
 * it (destroy_event).
 */

G_BEGIN_DECLS
/**
 * value that callback function can return
 *
 * @see EVENT_FLAG
 */
    typedef enum
{
        /**
	 * callback detects an unrecoverable error
	 * return CALLBACK_FAILED will cause event stopping 
	 * sent to remainder callbacks in the chain.
	 * this action may be changed, see EVENT_FLAG for more
	 */
    CALLBACK_FAILED,
        /**
	 * callback succeeds, and it know there is no need
	 * to call remainder callbacks in the chain
	 * this action may be changed, see EVENT_FLAG for more
	 */
    CALLBACK_BREAK,
        /**
	 * callback succeeds, call remainder callbacks as normal
	 */
    CALLBACK_CONTINUE,
} CALLBACK_RETURN;

/**
 * flag to be used to create a new event
 * it will affect some action in calling callbacks
 * you can use | to compose the flags
 *
 * @see CALLBACK_RETURN
 */
typedef enum
{
        /**
	 * Nothing special to set
	 */
    EVENT_FLAG_NONE = 0,
        /**
	 * callback registered to this event will
	 * be called in the reverse order of registering
	 */
    EVENT_FLAG_REVERSE_ORDER = 1 << 0,
        /**
	 * if this flags is set, CALLBACK_BREAK will be ignored
	 * the remainder callbacks in the chain will be called normally
	 */
    EVENT_FLAG_IGNORE_BREAK = 1 << 1,
        /**
	 * if this flags is set, CALLBACK_FAILED will be ignored
	 * the remainder callbacks in the chain will be called normally
	 */
    EVENT_FLAG_IGNORE_FAILED = 1 << 2,
} EVENT_FLAG;

struct _DIS_EVENT;
struct _DIS_EVENT_CALLBACK;

/**
 * EVENT_ID, export to user as event identification
 */
typedef struct _DIS_EVENT *EVENT_ID;

/**
 * CALLBACK_ID, export to user as callback identification
 */
typedef struct _DIS_EVENT_CALLBACK *CALLBACK_ID;

/**
 * the prototype of callback function.
 * @param callback_id the CALLBACK_ID that this callback function is related to
 * @param data the event data
 * @param user_data the user data
 *
 * @return CALLBACK_RETURN as callback need
 *
 * @see send_event
 * @see register_callback
 */
typedef CALLBACK_RETURN(*CALLBACK_F) (CALLBACK_ID callback_id, void *data,
                                      void *user_data);

/**
 * EVENT struct to stored infomation about event
 * don't define or access it outside dispatcher.c
 * use prototype EVENT_ID instead
 */
typedef struct _DIS_EVENT
{
    char *name;                 /* event name */
    GPtrArray *callbacks;       /* callbacks array */
    EVENT_FLAG flag;            /* event flag */
    gboolean active;            /* if this event is active: callback is being called */
} EVENT;

/**
 * EVENT_CALLBACK struct to stored infomation about callback
 * don't define or access it outside dispatcher.c
 * use prototype CALLBACK_ID instead
 */
typedef struct _DIS_EVENT_CALLBACK
{
    CALLBACK_F func;            /* callback function */
    void *user_data;            /* callback user data */
    EVENT *event;               /* the event which this callback relates to */
} EVENT_CALLBACK;

/**
 * create a new event.
 *
 * @param name the name of event, it should be unique
 * @param flag the flag of event
 *
 * @return new event
 *
 * @see EVENT_FLAG
 * @see destroy_event
 * @see send_event
 */
extern EVENT_ID create_event(const char *name, EVENT_FLAG flag);

/**
 * send an event, it means call the callback function registered
 * as event flag specified
 *
 * @param event_id the event to be sent
 * @param data the event data passed to callback function
 * 
 * @return if EVENT_FLAG_IGNORE_FAILED is not set and some callback
 *		return CALLBACK_FAILED, FALSE is returned.
 * 
 * @see CALLBACK_F
 * @see EVENT_FLAG
 * @see CALLBACK_RETURN
 */
extern gboolean send_event(EVENT_ID event_id, void *data);

/**
 * destroy event_id and free related resourcese
 *
 * @param event_id the event
 * @param free_callback if TRUE, destory callbacks related to this event_id too
 *
 * @see create_event
 */
extern void destroy_event(EVENT_ID event_id, gboolean free_callback);

/**
 * register callback function to event event_id
 *
 * @param event_id the event to be registered
 * @param callback the callback function
 * @param user_data the user data to be passed to callback function while being called
 *
 * @return the callback id of new registered callback function
 *
 * @see CALLBACK_F
 * @see remove_callback
 */
extern CALLBACK_ID register_callback(EVENT_ID event_id,
                                     CALLBACK_F callback, void *user_data);

/**
 * remove a callback from event and free related resources
 * you can saftely remove the callback in the callback function
 *
 * @param callback_id the callback to be removed
 *
 * @return FALSE if in error. 
 * 
 * @see register_callback
 */
extern gboolean remove_callback(CALLBACK_ID callback_id);

G_END_DECLS
#endif
/* end _H_DISPATCHER */
