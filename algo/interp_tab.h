/*
 * interp_tab.h
 *
 *  Created on: 2023年5月9日
 *      Author: t
 */

#ifndef ALGO_CODE_INC_INTERP_TAB_H_
#define ALGO_CODE_INC_INTERP_TAB_H_

#include "algo_code_config.h"

#define TE_IQ_ID0_2WAY_TAB_OFFSET 12.0F
#define TE_IQ_ID0_2WAY_TAB_MAX 24
#define TE_IDQ_MTPA_2WAY_TAB_OFFSET 25.0F
#define TE_IDQ_MTPA_2WAY_TAB_MAX 50

extern const float TE_IQ_ID0_2WAY_TAB[];
extern const float TE_ID_MTPA_2WAY_TAB[];
extern const float TE_IQ_MTPA_2WAY_TAB[];

float lookUp_1d_lin(float xIn, const float xTab[], const float yTab[], int16_t* hPrevIndex, int16_t maxIndex);

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(lookUp_1d_lin_puX)
#endif
static inline float lookUp_1d_lin_puX(float xIn, const float yTab[], int16_t maxIndex)
{
    float frac;
    float yAns;
    float yLeft;
    int16_t tabIdx;

    if (xIn <= 0)
    {
        return yTab[0];
    }
    else if (xIn >= maxIndex)
    {
        return yTab[maxIndex];
    }
    else
    {
        tabIdx = xIn;
        frac = xIn - tabIdx;
    }

    yLeft = yTab[tabIdx];
    yAns = (yTab[tabIdx + 1] - yLeft) * frac + yLeft;

    return yAns;
}

// ztab: z_x0_y0 z_x0_y1 ... z_x0_yN z_x1_y0 ... z_xN_yN
float lookUp_2d_lin_puX(float xIn, float yIn, const float zTab[], int16_t maxIdxX, int16_t maxIdxY);

#endif /* ALGO_CODE_INC_INTERP_TAB_H_ */

