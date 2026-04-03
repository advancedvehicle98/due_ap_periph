#include <srv_channel_skid.h>

#include <initializer_list>

#include <Arduino.h>

#include <math.h>


srv_channel_skid::
srv_channel_skid( void )
	:   
#ifdef CONFIG_USE_MXP_MD2
	  _md_left( _MD_LEFT_INA1, _MD_LEFT_INB1, _MD_LEFT_PWM1, 
				-1, -1, 				
				_MD_LEFT_INA2, _MD_LEFT_INB2, _MD_LEFT_PWM2, 
				-1, -1 )
              
	, _md_right( _MD_RIGHT_INA1, _MD_RIGHT_INB1, _MD_RIGHT_PWM1, 
				 -1, -1,
				 _MD_RIGHT_INA2, _MD_RIGHT_INB2, _MD_RIGHT_PWM2, 
				 -1, -1 )
#endif
{
	
}


void srv_channel_skid::
init( void )
{
#ifdef CONFIG_USE_MXP_MD2
	_md_right.init();
	_md_left.init();
#endif

	_high_out[__LEFT]  = 1000;
	_high_out[__RIGHT] = -1000;
}


void srv_channel_skid::
update( const bool armed, const float dt )
{
	_output( armed, dt );
}


// Private functions
//-------------------------------------------------------

uint16_t srv_channel_skid::
_calc_individual_pwm( const float _out_scaled, const int16_t high_out )
{
	if ( ! high_out ) return _PWM_TRIM;

	float val = math::constrain_float( _out_scaled, (float) high_out, (float) high_out );

	uint16_t add = (uint16_t) (val*(float)( _PWM_MAX - _PWM_TRIM ) / (float) high_out );
	uint16_t sub = (uint16_t) (-val*(float)( _PWM_TRIM - _PWM_MIN ) / (float) high_out );

	if ( val > 0 ) return _PWM_TRIM + add;
	else           return _PWM_TRIM - sub;
}
	

inline void srv_channel_skid::
_calc_pwm( void )
{
	_pwm[__LEFT]  = _calc_individual_pwm( _out_scaled[__LEFT], _high_out[__LEFT] );
	_pwm[__RIGHT] = _calc_individual_pwm( _out_scaled[__RIGHT], _high_out[__RIGHT] );
}


void srv_channel_skid::
_driver_specific_out( void )
{
#ifdef CONFIG_USE_MXP_MD2
	_md_left.setM1Speed( _pwm[__LEFT] );
	_md_left.setM2Speed( _pwm[__LEFT] );
	
	_md_right.setM1Speed( _pwm[__RIGHT] );
	_md_right.setM2Speed( _pwm[__RIGHT] );
#endif
}


void srv_channel_skid::
_get_rate_controlled_throttle( float& throttle_left,
							   float& throttle_right,
							   float  dt )
{
	if ( ! math::is_positive( dt ) ) return;

	throttle_left = _rate_ctrl.get_rate_controlled_throttle(
		ap_wheel_rate_control::inst_e::LEFT, throttle_left, dt
	);
	
	throttle_right = _rate_ctrl.get_rate_controlled_throttle(
		ap_wheel_rate_control::inst_e::RIGHT, throttle_right, dt
	);
}


float srv_channel_skid::
_get_slew_limited_throttle( const float throttle,
							const float dt ) const
{
	if ( _slew_rate <= 0 ) return throttle;

	const float throttle_change_max = static_cast< float >( _slew_rate )*dt;

	return math::constrain_float( throttle,
		_throttle_old - throttle_change_max,
		_throttle_old + throttle_change_max
	);
}


float srv_channel_skid::
_get_scaled_throttle( float throttle ) const
{
	if ( math::is_zero( throttle ) ) return throttle;

	if ( _THROTTLE_MIN > 0 ) {
		if ( math::is_negative( throttle ) )
			throttle = -_THROTTLE_MIN + throttle*(100.0f - _THROTTLE_MIN)*0.01f;
		else
			throttle =  _THROTTLE_MIN + throttle*(100.0f - _THROTTLE_MIN)*0.01f;
	}
	
	if ( math::is_zero( _TRUST_CURVE_EXP ) ) return throttle;

	const float sgn = math::is_negative( throttle ) ? -1.0f : 1.0f;
	const float throttle_pct = math::constrain_float( throttle, -100.0f, 100.0f ) * 0.01f;
	const float thst_expo_1 = _TRUST_CURVE_EXP - 1.0f;

	return sgn*100.0f*( thst_expo_1
						+ sqrtf( thst_expo_1*thst_expo_1
								 + 4.0f*thst_expo_1*fabsf( throttle_pct ) )
						)
		 / ( 2.0f*_TRUST_CURVE_EXP );
}


