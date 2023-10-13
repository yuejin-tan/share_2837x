/*
 * main.h
 *
 *  Created on: 2023年10月13日
 *      Author: t
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stdint.h"

 // bsp 接口函数
void bsp_init_cpu2dev();

void initSysCtrl();

void bsp_init_all();

void ctrl_init();

void bsp_start();

void infLoopMain();

#endif /* MAIN_H_ */
