#ifndef SPEED_CAL_H
#define SPEED_CAL_H

#include "algo_code_config.h"
#include "filter.h"

struct SpeedCal_struct
{
    float omegaE;
    float omegaE_inv;
    struct LPF_Ord1_2_struct* hLPF;
    uint16_t lastThetaE;
};

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(speedCal_init)
#endif
static inline void speedCal_init(struct SpeedCal_struct* hSpeedCal, struct LPF_Ord1_2_struct* hLPF_init, uint16_t thetaE)
{
    hSpeedCal->omegaE = 0;
    hSpeedCal->omegaE_inv = (12000.0 / 60.0 * 2.0 * M_PI * 2);
    hSpeedCal->lastThetaE = thetaE;
    hSpeedCal->hLPF = hLPF_init;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(speedCal_update)
#endif
static inline float speedCal_update(struct SpeedCal_struct* hSpeedCal, uint16_t thetaE)
{
    int16_t deltaThetaERaw = thetaE - hSpeedCal->lastThetaE;
    hSpeedCal->lastThetaE = thetaE;
    float omegaDiffed = deltaThetaERaw * vCTRL_FREQ * (float)(2.0 * M_PI / 65536.0);
    hSpeedCal->omegaE = LPF_Ord2_update_kahan(hSpeedCal->hLPF, omegaDiffed);
    hSpeedCal->omegaE_inv = __divf32(1.0f, __fmax(hSpeedCal->omegaE, 100.0f));
    return hSpeedCal->omegaE;
}

#ifndef TYJ_TEST
#pragma FUNC_ALWAYS_INLINE(speedCal_getAns)
#endif
static inline float speedCal_getAns(struct SpeedCal_struct* hSpeedCal)
{
    return hSpeedCal->omegaE;
}

#endif // SPEED_CAL_H
