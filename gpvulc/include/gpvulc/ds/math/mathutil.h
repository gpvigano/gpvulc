//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Mathematic classes and utilities
/// @file mathutil.h
/// @author Giovanni Paolo Vigano'


#pragma once

#include "gpvulc/ds/math/mathdefs.h"

#include <vector>
#include <string.h>
#include <stdio.h>

namespace gpvulc
{

	/// @defgroup Math Basic Mathematics
	/// Mathematic utility functions and data structures.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{

	/// @addtogroup Mathematics
	///@{

	/// @addtogroup Math
	/// @{

	//-----------------------------------------------------------------
	// MATH
	//-----------------------------------------------------------------


	//! constrain values to a cyclic range from min to max
	void LimitValueRange(float& val, float min, float max);


	//-----------------------------------------------------------------
	// VECTORS
	//-----------------------------------------------------------------
	//@name Vectors
	//@{

	//! gets 2 points and returns the vector going from p1 to p2
	void VecFromTo(const float p1[3], const float p2[3], float v[3]);

	//! the usual cross product: result = u x v
	void VecCrossProduct(const float u[3], const float v[3], float result[3]);

	//! the scalar product u.v
	float VecDotProduct(const float u[3], const float v[3]);

	//! norm of a vector
	float VecNorm(const float v[3]);

	//! normalizes vector v
	void VecNormalize(float v[3]);

	inline void VecNormalize(const float v1[3], float v2[3])
	{
		VecCopy(v2, v1);
		VecNormalize(v2);
	}

	//! returns in c the cross product (a "not normalized normal") at v1 (between v0->v1 and v1->v2)
	void VecCross(const float v0[3], const float v1[3], const float v2[3], float c[3]);

	//! returns in n the normal at v1 (between v0->v1 and v1->v2)
	void VecNormal(const float v0[3], const float v1[3], const float v2[3], float normal[3]);

	// rotate a vector by the given angle (IN DEGREES) around an axis, the result can be the vector p itself
	void VecRotate(const float p[3], float angle, const float axis[3], float result[3]);

	//! applies scaling, rotation (IN DEGREES) and then translation to a point
	void VecTransform(const float p[3], const float scaling[3], float angle, const float axis[3], const float translation[3], float result[3]);

	//! applies the inverse of the given transformation (scaling, rotation IN RADIANS and then translation) to a point
	void VecInverseTransform(const float p[3], const float scaling[3], float angle, const float axis[3], float translation[3], float result[3]);

	//! Interpolate vector v from src to dst according to t (that ranges from 0 to 1)
	inline void VecInterpolate(float v[3], float t, const float src[3], const float dst[3])
	{
		v[0] = LerpValues(t, src[0], dst[0]);
		v[1] = LerpValues(t, src[1], dst[1]);
		v[2] = LerpValues(t, src[2], dst[2]);
	}

	//! calculate the angle (DEGREES) between two vectors
	float VecAngle(const float v1[3], const float v2[3]);

	//! calculate the angle (DEGREES) between two vectors projected onto a plane defined by its normal
	float VecAngleOnPlane(const float v1[3], const float v2[3], const float n[3]);

	//! constrain angles in rot to a cyclic range from min to max
	void VecValueRange(float rot[3], float min, float max);

	//! Set rot1 angle according to the minor angle with rot2
	void SetMinorAngle(float& rot1, float rot2);

	//! Set rot1 angles according to the minor angles with rot2
	void VecMinorAngle(float rot1[3], const float rot2[3]);

	//! Calculate the distance between 2 points
	float VecDistance(const float a[3], const float b[3]);

	//! Calculate the square distance between 2 points
	float VecSquareDistance(const float a[3], const float b[3]);

	//! Fix euler angles in the form (+-180,a,+-180) to (0,a+-180,0)
	void VecFixEulerAngles(float angle[3]);

	//! Returns the euler angles (degrees 0-360) given an angle (degrees) and an axis
	void VecGetEuler(float angle, const float axis[3], float euler_angles[3]);

	//! Calculate a feasible orientation (heading&pitch Euler angles) from pos to pnt
	void VecLookAtHP(const float pos[3], const float pnt[3], float rot[3]);

	//! Calculate heading, pitch and radius given the position
	void PositionToHpR(const float pos[3], float &h, float &p, float &r);

	//! Calculate the position given heading, pitch and radius
	void HpRToPosition(float h, float p, float dist, float pos[3]);

	//********************************************************************
	///@name Vector structures
	/// @note Warning! Do not implement virtual methods in these structures (memory issue)
	///@{

	//! 2d point structure with integer coordinates
	struct Point2
	{

		///@name Coordinates
		///@{

		int X;
		int Y;

		///@}

		///@name Contructors
		///@{

		Point2() { this->X = this->Y = 0; }

		Point2(int x) { this->X = this->Y = x; }

		Point2(int x, int y) { this->X = x; this->Y = y; }

		Point2(const int f[2]) { this->X = f[0]; this->Y = f[1]; }

		///@}

		/// @name Arithmetic operators
		/// Warning! All of these operate component-by-component.
		///@{

		Point2  operator +(Point2 p1) { return Point2(this->X + p1.X, this->Y + p1.Y); }

		Point2  operator -(Point2 p1) { return Point2(this->X - p1.X, this->Y - p1.Y); }

		Point2  operator *(Point2 v) { return Point2(this->X * v.X, this->Y * v.Y); }

		Point2  operator /(Point2 v) { return Point2(this->X / v.X, this->Y / v.Y); }

		//! Negate all the vector components
		Point2 operator- () { return Point2(-(this->X), -(this->Y)); }

		///@}

		/// @name Scalar operators
		/// Each component of this vector is used with the same scalar value (no change in this vector)
		///@{

		Point2  operator *(int f) { return Point2(this->X * f, this->Y * f); }

		Point2  operator /(int f) { return Point2(this->X / f, this->Y / f); }

		///@}

		/// @name Scalar operators
		/// Each component of this vector is used with the same scalar value (this vector changes)
		///@{

		Point2  operator *=(int f) { this->X *= f; this->Y *= f; return *this; }

		Point2  operator /=(int f) { this->X /= f; this->Y /= f; return *this; }

		///@}

		/// @name Assignment operators
		/// Assignment from a vector or a vector expression
		///@{

		Point2& operator =(Point2 p1) { this->X = p1.X; this->Y = p1.Y; return *this; }

		Point2  operator +=(Point2 p1) { this->X += p1.X; this->Y += p1.Y; return *this; }

		Point2  operator -=(Point2 p1) { this->X -= p1.X; this->Y -= p1.Y; return *this; }

		Point2  operator *=(Point2 p1) { this->X *= p1.X; this->Y *= p1.Y; return *this; }

		Point2  operator /=(Point2 p1) { this->X /= p1.X; this->Y /= p1.Y; return *this; }

		///@}

		/// @name Comparison operators
		/// Implements comparison between vectors ("<" and ">" are not defined)
		///@{

		bool  operator ==(Point2 v) { return (this->X == v.X && this->Y == v.Y); }

		bool  operator !=(Point2 v) { return (this->X != v.X || this->Y != v.Y); }

		///@}

		/*!
		 unary operators
		 /// Automatic casting to int* (or int[]) when passing a VecX as parameter
		operator int* ()          { return (int*)this; }
		! Array operator: access X,Y using indices 0,1
		*/
		int& operator[] (int idx) { return idx == 0 ? this->X : this->Y; }
	};

	//! 2d vector structure
	struct Vec2
	{

		/// Coordinates
		//@{

		float X;
		float Y;

		//@}

		/// @name Contructors
		//@{

		Vec2() { this->X = this->Y = 0.0; }
		Vec2(float x) { this->X = this->Y = x; }
		Vec2(float x, float y) { this->X = x; this->Y = y; }
		Vec2(const float f[2]) { this->X = f[0]; this->Y = f[1]; }
		Vec2(const Point2& p) { this->X = (float)p.X; this->Y = (float)p.Y; }

		//@}

		//! Set the values of this vector
		void Set(float x, float y)
		{
			this->X = x;
			this->Y = y;
		}

		/// @name Arithmetic operators
		/// Warning! All of these operate component-by-component.
		//@{

