#include <navx.h>


navx_t navx0;


navx_t::
navx_t( TwoWire& wire )
	: _wire(wire)
{
	
}


void navx_t::
init( void )
{
	Wire.begin();

#ifdef DEBUG
	_console->println( "navx success" );
#endif
}


void navx_t::
update( void )
{
	
}
