#ifndef KALMANFILT_W_H_
#define KALMANFILT_W_H_

#include "algo_code_config.h"

struct KalmanFilter_w_struct
{
    float We;
    float Rs;
    float thetaOut;

    float WeErr;
    float RsErr;

};

#endif
