#ifndef PSI_LUT_
#define PSI_LUT_

#include "transform.h"
#include "interp_tab.h"


#ifdef TYJ_TEST

// 仿真就用线性模型算
static inline void PsiLUT(struct Trans_struct* Is_input, struct Trans_struct* Psi_output)
{
    Psi_output->d = Is_input->d * MATLAB_PARA_Ld + MATLAB_PARA_faif;
    Psi_output->q = Is_input->q * MATLAB_PARA_Lq;
}

static inline float Ld1dLUT(float Id)
{
    (void)Id;
    return MATLAB_PARA_Ld;
}

static inline float Lq1dLUT(float Iq)
{
    (void)Iq;
    return MATLAB_PARA_Lq;
}

#else

// TODO 磁链查找表在各工程里实现
extern const float psidLUTpu16negIdxIqy[16 * 16];
extern const float psiqLUTpu16negIdxIqy[16 * 16];
extern const float psiLUTis_1;

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(PsiLUT)
#endif
static inline void PsiLUT(struct Trans_struct* Is_input, struct Trans_struct* Psi_output)
{
    float puId = -Is_input->d * psiLUTis_1;
    float puIq = Is_input->q * psiLUTis_1;

    // 视作“偶对称”
    Psi_output->d = lookUp_2d_lin_puX(puId, fabsf(puIq), psidLUTpu16negIdxIqy, 15, 15);
    // 视作“奇对称”
    float PsiqAbs = lookUp_2d_lin_puX(puId, fabsf(puIq), psiqLUTpu16negIdxIqy, 15, 15);
    Psi_output->q = puIq < 0.0f ? -PsiqAbs : PsiqAbs;

    return;
}

extern const float LdLUTpu16negId[16];
extern const float LqLUTpu16Iq[16];

static inline float Ld1dLUT(float Id)
{
    return lookUp_1d_lin_puX(-Id * psiLUTis_1, LdLUTpu16negId, 15);
}

static inline float Lq1dLUT(float Iq)
{
    return lookUp_1d_lin_puX(fabsf(Iq * psiLUTis_1), LqLUTpu16Iq, 15);
}

#endif

#endif
