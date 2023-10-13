#ifndef COMP_H
#define COMP_H

#include "stdint.h"
#include "algo_code_config.h"

static inline float UdComp(float Ud, float omegaE, float iq)
{
    return Ud - iq * (float)MATLAB_PARA_Lq * omegaE;
}

static inline float getUdCompVal(float omegaE, float iq)
{
    return -iq * (float)MATLAB_PARA_Lq * omegaE;
}

static inline float UqComp(float Uq, float omegaE, float id)
{
    return Uq + (id * (float)MATLAB_PARA_Ld + (float)MATLAB_PARA_faif) * omegaE;
}

static inline float getUqCompVal(float omegaE, float id)
{
    return (id * (float)MATLAB_PARA_Ld + (float)MATLAB_PARA_faif) * omegaE;
}

static inline float thetaComp(float theta, float omegaE)
{
    return theta + omegaE * (float)(1.5f / CTRL_FREQ / MATLAB_PARA_pi2);
}

static inline float IqCompVal(float omegaE_inv, float Udc, float Idc)
{
    return Udc * Idc * omegaE_inv * (float)(1.0f / 1.5f / MATLAB_PARA_faif);
}

#endif // COMP_H
