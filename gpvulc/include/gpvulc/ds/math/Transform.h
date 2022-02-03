//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Transformation with position and orientation and scaling
/// @file Transform.h
/// @author Giovanni Paolo Vigano'


#pragma once


#include "gpvulc/ds/math/Location.h"
#include "gpvulc/ds/math/Constraints.h"

namespace gpvulc
{

	/// @defgroup Transformation Transformation
	/// @brief  Transformation with position and orientation and scaling
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{

	/// @addtogroup Mathematics
	/// @{

	/// @addtogroup Transformation
	/// @{


	//! Transformation: location (Location) with scaling
	class Transformation : public Location
	{

		void Init();
	protected:
		mutable Vec3 Scaling;  //!< scaling vector
		mutable bool ComputeSclFromMatrix; //!< scaling must be calculated from the matrix


		void UpdateMatrix3() const; //!< calculate matrix rotation submatrix from the rotation
		void UpdateFromMatrix3() const;  //!< calculate rotation and scaling from the matrix

	public:
		//TransformConstraints Constraints;
		Transformation();

		//! Reset this transform to initial values
		virtual void Reset();

		//! Set the scaling in 3D
		void SetScaling(const Vec3& scl);

		//! Get the scaling in this location
		Vec3 GetScaling() const;

		//! Set rotation matrix preserving scaling according to the given rotation matrix (in world coordinates)
		void SetRotationMatrix(const Mat4& rot);

		//! Set this location according to the given matrix, scaling is managed
		bool SetMatrix(const Mat4& m);

		//! Assignment operator: copy the location matrix and the Euler angles if defined
		Transformation& operator =(const Transformation& tr);

		/// @name Comparison operators
		//@{

		bool operator ==(const Transformation& t) const;
		bool operator !=(const Transformation& t) const { return !(*this == t); }

		//@}
	};


	//! Transformation with pivot coordinate system
	class PivotTransform
	{

	protected:
		mutable Mat4 FinalMatrix;

		mutable bool Valid;
		mutable bool ValidEulerAngles;

		mutable Mat4 PivotCoordSys;
		mutable Mat4 InvPivotCoordSys;
		mutable bool EnablePivotCoordSys;

		mutable Vec3 Translation;
		mutable bool EnableTranslation;
		mutable Vec3 Rotation;
		mutable Mat4 RotMatrix;
		mutable bool EnableRotation;

		mutable Vec3 Scaling;
		mutable bool EnableScaling;

		mutable Mat4 TransformMatrix;
		mutable bool EnableTransform;

		void _ComputeMatrix() const;
		void _CheckRotMatrix();
		void _UpdateRotation() const;
		void _Init();
		TransformConstraints* Constraints;

	public:

		/*!
		 This flag is set to true every time the transformation is updated.
		 To detect changes set it to false before calling methods, then test if it is set to true.
		*/
		bool Changed;

		/// @name Constructors
	  //@{

		PivotTransform(int i = 0); ///< Call _Init() (int parameter is to allow casting to 0)

	  //@}

		//! Completely reset the transformation (Constraints is set to null, but not deleted)
		void Reset();

		//! Check if the transformation is active
		bool IsActive() const
		{
			GetMatrix();
			return PivotEnabled() || PivotCoordSysEnabled() || !FinalMatrix.IsIdentity() || (Constraints && Constraints->Active());
		}

		///@name Set/get methods
		/// As you set a transformation to its default (neutral) value
		/// the relative calculation is disabled.
		/// Note that Set methods return true only if the value has been changed
	  //@{

		//! Get the pivot point position
		Vec3 GetPivot() const { return PivotCoordSys.GetCol(3); }

		//! Set the pivot point position
		bool SetPivot(Vec3 piv);

		//! Check if the pivot is enabled
		bool PivotEnabled() const { return !PivotCoordSys.GetCol(3).IsZero(); }

		//! Set the pivot coordinate system matrix
		bool SetPivotMatrix(Mat4 mat);

		//! Get the pivot coordinate system matrix
		Mat4 GetPivotMatrix() const { return PivotCoordSys; }

		//! Get the pivot coordinate system inverse matrix
		Mat4 GetPivotInverseMatrix() const { return InvPivotCoordSys; }

		//! Get the pivot coordinate system axes
		void GetPivotCoordSys(Vec3& xaxis, Vec3& axis, Vec3& zaxis) const;

		//! Set the pivot coordinate system axes
		bool SetPivotCoordSys(Vec3 xaxis, Vec3 axis, Vec3 zaxis);

		//! Check if the pivot coordinate system is enabled
		bool PivotCoordSysEnabled() const { return EnablePivotCoordSys; }


		//! Set the matrix without pivot coordinate system
		bool SetTransformMatrix(Mat4 mat);

		//! Set the current transformation matrix and extract translation, rotation, scaling (reset pivot data).
		void SetMatrix(Mat4 m) { SetPivotMatrix(Mat4()); SetTransformMatrix(m); }

		//! Get the matrix without pivot coordinate system, calculated if needed
		Mat4 GetTransformMatrix() const;


		//! Return the translation
		Vec3 GetTranslation() const { return Translation; }

		//! Set the translation [0,0,0]=disabled
		bool SetTranslation(Vec3 tr, unsigned changemask = DS_USEXYZ);

		//! Check if the translation is enabled
		bool TranslationEnabled() const { return EnableTranslation; }


		//! Return the rotation (Euler angles)
		Vec3 GetRotation() const { _UpdateRotation(); return Rotation; }

		//! Return the (pure) rotation matrix
		Mat4 GetRotationMatrix() const { return RotMatrix; }

		//! Return the quaternion related to the rotation matrix
		Quat GetRotationQuat() const
		{
			if (ValidEulerAngles)
			{
				Quat q;
				q.SetEulerAngles(Rotation);
				return q;
			}

			return QuatFromMatrix((Mat4)RotMatrix);
		}

		//! Return the axis and angle related to the rotation matrix (axis=(X,Y,Z) angle=W)
		Vec4 GetRotationAxisAngle() const { return MatGetAxisAngle(RotMatrix); }

		//! Set the rotation (Euler angles) [0,0,0]=disabled
		bool SetRotation(Vec3 rot, unsigned changemask = DS_USEXYZ);

		//! Set the rotation matrix (identity=disabled)
		bool SetRotationMatrix(Mat4 rotmat);

		//! Set the rotation quaternion (vector 0=disabled)
		bool SetRotationQuat(Quat q) { return SetRotationMatrix(q.GetMatrix()); }

		/*!
		 Set the rotation as reference system (set the 3x3 submatrix columns to the given vectors).
		 Note that constraints are ignored and the reference system is with Y up and Z out.
		*/
		bool SetReferenceSystem(Vec3 xAxis, Vec3 yAxis, Vec3 zAxis);

		/*!
		 Get the rotation as reference system (get the 3x3 submatrix columns in the given vectors).
		 Note that the reference system is with Y up and Z out, if a pointer is nullptr it is ignored.
		*/
		void GetReferenceSystem(Vec3* xAxis, Vec3* yAxis, Vec3* zAxis);

		//! Set the rotation (angle around an axis) [0,[0,0,0]]=disabled
		bool SetRotation(float angle, Vec3 axis);

		//! Check if the rotation is enabled
		bool RotationEnabled() const { return EnableRotation; }


		//! Get the (x,y,z) scaling
		Vec3 GetScaling() const { return Scaling; }

		//! Set the (x,y,z) scaling [1,1,1]=disabled
		bool SetScaling(Vec3 sc, unsigned changemask = DS_USEXYZ);

		//! Check if the scaling is enabled
		bool ScalingEnabled() const { return EnableScaling; }


		//! Check if translation/rotation/scaling is enabled
		bool TransformEnabled() const { return EnableTransform; }


		//! Get the constraints pointer (nullptr if not constrained)
		TransformConstraints* GetConstraints() const { return Constraints; }

		//! Set the constraints (nullptr = not constrained), the given pointer is directly used by this object (it will not be deleted with the object)
		TransformConstraints* SetConstraints(TransformConstraints* c) { Constraints = c; return Constraints; }

		/*!
		 Get the current transformation matrix.
		 This matrix is computed as follows:<BR>
		 <BR>
		 M = final matrix<BR>
		 P = Pivot coordinate system matrix<BR>
		 Pi = Inverted pivot coordinate system matrix<BR>
		 S = Scaling matrix<BR>
		 T = Translation matrix<BR>
		 R = Rotation matrix<BR>
		 <BR>
		 M = Pi * S * R * T * P
		*/
		Mat4 GetMatrix() const;

		//@}

		  //! Assignment operator
		PivotTransform& operator =(const PivotTransform& t);

		//! Assignment operator from a Transformation
		PivotTransform& operator =(const Transformation& t);

		/// @name Comparison operators
		//@{

		bool operator ==(const PivotTransform& t) const;
		bool operator !=(const PivotTransform& t) const { return !(*this == t); }

		//@}

	  /*!
	  PivotTransform operator *(const PivotTransform t) {
	    return PivotTransform();
	  }
	*/
	};

	///@}
	///@}
	///@}

} // namespace gpvulc