		Vec2  operator +(const Vec2 p1) { return Vec2(this->X + p1.X, this->Y + p1.Y); }
		Vec2  operator -(const Vec2 p1) { return Vec2(this->X - p1.X, this->Y - p1.Y); }
		Vec2  operator *(const Vec2 v) { return Vec2(this->X * v.X, this->Y * v.Y); }
		Vec2  operator /(const Vec2 v) { return Vec2(this->X / v.X, this->Y / v.Y); }
		//! Negate all the vector components
		Vec2 operator- () { return Vec2(-(this->X), -(this->Y)); }

		//@}

		/// @name Scalar operators
		/// Each component of this vector is used with the same scalar value (no change in this vector)
		//@{

		Vec2  operator *(float f) { return Vec2(this->X * f, this->Y * f); }
		Vec2  operator /(float f) { return Vec2(this->X / f, this->Y / f); }

		//@}

		/// @name Scalar operators
		/// Each component of this vector is used with the same scalar value (this vector changes)
		//@{

		Vec2  operator *=(float f) { this->X *= f; this->Y *= f; return *this; }
		Vec2  operator /=(float f) { this->X /= f; this->Y /= f; return *this; }

		//@}

		/// @name Assignment operators
		/// Assignment from a vector or a vector expression
		//@{

		Vec2& operator =(const Vec2 p1) { this->X = p1.X; this->Y = p1.Y; return *this; }
		Vec2  operator +=(const Vec2 p1) { this->X += p1.X; this->Y += p1.Y; return *this; }
		Vec2  operator -=(const Vec2 p1) { this->X -= p1.X; this->Y -= p1.Y; return *this; }
		Vec2  operator *=(const Vec2 p1) { this->X *= p1.X; this->Y *= p1.Y; return *this; }
		Vec2  operator /=(const Vec2 p1) { this->X /= p1.X; this->Y /= p1.Y; return *this; }

		//@}

		/// @name Comparison operators
		/// Implements comparison between vectors ("<" and ">" are not defined)
		//@{

		bool  operator ==(const Vec2 v) { return (this->X == v.X && this->Y == v.Y); }
		bool  operator !=(const Vec2 v) { return (this->X != v.X || this->Y != v.Y); }

		//@}

		/*!
		 unary operators
		! Automatic casting to float* (or float[]) when passing a VecX as parameter
		*/
		operator float* () { return (float*)this; }
		operator const float* () const { return (const float*)this; }
		//! Array operator: access X,Y using indices 0,1
		float& operator[] (int idx) { return idx == 0 ? this->X : this->Y; }
	};

	//! 3d vector structure
	struct Vec3 : public Vec2
	{

		//! X,Y,Z coordinates (X and Y are inherited from Vec2)
		float Z;

		/// @name Contructors
		//@{

		Vec3() { this->Z = 0.0; }
		Vec3(float x) { this->X = this->Y = this->Z = x; }
		Vec3(float x, float y, float z) : Vec2(x, y) { this->Z = z; }
		Vec3(const float f[2], float z) { if (f) { this->X = f[0]; this->Y = f[1]; this->Z = z; } }
		Vec3(const float f[3]) { if (f) { this->X = f[0]; this->Y = f[1]; this->Z = f[2]; } }
		Vec3(const Point2& p) { this->X = (float)p.X; this->Y = (float)p.Y; this->Z = 0.0f; }

		//@}

		/// @name Methods
		///@{

		  //! Set the values of this vector
		void Set(float x, float y, float z)
		{
			this->X = x;
			this->Y = y;
			this->Z = z;
		}

		//! Set the selected component(s) of this vector (default=all)
		void Set(Vec3 v, unsigned char changemask = DS_USEXYZ)
		{
			if (changemask&DS_USEX) this->X = v.X;
			if (changemask&DS_USEY) this->Y = v.Y;
			if (changemask&DS_USEZ) this->Z = v.Z;
		}

		//! Cross product between this vector and another one (this vector doesn't change)
		Vec3 Cross(Vec3 v) const { Vec3 r; VecCrossProduct((*(Vec3*)this), v, r); return r; }

		//! Cross product between two vectors (this vector become the cross product)
		Vec3 Cross(Vec3 u, Vec3 v) { VecCrossProduct(u, v, *this); return *this; }

		//! Dot product between this vector and another one (this vector doesn't change)
		float  Dot(Vec3 v) const { return VecDotProduct(*(Vec3*)this, v); }

		//! Dot product between two vectors (this vector doesn't change)
		float  Dot(Vec3 u, Vec3 v) const { return VecDotProduct(u, v); }

		//! Compute the length (norm) of this vector
		float  Length() const { return VecNorm(*(Vec3*)this); }

		//! Compute the square length (norm^2) of this vector
		float  SquareLength() const { return X*X + Y*Y + Z*Z; }

		//! Set the length of this vector keeping its direction
		Vec3&   SetLength(float l) { Normalize(); *this *= l; return *this; }

		//! Compute the distance of this point from the given one
		float  Distance(Vec3 v) const { return VecNorm(*(Vec3*)this - v); }

		//! Compute the square distance (faster) of this point from the given one
		float  SquareDistance(Vec3 v) const { return (*(Vec3*)this - v).SquareLength(); }

		//! Compute the orthogonal distance of this point from the given line
		float  DistanceToLine(Vec3 u) const
		{
			Vec3 p = ProjectedOnVector(u) - *this;
			return p.Length();
		}

		//! Normalize this vector and return itself
		Vec3& Normalize() { VecNormalize(*this); return *this; }

		//! Return this vector normalized (this vector doesn't change)
		Vec3 Normalized() const { Vec3 v = *this; VecNormalize(v); return v; }

		//! Return this vector normalized (this vector doesn't change)
		Vec3 AbsValues() const { return Vec3(fabs(X), fabs(Y), fabs(Z)); }

		//! Project this vector on another vector, if its length is zero do not change
		Vec3& ProjectOnVector(Vec3 u)
		{
			*this = ProjectedOnVector(u);
			return *this;
		}

		//! Project this vector on another vector (this vector doesn't change), if its length is zero do not change
		Vec3 ProjectedOnVector(Vec3 u) const
		{

			/*!
			 u^.v^ = u*v*cos(a)
			 cos(a) = p/v
			 p = v * cos(a) = u*v*cos(a)/u = u^.v^ / u
			 ^p = p*u^/u = (u^.v^ / (u*u)) * u^
			 v^=this, u^=given vector (u,v norm of vectors), p^=projected vector (p=norm), a=angle
			*/
			const Vec3& v = *this;
			Vec3 p = u*(u.Dot(v) / u.SquareLength());
			return p;
		}

		//! Project a vector on a plane defined by its normal with origin (0,0,0) (this vector doesn't change)
		Vec3 ProjectedOnPlane(const Vec3 normal)
		{
			Vec3& v = *this;
			return v - normal*v.Dot(normal);
		}

		//! Project a vector on a plane defined by its normal with origin (0,0,0) (this vector is changed)
		Vec3& ProjectOnPlane(const Vec3 normal)
		{
			*this = ProjectedOnPlane(normal);
			return *this;
		}

		//! Remap coordinates along the given axis (DS_X = no change, DS_Y align X along Y, DS_Z align X along Z)
		inline Vec3 RemapCoords(int align, float x, float y, float z)
		{
			Vec3 r;
			switch (align)
			{
			case DS_X:
				r.Set(X, Y, Z);
				break;
			case DS_Y:
				r.Set(Z, X, Y);
				break;
			case DS_Z:
				r.Set(Y, Z, X);
				break;
			}
			return r;
		}


		//! Compute the original coordinates after a remapping (see RemapCoords())
		Vec3 UnremapCoords(int align)
		{
			Vec3 r;
			switch (align)
			{
			case DS_X:
				r.Set(X, Y, Z);
				break;
			case DS_Y:
				r.Set(Y, Z, X);
				break;
			case DS_Z:
				r.Set(Z, X, Y);
				break;
			}
			return r;
		}



		///@}

		/// @name Arithmetic operators
		/// Warning! These operate component-by-component (neither cross nor dot product).
		///@{

		Vec3  operator +(const Vec3 p1) const { return Vec3(this->X + p1.X, this->Y + p1.Y, this->Z + p1.Z); }
		Vec3  operator -(const Vec3 p1) const { return Vec3(this->X - p1.X, this->Y - p1.Y, this->Z - p1.Z); }
		Vec3  operator *(const Vec3 v)  const { return Vec3(this->X * v.X, this->Y * v.Y, this->Z * v.Z); }
		Vec3  operator /(const Vec3 v)  const { return Vec3(this->X / v.X, this->Y / v.Y, this->Z / v.Z); }

