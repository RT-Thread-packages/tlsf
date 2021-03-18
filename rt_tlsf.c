/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include "rt_tlsf.h"

#define REGISTER register

static tlsf_t base_heap = 0;
static rt_mutex_t   tlsflock;

#define RT_MEM_CTOR(pheap, mem, size)   \
        do {    \
            TLSF_LOCK_INIT();   \
            (pheap) = (tlsf_t)tlsf_create_with_pool(mem, size);  \
        } while (0)

/*
 * destory memory management
 */
#define VP_MEM_DTOR(pheap)  \
        do {    \
            tlsf_destroy((tlsf_t)(pheap));   \
            TLSF_LOCK_DEINIT(); \
        } while (0)

/*
 * add memory to management
 */
#if CFG_VP_TLSF_LOCK_TYPE == 0
#define VP_MEM_ADD(pheap, mem, size) tlsf_add_pool((tlsf_t)(pheap), mem, size)
#else
#define VP_MEM_ADD(pheap, mem, size) \
        do {    \
            TLSF_LOCK();    \
            tlsf_add_pool((tlsf_t)(pheap), mem, size);  \
            TLSF_UNLOCK();  \
        } while (0)
#endif /* CFG_VP_TLSF_LOCK_TYPE */

/*
 * allocate memory
 */
static void *RT_MEM_MALLOC(tlsf_t  pheap, size_t nbytes)
{
    REGISTER void *ret;

    TLSF_LOCK();
    ret = tlsf_malloc((tlsf_t)(pheap), nbytes);
    TLSF_UNLOCK();

    return (ret);
}

/*
 * allocate memory align
 */
void *RT_MEM_MALLOC_ALIGN(tlsf_t pheap, size_t nbytes, size_t align)
{
    REGISTER void *ret;

    TLSF_LOCK();
    ret = tlsf_memalign((tlsf_t)(pheap), align, nbytes);
    TLSF_UNLOCK();

    return (ret);
}

/*
 * re-allocate memory
 */
void *RT_MEM_REALLOC(tlsf_t pheap, void *ptr, size_t new_size)
{
    REGISTER void *ret;

    TLSF_LOCK();
    ret = tlsf_realloc((tlsf_t)(pheap), ptr, new_size);
    TLSF_UNLOCK();

    return (ret);
}

/*
 * free memory
 */
#define RT_MEM_FREE(pheap, ptr)   \
        do {    \
            TLSF_LOCK();  \
            tlsf_free((tlsf_t)(pheap), ptr);  \
            TLSF_UNLOCK();  \
        } while (0)


void rt_app_heap_init(void *begin_addr, uint32_t size)
{
    if (!base_heap)
    {
        RT_MEM_CTOR(base_heap, begin_addr, size);
    }
}

void *malloc(size_t nbytes)
{
    return RT_MEM_MALLOC(base_heap, nbytes);
}
void *realloc(void *ptr, unsigned newsize)
{
    return RT_MEM_REALLOC(base_heap, ptr, newsize);
}
void *calloc(size_t count, size_t size)
{
    return malloc(count * size);
}

void free(void *ptr)
{
    RT_MEM_FREE(base_heap, ptr);
    return;
}
