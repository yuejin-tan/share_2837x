#ifndef INCX_SVPWM_H_
#define INCX_SVPWM_H_

#include "algo_code_config.h"

struct ThataCal_struct;

struct SVPWM_struct
{
    float Udc;
    float Udc_inv;

    int16_t epwmU;
    int16_t epwmV;
    int16_t epwmW;
};

static inline void SVPWM_setUdc(struct SVPWM_struct* hSVPWM, float Udc)
{
    hSVPWM->Udc = Udc;
    hSVPWM->Udc_inv = 1.0f / Udc;
}

static inline void SVPWM_init(struct SVPWM_struct* hSVPWM, float Udc)
{
    SVPWM_setUdc(hSVPWM, Udc);

    hSVPWM->epwmU = vPWM_CMP_DEFAILT_VAL_I;
    hSVPWM->epwmV = vPWM_CMP_DEFAILT_VAL_I;
    hSVPWM->epwmW = vPWM_CMP_DEFAILT_VAL_I;
}


// 带有过调制的SVPWM计算，改自袁雷一书
static inline void SVPWM_dutyCal(struct SVPWM_struct* hSVPWM, float Ual, float Ube)
{
#define SQRT_3 1.73205F

    //  基本空间矢量作用时间计算
    float X = (SQRT_3)*Ube;
    float Y = ((1.5F) * Ual + (SQRT_3 * 0.5F) * Ube);
    // float Z = ((-1.5f) * Ualpha + (SQRT_3 * 0.5F) * Ubeta);
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
    if (T1 + T2 > hSVPWM->Udc)
    {
        if (T1 >= T2)
        {
            if (T1 > hSVPWM->Udc)
            {
                // II区过调制
                T1 = hSVPWM->Udc;
                T2 = 0;
            }
            else
            {
                // I区过调制
                T1 = T1 / (T1 + T2) * hSVPWM->Udc;
                T2 = hSVPWM->Udc - T1;
            }
        }
        else
        {
            if (T2 > hSVPWM->Udc)
            {
                // II区过调制
                T1 = 0;
                T2 = hSVPWM->Udc;
            }
            else
            {
                // I区过调制
                T1 = T1 / (T1 + T2) * hSVPWM->Udc;
                T2 = hSVPWM->Udc - T1;
            }
        }
    }

    float ta, tb, tc;

    // 矢量切换点计算
    ta = (hSVPWM->Udc - T1 - T2) * 0.5F;
    tb = ta + T1;
    tc = tb + T2;

    switch (sector)
    {
    case 1:
        hSVPWM->epwmU = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    case 2:
        hSVPWM->epwmU = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    case 3:
        hSVPWM->epwmU = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    case 4:
        hSVPWM->epwmU = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    case 5:
        hSVPWM->epwmU = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    default:
        hSVPWM->epwmU = tb * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmV = tc * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        hSVPWM->epwmW = ta * hSVPWM->Udc_inv * vPWM_LOAD_VAL + 0.5f;
        break;
    }
}

// 快速SVPWM计算方法，但过调制时不能按原方向缩回六边形中
static inline void SVPWM_dutyCal2(struct SVPWM_struct* hSVPWM, float Ual, float Ube)
{
    const float one_div_2 = 0.5;
    const float sqrt_3_div_2 = 0.866025404;
    float Ua = Ual;
    float Ub = -one_div_2 * Ual + sqrt_3_div_2 * Ube;
    float Uc = -one_div_2 * Ual - sqrt_3_div_2 * Ube;

    float max = __fmax(Ua, __fmax(Ub, Uc));
    float min = __fmin(Ua, __fmin(Ub, Uc));
    float mid = (max + min) * 0.5F;

    Ua = Ua - mid;
    Ub = Ub - mid;
    Uc = Uc - mid;

    // 输出兼容屎山
    hSVPWM->epwmU = (0.5f - Ua * hSVPWM->Udc_inv) * vPWM_LOAD_VAL + 0.5f;
    hSVPWM->epwmV = (0.5f - Ub * hSVPWM->Udc_inv) * vPWM_LOAD_VAL + 0.5f;
    hSVPWM->epwmW = (0.5f - Uc * hSVPWM->Udc_inv) * vPWM_LOAD_VAL + 0.5f;
}

#endif /* INCX_SVPWM_H_ */
