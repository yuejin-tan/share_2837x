/*
 * protect.h
 *
 *  Created on: 2022年7月5日
 *      Author: tyj
 */

#ifndef INCX_PRCTECT_H_
#define INCX_PRCTECT_H_

#include "stdint.h"
#include "algo_code_config.h"

enum PROTECT_TYPE
{
    PRCT_T_THD_L = 1,
    PRCT_T_THD_H = 2,
    PRCT_T_INTG = 4,
    PRCT_T_FOLW = 8,
};

struct protect_struct
{
    float thd_l;
    float thd_h;
    float intg_base;
    float intg_max;
    float intg_val;
    float follow_Dmax;
    float follow_intg_max;
    float follow_intg_val;
};

static inline void protect_init(struct protect_struct* hProtect, float thd_l_init, float thd_h_init, float intg_base_init, float intg_max_init, float follow_DMax_init, float follow_INTG_MAX_init)
{
    hProtect->thd_l = thd_l_init;
    hProtect->thd_h = thd_h_init;
    hProtect->intg_base = intg_base_init;
    hProtect->intg_max = intg_max_init;
    hProtect->follow_Dmax = follow_DMax_init;
    hProtect->follow_intg_max = follow_INTG_MAX_init;
    hProtect->intg_val = 0;
    hProtect->follow_intg_val = 0;
}


static inline int16_t protectCalc_THD_HL(struct protect_struct* hProtect, float val)
{
    int16_t ret = 0;

    if (val < hProtect->thd_l)
    {
        ret += PRCT_T_THD_L;
    }
    if (val > hProtect->thd_h)
    {
        ret += PRCT_T_THD_H;
    }

    return ret;
}

static inline int16_t protectCalc_INTG(struct protect_struct* hProtect, float val)
{
    int16_t ret = 0;

    float ans = val - hProtect->intg_base;
    hProtect->intg_val += ans;
    hProtect->intg_val = __fmax(hProtect->intg_val, 0);
    if (hProtect->intg_val > hProtect->intg_max)
    {
        ret += PRCT_T_INTG;
    }

    return ret;
}

static inline int16_t protectCalc_FOLW(struct protect_struct* hProtect, float val, float folwTarget)
{
    int16_t ret = 0;

    float delta = fabsf(val - folwTarget);
    float intg = hProtect->follow_intg_val;
    intg += (delta - hProtect->follow_Dmax);
    intg = __fmax(intg, 0);
    if (intg > hProtect->follow_intg_max)
    {
        ret += PRCT_T_FOLW;
    }
    hProtect->follow_intg_val = intg;

    return ret;
}

static inline void protect_FOLW_Clear(struct protect_struct* hProtect)
{
    hProtect->follow_intg_val = 0;
}

#endif /* INCX_PRCTECT_H_ */
