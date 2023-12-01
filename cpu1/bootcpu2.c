/*
 * bootcpu2.c
 *
 *  Created on: 2023年10月12日
 *      Author: t
 */
#include "stdint.h"
#include "F2837xD_Ipc_drivers.h"
#include "F28x_Project.h"

#include "main.h"

uint16_t tyjBootCPU2(void)
{
    uint32_t bootStatus;
    uint16_t returnStatus = STATUS_PASS;

    // If CPU2 has already booted, return a fail to let the application
    // know that something is out of the ordinary.
    bootStatus = IPCGetBootStatus() & 0x0000000F;

    if (bootStatus == C2_BOOTROM_BOOTSTS_C2TOC1_BOOT_CMD_ACK)
    {
        // Check if MSB is set as well
        bootStatus = ((uint32_t)(IPCGetBootStatus() & 0x80000000)) >> 31U;

        if (bootStatus != 0)
        {
            returnStatus = STATUS_FAIL;

            return returnStatus;
        }
    }

    // Wait until CPU02 control system boot ROM is ready to receive
    // CPU01 to CPU02 INT1 interrupts.
    do
    {
        bootStatus = IPCGetBootStatus() & C2_BOOTROM_BOOTSTS_SYSTEM_READY;
    } while ((bootStatus != C2_BOOTROM_BOOTSTS_SYSTEM_READY));

    // Loop until CPU02 control system IPC flags 1 and 32 are available
    while ((IPCLtoRFlagBusy(IPC_FLAG0) == 1) ||
        (IPCLtoRFlagBusy(IPC_FLAG31) == 1))
    {
        // wait
    }

    // CPU01 to CPU02 IPC Boot Mode Register
    IpcRegs.IPCBOOTMODE = C1C2_BROM_BOOTMODE_BOOT_FROM_RAM;

    // addr
    IpcRegs.IPCSENDADDR = 0x1BF00ul;

    // para
    IpcRegs.IPCSENDDATA = 0;

    // CPU01 To CPU02 IPC Command Register
    IpcRegs.IPCSENDCOM = IPC_BRANCH;

    // CPU01 to CPU02 IPC flag register
    IpcRegs.IPCSET.all = 0x80000001;

    return returnStatus;
}


void bootCpu2(void)
{
    extern uint16_t Ramfuncs2RunStart, Ramfuncs2LoadStart, Ramfuncs2LoadSize;
    extern uint16_t cpu2bootRunStart, cpu2bootLoadStart, cpu2bootLoadSize;

    // 先复位CPU2
    EALLOW;
    DevCfgRegs.CPU2RESCTL.all = 0xa5a50001ul;
    EDIS;
    EALLOW;
    DevCfgRegs.CPU2RESCTL.all = 0xa5a50000ul;
    EDIS;

    // CPU2 boot程序
    memcpy(&cpu2bootRunStart, &cpu2bootLoadStart, (size_t)&cpu2bootLoadSize);

    // CPU2 运行时程序
    memcpy(&Ramfuncs2RunStart, &Ramfuncs2LoadStart, (size_t)&Ramfuncs2LoadSize);

    // 复制一份CLA程序 先到GS0
    extern uint16_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    extern uint16_t Cla1ConstRunStart, Cla1ConstLoadStart, Cla1ConstLoadSize;

    memcpy((void*)0xc000ul, (uint32_t*)&Cla1funcsLoadStart,
        (uint32_t)&Cla1funcsLoadSize);
    memcpy((void*)0xc800ul, (uint32_t*)&Cla1ConstLoadStart,
        (uint32_t)&Cla1ConstLoadSize);

    bsp_init_cpu2dev();

    // 利用 Boot IPC 来启动CPU2
    tyjBootCPU2();

    return;
}

