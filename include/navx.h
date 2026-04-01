/*
  References:
  - https://github.com/kauailabs/navxmxp/blob/5e010ba810bb7f7eaab597e0b708e34f159984db/roborio/c%2B%2B/navx_frc_cpp/include/AHRS.h
  - https://github.com/kauailabs/navxmxp/blob/master/arduino/navX_I2C/navX_I2C.ino
  - https://github.com/kauailabs/navxmxp/blob/5e010ba810bb7f7eaab597e0b708e34f159984db/roborio/c%2B%2B/navx_frc_cpp/src/SerialIO.cpp
 */


#ifndef __NAVX_H
#define __NAVX_H


#include <navx.h>

#include <config.h>

#include <HardwareSerial.h>
#include <Wire.h>

#include <vector3.h>


class navx_t {
public:
	
	navx_t( TwoWire& wire = Wire );
	
	struct ahrs {
		
	};

	byte rx_buffer[0x70];
   	
	void init( void );
	void update( void );

	void set_console( HardwareSerial *console ) {
		_console = console;
	}

	// const Vector3f& get_mag_vec3( void ) const {
	// 	const Vector3f& v;

	// 	// v.x = 
		
	// 	return v;
	// }

private:
	
	static const uint8_t I2C_ADDR = 0x32;
	
	TwoWire _wire;
	HardwareSerial *_console;
};


// нужно, чтобы ссылку конструкторам сразу давать без боли в жопе
#ifdef CONFIG_STATIC_NAVX
extern navx_t navx0;
#endif 


#endif // ! __NAVX_H
