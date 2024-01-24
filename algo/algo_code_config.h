#ifndef INC_ALGO_CODE_CONFIG_H_
#define INC_ALGO_CODE_CONFIG_H_

/// 版本号
#define TYJ_VERSION "1.0"

#include "matlab_export_para.h"

// PWM频率
#define PWM_FREQ (MATLAB_PARA_fs)
// 控制频率
#define CTRL_FREQ (MATLAB_PARA_ctrl_freq)
// PWM模块的输入时钟
#define PWM_CLK (MATLAB_PARA_PWM_clk)

// PWM装载值
#define PWM_LOAD_VAL (PWM_CLK / PWM_FREQ / 2)

// PWM默认比较值
#define PWM_CMP_DEFAILT_VAL (PWM_LOAD_VAL / 2)

// 补偿内联函数方便仿真
#ifdef TYJ_TEST

#include "math.h"

static inline float __div2pif32(float src)
{
    return src * (float)(1.0 / MATLAB_PARA_pi2);
}

static inline float __mpy2pif32(float src)
{
    return src * (float)(MATLAB_PARA_pi2);
}

static inline float __sinpuf32(float src)
{
    return sinf(src * (float)(MATLAB_PARA_pi2));
}

static inline float __cospuf32(float src)
{
    return cosf(src * (float)(MATLAB_PARA_pi2));
}

static inline float __atan2puf32(float y, float x)
{
    return atan2f(y, x) * (float)(1.0 / MATLAB_PARA_pi2);
}

static inline float __divf32(float num, float denom)
{
    return num / denom;
}

// 与simulink兼容
#ifndef TYJ_IN_SIMULINK

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
#endif

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

// RAM函数设置
#define IF_USE_HIGH_VER_TI_COMPLIER 1
#if IF_USE_HIGH_VER_TI_COMPLIER
#define MEM_MACRO ".TI.ramfunc"
#else
#define MEM_MACRO "ramfuncs"
#endif

#endif
