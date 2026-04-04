#include <ac_pid.h>


#ifdef CONFIG_SKID_PID


ac_pid::
ac_pid( void )
	: _p( _P )
	, _i( _I )
	, _d( _D )
	, _ff( _FF )
	, _imax( _IMAX )
	, _filt_t_hz( _FILT_T_HZ )
	, _filt_e_hz( _FILT_E_HZ )
	, _filt_d_hz( _FILT_D_HZ )
	, _srmax( _SRMAX )
	, _dff( _DFF )
{
	
}


#endif // CONFIG_SKID_PID
