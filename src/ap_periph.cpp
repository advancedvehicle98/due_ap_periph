#include <ap_periph.h>

#include <ap_param.h>


ap_periph_t::
ap_periph_t( HardwareSerial& uart,
			 HardwareSerial& console )
	: _uart( uart ),
	  _console( console )
{
	
}


void ap_periph_t::
init( void )
{
	if ( _uart != _console ) _console.begin( _CONSOLE_BAUD );

	_load_parameters();
	
	// Добавить инициализацию CAN
}


void ap_periph_t::
update( void )
{
	
}