		//! Negate all the vector components
		Vec3 operator- () const { return Vec3(-(this->X), -(this->Y), -(this->Z)); }

		///@}

		/// @name Scalar operators
		/// Each component of this vector is used with the same scalar value (no change in this vector)
		///@{

		Vec3  operator *(float f) const { return Vec3(this->X * f, this->Y * f, this->Z * f); }
		Vec3  operator /(float f) const { return Vec3(this->X / f, this->Y / f, this->Z / f); }

		///@}

		/// @name Scalar assignment
		/// Each component of this vector is multiplied/divided for a scalar value
		///@{

		Vec3  operator *=(float f) { this->X *= f; this->Y *= f; this->Z *= f; return *this; }
		Vec3  operator /=(float f) { this->X /= f; this->Y /= f; this->Z /= f; return *this; }

		///@}

		/// @name Assignment operators
		/// Assignment from a vector or a vector expression
		///@{

		Vec3& operator = (const Vec3 p1) { this->X = p1.X; this->Y = p1.Y; this->Z = p1.Z; return *this; }
		Vec3  operator +=(const Vec3 p1) { this->X += p1.X; this->Y += p1.Y; this->Z += p1.Z; return *this; }
		Vec3  operator -=(const Vec3 p1) { this->X -= p1.X; this->Y -= p1.Y; this->Z -= p1.Z; return *this; }
		Vec3  operator *=(const Vec3 v) { this->X *= v.X; this->Y *= v.Y; this->Z *= v.Z; return *this; }
		Vec3  operator /=(const Vec3 v) { this->X /= v.X; this->Y /= v.Y; this->Z /= v.Z; return *this; }

		///@}

		/// @name Comparison operators
		/// Implement comparison between vectors (other comparison operators are not defined)
		///@{

		bool operator ==(const Vec3 v) const { return IsAlmostEqual(X, v.X) && IsAlmostEqual(Y, v.Y) && IsAlmostEqual(Z, v.Z); }
		bool operator !=(const Vec3 v) const { return !((*this) == v); }

		///@}

		  //! Array operator: access X,Y,Z using indices 0,1,2
		float& operator[] (int idx) { return idx == 0 ? this->X : idx == 1 ? this->Y : this->Z; }

		//! Test if the vector is entirely made of zeroes
		bool IsZero() const { return IsAlmostZero(X) && IsAlmostZero(Y) && IsAlmostZero(Z); }

		//! Snap the given X,Y,Z values to the given step (>=0)
		void Snap(float step)
		{
			X = gvSnap(X, step);
			Y = gvSnap(Y, step);
			Z = gvSnap(Z, step);
		}

		/*!
		 Convert from Z-up reference system to Y-up
		 @note This method must be tested
		*/
		Vec3 ConvertZupYup() const
		{
			return Vec3(X, Z, -Y);
		}

		/*!
		 Convert from Y-up reference system to Z-up
		 @note This method must be tested
		*/
		Vec3 ConvertYupZup() const
		{
			return Vec3(X, -Z, Y);
		}
	};

	//! 4d point structure
	struct Vec4 : public Vec3
	{

		//! X,Y,Z,W coordinates (X,Y and Z are inherited from Vec3)
		float W;

		/// @name Contructors
		///@{

		Vec4() { this->W = 0.0; }
		Vec4(float x, float y, float z, float w) : Vec3(x, y, z) { this->W = w; }
		Vec4(const float f[3], float w) { this->X = f[0]; this->Y = f[1]; this->Z = f[2]; this->W = w; }
		Vec4(const float f[4]) { this->X = f[0]; this->Y = f[1]; this->Z = f[2]; this->W = f[3]; }

		///@}

		//! Test if the vector is entirely made of zeroes
		bool IsZero() const { return Vec3::IsZero() && IsAlmostZero(W); }

		//! Return a Vec3 built with the first 3 coordinates
		Vec3 XYZ() const { return Vec3(X, Y, Z); }

		/// @name Operators
		///@{

		/*!
		 Assignment from a 4-vector
		*/
		Vec4& operator =(const Vec4 p1) { this->X = p1.X; this->Y = p1.Y; this->Z = p1.Z; this->W = p1.W; return *this; }

		Vec4  operator *(float f) { return Vec4(this->X * f, this->Y * f, this->Z * f, this->W * f); }

		/*
		  Vec4  operator +(Vec4 p1)  { return Vec4(this->X + p1.X, this->Y + p1.Y, this->Z + p1.Z); }
		  Vec4  operator -(Vec4 p1)  { return Vec4(this->X - p1.X, this->Y - p1.Y, this->Z - p1.Z); }
		  Vec4  operator /(float f)    { return Vec4(this->X / f, this->Y / f, this->Z / f); }
		  Vec4  operator +=(Vec4 p1) { this->X += p1.X; this->Y += p1.Y; this->Z += p1.Z; return *this; }
		  Vec4  operator -=(Vec4 p1) { this->X -= p1.X; this->Y -= p1.Y; this->Z -= p1.Z; return *this; }
		  Vec4  operator *=(float f)   { this->X *= f; this->Y *= f; this->Z *= f; return *this; }
		  Vec4  operator /=(float f)   { this->X /= f; this->Y /= f; this->Z /= f; return *this; }

		// unary operators
		  Vec4 operator- ()       { return Vec4(-(this->X), -(this->Y), -(this->Z), -(this->W)); }
		*/

		bool  operator ==(Vec4 v) const { return IsAlmostEqual(X, v.X) && IsAlmostEqual(Y, v.Y) && IsAlmostEqual(Z, v.Z) && IsAlmostEqual(W, v.W); }
		bool  operator !=(Vec4 v) const { return !((*this) == v); }

		//! Array operator: access X,Y,Z,W using indices 0,1,2,3
		float& operator[] (int idx) { return idx == 0 ? this->X : idx == 1 ? this->Y : idx == 2 ? this->Z : this->W; }

		///@}
	};

	///@}

	inline Vec3 operator*(float scalar, const Vec3 &vec) { return vec*scalar; }



	/*!
	 Get the coefficients for 4 control point Bezier interpolation
	 (mu ranges from 0 to 1, start to end of curve)
	*/
	Vec4 GetBezier4params(double mu);

	/*!
	 Four control point Bezier interpolation
	 mu ranges from 0 to 1, start to end of curve
	*/
	Vec3 VecBezier4(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, double mu);

	//! get the euler angles (degrees) from angle (degrees),axis
	Vec3 VecGetEulerFromAngleAxis(float angle, Vec3 axis);

	///@}

	//-----------------------------------------------------------------
	// BOUNDING
	//-----------------------------------------------------------------

	///@name Bounding volumes
	///@{

	struct BoundingBox;

	//! Reset a bounding box structure
	void ResetBoundingBox(BoundingBox &b);

	//! Set boundaries for a bounding box
	bool SetBoundingBox(BoundingBox &b, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);

	//! Get boundaries of a bounding box
	void GetBoundingBoxValues(const BoundingBox &b, float &x_min, float &x_max, float &y_min, float &y_max, float &z_min, float &z_max);

	//! Get the center point of a bounding box
	void GetBoundingBoxCenter(BoundingBox &bb, float p[3]);

	//! Get the size (x,y,z) of a bounding box
	void GetBoundingBoxSize(BoundingBox &bb, float p[3]);

	//! Returns true if point is inside the given ranges
	bool IsVec3InBox(Vec3 p, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);

	//! Returns true if the two ranges intersect
	bool AreRangesIntersecting(float x_min_1, float x_max_1, float y_min_1, float y_max_1, float z_min_1, float z_max_1,
		float x_min_2, float x_max_2, float y_min_2, float y_max_2, float z_min_2, float z_max_2);


	//! Axis Aligned Bounding Box (AABB) structure
	struct AABBox
	{

		///@name Bounding box limits
		///@{

		float Xmin;
		float Xmax;
		float Ymin;
		float Ymax;
		float Zmin;
		float Zmax;
		///@}

		//! Validity flag (used to check for changes)
		bool Valid;

