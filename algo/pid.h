#ifndef INCX_PID_H_
#define INCX_PID_H_

#include "algo_code_config.h"

struct PIctrl_struct
{
    float kp;
    float ki;
    // back-calculation 抗饱和
    float kb;

    float max;
    float min;

    float integral;
    float ans;
};

static inline void PIctrl_init(struct PIctrl_struct* hPIctrl, float kp_init, float ki_init, float kb_init, float max_init, float min_init)
{
    hPIctrl->kp = kp_init;
    hPIctrl->ki = ki_init;
    hPIctrl->kb = kb_init;
    hPIctrl->max = max_init;
    hPIctrl->min = min_init;
    hPIctrl->integral = 0;
    hPIctrl->ans = 0;
}

static inline void PIctrl_Iloop_cfg(struct PIctrl_struct* hPIctrl, float Iloop_bw_factor, float Ldq, float Rall, float max_init, float min_init)
{
    float alpha = vCTRL_FREQ * Iloop_bw_factor;
    hPIctrl->kp = alpha * Ldq;
    hPIctrl->ki = alpha * Rall;
    hPIctrl->kb = hPIctrl->ki;
    hPIctrl->max = max_init;
    hPIctrl->min = min_init;
    hPIctrl->integral = 0;
    hPIctrl->ans = 0;
}

static inline float PIctrl_update_bCalc(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->integral = hPIctrl->integral + hPIctrl->kb * (hPIctrl->max - hPIctrl->ans);
        hPIctrl->ans = hPIctrl->max;
    }
    if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->integral = hPIctrl->integral + hPIctrl->kb * (hPIctrl->min - hPIctrl->ans);
        hPIctrl->ans = hPIctrl->min;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_bCalc2(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta * vCTRL_TS;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->integral = hPIctrl->integral + hPIctrl->kb * (hPIctrl->max - hPIctrl->ans);
        hPIctrl->ans = hPIctrl->max;
    }
    if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->integral = hPIctrl->integral + hPIctrl->kb * (hPIctrl->min - hPIctrl->ans);
        hPIctrl->ans = hPIctrl->min;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_clamp(struct PIctrl_struct* hPIctrl, float delta)
{
    float delta_intg = hPIctrl->ki * delta;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta + delta_intg;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->ans = hPIctrl->max;
        if (delta < 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->ans = hPIctrl->min;
        if (delta > 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else
    {
        hPIctrl->integral += delta_intg;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_clamp2(struct PIctrl_struct* hPIctrl, float delta)
{
    float delta_intg = hPIctrl->ki * delta * vCTRL_TS;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta + delta_intg;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->ans = hPIctrl->max;
        if (delta < 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->ans = hPIctrl->min;
        if (delta > 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else
    {
        hPIctrl->integral += delta_intg;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_clamp_comp(struct PIctrl_struct* hPIctrl, float delta, float compVal)
{
    float delta_intg = hPIctrl->ki * delta;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta + delta_intg + compVal;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->ans = hPIctrl->max;
        if (delta < 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->ans = hPIctrl->min;
        if (delta > 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else
    {
        hPIctrl->integral += delta_intg;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_clamp_comp2(struct PIctrl_struct* hPIctrl, float delta, float compVal)
{
    float delta_intg = hPIctrl->ki * delta * vCTRL_TS;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta + delta_intg + compVal;
    if (hPIctrl->ans > hPIctrl->max)
    {
        hPIctrl->ans = hPIctrl->max;
        if (delta < 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else if (hPIctrl->ans < hPIctrl->min)
    {
        hPIctrl->ans = hPIctrl->min;
        if (delta > 0)
        {
            hPIctrl->integral += delta_intg;
        }
    }
    else
    {
        hPIctrl->integral += delta_intg;
    }
    return hPIctrl->ans;
}

static inline float PIctrl_update_noSat(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta;
    return hPIctrl->ans;
}

static inline float PIctrl_update_noSat2(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta * vCTRL_TS;
    hPIctrl->ans = hPIctrl->integral + hPIctrl->kp * delta;
    return hPIctrl->ans;
}

static inline float PIctrl_update_intgOnly(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta;
    hPIctrl->ans = hPIctrl->integral;
    return hPIctrl->ans;
}

static inline float PIctrl_update_intgOnly2(struct PIctrl_struct* hPIctrl, float delta)
{
    hPIctrl->integral += hPIctrl->ki * delta * vCTRL_TS;
    hPIctrl->ans = hPIctrl->integral;
    return hPIctrl->ans;
}

static inline void PIctrl_setIntg(struct PIctrl_struct* hPIctrl, float intgVal)
{
    hPIctrl->integral = intgVal;
}

static inline float PIctrl_getAns(struct PIctrl_struct* hPIctrl)
{
    return hPIctrl->ans;
}

static inline void PIctrl_setKpKi_clamp(struct PIctrl_struct* hPIctrl, float newKp, float newKi)
{
    hPIctrl->kp = newKp;
    hPIctrl->ki = newKi;
}

static inline void PIctrl_setKpKi_bcalc(struct PIctrl_struct* hPIctrl, float newKp, float newKi)
{
    hPIctrl->kp = newKp;
    hPIctrl->ki = newKi;
    hPIctrl->kb = newKi;
}

static inline void PIctrl_svpwmBoundSet(struct PIctrl_struct* hPIctrl, float newUdc)
{
    hPIctrl->max = newUdc;
    hPIctrl->min = -newUdc;
}

#endif /* INCX_PID_H_ */
