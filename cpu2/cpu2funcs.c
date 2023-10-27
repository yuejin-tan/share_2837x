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

#include "can.h"

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

#pragma CODE_SECTION(CAN_readMessage2, "ramfuncs2");
uint16_t CAN_readMessage2(uint32_t base, uint32_t objID, uint16_t* msgData)
{
    uint16_t retSize;
    uint16_t msgCtrl = 0U;

    //
    // Set the Message Data A, Data B, and control values to be read
    // on request for data from the message object.
    //
    // Transfer the message object to the message object IF register.
    //
    HWREG_BP(base + CAN_O_IF2CMD) =
        ((uint32_t)CAN_IF2CMD_DATA_A | (uint32_t)CAN_IF2CMD_DATA_B |
            (uint32_t)CAN_IF2CMD_CONTROL | (objID & CAN_IF2CMD_MSG_NUM_M) |
            (uint32_t)CAN_IF2CMD_ARB);

    //
    // Wait for busy bit to clear
    //
    while ((HWREGH(base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY) == CAN_IF2CMD_BUSY)
    {
    }

    //
    // Read out the IF control Register.
    //
    msgCtrl = HWREGH(base + CAN_O_IF2MCTL);

    //
    // See if there is new data available.
    //
    if ((msgCtrl & CAN_IF2MCTL_NEWDAT) == CAN_IF2MCTL_NEWDAT)
    {
        //
        // Read out the data from the CAN registers.
        //
        CAN_readDataReg(msgData, (base + CAN_O_IF2DATA),
            ((uint32_t)msgCtrl & CAN_IF2MCTL_DLC_M));

        retSize = (msgCtrl & CAN_IF2MCTL_DLC_M);

        //
        // Now clear out the new data flag
        //
        HWREG_BP(base + CAN_O_IF2CMD) = ((uint32_t)CAN_IF2CMD_TXRQST |
            (objID & CAN_IF2CMD_MSG_NUM_M));

        //
        // Wait for busy bit to clear
        //
        while ((HWREGH(base + CAN_O_IF2CMD) & CAN_IF2CMD_BUSY) ==
            CAN_IF2CMD_BUSY)
        {
        }
    }
    else
    {
        retSize = 0;
    }

    return(retSize);
}

#pragma CODE_SECTION(CAN_sendMessage2, "ramfuncs2");
void CAN_sendMessage2(uint32_t base, uint32_t objID, uint16_t msgLen, const uint16_t* msgData)
{
    uint32_t msgCtrl = 0U;

    //
    // Set IF command to read message object control value
    //
    // Set up the request for data from the message object.
    // Transfer the message object to the IF register.
    //
    HWREG_BP(base + CAN_O_IF1CMD) = ((uint32_t)CAN_IF1CMD_CONTROL |
        (objID & CAN_IF1CMD_MSG_NUM_M));

    //
    // Wait for busy bit to clear
    //
    while ((HWREGH(base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY) == CAN_IF1CMD_BUSY)
    {
    }

    //
    // Read IF message control
    //
    msgCtrl = HWREGH(base + CAN_O_IF1MCTL);

    //
    // Write the data out to the CAN Data registers.
    //
    CAN_writeDataReg(msgData, (base + CAN_O_IF1DATA),
        (msgCtrl & CAN_IF1MCTL_DLC_M));

    //
    //  Set Data to be transferred from IF
    //
    if (msgLen > 0U)
    {
        msgCtrl = CAN_IF1CMD_DATA_B | CAN_IF1CMD_DATA_A;
    }
    else
    {
        msgCtrl = 0U;
    }

    //
    // Set Direction to write
    //
    // Set Tx Request Bit
    //
    // Transfer the message object to the message object specified by
    // objID.
    //
    HWREG_BP(base + CAN_O_IF1CMD) = (msgCtrl | (uint32_t)CAN_IF1CMD_DIR |
        (uint32_t)CAN_IF1CMD_TXRQST |
        (objID & CAN_IF1CMD_MSG_NUM_M));
}

#pragma CODE_SECTION(IPC_RamW16_Req_CPU2, "ramfuncs2");
void IPC_RamW16_Req_CPU2(uint32_t addr, uint16_t val)
{
    // 使用IPC告诉CPU1进行修改，死等回复
    volatile struct IPC_REGS_CPU2* IpcRegCpu2 = (void*)&IpcRegs;
    IpcRegCpu2->IPCSENDCOM = 1;
    IpcRegCpu2->IPCSENDADDR = addr;
    IpcRegCpu2->IPCSENDDATA = val;

    IpcRegCpu2->IPCSET.bit.IPC0 = 1;
    DELAY_US2(1);
    while (IpcRegCpu2->IPCFLG.bit.IPC0)
    {
        // wait
    }

    IpcRegCpu2->IPCSENDCOM = 0;
}

#pragma CODE_SECTION(IPC_RamW32_Req_CPU2, "ramfuncs2");
void IPC_RamW32_Req_CPU2(uint32_t addr, uint32_t val)
{
    // 使用IPC告诉CPU1进行修改，死等回复
    volatile struct IPC_REGS_CPU2* IpcRegCpu2 = (void*)&IpcRegs;
    IpcRegCpu2->IPCSENDCOM = 2;
    IpcRegCpu2->IPCSENDADDR = addr;
    IpcRegCpu2->IPCSENDDATA = val;

    IpcRegCpu2->IPCSET.bit.IPC0 = 1;
    DELAY_US2(1);
    while (IpcRegCpu2->IPCFLG.bit.IPC0)
    {
        // wait
    }

    IpcRegCpu2->IPCSENDCOM = 0;
}
