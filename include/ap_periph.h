#ifndef __AP_PERIPH_H
#define __AP_PERIPH_H


#include <Arduino.h>
#include <config.h>

#include <due_can.h>

#include <ap_param.h>
#ifdef CONFIG_USE_NAVX
#   include <navx.h>
#endif
#include <srv_channel_skid.h>


#ifdef CONFIG_SKID_STEER
typedef srv_channel_skid srv_channels;
#else 
typedef srv_channel_steer srv_channels;
#endif


class ap_periph_t
{	
public:
	ap_periph_t( HardwareSerial& uart    = Serial,
				 HardwareSerial& console = Serial,
				 CANRaw&         can     = Can1,
#if defined( CONFIG_USE_NAVX ) && defined( CONFIG_STATIC_NAVX )
				 navx_t&         navx    = navx0
#endif
				 );
	
	void init( void );
	void update( void );

private:
	static const int _UART_BAUD    = 115200;
	static const int _CONSOLE_BAUD = _UART_BAUD;
	
	HardwareSerial& _uart;
	HardwareSerial& _console;
	CANRaw&         _can;

#ifdef CONFIG_USE_NAVX
	navx_t&         _navx;
#endif

	srv_channels _motors;
	
	void _can_init( void );
	void _load_parameters( void );
};


#endif // ! __AP_PERIPH_H
