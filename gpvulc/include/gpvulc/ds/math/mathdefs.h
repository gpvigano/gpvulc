//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Mathematic definitions
/// @file mathdefs.h
/// @author Giovanni Paolo Vigano'


#pragma once

#include <cmath>
#include <cstdlib> // USES rand(), RAND_MAX

/// @defgroup MathDef Basic Mathematic definitions
/// Mathematic definitions and inline utility functions.
/// @author Giovanni Paolo Vigano'

/// @addtogroup DS
/// @{

/// @addtogroup Mathematics
///@{ 

/// @addtogroup MathDef
/// @{

namespace gpvulc
{
//******************************************************************************
// CONSTANTS
//******************************************************************************

// MS Visual C++ version of math.h does not define M_PI...
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

//! Alias for M_PI for float
#define DS_PI 3.141592653589793f

//! Precalculated 2*M_PI
#define DS_2PId 6.283185307179586476925286766559

//! Precalculated 2*M_PI
#define DS_2PI 6.283185307179586f

//! Precalculated 0.5f*M_PI
#define DS_HPId 1.5707963267948966192313216916398

//! Precalculated 0.5f*M_PI
#define DS_HPI 1.570796326794896f

//! multiply by this value to convert radians to degrees
#define DS_RAD2DEGd 57.295779513082320876798154814105

//! multiply by this value to convert radians to degrees
#define DS_RAD2DEG 57.29577951308232f

//! multiply by this value to convert degrees to radians
#define DS_DEG2RADd 0.017453292519943295769236907684886

//! multiply by this value to convert degrees to radians
#define DS_DEG2RAD 0.01745329251994329f

#define DS_EPSILONd 0.00000000001 //((float)1E-12f)
#define DS_EPSILONf 0.000001f //((float)1E-6f)

	//! Coordinates usage
	enum gvCoordUseEnum { DS_USENONE = 0, DS_USEX = 1, DS_USEY = 2, DS_USEXY, DS_USEZ = 4, DS_USEXZ, DS_USEYZ, DS_USEXYZ = 7 };
	//! Cardinality
	enum { DS_NONE = 0, DS_ALL = 0xFFFF, DS_ONE = 1 };
	//! Coordinates
	enum { DS_X = 0, DS_Y, DS_Z, DS_RX, DS_RY, DS_RZ, DS_SX, DS_SY, DS_SZ };

	//!@name Transformation component identification
	enum TransformComponentEnum
	{
		DS_POS_X = 0x0001, //!< Position X coord
		DS_POS_Y = 0x0002, //!< Position Y coord
		DS_POS_Z = 0x0004, //!< Position Z coord
		DS_POS = 0x0007, //!< Any position
		DS_ROT_X = 0x0010, //!< Rotation X coord
		DS_ROT_Y = 0x0020, //!< Rotation Y coord
		DS_ROT_Z = 0x0040, //!< Rotation Z coord
		DS_ROT = 0x0070, //!< Rotation
		DS_SCALE_X = 0x0100, //!< Scaling X coord
		DS_SCALE_Y = 0x0200, //!< Scaling Y coord
		DS_SCALE_Z = 0x0400, //!< Scaling Z coord
		DS_SCALE = 0x0700, //!< Scaling
		DS_PIV_X = 0x1000, //!< Pivot X coord
		DS_PIV_Y = 0x2000, //!< Pivot Y coord
		DS_PIV_Z = 0x4000, //!< Pivot Z coord
		DS_PIV = 0x7000, //!< Pivot
		DS_MAT_POS = 0x0008, //!< Position matrix
		DS_MAT_ROT = 0x0080, //!< Rotation matrix
		DS_MAT_SCL = 0x0800, //!< Scaling matrix
		DS_MAT_PIV = 0x8000, //!< Pivot matrix
		DS_MAT = 0x0888, //!< Transformation matrix (Position*Rotation*Scaling)
		DS_MAT_FUL = 0x8888, //!< Full matrix
	};

	// Do not modify these values.

	//! Alignment along the x,y,z axis.
	enum AlignmentEnum
	{
		DS_ALIGN_NONE = 0x0000,
		DS_XALIGN_LEFT = 0x0001, DS_XALIGN_CENTER = 0x0002, DS_XALIGN_RIGHT = 0x0004, DS_XALIGN = DS_XALIGN_LEFT | DS_XALIGN_CENTER | DS_XALIGN_RIGHT,
		DS_YALIGN_TOP = 0x0008, DS_YALIGN_CENTER = 0x0010, DS_YALIGN_BOTTOM = 0x0020, DS_YALIGN = DS_YALIGN_TOP | DS_YALIGN_CENTER | DS_YALIGN_BOTTOM,
		DS_ZALIGN_BACK = 0x0040, DS_ZALIGN_FRONT = 0x0080, DS_ZALIGN_CENTER = 0x0100, DS_ZALIGN = DS_ZALIGN_BACK | DS_ZALIGN_FRONT | DS_ZALIGN_CENTER,
		DS_ALIGN_CENTER = DS_XALIGN_CENTER | DS_YALIGN_CENTER | DS_ZALIGN_CENTER,

