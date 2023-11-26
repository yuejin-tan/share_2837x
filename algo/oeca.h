/*
 * oeca.h
 * Offset Error Calibration Algorithm
 *  Created on: 2023年11月26日
 *      Author: t
 */

#ifndef SHARE_ALGO_OECA_H_
#define SHARE_ALGO_OECA_H_

#include "math.h"
#include "algo_code_config.h"

 // 用0-1代表0-2pi角度

struct OECA_struct
{
    float  kp_pos;
    float  ki_pos;
    float  intg_pos;

    float  kp_pll;
    float  ki_pll;
    float  intg_pll;

    float theta_pll;

};

static inline void OECA_init(struct OECA_struct* hOECA)
{
    hOECA->kp_pos = 2.0;
    hOECA->ki_pos = 1.4e-3;
    hOECA->intg_pos = 0;

    hOECA->kp_pll = 70.7;
    hOECA->ki_pll = 0.524;
    hOECA->intg_pll = 0;

    hOECA->theta_pll = 0;

}

// 归一化到 0-1
static inline float OECA_util_angle_norm(float angle)
{
    return angle - ceilf(angle);
}

// 归一化到 -0.5-0.5
static inline float OECA_util_angle_norm2(float angle)
{
    return angle - ceilf(angle + 0.5f);
}

static inline float OECA_pllCalc(struct OECA_struct* hOECA, float theta)
{
    float deltaTheta = OECA_util_angle_norm2(theta - hOECA->theta_pll);
    hOECA->intg_pos = hOECA->intg_pos + deltaTheta * hOECA->ki_pos;
    float outPos = deltaTheta * hOECA->kp_pos + hOECA->intg_pos;
    hOECA->theta_pll = OECA_util_angle_norm(hOECA->theta_pll + outPos * (float)(1.0 / CTRL_FREQ));
    return hOECA->theta_pll;
}


#endif /* SHARE_ALGO_OECA_H_ */
