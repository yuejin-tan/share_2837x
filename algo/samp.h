#ifndef SAMP_H
#define SAMP_H

#include "math.h"
#include "algo_code_config.h"

static inline float getCurSI(int16_t adcVal, float offset)
{
    return (adcVal - offset) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_Isense_gain);
}

static inline float getThetaESI(uint16_t RDCVal)
{
    float thetaE = RDCVal * (float)(MATLAB_PARA_p0 / MATLAB_PARA_RDC_gain / 2.0 / M_PI);

    while (thetaE > 1.0f)
    {
        thetaE -= 1.0f;
    }
    while (thetaE < 0.0f)
    {
        thetaE += 1.0f;
    }

    return thetaE;
}

static inline uint16_t getThetaEUint(uint16_t RDCVal)
{
    uint16_t thetaE = RDCVal * (uint16_t)(MATLAB_PARA_RDC2ELE_RATIO);

    return thetaE;
}

static inline float getBusCurSI(int16_t adcVal, float offset)
{
    return (adcVal - offset) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_I_bus_sense_gain);
}

static inline float getBusVoltSI(int16_t adcVal)
{
    return (adcVal - (int)(MATLAB_PARA_U_bus_sense_offset * MATLAB_PARA_adc_gain)) * (float)(1.0 / MATLAB_PARA_adc_gain / MATLAB_PARA_U_bus_sense_gain);
}

static inline float getDeltaCapEnSI(float cV, float tarV)
{
    return (float)(0.5f * MATLAB_PARA_C_dclink) * (cV * cV - tarV * tarV);
}

#endif // SAMP_H
