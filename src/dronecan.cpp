#include <dronecan.h>


using trx_type_e = dronecan::trx_type_e;
using error_e = dronecan::error_e;


error_e dronecan::
handle_rx_frame( CAN_FRAME& rx_msg )
{
	const trx_type_e trx_type = _extract_transfer_type( rx_msg.id );

	const uint8_t dest_id = trx_type == trx_type_e::BROADCAST
		                  ? (uint8_t) _BROADCAST_ID
		                  : _id_to_dest_id( rx_msg.id );

	if ( ! ( rx_msg.id & _FRAME_EFF_MASK )
	    || ( rx_msg.id & _FRAME_RTR_MASK )
		|| ( rx_msg.id & _FRAME_ERR_MASK )
		|| ( rx_msg.length < 1 ) )
		return error_e::RX_INCOMPATIBLE_FRAME;
	
	if ( trx_type != trx_type_e::BROADCAST
	  && dest_id != _node_id )
		return error_e::RX_WRONG_ADDRESS;

	const uint8_t  priority  = _id_to_priority( rx_msg.id );
	const uint8_t  src_id    = _id_to_src_id( rx_msg.id );
	const uint16_t data_type = _extract_data_type( rx_msg.id, trx_type, src_id );

	const uint32_t trx_desc = _make_trx_desc( data_type, trx_type, src_id, dest_id );

	const uint8_t tail_byte = rx_msg.data.uint8[rx_msg.length-1];

	uint64_t data_type_sig = 0;
	struct _rx_state *rx_state = nullptr;

	if ( _is_trx_start( tail_byte ) )
		if ( _should_accept( data_type_sig, data_type, trx_type ) );
	else {
		
	}
	
	return error_e::OK;
}


void dronecan::
init( void )
{
	_node_id = _BROADCAST_ID;
}


// Private functions
//------------------------------------------------------------------

uint16_t dronecan::
_extract_data_type( const uint32_t id )
{
	const trx_type_e trx_type = _extract_transfer_type( id );
	const uint8_t   src_id  = _id_to_src_id( id );
	
	return _extract_data_type( id, trx_type, src_id );
}

uint16_t dronecan::
_extract_data_type( const uint32_t  id,
					const trx_type_e trx_type,
					const uint8_t   src_id )
{
	if ( trx_type == trx_type_e::BROADCAST ) {
		uint16_t msg_type = _id_to_msg_type( id );

		if ( src_id == _BROADCAST_ID )
			msg_type &= ( 1 << _ANON_MSG_DATA_TYPE_ID_BIT_LEN ) - 1;

		return msg_type;
	}

	return _id_to_srv_type( id );
}


trx_type_e dronecan::
_extract_transfer_type( const uint32_t id )
{
	if      ( ! _service_not_msg_from_id( id ) ) return trx_type_e::BROADCAST;
	else if ( ! _req_not_rest_from_id( id ) )    return trx_type_e::REQUEST;
	else                                         return trx_type_e::RESPONSE;
}


bool dronecan::
_should_accept(       uint64_t&  out_data_type_sig,
			    const uint16_t   data_type,
			    const trx_type_e trx_type )
{
	if ( _node_id == _BROADCAST_ID ) {
		if ( trx_type == trx_type_e::BROADCAST
		  && data_type == protocol_e::_DYNAMIC_NODE_ID_ALLOCATION ) {
				
		}

		return false;
	}
}
