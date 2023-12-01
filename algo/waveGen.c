/*
 * waveGen.c
 *
 *  Created on: 2022年7月6日
 *      Author: tyj
 */
#include "waveGen.h"
#include "algo_code_config.h"

 /// 指定查找表的大小
#define TAB_DIV_QUARD (20)
float sinTab[TAB_DIV_QUARD + 1] = {
    0.000000f,
    0.078459f,
    0.156434f,
    0.233445f,
    0.309017f,
    0.382683f,
    0.453990f,
    0.522499f,
    0.587785f,
    0.649448f,
    0.707107f,
    0.760406f,
    0.809017f,
    0.852640f,
    0.891007f,
    0.923880f,
    0.951057f,
    0.972370f,
    0.987688f,
    0.996917f,
    1.000000f,
};

#ifndef TYJ_TEST
#pragma CODE_SECTION(targetTria, MEM_MACRO);
#endif
float targetTria(float freq, float lastTarget, float amp, float offset)
{
    // 1 向上，0 向下
    static int direction = 1;
    float retVal;
    float stepSize = (float)(4.0f / CTRL_FREQ) * amp * freq;
    if (direction)
    {
        // 向上
        float tmp = lastTarget + stepSize;
        float highThreshold = amp + offset;
        if (tmp < highThreshold)
        {
            retVal = tmp;
        }
        else
        {
            retVal = highThreshold;
            direction = 0;
        }
    }
    else
    {
        // 向下
        float tmp = lastTarget - stepSize;
        float lowThreshold = -amp + offset;
        if (tmp > lowThreshold)
        {
            retVal = tmp;
        }
        else
        {
            retVal = lowThreshold;
            direction = 1;
        }
    }
    return retVal;
}

#ifndef TYJ_TEST
#pragma CODE_SECTION(targetSine, MEM_MACRO);
#endif
float targetSine(float freq, float amp, float offset)
{
    // 每周期分4段,查表
    static float now_tick = 0;

    float inc = (float)(4.0f * TAB_DIV_QUARD / CTRL_FREQ) * freq;

    float tmp = now_tick + inc;
    if (tmp > (float)(4 * TAB_DIV_QUARD))
    {
        now_tick = tmp - (float)(4 * TAB_DIV_QUARD);
    }
    else
    {
        now_tick = tmp;
    }

    int section = now_tick * (float)(1.0f / TAB_DIV_QUARD);
    float index, sign;
    switch (section)
    {
    case 0:
        index = now_tick;
        sign = 1;
        break;
    case 1:
        index = (float)(2 * TAB_DIV_QUARD) - now_tick;
        sign = 1;
        break;
    case 2:
        index = now_tick - (float)(2 * TAB_DIV_QUARD);
        sign = -1;
        break;
    default:
        index = (float)(4 * TAB_DIV_QUARD) - now_tick;
        sign = -1;
        break;
    }
    int index1 = index;
    int index2 = index + 1;
    float weight1 = index2 - index;
    float weight2 = index - index1;
    float retAbs = sinTab[index1] * weight1 + sinTab[index2] * weight2;
    return retAbs * amp * sign + offset;
}