		/// @name Center, size and maximum size (among X,Y,Z) of the AABB
		/// These must be updated using Update() method.
		///@{

		Vec3 Origin;
		Vec3 Center;
		Vec3 Size;
		float MaxSize;

		///@}

		//! Constructor
		AABBox();
		AABBox(const AABBox& bb);
		void Reset();

		//! Setter method with single coordinates
		void Set(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);

		//! Setter method with minimum an maximum corner points
		void Set(Vec3 min_pnt, Vec3 max_pnt);

		//! Constrain a point to be inside the box
		void Constrain(Vec3& pnt);

		//! Test if a point is inside the box
		bool Including(Vec3 pnt);

		//! Test if a box is inside the box
		bool IncludingBox(AABBox bb);

		//! Change th box to include a point
		void IncludePoint(Vec3 pnt);

		//! Change this box to include the given the box
		void EmbodyBox(const AABBox& bb);

		//! Set the box using centre and size
		void SetCenterSize(Vec3 c, Vec3 s);

		//! Set the box size
		void SetSize(Vec3 s);

		//! Set the box size
		void SetSizeOffsetAlignment(Vec3 size, Vec3 offset, int align);

		//! Set the box centre
		void SetCenter(Vec3 c);

		//! Get the position of the specified alignment
		Vec3 GetAlignPos(int align);

		//! Update and calculate AABB attributes
		void Update();

		//! Access AABB limits using an index (0=Xmin,1=Xmax,2=Ymin,...)
		float operator[] (int idx);

		AABBox& operator =(const AABBox& bb);
	};

	class Mat4;

	//! bounding box structure
	struct BoundingBox
	{

		Vec3 xyz;
		Vec3 xyZ;
		Vec3 xYz;
		Vec3 xYZ;
		Vec3 Xyz;
		Vec3 XyZ;
		Vec3 XYz;
		Vec3 XYZ;

		bool Valid;

		/// @name Center, size and maximum size (among X,Y,Z) of the AABB
		/// Must be updated using Update() method
		///@{

		Vec3 Center;
		Vec3 Size;
		float MaxSize;

		///@}

		BoundingBox();
		void Reset();
		void Set(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
		void Update();

		Vec3& GetCenter();

		AABBox GetAABB() const;

		//! Extend the bounding box to include the given box
		void EmbodyBox(const BoundingBox& bb);

		//! Set the bounding box wrapping the given points
		void WrapPoints(Vec3* points, int count);

		//! Extend the bounding box to include the given points
		void IncludePoints(Vec3* points, int count, Mat4* transformMatrix = nullptr);

		Vec3& operator[] (int idx);
		BoundingBox& operator =(const BoundingBox& bb);
		BoundingBox& operator =(const AABBox& aabb);
		bool operator ==(const BoundingBox& bb);
		bool operator !=(const BoundingBox& bb) { return !(*this == bb); }
	};

	//! Bounding sphere
	struct BoundingSphere
	{
		Vec3 Center;
		float Radius;

		BoundingSphere();
		BoundingSphere(const BoundingBox& bb);
		BoundingSphere(Vec3 c, float r);

		BoundingSphere& operator =(const BoundingSphere& bs) { Center = bs.Center; Radius = bs.Radius; return *this; }
		BoundingSphere& operator =(const AABBox& aabb)
		{
			Center = aabb.Center;
			Radius = (aabb.Size*0.5f).Length();
			return *this;
		}
		BoundingSphere& operator =(const BoundingBox& bb)
		{
			Center = bb.Center;
			Radius = (bb.Size*0.5f).Length();
			return *this;
		}

		bool operator ==(const BoundingSphere& bs) { return (bs.Center == Center && bs.Radius == Radius); }
		void Reset();

		//! Set this sphere center and radius to include the given points
		void WrapPoints(Vec3* points, int count);

		//! Keepeing the current center set the sphere radius to enclose the given points
		void IncludePoints(Vec3* points, int count);

		BoundingSphere& EmbodyBoundingSphere(const BoundingSphere& bs);
	};

	//@}

	//-----------------------------------------------------------------
	// QUATERNIONS
	//-----------------------------------------------------------------

	///@name Quaternions
	///@{

	struct Quat;
	class Mat4;

	//! Multiplication between quaternions: result = q1 x q2.
	Quat QuatMultiply(Quat q1, Quat q2);

	//! Makes a rotation quaternion given an angle (in degrees) and a generic axis
	void QuatFromAxisAngle(float axis[3], float angle, Quat &q);

	//! Makes a rotation quaternion given the Euler angles (in degrees)
	void QuatFromEulerAngles(float angles[3], Quat &q);

	//! Makes a rotation quaternion given a rotation matrix (assuming no scaling)
	Quat QuatFromMatrix(const float m[16]);

	//! Makes a rotation quaternion given a rotation matrix
	Mat4 QuatToMatrix(const Quat& q);

	//! Rotation of p on the axis "axis" of angle degrees using quaternions
	void QuatRotate(float p[3], float axis[3], float angle, float result[3]);

	//! Quaternion normalization
	Quat QuatNormalize(const Quat& q);

	//! Quaternion negation
	Quat QuatNegate(Quat q);

	//! Quaternion inversion
	Quat QuatInvert(Quat q);

	//! Returns the axis and angle (in degrees) given a quaternion
	void QuatToAxisAngle(const Quat& q, Vec3& axis, float& angle);

	//! Returns the euler angles (in degrees) given a quaternion
	void QuatToEulerAngles(const Quat& q, Vec3& euler_angles);

	//! Returns the Spherical Linear inteERPolation of two (unit) quaternions, with trim = 0..1
	Quat QuatSlerp(const Quat& q1, float trim, const Quat& q2);

	///@}

	//-----------------------------------------------------------------
	// MATRICES
	//-----------------------------------------------------------------

	///@name Matrices
	///@{

	//! Check if the given matrix is the identity matrix
	bool MatIsIdentity(const float m[16]);

	//! Check if the given matrix is the a pure rotation matrix
	bool MatIsRotation(float m[16]);

	//! Returns the identity matrix
	void MatMakeIdentity(float m[16]);

	//! Rreturns the translation matrix
	void MatMakeTranslation(float t[3], float m[16]);

	//! Returns the scaling matrix
	void MatMakeScaling(float s[3], float m[16]);

	/*!
	 Make the euler matrix
	 @param angles a vector with the angles around X, Y, Z axes
	 @param m 4x4 column-major matrix where the result is stored
	*/
	void MatMakeEuler(Vec3 angles, float m[16]);

	/*!
	 Returns the euler angles given a matrix
	 @return a vector with the angles around X, Y, Z axes
	 @param m 4x4 column-major matrix used to compute the angles
	*/
	Vec3 MatGetEuler(const float m[16]);

	//! Get the axis-angle rotation from a <B>pure</B> rotation matrix.
	Vec4 MatGetAxisAngle(const float m[16]);

	//! Returns a rotation matrix
	void MatMakeRotation(float angle, float axis[3], float m[16]);

	//! Rows x columns 4x4 matrix product
	void MatProduct(const float m1[16], const float m2[16], float r[16]);

	//! Rows x columns 4x4 product
	float MatRowColumnProduct(float m1[16], unsigned int row, float m2[16], unsigned int column);

	//! Rows x columns generic matrix product of order n (less efficient)
	bool MatProduct(float *m1, float *m2, float *r, unsigned int n);

	//! Rows x columns generic product of order n (less efficient)
	float MatRowColumnProduct(float *m1, unsigned int row, float *m2, unsigned int column, unsigned int n);

	//! 1x4 point X 4x4 column-major matrix (result can be the same as point)
	void MatPointTransform(const float point[3], const float matrix[16], float result[3]);

	//! 1x4 vector X 4x4 column-major matrix (result can be the same as vector)
	void MatVecTransform(const float vector[3], const float matrix[16], float result[3]);

	//! Box X 4x4 column-major matrix (result can be the same as box)
	void MatBoxTransform(BoundingBox box, float matrix[16], BoundingBox &result);

	//! Sphere X 4x4 column-major matrix (result can be the same as sphere)
	void MatSphereTransform(BoundingSphere sphere, float matrix[16], BoundingSphere &result);

	//! Transposed of m (returned into m) of order n
	bool MatTranspose(float *m, unsigned int n = 4);

	//! Inverse of m (returned into m) of order n
	bool MatInvert(float *m, unsigned int n = 4);

