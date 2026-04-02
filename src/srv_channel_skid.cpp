#include <srv_channel_skid.h>

#include <initializer_list>

#include <math.h>


srv_channel_skid::
srv_channel_skid(  )
	: _throttle_ch( srv_channel::srv_func_e::THROTTLE )
	, _steer_ch( srv_channel::srv_func_e::STEER )
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


void srv_channel_skid::
init( void )
{
#ifdef CONFIG_USE_MXP_MD2
	_md_right.init();
	_md_left.init();
#endif
}


void srv_channel_skid::
update( void )
{
	// _output( true,  );
}


// Private functions
//-------------------------------------------------------

float srv_channel_skid::
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
	
	// if ( math::is_zero(  ) )
}


// см. AP_MotorsUGV::output_regular
void srv_channel_skid::
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

_out_throttle:
	_output_throttle( throttle );
	
_set_output:
	_set_limits_from_input( steer, throttle );
	
	steer = math::constrain_float( steer, -_STEER_MAX, _STEER_MAX );

	_set_output_scaled( srv_channel::srv_func_e::STEER, steer );
}


void srv_channel_skid::
_output_throttle( float throttle )
{
	throttle = _get_scaled_throttle( throttle );
}


void srv_channel_skid::
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


void srv_channel_skid::
_set_output_scaled( const srv_channel::srv_func_e func,
					const float                   val )
{
	int func_ix = func == srv_channel::srv_func_e::STEER ?
		          __STEER_FUNC : __THROTTLE_FUNC;

	_out_scaled[func_ix] = val;
}


void srv_channel_skid::
_set_output_limit( const srv_channel::srv_func_e func,
				   const srv_channel::limit_e    limit )
{
	srv_channel& ch = func == srv_channel::srv_func_e::STEER ?
		              _steer_ch : _throttle_ch;

	ch.set_output_limit( limit );
}


void srv_channel_skid::
_slew_limit_throttle( const float dt )
{
	const float throttle_old = _val[ __THROTTLE_FUNC ];

	_val[ __THROTTLE_FUNC ] = _get_slew_limited_throttle( _val[ __THROTTLE_FUNC ], dt );
	
	_val_old[ __THROTTLE_FUNC ] = _val[ __THROTTLE_FUNC ];
}
