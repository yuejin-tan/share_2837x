#include "algo_code_config.h"

// 控制频率
float vCTRL_FREQ = CTRL_FREQ;
float vCTRL_TS = 1.0f / CTRL_FREQ;

// PWM装载值
int16_t vPWM_LOAD_VAL_I = PWM_LOAD_VAL;
float vPWM_LOAD_VAL = PWM_LOAD_VAL;

// PWM默认比较值
int16_t vPWM_CMP_DEFAILT_VAL_I = PWM_CMP_DEFAILT_VAL;

// eCAP值
float vECAP_HALF_VAL_INV = 2 * PWM_FREQ / ECAP_CLK;
