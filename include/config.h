#ifndef __CONFIG_H
#define __CONFIG_H


// MXP-MD2 config
//--------------------------------------------------------
#define CONFIG_USE_MXP_MD2

#ifdef CONFIG_USE_MXP_MD2

#   define CONFIG_MXP_MD2_SKID_STEER

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

#   define CONFIG_MXP_MD2_MAX_PWM 500
#   define CONFIG_MXP_MD2_MIN_PWM (-500)

#endif


// Navx config
//--------------------------------------------------------
#define CONFIG_USE_NAVX

#ifdef CONFIG_USE_NAVX
#   define CONFIG_STATIC_NAVX
#endif


// Flash parameters config (в процессе)
//--------------------------------------------------------

//// #define CONFIG_LOAD_PARAMS


// Misc
//--------------------------------------------------------


//// #define CONFIG_PWM_CALC_SLEW


#endif // ! __CONFIG_H
