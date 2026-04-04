#ifndef __CONFIG_ASSERTS
#define __CONFIG_ASSERTS


// Motor config
//--------------------------------------------------------

#define _assert_range_string( CFG, MIN, MAX ) \
	#CFG " must be in range [" #MIN ", " #MAX "]"

#define config_assert_range( CFG, MIN, MAX ) \
	static_assert( MIN <= CFG && CFG <= MAX, \
				   _assert_range_string( CFG, MIN, MAX) )

#define config_assert_float_range( CFG, MIN, MAX ) \
	static_assert( MIN - __FLT_EPSILON__ <= float( CFG ) \
				&&                          float( CFG ) <= MAX + __FLT_EPSILON__, \
				   _assert_range_string( CFG, MIN, MAX) )
	
config_assert_range( CONFIG_MOT_THR_MAX, 5, 100 );	
config_assert_range( CONFIG_MOT_THR_MIN, 0, 20 );

static_assert( CONFIG_MOT_THR_MIN <= CONFIG_MOT_THR_MAX,
			   "CONFIG_MOT_THR_MIN must be less or equal to CONFIG_MOT_THR_MAX");

config_assert_float_range( CONFIG_MOT_THST_EXPO, -1.0f, 1.0f );

config_assert_float_range( CONFIG_MOT_SPD_SCA_BASE, 0.0f, 10.0f );;

config_assert_float_range( CONFIG_MOT_REV_DELAY, 0.0f, 1.0f );


#ifdef CONFIG_SKID_STEER

config_assert_float_range( CONFIG_MOT_THST_ASYM, 1.0f, 10.0f );

config_assert_float_range( CONFIG_MOT_STR_THR_MIX, 0.2f, 1.0f );

#endif // CONFIG_SKID_STEER


// MXP-MD2 config
//--------------------------------------------------------

#ifdef CONFIG_USE_MXP_MD2

config_assert_range( CONFIG_MXP_MD2_PWM_TRIM, -1500, 1500 );
config_assert_range( CONFIG_MXP_MD2_PWM_MAX,  -1500, 1500 );
config_assert_range( CONFIG_MXP_MD2_PWM_MIN,  -1500, 1500 );

static_assert( CONFIG_MXP_MD2_PWM_MIN <= CONFIG_MXP_MD2_PWM_TRIM,
			   "CONFIG_MXP_MD2_PWM_MIN must be less or equal to CONFIG_MXP_MD2_PWM_TRIM");

static_assert( CONFIG_MXP_MD2_PWM_TRIM <= CONFIG_MXP_MD2_PWM_MAX,
			   "CONFIG_MXP_MD2_PWM_TRIM must be less or equal to CONFIG_MXP_MD2_PWM_MAX");

#endif // CONFIG_USE_MXP_MD2


// Navx config
//--------------------------------------------------------

#if defined( CONFIG_STATIC_NAVX ) && ! defined( CONFIG_USE_NAVX )
#   error "CONFIG_STATIC_NAVX is defined, but NavX is not used (CONFIG_USE_NAVX is not defined)"
#endif


// PID rate control parameters
//--------------------------------------------------------

#ifdef CONFIG_SKID_PID

config_assert_float_range( CONFIG_PID_P,         0.1f, 2.0f );
config_assert_float_range( CONFIG_PID_I,         0.0f, 2.0f );
config_assert_float_range( CONFIG_PID_D,         0.0f, 0.4f );
config_assert_float_range( CONFIG_PID_FF,        0.1f, 2.0f );
config_assert_float_range( CONFIG_PID_IMAX,      0.0f, 1.0f );
config_assert_float_range( CONFIG_PID_FILT_T_HZ, 1.0f, 50.0f );
config_assert_float_range( CONFIG_PID_FILT_E_HZ, 1.0f, 50.0f );
config_assert_float_range( CONFIG_PID_FILT_D_HZ, 1.0f, 50.0f );
config_assert_float_range( CONFIG_PID_SRMAX,     0.0f, 200.0f );
config_assert_float_range( CONFIG_PID_DFF,       0.0f, 0.02f );

#endif


// CAN parameters
//----------------------------------------------------

#if CONFIG_CAN_IF != CAN_IF0 && CONFIG_CAN_IF != CAN_IF1
#   error "CONFIG_CAN_IF_NUM must be CAN_IF0 or CAN_IF1"
#endif 


// Misc
//----------------------------------------------------

#ifndef CONFIG_DT
#define CONFIG_DT 0.02f
#warning "CONFIG_DT is undefined, setting to default (0.02 s)"
#endif


#endif // ! __CONFIG_ASSERTS
