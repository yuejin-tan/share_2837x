#ifndef SAMP_H
#define SAMP_H

#include "math.h"
#include "algo_code_config.h"

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getCurSI)
#endif
static inline float getCurSI(int16_t adcVal, float offset)
{
    return (adcVal - offset) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_Isense_gain);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getThetaEUint)
#endif
static inline uint16_t getThetaEUint(uint16_t RDCVal)
{
    uint16_t thetaE = RDCVal * (uint16_t)(MATLAB_PARA_RDC2ELE_RATIO);
    return thetaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getThetaEpu)
#endif
static inline float getThetaEpu(uint16_t RDCVal)
{
    return getThetaEUint(RDCVal) * (float)(1.0 / 65536.0);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getThetaESI)
#endif
static inline float getThetaESI(uint16_t RDCVal)
{
    return getThetaEUint(RDCVal) * (float)(2.0 * M_PI / 65536.0);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getBusCurSI)
#endif
static inline float getBusCurSI(int16_t adcVal, float offset)
{
    return (adcVal - offset) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_I_bus_sense_gain);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getBusVoltSI)
#endif
static inline float getBusVoltSI(int16_t adcVal)
{
    return (adcVal - (int)(MATLAB_PARA_U_bus_sense_offset * MATLAB_PARA_adc_gain)) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_U_bus_sense_gain);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getDeltaCapEnSI)
#endif
static inline float getDeltaCapEnSI(float cV, float tarV)
{
    return (float)(0.5f * MATLAB_PARA_C_dclink) * (cV * cV - tarV * tarV);
}

#endif // SAMP_H