// см. AP_MotorsUGV::output_regular
void srv_channel_skid::
_output( const bool  armed,
		 const float dt )
{
	float motor_left, motor_right;
	
	if ( ! armed ) {
		_out_scaled[ __LEFT ] = 0;
		_out_scaled[ __RIGHT ] = 0;
		
		goto _set_output;
	}
	
	_limit_mask = 0;

	_set_limits_from_input();

	_steer = math::constrain_float( _steer, _STEER_MIN, _STEER_MAX );
	
	{
		float steer_scaled = _steer/_STEER_MAX;
		float throttle_scaled = _throttle/100.0f;

		const float thrust_asymmetry = max(1.0, _THST_ASYM);
		const float lower_throttle_limit = -1.0/thrust_asymmetry;
		const float best_steer_throttle = (1.0f + lower_throttle_limit)*0.5f;

		float steer_range;

		if ( throttle_scaled < best_steer_throttle )
			steer_range = max( throttle_scaled, 0.0f ) - lower_throttle_limit;
		else
			steer_range = 1.0f - best_steer_throttle;

		if      ( steer_scaled >  steer_range ) {
			_limit_mask |= _LIMIT_STEER_RIGHT_MASK;
			steer_scaled =  steer_range;
		}
		else if ( steer_scaled < -steer_range ) {
			_limit_mask |= _LIMIT_STEER_LEFT_MASK;
			steer_scaled = -steer_range;
		}

		if      ( throttle_scaled > 1.0f ) {
			_limit_mask |= _LIMIT_THROTTLE_UPPER_MASK;
			throttle_scaled = 1.0f;
		}
		else if ( throttle_scaled < lower_throttle_limit ) {
			_limit_mask |= _LIMIT_THROTTLE_LOWER_MASK;
			throttle_scaled = lower_throttle_limit;
		}

		const float max_out = throttle_scaled + fabsf( steer_scaled );
		const float min_out = throttle_scaled - fabsf( steer_scaled );

		const float saturation_value = max( max_out, min_out/lower_throttle_limit );

		if ( saturation_value > 1.0f ) {
			const float steer_scaled_orig    = steer_scaled;
			const float throttle_scaled_orig = throttle_scaled;

			const float fair_scaler = 1.0f/saturation_value;

			if ( _STR_THR_MIX >= 0.5f ) {
				steer_scaled *= math::lerp( fair_scaler, 1.0f, _STR_THR_MIX, 0.5f, 1.0f );
				
				if ( throttle_scaled >= best_steer_throttle )
					throttle_scaled = 1.0f - fabsf( steer_scaled );
				else
					throttle_scaled = fabsf( steer_scaled ) + lower_throttle_limit;
			}
			else {
				throttle_scaled *= math::lerp( fair_scaler, 1.0f, 0.5f - _STR_THR_MIX, 0.0f, 0.5f );
				const float steer_sgn = math::is_positive( steer_scaled ) ? 1.0f : -1.0f;

				if ( throttle_scaled >= best_steer_throttle )
					throttle_scaled = ( 1.0f - throttle_scaled )*steer_sgn;
				else
					throttle_scaled = ( throttle_scaled - lower_throttle_limit )*steer_sgn;	
			}
			
			if ( fabsf( steer_scaled ) < fabsf( steer_scaled_orig ) ) {
				if      ( math::is_negative( steer_scaled_orig ) )
					_limit_mask |= _LIMIT_STEER_LEFT_MASK;
			
				else if ( math::is_positive( steer_scaled_orig ) )
					_limit_mask |= _LIMIT_STEER_RIGHT_MASK;
			}
			
			if ( fabsf( throttle_scaled ) < fabsf( throttle_scaled_orig ) ) {
				if      ( math::is_negative( throttle_scaled_orig ) )
					_limit_mask |= _LIMIT_THROTTLE_LOWER_MASK;
			
				else if ( math::is_positive( throttle_scaled_orig ) )
					_limit_mask |= _LIMIT_THROTTLE_UPPER_MASK;
			}
		}

		motor_left  = throttle_scaled + steer_scaled;
		motor_right = throttle_scaled - steer_scaled;
	
		if ( math::is_negative( motor_right ) ) motor_right *= thrust_asymmetry;
		if ( math::is_negative( motor_left ) )  motor_left  *= thrust_asymmetry;
	}

	_output_throttle( 100.0f*motor_left, 100.0f*motor_right, dt );
	
_set_output:
	_calc_pwm();

#ifdef DEBUG
	_console->print( "pwm left: " );
	_console->println( _pwm[__LEFT] );
	_console->print( "pwm right: " );
	_console->println( _pwm[__RIGHT] );
#endif

	_driver_specific_out();
}


