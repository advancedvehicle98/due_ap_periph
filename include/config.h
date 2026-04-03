#ifndef __CONFIG_H
#define __CONFIG_H


#include <hdefs.h>


// Motor config
//--------------------------------------------------------

// см. параметры https://ardupilot.org/rover/docs/parameters.html#servo1-function

#define CONFIG_MOT_THR_MAX      100
#define CONFIG_MOT_THR_MIN      0
#define CONFIG_MOT_THST_EXPO    0.0
#define CONFIG_MOT_SPD_SCA_BASE 0.0
#define CONFIG_MOT_REV_DELAY    0.0

#define CONFIG_SKID_STEER

#ifdef CONFIG_SKID_STEER

#   define CONFIG_MOT_THST_ASYM   1.0
#   define CONFIG_MOT_STR_THR_MIX 0.6

#endif


// MXP-MD2 config
//--------------------------------------------------------

#define CONFIG_USE_MXP_MD2

#ifdef CONFIG_USE_MXP_MD2

#   define CONFIG_MXP_MD2_LEFT_INA1_PIN  2
#   define CONFIG_MXP_MD2_LEFT_INB1_PIN  3
#   define CONFIG_MXP_MD2_LEFT_PWM1_PIN  4

#   define CONFIG_MXP_MD2_LEFT_INA2_PIN  5
#   define CONFIG_MXP_MD2_LEFT_INB2_PIN  6
#   define CONFIG_MXP_MD2_LEFT_PWM2_PIN  7

#   define CONFIG_MXP_MD2_RIGHT_INA1_PIN 8
#   define CONFIG_MXP_MD2_RIGHT_INB1_PIN 9
#   define CONFIG_MXP_MD2_RIGHT_PWM1_PIN 10

#   define CONFIG_MXP_MD2_RIGHT_INA2_PIN 11
#   define CONFIG_MXP_MD2_RIGHT_INB2_PIN 12
#   define CONFIG_MXP_MD2_RIGHT_PWM2_PIN 13

#   define CONFIG_MXP_MD2_PWM_TRIM 0
#   define CONFIG_MXP_MD2_PWM_MAX  700
#   define CONFIG_MXP_MD2_PWM_MIN  -700

#endif


// Studica maverick motor config
//--------------------------------------------------------

#define CONFIG_ENC_FR_PIN 22
#define CONFIG_ENC_BR_PIN 23
#define CONFIG_ENC_FL_PIN 24
#define CONFIG_ENC_BL_PIN 25


// Navx config
//--------------------------------------------------------

#define CONFIG_USE_NAVX

#ifdef CONFIG_USE_NAVX
#   define CONFIG_STATIC_NAVX
#endif


// PID rate control parameters (тоже в процессе)
//--------------------------------------------------------

// https://github.com/ArduPilot/ardupilot/blob/7ae671e53f25e8f784d3e6ede9f125c6aca12cf7/libraries/AP_WheelEncoder/AP_WheelRateControl.cpp#L231

#ifdef CONFIG_SKID_STEER

#   undef CONFIG_SKID_PID

#   ifdef CONFIG_SKID_PID

#       define   CONFIG_PID_P         0.1f
#       define   CONFIG_PID_I         0.0f
#       define   CONFIG_PID_D         0.0f
#       define   CONFIG_PID_FF        0.1f 
#       define   CONFIG_PID_IMAX      0.0f 
#       define   CONFIG_PID_FILT_T_HZ 1.0f 
#       define   CONFIG_PID_FILT_E_HZ 1.0f
#       define   CONFIG_PID_FILT_D_HZ 1.0f 
#       define   CONFIG_PID_SRMAX     0.0f
#       define   CONFIG_PID_DFF       0.0f

#   endif // CONFIG_SKID_PID

#endif // CONFIG_SKID_STEER


// Flysky parameters
//--------------------------------------------------------

#define CONFIG_USE_FLYSKY

#ifdef CONFIG_USE_FLYSKY

#   define CONFIG_FLYSKY_CH_MODE      4
#   define CONFIG_FLYSKY_CH_THROTTLE  1
#   define CONFIG_FLYSKY_CH_STEER     0

#   define CONFIG_FLYSKY_DISABLED 1000

#   define CONFIG_FLYSKY_IF Serial

#endif // CONFIG_USE_FLYSKY


// CAN parameters
//--------------------------------------------------------

#define CONFIG_CAN_IF CAN_IF1


// Debug console parameters
//--------------------------------------------------------

#define CONFIG_CONSOLE_IF Serial


// Flash parameters config (в процессе)
//--------------------------------------------------------

#undef CONFIG_LOAD_PARAMS


// Misc
//----------------------------------------------------

#define CONFIG_DT 0.02f


#include <config_asserts.h>


#endif // ! __CONFIG_H
