#ifndef __AP_MOTORS_UGV
#define __AP_MOTORS_UGV


#include <config.h>

#ifdef CONFIG_USE_MXP_MD2
#   include <DualVNH5019MotorShield.h>
#endif


class srv_channel_skid
{
public:
	srv_channel_skid( // ap_wheel_rate_ctrl& rate_ctrl
				   );

	void init( void );
	void update( void );

#ifdef CONFIG_USE_MXP_MD2
	DualVNH5019MotorShield _md_left, _md_right;
#endif

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
	
	float _throttle, _steering;
	bool _corking = false;
};


#endif // ! __SRV_CHANNEL_SKID
