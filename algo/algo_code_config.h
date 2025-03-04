#ifndef INC_ALGO_CODE_CONFIG_H_
#define INC_ALGO_CODE_CONFIG_H_

/// 版本号
#define TYJ_VERSION "1.0"

#include "stdint.h"
#include "matlab_export_para.h"

// PWM模块的输入时钟
#define PWM_CLK (MATLAB_PARA_PWM_clk)
#define ECAP_CLK (MATLAB_PARA_PWM_clk * 2)
// PWM频率
#define PWM_FREQ (MATLAB_PARA_fs)
// 控制频率
#define CTRL_FREQ (MATLAB_PARA_ctrl_freq)
// PWM装载值，为简化后续程序处理，近似到邻近的偶数上去
#define PWM_LOAD_VAL ((((int32_t)(PWM_CLK / PWM_FREQ / 2))/2)*2)
// PWM默认比较值
#define PWM_CMP_DEFAILT_VAL (PWM_LOAD_VAL / 2)

extern float vCTRL_FREQ;
extern float vCTRL_TS;
extern float vPWM_LOAD_VAL;
extern float vECAP_1CYCLE_VAL_INV;

extern int16_t vPWM_LOAD_VAL_I;
extern int16_t vPWM_CMP_DEFAILT_VAL_I;

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(algo_clk_cfg)
#endif
static inline void algo_clk_cfg(uint16_t pwm_freq)
{
    vCTRL_FREQ = pwm_freq * MATLAB_PARA_ctrl_freq_mul;
    vCTRL_TS = 1.0f / vCTRL_FREQ;
    // 为简化后续程序处理，近似到邻近的偶数上去
    vPWM_LOAD_VAL_I = ((((uint32_t)PWM_CLK) / ((uint32_t)2 * pwm_freq)) / 2) * 2;
    vPWM_LOAD_VAL = vPWM_LOAD_VAL_I;
    vECAP_1CYCLE_VAL_INV = MATLAB_PARA_ctrl_freq_mul * pwm_freq / ECAP_CLK;
    vPWM_CMP_DEFAILT_VAL_I = vPWM_LOAD_VAL_I / 2;
}

#ifdef TYJ_TEST

#include "math.h"

// ccs有，但matlab和simulink都没有

static inline float __div2pif32(float src)
{
    return src * (float)(1.0 / M_PI / 2.0);
}

static inline float __mpy2pif32(float src)
{
    return src * (float)(2.0 * M_PI);
}

static inline float __sinpuf32(float src)
{
    return sinf(src * (float)(2.0 * M_PI));
}

static inline float __cospuf32(float src)
{
    return cosf(src * (float)(2.0 * M_PI));
}

static inline float __atan2puf32(float y, float x)
{
    return atan2f(y, x) * (float)(1.0 / M_PI / 2.0);
}

static inline float __atanpuf32(float x)
{
    return atanf(x) * (float)(1.0 / M_PI / 2.0);
}

static inline float __divf32(float num, float denom)
{
    return num / denom;
}

static inline float __fmax(float x, float y)
{
    if (x >= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

static inline float __fmin(float x, float y)
{
    if (x <= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

#ifdef TYJ_IN_MATLABMEX

// ccs有，matlab没有，simulink有

#else

// ccs有，matlab有，simulink没有

static inline int __max(int x, int y)
{
    if (x >= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

static inline int __min(int x, int y)
{
    if (x <= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

#endif

#else

// 不在仿真环境中
// RAM函数设置
#define IF_USE_HIGH_VER_TI_COMPLIER 1
#if IF_USE_HIGH_VER_TI_COMPLIER
#define MEM_MACRO ".TI.ramfunc"
#else
#define MEM_MACRO "ramfuncs"
#endif

#endif

#endif
