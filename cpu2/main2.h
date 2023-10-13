/*
 * main.h
 *
 *  Created on: 2023年10月13日
 *      Author: t
 */

#ifndef MAIN2_H_
#define MAIN2_H_

#include "stdint.h"

 // bsp 接口函数
void cpu2InitSysCtrl();

void bsp_init_all2();

void ctrl_init2();

void bsp_start2();

void infLoopMain2();

#endif /* MAIN2_H_ */
