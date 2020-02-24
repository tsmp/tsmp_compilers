///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains FPU related code.
 *	\file		IceFPU.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEFPU_H__
#define __ICEFPU_H__

	#define	SIGN_BITMASK			0x80000000

	//! Integer representation of a floating-point value.
	#define IR(x)					((unsigned int&)(x))

	//! Signed integer representation of a floating-point value.
	#define SIR(x)					((signed int&)(x))

	//! Absolute integer representation of a floating-point value
	#define AIR(x)					(IR(x)&0x7fffffff)

	//! Floating-point representation of an integer value.
	#define FR(x)					((float&)(x))

	//! Integer-based comparison of a floating point value.
	//! Don't use it blindly, it can be faster or slower than the FPU comparison, depends on the context.
	#define IS_NEGATIVE_FLOAT(x)	(IR(x)&0x80000000)

	//! Fast fabs for floating-point values. It just clears the sign bit.
	//! Don't use it blindy, it can be faster or slower than the FPU comparison, depends on the context.
	inline_ float FastFabs(float x)
	{
		unsigned int FloatBits = IR(x)&0x7fffffff;
		return FR(FloatBits);
	}

	//! Saturates positive to zero.
	inline_ float fsat(float f)
	{
		unsigned int y = (unsigned int&)f & ~((signed int&)f >>31);
		return (float&)y;
	}

	//! Computes 1.0f / sqrtf(x).
	inline_ float frsqrt(float f)
	{
		float x = f * 0.5f;
		unsigned int y = 0x5f3759df - ((unsigned int&)f >> 1);
		// Iteration...
		(float&)y  = (float&)y * ( 1.5f - ( x * (float&)y * (float&)y ) );
		// Result
		return (float&)y;
	}

	//! Computes 1.0f / sqrtf(x). Comes from Quake3. Looks like the first one I had above.
	//! See http://www.magic-software.com/3DGEDInvSqrt.html
	inline_ float RSqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = number * 0.5f;
		y  = number;
		i  = * (long *) &y;
		i  = 0x5f3759df - (i >> 1);
		y  = * (float *) &i;
		y  = y * (threehalfs - (x2 * y * y));

		return y;
	}

	//! TO BE DOCUMENTED
	inline_ float fsqrt(float f)
	{
		unsigned int y = ( ( (signed int&)f - 0x3f800000 ) >> 1 ) + 0x3f800000;
		// Iteration...?
		// (float&)y = (3.0f - ((float&)y * (float&)y) / f) * (float&)y * 0.5f;
		// Result
		return (float&)y;
	}

	//! Returns the float ranged espilon value.
	inline_ float fepsilon(float f)
	{
		unsigned int b = (unsigned int&)f & 0xff800000;
		unsigned int a = b | 0x00000001;
		(float&)a -= (float&)b;
		// Result
		return (float&)a;
	}

	//! Is the float valid ?
	inline_ bool IsNAN(float value)	{ return ((*(unsigned int*)&value)&0x7f800000)==0x7f800000; }
	#define	NaN(value) (!((value>=0) || (value<0)))

	//! This function computes the slowest possible floating-point value (you can also directly use FLT_EPSILON)
	inline_ float ComputeFloatEpsilon()
	{
		float f = 1.0f;
		((unsigned int&)f)^=1;
		return f - 1.0f;	// You can check it's the same as FLT_EPSILON
	}

	inline_ bool IsFloatZero(float x, float epsilon=1e-6f)
	{
		return x*x < epsilon;
	}

	#define FCOMI_ST0	_asm	_emit	0xdb	_asm	_emit	0xf0
	#define FCOMIP_ST0	_asm	_emit	0xdf	_asm	_emit	0xf0
	#define FCMOVB_ST0	_asm	_emit	0xda	_asm	_emit	0xc0
	#define FCMOVNB_ST0	_asm	_emit	0xdb	_asm	_emit	0xc0

	#define FCOMI_ST1	_asm	_emit	0xdb	_asm	_emit	0xf1
	#define FCOMIP_ST1	_asm	_emit	0xdf	_asm	_emit	0xf1
	#define FCMOVB_ST1	_asm	_emit	0xda	_asm	_emit	0xc1
	#define FCMOVNB_ST1	_asm	_emit	0xdb	_asm	_emit	0xc1

	#define FCOMI_ST2	_asm	_emit	0xdb	_asm	_emit	0xf2
	#define FCOMIP_ST2	_asm	_emit	0xdf	_asm	_emit	0xf2
	#define FCMOVB_ST2	_asm	_emit	0xda	_asm	_emit	0xc2
	#define FCMOVNB_ST2	_asm	_emit	0xdb	_asm	_emit	0xc2

	//! A global function to find MAX(a,b,c) using FCOMI/FCMOV
/*	inline_ float FCMax3(float a, float b, float c)
	{
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		_asm	fld		[c]
		FCOMI_ST1
		FCMOVB_ST1
		FCOMI_ST2
		FCMOVB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
		return Res;
	}

	//! A global function to find MIN(a,b,c) using FCOMI/FCMOV
	inline_ float FCMin3(float a, float b, float c)
	{
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		_asm	fld		[c]
		FCOMI_ST1
		FCMOVNB_ST1
		FCOMI_ST2
		FCMOVNB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
		return Res;
	}
*/
	inline_ int ConvertToSortable(float f)
	{
		int& Fi = (int&)f;
		int Fmask = (Fi>>31);
		Fi ^= Fmask;
		Fmask &= ~(1<<31);
		Fi -= Fmask;
		return Fi;
	}

	enum FPUMode
	{
		FPU_FLOOR		= 0,
		FPU_CEIL		= 1,
		FPU_BEST		= 2,
		FPU_FORCE_DWORD	= 0x7fffffff
	};

	FUNCTION ICECORE_API FPUMode	GetFPUMode();
	FUNCTION ICECORE_API void		SaveFPU();
	FUNCTION ICECORE_API void		RestoreFPU();
	FUNCTION ICECORE_API void		SetFPUFloorMode();
	FUNCTION ICECORE_API void		SetFPUCeilMode();
	FUNCTION ICECORE_API void		SetFPUBestMode();

	FUNCTION ICECORE_API void		SetFPUPrecision24();
	FUNCTION ICECORE_API void		SetFPUPrecision53();
	FUNCTION ICECORE_API void		SetFPUPrecision64();
	FUNCTION ICECORE_API void		SetFPURoundingChop();
	FUNCTION ICECORE_API void		SetFPURoundingUp();
	FUNCTION ICECORE_API void		SetFPURoundingDown();
	FUNCTION ICECORE_API void		SetFPURoundingNear();

	FUNCTION ICECORE_API int		intChop(const float& f);
	FUNCTION ICECORE_API int		intFloor(const float& f);
	FUNCTION ICECORE_API int		intCeil(const float& f);

#endif // __ICEFPU_H__
