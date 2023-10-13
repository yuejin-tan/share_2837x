/*
 * cpu2funcs.c
 *
 *  Created on: 2023年8月15日
 *      Author: t
 */

#include "F2837xD_Ipc_drivers.h"
#include "F28x_Project.h"

#include "cpu2funcs.h"
#include "topCfg.h"

#include "cdb.h"

#pragma CODE_SECTION(memcpy_tyj, "ramfuncs2");
void* memcpy_tyj(void* dst, const void* src, uint32_t size)
{
    void* ret = dst;

    while (size)
    {
        *(uint16_t*)dst = *(uint16_t*)src;
        dst = (uint16_t*)dst + 1;
        src = (uint16_t*)src + 1;
        size--;
    }

    return ret;
}

// time stamp
#pragma CODE_SECTION(getIpcTimeStamp, "ramfuncs2");
uint64_t getIpcTimeStamp()
{
    volatile uint64_t ret = 0;

    uint32_t* low = (void*)&ret;

    *low = IpcRegs.IPCCOUNTERL;
    *(low + 1) = IpcRegs.IPCCOUNTERH;

    return ret;
}
