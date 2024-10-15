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
    OECA_ready = 0,
    OECA_Isweep0,
    OECA_Isweep1,
    OECA_algo0,
    OECA_stop,
};

#define OECA_M_AUTOALGO 1
#define OECA_M_AUTO_PI_PARA 2
#define OECA_M_AUTO_INIT_VAL 4
#define OECA_M_AUTO_GOBACK 8

struct OECA_struct
{
    // PI
    float  kp_eComp;
    float  ki_eComp;
    float  intg_eComp;

    float theta_eComp;

    float T_sigma_eComp;
    float h_eComp;
    float J_coeff;

    float PICurrent;

    float maxOmegaMThd;

    // PLL
    float  kp_pll;
    float  ki_pll;
    float  intg_pll;

    float theta_pll;

    float lastThetaIn;
    float omegaOB;

    // I-THETA SWEEP
    float sweepCurrent;
    float sweepTime;

    float estAns1;

    int sweepCnt;
    int sweepMax;

    int sampDiv;
    int sampCnt;

    int32_t timeCnt;
    int32_t timeCntMax;

    // debug
    float* hDbgBuff;
    int dbgDiv;
    int dbgCnt;
    int dbgPtr;

    // control sta
    int status;
    int modeCfg;

    struct LPF_Ord1_2_struct* hFilter1;
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

static inline void OECA_init(struct OECA_struct* hOECA, struct LPF_Ord1_2_struct* hFilter1)
{
    hOECA->kp_eComp = 0.1;
    hOECA->ki_eComp = 2e-4;
    hOECA->intg_eComp = 0;

    hOECA->theta_eComp = 0;

    hOECA->T_sigma_eComp = 1.0f / 2.0f / M_PI / (100.0f * 0.5f) + 1.0f / 6000.0f * 5.0f + 1.0f / 300.0f;
    hOECA->h_eComp = 10.0f;
    hOECA->J_coeff = 1.0f;
    hOECA->PICurrent = 15.0f;
    hOECA->maxOmegaMThd = 0.05f;

    hOECA->kp_pll = 177.0f;
    hOECA->ki_pll = 0.53f;
    hOECA->intg_pll = 0;

    hOECA->theta_pll = 0;

    hOECA->lastThetaIn = 0;
    hOECA->omegaOB = 0;

    hOECA->sweepCurrent = 15.0f;
    hOECA->sweepTime = 1.0f;

    hOECA->estAns1 = 0;

    hOECA->sweepCnt = 0;
    hOECA->sweepMax = 100;

    hOECA->sampCnt = 0;
    hOECA->sampDiv = 30e3f * hOECA->sweepTime / hOECA->sweepMax + 0.5f;

    hOECA->timeCnt = 0;
    hOECA->timeCntMax = 30e3f * hOECA->sweepTime + 0.5f;

    hOECA->hDbgBuff = (float*)(void*)0xC000ul;
    hOECA->dbgDiv = 30;
    hOECA->dbgCnt = 0;
    hOECA->dbgPtr = 0;

    hOECA->status = OECA_ready;
    hOECA->modeCfg = 0;

    hOECA->hFilter1 = hFilter1;

#define LPF_2Ord_200Hz_T30000Hz_ksi0_70_PARA 0.0017038908F, -1.9413394F, 0.94304332F

    LPF_Ord1_2_init(hOECA->hFilter1, LPF_2Ord_200Hz_T30000Hz_ksi0_70_PARA);

}

// 归一化到 0-1
static inline float OECA_util_angle_norm(float angle)
{
    return angle - floorf(angle);
}

// 归一化到 -0.5-0.5
static inline float OECA_util_angle_norm2(float angle)
{
    return angle - floorf(angle + 0.5f);
}

static inline float OECA_pllCalc(struct OECA_struct* hOECA)
{
    float deltaTheta = OECA_util_angle_norm2(hOECA->theta_eComp - hOECA->theta_pll);
    hOECA->intg_pll = hOECA->intg_pll + deltaTheta * hOECA->ki_pll;
    float outPll = deltaTheta * hOECA->kp_pll + hOECA->intg_pll;
    hOECA->theta_pll = OECA_util_angle_norm(hOECA->theta_pll + outPll * vCTRL_TS);
    return hOECA->theta_pll;
}

static inline float OECA_PICalc(struct OECA_struct* hOECA)
{
    hOECA->intg_eComp = OECA_util_angle_norm(hOECA->intg_eComp - hOECA->omegaOB * hOECA->ki_eComp);
    hOECA->theta_eComp = OECA_util_angle_norm(hOECA->intg_eComp - hOECA->omegaOB * hOECA->kp_eComp);
    return OECA_util_angle_norm(0.5f - hOECA->theta_eComp);
}

static inline void OECA_algoStaSet(struct OECA_struct* hOECA, float initVal)
{
    hOECA->intg_eComp = initVal;
    hOECA->theta_eComp = initVal;
    hOECA->intg_pll = 0.0f;
    hOECA->theta_pll = initVal;
}

static inline float OECA_getSweepTheta(struct OECA_struct* hOECA)
{
    return __divf32((float)hOECA->timeCnt, (float)hOECA->timeCntMax);
}

static inline float OECA_omegaOB(struct OECA_struct* hOECA, float thetaIn)
{
    float omegaRaw = OECA_util_angle_norm2(thetaIn - hOECA->lastThetaIn) * vCTRL_FREQ * (float)(M_PI * 2.0);
    hOECA->lastThetaIn = thetaIn;
    hOECA->omegaOB = LPF_Ord2_update(hOECA->hFilter1, omegaRaw);
    return hOECA->omegaOB;
}

static inline void Goertz_init(struct Goertz_struct* hGeotz, int k, int N)
{
    hGeotz->k = k;
    hGeotz->N = N;

    float k_N = __divf32((float)k, (float)N);

    hGeotz->sinVal = __sinpuf32(k_N);
    hGeotz->cosVal = __cospuf32(k_N);

    hGeotz->wTab[0] = 0;
    hGeotz->wTab[1] = 0;
    hGeotz->wTab[2] = 0;

}

static inline void Goertz_reset(struct Goertz_struct* hGeotz)
{
    hGeotz->wTab[0] = 0;
    hGeotz->wTab[1] = 0;
    hGeotz->wTab[2] = 0;
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
