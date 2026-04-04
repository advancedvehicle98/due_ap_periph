#ifndef __SRV_CHANNEL_H
#define __SRV_CHANNEL_H


#include <stdint.h>


class srv_channel
{
public:

	enum class srv_func_e
	{
		  STEER          = 26
		, THROTTLE       = 70
		, THROTTLE_LEFT  = 73
		, THROTTLE_RIGHT = 74
	};
	
	srv_channel(       srv_func_e func,
				 const uint16_t   pwm_trim,
				 const uint16_t   pwm_max,
				 const uint16_t   pwm_min );

	enum class limit_e
	{
		ZERO_PWM
	};

	void calc_pwm( const float _out_scaled );

	srv_func_e get_func( void ) const {
		return _func;
	}

	uint16_t get_limit_pwm( const limit_e limit ) const
	{
		switch ( limit ) {
		case limit_e::ZERO_PWM: return 0;
		}

		return 0;
	}

	void output( void );

	inline void set_output_pwm( const uint16_t pwm )
	{
		_output_pwm = pwm;
	}

	inline void set_output_limit( const limit_e limit )
	{
		_output_pwm = get_limit_pwm( limit );
	}

private:
	srv_func_e _func;

	uint16_t _output_pwm,
		     _pwm_trim, _pwm_max, _pwm_min,
		     _high_out;
};


#endif // ! __SRV_CHANNEL_H
