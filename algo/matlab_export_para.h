// matlab 输出参数选择文件

#ifdef PROJ_SHARE

#define MATLAB_PARA_ifUseDoubleCtrlFreq 1
#define MATLAB_PARA_ctrl_freq_mul 2
#define MATLAB_PARA_PWM_clk 100000000.000000
#define MATLAB_PARA_fs 20000.000000
#define MATLAB_PARA_ctrl_freq 40000.000000
#define MATLAB_PARA_Isense_gain 0.003750
#define MATLAB_PARA_Isense_offset 1.500000
#define MATLAB_PARA_I_bus_sense_gain 0.001563
#define MATLAB_PARA_I_bus_sense_offset 1.250000
#define MATLAB_PARA_U_bus_sense_gain 0.003322
#define MATLAB_PARA_U_bus_sense_offset 0.000000
#define MATLAB_PARA_adc_gain 1365.333333
#define MATLAB_PARA_RDC_gain 10430.378350
#define MATLAB_PARA_RDC2ELE_RATIO 2
#define MATLAB_PARA_C_dclink 0.001200
#define MATLAB_PARA_Ld 0.000170
#define MATLAB_PARA_Lq 0.000500
#define MATLAB_PARA_faif 0.017000
#define MATLAB_PARA_p0 2.000000
#define MATLAB_PARA_pi2 6.283185
#define MATLAB_PARA_pi1 3.141593
#define MATLAB_PARA_kpd_iloop 1.360000
#define MATLAB_PARA_kpq_iloop 4.000000
#define MATLAB_PARA_ki_iloop 0.006000
#define MATLAB_PARA_Upi_max 155.884573
#define MATLAB_PARA_tick_dead 200.000000
#define MATLAB_PARA_tick_db_comp 70.000000
#define MATLAB_PARA_kp_wloop 0.866667
#define MATLAB_PARA_ki_wloop 0.000029
#define MATLAB_PARA_Wpi_max 22.000000
#define MATLAB_PARA_Wpi_min -11.000000
#define MATLAB_PARA_kp_vloop 733.333333
#define MATLAB_PARA_ki_vloop 2.444444
#define MATLAB_PARA_Wdcpi_max 4000.000000
#define MATLAB_PARA_Wdcpi_min -20000.000000
#define MATLAB_PARA_kp_vloop_conv 0.400000
#define MATLAB_PARA_ki_vloop_conv 0.004000
#define MATLAB_PARA_Wdcpi_max_conv 8.000000
#define MATLAB_PARA_Wdcpi_min_conv -22.000000
#define MATLAB_PARA_ADRC_gamma 0.001000
#define MATLAB_PARA_ADRC_c_u1 20.000000
#define MATLAB_PARA_ADRC_c_u2 200.000000
#define MATLAB_PARA_ADRC_beta1 2000.000000
#define MATLAB_PARA_ADRC_beta2 550000.000000
#define MATLAB_PARA_ADRC_SMCk 0.000000
#define LPF_1Ord_1273Hz_T40000Hz_PARA 0.18126925F, -0.81873075F, 0.0F
#define LPF_2Ord_4000Hz_T40000Hz_ksi0_80_PARA 0.24104576F, -1.1248855F, 0.36593131F
#define LPF_2Ord_20Hz_T40000Hz_ksi1_00_KAHAN_PARA 0.003136663F, 0.0F, 0.0F
#define LPF_2Ord_400Hz_T40000Hz_ksi0_90_PARA 0.0037315524F, -1.8893322F, 0.89306373F
#define LPF_2Ord_1000Hz_T40000Hz_ksi0_90_PARA 0.021448479F, -1.7322647F, 0.75371321F
#define LPF_1Ord_20Hz_T40000Hz_PARA 0.003136663F, -0.99686334F, 0.0F

#else

#include "matlab_exp_para_local.h"

#endif