		DS_ALIGN_TOPLEFT = DS_YALIGN_TOP | DS_XALIGN_LEFT,
		DS_ALIGN_TOPCENTER = DS_YALIGN_TOP | DS_XALIGN_CENTER,
		DS_ALIGN_TOPRIGHT = DS_YALIGN_TOP | DS_XALIGN_RIGHT,
		DS_ALIGN_CENTERLEFT = DS_YALIGN_CENTER | DS_XALIGN_LEFT,
		DS_ALIGN_CENTERCENTER = DS_YALIGN_CENTER | DS_XALIGN_CENTER,
		DS_ALIGN_CENTERRIGHT = DS_YALIGN_CENTER | DS_XALIGN_RIGHT,
		DS_ALIGN_BOTTOMLEFT = DS_YALIGN_BOTTOM | DS_XALIGN_LEFT,
		DS_ALIGN_BOTTOMCENTER = DS_YALIGN_BOTTOM | DS_XALIGN_CENTER,
		DS_ALIGN_BOTTOMRIGHT = DS_YALIGN_BOTTOM | DS_XALIGN_RIGHT,

		DS_ALIGN_TOPLEFTCENTER = DS_YALIGN_TOP | DS_XALIGN_LEFT | DS_ZALIGN_CENTER,
		DS_ALIGN_TOPCENTERCENTER = DS_YALIGN_TOP | DS_XALIGN_CENTER | DS_ZALIGN_CENTER,
		DS_ALIGN_TOPRIGHTCENTER = DS_YALIGN_TOP | DS_XALIGN_RIGHT | DS_ZALIGN_CENTER,
		DS_ALIGN_CENTERLEFTCENTER = DS_YALIGN_CENTER | DS_XALIGN_LEFT | DS_ZALIGN_CENTER,
		DS_ALIGN_CENTERRIGHTCENTER = DS_YALIGN_CENTER | DS_XALIGN_RIGHT | DS_ZALIGN_CENTER,
		DS_ALIGN_BOTTOMLEFTCENTER = DS_YALIGN_BOTTOM | DS_XALIGN_LEFT | DS_ZALIGN_CENTER,
		DS_ALIGN_BOTTOMCENTERCENTER = DS_YALIGN_BOTTOM | DS_XALIGN_CENTER | DS_ZALIGN_CENTER,
		DS_ALIGN_BOTTOMRIGHTCENTER = DS_YALIGN_BOTTOM | DS_XALIGN_RIGHT | DS_ZALIGN_CENTER,

		DS_ALIGN_TOPLEFTFRONT = DS_YALIGN_TOP | DS_XALIGN_LEFT | DS_ZALIGN_FRONT,
		DS_ALIGN_TOPCENTERFRONT = DS_YALIGN_TOP | DS_XALIGN_CENTER | DS_ZALIGN_FRONT,
		DS_ALIGN_TOPRIGHTFRONT = DS_YALIGN_TOP | DS_XALIGN_RIGHT | DS_ZALIGN_FRONT,
		DS_ALIGN_CENTERLEFTFRONT = DS_YALIGN_CENTER | DS_XALIGN_LEFT | DS_ZALIGN_FRONT,
		DS_ALIGN_CENTERCENTERFRONT = DS_YALIGN_CENTER | DS_XALIGN_CENTER | DS_ZALIGN_FRONT,
		DS_ALIGN_CENTERRIGHTFRONT = DS_YALIGN_CENTER | DS_XALIGN_RIGHT | DS_ZALIGN_FRONT,
		DS_ALIGN_BOTTOMLEFTFRONT = DS_YALIGN_BOTTOM | DS_XALIGN_LEFT | DS_ZALIGN_FRONT,
		DS_ALIGN_BOTTOMCENTERFRONT = DS_YALIGN_BOTTOM | DS_XALIGN_CENTER | DS_ZALIGN_FRONT,
		DS_ALIGN_BOTTOMRIGHTFRONT = DS_YALIGN_BOTTOM | DS_XALIGN_RIGHT | DS_ZALIGN_FRONT,
	};



	//******************************************************************************
	// MATHS
	//******************************************************************************

	//! Return max between two floats
	template < class itemType >
	inline itemType DsMax(itemType a, itemType b) { return (a > b) ? a : b; }

	//! Return max between 3 floats
	template < class itemType >
	inline itemType DsMax(itemType a, itemType b, itemType c) { return DsMax(DsMax(a, b), c); }

	//! Return max between two floats
	template < class itemType >
	inline itemType DsMin(itemType a, itemType b) { return (a<b) ? a : b; }

	//! Return min between 3 floats
	template < class itemType >
	inline itemType DsMin(itemType a, itemType b, itemType c) { return DsMin(DsMin(a, b), c); }

	//! Return the sign of a number
	inline float gvSign(float a) { return fabs(a)<DS_EPSILONf ? 0.0f : a / fabs(a); }

