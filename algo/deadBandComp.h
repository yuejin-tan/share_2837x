/*
 * deadBandComp.h
 *
 *  Created on: 2023年5月19日
 *      Author: t
 */

#ifndef ALGO_CODE_INC_DEADBANDCOMP_H_
#define ALGO_CODE_INC_DEADBANDCOMP_H_

#include "algo_code_config.h"
#include "SVPWM.h"
#include "transform.h"

static inline void dbComp(struct SVPWM_struct* hSVPWM, float thetaI_add30degPu, int compVal)
{
    int idx = thetaI_add30degPu * 6.0f;
    switch (idx)
    {
        // 考虑计算误差
    case 6:
    case 0:
        hSVPWM->epwmU -= compVal;
        hSVPWM->epwmV += compVal;
        hSVPWM->epwmW += compVal;
        break;
    case 1:
        hSVPWM->epwmU -= compVal;
        hSVPWM->epwmV -= compVal;
        hSVPWM->epwmW += compVal;
        break;
    case 2:
        hSVPWM->epwmU += compVal;
        hSVPWM->epwmV -= compVal;
        hSVPWM->epwmW += compVal;
        break;
    case 3:
        hSVPWM->epwmU += compVal;
        hSVPWM->epwmV -= compVal;
        hSVPWM->epwmW -= compVal;
        break;
    case 4:
        hSVPWM->epwmU += compVal;
        hSVPWM->epwmV += compVal;
        hSVPWM->epwmW -= compVal;
        break;
    case 5:
    case -1:
        hSVPWM->epwmU -= compVal;
        hSVPWM->epwmV += compVal;
        hSVPWM->epwmW -= compVal;
        break;
    }
}

static inline float dbComp2_UdcOffset(float Udc, float offset)
{
    return __fmax(Udc - offset, 1.0f);
}

static inline void dbComp2_up(struct SVPWM_struct* hSVPWM, struct Trans_struct* hIuvw, float Ithd_1, float compVal)
{
    hSVPWM->epwmU -= (int16_t)(__fmin(__fmax(hIuvw->U * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
    hSVPWM->epwmV -= (int16_t)(__fmin(__fmax(hIuvw->V * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
    hSVPWM->epwmW -= (int16_t)(__fmin(__fmax(hIuvw->W * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
}

static inline void dbComp2_down(struct SVPWM_struct* hSVPWM, struct Trans_struct* hIuvw, float Ithd_1, float compVal)
{
    hSVPWM->epwmU -= (int16_t)(__fmin(__fmax(hIuvw->U * Ithd_1, -1.0f), 0.0f) * compVal + 0.5f);
    hSVPWM->epwmV -= (int16_t)(__fmin(__fmax(hIuvw->V * Ithd_1, -1.0f), 0.0f) * compVal + 0.5f);
    hSVPWM->epwmW -= (int16_t)(__fmin(__fmax(hIuvw->W * Ithd_1, -1.0f), 0.0f) * compVal + 0.5f);
}

static inline void dbComp2_all(struct SVPWM_struct* hSVPWM, struct Trans_struct* hIuvw, float Ithd_1, float compVal)
{
    hSVPWM->epwmU -= (int16_t)(__fmin(__fmax(hIuvw->U * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
    hSVPWM->epwmV -= (int16_t)(__fmin(__fmax(hIuvw->V * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
    hSVPWM->epwmW -= (int16_t)(__fmin(__fmax(hIuvw->W * Ithd_1, 0.0f), 1.0f) * compVal + 0.5f);
}


#endif /* ALGO_CODE_INC_DEADBANDCOMP_H_ */
