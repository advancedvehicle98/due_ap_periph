#include <Arduino.h>

#include <ap_periph.h>


static constexpr int INITIAL_BAUD = 115200;


ap_periph_t ap_periph;


void
setup( void )
{
	Serial.begin(INITIAL_BAUD);

#ifdef DEBUG	
	Serial.println( "starting ap_periph" );
#endif

#ifdef CONFIG_LOAD_PARAMS
	ap_param_s::set_console( &Serial );
#endif 
	
#ifdef DEBUG
	Serial.println( "initializing ap_periph" );
#endif
	
	ap_periph.init();
}


void
loop( void )
{
		
}