	//! Determinant of a n-sided matrix
	float MatDeterminant(const float *m, unsigned int n = 4);

	//! Given a n x n matrix returns the (n-1) x (n-1) matrix obtained removing a row and a column
	void MatGetSubMatrix(const float *src, float *dst, unsigned int src_order,
		unsigned int row_to_skip, unsigned int column_to_skip);

	//! Copy src matrix to dst
	inline void MatCopy(float dst[16], const float src[16]) { memcpy(dst, src, 16 * sizeof(float)); }

	//! Set rotation matrix (leaving the translation unchanged)
	void MatSetRotation(float mat[16], float rot[3]);

	//! set translation matrix (leaving the rotation unchanged)
	void MatSetTranslation(float mat[16], const float pos[3]);

	//! get translation from a matrix
	void MatGetTranslation(const float mat[16], float pos[3]);

	//! Compute the homogeneous matrix according to given position and rotation (degrees)
	void MatMakeHomogeneous(float mat[16], float pos[3], float rot[3]);

	//! Returns a rotation matrix given a quaternion
	void MatMakeRotFromQuat(const Quat& q, float m[16], bool reset_translation = true);

	//! Use lookup table to quickly detect 90 degrees rotations (return false if no entry is found)
	bool MatAngleLookUp(const float m[16], Vec3* angles = nullptr);

	//! Calculate the axis and angle for rotating the given vector dir (starting at the given origin, orig) toward the given point, pnt
	void VecLookAtPointAxisAngle(const Vec3& orig, const Vec3& dir, const Vec3& pnt, Vec3& axis, float& angle);

	///@}

	//! Quaternion structure.
	struct Quat
	{

		Vec3 V;
		float S;

		Quat() { Reset(); }
		Quat(const Quat& q) { S = q.S; V = q.V; }
		Quat(float s, Vec3 v) : S(s), V(v) {}
		Quat(Vec3 axis, float angle) { SetAxisAngle(axis, angle); }
		Quat(float q[4]) { V.Set(q[0], q[1], q[2]); S = q[3]; }

		//! Normalize this quaternion and return a reference to it
		Quat& Normalize() { *this = QuatNormalize(*this); return *this; }

		//! Return a copy of this quaternion normalized
		Quat Normalized() const { Quat q(QuatNormalize(*this)); return q; }

		//! Invert this quaternion and return a reference to it
		Quat& Invert() { *this = QuatInvert(*this); return *this; }

		//! Return a copy of this quaternion inverted
		Quat Inverted() const { return QuatInvert(*this); }

		//! Set this quaternion according to the given rotation matrix
		void SetMatrix(const float m[16]) { *this = QuatFromMatrix(m); }

		//! Reset the quaternion to (0,0,0,1)
		void Reset() { V.Set(0.0f, 0.0f, 0.0f); S = 1.0f; }

		//! Get the rotation matrix from this quaternion
		Mat4 GetMatrix();

		//! Set this quaternion according to the given axis and angle
		void SetAxisAngle(Vec3 axis, float angle) { QuatFromAxisAngle(axis, angle, *this); }

		//! Set this quaternion according to the given Euler angles
		void SetEulerAngles(Vec3 angles) { QuatFromEulerAngles(angles, *this); }

		//! Returns the axis and angle given a quaternion
		void GetAxisAngle(Vec3& axis, float& angle) const { QuatToAxisAngle(*this, axis, angle); }

		//! Returns the euler angles given a quaternion
		Vec3 GetEulerAngles() const { Vec3 euler_angles; QuatToEulerAngles(*this, euler_angles); return euler_angles; }

		Quat Slerp(float t, const Quat& q) const { return QuatSlerp(*this, t, q); }

		operator Vec4 () { return (Vec4)(*this); }
		operator const Vec4() const { return (const Vec4)(*this); }
		float& operator[] (int idx) { return idx >= 3 ? this->S : this->V[idx]; }
		const float& operator[] (int idx) const { return idx >= 3 ? this->S : this->V[idx]; }
		Quat& operator =(const Quat& q) { memcpy(this, &q, sizeof(Quat)); return *this; }
		Quat operator *(const Quat& q) const { Quat q1 = QuatMultiply(*this, q); return q1; }
		Quat operator *(float f) const { Quat q(*this); q.V *= f; q.S *= f; return q; }
		Quat operator +(const Quat& q) const { Quat q1(this->S + q.S, this->V + q.V); return q1; }
		Quat operator -(const Quat& q) const { Quat q1(this->S - q.S, this->V - q.V); return q1; }
		Quat& operator *=(const Quat& q) { *this = QuatMultiply(*this, q); return *this; }
		Quat& operator *=(float f) { V *= f; S *= f; return *this; }
		Quat operator -() { Quat q(S, -V); return q; }
		bool operator ==(const Quat& q) const { return q.V == V && q.S == S; }
		bool operator !=(const Quat& q) const { return !(*this == q); }
	};




	//-----------------------------------------------------------------

	//! 4x4 column-major matrix
	class Mat4
	{

		//! Column major matrix
		float M[16];

	public:

		/// @name Contructors
		///@{

		Mat4()
		{
			MatMakeIdentity(M);
		}

		//! Set the matrix values in column-major order (m00, m10, m20, m30, m01, m11,...)
		Mat4(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7,
			float m8, float m9, float m10, float m11, float m12, float m13, float m14, float m15)
		{
			SetColValues(m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);
		}

		Mat4(const float m[16])
		{
			memcpy(M, m, sizeof(Mat4));
		}

		///@}

		//! Set the matrix values in row-major order (m00, m01, m02, m03, m10, m11,...)
		void SetRowValues(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7,
			float m8, float m9, float m10, float m11, float m12, float m13, float m14, float m15)
		{
			M[0] = m0; M[1] = m4; M[2] = m8; M[3] = m12;
			M[4] = m1; M[5] = m5; M[6] = m9; M[7] = m13;
			M[8] = m2; M[9] = m6; M[10] = m10; M[11] = m14;
			M[12] = m3; M[13] = m7; M[14] = m11; M[15] = m15;
		}

		//! Set the matrix values in row-major order (m00, m01, m02, m03, m10, m12,...)
		void SetColValues(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7,
			float m8, float m9, float m10, float m11, float m12, float m13, float m14, float m15)
		{
			M[0] = m0; M[1] = m1; M[2] = m2; M[3] = m3;
			M[4] = m4; M[5] = m5; M[6] = m6; M[7] = m7;
			M[8] = m8; M[9] = m9; M[10] = m10; M[11] = m11;
			M[12] = m12; M[13] = m13; M[14] = m14; M[15] = m15;
		}

		//! Copy a matrix
		void Copy(Mat4 m) { memcpy(M, m, sizeof(Mat4)); }

		//! Check if this matrix is the identity
		bool IsIdentity() const
		{
			return M[0] == 1.0f && M[1] == 0.0f && M[2] == 0.0f && M[3] == 0.0f
				&& M[4] == 0.0f && M[5] == 1.0f && M[6] == 0.0f && M[7] == 0.0f
				&& M[8] == 0.0f && M[9] == 0.0f && M[10] == 1.0f && M[11] == 0.0f
				&& M[12] == 0.0f && M[13] == 0.0f && M[14] == 0.0f && M[15] == 1.0f;
		}

		//! Reset this matrix to the identity
		void Reset() { MakeIdentity(); }

		/// @name Setup a specific matrix
		///@{

		void MakeZero()
		{// memset(M,0,sizeof(Mat4)); }
			for (int i = 0; i < 16; i++) M[i] = 0.0f;
		}

		void MakeIdentity()
		{// MatMakeIdentity(M); }
			M[0] = 1.0f; M[1] = 0.0f; M[2] = 0.0f; M[3] = 0.0f;
			M[4] = 0.0f; M[5] = 1.0f; M[6] = 0.0f; M[7] = 0.0f;
			M[8] = 0.0f; M[9] = 0.0f; M[10] = 1.0f; M[11] = 0.0f;
			M[12] = 0.0f; M[13] = 0.0f; M[14] = 0.0f; M[15] = 1.0f;
		}

		void MakeTranslation(float t[3]) { MatMakeTranslation(t, M); }

		void MakeScaling(float s[3]) { MatMakeScaling(s, M); }

