/*
 * cdb.h
 *
 *  Created on: 2023年4月16日
 *      Author: t
 */

#ifndef MCU_CODE_INCX_CDB_H_
#define MCU_CODE_INCX_CDB_H_

#include "stdint.h"

enum CDB_MODE
{
    CDB_Err = -1,
    CDB_stop = 0,
    CDB_ready,
    CDB_run,
};

enum TRIG_SRC
{
    SRC_1 = 0,
    SRC_1_neg,
    SRC_1_ADD_2,
    SRC_1_SUB_2,
};

enum TRIG_MODE
{
    TRIG_force = 0,
    TRIG_edge_up,
    TRIG_edge_down,
    TRIG_edge_both,
    TRIG_pulse_highLong,
    TRIG_pulse_highShort,
    TRIG_pulse_lowLong,
    TRIG_pulse_lowShort,
};

enum TRIG_STATUS
{
    TSTA_stop = 0,
    TSTA_prepare,
    TSTA_logging,
    TSTA_trigged,
    TSTA_move,
    TSTA_fin,
};

struct cdb_struct
{
    // 触发器状态
    int16_t trigSta; // 25
    // 触发模式
    int16_t trigMode; // 24
    // dump 开始地址 该地址处数据发送
    uint32_t dumpStartAddr;
    // dump 正处理的地址
    uint32_t dumpNowAddr;
    // dump 结束地址 该地址处的数据不发送
    uint32_t dumpEndAddr;
    // CAN TX 缓存
    uint16_t canTxBuff[8];
    // CAN RX 缓存
    uint16_t canRxBuff[8];
    // 触发值待选对象地址
    uint32_t trigSrcAddr[2]; // 0-1
    // 需记录的数据地址
    uint32_t logSrcAddr[16]; // 2-17
    // 需记录的数据个数
    uint32_t trigLogSrcCnt; // 18
    // 需保存的触发前的数据个数
    uint32_t trigIdxKeep; // 19
    // 和时间有关触发的目标值
    uint32_t trigTimeTar; // 20
    // 和时间有关触发的计数
    uint32_t trigTimeCnt;
    // 开始处的前一个
    uint32_t trigIdxStart;
    // 现在记录的，指着记完的那一个
    uint32_t trigIdxNow;
    // 触发阈值
    float trigThd; // 21
    // 触发值选择
    // bit 0-2 类型 0 int16 1 uint16 2 int32 3 uint32 4 float 5 =0
    // bit 3 是否取反
    // src[0]在低位 src[1]在高位
    uint32_t trigSrc; // 22
    // 采样间隔，0表示无间隔，1表示间隔一次采一次……
    uint32_t sampInter; // 23
    // 采样间隔计数值
    uint32_t sampInterCnt;
    // 计算的触发值
    float trigVal;
    // 上一次计算的触发值
    float trigValLast;
    // cdb模块状态
    int16_t cdbSta;
};

extern struct cdb_struct cdb1;

#endif /* MCU_CODE_INCX_CDB_H_ */