inline void srv_channel_skid::
_output_ch_all( void )
{
	// _throttle_ch.output();
	// _skid_ch.output();
}


void srv_channel_skid::
_output_reverse_throttle( float throttle_left,
						  float throttle_right,
						  const float delay )
{
	const uint32_t now_ms = millis();

	if ( ! math::is_zero( throttle_left ) ) {
		if ( throttle_left * _last_throttle[ __LEFT ] < 0
		  && now_ms - _last_output_ms[ __LEFT ] < delay*1000 )
			throttle_left = 0;
		else {
			_last_output_ms[ __LEFT ] = now_ms;
			_last_throttle[ __LEFT ] = throttle_left;
		}
	}

	if ( ! math::is_zero( throttle_right ) ) {
		if ( throttle_right * _last_throttle[ __RIGHT ] < 0
		  && now_ms - _last_output_ms[ __RIGHT ] < delay*1000 )
			throttle_right = 0;
		else {
			_last_output_ms[ __RIGHT ] = now_ms;
			_last_throttle[ __RIGHT ] = throttle_right;
		}
	}

	_set_output_scaled( throttle_left, throttle_right );
}
	

void srv_channel_skid::
_output_throttle( float throttle_left, float throttle_right, const float dt )
{
	throttle_left  = _get_scaled_throttle( throttle_left );
	throttle_right = _get_scaled_throttle( throttle_right );

	// это потом можно раскоментировать, когда ПИД будет
	// щас пока просто главное, чтобы он че то маломальское выдавал
	// _get_rate_controlled_throttle( throttle_left, throttle_right, dt );
	
	if ( _REV_DELAY > 0 )
		_output_reverse_throttle( 10*throttle_left, 10*throttle_right, _REV_DELAY );

	_set_output_scaled( 10*throttle_left, 10*throttle_right );
}


void srv_channel_skid::
_set_limits_from_input( void )
{
	uint8_t _limit_steer_left = _steer <= -_STEER_MAX ?
		                        _LIMIT_STEER_LEFT_MASK : 0;
	
	uint8_t _limit_steer_right = _steer >= _STEER_MAX ?
		                         _LIMIT_STEER_RIGHT_MASK : 0;
	
	uint8_t _limit_throttle_lower = _throttle <= -_THROTTLE_MAX ?
		                            _LIMIT_THROTTLE_LOWER_MASK : 0;
	
	uint8_t _limit_throttle_upper = _throttle >= _THROTTLE_MAX ?
		                            _LIMIT_THROTTLE_UPPER_MASK : 0;
	
	_limit_mask |= _limit_steer_left
		        |  _limit_steer_right
		        |  _limit_throttle_lower
		        |  _limit_throttle_upper;
}


void srv_channel_skid::
_set_output_scaled( float throttle_left, float throttle_right )
{
	_out_scaled[ __LEFT ]  = throttle_left;
	_out_scaled[ __RIGHT ] = throttle_right;
}


void srv_channel_skid::
_slew_limit_throttle( const float dt )
{
	const float throttle_orig = _throttle;

	_throttle = _get_slew_limited_throttle( _throttle, dt );

	if      ( throttle_orig > _throttle ) _limit_mask |= _LIMIT_THROTTLE_UPPER_MASK;
	else if ( throttle_orig < _throttle ) _limit_mask |= _LIMIT_THROTTLE_LOWER_MASK;
	
	_throttle_old = _throttle;
}
