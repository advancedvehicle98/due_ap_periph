#include <ap_param.h>

#include <Arduino.h>


DueFlashStorage *ap_param_s::_flash;
HardwareSerial *ap_param_s::_console;


bool ap_param_s::
setup( void )
{
	struct _flash_header hdr;

	for ( uint32_t i = 0x0; i < sizeof( struct _flash_header ); ++i )
		( (uint8_t *) &hdr )[i] = _flash->read( i );

	if ( hdr.magic[0] != _FLASH_MAGIC0
	  || hdr.magic[1] != _FLASH_MAGIC1
	  || hdr.rev      != _FLASH_REV )
		_console->println( "restoring params" );
	
	return true;
}


void ap_param_s::
setup_sketch_defaults( void )
{
	setup();
}
