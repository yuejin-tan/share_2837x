#ifndef MRASWR_H_
#define MRASWR_H_

#include "algo_code_config.h"

#include "pid.h"
#include "transform.h"
#include "filter.h"
#include "thetaCal.h"

struct MRASwr_struct
{
    float We;
    float Rs;
    float thetaOut;

    float WeErr;
    float RsErr;

    float lpfW;
    float lpfGain;

    float Is2Min;

    struct Trans_struct Psi_I;
    struct Trans_struct Psi_U;
    struct Trans_struct Ucomp;

    struct ThataCal_struct thetaE;
    struct ThataCal_struct thetaLPF;

    struct LPF_Ord1_2_struct PalLPF;
    struct LPF_Ord1_2_struct PbeLPF;
    struct LPF_Ord1_2_struct compLPF;

    struct PIctrl_struct* wPI;
    struct PIctrl_struct* RsPI;

    // struct PIctrl_struct* PsiPI1;
    // struct PIctrl_struct* PsiPI2;
};

static inline void MRAS_wr_init(struct MRASwr_struct* hMRAS, struct PIctrl_struct* hwPI, struct PIctrl_struct* hRsPI, float lpfWc, float Rs0, float minIs24Rs)
{

    hMRAS->Rs = Rs0;
    // f_c
    hMRAS->lpfW = lpfWc;
    // 1/Omega_c
    hMRAS->lpfGain = 1.0f / hMRAS->lpfW;

    hMRAS->Is2Min = minIs24Rs;

    hMRAS->WeErr = 0;
    hMRAS->RsErr = 0;

    hMRAS->Psi_I.abdq0 = 0;
    hMRAS->Psi_I.al = 0;
    hMRAS->Psi_I.be = 0;
    hMRAS->Psi_I.d = 0;
    hMRAS->Psi_I.q = 0;

    hMRAS->Psi_U.abdq0 = 0;
    hMRAS->Psi_U.al = 0;
    hMRAS->Psi_U.be = 0;
    hMRAS->Psi_U.d = 0;
    hMRAS->Psi_U.q = 0;

    hMRAS->We = 0;
    thetaCal_init(&hMRAS->thetaE);
    thetaCal_init(&hMRAS->thetaLPF);

#ifdef TYJ_TEST
    // 仿真时加入的初始角度误差
    thetaCal_setTheta(&hMRAS->thetaE, 0.4);
#endif

    LPF_Ord1_2_cfg(&hMRAS->PalLPF, LPF_KAHAN_1_t, vCTRL_TS, hMRAS->lpfW * (float)(1.0f / 2.0 / M_PI), 0);
    LPF_Ord1_2_cfg(&hMRAS->PbeLPF, LPF_KAHAN_1_t, vCTRL_TS, hMRAS->lpfW * (float)(1.0f / 2.0 / M_PI), 0);
    // 稍大于另两者
    LPF_Ord1_2_cfg(&hMRAS->compLPF, LPF_ORD_1_t, vCTRL_TS, hMRAS->lpfW, 0);

    hMRAS->wPI = hwPI;
    hMRAS->RsPI = hRsPI;
    // 电阻给初值，需在指针赋值后进行
    PIctrl_setIntg(hMRAS->RsPI, hMRAS->Rs);

}


