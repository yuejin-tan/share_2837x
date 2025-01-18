#ifndef COMP_H
#define COMP_H

#include "algo_code_config.h"

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(UdComp)
#endif
static inline float UdComp(float Ud, float omegaE, float iq)
{
    return Ud - iq * (float)MATLAB_PARA_Lq * omegaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getUdCompVal)
#endif
static inline float getUdCompVal(float omegaE, float iq)
{
    return -iq * (float)MATLAB_PARA_Lq * omegaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(UqComp)
#endif
static inline float UqComp(float Uq, float omegaE, float id)
{
    return Uq + (id * (float)MATLAB_PARA_Ld + (float)MATLAB_PARA_faif) * omegaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(getUqCompVal)
#endif
static inline float getUqCompVal(float omegaE, float id)
{
    return (id * (float)MATLAB_PARA_Ld + (float)MATLAB_PARA_faif) * omegaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(thetaComp)
#endif
static inline float thetaComp(float theta, float omegaE)
{
    return theta + omegaE * (float)(1.5 / 2.0 / M_PI) * vCTRL_TS;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(IqCompVal)
#endif
static inline float IqCompVal(float omegaE_inv, float Udc, float Idc)
{
    return Udc * Idc * omegaE_inv * (float)(1.0 / 1.5 / MATLAB_PARA_faif);
}

#endif // COMP_H
