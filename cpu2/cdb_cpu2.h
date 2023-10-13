/*
 * cdb_cpu2.h
 *
 *  Created on: 2023年10月12日
 *      Author: t
 */

#ifndef MCU_CODE_CPU2_SHARE2_CDB_CPU2_H_
#define MCU_CODE_CPU2_SHARE2_CDB_CPU2_H_


#include "cdb.h"

void cdb_init(struct cdb_struct* hcdb);

void cdb_prd_call(struct cdb_struct* hcdb, uint32_t* cdb_mBuff);

void cdb_dlog_mIsr(struct cdb_struct* hcdb, uint32_t* cdb_mBuff);

#endif /* MCU_CODE_CPU2_SHARE2_CDB_CPU2_H_ */
