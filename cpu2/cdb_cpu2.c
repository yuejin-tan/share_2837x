/*
 * cdb_cpu2.c
 *
 * 简单的基于can通信的内存查看的实现，几乎没有用到id区分
 * 拓展为CPU1+CPU2的形式
 *
 *  Created on: 2023年8月17日
 *      Author: t
 */

#include "cdb_cpu2.h"

#include "algo_code_config.h"

#include "F2837xD_Ipc_drivers.h"
#include "F28x_Project.h"
#include "can.h"

#include "cpu2funcs.h"
#include "topCfg.h"

const uint32_t buffSize = RAMBUFFSIZE;

#pragma CODE_SECTION(cdb_init, "ramfuncs2");
void cdb_init(struct cdb_struct* hcdb)
{
    hcdb->dumpStartAddr = 0;
    hcdb->dumpEndAddr = 0;
    hcdb->dumpNowAddr = 0;
    hcdb->cdbSta = CDB_stop;

    hcdb->trigLogSrcCnt = 0;
    hcdb->trigIdxKeep = 0;
    hcdb->trigTimeTar = 0;
    hcdb->trigTimeCnt = 0;
    hcdb->trigIdxStart = 0;
    hcdb->trigIdxNow = 0;
    hcdb->trigThd = 0;
    hcdb->trigVal = 0;
    hcdb->trigValLast = 0;
    hcdb->trigSrc = 0;
    hcdb->sampInter = 0;
    hcdb->sampInterCnt = 0;
    hcdb->trigMode = TRIG_edge_up;
    hcdb->trigSta = TSTA_stop;

    return;
}

// little endian
static inline uint32_t getUint32FromBuff(uint16_t* buffStart)
{
    return
        (((uint32_t)(buffStart[0] & 0xffu)) << 0) |
        (((uint32_t)(buffStart[1] & 0xffu)) << 8) |
        (((uint32_t)(buffStart[2] & 0xffu)) << 16) |
        (((uint32_t)(buffStart[3] & 0xffu)) << 24);
}

static inline uint16_t getUint16FromBuff(uint16_t* buffStart)
{
    return
        (((uint16_t)(buffStart[0] & 0xffu)) << 0) |
        (((uint16_t)(buffStart[1] & 0xffu)) << 8);
}

static inline float getFloatFromBuff(uint16_t* buffStart)
{
    union UI32_F32
    {
        uint32_t ui32;
        float f32;
    }reinterp1;
    reinterp1.ui32 = getUint32FromBuff(buffStart);
    return reinterp1.f32;
}

static inline void loadUint32ToBuff(uint16_t* buffStart, uint32_t data)
{
    buffStart[0] = (data >> 0) & 0xffu;
    buffStart[1] = (data >> 8) & 0xffu;
    buffStart[2] = (data >> 16) & 0xffu;
    buffStart[3] = (data >> 24) & 0xffu;
    return;
}

static inline void loadUint16ToBuff(uint16_t* buffStart, uint16_t data)
{
    buffStart[0] = data & 0xffu;
    buffStart[1] = (data >> 8) & 0xffu;
    return;
}

static inline void reverseBuff(uint32_t* buff, uint32_t begin, uint32_t end)
{
    for (;begin < end;begin++, end--)
    {
        uint32_t temp = buff[end];
        buff[end] = buff[begin];
        buff[begin] = temp;
    }
    return;
}

static inline void rightShiftBuff(uint32_t* buff, uint32_t buffLenth, uint32_t rShiftCnt)
{
    rShiftCnt = rShiftCnt % buffLenth;
    if (rShiftCnt == 0)
    {
        return;
    }
    reverseBuff(buff, 0, buffLenth - rShiftCnt - 1);
    reverseBuff(buff, buffLenth - rShiftCnt, buffLenth - 1);
    reverseBuff(buff, 0, buffLenth - 1);
    return;
}