	//! Return -1^n
	inline float gvMinusOneExp(int n) { return (n % 2) == 0 ? 1.0f : -1.0f; }

	//! Clip a value between min and max
	template < class itemType >
	inline itemType gvClip(itemType v, itemType min, itemType max) { return v>max ? max : v < min ? min : v; }


	//! Snap the given value to the given step (>=0)
	inline float gvSnap(float x, float step) { return step == 0.0 ? x : ((float)((int)(x / step + 0.5f*gvSign(x))))*step; }


	//! Linear interpolation from src to dst according to t (that ranges from 0 to 1)
	inline float LerpValues(const float t, const float src, const float dst)
	{

		return src*(1.0f - t) + dst*t;
	}


	//!Get the integer nearest to the given number
	inline int FloatToNearestInt(float val) { return int(val) < 0 ? int(val - 0.5f) : int(val + 0.5f); }

	//!Round a number to the nearest integer
	inline float RoundFloat(float val) { return float(FloatToNearestInt(val)); }


	//!Round a number to keep just the given decimal digits
	inline float RoundFloat(float val, unsigned short decimals)
	{
		int d10(1);
		for (unsigned short i = 0; i < decimals; i++) d10 *= 10;
		return RoundFloat(val*(float)d10) / (float)d10;
	}


	//! Test if the given floating point number is zero (<DS_EPSILONf)
	inline bool IsAlmostZero(float v)
	{
		return fabs(v) < DS_EPSILONf;
	}


	//! Test if the given floating point number is zero (<DS_EPSILONf)
	inline bool IsAlmostZero(double v)
	{
		return fabs(v) < DS_EPSILONd;
	}


	//! Test if the given floating point numbers are equal (difference<DS_EPSILONf)
	inline bool IsAlmostEqual(float v1, float v2)
	{
		return fabs(v1 - v2) < DS_EPSILONf;
	}


	//! Test if the given double precision floating point numbers are equal (difference<DS_EPSILONd)
	inline bool IsAlmostEqual(double v1, double v2)
	{
		return fabs(v1 - v2) < DS_EPSILONd;
	}



	//! Test if the given values are equal until the given decimal digits
	inline bool IsAlmostEqual(float v1, float v2, unsigned short precision)
	{
		float d10(1.0f);
		for (unsigned short i = 0; i < precision; i++) d10 *= 10.0f;
		return int(v1*d10) == int(v2*d10);
	}


	//! Scale and bias a value in the range [0..1] to fit into the range [bias .. 1.0]
	inline void ScaleAndBias(float& val, float bias)
	{
		val = val * (1.0f - bias) + bias;
	}


	//! Return the power of 2 nearest and less or equal to val
	inline unsigned NearestMaxPow2(unsigned val)
	{
		unsigned result = 1;
		while (result * 2 <= val)
		{
			result *= 2;
		}
		return result;
	}

	//! Return the power of 2 nearest and less or equal to val and to a given maximum allowed value
	inline int NearestAllowedPow2(int input, int maxval)
	{
		int value = 1;
		while (value * 2 <= input && value * 2 <= maxval)
		{
			value *= 2;
		}
		return value;
	}


	//! Return the power of 10 nearest to val
	inline int NearestPow10(float val) { float l = log10f(val); return ((int)(l + ((l < 0.0f) ? -1.0f : 1.0f)*0.5f)); }


	//! Return the non zero power of 10 nearest to val
	inline int NearestNZPow10(float val) { float l = log10f(val); return ((int)(l + ((l < 0.0f) ? -1.0f : 1.0f))); }


	//! Round to the power of 10 nearest to val
	inline float RoundToNearestPow10(float a) { return powf(10.0f, (float)NearestPow10(a)); }


	//! Round to the non zero power of 10 nearest to val
	inline float RoundToNearestNZPow10(float a) { return powf(10.0f, (float)NearestNZPow10(a)); }


	inline float Log2(float n)
	{
#ifdef log2f
		return log2f(n);
#else
		return (float)(log10(n) / log10(2.0));
#endif
	}


	//! Generate a random number from 0 to x
	inline float Rnd(float x)
	{
		return x*(float)rand() / (float)RAND_MAX;
	}


	//! Generate a random number from -x to x
	inline float SignedRnd(float x)
	{
		return Rnd(x*2.0f) - x;
	}


	//! Calculate the distance between 2 values
	inline float ValuesDistance(float a, float b)
	{
		return (float)fabs(a - b);
	}



	//******************************************************************************
	// VECTORS
	//******************************************************************************
	//@name Vectors
	//@{

	//! fill dst vector components with x,y,z
	inline void VecSet(float dst[3], float x, float y, float z) { dst[0] = x; dst[1] = y; dst[2] = z; }

	//! copy src vector to dst
	inline void VecCopy(float dst[3], const float src[3]) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; }

	//! negate a vector
	inline void VecNegate(float v[3]) { v[0] = -v[0]; v[1] = -v[1]; v[2] = -v[2]; }

	//@}

	/// @}
	/// @}
	/// @}
} // namespace gpvulc


