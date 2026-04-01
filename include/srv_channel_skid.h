#ifndef __AP_MOTORS_UGV
#define __AP_MOTORS_UGV


#include <config.h>

#ifdef CONFIG_USE_MXP_MD2
#   include <DualVNH5019MotorShield.h>
#endif

#include <ar_attitude_control.h>
#include <srv_channel.h>


class srv_channel_skid
{
public:
	srv_channel_skid( void );

	void init( void );
	void update( void );

private:

#ifdef CONFIG_USE_MXP_MD2
	static const int _MD_LEFT_INA1  = CONFIG_MXP_MD2_LEFT_INA1_PIN;
	static const int _MD_LEFT_INB1  = CONFIG_MXP_MD2_LEFT_INB1_PIN;
	static const int _MD_LEFT_PWM1  = CONFIG_MXP_MD2_LEFT_PWM1_PIN;

	static const int _MD_LEFT_INA2  = CONFIG_MXP_MD2_LEFT_INA2_PIN;
	static const int _MD_LEFT_INB2  = CONFIG_MXP_MD2_LEFT_INB2_PIN;
	static const int _MD_LEFT_PWM2  = CONFIG_MXP_MD2_LEFT_PWM2_PIN;

	static const int _MD_RIGHT_INA1 = CONFIG_MXP_MD2_RIGHT_INA1_PIN;
	static const int _MD_RIGHT_INB1 = CONFIG_MXP_MD2_RIGHT_INB1_PIN;
	static const int _MD_RIGHT_PWM1 = CONFIG_MXP_MD2_RIGHT_PWM1_PIN;

	static const int _MD_RIGHT_INA2 = CONFIG_MXP_MD2_RIGHT_INA2_PIN;
	static const int _MD_RIGHT_INB2 = CONFIG_MXP_MD2_RIGHT_INB2_PIN;
	static const int _MD_RIGHT_PWM2 = CONFIG_MXP_MD2_RIGHT_PWM2_PIN;
#endif // CONFIG_USE_MXP_MD2

	const int __STEER_FUNC    = 0;
	const int __THROTTLE_FUNC = 1;

	static const int _LIMIT_STEER_LEFT     = 1 << 0;
	static const int _LIMIT_STEER_RIGHT    = 1 << 1;
	static const int _LIMIT_THROTTLE_LOWER = 1 << 2;
	static const int _LIMIT_THROTTLE_UPPER = 1 << 3;
	
	float _val[2], _val_old[2], _out_scaled[2], _slew_rate;

	srv_channel _throttle_ch, _steer_ch;

	uint8_t _limit_mask = 0;
	
#ifdef CONFIG_USE_MXP_MD2
	DualVNH5019MotorShield _md_left, _md_right;
#endif

	float _get_slew_limited_throttle( const float throttle, const float dt );
	void _output( const bool armed, const float speed, const float dt );
	void _slew_limit_throttle( const float dt );
};


#endif // ! __SRV_CHANNEL_SKID
