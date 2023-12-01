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

#include "filter.h"

 /// 用0-1代表0-2pi角度
enum OECA_Sta
{
    OECA_stop = 0,
    OECA_Isweep0,
    OECA_Isweep1,
    OECA_algo0,
    OECA_algo1,
};

struct OECA_struct
{
    // PI
    float  kp_eComp;
    float  ki_eComp;
    float  intg_eComp;

    float theta_eComp;

    // PLL
    float  kp_pll;
    float  ki_pll;
    float  intg_pll;

    float theta_pll;

    float lastThetaIn;
    float omegaOB;

    float lastOmegaOB;
    float alphaOB;

    // I-THETA SWEEP
    float sweepCurrent;

    float* dbgBuffPos;

    int32_t sweepCnt;
    int32_t sweepMax;

    int sampDiv;
    int sampCnt;

    // control sta
    int status;

    struct LPF_Ord1_2_struct spdFilt1;
    struct LPF_Ord1_2_struct spdFilt2;
};

struct Goertz_struct
{
    int k;
    int N;

    float cosVal;
    float sinVal;

    float wTab[3];

    float AnsImag;
    float AnsReal;

    float AnsAbs;
    float AnsArg;
};

static inline void OECA_init(struct OECA_struct* hOECA)
{
    hOECA->kp_eComp = 0.715;
    hOECA->ki_eComp = 5.93e-4;
    hOECA->intg_eComp = 0;

    hOECA->theta_eComp = 0;

    hOECA->kp_pll = 444.3;
    hOECA->ki_pll = 3.29;
    hOECA->intg_pll = 0;

    hOECA->theta_pll = 0;

    hOECA->sampCnt = 0;
    hOECA->sampDiv = 40 * 5;

    hOECA->sweepCurrent = 30;
    hOECA->sweepCnt = 0;
    hOECA->sweepMax = 100 / 5;

    hOECA->dbgBuffPos = (void*)0xC000ul;

    hOECA->lastThetaIn = 0;
    hOECA->omegaOB = 0;
    hOECA->lastOmegaOB = 0;
    hOECA->alphaOB = 0;


    // control sta
    hOECA->status = OECA_stop;

#define LPF_2Ord_200Hz_T30000Hz_ksi0_70_PARA 0.0017038908F, -1.9413394F, 0.94304332F

    LPF_Ord1_2_init(&hOECA->spdFilt1, LPF_2Ord_200Hz_T30000Hz_ksi0_70_PARA);
    LPF_Ord1_2_init(&hOECA->spdFilt2, LPF_2Ord_200Hz_T30000Hz_ksi0_70_PARA);

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
    hOECA->intg_pll = hOECA->intg_pll + deltaTheta * hOECA->ki_pll;
    float outPll = deltaTheta * hOECA->kp_pll + hOECA->intg_pll;
    hOECA->theta_pll = OECA_util_angle_norm(hOECA->theta_pll + outPll * (float)(1.0 / CTRL_FREQ));
    return hOECA->theta_pll;
}

static inline float OECA_PICalc(struct OECA_struct* hOECA, float omegaIn)
{
    hOECA->intg_eComp = OECA_util_angle_norm(hOECA->intg_eComp + omegaIn * hOECA->ki_eComp);
    hOECA->theta_eComp = OECA_util_angle_norm(hOECA->intg_eComp + omegaIn * hOECA->kp_eComp);
    return OECA_util_angle_norm(hOECA->theta_eComp + 0.5f);
}

static inline float OECA_omegaOB(struct OECA_struct* hOECA, float thetaIn)
{
    float omegaDiffed = OECA_util_angle_norm2(thetaIn - hOECA->lastThetaIn) * (float)(CTRL_FREQ * MATLAB_PARA_pi2 / 65536.0);
    hOECA->lastThetaIn = thetaIn;
    hOECA->omegaOB = LPF_Ord2_update_kahan(hOECA->spdFilt1, omegaDiffed);
    return hOECA->omegaOB;
}

static inline float OECA_alphaOB(struct OECA_struct* hOECA)
{

}

static inline void Goertz_init(struct Goertz_struct* hGeotz, int k, int N)
{
    hGeotz->k = k;
    hGeotz->N = N;

    float N_1 = __divf32(1.0f, (float)hGeotz->N);

    hGeotz->sinVal = sinf((float)(MATLAB_PARA_pi2) * (float)k * (float)N_1);
    hGeotz->cosVal = cosf((float)(MATLAB_PARA_pi2) * (float)k * (float)N_1);

    hGeotz->wTab[0] = 0;
    hGeotz->wTab[1] = 0;
    hGeotz->wTab[2] = 0;

    hGeotz->AnsImag = 0;
    hGeotz->AnsReal = 0;
    hGeotz->AnsAbs = 0;
    hGeotz->AnsReal = 0;

}

static inline float Goertz_iter(struct Goertz_struct* hGeotz, float inVal)
{
    hGeotz->wTab[0] = inVal + 2.0f * hGeotz->cosVal * hGeotz->wTab[1] - hGeotz->wTab[2];
    hGeotz->wTab[2] = hGeotz->wTab[1];
    hGeotz->wTab[1] = hGeotz->wTab[0];

    return hGeotz->wTab[0];
}

static inline void Goertz_getAns(struct Goertz_struct* hGeotz)
{
    float N_2 = __divf32(2.0f, (float)hGeotz->N);
    hGeotz->AnsImag = N_2 * hGeotz->sinVal * hGeotz->wTab[1];
    hGeotz->AnsReal = N_2 * (hGeotz->cosVal * hGeotz->wTab[1] - hGeotz->wTab[2]);

    hGeotz->AnsArg = __atan2puf32(hGeotz->AnsImag, hGeotz->AnsReal);
    hGeotz->AnsAbs = sqrtf(hGeotz->AnsImag * hGeotz->AnsImag + hGeotz->AnsReal * hGeotz->AnsReal);
}

#endif /* SHARE_ALGO_OECA_H_ */