#pragma CODE_SECTION(cdb_prd_call, "ramfuncs2");
void cdb_prd_call(struct cdb_struct* hcdb, uint32_t* cdb_mBuff)
{
    // dump 指令接收
    // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
    if ((HWREG_BP(CAN_CDB_BASE + CAN_O_NDAT_21)) == 0x00000001ul)
    {
        // Get the received message
        if (CAN_readMessage2(CAN_CDB_BASE, 1, hcdb->canRxBuff))
        {
            switch (hcdb->cdbSta)
            {
            case CDB_stop:
            case CDB_ready:
            case CDB_run:
                hcdb->dumpStartAddr = getUint32FromBuff(&hcdb->canRxBuff[0]);
                hcdb->dumpEndAddr = getUint32FromBuff(&hcdb->canRxBuff[4]);
                hcdb->cdbSta = CDB_ready;
                break;
            }
        }
        else
        {
            // 应读而不得
            ESTOP0;
        }
    }

    // trig 指令接收
    // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
    if ((HWREG_BP(CAN_CDB_BASE + CAN_O_NDAT_21)) == 0x00000002ul)
    {
        // Get the received message
        if (CAN_readMessage2(CAN_CDB_BASE, 2, hcdb->canRxBuff))
        {
            switch (hcdb->trigSta)
            {
            default:
                hcdb->trigSta = TSTA_stop;
                // 给一个警告 TODO
                // scd_print(&scd_1, "trig cfg forced changed\n");

            case TSTA_stop:
            case TSTA_fin:
            {
                uint32_t canAddrRegVal = getUint32FromBuff(&hcdb->canRxBuff[0]);
                switch (canAddrRegVal)
                {
                case 0:
                case 1:
                    hcdb->trigSrcAddr[canAddrRegVal] = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                    hcdb->logSrcAddr[canAddrRegVal - 2] = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 18:
                    hcdb->trigLogSrcCnt = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 19:
                    hcdb->trigIdxKeep = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 20:
                    hcdb->trigTimeTar = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 21:
                    hcdb->trigThd = getFloatFromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 22:
                    hcdb->trigSrc = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 23:
                    hcdb->sampInter = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 24:
                    hcdb->trigMode = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                case 25:
                    hcdb->trigSta = getUint32FromBuff(&hcdb->canRxBuff[4]);
                    break;

                default:
                    break;
                }
            }
            break;
            }
        }
        else
        {
            // 应读而不得
            ESTOP0;
        }
    }

    // 内存修改16b指令接收
    // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
    if ((HWREG_BP(CAN_CDB_BASE + CAN_O_NDAT_21)) == 0x00000004ul)
    {
        // Get the received message
        if (CAN_readMessage2(CAN_CDB_BASE, 3, hcdb->canRxBuff))
        {
            // 简单粗暴地修改并给出应答
            uint32_t tarAddr = getUint32FromBuff(&hcdb->canRxBuff[0]);
            uint16_t tarVal = getUint16FromBuff(&hcdb->canRxBuff[4]);

            if (tarAddr >= 0x18000ul && tarAddr <= 0x19000ul)
            {
                IPC_RamW16_Req_CPU2(tarAddr, tarVal);
            }
            else
            {
                *((uint16_t*)(void*)tarAddr) = tarVal;
            }

            loadUint32ToBuff(&hcdb->canTxBuff[0], tarAddr);
            loadUint16ToBuff(&hcdb->canTxBuff[4], tarVal);
            loadUint16ToBuff(&hcdb->canTxBuff[6], 0);
            CAN_sendMessage2(CAN_CDB_BASE, 5, 5, hcdb->canTxBuff);
        }
        else
        {
            // 应读而不得
            ESTOP0;
        }
    }

    // 内存修改32b指令接收
    // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
    if ((HWREG_BP(CAN_CDB_BASE + CAN_O_NDAT_21)) == 0x00000008ul)
    {
        // Get the received message
        if (CAN_readMessage2(CAN_CDB_BASE, 4, hcdb->canRxBuff))
        {
            // 简单粗暴地修改并给出应答
            uint32_t tarAddr = getUint32FromBuff(&hcdb->canRxBuff[0]);
            uint32_t tarVal = getUint32FromBuff(&hcdb->canRxBuff[4]);

            if (tarAddr >= 0x18000ul && tarAddr <= 0x19000ul)
            {
                IPC_RamW32_Req_CPU2(tarAddr, tarVal);
            }
            else
            {
                *((uint32_t*)(void*)tarAddr) = tarVal;
            }

            loadUint32ToBuff(&hcdb->canTxBuff[0], tarAddr);
            loadUint32ToBuff(&hcdb->canTxBuff[4], tarVal);
            CAN_sendMessage2(CAN_CDB_BASE, 5, 5, hcdb->canTxBuff);
        }
        else
        {
            // 应读而不得
            ESTOP0;
        }
    }

    // 内存读取指令接收
    // Poll bit0 in CAN_NDAT_21 register to check completion of Reception
    if ((HWREG_BP(CAN_CDB_BASE + CAN_O_NDAT_21)) == 0x00000020ul)
    {
        // Get the received message
        if (CAN_readMessage2(CAN_CDB_BASE, 6, hcdb->canRxBuff))
        {
            // 简单粗暴地修改并给出应答
            uint32_t tarAddr = getUint32FromBuff(&hcdb->canRxBuff[0]);
            uint32_t tarVal = getUint32FromBuff(&hcdb->canRxBuff[4]);

            loadUint32ToBuff(&hcdb->canTxBuff[0], tarAddr);
            if (tarVal > 1)
            {
                loadUint32ToBuff(&hcdb->canTxBuff[4], *((uint32_t*)(void*)tarAddr));
            }
            else
            {
                loadUint16ToBuff(&hcdb->canTxBuff[4], *((uint16_t*)(void*)tarAddr));
                loadUint16ToBuff(&hcdb->canTxBuff[6], 0);
            }
            CAN_sendMessage2(CAN_CDB_BASE, 5, 5, hcdb->canTxBuff);
        }
        else
        {
            // 应读而不得
            ESTOP0;
        }
    }

    // trig 内容处理
    switch (hcdb->trigSta)
    {
    case TSTA_prepare:
        hcdb->trigTimeCnt = 0;
        hcdb->trigIdxStart = 0;
        hcdb->trigIdxNow = 0;
        hcdb->sampInterCnt = 0;

        // 生成trig判定量
        hcdb->trigVal = 0;
        for (int ii = 0;ii < 2;ii++)
        {
            float tmp = 0;
            switch ((hcdb->trigSrc >> (ii * 4)) & 0b111u)
            {
            case 0:
                tmp = *((int16_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 1:
                tmp = *((uint16_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 2:
                tmp = *((int32_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 3:
                tmp = *((uint32_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 4:
                tmp = *((float*)hcdb->trigSrcAddr[ii]);
                break;
            }
            if ((hcdb->trigSrc >> (ii * 4)) & 0b1000u)
            {
                hcdb->trigVal -= tmp;
            }
            else
            {
                hcdb->trigVal += tmp;
            }
        }
        hcdb->trigValLast = hcdb->trigVal;
        hcdb->trigSta = TSTA_logging;
        break;
    case TSTA_move:
        rightShiftBuff(cdb_mBuff, buffSize, buffSize - hcdb->trigIdxStart - 1);
        // TODO 
        // scd_print(&scd_1, "trig fin\n");
        hcdb->trigSta = TSTA_fin;
        break;
    }

    // dump 内容发送，留足够的时间余量，然后直接发，不做轮询标志位的处理
    switch (hcdb->cdbSta)
    {
    case CDB_ready:
        hcdb->dumpNowAddr = hcdb->dumpStartAddr;
        hcdb->cdbSta = CDB_run;
        break;
    case CDB_run:
        for (int ii = 0;ii < 8;ii++)
        {
            if (hcdb->dumpNowAddr >= hcdb->dumpEndAddr)
            {
                // 发完了
                hcdb->cdbSta = CDB_stop;
                goto cdb_tx_end;
            }
            loadUint32ToBuff(&hcdb->canTxBuff[0], hcdb->dumpNowAddr);
            for (int jj = 0;jj < 2;jj++)
            {
                uint16_t tmpData = 0;
                if (hcdb->dumpNowAddr < hcdb->dumpEndAddr)
                {
                    // 发完了
                    tmpData = *((uint16_t*)hcdb->dumpNowAddr);
                }
                hcdb->dumpNowAddr++;
                loadUint16ToBuff(&hcdb->canTxBuff[4 + jj * 2], tmpData);
            }
            CAN_sendMessage2(CAN_CDB_BASE, 8 + ii, 8 + ii, hcdb->canTxBuff);
        }

        break;
    }

cdb_tx_end:

    return;
}

#pragma CODE_SECTION(cdb_dlog_mIsr, "ramfuncs2");
void cdb_dlog_mIsr(struct cdb_struct* hcdb, uint32_t* cdb_mBuff)
{
    switch (hcdb->trigSta)
    {
    case TSTA_logging:
        // 进行采样间隔处理
        hcdb->sampInterCnt++;
        if (hcdb->sampInterCnt > hcdb->sampInter)
        {
            hcdb->sampInterCnt = 0;
        }
        else
        {
            break;
        }

        // 先记录
        for (int ii = 0;ii < hcdb->trigLogSrcCnt;ii++)
        {
            hcdb->trigIdxNow++;
            if (hcdb->trigIdxNow == buffSize)
            {
                hcdb->trigIdxNow = 0;
            }
            // 预防内存不对齐造成的访存违例
            uint32_t tmp = 0;
            tmp |= *((uint16_t*)(hcdb->logSrcAddr[ii] + 1));
            tmp <<= 16;
            tmp |= *((uint16_t*)(hcdb->logSrcAddr[ii]));
            cdb_mBuff[hcdb->trigIdxNow] = tmp;
        }

        // 生成trig判定量
        hcdb->trigVal = 0;

        // 指示编译器循环展开，空间换时间
#pragma MUST_ITERATE ( 2, 2, 2 )
#pragma UNROLL ( 2 )
        for (int ii = 0;ii < 2;ii++)
        {
            float tmp = 0;
            switch ((hcdb->trigSrc >> (ii * 4)) & 0b111u)
            {
            case 0:
                tmp = *((int16_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 1:
                tmp = *((uint16_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 2:
                tmp = *((int32_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 3:
                tmp = *((uint32_t*)hcdb->trigSrcAddr[ii]);
                break;
            case 4:
                tmp = *((float*)hcdb->trigSrcAddr[ii]);
                break;
            }
            if ((hcdb->trigSrc >> (ii * 4)) & 0b1000u)
            {
                hcdb->trigVal -= tmp;
            }
            else
            {
                hcdb->trigVal += tmp;
            }
        }

        // trig判定
        switch (hcdb->trigMode)
        {
        default:
        case TRIG_force:
            // 触发成功
            hcdb->trigIdxStart = hcdb->trigIdxNow;
            hcdb->trigSta = TSTA_trigged;
            break;

        case TRIG_edge_up:
            if (hcdb->trigVal >= hcdb->trigThd && hcdb->trigValLast <= hcdb->trigThd)
            {
                // 触发成功
                if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                {
                    hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                }
                else
                {
                    hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                }
                hcdb->trigSta = TSTA_trigged;
            }
            break;

        case TRIG_edge_down:
            if (hcdb->trigVal <= hcdb->trigThd && hcdb->trigValLast >= hcdb->trigThd)
            {
                // 触发成功
                if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                {
                    hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                }
                else
                {
                    hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                }
                hcdb->trigSta = TSTA_trigged;
            }
            break;

        case TRIG_edge_both:
            if ((hcdb->trigVal - hcdb->trigThd) * (hcdb->trigValLast - hcdb->trigThd) <= 0)
            {
                // 触发成功
                if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                {
                    hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                }
                else
                {
                    hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                }
                hcdb->trigSta = TSTA_trigged;
            }
            break;

        case TRIG_pulse_highLong:
            if (hcdb->trigVal > hcdb->trigThd)
            {
                hcdb->trigTimeCnt++;
                if (hcdb->trigTimeCnt > hcdb->trigTimeTar)
                {
                    // 触发成功
                    if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                    {
                        hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                    }
                    else
                    {
                        hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                    }
                    hcdb->trigSta = TSTA_trigged;
                }
            }
            else
            {
                hcdb->trigTimeCnt = 0;
            }
            break;

        case TRIG_pulse_highShort:
            if (hcdb->trigVal > hcdb->trigThd)
            {
                hcdb->trigTimeCnt++;
            }
            else
            {
                if (hcdb->trigTimeCnt > 0 && hcdb->trigTimeCnt <= hcdb->trigTimeTar)
                {
                    // 触发成功
                    if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                    {
                        hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                    }
                    else
                    {
                        hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                    }
                    hcdb->trigSta = TSTA_trigged;
                }
                hcdb->trigTimeCnt = 0;
            }
            break;

        case TRIG_pulse_lowLong:
            if (hcdb->trigVal < hcdb->trigThd)
            {
                hcdb->trigTimeCnt++;
                if (hcdb->trigTimeCnt > hcdb->trigTimeTar)
                {
                    // 触发成功
                    if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                    {
                        hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                    }
                    else
                    {
                        hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                    }
                    hcdb->trigSta = TSTA_trigged;
                }
            }
            else
            {
                hcdb->trigTimeCnt = 0;
            }
            break;

        case TRIG_pulse_lowShort:
            if (hcdb->trigVal < hcdb->trigThd)
            {
                hcdb->trigTimeCnt++;
            }
            else
            {
                if (hcdb->trigTimeCnt > 0 && hcdb->trigTimeCnt <= hcdb->trigTimeTar)
                {
                    // 触发成功
                    if (hcdb->trigIdxNow > hcdb->trigIdxKeep)
                    {
                        hcdb->trigIdxStart = hcdb->trigIdxNow - hcdb->trigIdxKeep;
                    }
                    else
                    {
                        hcdb->trigIdxStart = (hcdb->trigIdxNow + buffSize) - hcdb->trigIdxKeep;
                    }
                    hcdb->trigSta = TSTA_trigged;
                }
                hcdb->trigTimeCnt = 0;
            }
            break;

        }
        hcdb->trigValLast = hcdb->trigVal;
        break;

    case TSTA_trigged:
        // 进行采样间隔处理
        hcdb->sampInterCnt++;
        if (hcdb->sampInterCnt > hcdb->sampInter)
        {
            hcdb->sampInterCnt = 0;
        }
        else
        {
            break;
        }

        // 完成剩余的记录
        for (int ii = 0;ii < hcdb->trigLogSrcCnt;ii++)
        {
            hcdb->trigIdxNow++;
            if (hcdb->trigIdxNow == buffSize)
            {
                hcdb->trigIdxNow = 0;
            }
            // 预防内存不对齐造成的访存违例
            uint32_t tmp = 0;
            tmp |= *((uint16_t*)(hcdb->logSrcAddr[ii] + 1));
            tmp <<= 16;
            tmp |= *((uint16_t*)(hcdb->logSrcAddr[ii]));
            cdb_mBuff[hcdb->trigIdxNow] = tmp;
            if (hcdb->trigIdxNow == hcdb->trigIdxStart)
            {
                // 记完了
                hcdb->trigSta = TSTA_move;
                break;
            }
        }
        break;
    }

    return;
}

#ifdef _SCD_ENABLE

#pragma DATA_SECTION(cdb1, "cpu2bss")
struct cdb_struct cdb1;

#endif
