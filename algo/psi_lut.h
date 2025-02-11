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

    Psi_output->d = lookUp_2d_lin_puX(puId, fabsf(puIq), psidLUTpu16negIdxIqy, 15, 15);
    float PsiqAbs = lookUp_2d_lin_puX(puId, fabsf(puIq), psiqLUTpu16negIdxIqy, 15, 15);
    Psi_output->q = puIq < 0.0f ? -PsiqAbs : PsiqAbs;

    return;
}

#endif

#endif
