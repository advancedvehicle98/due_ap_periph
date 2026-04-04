#ifndef __AC_PID
#define __AC_PID


#include <config.h>


#ifdef CONFIG_SKID_PID


#undef _P


class ac_pid
{
public:
	ac_pid( void );

private:
	// дефолты
	static constexpr float _P         = CONFIG_PID_P;
	static constexpr float _I         = CONFIG_PID_I;
	static constexpr float _D         = CONFIG_PID_D;
	static constexpr float _FF        = CONFIG_PID_FF;
	static constexpr float _IMAX      = CONFIG_PID_IMAX;
	static constexpr float _FILT_T_HZ = CONFIG_PID_FILT_T_HZ;
	static constexpr float _FILT_E_HZ = CONFIG_PID_FILT_E_HZ;
	static constexpr float _FILT_D_HZ = CONFIG_PID_FILT_D_HZ;
	static constexpr float _SRMAX     = CONFIG_PID_SRMAX;
	static constexpr float _DFF       = CONFIG_PID_DFF;
	
	float _p, _i, _d, _ff, _imax,
		  _filt_t_hz, _filt_e_hz, _filt_d_hz,
		  _srmax, _dff;
};


#endif // CONFIG_SKID_PID


#endif // ! __AC_PID
