// Reference: https://github.com/ArduPilot/ardupilot/blob/a004f8b27b3f0847a0bc06a82f55f01c9f044110/libraries/AP_Param/AP_Param.h


#ifndef __AP_PARAM_H
#define __AP_PARAM_H

#ifdef CONFIG_LOAD_PARAMS


#include <stdint.h>

#include <Arduino.h>
#include <DueFlashStorage.h>


class ap_param_s
{
public:
	static void set_flash( DueFlashStorage *flash ) {
		_flash = flash;
	}

	static void set_console( HardwareSerial *console ) {
		_console = console;
	}
	
	static void setup_sketch_defaults( void );
	static bool setup( void );

private:

	struct __param_hdr {
		uint32_t key_low       : 8;
		uint32_t type          : 5;
		uint32_t key_high      : 1;
		uint32_t group_element : 18;
	};
	
	typedef uint32_t _param_hdr_t;
	
	static const uint32_t _FLASH_BASE = 0x0;
	
	static const uint8_t _FLASH_MAGIC0 = 'A';
	static const uint8_t _FLASH_MAGIC1 = 'P';
	static const uint8_t _FLASH_REV    = 0;

	static const uint8_t  _SENTINEL_TYPE  = 0x1F;
	static const uint16_t _SENTINEL_KEY   = 0x1FF;
	static const uint8_t  _SENTINEL_GROUP = 0xFF;

	static const uint32_t _SENTINEL_HDR =
		  (   _SENTINEL_KEY & 0xFF )
		| (   _SENTINEL_TYPE       << 8 )
		| ( ( _SENTINEL_KEY >> 8 ) << 13 )
	    | (   _SENTINEL_GROUP      << 14 );

	static_assert( ( _SENTINEL_HDR & ( 0b11111 << 8 ) ) >> 8 == _SENTINEL_TYPE,
				   "bad sentinel parameter header" );
	
	struct _flash_hdr { uint8_t magic[2], rev, spare; };

	static_assert( sizeof( struct _flash_hdr ) == 4,
				   "bad _flash_header size" );
	
	static DueFlashStorage *_flash;
	static HardwareSerial *_console;

	static void _erase_all( void );
	static void _write_sentinel( uint32_t off );
};


#endif // CONFIG_LOAD_PARAMS

#endif // ! __AP_PARAM_H
