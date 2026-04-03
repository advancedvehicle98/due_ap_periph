#include <ap_periph.h>

#include <ap_param.h>


ap_periph_t::
ap_periph_t( HardwareSerial& uart
		   , HardwareSerial& console
		   , CANRaw&         can
#if defined( CONFIG_USE_NAVX ) && defined( CONFIG_STATIC_NAVX )
		   , navx_t&         navx
#endif
#ifdef CONFIG_USE_FLYSKY
		   , HardwareSerial& ibus
#endif
		   )
	: _uart( uart )
	, _console( console )
	, _can( can )
#if defined( CONFIG_USE_NAVX ) && defined( CONFIG_STATIC_NAVX )
	, _navx( navx )
#endif
#ifdef CONFIG_USE_FLYSKY
	, _flysky( ibus )
#endif
{
	
}


void ap_periph_t::
init( void )
{
	if ( _uart != _console ) _console.begin( _CONSOLE_BAUD );

#ifdef CONFIG_LOAD_PARAMS
	_load_parameters();
#endif

#ifdef CONFIG_USE_NAVX
	_navx.set_console( &_console );
	_navx.init();
#endif
	
	_motors.set_console( &_console );

	_flysky.begin();
	// Добавить инициализацию CAN
	
}


void ap_periph_t::
update( void )
{
	// const Vector3f& print_data;
		
	// // Компас
	// print_data = _navx.get_mag_vec3();
#ifdef CONFIG_USE_NAVX
	_navx.update();
#endif

	int16_t ibus_mode = _flysky.getChannel( CONFIG_FLYSKY_CH_MODE );

	bool armed = ibus_mode != CONFIG_FLYSKY_DISABLED;
	
	if ( armed ) {
		int16_t ibus_throttle = _flysky.getChannel( CONFIG_FLYSKY_CH_THROTTLE ) - 1500;
		int16_t ibus_steer    = _flysky.getChannel( CONFIG_FLYSKY_CH_STEER ) - 1500;

		float throttle = (float) ibus_throttle/500;
		float steer = (float) ibus_steer*9; // 4500/500 ( _STEER_MAX/_IBUS_MAX )

#ifdef DEBUG
		_console.print( "throttle: " );
		_console.println( throttle );

		_console.print( "steer: " );
		_console.println( steer );
#endif
		
		_motors.set_throttle( throttle );
		_motors.set_steer( steer, false );
	}

	_motors.update( armed, CONFIG_DT );
}
