#ifndef INCX_THETACAL_H_
#define INCX_THETACAL_H_

struct ThataCal_struct
{
    float theta;
    float thetaCos;
    float thetaSin;

    int16_t ifNeedTriCal;
};

#include "math.h"
#include "algo_code_config.h"

static inline void _calSinCos(struct ThataCal_struct* hThetaCal)
{
    hThetaCal->thetaSin = __sinpuf32(hThetaCal->theta);
    hThetaCal->thetaCos = __cospuf32(hThetaCal->theta);
    hThetaCal->ifNeedTriCal = 0;
}

static inline void thetaCal_init(struct ThataCal_struct* hThetaCal)
{
    hThetaCal->theta = 0;
    hThetaCal->thetaSin = 0;
    hThetaCal->thetaCos = 1;
    hThetaCal->ifNeedTriCal = 0;
}

static inline void thetaCal_setTheta(struct ThataCal_struct* hThetaCal, float theta)
{
    hThetaCal->theta = theta - floorf(theta);;
    hThetaCal->ifNeedTriCal = 1;
}

// 归一化到 0-1
static inline float thetaCal_util_angle_norm(float angle)
{
    return angle - floorf(angle);
}

static inline void thetaCal_setTheta_Uint(struct ThataCal_struct* hThetaCal, uint16_t theta_uint)
{
    hThetaCal->theta = theta_uint * (1.0f / 65536.0f);
    hThetaCal->ifNeedTriCal = 1;
}

static inline void thetaCal_setTheta_noWrap(struct ThataCal_struct* hThetaCal, float theta)
{
    hThetaCal->theta = theta;
    hThetaCal->ifNeedTriCal = 1;
}

static inline float thetaCal_getSinVal(struct ThataCal_struct* hThetaCal)
{
    if (hThetaCal->ifNeedTriCal)
    {
        _calSinCos(hThetaCal);
    }
    return hThetaCal->thetaSin;
}

static inline float thetaCal_getCosVal(struct ThataCal_struct* hThetaCal)
{
    if (hThetaCal->ifNeedTriCal)
    {
        _calSinCos(hThetaCal);
    }
    return hThetaCal->thetaCos;
}

static inline float thetaCal_getTheta(struct ThataCal_struct* hThetaCal)
{
    return hThetaCal->theta;
}

#endif /* INCX_THETACAL_H_ */
