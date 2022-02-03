//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Movable location with position and orientation
/// @file Location.h
/// @author Giovanni Paolo Vigano'


#pragma once


#include "gpvulc/ds/math/mathutil.h"

namespace gpvulc
{
	/// @defgroup Location Location
	/// @brief Movable location with position and orientation
	/// @author Giovanni Paolo Vigano'


	/// @addtogroup DS
	/// @{

	/// @addtogroup Mathematics
	/// @{

	/// @addtogroup Location
	/// @{



	enum
	{
		DS_MAT_CHANGE = DS_MAT,
		DS_POS_CHANGE = DS_MAT_CHANGE | DS_POS,
		DS_ROT_CHANGE = DS_MAT_CHANGE | DS_ROT,
		DS_SCL_CHANGE = DS_MAT_CHANGE | DS_SCALE,
	};

	//! Movable location with position and orientation
	class Location
	{

		// Set to true to avoid calling OnChange() during composite operations
		bool _DisableChangesNotification;
		void Init();

	protected:

		mutable Vec3 Position; //!< position vector
		mutable Quat Rotation; //!< rotation expressed as quaternion
		mutable Vec3 EulerAngles;  //!< Euler angles internally stored until the Matrix or the Rotation quaternion is changed
		mutable Mat4 Matrix;   //!< resulting transformation matrix
		mutable Mat4 InverseMatrix; //!< inverse transformation matrix
		mutable bool ComputePos; //!< position must be calculated from the matrix
		mutable bool ComputeRotFromMatrix; //!< rotation must be calculated from the matrix
		mutable bool ComputeSubMatrixPos; //!< matrix last column must be calculated from the position
		mutable bool ComputeSubMatrix3; //!< matrix 3x3 submatrix must be calculated from the rotation (and scaling)
		mutable bool ComputeInverseMatrix; //!< inverse matrix must be calculated from the matrix
		mutable bool ComputeFromEulerAngles; //!< Rotation quaternion must be calculated from the Euler angles

		/*!
		 Virtual method to be overridden in event of changes
		 @param what property changed, a bitmask composed by DS_POS_CHANGE, DS_ROT_CHANGE, DS_SCL_CHANGE, DS_MAT_CHANGE
		*/
		virtual void OnChange(unsigned what) {}
		void NotifyChange(unsigned what)
		{
			// avoid looping OnChange()->method()->OnChange()-->...
			if (!_DisableChangesNotification)
			{
				_DisableChangesNotification = true;
				OnChange(what);
				_DisableChangesNotification = false;
			}
		}

		void UpdatePositionFromMatrix() const;  //!< calculate the position from the matrix
		virtual void UpdateFromMatrix3() const;  //!< calculate the rotation from the matrix
		void UpdateMatrixPos() const; //!< calculate matrix translation column from the position
		virtual void UpdateMatrix3() const;   //!< calculate first 3 column of the matrix from the rotation
		void UpdateInvMatrix() const; //!< calculate the inverse matrix from the matrix

	public:

		Location();

		//! Reset this location to initial values
		virtual void Reset();

		//! Move the position to the given vector (in world coordinates)
		void SetPosition(const Vec3& pos);

		//! Rotate around the X,Y,Z axis (in world coordinates) by the given angle (degrees)
		void SetRotationEulerAngles(const Vec3& rot);

		//! Rotate around the given axis (in world coordinates) by the given angle (degrees)
		void SetRotationAxisAngle(const Vec3& axis, float angle);

		//! Rotate according to the given quaternion (in world coordinate system)
		void SetRotationQuaternion(const Quat& rot);

		//! Rotate according to the given rotation matrix (in world coordinate system)
		virtual void SetRotationMatrix(const Mat4& rot);

		//! Set this location according to the given matrix (it must not include scaling)
		virtual bool SetMatrix(const Mat4& m);

		//! Get the position of this location (world coordinate system)
		Vec3 GetPosition() const;

		//! Rotation quaternion is calculated from the Euler angles originally set
		bool EulerAnglesAvailable() const { return ComputeFromEulerAngles; }

		//! Get the rotation around X,Y,Z axes (world coordinate system)
		Vec3 GetRotationEulerAngles() const;

		//! Get the rotation axis and angle of this location (world coordinate system)
		void GetRotationAxisAngle(Vec3& axis, float& angle) const;

		//! Get the quaternion equivalent to the rotation of this location (world coordinate system)
		Quat GetRotationQuaternion() const;

		//! Get the matrix equivalent to the rotation of this location (world coordinate system)
		Mat4 GetRotationMatrix() const;

		//! Get the matrix related to this location, calculated if needed
		Mat4 GetMatrix() const;

		//! Get the inverse matrix related to this location, calculted if needed
		Mat4 GetInverseMatrix() const;

		//! Move the position by the given vector (in world coordinates)
		void MoveBy(const Vec3& pos);

		//! Rotate around an axis
		void RotateBy(float angle, const Vec3& axis);

		//! Slide along local axes
		void Slide(const Vec3& offset);

		//! Get the absolute offset if sliding along local axes
		Vec3 GetSlide(const Vec3& offset);

		//! Spin around a local axis
		void Spin(float angle, const Vec3& axis);

		//! Orbit over a point around a local axis
		void Orbit(const Vec3& pnt, float angle, const Vec3& axis);

		//! Assignment operator: copy the location matrix and the Euler angles if defined
		Location& operator =(const Location& loc);

		//! Equivalence operator: compare the location matrices
		bool operator ==(const Location& loc)
		{
			return GetMatrix() == loc.GetMatrix();
		}

		//! Negated equivalence operator: compare the location matrices
		bool operator !=(const Location& loc)
		{
			return !(*this == loc);
		}

		/// Add/Subtract operator: add/subtract the given vector to the location position
		/// @name Arithmetic operators
		///@{

		Location& operator +=(const Vec3& pos)
		{
			SetPosition(GetPosition() + pos);
			return *this;
		}
		Location& operator -=(const Vec3& pos)
		{
			return (*this -= pos);
		}
		Location operator +(const Vec3& pos) const
		{
			Location loc(*this);
			loc += pos;
			return loc;
		}
		Location operator -(const Vec3& pos) const
		{
			return *this + (-pos);
		}

		///@}

		/*!
		 @name Composition operators
		@{
		 Compose this location with the given location
		*/
		Location& operator *=(const Location& loc)
		{
			SetMatrix(GetMatrix()*loc.GetMatrix());
			return *this;
		}
		//! Calculate the location resulting from composing this location with the given location
		Location operator *(const Location& loc)
		{
			Location tmploc(*this);
			tmploc *= loc;
			return tmploc;
		}
		//! Compose this location with the inverse of the given location
		Location& operator /=(const Location& loc)
		{
			SetMatrix(GetMatrix()*loc.GetInverseMatrix());
			return *this;
		}
		//! Calculate the location resulting from composing this location with the inverse of the given location
		Location operator /(const Location& loc)
		{
			Location tmploc(*this);
			tmploc /= loc;
			return tmploc;
		}

		///@}

	};

	///@}
	///@}
	///@}

}
