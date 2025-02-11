#ifndef KALMANFILT_W_H_
#define KALMANFILT_W_H_

#include "algo_code_config.h"
#include "thetaCal.h"
#include "filter.h"

struct KFw_struct
{
    float x_k_j[3];
    float x_k[3];
    float P_k_j[9];
    float P_k[9];

    float K[6];

    float Q_tr[3];
    float R_tr[2];

    float err[2];

    float R11Min;
    float R11Max;
    float R22Min;
    float R22Max;

    struct LPF_Ord1_2_struct R11LPF;
    struct LPF_Ord1_2_struct R22LPF;
};

static inline void KFw_init(struct KFw_struct* hKFw, float Q33, float R11Max, float R11Min, float R22Max, float R22Min)
{
    hKFw->x_k_j[0] = 0;
    hKFw->x_k_j[1] = 0;
    hKFw->x_k_j[2] = 0;

    hKFw->x_k[0] = 0;
    hKFw->x_k[1] = 0;
    hKFw->x_k[2] = 0;

    hKFw->P_k_j[0] = 0;
    hKFw->P_k_j[1] = 0;
    hKFw->P_k_j[2] = 0;
    hKFw->P_k_j[3] = 0;
    hKFw->P_k_j[4] = 0;
    hKFw->P_k_j[5] = 0;
    hKFw->P_k_j[6] = 0;
    hKFw->P_k_j[7] = 0;
    hKFw->P_k_j[8] = 0;

    hKFw->P_k[0] = (2.0 * M_PI * 2.0 * M_PI / 12.0);
    hKFw->P_k[1] = 0;
    hKFw->P_k[2] = 0;
    hKFw->P_k[3] = 0;
    hKFw->P_k[4] = 100;
    hKFw->P_k[5] = 0;
    hKFw->P_k[6] = 0;
    hKFw->P_k[7] = 0;
    hKFw->P_k[8] = 100;

    hKFw->K[0] = 0;
    hKFw->K[1] = 0;
    hKFw->K[2] = 0;
    hKFw->K[3] = 0;
    hKFw->K[4] = 0;
    hKFw->K[5] = 0;


    hKFw->R_tr[0] = R11Max;
    hKFw->R_tr[1] = R22Max;

    // Q_tr[0]用来记录原始值
    hKFw->Q_tr[0] = Q33;
    hKFw->Q_tr[1] = 0;
    // hKFw->Q_tr[2] = hKFw->Q_tr[0] * hKFw->R_tr[0] * hKFw->R_tr[1] / (hKFw->R_tr[0] + hKFw->R_tr[1]);
    hKFw->Q_tr[2] = Q33;

    hKFw->err[0] = 0;
    hKFw->err[1] = 0;

    hKFw->R11Min = R11Min;
    hKFw->R11Max = R11Max;
    hKFw->R22Min = R22Min;
    hKFw->R22Max = R22Max;

    // 截止频率定义为超参数
    LPF_Ord1_2_cfg(&hKFw->R11LPF, LPF_KAHAN_1_t, vCTRL_TS, 1, 0);
    LPF_Ord1_2_cfg(&hKFw->R22LPF, LPF_KAHAN_1_t, vCTRL_TS, 1, 0);

    LPF_Ord1_setVal(&hKFw->R11LPF, R11Max);
    LPF_Ord1_setVal(&hKFw->R22LPF, R22Max);

#ifdef TYJ_TEST
    hKFw->x_k[0] = M_PI / 3.0;
    hKFw->x_k[1] = 0;
    hKFw->x_k[2] = 0;
#endif
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(angle_norm_0_2pi)
#endif
static inline float angle_norm_0_2pi(float x)
{
    x *= (float)(1.0 / 2.0 / M_PI);
    return thetaCal_util_angle_norm(x) * (float)(2.0 * M_PI);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(angle_norm_negPi_pi)
#endif
static inline float angle_norm_negPi_pi(float x)
{
    x *= (float)(1.0 / 2.0 / M_PI);
    return thetaCal_util_angle_norm2(x) * (float)(2.0 * M_PI);
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(KFw_update)
#endif
static inline float KFw_update(struct KFw_struct* hKFw, float y1, float y2)
{
    // 该计算放在sigsamp部分
    // x_k_j=F*x_j;
    // hKFw->x_k_j[0] = hKFw->x_k[0] + vCTRL_TS * hKFw->x_k[1];
    hKFw->x_k_j[0] = angle_norm_0_2pi(hKFw->x_k[0] + vCTRL_TS * hKFw->x_k[1]);
    hKFw->x_k_j[1] = hKFw->x_k[1] + vCTRL_TS * hKFw->x_k[2];
    hKFw->x_k_j[2] = hKFw->x_k[2];

    // P_k_j=F*P_j*F.'+Q
    hKFw->P_k_j[0] = hKFw->P_k[0] + vCTRL_TS * (hKFw->P_k[1] + hKFw->P_k[3] + vCTRL_TS * hKFw->P_k[4])/* + hKFw->Q_tr[0] */;
    hKFw->P_k_j[1] = hKFw->P_k[1] + vCTRL_TS * (hKFw->P_k[2] + hKFw->P_k[4] + vCTRL_TS * hKFw->P_k[5]);
    hKFw->P_k_j[2] = hKFw->P_k[2] + vCTRL_TS * hKFw->P_k[5];
    hKFw->P_k_j[3] = hKFw->P_k[3] + vCTRL_TS * (hKFw->P_k[4] + hKFw->P_k[6] + vCTRL_TS * hKFw->P_k[7]);
    hKFw->P_k_j[4] = hKFw->P_k[4] + vCTRL_TS * (hKFw->P_k[5] + hKFw->P_k[7] + vCTRL_TS * hKFw->P_k[8]) + hKFw->Q_tr[1];
    hKFw->P_k_j[5] = hKFw->P_k[5] + vCTRL_TS * hKFw->P_k[8];
    hKFw->P_k_j[6] = hKFw->P_k[6] + vCTRL_TS * hKFw->P_k[7];
    hKFw->P_k_j[7] = hKFw->P_k[7] + vCTRL_TS * hKFw->P_k[8];
    hKFw->P_k_j[8] = hKFw->P_k[8] + hKFw->Q_tr[2];

    // K_k=P_k_j*H.'/(H*P_k_j*H.'+R)
    {
        float temp = 1.0f / (hKFw->P_k_j[0] * (hKFw->R_tr[0] + hKFw->R_tr[1]) + hKFw->R_tr[0] * hKFw->R_tr[1]);
        hKFw->K[0] = hKFw->P_k_j[0] * hKFw->R_tr[1] * temp;
        hKFw->K[1] = hKFw->P_k_j[0] * hKFw->R_tr[0] * temp;
        hKFw->K[2] = hKFw->P_k_j[3] * hKFw->R_tr[1] * temp;
        hKFw->K[3] = hKFw->P_k_j[3] * hKFw->R_tr[0] * temp;
        hKFw->K[4] = hKFw->P_k_j[6] * hKFw->R_tr[1] * temp;
        hKFw->K[5] = hKFw->P_k_j[6] * hKFw->R_tr[0] * temp;
    }

    // x_k=x_k_j+K_k*(y_k-H*x_k_j)
    {
        // hKFw->err[0] = y1 - hKFw->x_k_j[0];
        // hKFw->err[1] = y2 - hKFw->x_k_j[0];
        hKFw->err[0] = angle_norm_negPi_pi(y1 - hKFw->x_k_j[0]);
        hKFw->err[1] = angle_norm_negPi_pi(y2 - hKFw->x_k_j[0]);

        // hKFw->x_k[0] = hKFw->x_k_j[0] + err1 * hKFw->K[0] + err2 * hKFw->K[1];
        hKFw->x_k[0] = angle_norm_0_2pi(hKFw->x_k_j[0] + hKFw->err[0] * hKFw->K[0] + hKFw->err[1] * hKFw->K[1]);
        hKFw->x_k[1] = hKFw->x_k_j[1] + hKFw->err[0] * hKFw->K[2] + hKFw->err[1] * hKFw->K[3];
        hKFw->x_k[2] = hKFw->x_k_j[2] + hKFw->err[0] * hKFw->K[4] + hKFw->err[1] * hKFw->K[5];
    }

    // P_k=P_k_j-K_k*H*P_k_j
    {
        float temp1 = 1.0f - hKFw->K[0] - hKFw->K[1];
        float temp2 = hKFw->K[2] + hKFw->K[3];
        float temp3 = hKFw->K[4] + hKFw->K[5];

        hKFw->P_k[0] = hKFw->P_k_j[0] * temp1;
        hKFw->P_k[1] = hKFw->P_k_j[1] * temp1;
        hKFw->P_k[2] = hKFw->P_k_j[2] * temp1;
        hKFw->P_k[3] = hKFw->P_k_j[3] - hKFw->P_k_j[0] * temp2;
        hKFw->P_k[4] = hKFw->P_k_j[4] - hKFw->P_k_j[1] * temp2;
        hKFw->P_k[5] = hKFw->P_k_j[5] - hKFw->P_k_j[2] * temp2;
        hKFw->P_k[6] = hKFw->P_k_j[6] - hKFw->P_k_j[0] * temp3;
        hKFw->P_k[7] = hKFw->P_k_j[7] - hKFw->P_k_j[1] * temp3;
        hKFw->P_k[8] = hKFw->P_k_j[8] - hKFw->P_k_j[2] * temp3;
    }

    // 自适应更新噪声

    hKFw->R_tr[0] = LPF_Ord1_setVal(&hKFw->R11LPF, __fmin(__fmax(LPF_Ord1_update_kahan(&hKFw->R11LPF, hKFw->err[0] * hKFw->err[0]), hKFw->R11Min), hKFw->R11Max));
    hKFw->R_tr[1] = LPF_Ord1_setVal(&hKFw->R22LPF, __fmin(__fmax(LPF_Ord1_update_kahan(&hKFw->R22LPF, hKFw->err[1] * hKFw->err[1]), hKFw->R22Min), hKFw->R22Max));
    // hKFw->Q_tr[2] = hKFw->Q_tr[0] * hKFw->R_tr[0] * hKFw->R_tr[1] / (hKFw->R_tr[0] + hKFw->R_tr[1]);

    return hKFw->x_k[0];
}

#endif
