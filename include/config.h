#ifndef __CONFIG_H
#define __CONFIG_H


// Motor config
//--------------------------------------------------------
#define CONFIG_MOT_THR_MAX   100

static_assert( 5 <= CONFIG_MOT_THR_MAX
			&&      CONFIG_MOT_THR_MAX <= 100,
			   "CONFIG_MOT_THR_MAX must be in range [5, 100]" );

#define CONFIG_MOT_THR_MIN   0

static_assert( 0 <= CONFIG_MOT_THR_MIN
			&&      CONFIG_MOT_THR_MIN <= 20,
			   "CONFIG_MOT_THR_MIN must be in range [0, 20]" );

static_assert( CONFIG_MOT_THR_MIN <= CONFIG_MOT_THR_MAX,
			   "CONFIG_MOT_THR_MIN must be less or equal t0 CONFIG_MOT_THR_MAX");

#define CONFIG_MOT_THST_EXPO 0.0

static_assert( -1.0f - __FLT_EPSILON__ < float( CONFIG_MOT_THST_EXPO )
			&&                           float( CONFIG_MOT_THST_EXPO ) < 1.0f + __FLT_EPSILON__,
			   "CONFIG_MOT_THST_EXPO must be in range [-1.0, 1.0]" );


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
