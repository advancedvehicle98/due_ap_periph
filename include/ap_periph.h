#ifndef __AP_PERIPH_H
#define __AP_PERIPH_H


#include <Arduino.h>

#include <due_can.h>
#include <vector3.h>

#include <ap_param.h>
#include <navx.h>


class ap_periph_t
{	
public:
	ap_periph_t( HardwareSerial& uart    = Serial,
				 HardwareSerial& console = Serial,
				 CANRaw&         can     = Can1,
				 navx_t&         navx    = navx0 );
	
	void init( void );
	void update( void );

private:
	static const int _UART_BAUD    = 115200;
	static const int _CONSOLE_BAUD = _UART_BAUD;
	
	HardwareSerial& _uart;
	HardwareSerial& _console;
	CANRaw&         _can;
	navx_t&         _navx;

	void _can_init( void );
	void _load_parameters( void );
};


#endif // ! __AP_PERIPH_H
