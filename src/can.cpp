#include <can.h>

#include <math.h>
#include <dronecan.h>

#include <dronecan/dronecan_msgs.h>


dronecan can::_dronecan;
uavcan_protocol_NodeStatus can::_node_status;

bool can::_ok = false;

#ifdef CONFIG_CAN_USE_MCP
MCP_CAN can::_if( CONFIG_CAN_MCP_CS );
#else
CANRaw& can::_if( CONFIG_CAN_IF );
#endif


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
	
	_dronecan.init( _should_accept, _on_reception );

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


// statics
//---------------------------------------------

bool can::
_should_accept(       uint64_t&            out_data_type_sig,
 			    const uint16_t             data_type,
			    const dronecan::trx_type_e trx_type )
{
	if ( _dronecan.get_node_id() == dronecan::BROADCAST_ID ) {
		if ( trx_type == dronecan::trx_type_e::BROADCAST
		  && data_type == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID ) {
			out_data_type_sig = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE;
			return true;
		}

		return false;
	}

	out_data_type_sig = dronecan::data_type_to_sig( data_type );
	return out_data_type_sig != -1UL;
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


inline void can::
_get_sys_uid( uint8_t *uid, const uint8_t len )
{
	memcpy( uid, (const void *) ATSAM_UDID_START, min( 12, len ) );
}


void can::
_handle_alloc_response( struct CanardRxTransfer& rx_trx )
{
	_dronecan.send_next_node_id_alloc_request_at_ms =
		  millis()
		+ UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS
		+ (   math::get_random16()
		    % UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_FOLLOWUP_DELAY_MS );

	if ( rx_trx.source_node_id == dronecan::BROADCAST_ID ) {
		_dronecan.node_id_alloc_unique_id_offset = 0;
		return;
	}
	
	uavcan_protocol_dynamic_node_id_Allocation msg;

	if ( uavcan_protocol_dynamic_node_id_Allocation_decode( &rx_trx, &msg ) )
		return;

	if      ( msg.unique_id.len < sizeof( msg.unique_id.data ) ) {
		_dronecan.node_id_alloc_unique_id_offset = msg.unique_id.len;
		_dronecan.send_next_node_id_alloc_request_at_ms -=
			UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS;
		return;
	}

	if ( msg.node_id == dronecan::BROADCAST_ID ) return;

	_dronecan.set_node_id( msg.node_id );
}


void can::
_handle_get_node_info( struct CanardRxTransfer& rx_trx )
{
	uint8_t buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
	uavcan_protocol_GetNodeInfoResponse response{};

	_node_status.uptime_sec = millis()/1000U;

	response.status                 = _node_status;
	response.software_version.major = FW_VER_MAJOR;
	response.software_version.minor = FW_VER_MINOR;

	_read_uid( response.hardware_version.unique_id );

	response.name.len = strlen( (char *) response.name.data );

	uint16_t total_size = uavcan_protocol_GetNodeInfoResponse_encode( &response, buffer );

	_respond( rx_trx,
			  UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
			  UAVCAN_PROTOCOL_GETNODEINFO_ID,
			  buffer,
			  total_size );
}


void can::
_on_reception( struct CanardRxTransfer& rx_trx )
{
	if ( _dronecan.get_node_id() == dronecan::BROADCAST_ID ) {
		if ( rx_trx.transfer_type == static_cast< uint8_t >( dronecan::trx_type_e::BROADCAST )
		  && rx_trx.data_type_id == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID )
			_handle_alloc_response( rx_trx );
		
		return;
	}

	switch ( rx_trx.data_type_id ) {

	case UAVCAN_PROTOCOL_GETNODEINFO_ID : {
		_handle_get_node_info( rx_trx );
	} break;
		
	// case UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID :
	// 	return UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE;
		
	// case UAVCAN_PROTOCOL_RESTARTNODE_ID : 
	// 	return UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE;
		
	// case UAVCAN_PROTOCOL_PARAM_GETSET_ID :
	// 	return UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
		
	// case UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID :
	// 	return UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE;
		
	// case UAVCAN_EQUIPMENT_SAFETY_ARMINGSTATUS_ID :
	// 	return UAVCAN_EQUIPMENT_SAFETY_ARMINGSTATUS_SIGNATURE;

	// case UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID:
	// 	return UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE;

	// case UAVCAN_EQUIPMENT_ACTUATOR_ARRAYCOMMAND_ID:
	// 	return UAVCAN_EQUIPMENT_ACTUATOR_ARRAYCOMMAND_SIGNATURE;

	// case ARDUPILOT_INDICATION_NOTIFYSTATE_ID:
	// 	return ARDUPILOT_INDICATION_NOTIFYSTATE_SIGNATURE;
		
	}
}


void can::
_process_rx( void )
{
	if ( _available() <= 0 ) return;

	CAN_FRAME rx_msg;

	while ( true ) {
		if ( ! _read( rx_msg ) ) break;
		
		unsigned long ts_usec = micros();

		dronecan::error_e status = _dronecan.handle_rx_frame( rx_msg, ts_usec );

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


inline void can::
_read_uid( uint8_t *uid )
{
	uint8_t len = sizeof( uavcan_protocol_dynamic_node_id_Allocation::unique_id.data );

	memset( uid, 0, len );
	_get_sys_uid( uid, len );
}


bool can::
_respond(       struct CanardRxTransfer&  rx_trx,
		  const uint64_t                  data_type_sig,
		  const uint16_t                  data_type_id,
		  const uint8_t                  *payload,
		  const uint16_t                  payload_len )
{
	CanardTxTransfer tx_trx;

	tx_trx.transfer_type       = CanardTransferTypeResponse;
	tx_trx.data_type_signature = data_type_sig;
	tx_trx.data_type_id        = data_type_id;
	tx_trx.inout_transfer_id   = &rx_trx.transfer_id;
	tx_trx.priority            = rx_trx.priority;
	tx_trx.payload             = payload;
	tx_trx.payload_len         = payload_len;
	// tx_trx.deadline_usec       = (uint64_t) ( micros() + can::FRAME_TIMEOUT );
}
