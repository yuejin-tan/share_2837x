/*
 * main.h
 *
 *  Created on: 2023年10月13日
 *      Author: t
 */

#ifndef TOPCFG_H_
#define TOPCFG_H_

#ifdef PROJ_SHARE


 // 数组大小 LS2-5: 2k*4; D0-1: 2k*2; GS0-11: 4k*12;
#define RAMBUFFSIZE (15ul * 4ul * 1024ul / 2ul)

#define CDB_BUFF_ADDR ((void*)0x9000ul)

#define CAN_CDB_BASE CANA_BASE

#else

#include "topCfg_local.h"

#endif

#endif /* TOPCFG_H_ */
