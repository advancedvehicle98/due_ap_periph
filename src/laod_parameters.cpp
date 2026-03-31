#include <ap_periph.h>



void ap_periph_t::
_load_parameters( void )
{
	_console.println( "loading params" );
	
	ap_param_s::setup_sketch_defaults();
}
