#ifndef __AP_PARAM_H
#define __AP_PARAM_H


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
	static const uint8_t _FLASH_MAGIC0 = 'A';
	static const uint8_t _FLASH_MAGIC1 = 'P';
	static const uint8_t _FLASH_REV    = 0;
	
	static DueFlashStorage *_flash;
	static HardwareSerial *_console;
		
	struct _flash_header {
		uint8_t magic[2], rev, spare;
	};

	static_assert(
		sizeof( struct _flash_header ) == 4,
		"bad _flash_header size"
	);
};


#endif // ! __AP_PARAM_H
