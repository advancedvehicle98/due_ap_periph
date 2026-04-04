#include <srv_channel.h>

#include <math.h>


srv_channel::
srv_channel(       srv_func_e func,
			 const uint16_t   pwm_trim,
			 const uint16_t   pwm_max,
			 const uint16_t   pwm_min )
	: _func( func )
	, _pwm_trim( pwm_trim )
	, _pwm_max( pwm_max )
	, _pwm_min( pwm_min )
{
	
}


void srv_channel::
calc_pwm( const float _out_scaled )
{
	if ( ! _high_out ) {
		_output_pwm = _pwm_trim;
		return;
	}

	float val = math::constrain_float( _out_scaled, (float) -_high_out, (float) _high_out );

	uint16_t add = (uint16_t) (val*(float)( _pwm_max - _pwm_trim ) / (float) _high_out );
	uint16_t sub = (uint16_t) (-val*(float)( _pwm_trim - _pwm_min ) / (float) _high_out );

	if ( val > 0 ) _output_pwm = _pwm_trim + add;
	else           _output_pwm = _pwm_trim - sub;
}


void srv_channel::
output( void )
{
	
}
