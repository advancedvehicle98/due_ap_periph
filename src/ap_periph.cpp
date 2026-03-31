#include <ap_periph.h>

#include <ap_param.h>


ap_periph_t::
ap_periph_t( HardwareSerial& uart,
			 HardwareSerial& console,
			 CANRaw&         can,
			 navx_t&         navx )
	: _uart( uart ),
	  _console( console ),
	  _can( can ),
	  _navx( navx )
{
	
}


void ap_periph_t::
init( void )
{
	if ( _uart != _console ) _console.begin( _CONSOLE_BAUD );

	// _load_parameters();

	_navx.set_console( &_console );
	_navx.init();
	
	// Добавить инициализацию CAN
	
}


void ap_periph_t::
update( void )
{
#ifdef DEBUG 
	const Vector3f& print_data;
#endif 
		
	// Компас
	print_data = _navx.get_mag_vec3();
}
