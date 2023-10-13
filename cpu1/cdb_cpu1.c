/*
 * cdb_cpu1.c
 * 简单的基于can通信的内存查看的实现，几乎没有用到id区分
 *  Created on: 2023年4月16日
 *      Author: t
 */

#include "cdb_cpu1.h"
#include "F2837xD_Ipc_drivers.h"
#include "F28x_Project.h"

void cdb_ack_cpu1(void)
{
    if (IpcRegs.IPCSTS.bit.IPC0)
    {
        if (IpcRegs.IPCRECVCOM == 1)
        {
            // 16bit 修改
            uint32_t tarAddr = IpcRegs.IPCRECVADDR;
            uint16_t tarVal = IpcRegs.IPCRECVDATA;

            *((uint16_t*)(void*)tarAddr) = tarVal;
        }

        if (IpcRegs.IPCRECVCOM == 2)
        {
            // 32bit 修改
            uint32_t tarAddr = IpcRegs.IPCRECVADDR;
            uint32_t tarVal = IpcRegs.IPCRECVDATA;

            *((uint32_t*)(void*)tarAddr) = tarVal;
        }

        IpcRegs.IPCACK.bit.IPC0 = 1;
    }

}

// // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
// if (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_RXOK)) == CAN_ES_RXOK)
// if ((HWREG_BP(CANA_BASE + CAN_O_NDAT_21)) == 0x00000001ul)
// {
//     // Get the received message
//     CAN_readMessage(CANA_BASE, 3, rxMsgData);
// }

// tx poll
// while (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_TXOK)) != CAN_ES_TXOK)
