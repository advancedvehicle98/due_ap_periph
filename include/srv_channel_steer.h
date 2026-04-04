#ifdef __IGNORE__

#ifndef __SRV_CHANNEL_STEER
#define __SRV_CHANNEL_STEER


#include <config.h>

#ifdef CONFIG_USE_MXP_MD2
#   include <DualVNH5019MotorShield.h>
#endif

#include <ar_attitude_control.h>
#include <srv_channel.h>


class srv_channel_steer
{
public:
	srv_channel_steer( void );

	void init( void );
	void update( void );

	void set_steer( const float steer,
					const bool  apply_scaling ) {
		_val[ __STEER_FUNC ] = steer;
		_apply_scaling = apply_scaling;
	}

	void set_throttle( const float throttle ) {
		_val[ __THROTTLE_FUNC ]
			= math::constrain_float( throttle, -_THROTTLE_MAX, _THROTTLE_MAX );
	}

private:
	
#ifdef CONFIG_USE_MXP_MD2
	static const int _MD_LEFT_INA1  = CONFIG_MXP_MD2_LEFT_INA1_PIN;
	static const int _MD_LEFT_INB1  = CONFIG_MXP_MD2_LEFT_INB1_PIN;
	static const int _MD_LEFT_PWM1  = CONFIG_MXP_MD2_LEFT_PWM1_PIN;

	static const int _MD_LEFT_INA2  = CONFIG_MXP_MD2_LEFT_INA2_PIN;
	static const int _MD_LEFT_INB2  = CONFIG_MXP_MD2_LEFT_INB2_PIN;
	static const int _MD_LEFT_PWM2  = CONFIG_MXP_MD2_LEFT_PWM2_PIN;

	static const int _MD_RIGHT_INA1 = CONFIG_MXP_MD2_RIGHT_INA1_PIN;
	static const int _MD_RIGHT_INB1 = CONFIG_MXP_MD2_RIGHT_INB1_PIN;
	static const int _MD_RIGHT_PWM1 = CONFIG_MXP_MD2_RIGHT_PWM1_PIN;

	static const int _MD_RIGHT_INA2 = CONFIG_MXP_MD2_RIGHT_INA2_PIN;
	static const int _MD_RIGHT_INB2 = CONFIG_MXP_MD2_RIGHT_INB2_PIN;
	static const int _MD_RIGHT_PWM2 = CONFIG_MXP_MD2_RIGHT_PWM2_PIN;
#endif // CONFIG_USE_MXP_MD2

	static constexpr float _TRUST_CURVE_EXP = CONFIG_MOT_THST_EXPO;

	static constexpr float _REV_DELAY = CONFIG_MOT_REV_DELAY;

	static constexpr float _SPD_SCA_BASE = CONFIG_MOT_SPD_SCA_BASE;

	static constexpr float _THROTTLE_MAX = CONFIG_MOT_THR_MAX;
	static constexpr float _THROTTLE_MIN = CONFIG_MOT_THR_MIN;
	
	static constexpr float _STEER_MAX = 4500.0f;
	static constexpr float _STEER_MIN = -_STEER_MAX;
	
	const int __STEER_FUNC    = 0;
	const int __THROTTLE_FUNC = 1;
	
	static const uint8_t _LIMIT_STEER_LEFT_MASK     = 1 << 0;
	static const uint8_t _LIMIT_STEER_RIGHT_MASK    = 1 << 1;
	static const uint8_t _LIMIT_THROTTLE_LOWER_MASK = 1 << 2;
	static const uint8_t _LIMIT_THROTTLE_UPPER_MASK = 1 << 3;
	
	float _val[2], _val_old[2],
		  _out_scaled[2],
		  _slew_rate,
		  _last_throttle;

	srv_channel _throttle_ch, _steer_ch;

	uint8_t _limit_mask = 0;

	bool _apply_scaling, _corking;

	uint32_t _last_output_ms;
	
#ifdef CONFIG_USE_MXP_MD2
	static constexpr uint16_t _PWM_TRIM = CONFIG_MXP_MD2_PWM_TRIM;
	static constexpr uint16_t _PWM_MAX  = CONFIG_MXP_MD2_PWM_MAX;
	static constexpr uint16_t _PWM_MIN  = CONFIG_MXP_MD2_PWM_MAX;
	
	DualVNH5019MotorShield _md_left, _md_right;
#endif

	inline void _calc_pwm( void );
	float _get_scaled_throttle( float throttle ) const;
	float _get_slew_limited_throttle( const float throttle, const float dt ) const;
	void _output( const bool armed, const float speed, const float dt );
	inline void _output_ch_all( void );
	void _output_reverse_throttle( float throttle, const float delay );
	void _output_throttle( float throttle, const float dt = 0.0d );
	void _set_limits_from_input( const float steer, const float throttle );
	void _set_output_limit( const srv_channel::srv_func_e func,
							const srv_channel::limit_e limit );
	void _set_output_scaled( const srv_channel::srv_func_e func,
							 const float val );
	void _slew_limit_throttle( const float dt );
};


#endif // ! __SRV_CHANNEL_STEER

#endif // __IGNORE__
