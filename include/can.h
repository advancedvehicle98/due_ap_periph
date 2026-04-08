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
	can( void );
	
	void init( void );
	void update( void );

	bool is_ok( void ) const { return _ok; }
	
private:
	bool _ok = false;
	
	dronecan _dronecan;

#ifdef CONFIG_CAN_USE_MCP
	MCP_CAN _if;
#else
	CANRaw& _if;
#endif

	inline uint16_t _available( void );
	void _process_rx( void );
	inline uint32_t _read( CAN_FRAME& f );
};


#endif // ! __CAN_H
