/**
 * Following functions use GNU LIBC obstack to construct a memory pool
 * which provide efficient dynamic memory allocation and allocated-and-forgot
 * memory scheme. It aims to help programmer from ugly and buggy malloc-free scheme. 
 *
 * This memory pool will be vaild between every request starting and
 * ending. Don't use it when you need a valid memory ouside the times.
 *
 * There are two type of memory pool: normal and growing
 * The normal one is for allocating known size memory.
 * The growing one is for allocating unknown size memory, you
 * can grow it as need, and call mp_grow_finish() when you finish growing.
 * The drawback is you can't grow two object at the same time.
 *
 * function for normal one is:
 *	mp_alloc, mp_copy, mp_copy0
 * 
 * function for growing one is:
 *  mp_grow, mp_grow0, mp_growc, mp_grow_size, mp_grow_finish
 */

#ifndef _H_MEMPOOL
#define _H_MEMPOOL

#include <obstack.h>
#include <glib.h>

G_BEGIN_DECLS
/**
 * internal struct, don't access it outside mempool.c 
 * mempool struct
 */
    struct _FWEB_Mempool
{
    struct obstack obs;         /* obstack */
    struct obstack large_obs;   /* obstack for large memory alloced */
    void *first_ptr;            /* first alloced pointer in obs */
    void *lfirst_ptr;           /* first alloced pointer in large_obs */
};

/* initialize mempool, return TRUE for success */
extern gboolean mp_init(void);

/* export these variables for macro only, don't use it directly */
extern struct _FWEB_Mempool __req_mempool;      /* normal allocation memory pool */
extern struct _FWEB_Mempool __req_mempool2;     /* growing allocation memory pool */

/**
 * alloc size memory
 *
 * @param size the size to malloc
 *
 * @return the allocated memory, it will not be NULL 
 */
extern void *mp_alloc(size_t size);

/**
 * alloc size bytes, and copy from address with size length to 
 * new alloc memory
 *
 * @param address the source address
 * @param size 'size' length byte to copy
 *
 * @return the allocated memory, it will not be NULL
 */
extern void *mp_copy(void *address, size_t size);

/**
 * use as mp_copy, but alloc size+1 bytes, and copy from
 * address with size length to new malloc memory, then
 * add a null char at the end
 *
 * @param address the source address
 * @param size 'size' length byte to copy
 *
 * @return the allocated memory, it will not be NULL
 */
extern void *mp_copy0(void *address, size_t size);

/**
 * following is macro for growing an memory area dynamicly
 * BE CAREFUL: 
 *    you can not grow two memory area SIMULTANEOUSLY
 * mp_grow is used as mp_copy, but no return value
 * mp_grow0 is used as mp_copy0, but no return value
 * mp_growc is add char c at the end, no return value
 * mp_grow_finish: finish growing and return the address
 * mp_grow_size: before finish a growing, call it return the 
 *               size of current growing memory area
 * mp_grow_room: how many space available in current growing
 * mp_growc_fast: add char c at the end, do not check if there 
 *				is enough space. use mp_grow_room to make sure
 *				there is enough space
 * mp_grow_cancel: cancel current growing
 */

/* prototype: extern void mp_grow(void *address, size_t size); */
#define mp_grow(address,size) \
	obstack_grow((&(__req_mempool2.obs)),(address),(size))

/* prototype: extern void mp_grow0(void *address, size_t size); */
#define mp_grow0(address,size) \
	obstack_grow0((&(__req_mempool2.obs)),(address),(size))

/* prototype: extern void mp_growc(const char c); */
#define mp_growc(c) \
	obstack_1grow((&(__req_mempool2.obs)),(c))

/* prototype: extern unsigned int mp_grow_size(void); */
#define mp_grow_size() \
	obstack_object_size((&(__req_mempool2.obs)))

/* prototype: extern void *mp_grow_finish(void); */
#define mp_grow_finish() \
	obstack_finish((&(__req_mempool2.obs)))

/* prototype: extern unsigned int mp_grow_room(void); */
#define mp_grow_room() \
	obstack_room((&(__req_mempool2.obs)))

/* prototype: extern void mp_growc_fast(const char c); */
#define mp_growc_fast(c) \
	obstack_1grow_fast((&(__req_mempool2.obs)),(c))

/* prototype: extern void mp_grow_cancel(void); */
#define mp_grow_cancel() \
	obstack_free((&(__req_mempool2.obs)),obstack_finish((&(__req_mempool2.obs))))

G_END_DECLS
#endif                          /* end _H_MEMPOOL */
