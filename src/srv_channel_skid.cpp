#include <srv_channel_skid.h>


srv_channel_skid::
srv_channel_skid(  )
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
}


void srv_channel_skid::
update( void )
{
	_corking = true;
	_calc_pwm();
}
