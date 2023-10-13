/*
 * adrc.h
 *
 *  Created on: 2023年6月19日
 *      Author: t
 */

#ifndef ALGO_CODE_INC_ADRC_H_
#define ALGO_CODE_INC_ADRC_H_


#include "stdint.h"
#include "math.h"
#include "algo_code_config.h"

struct ADRC_struct
{
    float para_gamma;
    float para_cu1;
    float para_cu2;
    float para_beta1;
    float para_beta2;
    float para_TeMax;
    float para_TeMin;
    float para_SMCk;

    float x1;

    float e_u;
    float e1;

    float z1;
    float z2;

    float out_pi;
    float out_smc;
    float out_eso;
    float Te;
    float u;

    float pi_intg;
    float b0;
    float b0_inv;

    float b0_k;
};

static inline void adrc_init(struct ADRC_struct* hADRC, float gamma, float cu1, float cu2, float beta1, float beta2, float TeMax, float TeMin, float SMCk)
{
    hADRC->para_gamma = gamma;
    hADRC->para_cu1 = cu1;
    hADRC->para_cu2 = cu2;
    hADRC->para_beta1 = beta1;
    hADRC->para_beta2 = beta2;
    hADRC->para_TeMax = TeMax;
    hADRC->para_TeMin = TeMin;
    hADRC->para_SMCk = SMCk;

    hADRC->x1 = 0;
    hADRC->e_u = 0;
    hADRC->e1 = 0;
    hADRC->z1 = 0;
    hADRC->z2 = 0;
    hADRC->b0 = 0;
    hADRC->b0_inv = 0;
    hADRC->out_pi = 0;
    hADRC->out_smc = 0;
    hADRC->out_eso = 0;
    hADRC->Te = 0;
    hADRC->u = 0;
    hADRC->pi_intg = 0;

    hADRC->b0_k = 1.0f;
}

static inline float adrc_update(struct ADRC_struct* hADRC, float UdcRef, float UdcFbk, float omega_m)
{
    hADRC->b0 = omega_m * ((float)(2.0f / MATLAB_PARA_C_dclink)) * hADRC->b0_k;
    hADRC->b0_inv = 1.0f / hADRC->b0;
    hADRC->x1 = UdcFbk * UdcFbk;
    hADRC->e_u = hADRC->x1 - UdcRef * UdcRef;

    hADRC->pi_intg += hADRC->para_cu1 * hADRC->e_u * ((float)(1.0f / MATLAB_PARA_ctrl_freq));
    hADRC->out_pi = (hADRC->pi_intg + hADRC->e_u) * hADRC->para_gamma;

    float adrc_pi_sign = hADRC->out_pi >= 0.0f ? 1.0f : -1.0f;
    hADRC->out_smc = fabsf(hADRC->e_u) * hADRC->para_cu2 * hADRC->b0_inv * adrc_pi_sign;

    hADRC->e1 = hADRC->x1 - hADRC->z1;

    hADRC->z2 += (hADRC->e1 * hADRC->para_beta2) * ((float)(1.0f / MATLAB_PARA_ctrl_freq));

    hADRC->out_eso = hADRC->z2 * hADRC->b0_inv;

    hADRC->Te = hADRC->out_pi + hADRC->out_smc + hADRC->out_eso;
    hADRC->u = __fmax(__fmin(hADRC->Te, hADRC->para_TeMax), hADRC->para_TeMin);

    hADRC->z1 += (hADRC->z2 - hADRC->u * hADRC->b0 + hADRC->e1 * hADRC->para_beta1) * ((float)(1.0f / MATLAB_PARA_ctrl_freq));

    return hADRC->u;
}

static inline float adrc_update2(struct ADRC_struct* hADRC, float UdcRef, float UdcFbk, float omega_m)
{
    hADRC->b0 = omega_m * ((float)(2.0f / MATLAB_PARA_C_dclink));
    hADRC->b0_inv = 1.0f / hADRC->b0;
    hADRC->x1 = UdcFbk * UdcFbk;
    hADRC->e_u = hADRC->x1 - UdcRef * UdcRef;

    hADRC->pi_intg += hADRC->para_cu1 * hADRC->e_u * ((float)(1.0f / MATLAB_PARA_ctrl_freq));
    hADRC->out_pi = (hADRC->pi_intg + hADRC->e_u) * hADRC->para_gamma;

    float adrc_pi_sign = hADRC->out_pi >= 0.0f ? 1.0f : -1.0f;
    // 下一行简化
    hADRC->out_smc = hADRC->para_SMCk * hADRC->b0_inv * adrc_pi_sign;

    hADRC->e1 = hADRC->x1 - hADRC->z1;

    hADRC->z2 += (hADRC->e1 * hADRC->para_beta2) * ((float)(1.0f / MATLAB_PARA_ctrl_freq));

    hADRC->out_eso = hADRC->z2 * hADRC->b0_inv;

    // 下一行少加一项
    hADRC->Te = hADRC->out_pi + hADRC->out_smc;
    hADRC->u = __fmax(__fmin(hADRC->Te, hADRC->para_TeMax), hADRC->para_TeMin);

    hADRC->z1 += (hADRC->z2 - hADRC->u * hADRC->b0 + hADRC->e1 * hADRC->para_beta1) * ((float)(1.0f / MATLAB_PARA_ctrl_freq));

    return hADRC->u;
}

static inline void adrc_clear(struct ADRC_struct* hADRC)
{
    hADRC->z1 = 0;
    hADRC->z2 = 0;
    hADRC->pi_intg = 0;
}


#endif /* ALGO_CODE_INC_ADRC_H_ */
