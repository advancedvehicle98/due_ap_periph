// Reference: https://github.com/ArduPilot/ardupilot/blob/a004f8b27b3f0847a0bc06a82f55f01c9f044110/libraries/AP_Param/AP_Param.cpp

#ifdef CONFIG_LOAD_PARAMS

#include <ap_param.h>

#include <Arduino.h>


DueFlashStorage *ap_param_s::_flash;
HardwareSerial *ap_param_s::_console;


bool ap_param_s::
setup( void )
{
	struct _flash_hdr hdr;

	for ( uint32_t i = _FLASH_BASE; i < sizeof( struct _flash_hdr ); ++i )
		( (uint8_t *) &hdr )[i] = _flash->read( i );

	if ( hdr.magic[0] != _FLASH_MAGIC0
	  || hdr.magic[1] != _FLASH_MAGIC1
	  || hdr.rev      != _FLASH_REV ) {
#ifdef DEBUG
		_console->println( "restoring params" );
#endif
		_erase_all();
	}
	
	return true;
}


void ap_param_s::
setup_sketch_defaults( void )
{
	setup();
}


// Private functions
//-----------------------------------------------------

void ap_param_s::
_erase_all( void )
{
	struct _flash_hdr hdr;

	hdr.magic[0] = _FLASH_MAGIC0;
	hdr.magic[1] = _FLASH_MAGIC1;
	hdr.rev      = _FLASH_REV;
	hdr.spare    = 0;

	_flash->write( _FLASH_BASE, (byte *) &hdr, sizeof( struct _flash_hdr ) );

	_write_sentinel( _FLASH_BASE + sizeof( struct _flash_hdr ) );
	
#ifdef DEBUG
	_console->println( "erase success" );
#endif
}


void ap_param_s::
_write_sentinel( uint32_t off )
{
	byte sentinel[ sizeof( uint32_t ) ];

	*( (uint32_t *) sentinel ) = _SENTINEL_HDR;
	
	_flash->write( off, sentinel, sizeof( sentinel ) );
}


#endif // CONFIG_LOAD_PARAMS
