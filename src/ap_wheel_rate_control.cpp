#include <ap_wheel_rate_control.h>


#ifdef CONFIG_SKID_PID


float ap_wheel_rate_control::
get_rate_controlled_throttle( inst_e i,
							  float  desired_rate,
							  float  dt )
{
	ac_pid& pid = i == inst_e::LEFT ? _pid_left : _pid_right;
}


#endif // CONFIG_SKID_PID 