		void MakeEuler(float x_angle, float y_angle, float z_angle) { MatMakeEuler(Vec3(x_angle, y_angle, z_angle), M); }

		void MakeEuler(Vec3 angles) { MatMakeEuler(angles, M); }

		void MakeRotation(float angle, float axis[3]) { MatMakeRotation(angle, axis, M); }

		void MakeRotQuat(const Quat& q) { MatMakeRotFromQuat(q, M); }

		void MakeHomogeneous(float pos[3], float rot[3]) { MatMakeHomogeneous(M, pos, rot); }

		/*!
		@}
		! Set rotation matrix using Euler angles (overrides scaling, leaving the translation unchanged)
		*/
		void SetRotation(float rot[3]) { MatSetRotation(M, rot); }

		//! Copy the first 3 columns of the matrix from the given one (leaving the translation unchanged)
		void CopySubMatrix3(const Mat4& m) { SetCol(0, m.GetCol(0)); SetCol(1, m.GetCol(1)); SetCol(2, m.GetCol(2)); }

		//! Set the rotation submatrix from a quaternion (leaving the translation unchanged)
		void SetRotationQuat(const Quat& q) { MatMakeRotFromQuat(q, M, false); }

		//! Set scaling matrix (overrides rotation, leaving the translation unchanged)
		void SetScaling(float s[3]) { Vec3 t = GetTranslation(); MatMakeScaling(s, M); SetTranslation(t); }

		//! Set translation matrix (leaving the rotation unchanged)
		void SetTranslation(const Vec3 pos, unsigned char changemask = DS_USEXYZ)
		{
			if (changemask&DS_USEX) M[12] = pos.X;
			if (changemask&DS_USEY) M[13] = pos.Y;
			if (changemask&DS_USEZ) M[14] = pos.Z;
		}

		//! Get translation vector
		Vec3 GetTranslation() const
		{
			Vec3 t;
			MatGetTranslation(M, t);
			return t;
		}

		//! Get rotation matrix and return scaling
		Vec3 GetRotScaling(Mat4& r) const
		{
			Vec3 xaxis(GetCol(0));
			Vec3 yaxis(GetCol(1));
			Vec3 zaxis(GetCol(2));

			Vec3 sc(xaxis.Length(), yaxis.Length(), zaxis.Length());
			r.SetCol(0, xaxis / sc.X);
			r.SetCol(1, yaxis / sc.Y);
			r.SetCol(2, zaxis / sc.Z);

			return sc;
		}

		//! Get the scaling (if you need also the rotation matrix use GetRotScaling() )
		Vec3 GetScaling() const
		{
			Vec3 xaxis(GetCol(0));
			Vec3 yaxis(GetCol(1));
			Vec3 zaxis(GetCol(2));

			return Vec3(xaxis.Length(), yaxis.Length(), zaxis.Length());
		}

		//! Remove the scaling
		void RemoveScaling()
		{
			Vec3 xaxis(GetCol(0));
			Vec3 yaxis(GetCol(1));
			Vec3 zaxis(GetCol(2));

			Vec3 sc(xaxis.Length(), yaxis.Length(), zaxis.Length());
			if (sc.X) SetCol(0, xaxis / sc.X);
			if (sc.Y) SetCol(1, yaxis / sc.Y);
			if (sc.Z) SetCol(2, zaxis / sc.Z);
		}

		//! Get rotation matrix (if you need also the scaling use GetRotScaling() )
		Mat4 GetRotMat() const
		{
			Mat4 r;
			GetRotScaling(r);

			return r;
		}

		//! Extract axis and angle from this matrix (assume no scaling is applied)
		void GetRotAxisAngle(Vec3& axis, float& angle) const { return QuatFromMatrix(M).GetAxisAngle(axis, angle); }

		//! Extract Euler angles from this matrix (assume no scaling is applied)
		Vec3 GetEuler() const { return MatGetEuler(M); }

		//! Invert this matrix
		float Determinant() const { return MatDeterminant(M); }

		//! Invert this matrix
		void Invert() { MatInvert(M); }

		//! Transpose this matrix
		void Transpose() { MatTranspose(M); }

		//! Invert this matrix
		Mat4 Inverted() const { Mat4 mat(M); MatInvert(mat); return mat; }

		/*!
		 Convert from Z-up reference system to Y-up
		 @todo This method must be tested
		*/
		Mat4 ConvertZupYup() const
		{
			/*!
			 Z-up-->Y-up matrix and its inverse must be muliplied before and after the given matrix
			 My = Tyz*Mz*Tzy
			 My matrix in the Y-up reference system
			 Mz matrix in the Z-up reference system
			 Tzy = matrix from the Z-up reference system to the Y-up reference system
			 Tyz = inverse of Tzy
			 (the rotation is defined in the Y-up reference system)
			 |1  0  0  0|   |Mx.X My.X Mz.X Mw.X|   |1  0  0  0|   | Mx.X -Mz.X  My.X  Mw.X|
			 |0  0 -1  0| * |Mx.Y My.Y Mz.Y Mw.Y| * |0  0  1  0| = |-Mx.Z  Mz.Z -My.Z  Mw.Z|
			 |0  1  0  0|   |Mx.Z My.Z Mz.Z Mw.Z|   |0 -1  0  0|   | Mx.Y -Mz.Y  My.Y -Mw.Y|
			 |0  0  0  1|   |Mx.W My.W Mz.W Mw.W|   |0  0  0  1|   | Mx.W -Mz.Y  My.Y  Mw.W|
			 Mx,My,Mz,Mw = first 3 matrix columns = axes in Y-up reference system
			 Mw = last matrix column
			*/
			Mat4 m;
			m.SetCol(0, Vec4(Get(0, 0), Get(0, 2), -Get(0, 1), Get(0, 3)));
			m.SetCol(1, Vec4(Get(2, 0), Get(2, 2), -Get(2, 1), Get(1, 3)));
			m.SetCol(2, Vec4(-Get(1, 0), -Get(1, 2), Get(1, 1), Get(2, 3)));
			m.SetCol(3, Vec4(Get(3, 0), Get(3, 2), -Get(3, 1), Get(3, 3)));
			return m;
		}

		/*!
		 Convert from Y-up reference system to Z-up
		 @todo This method must be tested
		*/
		Mat4 ConvertYupZup() const
		{
			/*!
			 Y-up --> Z-up matrix and its inverse must be muliplied before and after the given matrix
			 Mz = Tzy*My*Tyz
			 My matrix in the Y-up reference system
			 Mz matrix in the Z-up reference system
			 Tyz = matrix transforming from the Y-up to the Z-up reference system
			 Tzy = inverse of Tyz
			 (the rotation is defined in the Z-up reference system)
			 |1  0  0  0|   |Mx.X My.X Mz.X Mw.X|   |1  0  0  0|   | Mx.X  Mz.X -My.X  Mw.X|
			 |0  0  1  0| * |Mx.Y My.Y Mz.Y Mw.Y| * |0  0 -1  0| = | Mx.Z  Mz.Z -My.Z -Mw.Z|
			 |0 -1  0  0|   |Mx.Z My.Z Mz.Z Mw.Z|   |0  1  0  0|   |-Mx.Y -Mz.Y  My.Y  Mw.Y|
			 |0  0  0  1|   |Mx.W My.W Mz.W Mw.W|   |0  0  0  1|   | Mx.W -Mz.Y  My.Y  Mw.W|
			 Mx,My,Mz = matrix columns = axes in Z-up reference system
			*/
			Mat4 m;
			m.SetCol(0, Vec4(Get(0, 0), -Get(0, 2), Get(0, 1), Get(0, 3)));
			m.SetCol(1, Vec4(-Get(2, 0), Get(2, 2), -Get(2, 1), Get(1, 3)));
			m.SetCol(2, Vec4(Get(1, 0), -Get(1, 2), Get(1, 1), Get(2, 3)));
			m.SetCol(3, Vec4(Get(3, 0), -Get(3, 2), Get(3, 1), Get(3, 3)));
			return m;
		}


		//! Output to console the matrix elements
		void Print() const
		{
			for (int i = 0; i < 4; i++)
            {
                 for (int j = 0; j < 4; j++)
                 {
                     printf("%4.3f ", M[j * 4 + i]);
                 }
                 printf("\n");
             }
		}

