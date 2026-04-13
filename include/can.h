#ifndef __CAN_H
#define __CAN_H


#include <config.h>
#include <hdefs.h>

#include <dronecan.h>

// надо для CAN_FRAME
#include <due_can.h>

#ifdef CONFIG_CAN_USE_MCP
#   include <mcp_can.h>
#   define CAN_BITRATE MCP_CAN_BITRATE( CONFIG_CAN_BITRATE )
#   define CAN_MHZ     MCP_CAN_MHZ( CONFIG_CAN_MCP_MHZ )
#else 
#   define CAN_BITRATE DUE_CAN_BITRATE( CONFIG_CAN_BITRATE )
#endif


class can {
public:
	void init( void );
	void update( void );

	bool is_ok( void ) const { return _ok; }
	
private:
	
	static bool _should_accept(        uint64_t&            out_data_type_sig,
								 const uint16_t             data_type,
								 const dronecan::trx_type_e trx_type );
	static void _on_reception( struct CanardRxTransfer& rx_trx );

	// protocol handlers
	static void _handle_alloc_response( struct CanardRxTransfer& rx_trx );
	static void _handle_get_node_info( struct CanardRxTransfer& rx_trx );
	
	static dronecan _dronecan;

	static uavcan_protocol_NodeStatus _node_status;

	static bool _ok;
	
#ifdef CONFIG_CAN_USE_MCP
	static MCP_CAN _if;
#else
	static CANRaw& _if;
#endif
	
	static inline uint16_t _available( void );
	static inline void _get_sys_uid( uint8_t *uid, const uint8_t len );
	static void _process_rx( void );
	static inline uint32_t _read( CAN_FRAME& f );
	static inline void _read_uid( uint8_t *uid );
	static bool _respond( struct CanardRxTransfer& rx_trx,
						  const uint64_t data_type_sig,
						  const uint16_t data_type_id,
						  const uint8_t *paylosd,
						  const uint16_t payload_len );
};


#endif // ! __CAN_H
