#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "thetaCal.h"

struct Trans_struct
{
    float U;
    float V;
    float W;

    float al;
    float be;

    float d;
    float q;

    float abdq0;
};

// -------- 3 ----------

static inline void trans3_uvw2albe0(struct Trans_struct* hTrans)
{
    const float one_div_3 = 1.0 / 3.0;
    const float one_div_sqrt_3 = 0.57773502692;
    hTrans->abdq0 = (hTrans->U + hTrans->V + hTrans->W) * one_div_3;
    hTrans->al = hTrans->U - hTrans->abdq0;
    hTrans->be = (hTrans->V - hTrans->W) * one_div_sqrt_3;
}

static inline void trans3_albe02dq0(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    float sinVal = thetaCal_getSinVal(hTheta);
    float cosVal = thetaCal_getCosVal(hTheta);
    hTrans->d = cosVal * hTrans->al + sinVal * hTrans->be;
    hTrans->q = -sinVal * hTrans->al + cosVal * hTrans->be;
}

static inline void trans3_uvw2dq0(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans3_uvw2albe0(hTrans);
    trans3_albe02dq0(hTrans, hTheta);
}

static inline void trans3_dq02albe0(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    float sinVal = thetaCal_getSinVal(hTheta);
    float cosVal = thetaCal_getCosVal(hTheta);
    hTrans->al = cosVal * hTrans->d - sinVal * hTrans->q;
    hTrans->be = sinVal * hTrans->d + cosVal * hTrans->q;
}

static inline void trans3_albe02uvw(struct Trans_struct* hTrans)
{
    const float one_div_2 = 1.0 / 2.0;
    const float sqrt_3_div_2 = 0.866025404;
    hTrans->U = hTrans->al + hTrans->abdq0;
    hTrans->V = -one_div_2 * hTrans->al + sqrt_3_div_2 * hTrans->be + hTrans->abdq0;
    hTrans->W = -one_div_2 * hTrans->al - sqrt_3_div_2 * hTrans->be + hTrans->abdq0;
}

static inline void trans3_dq02uvw(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans3_dq02albe0(hTrans, hTheta);
    trans3_albe02uvw(hTrans);
}

// -------- 2 ----------

static inline void trans2_uv2albe(struct Trans_struct* hTrans)
{
    const float one_div_sqrt_3 = 0.57773502692;
    const float two_div_sqrt_3 = 1.154700538;
    hTrans->al = hTrans->U;
    hTrans->be = hTrans->U * one_div_sqrt_3 + hTrans->V * two_div_sqrt_3;
}

static inline void trans2_uw2albe(struct Trans_struct* hTrans)
{
    const float one_div_sqrt_3 = 0.57773502692;
    const float two_div_sqrt_3 = 1.154700538;
    hTrans->al = hTrans->U;
    hTrans->be = -hTrans->U * one_div_sqrt_3 - hTrans->W * two_div_sqrt_3;
}

static inline void trans2_vw2albe(struct Trans_struct* hTrans)
{
    const float one_div_sqrt_3 = 0.57773502692;
    hTrans->al = -hTrans->V - hTrans->W;
    hTrans->be = (hTrans->V - hTrans->W) * one_div_sqrt_3;
}

static inline void trans2_albe2dq(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans3_albe02dq0(hTrans, hTheta);
}

static inline void trans2_uv2dq(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans2_uv2albe(hTrans);
    trans2_albe2dq(hTrans, hTheta);
}

static inline void trans2_uw2dq(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans2_uw2albe(hTrans);
    trans2_albe2dq(hTrans, hTheta);
}

static inline void trans2_vw2dq(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans2_vw2albe(hTrans);
    trans2_albe2dq(hTrans, hTheta);
}

static inline void trans2_dq2albe(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans3_dq02albe0(hTrans, hTheta);
}

static inline void trans2_albe2uvw(struct Trans_struct* hTrans)
{
    const float one_div_2 = 1.0 / 2.0;
    const float sqrt_3_div_2 = 0.866025404;
    hTrans->U = hTrans->al;
    hTrans->V = -one_div_2 * hTrans->al + sqrt_3_div_2 * hTrans->be;
    hTrans->W = -one_div_2 * hTrans->al - sqrt_3_div_2 * hTrans->be;
}

static inline void trans2_dq2uvw(struct Trans_struct* hTrans, struct ThataCal_struct* hTheta)
{
    trans2_dq2albe(hTrans, hTheta);
    trans2_albe2uvw(hTrans);
}


#endif // TRANSFORM_H
