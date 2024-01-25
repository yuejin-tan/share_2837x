/*
 * deadBandComp.h
 *
 *  Created on: 2023年5月19日
 *      Author: t
 */

#ifndef ALGO_CODE_INC_DEADBANDCOMP_H_
#define ALGO_CODE_INC_DEADBANDCOMP_H_

#include "SVPWM.h"

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



#endif /* ALGO_CODE_INC_DEADBANDCOMP_H_ */
