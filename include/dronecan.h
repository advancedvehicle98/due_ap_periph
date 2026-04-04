// частичная реализация dronecan (canard) с due_can специальзированная
// под сообщения для ap_periph
// см. https://github.com/dronecan/libcanard/blob/601ed35467e0ac38819df17cd7c918de19f62d58/canard.c#L4


#ifndef __DRONECAN_H
#define __DRONECAN_H


#include <due_can.h>

#include <config.h>


class dronecan
{
public:
	enum class error_e
	{
		OK              = 0,
		RX_MISSED_START = 2,
		RX_INCOMPATIBLE_FRAME,
		RX_WRONG_ADDRESS
	};

	enum class trx_type_e
	{
		RESPONSE  = 0,
		REQUEST   = 1,
		BROADCAST = 2
	};

	dronecan( void ) = default;

	void init( void );
	error_e handle_rx_frame( CAN_FRAME& rx_msg );

	void set_node_id( const uint8_t id ) {
		if ( _node_id == _BROADCAST_ID
		  && _MIN_ID <= id && id <= _MAX_ID ) _node_id = id;
	}

private:
	static const uint8_t _BROADCAST_ID = 0;
	static const uint8_t _MIN_ID       = 1;
	static const uint8_t _MAX_ID       = 127;

	static const uint16_t _ANON_MSG_DATA_TYPE_ID_BIT_LEN = 2U;

	static const uint32_t _FRAME_EFF_MASK = 1UL << 31U;
	static const uint32_t _FRAME_RTR_MASK = 1UL << 30U;
	static const uint32_t _FRAME_ERR_MASK = 1UL << 29U;

	enum class protocol_e {
		_DYNAMIC_NODE_ID_ALLOCATION = 1
	};

	struct _rx_state {
		
	};
	
	uint8_t _node_id;
	uint8_t _pool[CONFIG_DRONECAN_POOL_SIZE];

	uint16_t _extract_data_type( const uint32_t id );
	uint16_t _extract_data_type( const uint32_t id, const trx_type_e trx_type, const uint8_t src_id );
	trx_type_e _extract_transfer_type( const uint32_t id );
	bool _should_accept(       uint64_t&  out_data_type_sig,
						 const uint16_t   data_type,
						 const trx_type_e trx_type );

	inline uint8_t
	_id_to_dest_id( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 8 ) & 0x7F );
	}

	inline uint16_t
	_id_to_msg_type( const uint32_t id )
	{
		return (uint16_t) ( ( id >> 8 ) & 0xFFFF );
	}

	inline uint8_t
	_id_to_priority( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 24 ) & 0x1F );
	}

	inline uint8_t
	_id_to_src_id( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 0 ) & 0x7F );
	}

	inline uint16_t
	_id_to_srv_type( const uint32_t id )
	{
		return (uint16_t) ( ( id >> 16 ) & 0xFF );
	}

	inline bool
	_is_trx_start( const uint8_t x )
	{
		return (bool) ( x >> 7 ) & 0x1;
	}

	inline uint32_t
	_make_trx_desc( const uint16_t  data_type,
					const trx_type_e trx_type,
					const uint8_t   src_id,
					const uint8_t   dest_id )
	{
		uint8_t trx_type_u = static_cast< uint8_t >( trx_type );
		
		return ( (uint32_t) data_type )
			 | ( (uint32_t) trx_type_u << 16 )
			 | ( (uint32_t) src_id     << 18 )
			 | ( (uint32_t) dest_id    << 25 );
	}
	
	inline bool
	_req_not_rest_from_id( const uint32_t id )
	{
		return (bool) ( ( id >> 15 ) & 0x1 );
	}

	inline bool
	_service_not_msg_from_id( const uint32_t id )
	{
		return (bool) ( ( id >> 7 ) & 0x1 );
	}
	
}; // class dronecan


#endif // __DRONECAN_H
