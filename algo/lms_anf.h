#pragma once

#include "stdint.h"
#include "algo_code_config.h"
#include "math.h"

struct LMSanf_struct
{
    float W[6];

    float mu;
};

static inline void LMSanfInit(struct LMSanf_struct* hLMSanf)
{
    hLMSanf->W[0] = 0;
    hLMSanf->W[1] = 0;
    hLMSanf->W[2] = 0;
    hLMSanf->W[3] = 0;
    hLMSanf->W[4] = 0;
    hLMSanf->W[5] = 0;

    hLMSanf->mu = 0.001;
}

static inline float LMSanfUpdate(struct LMSanf_struct* hLMSanf, uint16_t angle, float input)
{
    float thetaPU1 = ((uint16_t)(angle * 1)) * (float)(1.0f / 65536.0f);
    float thetaPU2 = ((uint16_t)(angle * 2)) * (float)(1.0f / 65536.0f);
    float thetaPU4 = ((uint16_t)(angle * 4)) * (float)(1.0f / 65536.0f);
    // [cos1 sin1 cos2 sin2 cos4 sin4]
    float X[] = { __cospuf32(thetaPU1),__sinpuf32(thetaPU1),__cospuf32(thetaPU2),__sinpuf32(thetaPU2),__cospuf32(thetaPU4),__sinpuf32(thetaPU4), };

    float y = 0;

    // 若在CCS环境中，提示编译器展开循环
#ifndef TYJ_TEST
#pragma UNROLL(6)
#endif
    for (int i = 0;i < 6;i++)
    {
        y += X[i] * hLMSanf->W[i];
    }

    // 若在CCS环境中，提示编译器展开循环
#ifndef TYJ_TEST
#pragma UNROLL(6)
#endif
    for (int i = 0;i < 6;i++)
    {
        hLMSanf->W[i] += hLMSanf->mu * (input - y) * X[i];
    }

    return y;
}

static inline uint16_t thetaHarComp(struct LMSanf_struct* hLMSanf, uint16_t angle)
{
    float thetaPU1 = ((uint16_t)(angle * 1)) * (float)(1.0f / 65536.0f);
    float thetaPU2 = ((uint16_t)(angle * 2)) * (float)(1.0f / 65536.0f);
    float thetaPU4 = ((uint16_t)(angle * 4)) * (float)(1.0f / 65536.0f);
    // [cos1 sin1 cos2 sin2 cos4 sin4]
    float X[] = { __cospuf32(thetaPU1),__sinpuf32(thetaPU1),__cospuf32(thetaPU2),__sinpuf32(thetaPU2),__cospuf32(thetaPU4),__sinpuf32(thetaPU4), };

    float y = 0;
    // 若在CCS环境中，提示编译器展开循环
#ifndef TYJ_TEST
#pragma UNROLL(6)
#endif
    for (int i = 0;i < 6;i++)
    {
        y += X[i] * hLMSanf->W[i];
    }

    y *= (float)(65536.0 / M_PI / 2.0);

    return y;
}

static inline float harmInjUtil(uint16_t anglPuQ16, uint16_t order, float Acos, float Asin)
{
    float ang = ((uint16_t)(anglPuQ16 * order)) * (float)(1.0f / 65536.0f);
    return Acos * __cospuf32(ang) + Asin * __sinpuf32(ang);
}
