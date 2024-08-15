#ifndef INCX_SVPWM_H_
#define INCX_SVPWM_H_

#include "algo_code_config.h"

struct ThataCal_struct;

struct SVPWM_struct
{
    float Ua;
    float Ub;
    float Uc;
    float Udc;
    float Udc_inv;
    float Udc_times_2;

    int16_t epwmU;
    int16_t epwmV;
    int16_t epwmW;
};

static inline void SVPWM_setUdc(struct SVPWM_struct* hSVPWM, float Udc)
{
    hSVPWM->Udc = Udc;
    hSVPWM->Udc_inv = 1.0f / Udc;
    hSVPWM->Udc_times_2 = 2.0f * Udc;
}

static inline void SVPWM_init(struct SVPWM_struct* hSVPWM, float Udc)
{
    SVPWM_setUdc(hSVPWM, Udc);

    hSVPWM->Ua = Udc * 0.5f;
    hSVPWM->Ub = Udc * 0.5f;
    hSVPWM->Uc = Udc * 0.5f;

    hSVPWM->epwmU = vPWM_CMP_DEFAILT_VAL_I;
    hSVPWM->epwmV = vPWM_CMP_DEFAILT_VAL_I;
    hSVPWM->epwmW = vPWM_CMP_DEFAILT_VAL_I;
}


// 带有过调制的SVPWM计算，改自袁雷一书
static inline void SVPWM_dutyCal(struct SVPWM_struct* hSVPWM, float Ual, float Ube)
{
#define SQRT_3 1.73205F

    //  基本空间矢量作用时间计算
    float X = (SQRT_3 * 2.0f) * Ube;
    float Y = ((3.0f) * Ual + SQRT_3 * Ube);
    // float Z = ((-3.0f) * Ualpha + SQRT_3 * Ubeta);
    float Z = X - Y;

    // 变量初始化
    int16_t sector = 0;

    // 扇区计算
    if (X > 0)
        sector = 1;
    if (Z < 0)
        sector = sector + 2;
    if (Y < 0)
        sector = sector + 4;

    float T1, T2;

    // 按扇区分配T1 T2给各自的基本空间矢量
    switch (sector)
    {
    case 1:
        T1 = Z;
        T2 = Y;
        break;
    case 2:
        T1 = Y;
        T2 = -X;
        break;
    case 3:
        T1 = -Z;
        T2 = X;
        break;
    case 4:
        T1 = -X;
        T2 = Z;
        break;
    case 5:
        T1 = X;
        T2 = -Y;
        break;
    default:
        T1 = -Y;
        T2 = -Z;
        break;
    }

    // 过调制处理
    if (T1 + T2 > hSVPWM->Udc_times_2)
    {
        T1 = T1 / (T1 + T2) * hSVPWM->Udc_times_2;
        T2 = hSVPWM->Udc_times_2 - T1;
    }

    float ta, tb, tc;

    // 矢量切换点计算
    ta = (hSVPWM->Udc_times_2 - T1 - T2) * 0.25f;
    tb = ta + T1 * 0.5f;
    tc = tb + T2 * 0.5f;

    switch (sector)
    {
    case 1:
        hSVPWM->Ua = tb;
        hSVPWM->Ub = ta;
        hSVPWM->Uc = tc;
        break;
    case 2:
        hSVPWM->Ua = ta;
        hSVPWM->Ub = tc;
        hSVPWM->Uc = tb;
        break;
    case 3:
        hSVPWM->Ua = ta;
        hSVPWM->Ub = tb;
        hSVPWM->Uc = tc;
        break;
    case 4:
        hSVPWM->Ua = tc;
        hSVPWM->Ub = tb;
        hSVPWM->Uc = ta;
        break;
    case 5:
        hSVPWM->Ua = tc;
        hSVPWM->Ub = ta;
        hSVPWM->Uc = tb;
        break;
    default:
        hSVPWM->Ua = tb;
        hSVPWM->Ub = tc;
        hSVPWM->Uc = ta;
        break;
    }

    hSVPWM->epwmU = hSVPWM->Ua * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.4f;
    hSVPWM->epwmV = hSVPWM->Ub * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.4f;
    hSVPWM->epwmW = hSVPWM->Uc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.4f;
}

#endif /* INCX_SVPWM_H_ */
