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

	if ( _is_trx_start( tail_byte ) ) {
		if ( ! _should_accept( data_type_sig, data_type, trx_type ) )
			return error_e::RX_NOT_WANTED;

		rx_state = _traverse_rx_states( trx_desc );

		if ( ! rx_state ) return error_e::OUT_OF_MEMORY;
	}
	else {
		rx_state = _find_rx_state( trx_desc );

		if ( rx_state ) return error_e::RX_MISSED_START;

		if ( ! _should_accept( data_type_sig, data_type, trx_type ) )
			return error_e::RX_NOT_WANTED;
	}
	
	return error_e::OK;
}


void dronecan::
init( void )
{
	_node_id = _BROADCAST_ID;

	_init_pool_allocator( CONFIG_DRONECAN_POOL_SIZE / _MEM_BLOCK_SIZE );
}


// Private functions
//------------------------------------------------------------------


void *dronecan::
_allocate_block( void )
{
	void *block = _allocator.free_list;
	
	if ( ! block ) return nullptr;

	_allocator.free_list = _allocator.free_list->next;
	++_allocator.stats.current_usage_blocks;

	if ( _allocator.stats.peak_usage_blocks < _allocator.stats.current_usage_blocks )
		_allocator.stats.peak_usage_blocks = _allocator.stats.current_usage_blocks;
	
	return block;
}


struct dronecan::_rx_state *dronecan::
_create_rx_state( const uint32_t trx_desc )
{
	struct _rx_state state_init( nullptr, nullptr, trx_desc );
	struct _rx_state *new_state = (struct _rx_state *) _allocate_block();

	if ( ! new_state ) return nullptr;

	memcpy( new_state, &state_init, sizeof( struct _rx_state ) );
	
	return new_state;
}


uint64_t dronecan::
_data_type_to_sig( const uint16_t data_type ) const
{
	switch ( data_type ) {
		
	case UAVCAN_PROTOCOL_GETNODEINFO_ID :
		return UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE;
		
	case UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID :
		return UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE;
		
	case UAVCAN_PROTOCOL_RESTARTNODE_ID : 
		return UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE;
		
	case UAVCAN_PROTOCOL_PARAM_GETSET_ID :
		return UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
		
	case UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID :
		return UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE;
		
	case UAVCAN_EQUIPMENT_SAFETY_ARMINGSTATUS_ID :
		return UAVCAN_EQUIPMENT_SAFETY_ARMINGSTATUS_SIGNATURE;

	case UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID:
		return UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE;

	case UAVCAN_EQUIPMENT_ACTUATOR_ARRAYCOMMAND_ID:
		return UAVCAN_EQUIPMENT_ACTUATOR_ARRAYCOMMAND_SIGNATURE;

	case ARDUPILOT_INDICATION_NOTIFYSTATE_ID:
		return ARDUPILOT_INDICATION_NOTIFYSTATE_SIGNATURE;
		
	}
	
	return -1UL;
}


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


struct dronecan::_rx_state *dronecan::
_find_rx_state( const uint32_t trx_desc )
{
	struct _rx_state *state = _rx_states;

	while ( state ) {
		if ( state->trx_desc == trx_desc ) break;
		state = (struct _rx_state *) state->next;
	}

	return state;
}


void dronecan::
_init_pool_allocator( const uint16_t buf_len )
{
	size_t ix = 0;
	union _pool_alloc_block *alloc_buf = (union _pool_alloc_block *) _pool;
	union _pool_alloc_block **current_block = &( _allocator.free_list );

	_allocator.arena = (void *) _pool;

	while ( ix++ < buf_len ) {
		*current_block = &alloc_buf[ix];
		current_block = &( ( *current_block )->next );
	}

	*current_block = nullptr;

	_allocator.stats.capacity_blocks      = buf_len;
	_allocator.stats.current_usage_blocks = 0;
	_allocator.stats.peak_usage_blocks    = 0;
}


struct dronecan::_rx_state *dronecan::
_prepend_rx_state( const uint32_t trx_desc )
{
	struct _rx_state *state = _create_rx_state( trx_desc );

	if ( ! state ) return nullptr;

	state->next = _rx_states;
	_rx_states = state;

	return state;
}


bool dronecan::
_should_accept(       uint64_t&  out_data_type_sig,
			    const uint16_t   data_type,
			    const trx_type_e trx_type )
{
	if ( _node_id == _BROADCAST_ID ) {
		if ( trx_type == trx_type_e::BROADCAST
		  && data_type == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID ) {
			out_data_type_sig = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE;
			return true;
		}

		return false;
	}

	out_data_type_sig = _data_type_to_sig( data_type );
	return out_data_type_sig != -1UL;
}


struct dronecan::_rx_state *dronecan::
_traverse_rx_states( const uint32_t trx_desc )
{
	if ( ! _rx_states ) {
		_rx_states = _create_rx_state( trx_desc );
		if ( ! _rx_states ) return nullptr;
		return _rx_states;
	}

	struct _rx_state *state = _find_rx_state( trx_desc );

	return state ? state : _prepend_rx_state( trx_desc );
}
