#include <math.h>


float lerp( float out_low, float out_high,
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
