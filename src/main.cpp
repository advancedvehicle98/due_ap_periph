#include <Arduino.h>

#include <ap_periph.h>


static constexpr int INITIAL_BAUD = 115200;


ap_periph_t ap_periph;

DueFlashStorage due_flash;


void
setup( void )
{
	Serial.begin(INITIAL_BAUD);
	
	Serial.println( "starting ap_periph" );
	
	ap_param_s::set_console( &Serial );
	ap_param_s::set_flash( &due_flash );
	
	Serial.println( "initializing ap_periph" );
	
	ap_periph.init();
}


void
loop( void )
{
		
}
