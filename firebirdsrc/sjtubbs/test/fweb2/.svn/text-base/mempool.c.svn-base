#include <stdlib.h>
#include <string.h>
#include "mempool.h"
#include "dispatcher.h"
#include "fweb.h"

/* define obstack memory allocating function */

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#define SMALL_CHUNK_SIZE (32*1024)
#define LARGE_CHUNK_SIZE (128*1024)
#define GROW_1ST_CHUNK_SIZE (64*1024)
#define LARGE_SIZE (10*1024)

/* use for normal malloc */
struct _FWEB_Mempool __req_mempool;

/* use for growing malloc */
struct _FWEB_Mempool __req_mempool2;

static CALLBACK_RETURN mp_final(CALLBACK_ID callback_id, void *data,
                                void *userdata)
{
    obstack_free(&(__req_mempool.obs), NULL);
    obstack_free(&(__req_mempool.large_obs), NULL);
    memset(&__req_mempool, 0, sizeof(__req_mempool));
    obstack_free(&(__req_mempool2.obs), NULL);
    memset(&__req_mempool2, 0, sizeof(__req_mempool2));

    return CALLBACK_CONTINUE;
}

static CALLBACK_RETURN mp_request_init(CALLBACK_ID callback_id, void *data,
                                       void *userdata)
{
    __req_mempool.first_ptr =
        obstack_alloc(&(__req_mempool.obs), sizeof(int));
    __req_mempool.lfirst_ptr =
        obstack_alloc(&(__req_mempool.large_obs), sizeof(int));
    __req_mempool2.first_ptr =
        obstack_alloc(&(__req_mempool2.obs), sizeof(int));
    return CALLBACK_CONTINUE;
}

static CALLBACK_RETURN mp_request_final(CALLBACK_ID callback_id,
                                        void *data, void *userdata)
{
    obstack_free(&(__req_mempool.obs), __req_mempool.first_ptr);
    obstack_free(&(__req_mempool.large_obs), __req_mempool.lfirst_ptr);
    obstack_free(&(__req_mempool2.obs), __req_mempool2.first_ptr);
    return CALLBACK_CONTINUE;
}
static void my_obstack_alloc_failed(void)
{
    g_error("allocat memory failed");
    exit(128);
}
gboolean mp_init(void)
{
    /* setup error handler */
    obstack_alloc_failed_handler = &my_obstack_alloc_failed;

    /* setup normal memory pool */
    obstack_begin(&(__req_mempool.obs), SMALL_CHUNK_SIZE);
    obstack_begin(&(__req_mempool.large_obs), 64);      /* don't actually allocate memory for large obstack now */
    /* change following chunks size */
    obstack_chunk_size(&(__req_mempool.large_obs)) = LARGE_CHUNK_SIZE;

    /* setup growing memory pool */
    obstack_begin(&(__req_mempool2.obs), GROW_1ST_CHUNK_SIZE);
    /* change following chunks size */
    obstack_chunk_size(&(__req_mempool2.obs)) = LARGE_CHUNK_SIZE;

    register_callback(event_program_exiting, mp_final, NULL);
    register_callback(event_new_request, mp_request_init, NULL);
    register_callback(event_finish_request, mp_request_final, NULL);
    return TRUE;
}

void *mp_alloc(size_t size)
{
    if (G_LIKELY(size < LARGE_SIZE))
    {
        return obstack_alloc(&(__req_mempool.obs), size);
    }
    else
    {
        return obstack_alloc(&(__req_mempool.large_obs), size);
    }
}

void *mp_copy(void *address, size_t size)
{
    if (G_LIKELY(size < LARGE_SIZE))
    {
        return obstack_copy(&(__req_mempool.obs), address, size);
    }
    else
    {
        return obstack_copy(&(__req_mempool.large_obs), address, size);
    }
}

void *mp_copy0(void *address, size_t size)
{
    if (G_LIKELY(size < LARGE_SIZE))
    {
        return obstack_copy0(&(__req_mempool.obs), address, size);
    }
    else
    {
        return obstack_copy0(&(__req_mempool.large_obs), address, size);
    }
}
