#include <can.h>

#include <dronecan.h>



can::
can( void )
	:
#ifdef CONFIG_CAN_USE_MCP
	  _if( CONFIG_CAN_MCP_CS )
#else
	  _if( CONFIG_CAN_IF )
#endif
{
		
}



void can::
init( void )
{
	int status;
	
#ifdef CONFIG_CAN_USE_MCP
	status = _if.begin( MCP_ANY, CONFIG_CAN_BITRATE, CAN_MHZ );
	if ( status != CAN_OK ) goto _can_error;
	
	_if.setMode( MCP_NORMAL );
#else
	_if.setRxBufferSize( CONFIG_CAN_RX_BUFFER_SIZE );

	status = _if.begin( CONFIG_CAN_BITRATE );
	if ( status == 0 ) goto _can_error;
	
	_if.watchFor();
#endif
	
	_dronecan.init();

#ifdef DEBUG
	_console.println( "can ok" );
#endif

	_ok = true;

 _can_error:
	Serial.println( "can fail" );
	return;
}


void can::
update( void )
{
	_process_rx();
}


// private parts
//------------------------------------------------

inline uint16_t can::
_available( void )
{
#ifdef CONFIG_CAN_USE_MCP
	return _if.checkReceive() == CAN_MSGAVAIL ? 1 : 0;
#else
	return _if.available();
#endif
}


void can::
_process_rx( void )
{
	if ( _available() <= 0 ) return;

	CAN_FRAME rx_msg;

	while ( true ) {
		if ( ! _read( rx_msg ) ) break;

		dronecan::error_e status = _dronecan.handle_rx_frame( rx_msg );

		if ( status == dronecan::error_e::RX_MISSED_START );
	}
}


inline uint32_t can::
_read( CAN_FRAME& f )
{
#ifdef CONFIG_CAN_USE_MCP
	return (uint32_t) _if.readMsgBuf( &f.id, &f.length, f.data.uint8 );
#else
	return _if.read( f );
#endif		
}
