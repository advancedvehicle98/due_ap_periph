#ifndef __AP_WHEEL_RATE_CONTROL_H
#define __AP_WHEEL_RATE_CONTROL_H


#include <config.h>

#include <ac_pid.h>


#ifdef CONFIG_SKID_PID


class ap_wheel_rate_control
{
public:
	ap_wheel_rate_control( void ) = default;
	
	enum class inst_e { LEFT, RIGHT };

	float get_rate_controlled_throttle( inst_e i, float desired_rate, float dt );

private:
	static const int _ENC_FR_PIN = CONFIG_ENC_FR_PIN;
	static const int _ENC_BR_PIN = CONFIG_ENC_BR_PIN;
	static const int _ENC_FL_PIN = CONFIG_ENC_FL_PIN;
	static const int _ENC_BL_PIN = CONFIG_ENC_BL_PIN;

	ac_pid _pid_left, _pid_right;
};


#endif // CONFIG_SKID_PID


#endif // ! __AP_WHEEL_RATE_CONTROL_H
