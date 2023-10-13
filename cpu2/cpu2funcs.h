/*
 * cpu2funcs.h
 *
 *  Created on: 2023年8月15日
 *      Author: t
 */

#ifndef MCU_CODE_CPU2_CPU2FUNCS_H_
#define MCU_CODE_CPU2_CPU2FUNCS_H_

#include "stdint.h"

extern void F28x_usDelay2(long LoopCount);
#define DELAY_US2(A)  F28x_usDelay2(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)


void * memcpy_tyj(void * dst, const void * src, uint32_t size);

uint64_t getIpcTimeStamp();


#endif /* MCU_CODE_CPU2_CPU2FUNCS_H_ */
