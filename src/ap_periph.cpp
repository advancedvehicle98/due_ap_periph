#include <ap_periph.h>

#include <ap_param.h>


ap_periph_t::
ap_periph_t( HardwareSerial& uart,
			 HardwareSerial& console,
			 CANRaw&         can,
#ifdef CONFIG_USE_NAVX
			 navx_t&         navx
#endif
			 )
	: _uart( uart )
	, _console( console )
	, _can( can )
#ifdef CONFIG_USE_NAVX
	, _navx( navx )
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

	_motors.update();
}
