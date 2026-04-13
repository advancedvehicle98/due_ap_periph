#ifndef __HDEFS_H
#define __HDEFS_H


#define FW_VER_MAJOR 0
#define FW_VER_MINOR 0


// для due_can
#include <due_can.h>

#define CAN_IF0 Can0
#define CAN_IF1 Can1

// прим. имена CAN0 и CAN1 уже заняты cmsis для atsam3xa

#define CAN_MBOX_COUNT 8


/* #define DUE_CAN_BITRATE( b ) CAN_BPS_##b##K */
/* #define MCP_CAN_BITRATE( b ) CAN_##b##KBPS */

/* #define MCP_CAN_MHZ( f ) MCP_##f##MHZ */

#define DUE_CAN_BITRATE( b ) __CONCAT( CAN_BPS_, \
							 __CONCAT( b, \
									   K) )

#define MCP_CAN_BITRATE( b ) __CONCAT( CAN_, \
							 __CONCAT( b, \
									   KBPS ) )

#define MCP_CAN_MHZ( f ) __CONCAT( MCP_, \
						 __CONCAT( f, \
								   MHZ) )


#define ATSAM_UDID_START 0x285E0A60


#endif // ! __HDEFS_H
