/*
 * waveGen.h
 *
 *  Created on: 2022年7月6日
 *      Author: tyj
 */

#ifndef INCX_WAVEGEN_H_
#define INCX_WAVEGEN_H_

#include "algo_code_config.h"

 /**
  * @brief 斜坡给定函数
  * @details 根据lastTarget来判定下次三角波值
  * @param[in] freq 三角波频率
  * @param[in] lastTarget 给定指令值，该次的三角波输出依赖于该值
  * @param[in] amp 三角波幅值
  * @param[in] offset 三角波偏置
  * @return float 获得的指令值
  * @see CTRL_FREQ 这里假定调用频率即为中断频率
  */
float targetTria(float freq, float lastTarget, float amp, float offset);

/**
 * @brief 正弦给定函数
 * @details 下次正弦值由内部状态变量决定，首次调用可能出现波形不连续
 * @param[in] freq 正弦频率
 * @param[in] amp 正弦幅值
 * @param[in] offset 正弦偏置
 * @return float 获得的指令值
 * @see CTRL_FREQ 这里假定调用频率即为中断频率
 */
float targetSine(float freq, float amp, float offset);

/**
 * @brief 斜坡给定函数
 * @details 输出值将慢慢逼近斜坡
 * @param[in] finalVal 最终指令值
 * @param[in] lastTarget 上次指令值
 * @param[in] stepSize 斜坡步长
 * @return float 获得的指令值
 */
static inline float targetRamp(float finalVal, float lastTarget, float stepSize)
{
    if (finalVal > lastTarget)
    {
        if (finalVal > lastTarget + stepSize)
        {
            return lastTarget + stepSize;
        }
        else
        {
            return finalVal;
        }
    }
    else
    {
        if (finalVal < lastTarget - stepSize)
        {
            return lastTarget - stepSize;
        }
        else
        {
            return finalVal;
        }
    }
}

#endif /* INCX_WAVEGEN_H_ */
