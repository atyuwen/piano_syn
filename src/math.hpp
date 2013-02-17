#ifndef _MATH_HPP_INCLUDED_
#define _MATH_HPP_INCLUDED_

#include <math.h>

#ifdef RELEASE
#define exp(x) pow(2.71828183f, (x))
#endif

#define PI 3.1415927f

struct complex
{
	union
	{
		struct
		{
			float real;
			float imag;
		};
		float value[2];
	};

	complex(float real = 0, float imag = 0) : real(real), imag(imag) {}

	float mag() const;
	float arg() const;
};

complex make_complex(float mag,  float arg);
complex operator - (const complex& operand);
complex operator + (const complex& lhs, const complex& rhs);
complex operator - (const complex& lhs, const complex& rhs);
complex operator * (const complex& lhs, const complex& rhs);
complex operator / (const complex& lhs, const complex& rhs);

#endif // _MATH_HPP_INCLUDED_
