#include <sys/time.h>
#include <glib.h>
#include <string.h>
#include "dispatcher.h"

static void free_event(EVENT * event)
{
    if (event->active)
        return;
    if (event->callbacks->len != 0)
        return;
    g_ptr_array_free(event->callbacks, TRUE);
    g_free(event->name);
    g_free(event);
}

EVENT *create_event(const char *name, EVENT_FLAG flag)
{
    EVENT *event;

#ifdef DEBUG_DISPATCHER
    g_debug("create event: %s", name);
#endif
    event = g_malloc(sizeof(EVENT));
    event->callbacks = g_ptr_array_new();
    event->name = g_strdup(name);
    event->flag = flag;
    event->active = TRUE;
    return event;
}

gboolean send_event(EVENT * event, void *data)
{
    EVENT_CALLBACK **ptr;
    gint len, index, start, end, off;
    gboolean ret;

    g_debug("send event %s", event->name);
    if (!event->active)
    {
        g_warning("send disabled event %s", event->name);
        return FALSE;
    }
    if (event->callbacks->len == 0)
        return TRUE;

    len = (gint) event->callbacks->len;
    ptr = g_malloc(len * sizeof(EVENT_CALLBACK *));
    memcpy(ptr, event->callbacks->pdata, len * sizeof(EVENT_CALLBACK *));
    if (event->flag & EVENT_FLAG_REVERSE_ORDER)
    {
        start = len - 1;
        end = -1;
        off = -1;
    }
    else
    {
        start = 0;
        end = len;
        off = 1;
    }
    ret = TRUE;
    for (index = start; index != end; index += off)
    {
        EVENT_CALLBACK *callback;
        CALLBACK_RETURN cbret;
        gboolean brk = FALSE;

        callback = ptr[index];
        cbret = callback->func(callback, data, callback->user_data);
        switch (cbret)
        {
        case CALLBACK_FAILED:
            if (!(event->flag & EVENT_FLAG_IGNORE_FAILED))
            {
                ret = FALSE;
                brk = TRUE;
            }
            break;
        case CALLBACK_BREAK:
            if (!(event->flag & EVENT_FLAG_IGNORE_BREAK))
            {
                brk = TRUE;
            }
            break;
        case CALLBACK_CONTINUE:
            break;
        default:
            g_warning
                ("callback return unknown CALLBACK_RETURN in event %s",
                 event->name);
            ret = FALSE;
            brk = TRUE;
            break;
        }
        if (brk)
            break;
    }
    g_free(ptr);
    return ret;
}

void destroy_event(EVENT * event, gboolean free_callback)
{
    g_debug("destroy event: %s", event->name);
    if (free_callback)
    {
        gint len, index;

        len = (gint) event->callbacks->len;
        for (index = len - 1; index > -1; index--)
        {
            remove_callback(g_ptr_array_index(event->callbacks, index));
        }
    }
    event->active = FALSE;
    free_event(event);
}

EVENT_CALLBACK *register_callback(EVENT * event, CALLBACK_F callback_f,
                                  void *user_data)
{
    EVENT_CALLBACK *callback;

#ifdef DEBUG_DISPATCHER
    g_debug("register a callback for event %s", event->name);
#endif
    if (!event->active)
    {
        g_warning("event %s is disabled, but wanna register an callback",
                  event->name);
        return NULL;
    }
    callback = g_malloc(sizeof(EVENT_CALLBACK));
    callback->user_data = user_data;
    callback->func = callback_f;
    callback->event = event;
    g_ptr_array_add(event->callbacks, callback);
    return callback;
}

gboolean remove_callback(EVENT_CALLBACK * callback)
{
    EVENT *event = callback->event;

    g_debug("remove a callback for event %s", event->name);
    if (g_ptr_array_remove(event->callbacks, callback))
    {
        free_event(event);
        g_free(callback);
        return TRUE;
    }
    else
    {
        g_warning
            ("callback is not registered in event %s, but call remove_callback",
             event->name);
        return FALSE;
    }
}
