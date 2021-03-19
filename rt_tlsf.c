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

#if defined (RT_USING_USERHEAP) && defined (PKG_USING_TLSF)

static tlsf_t tlsf_ptr = 0;
static struct rt_semaphore heap_sem;

#ifdef RT_USING_HOOK
static void (*rt_malloc_hook)(void *ptr, rt_size_t size);
static void (*rt_free_hook)(void *ptr);

void rt_malloc_sethook(void (*hook)(void *ptr, rt_size_t size))
{
    rt_malloc_hook = hook;
}

void rt_free_sethook(void (*hook)(void *ptr))
{
    rt_free_hook = hook;
}
#endif

void rt_system_heap_init(void *begin_addr, void *end_addr)
{
    size_t size;
    if (begin_addr < end_addr)
    {
        size = (rt_uint32_t)end_addr - (rt_uint32_t)begin_addr;
    }
    else
    {
        return;
    }
    if (!tlsf_ptr)
    {
        tlsf_ptr = (tlsf_t)tlsf_create_with_pool(begin_addr, size);
    }

    rt_sem_init(&heap_sem, "heap", 1, RT_IPC_FLAG_FIFO);
}

void *rt_malloc(rt_size_t nbytes)
{
    void *ptr;

    if (tlsf_ptr)
    {
        rt_sem_take(&heap_sem, RT_WAITING_FOREVER);

        ptr = tlsf_malloc(tlsf_ptr, nbytes);
        RT_OBJECT_HOOK_CALL(rt_malloc_hook, ((void *)ptr, nbytes));

        rt_sem_release(&heap_sem);
    }
    return ptr;
}

void rt_free(void *ptr)
{
    if (tlsf_ptr)
    {
        rt_sem_take(&heap_sem, RT_WAITING_FOREVER);

        tlsf_free(tlsf_ptr, ptr);
        RT_OBJECT_HOOK_CALL(rt_free_hook, (ptr));

        rt_sem_release(&heap_sem);
    }
}

void *rt_realloc(void *ptr, rt_size_t nbytes)
{
    if (tlsf_ptr)
    {
        rt_sem_take(&heap_sem, RT_WAITING_FOREVER);

        tlsf_realloc(tlsf_ptr, ptr, nbytes);

        rt_sem_release(&heap_sem);
    }
    return ptr;
}

void *rt_calloc(rt_size_t count, rt_size_t size)
{
    void *ptr;
    rt_size_t total_size;

    total_size = count * size;
    ptr = rt_malloc(total_size);
    if (ptr != RT_NULL)
    {
        /* clean memory */
        rt_memset(ptr, 0, total_size);
    }

    return ptr;
}

static size_t used_mem = 0;
static size_t total_mem = 0;

static void mem_info(void *ptr, size_t size, int used, void *user)
{
    if (used)
    {
        used_mem += size;
    }
    total_mem += size;
}

void rt_memory_info(rt_uint32_t *total,
                    rt_uint32_t *used,
                    rt_uint32_t *max_used)
{
    used_mem = 0;
    total_mem = 0;

    tlsf_walk_pool(tlsf_get_pool(tlsf_ptr), mem_info, 0);

    *total = total_mem;
    *used = used_mem;
    *max_used = used_mem;
}

void list_mem(void)
{
    used_mem = 0;
    total_mem = 0;

    tlsf_walk_pool(tlsf_get_pool(tlsf_ptr), mem_info, 0);

    rt_kprintf("total memory: %d\n", total_mem);
    rt_kprintf("used memory : %d\n", used_mem);
}

#endif
