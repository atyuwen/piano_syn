#include "math.hpp"

#ifdef RELEASE
extern "C" void __cdecl _CIpow()
{
	__asm
	{
		fxch	st(1)
		fyl2x
		fld		st(0)
		frndint
		fsubr	st(1), st(0)
		fxch	st(1)
		fchs
		f2xm1
		fld1
		faddp	st(1), st(0)
		fscale
		fstp	st(1)
		ret
	}
}
#endif

float complex::mag() const
{
	return sqrt(real * real + imag * imag);
}

float complex::arg() const
{
	return atan2(imag, real);
}

complex make_complex(float mag, float arg)
{
	return complex(mag * cos(arg), mag * sin(arg));
}

complex operator - (const complex& operand)
{
	return complex(-operand.real, -operand.imag);
}

complex operator + (const complex& lhs, const complex& rhs)
{
	return complex(lhs.real + rhs.real, lhs.imag + rhs.imag);
}

complex operator - (const complex& lhs, const complex& rhs)
{
	return complex(lhs.real - rhs.real, lhs.imag - rhs.imag);
}

complex operator * (const complex& lhs, const complex& rhs)
{
	return complex(
		lhs.real * rhs.real - lhs.imag * rhs.imag,
		lhs.imag * rhs.real + lhs.real * rhs.imag);
}

complex operator / (const complex& lhs, const complex& rhs)
{
	float rhs_mad_sqr = rhs.real * rhs.real + rhs.imag * rhs.imag;
	return complex(
		(lhs.real * rhs.real + lhs.imag * rhs.imag) / rhs_mad_sqr,
		(lhs.imag * rhs.real - lhs.real * rhs.imag) / rhs_mad_sqr);
}
