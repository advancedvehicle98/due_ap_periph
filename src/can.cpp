#include <ap_periph.h>

#include <dronecan.h>

#include <due_can.h>



void ap_periph_t::
_can_init( void )
{
	_can.setRxBufferSize( CONFIG_CAN_RX_BUFFER_SIZE );
		
	_can.begin( CONFIG_CAN_BITRATE );
	_can.watchFor();

	_dronecan.init();

#ifdef DEBUG
	_console.println( "can ok" );
#endif
}


void ap_periph_t::
_can_process_rx( void )
{
	if ( _can.available() <= 0 ) return;

	CAN_FRAME rx_msg;

	while ( true ) {
		if ( ! _can.read( rx_msg ) ) break;

		dronecan::error_e status = _dronecan.handle_rx_frame( rx_msg );

		if ( status == dronecan::error_e::RX_MISSED_START );
	}
}


void ap_periph_t::
_can_update( void )
{
	_can_process_rx();
}
