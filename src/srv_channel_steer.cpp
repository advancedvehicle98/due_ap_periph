#ifdef __IGNORE__

#include <srv_channel_steer.h>

#include <initializer_list>

#include <Arduino.h>

#include <math.h>


srv_channel_steer::
srv_channel_steer(  )
	: _throttle_ch( srv_channel::srv_func_e::THROTTLE,
					_PWM_TRIM, _PWM_MAX, _PWM_MIN )
	  
	, _steer_ch( srv_channel::srv_func_e::STEER,
				 _PWM_TRIM, _PWM_MAX, _PWM_MIN )
	  
#ifdef CONFIG_USE_MXP_MD2
	, _md_left( _MD_LEFT_INA1, _MD_LEFT_INB1, _MD_LEFT_PWM1, 
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


void srv_channel_steer::
init( void )
{
#ifdef CONFIG_USE_MXP_MD2
	_md_right.init();
	_md_left.init();
#endif
}


void srv_channel_steer::
update( void )
{
	// _output( true,  );
}


// Private functions
//-------------------------------------------------------

inline void srv_channel_steer::
_calc_pwm( void )
{
	_throttle_ch.calc_pwm( _out_scaled[ __THROTTLE_FUNC ] );
	_steer_ch.calc_pwm( _out_scaled[ __STEER_FUNC ] );
}
	

float srv_channel_steer::
_get_slew_limited_throttle( const float throttle,
							const float dt ) const
{
	if ( _slew_rate <= 0 ) return throttle;

	const float throttle_change_max = static_cast< float >( _slew_rate )*dt;

	return math::constrain_float( throttle,
		_val_old[__THROTTLE_FUNC] - throttle_change_max,
		_val_old[__THROTTLE_FUNC] + throttle_change_max
	);
}


float srv_channel_steer::
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
void srv_channel_steer::
_output( const bool  armed,
		 const float speed,
		 const float dt )
{
	float throttle = _val[ __THROTTLE_FUNC ],
		  steer    = _val[ __STEER_FUNC ];
	
	if ( ! armed ) {
		_set_output_limit( srv_channel::srv_func_e::THROTTLE,
						   srv_channel::limit_e::ZERO_PWM );
		goto _set_output;
	}
	
	_limit_mask = 0;

	if ( ! _apply_scaling ) {
		if ( math::is_negative( throttle ) ) steer *= -1.0f;
		goto _out_throttle;
	}
	
	if ( math::is_positive( _SPD_SCA_BASE )
	  && fabsf( speed ) > _SPD_SCA_BASE )
		steer *= _SPD_SCA_BASE/fabsf( speed );
	else
		_limit_mask |= _LIMIT_STEER_LEFT_MASK | _LIMIT_STEER_RIGHT_MASK;

	if ( math::is_negative( speed ) ) steer *= -1.0f;

_out_throttle:
	_output_throttle( throttle );
	
_set_output:
	_set_limits_from_input( steer, throttle );
	
	steer = math::constrain_float( steer, -_STEER_MAX, _STEER_MAX );

	_set_output_scaled( srv_channel::srv_func_e::STEER, steer );

	_calc_pwm();

	_output_ch_all();
}


inline void srv_channel_steer::
_output_ch_all( void )
{
	_throttle_ch.output();
	_steer_ch.output();
}


void srv_channel_steer::
_output_reverse_throttle( float throttle, const float delay )
{
	const uint32_t now_ms = millis();

	if ( ! math::is_zero( throttle ) ) {
		if ( throttle*_last_throttle < 0
		  && now_ms - _last_output_ms < delay*1000 )
			throttle = 0;
		else {
			_last_output_ms = now_ms;
			_last_throttle = throttle;
		}
	}

	_set_output_scaled( srv_channel::srv_func_e::THROTTLE, throttle );
}
	

void srv_channel_steer::
_output_throttle( float throttle, const float dt )
{
	throttle = _get_scaled_throttle( throttle );

	if ( _REV_DELAY > 0 )
		_output_reverse_throttle( throttle, _REV_DELAY );

	_set_output_scaled( srv_channel::srv_func_e::THROTTLE, throttle );
}


void srv_channel_steer::
_set_limits_from_input( const float steer,
						const float throttle )
{
	uint8_t _limit_steer_left = steer <= -_STEER_MAX ?
		                        _LIMIT_STEER_LEFT_MASK : 0;
	
	uint8_t _limit_steer_right = steer >= _STEER_MAX ?
		                         _LIMIT_STEER_RIGHT_MASK : 0;
	
	uint8_t _limit_throttle_lower = throttle <= -_THROTTLE_MAX ?
		                            _LIMIT_THROTTLE_LOWER_MASK : 0;
	
	uint8_t _limit_throttle_upper = throttle >= _THROTTLE_MAX ?
		                            _LIMIT_THROTTLE_UPPER_MASK : 0;
	
	_limit_mask |= _limit_steer_left
		        |  _limit_steer_right
		        |  _limit_throttle_lower
		        |  _limit_throttle_upper;
}


void srv_channel_steer::
_set_output_scaled( const srv_channel::srv_func_e func,
					const float                   val )
{
	int func_ix = func == srv_channel::srv_func_e::STEER ?
		          __STEER_FUNC : __THROTTLE_FUNC;

	_out_scaled[func_ix] = val;
}


void srv_channel_steer::
_set_output_limit( const srv_channel::srv_func_e func,
				   const srv_channel::limit_e    limit )
{
	srv_channel& ch = func == srv_channel::srv_func_e::STEER ?
		              _steer_ch : _throttle_ch;

	ch.set_output_limit( limit );
}


void srv_channel_steer::
_slew_limit_throttle( const float dt )
{
	const float throttle_old = _val[ __THROTTLE_FUNC ];

	_val[ __THROTTLE_FUNC ] = _get_slew_limited_throttle( _val[ __THROTTLE_FUNC ], dt );
	
	_val_old[ __THROTTLE_FUNC ] = _val[ __THROTTLE_FUNC ];
}


#endif // __INGNORE__
