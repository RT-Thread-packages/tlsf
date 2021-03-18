/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */


#ifndef __TLSF_MALLOC_H
#define __TLSF_MALLOC_H

#include "tlsf.h"
#include <stdio.h>

/*
 * tlsf lock
 */
#define CFG_VP_TLSF_LOCK_TYPE 1

#if CFG_VP_TLSF_LOCK_TYPE == 1
#include <rtdevice.h>

#define TLSF_LOCK_INIT()    do{tlsflock = rt_mutex_create("_Tlsflock", RT_IPC_FLAG_PRIO);}while(0)
#define TLSF_LOCK_DEINIT()  rt_mutex_delete(tlsflock)
#define TLSF_LOCK()         rt_mutex_take(tlsflock, -1)
#define TLSF_UNLOCK()       rt_mutex_release(tlsflock)

#endif /* CFG_VP_TLSF_LOCK_TYPE */
#endif
