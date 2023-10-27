/*
 * cpu2funcs.h
 *
 *  Created on: 2023年8月15日
 *      Author: t
 */

#ifndef MCU_CODE_CPU2_CPU2FUNCS_H_
#define MCU_CODE_CPU2_CPU2FUNCS_H_

#include "stdint.h"
#include "F28x_Project.h"
#include "can.h"

extern void F28x_usDelay2(long LoopCount);
#define DELAY_US2(A)  F28x_usDelay2(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)


void* memcpy_tyj(void* dst, const void* src, uint32_t size);

uint64_t getIpcTimeStamp();

uint16_t CAN_readMessage2(uint32_t base, uint32_t objID, uint16_t* msgData);

void CAN_sendMessage2(uint32_t base, uint32_t objID, uint16_t msgLen, const uint16_t* msgData);

void IPC_RamW16_Req_CPU2(uint32_t addr, uint16_t val);

void IPC_RamW32_Req_CPU2(uint32_t addr, uint32_t val);


#endif /* MCU_CODE_CPU2_CPU2FUNCS_H_ */
