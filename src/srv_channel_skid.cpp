#include <srv_channel_skid.h>

#include <initializer_list>

#include <math.h>


srv_channel_skid::
srv_channel_skid(  )
	: _throttle_ch( srv_func_e::THROTTLE )
	, _steer_ch( srv_func_e::STEER )
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
	_output( true,  );
}


// Private functions
//-------------------------------------------------------

float srv_channel_skid::
_get_slew_limited_throttle( const float throttle,
							const float dt )
{
	if ( _slew_rate <= 0 ) return throttle;

	const float throttle_change_max = static_cast< float >( _slew_rate )*dt;

	return math::constrain_float( throttle,
		_val_old[__THROTTLE_FUNC] - throttle_change_max,
		_val_old[__THROTTLE_FUNC] + throttle_change_max
	);
}


// см. AP_MotorsUGV::output_regular
void srv_channel_skid::
_output( const bool  armed,
		 const float speed,
		 const float dt )
{
	if ( ! armed ) _val[ __THROTTLE_FUNC ] = 0.0f;
	
	_limit_mask = 0;

	
}


void srv_channel_skid::
_slew_limit_throttle( const float dt )
{
	const float throttle_old = _val[ __THROTTLE_FUNC ];

	_val[ __THROTTLE_FUNC ] = _get_slew_limited_throttle( _throttle, dt );
	
	_val_old[ __THROTTLE_FUNC ] = _val[ __THROTTLE_FUNC ];
}
