#include <ap_periph.h>



void ap_periph_t::
_load_parameters( void )
{
#ifdef DEBUG
	_console.println( "loading params" );
#endif
	
	ap_param_s::setup_sketch_defaults();

	// TODO: доделать загрузку параметров
}
