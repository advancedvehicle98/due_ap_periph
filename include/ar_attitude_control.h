#ifndef __AR_ATTITUDE_CONTROL_H
#define __AR_ATTITUDE_CONTROL_H


class ar_attitude_control
{
public:
	ar_attitude_control( void ) = default;

	bool get_fwd_speed( float& speed ) const;
};


#endif // ! __AR_ATTITUDE_CONTROL_H