		//! Get the matrix value at the given row and column
		const float& Get(int row, int col) const
		{
			//if( col<0 || col>3 || row<0 || row>3 ) throw( std::out_of_range("Mat4::Get()");
			return M[4 * col + row];
		}

		//! Set a row (from 0 to 3)
		void SetRow(int which, const Vec4& row)
		{
			switch (which)
			{
			case 0: M[0] = row[0]; M[4] = row[1]; M[8] = row[2]; M[12] = row[3]; break;
			case 1: M[1] = row[0]; M[5] = row[1]; M[9] = row[2]; M[13] = row[3]; break;
			case 2: M[2] = row[0]; M[6] = row[1]; M[10] = row[2]; M[14] = row[3]; break;
			case 3: M[3] = row[0]; M[7] = row[1]; M[11] = row[2]; M[15] = row[3]; break;
			}
		}

		//! Set a column (from 0 to 3)
		void SetCol(int which, const Vec4& col)
		{
			//    if(which>=0 && which<=3) memcpy(&M[which],col,4*sizeof(float));
			switch (which)
			{
			case 0: M[0] = col[0]; M[1] = col[1]; M[2] = col[2]; M[3] = col[3]; break;
			case 1: M[4] = col[0]; M[5] = col[1]; M[6] = col[2]; M[7] = col[3]; break;
			case 2: M[8] = col[0]; M[9] = col[1]; M[10] = col[2]; M[11] = col[3]; break;
			case 3: M[12] = col[0]; M[13] = col[1]; M[14] = col[2]; M[15] = col[3]; break;
			}
		}

		//! Set a row (from 0 to 3)
		void SetRow(int which, const Vec3& row)
		{
			switch (which)
			{
			case 0: M[0] = row[0]; M[4] = row[1]; M[8] = row[2]; break;
			case 1: M[1] = row[0]; M[5] = row[1]; M[9] = row[2]; break;
			case 2: M[2] = row[0]; M[6] = row[1]; M[10] = row[2]; break;
			case 3: M[3] = row[0]; M[7] = row[1]; M[11] = row[2]; break;
			}
		}

		//! Set a column (from 0 to 3)
		void SetCol(int which, const Vec3& col)
		{
			//    if(which>=0 && which<=3) memcpy(&M[which],col,4*sizeof(float));
			switch (which)
			{
			case 0: M[0] = col[0]; M[1] = col[1]; M[2] = col[2]; break;
			case 1: M[4] = col[0]; M[5] = col[1]; M[6] = col[2]; break;
			case 2: M[8] = col[0]; M[9] = col[1]; M[10] = col[2]; break;
			case 3: M[12] = col[0]; M[13] = col[1]; M[14] = col[2]; break;
			}
		}

		//! Get a row (from 0 to 3)
		Vec4 GetRow(int which) const
		{
			Vec4 row;
			switch (which)
			{
			case 0: row[0] = M[0]; row[1] = M[4]; row[2] = M[8]; row[3] = M[12]; break;
			case 1: row[0] = M[1]; row[1] = M[5]; row[2] = M[9]; row[3] = M[13]; break;
			case 2: row[0] = M[2]; row[1] = M[6]; row[2] = M[10]; row[3] = M[14]; break;
			case 3: row[0] = M[3]; row[1] = M[7]; row[2] = M[11]; row[3] = M[15]; break;
			}
			return row;
		}

		//! Get a column (from 0 to 3)
		Vec4 GetCol(int which) const
		{
			//    return Vec4(&M[which]);
			Vec4 col;
			switch (which)
			{
			case 0: col[0] = M[0]; col[1] = M[1]; col[2] = M[2]; col[3] = M[3]; break;
			case 1: col[0] = M[4]; col[1] = M[5]; col[2] = M[6]; col[3] = M[7]; break;
			case 2: col[0] = M[8]; col[1] = M[9]; col[2] = M[10]; col[3] = M[11]; break;
			case 3: col[0] = M[12]; col[1] = M[13]; col[2] = M[14]; col[3] = M[15]; break;
			}
			return col;
		}

		//! Assignment operator
		Mat4& operator =(Mat4 m) { memcpy(M, m, sizeof(Mat4)); return *this; }

		//! Assignment operator for quaternion
		Mat4& operator =(Quat q) { MatMakeRotFromQuat(q, M); return *this; }

		///@name Arithmetic operators
		///@{

		//! Component-by-component sum.
		Mat4 operator +(Mat4 m) const
		{
			return Mat4(
				M[0] + m[0], M[1] + m[1], M[2] + m[2], M[3] + m[3],
				M[4] + m[4], M[5] + m[5], M[6] + m[6], M[7] + m[7],
				M[8] + m[8], M[9] + m[9], M[10] + m[10], M[11] + m[11],
				M[12] + m[12], M[13] + m[13], M[14] + m[14], M[15] + m[15]);
		}

		//! Component-by-component subtraction.
		Mat4 operator -(Mat4 m) const
		{
			return Mat4(
				M[0] - m[0], M[1] - m[1], M[2] - m[2], M[3] - m[3],
				M[4] - m[4], M[5] - m[5], M[6] - m[6], M[7] - m[7],
				M[8] - m[8], M[9] - m[9], M[10] - m[10], M[11] - m[11],
				M[12] - m[12], M[13] - m[13], M[14] - m[14], M[15] - m[15]);
		}

		//! Add translation vector.
		Mat4 operator +(Vec3 v) const
		{
			return Mat4(
				M[0], M[1], M[2], M[3],
				M[4], M[5], M[6], M[7],
				M[8], M[9], M[10], M[11],
				M[12] + v[0], M[13] + v[1], M[14] + v[2], M[15]);
		}

		//! Subtract translation vector.
		Mat4 operator -(Vec3 v) const
		{
			return Mat4(
				M[0], M[1], M[2], M[3],
				M[4], M[5], M[6], M[7],
				M[8], M[9], M[10], M[11],
				M[12] - v[0], M[13] - v[1], M[14] - v[2], M[15]);
		}

		//! Multiply for a scalar.
		Mat4 operator *(float f) const
		{
			return Mat4(
				M[0] * f, M[1] * f, M[2] * f, M[3] * f,
				M[4] * f, M[5] * f, M[6] * f, M[7] * f,
				M[8] * f, M[9] * f, M[10] * f, M[11] * f,
				M[12] * f, M[13] * f, M[14] * f, M[15] * f);
		}

		//! Divide by a scalar.
		Mat4 operator /(float f) const
		{
			return Mat4(
				M[0] / f, M[1] / f, M[2] / f, M[3] / f,
				M[4] / f, M[5] / f, M[6] / f, M[7] / f,
				M[8] / f, M[9] / f, M[10] / f, M[11] / f,
				M[12] / f, M[13] / f, M[14] / f, M[15] / f);
		}

		//! Negate all the elements
		Mat4 operator- () const
		{
			return Mat4(
				-M[0], -M[1], -M[2], -M[3],
				-M[4], -M[5], -M[6], -M[7],
				-M[8], -M[9], -M[10], -M[11],
				-M[12], -M[13], -M[14], -M[15]);
		}

		///@}

		/*!
		 Matrix product (this x m).
		 @return The product is returned and the matrix is leaved unchanged
		*/
		Mat4 operator *(const Mat4& m) const
		{
			Mat4 r;
			MatProduct(M, (const float*)m, r);
			return r;
		}

		/*!
		 Matrix-vector product, point transformation (v x this).
		 @return The product is returned and the matrix is leaved unchanged
		*/
		Vec3 operator *(Vec3 v) const
		{
			Vec3 result;
			MatPointTransform(v, M, result);
			return result;
		}

		/*!
		 Matrix-vector product, vector transformation (v x this).
		 @return The product is returned and the matrix is leaved unchanged
		*/
		Vec3 operator *(Vec4 v) const
		{
			Vec3 result;
			MatVecTransform(v, M, result);
			return result;
		}

		///@name Expression assignment operators
		///@{

		Mat4 operator +=(Mat4 m) { (*this) = ((*this) + m); return *this; }
		Mat4 operator -=(Mat4 m) { (*this) = ((*this) - m); return *this; }
		Mat4 operator +=(Vec3 v) { (*this) = ((*this) + v); return *this; }
		Mat4 operator -=(Vec3 v) { (*this) = ((*this) - v); return *this; }
		Mat4 operator *=(float f) { (*this) = ((*this)*f); return *this; }
		Mat4 operator /=(float f) { (*this) = ((*this) / f); return *this; }
		Mat4 operator *=(Mat4 m) { (*this) = ((*this)*m); return *this; }

