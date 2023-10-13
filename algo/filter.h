/*
 * filter.h
 *
 *  Created on: 2022年6月25日
 *      Author: tyj
 */

#ifndef INCX_FILTER_H_
#define INCX_FILTER_H_

#include "stdint.h"

struct LPF_Ord1_2_struct
{
    float A;
    float B1;
    float B2;
    float C1;
    float C2;
};

static inline void LPF_Ord1_2_init(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float A_init, float B1_init, float B2_init)
{
    hLPF_Ord1_2->A = A_init;
    hLPF_Ord1_2->B1 = B1_init;
    hLPF_Ord1_2->B2 = B2_init;
    hLPF_Ord1_2->C1 = 0;
    hLPF_Ord1_2->C2 = 0;
}

static inline float LPF_Ord1_update(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float Rval)
{
    float ret;
    ret = hLPF_Ord1_2->A * (Rval - hLPF_Ord1_2->C1) + hLPF_Ord1_2->C1;
    hLPF_Ord1_2->C1 = ret;
    return ret;
}

static inline float LPF_Ord1_update_kahan(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float Rval)
{
    volatile float dx_1 = hLPF_Ord1_2->A * (Rval - hLPF_Ord1_2->C1) - hLPF_Ord1_2->B2;
    volatile float sum_temp = hLPF_Ord1_2->C1 + dx_1;
    volatile float no_opti1 = sum_temp - hLPF_Ord1_2->C1;
    hLPF_Ord1_2->B2 = no_opti1 - dx_1;
    hLPF_Ord1_2->C1 = sum_temp;
    return sum_temp;
}

static inline float LPF_Ord2_update(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float Rval)
{
    float ret;
    ret = hLPF_Ord1_2->A * Rval - hLPF_Ord1_2->B1 * hLPF_Ord1_2->C1 - hLPF_Ord1_2->B2 * hLPF_Ord1_2->C2;
    hLPF_Ord1_2->C2 = hLPF_Ord1_2->C1;
    hLPF_Ord1_2->C1 = ret;
    return ret;
}

static inline float LPF_Ord2_update_kahan(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float Rval)
{
    {
        volatile float dx_1 = hLPF_Ord1_2->A * (Rval - hLPF_Ord1_2->C2) - hLPF_Ord1_2->B2;
        volatile float sum_temp1 = hLPF_Ord1_2->C2 + dx_1;
        volatile float no_opti1 = sum_temp1 - hLPF_Ord1_2->C2;
        hLPF_Ord1_2->B2 = no_opti1 - dx_1;
        hLPF_Ord1_2->C2 = sum_temp1;
    }

    {
        volatile float dx_2 = hLPF_Ord1_2->A * (hLPF_Ord1_2->C2 - hLPF_Ord1_2->C1) - hLPF_Ord1_2->B1;
        volatile float sum_temp2 = hLPF_Ord1_2->C1 + dx_2;
        volatile float no_opti2 = sum_temp2 - hLPF_Ord1_2->C1;
        hLPF_Ord1_2->B1 = no_opti2 - dx_2;
        hLPF_Ord1_2->C1 = sum_temp2;
    }

    return hLPF_Ord1_2->C1;
}

static inline void LPF_Ord2_setVal(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float cVal)
{
    hLPF_Ord1_2->C2 = cVal;
    hLPF_Ord1_2->C1 = cVal;
}

static inline void LPF_Ord1_setVal(struct LPF_Ord1_2_struct* hLPF_Ord1_2, float cVal)
{
    hLPF_Ord1_2->C1 = cVal;
}

static inline float LPF_Ord1_2_getVal(struct LPF_Ord1_2_struct* hLPF_Ord1_2)
{
    return hLPF_Ord1_2->C1;
}

#endif /* INCX_FILTER_H_ */

