#ifndef __SRV_CHANNEL_SKID
#define __SRV_CHANNEL_SKID


#include <config.h>

#include <HardwareSerial.h>
#ifdef CONFIG_USE_MXP_MD2
#   include <DualVNH5019MotorShield.h>
#endif

#ifdef CONFIG_SKID_PID
#   include <ap_wheel_rate_control.h>
#endif


class srv_channel_skid
{
public:
	srv_channel_skid( void );

	void init( void );
	void update( const bool armed, const float dt );

	void set_steer( const float steer,
					const bool  apply_scaling ) {
		_steer = steer;
		_apply_scaling = apply_scaling;
	}

	void set_throttle( const float throttle ) {
		_throttle = math::constrain_float( throttle, -_THROTTLE_MAX, _THROTTLE_MAX );
	}

	void set_console( HardwareSerial *console ) {
		_console = console;
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

	static constexpr float _THST_ASYM = CONFIG_MOT_THST_ASYM;
	
	static constexpr float _STR_THR_MIX = CONFIG_MOT_STR_THR_MIX;

	static constexpr float _THROTTLE_MAX = CONFIG_MOT_THR_MAX;
	static constexpr float _THROTTLE_MIN = CONFIG_MOT_THR_MIN;
	
	static constexpr float _STEER_MAX = 4500.0f;
	static constexpr float _STEER_MIN = -_STEER_MAX;
		
	static const uint8_t _LIMIT_STEER_LEFT_MASK      = 1 << 0;
	static const uint8_t _LIMIT_STEER_RIGHT_MASK     = 1 << 1;
	static const uint8_t _LIMIT_THROTTLE_LOWER_MASK = 1 << 2;
	static const uint8_t _LIMIT_THROTTLE_UPPER_MASK = 1 << 3;

	HardwareSerial *_console;
	
	float _throttle, _steer,
		  _throttle_old,
		  _slew_rate;

#ifdef CONFIG_SKID_PID
	ap_wheel_rate_control _rate_ctrl;

	void _get_rate_controlled_throttle( float& throttle_left, float& throttle_right, float dt );
#endif 
   
	uint8_t _limit_mask = 0;

	bool _apply_scaling;

	static const int __LEFT  = 0;
	static const int __RIGHT = 0;
	
	uint32_t _last_output_ms[2];
	float _last_throttle[2], _out_scaled[2];
	uint16_t _pwm[2];
	int16_t _high_out[2];
	
#ifdef CONFIG_USE_MXP_MD2
	static const uint16_t _PWM_TRIM = CONFIG_MXP_MD2_PWM_TRIM;
	static const uint16_t _PWM_MAX  = CONFIG_MXP_MD2_PWM_MAX;
	static const uint16_t _PWM_MIN  = CONFIG_MXP_MD2_PWM_MAX;
	
	DualVNH5019MotorShield _md_left, _md_right;
#endif

	uint16_t _calc_individual_pwm( const float _out_scaled, const int16_t high_out );
	inline void _calc_pwm( void );
	void _driver_specific_out( void );
	float _get_scaled_throttle( float throttle ) const;
	float _get_slew_limited_throttle( const float throttle, const float dt ) const;
	void _output( const bool armed, const float dt );
	inline void _output_ch_all( void );
	void _output_reverse_throttle( float throttle_left, float throttle_right, const float delay );
	void _output_throttle( float throttle_left, float throttle_right, const float dt );
	void _set_limits_from_input( void );
	void _set_output_scaled( float throttle_left, float throttle_right );
	void _slew_limit_throttle( const float dt );
};


#endif // ! __SRV_CHANNEL_SKID
