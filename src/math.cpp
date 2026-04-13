#include <math.h>


uint16_t math::
get_random16( void )
{
    static uint32_t m_z = 1234;
    static uint32_t m_w = 76542;
	
    m_z = 36969 * ( m_z & 0xFFFFu ) + ( m_z >> 16 );
    m_w = 18000 * ( m_w & 0xFFFFu ) + ( m_w >> 16 );
	
    return ( (m_z << 16) + m_w ) & 0xFFFF;
}


float math::
lerp( float out_low, float out_high,
	  float in_val,
	  float in_low, float in_high )
{
	if ( in_low > in_high ) {
		float _high = out_high;
		float _low  = out_low;

		out_low  = _high;
		out_high = _low;

		_high = in_high;
		_low  = in_low;
		
		in_low  = _high;
		in_high = _low;
	}

	if ( in_val <= in_low )  return out_low;
	if ( in_val >= in_high ) return out_high;

	float p = ( in_val - in_low )/( in_high - in_low );

	return out_low + p*( out_high - out_low );

}