#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(MRAS_wr_update)
#endif
static inline float MRAS_wr_update(struct MRASwr_struct* hMRAS, struct Trans_struct* Us, struct Trans_struct* Is)
{
    // 该计算放在sigsamp部分

    // thetaOut已在上周期算出
    hMRAS->thetaOut = thetaCal_getTheta(&hMRAS->thetaE);

    // 计算电流模型的磁链（本周期开始点）
    trans2_albe2dq(Is, &hMRAS->thetaE);
    // TODO 磁链查找表
    hMRAS->Psi_I.d = Is->d * MATLAB_PARA_Ld + MATLAB_PARA_faif;
    hMRAS->Psi_I.q = Is->q * MATLAB_PARA_Lq;
    trans2_dq2albe(&hMRAS->Psi_I, &hMRAS->thetaE);
    hMRAS->Psi_I.abdq0 = fmaxf(transX_albe2abs2(&hMRAS->Psi_I), 1e-6);

    // 比较电流模型磁链和电压模型磁链，计算PI修正值
    // hMRAS->Ucomp.al = PIctrl_update_noSat2(hMRAS->PsiPI1, hMRAS->Psi_I.al - hMRAS->Psi_U.al);
    // hMRAS->Ucomp.be = PIctrl_update_noSat2(hMRAS->PsiPI2, hMRAS->Psi_I.be - hMRAS->Psi_U.be);
    // // debug
    // trans2_albe2dq(&hMRAS->Psi_U, &hMRAS->thetaE);

    // 尝试从dq轴来用PI补，也许也能抑制积分饱和的问题？
    // Kp项与albe效果一致？Ki项可能意义不明显
    // trans2_albe2dq(&hMRAS->Psi_U, &hMRAS->thetaE);
    // hMRAS->Ucomp.d = PIctrl_update_noSat2(hMRAS->PsiPI1, hMRAS->Psi_I.d - hMRAS->Psi_U.d);
    // hMRAS->Ucomp.q = PIctrl_update_noSat2(hMRAS->PsiPI2, hMRAS->Psi_I.q - hMRAS->Psi_U.q);
    // trans2_dq2albe(&hMRAS->Ucomp, &hMRAS->thetaE);

    // 计算电压模型的磁链，Us用的是上一个控制周期结尾的，恰好用来估算本周期结束时的磁链
    // 会增加角度误差？
    // hMRAS->Psi_U.al += vCTRL_TS * (Us->al - hMRAS->Rs * Is->al + hMRAS->Ucomp.al);
    // hMRAS->Psi_U.be += vCTRL_TS * (Us->be - hMRAS->Rs * Is->be + hMRAS->Ucomp.be);

    // 使用低通滤波器代替积分器以改善饱和
    hMRAS->Psi_U.al = LPF_Ord1_update_kahan(&hMRAS->PalLPF, Us->al - hMRAS->Rs * Is->al) * hMRAS->lpfGain;
    hMRAS->Psi_U.be = LPF_Ord1_update_kahan(&hMRAS->PbeLPF, Us->be - hMRAS->Rs * Is->be) * hMRAS->lpfGain;
    // 旋转修正相移
    trans2_albe2dq(&hMRAS->Psi_U, &hMRAS->thetaLPF);


    // 由albe轴下计算更新观测值
    // hMRAS->WeErr = (hMRAS->Psi_I.al * hMRAS->Psi_U.be - hMRAS->Psi_I.be * hMRAS->Psi_U.al) / hMRAS->Psi_I.abs2;
    // 使用相移修正的值
    hMRAS->WeErr = (hMRAS->Psi_I.al * hMRAS->Psi_U.q - hMRAS->Psi_I.be * hMRAS->Psi_U.d) / hMRAS->Psi_I.abs2;
    hMRAS->We = PIctrl_update_noSat2(hMRAS->wPI, hMRAS->WeErr);
    // 计算该角速度对应的修正角，加入滤波
    if (fabsf(hMRAS->wPI->integral) < hMRAS->lpfW)
    {
        // 限制角度，低速时放弃
        LPF_Ord1_update(&hMRAS->compLPF, 0);
    }
    else
    {
        // 采用积分项计算相移，减小噪声影响
        LPF_Ord1_update(&hMRAS->compLPF, __atanpuf32(hMRAS->lpfW / hMRAS->wPI->integral));

        // 这时候角度才可能收敛，考虑在电流足够大时更新电阻
        if (Is->abs2 > hMRAS->Is2Min)
        {
            hMRAS->RsErr = (Is->al * (hMRAS->Psi_U.al - hMRAS->Psi_I.al) + Is->be * (hMRAS->Psi_U.be - hMRAS->Psi_I.be)) / Is->abs2;
            hMRAS->Rs = PIctrl_update_clamp(hMRAS->RsPI, hMRAS->RsErr);
        }
    }
    thetaCal_setTheta(&hMRAS->thetaLPF, LPF_Ord1_2_getVal(&hMRAS->compLPF));
    hMRAS->lpfGain = 1.0f / (thetaCal_getCosVal(&hMRAS->thetaLPF) * hMRAS->lpfW);

    // 仿真调试

#ifdef TYJ_TEST
    static int32_t flowCtrlCnt1 = 0;
    if (flowCtrlCnt1++ == (2.0 * MATLAB_PARA_ctrl_freq))
    {
        // 手动改变电阻
        hMRAS->Rs = -0.015;
        PIctrl_setIntg(hMRAS->RsPI, hMRAS->Rs);
    }
#endif

    // 尝试在dq轴下进行计算更新观测值
    // 似乎稳定性极差?
    // trans2_albe2dq(Is, &hMRAS->thetaE);
    // float dPsid = vCTRL_TS * (Us->d + hMRAS->We * hMRAS->Psi_U.q - hMRAS->Rs * Is->d + hMRAS->Ucomp.d);
    // float dPsiq = vCTRL_TS * (Us->q - hMRAS->We * hMRAS->Psi_U.d - hMRAS->Rs * Is->q + hMRAS->Ucomp.q);
    // hMRAS->Psi_U.d += dPsid;
    // hMRAS->Psi_U.q += dPsiq;
    // hMRAS->WeErr = (hMRAS->Psi_I.d * hMRAS->Psi_U.q - hMRAS->Psi_I.q * hMRAS->Psi_U.d) / hMRAS->Psi_I.abs2;
    // hMRAS->We = PIctrl_update_noSat2(hMRAS->wPI, hMRAS->WeErr);
    // // debug
    // trans2_dq2albe(&hMRAS->Psi_U, &hMRAS->thetaE);

    // 更新观测角度
    thetaCal_setTheta(&hMRAS->thetaE, thetaCal_getTheta(&hMRAS->thetaE) + hMRAS->We * vCTRL_TS * (float)(1.0f / 2.0f / M_PI));

    // 输出本周期观测角度
    return hMRAS->thetaOut;
}

#endif
