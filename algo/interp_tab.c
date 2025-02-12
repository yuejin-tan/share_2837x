/*
 * interp_tab.c
 *
 *  Created on: 2023年5月9日
 *      Author: t
 */

#include "interp_tab.h"
#include "algo_code_config.h"

#ifdef TYJ_TEST

 // 仿真所用插值表

 // MTPA情况下转矩和用D轴电流的对应关系，对应转矩为0，1，2……25
const float TE_ID_MTPA_2WAY_TAB[] = {
-76.4522F,
-74.1545F,
-71.8568F,
-69.5591F,
-67.2123F,
-64.8132F,
-62.3588F,
-59.8444F,
-57.2659F,
-54.6185F,
-51.8963F,
-49.0928F,
-46.2006F,
-43.2117F,
-40.1157F,
-36.9024F,
-33.5571F,
-30.0660F,
-26.4112F,
-22.5761F,
-18.5437F,
-14.3093F,
-9.9069F,
-5.5048F,
-1.6960F,
-0.0000F,
-1.6960F,
-5.5048F,
-9.9069F,
-14.3093F,
-18.5437F,
-22.5761F,
-26.4112F,
-30.0660F,
-33.5571F,
-36.9024F,
-40.1157F,
-43.2117F,
-46.2006F,
-49.0928F,
-51.8963F,
-54.6185F,
-57.2659F,
-59.8444F,
-62.3588F,
-64.8132F,
-67.2123F,
-69.5591F,
-71.8568F,
-74.1545F,
-76.4522F,
};

// MTPA情况下转矩和用Q轴电流的对应关系，对应转矩为0，1，2……25
const float TE_IQ_MTPA_2WAY_TAB[] = {
-98.9228F,
-96.5387F,
-94.1546F,
-91.7705F,
-89.3305F,
-86.8309F,
-84.2676F,
-81.6348F,
-78.9267F,
-76.1370F,
-73.2575F,
-70.2788F,
-67.1901F,
-63.9789F,
-60.6286F,
-57.1211F,
-53.4300F,
-49.5258F,
-45.3662F,
-40.8982F,
-36.0429F,
-30.6899F,
-24.6659F,
-17.7129F,
-9.4901F,
0.0000F,
9.4901F,
17.7129F,
24.6659F,
30.6899F,
36.0429F,
40.8982F,
45.3662F,
49.5258F,
53.4300F,
57.1211F,
60.6286F,
63.9789F,
67.1901F,
70.2788F,
73.2575F,
76.1370F,
78.9267F,
81.6348F,
84.2676F,
86.8309F,
89.3305F,
91.7705F,
94.1546F,
96.5387F,
98.9228F,
};

#endif

#if 0

#ifndef TYJ_TEST
#pragma CODE_SECTION(lookUp_1d_lin, MEM_MACRO);
#endif
float lookUp_1d_lin(float xIn, const float xTab[], const float yTab[], int16_t* hPrevIndex, int16_t maxIndex)
{
    float frac;
    float yAns;
    float yLeft;
    int16_t tabIdx;
    int16_t isFound;
    int16_t idxLeft;
    int16_t idxRight;

    if (xIn <= xTab[0])
    {
        tabIdx = 0;
        frac = 0.0F;
    }
    else if (xIn < xTab[maxIndex])
    {
        tabIdx = *hPrevIndex;
        idxLeft = 0;
        idxRight = maxIndex;
        isFound = 0;
        while (isFound == 0)
        {
            if (xIn < xTab[tabIdx])
            {
                idxRight = tabIdx - 1;
                tabIdx = ((tabIdx + idxLeft) - 1) >> 1;
            }
            else if (xIn < xTab[tabIdx + 1])
            {
                isFound = 1;
            }
            else
            {
                idxLeft = tabIdx + 1;
                tabIdx = ((tabIdx + idxRight) + 1) >> 1;
            }
        }

        frac = (xIn - xTab[tabIdx]) / (xTab[tabIdx + 1] - xTab[tabIdx]);
    }
    else
    {
        tabIdx = maxIndex;
        frac = 0.0F;
    }

    *hPrevIndex = tabIdx;

    if (tabIdx == maxIndex)
    {
        yAns = yTab[tabIdx];
    }
    else
    {
        yLeft = yTab[tabIdx];
        yAns = (yTab[tabIdx + 1] - yLeft) * frac + yLeft;
    }

    return yAns;
}

#endif

#if 1

// ztab: z_x0_y0 z_x0_y1 ... z_x0_yN z_x1_y0 ... z_xN_yN

#ifndef TYJ_TEST
#pragma CODE_SECTION(lookUp_2d_lin_puX, MEM_MACRO);
#endif
float lookUp_2d_lin_puX(float xIn, float yIn, const float zTab[], int16_t maxIdxX, int16_t maxIdxY)
{
    int16_t zLeftIdx;
    int16_t zRightIdx;
    int16_t tabIdx;

    if (xIn <= 0)
    {
        zLeftIdx = zRightIdx = 0;
    }
    else if (xIn >= maxIdxX)
    {
        zLeftIdx = zRightIdx = maxIdxX * (maxIdxY + 1);
    }
    else
    {
        tabIdx = xIn;
        zLeftIdx = tabIdx * (maxIdxY + 1);
        zRightIdx = zLeftIdx + (maxIdxY + 1);
    }

    float zLeft = lookUp_1d_lin_puX(yIn, &zTab[zLeftIdx], maxIdxY);
    float zRight = lookUp_1d_lin_puX(yIn, &zTab[zRightIdx], maxIdxY);
    float frac = xIn - tabIdx;

    return (zRight - zLeft) * frac + zLeft;
}

#endif
