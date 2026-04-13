// частичная реализация dronecan (canard) с due_can специальзированная
// под сообщения для ap_periph
// см. https://github.com/dronecan/libcanard/blob/601ed35467e0ac38819df17cd7c918de19f62d58/canard.c#L4


#ifndef __DRONECAN_H
#define __DRONECAN_H


#include <config.h>

#include <dronecan/dronecan_msgs.h>


class dronecan
{	
public:
	enum class trx_type_e
	{
		RESPONSE  = 0,
		REQUEST   = 1,
		BROADCAST = 2
	};
	
	enum class error_e
	{
		OK              = 0,
		RX_MISSED_START = 2,
		RX_INCOMPATIBLE_FRAME,
		RX_WRONG_ADDRESS,
		RX_NOT_WANTED,
		OUT_OF_MEMORY
	};
	
	static const uint8_t BROADCAST_ID = 0;
	static const uint8_t MIN_ID       = 1;
	static const uint8_t MAX_ID       = 127;
	
	typedef bool ( * should_accept_t )( uint64_t&, const uint16_t , const trx_type_e );
	typedef void ( * on_reception_t )( struct CanardRxTransfer& rx_trx );

	dronecan( void ) = default;

	uint32_t send_next_node_id_alloc_request_at_ms;
	uint8_t node_id_alloc_unique_id_offset;
	
	static uint64_t data_type_to_sig( const uint16_t );

	static inline uint8_t
	id_to_dest_id( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 8 ) & 0x7F );
	}

	static inline uint16_t
	id_to_msg_type( const uint32_t id )
	{
		return (uint16_t) ( ( id >> 8 ) & 0xFFFF );
	}

	static inline uint8_t
	id_to_priority( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 24 ) & 0x1F );
	}

	static inline uint8_t
	id_to_src_id( const uint32_t id )
	{
		return (uint8_t) ( ( id >> 0 ) & 0x7F );
	}

	static inline uint16_t
	id_to_srv_type( const uint32_t id )
	{
		return (uint16_t) ( ( id >> 16 ) & 0xFF );
	}

	static inline bool
	is_trx_end( const uint8_t x )
	{
		return (bool) ( x >> 6 ) & 0x1;
	}

	static inline bool
	is_trx_start( const uint8_t x )
	{
		return (bool) ( x >> 7 ) & 0x1;
	}
	
	static inline uint32_t
	make_trx_desc( const uint16_t    data_type,
					const trx_type_e trx_type,
					const uint8_t     src_id,
					const uint8_t     dest_id )
	{
		uint8_t trx_type_u = static_cast< uint8_t >( trx_type );
		
		return ( (uint32_t) data_type )
			 | ( (uint32_t) trx_type_u << 16 )
			 | ( (uint32_t) src_id     << 18 )
			 | ( (uint32_t) dest_id    << 25 );
	}
	
	static inline bool
	req_not_rest_from_id( const uint32_t id )
	{
		return (bool) ( ( id >> 15 ) & 0x1 );
	}

	static inline bool
	service_not_msg_from_id( const uint32_t id )
	{
		return (bool) ( ( id >> 7 ) & 0x1 );
	}

	static inline uint8_t
	tail_byte_to_trx_id( const uint8_t tail_byte )
	{
		return tail_byte & 0x1u;
	}

	const uint8_t get_node_id( void ) const { return _node_id; }

	void set_node_id( const uint8_t id ) {
		if ( _node_id == BROADCAST_ID
		  && MIN_ID <= id && id <= MAX_ID ) _node_id = id;
	}

	void init( should_accept_t should_accept,
			   on_reception_t  on_reception );
	error_e handle_rx_frame( CAN_FRAME& rx_msg, const unsigned long ts_usec );

private:

	static const uint16_t _ANON_MSG_DATA_TYPE_ID_BIT_LEN = 2U;

	static const uint32_t _FRAME_EFF_MASK = 1UL << 31U;
	static const uint32_t _FRAME_RTR_MASK = 1UL << 30U;
	static const uint32_t _FRAME_ERR_MASK = 1UL << 29U;

	static const int _MEM_BLOCK_SIZE = 32;
	static const int _TRX_ID_LEN = 5;
	static const int _PAYLOAD_LEN = 10;

	static const unsigned long _TRX_TIMEOUT_USEC = 2000000U;
	
	struct _rx_state {
		struct _rx_state *next;
		void             *buffer_blocks;
		uint32_t          ts_usec;

		unsigned calc_crc    : 16;
		unsigned payload_len : _PAYLOAD_LEN;
		unsigned trx_id      : _TRX_ID_LEN;
		unsigned next_toggle : 1;
		
		const uint32_t trx_desc;
		
		_rx_state(       struct _rx_state *n,
				         void             *bb,
				   const uint32_t          td )
			: next( n )
			, buffer_blocks( bb )
			, trx_desc( td )
		{		
		}
	};

	struct _tx_queue {
		
	};

	struct _pool_alloc_block {
		uint8_t bytes[_MEM_BLOCK_SIZE];
		struct _pool_alloc_block *next;
	};

	struct _pool_alloc_stats {
		uint16_t capacity_blocks, current_usage_blocks, peak_usage_blocks;
	};

	struct _pool_allocator {
		void                     *arena;
		struct _pool_alloc_block  *free_list;
		struct _pool_alloc_stats  stats;
	};

	struct _buffer_block {
		struct _buffer_block *next;
		uint8_t *data;
	};
	
	uint8_t _node_id;
	
	uint8_t _pool[CONFIG_DRONECAN_POOL_SIZE];
	struct _pool_allocator _allocator;

	struct _rx_state *_rx_states = nullptr;
	struct _tx_queue *_tx_queue = nullptr;

	should_accept_t _should_accept;
	on_reception_t _on_reception;
	
	void *_allocate_block( void );
	struct _rx_state *_create_rx_state( const uint32_t trx_desc );
	uint16_t _extract_data_type( const uint32_t id );
	uint16_t _extract_data_type( const uint32_t id, const trx_type_e trx_type, const uint8_t src_id );
	trx_type_e _extract_transfer_type( const uint32_t id );
	struct _rx_state *_find_rx_state( const uint32_t trx_desc );
	void _free_block( void *p );
	int16_t _get_trx_id_fwd_distance( const uint8_t a, const uint8_t b );
	void _init_pool_allocator( const uint16_t buf_len );
	struct _rx_state * _prepend_rx_state( const uint32_t trx_desc );
	void _release_state_payload( struct _rx_state *state );
	struct _rx_state *_traverse_rx_states( const uint32_t trx_desc );
	
}; // class dronecan


#endif // __DRONECAN_H