		///@}

		///@name Comparison operators
		///@{

		bool operator ==(float m[16]) const { return memcmp((const void*)M, (const void*)m, sizeof(Mat4)) == 0; }

		bool operator !=(float m[16]) const { return memcmp((const void*)M, (const void*)m, sizeof(Mat4)) != 0; }

		///@}

		  //! Automatic cast operator (convert a Mat4 to a float* or float[])
		operator float* () { return (float*)this; }

		operator const float* () const { return (float*)this; }

		//! Indexing of the matrix (0=M-0-0,1=M-0-1,...,4=M-1-0,5=M-1-1,...)
		float& operator[] (int idx) { return M[idx]; }

	};

	inline Mat4 Quat::GetMatrix() { return QuatToMatrix(*this); }


	//-----------------------------------------------------------------
	// POSITIONS
	//-----------------------------------------------------------------

	///@name Position
	///@{

	//! Position class
	class PosRot
	{

	public:
		Vec3 Pos;
		Vec3 Rot;
		// constructors
		PosRot() {}
		PosRot(const PosRot& pos) { Pos = pos.Pos; Rot = pos.Rot; }
		PosRot(const Vec3 p) : Pos(p) {}
		PosRot(const Vec3 p, const Vec3 r) : Pos(p), Rot(r) {}

		// methods
		void SetPos(const Vec3 p) { Pos = Vec3(p); }
		void SetPosRot(const Vec3 p, const Vec3 r) { Pos = Vec3(p); Rot = Vec3(r); }
		void SetRot(const Vec3 r) { Rot = Vec3(r); }
		// binary operators
		PosRot& operator =(const PosRot& pos) { Pos = pos.Pos; Rot = pos.Rot; return *this; }
		PosRot operator +(const PosRot& p)
		{
			return PosRot(Pos + p.Pos, Rot + p.Rot);
		}
		PosRot operator -(const PosRot& p)
		{
			return PosRot(Pos - p.Pos, Rot - p.Rot);
		}
		bool operator ==(const PosRot& pos) { return (Pos == pos.Pos && Rot == pos.Rot); }
		bool operator !=(const PosRot& pos) { return (Pos != pos.Pos || Rot != pos.Rot); }
		//! Array operator (prevents out of range errors)
		float& operator[] (int idx) { return idx < 0 ? Pos.X : idx < 3 ? Pos[idx] : idx < 6 ? Rot[idx - 3] : Rot.Z; }
	};


	//! Path in the space with 6 degrees of freedom
	class PosRotPath
	{
		std::vector<PosRot> WayPoint; //!< Set of way points forming the path
	public:
		PosRotPath() {}

		PosRot& operator[] (unsigned int idx);

		const PosRot& operator[] (unsigned int idx) const { return WayPoint[idx]; }

		int NumWayPoints() const { return (int)WayPoint.size(); }

		float Distance() const;

		void Reset();

		void Insert(const PosRot& p, unsigned int idx);

		void Remove(unsigned int idx);
	};


	///@}


	//-----------------------------------------------------------------
	// LINEAR SYSTEMS
	//-----------------------------------------------------------------

	///@name Linear Systems
	///@{

	//! Gaussian resolution of a linear system M x a = b (dimension=n)
	bool GaussResolve(float *M, float *a, float *b, unsigned int n = 4);

	///@}


	//-----------------------------------------------------------------
	// INTERSECTIONS
	//-----------------------------------------------------------------

	///@name Intersections
	///@{

	struct Ray3;

	/*!
	 Tests if a ray intersects a plane.
	 Returns distance to the intersection point or -1 if no intersection.
	 Intersection point is stored to "intersection" if it is not nullptr.
	 "planeNormal" and "rayDirection" must be normalized. (length=1)
	*/
	float RayPlaneIntersectionLength(Vec3 rayOrigin, Vec3 rayDirection,
		Vec3 planeOrigin, Vec3 planeNormal, Vec3* intersection = nullptr);

	//! Plane structure
	struct Plane3
	{

		Vec3 Origin;    //!< Origin of the plane

		Vec3 Normal;    //!< Normal of the plane (normalized)


		//! Default constructor
		Plane3() {}

		//! Constructor
		Plane3(Vec3 o, Vec3 n) : Origin(o), Normal(n) {}


		//! Set origin and normal
		void Set(Vec3 o, Vec3 n) { Origin = o; Normal = n; }

		//! Test if the given point is in front of this plane
		bool IsPointInFront(Vec3 p)
		{
			if (VecDotProduct(p - Origin, Normal) < 0) return false;
			return true;
		}

		//! Test if the given point is on this plane
		bool IsPointOnPlane(Vec3 p)
		{
			return (IsAlmostZero(VecDotProduct(p - Origin, Normal)));
		}

		//! Get the vector from the origin to the given point
		Vec3 GetVecFromOrigin(const Vec3& p) const
		{
			return p - Origin;
		}

		//! Compute the distance of the given point from this plane
		float Distance(const Vec3& p) const
		{
			// transform the point in local coordinates (p-Origin) and project it onto the normal (dot product)
			return GetVecFromOrigin(p).ProjectedOnVector(Normal).Length();
		}

		//! Project a point on this plane, return the distance vector (orthogonal to the plane)
		Vec3 DistanceVector(const Vec3& p)
		{
			/*!
			 calculate the distance vector as oriented as the normal
			 and with the distance as length
			*/
			return Normal*Distance(p);
		}

		//! Project a point (in world coordinates) on this plane, return the projected point (in world coordinates)
		Vec3 Project(const Vec3& p)
		{
			Vec3 po = p - Origin;
			// subtract the distance vector from the point and return it in world coordinates
			return po - Normal*po.Dot(Normal) + Origin;
		}
	};


	//! Ray structure
	struct Ray3
	{

		Vec3 Origin;    //!< Origin (starting point) of the ray
		Vec3 Direction; //!< Direction of the ray
		float Length;     //!< Optional length of the ray, if <0 it is ignored (by default is -1)

		//! Default constructor
		Ray3() {}

		//! Constructor
		Ray3(Vec3 o, Vec3 d) : Origin(o), Direction(d), Length(-1.0f) {}

		//! Set origin direction and (optionally) length
		void Set(Vec3 o, Vec3 d, float len = -2.0f) { Origin = o; Direction = d; if (len != -2.0f) Length = len; }

		/*!
		 Compute the intersection with a plane.
		 If the member Length is >=0 it is used to test if the ray "reaches" the plane
		 @param p plane to intersect
		 @param intersection pointer to the intersection point to be computed (if nullptr it will not be computed)
		 @return true in case of intersection
		*/
		bool Intersect(Plane3 p, Vec3* intersection)
		{
			if (!intersection) return false;
			Vec3 i(*intersection);
			float d = RayPlaneIntersectionLength(Origin, Direction, p.Origin, p.Normal, &i);
			if (d < 0) return false;
			*intersection = i;
			if (Length < 0) return true;
			return d <= Length;
		}
	};

	///@}

	//-----------------------------------------------------------------
	// Geometry
	//-----------------------------------------------------------------

	///@name Geometry
	///@{


	//! Triangle structure
	struct TriangleIndices
	{

		unsigned VertsIndices[3];     //!< Indices of vertices

		TriangleIndices()
		{
			VertsIndices[0] = VertsIndices[1] = VertsIndices[2] = 0;
		}

		TriangleIndices(unsigned i1, unsigned i2, unsigned i3)
		{
			VertsIndices[0] = i1;
			VertsIndices[1] = i2;
			VertsIndices[2] = i3;
		}

		TriangleIndices& operator =(const TriangleIndices& triIndices)
		{
			memcpy(this->VertsIndices, triIndices.VertsIndices, sizeof(TriangleIndices));
			return *this;
		}

		TriangleIndices& operator =(unsigned* idx)
		{
			memcpy(this, idx, 3 * sizeof(unsigned));
			return *this;
		}

		operator unsigned* () { return (unsigned*)this; }

		void Flip() { std::swap(VertsIndices[1], VertsIndices[2]); }
	};


	///@}

	/// @}
	/// @}
	/// @}

}
