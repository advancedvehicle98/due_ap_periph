#ifndef __MATH_H
#define __MATH_H


#include <cmath>
#include <cstdint>
#include <type_traits>


#define DEG2RAD (M_PI/180.f)


namespace math {


float lerp( float out_low, float out_high,
			float in_val,
			float in_low, float in_high );
	
	
template< typename F >
inline bool
is_positive( const F x )
{
	static_assert( std::is_floating_point< F >::value,
				   "is_positive< F >: F is not float" );
	return x > static_cast< float >( __FLT_EPSILON__ );
}

	
template< typename F >
inline bool
is_negative( const F x )
{
	static_assert( std::is_floating_point< F >::value,
				   "is_negative< F >: F is not float" );
	return x < (-1.0f) * static_cast< float >( __FLT_EPSILON__ );
}

	
template< typename F >
inline bool
is_zero( const F x )
{
	static_assert( std::is_floating_point< F >::value,
				   "is_zero< F >: F is not float" );

	float right = static_cast< float >( __FLT_EPSILON__ );
	float left  = (-1.0f)*right; 
	
	return left < x && x < right;
}



template< typename F > F
constrain_float( const F amt, const F low, const F high )
{
	static_assert( std::is_floating_point< F >::value,
				   "constrain_float< F >: F is not float" );

	if      ( std::isnan( amt ) ) return ( low + high )/2;
	else if ( amt < low )         return low;
	else if ( amt > high )        return high;

	return amt;
}


inline constexpr float
radians( const float deg )
{
	return deg*DEG2RAD;
}


uint16_t get_random16( void );
	
	
}; // namespace math


#endif // ! __MATH_H
