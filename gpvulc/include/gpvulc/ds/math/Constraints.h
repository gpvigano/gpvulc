//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Constraints for vectors, location and transformations
/// @file Constraints.h
/// @author Giovanni Paolo Vigano'


#pragma once


#include "gpvulc/ds/math/Location.h"

namespace gpvulc
{
	/// @defgroup Constraints Constraints
	/// @brief Constraints for vectors, location and transformations
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{

	/// @addtogroup Mathematics
	/// @{

	/// @addtogroup Constraints
	/// @{

	//! Limit for a single float
	struct ValueLimit
	{
		bool Enabled;
		float Value;
		ValueLimit() { Reset(); }
		void Reset() { Enabled = false; Value = 0.0f; }
		operator float() { return Value; }
		void Set(bool en, float val = 0.0f) { Enabled = en; if (Enabled) Value = val; }
		bool operator ==(const ValueLimit& lim) const
		{
			return lim.Enabled == Enabled && lim.Value == Value;
		}
	};

	//! Constraint for a single float
	struct Constraint
	{
		bool Active;
		bool Locked;
		bool Loop;
		ValueLimit Min;
		ValueLimit Max;
		ValueLimit Snap;
		Constraint() { Reset(); }
		void Reset() { Active = false; Locked = false; Loop = false; Min.Reset(); Max.Reset(); Snap.Reset(); }
		float Center() { return (Max.Value + Min.Value) / 2.0f; }
		bool Apply(float& val, float new_val)
		{
			if (!Active)
			{
				val = new_val;
				return true;
			}
			if (Locked)
			{
				return false;
			}
			if (Snap.Enabled) new_val = gvSnap(new_val, (float)Snap);
			if (Min.Enabled && Max.Enabled && Loop && Min < Max)
			{
				float range = Max - Min;
				while (new_val > Max) new_val -= range;
				while (new_val < Min) new_val += range;
			}
			if (Min.Enabled && new_val < Min) new_val = Min;
			if (Max.Enabled && new_val > Max) new_val = Max;
			val = new_val;
			return true;
		}
		void SetRange(float min, float max, bool loop, bool en) { Max.Enabled = Min.Enabled = en; Min.Value = min; Max.Value = max; Loop = loop; }

		bool operator ==(const Constraint& c) const
		{
			return c.Active == Active && c.Locked == Locked && c.Loop == Loop &&
				c.Min == Min && c.Max == Max && c.Snap == Snap;
		}
	};

	//! Constraint for a 3-dimensional vector
	struct Vec3Constraint
	{
		Constraint X;
		Constraint Y;
		Constraint Z;
		operator Constraint*() { return (Constraint*)this; }
		bool Active() { return X.Active || Y.Active || Z.Active; }
		bool Apply(Vec3& val, const Vec3& new_val)
		{
			bool result = false;
			if (X.Apply(val.X, new_val.X)) result = true;
			if (Y.Apply(val.Y, new_val.Y)) result = true;
			if (Z.Apply(val.Z, new_val.Z)) result = true;
			return result;
		}

		//! Check if any constraint is active an locked
		bool SomethingLocked() { return (X.Active && X.Locked) || (Y.Active && Y.Locked) || (Y.Active && Y.Locked); }

		bool operator ==(const Vec3Constraint& c) const
		{
			return c.X == X && c.Y == Y && c.Z == Z;
		}
	};


	//! Constraint for a location (position+rotation)
	class LocationConstraints
	{

	public:

		/// @name Constraints data
		/// Use Set... methods to easier define this data.
		/// @see Vec3Constraint
		//@{

		Vec3Constraint Translation; //!< Translation constraint
		Vec3Constraint Rotation;    ///< Default constructor

		//@}

		LocationConstraints(); //!< Default constructor

		/// @name Constraints definition methods
		/// The "which" parameters in these methods is a bitmask build OR-ing the constraint types (see DS_POS,DS_ROT,...).
		///
		/// @arg @c DS_POS_X  DOF position X coord
		/// @arg @c DS_POS_Y  DOF position Y coord
		/// @arg @c DS_POS_Z  DOF position Z coord
		/// @arg @c DS_POS   DOF position X,Y,Z
		/// @arg @c DS_ROT_X  DOF rotation X coord
		/// @arg @c DS_ROT_Y  DOF rotation Y coord
		/// @arg @c DS_ROT_Z  DOF rotation Z coord
		/// @arg @c DS_ROT   DOF rotation X,Y,Z
		/// @arg @c DS_SCALE_X   scaling X coord
		/// @arg @c DS_SCALE_Y   scaling Y coord
		/// @arg @c DS_SCALE_Z   scaling Z coord
		/// @arg @c DS_SCALE    scaling X,Y,Z
		//@{

		//! Enable/disable the given constraint: constrained=false=disabled, constrained=true=enabled (default)
		void SetDofActive(int which, bool constrained = true);

		//! Enable the given constraint
		void EnableDof(int which) { SetDofActive(which, true); }

		//! Disable the given constraint
		void DisableDof(int which) { SetDofActive(which, false); }

		//! Enable the given constraint and lock the DOF (default) or unlock it if false is passed as second argument
		void SetDofLocked(int which, bool locked = true);

		//! Set the maximum limit for the given constraint, if the optional paramenter is true (default) enable the constraint
		void SetDofMax(int which, float val, bool enabled = true);

		//! Set the minimum limit for the given constraint, if the optional paramenter is true (default) enable the constraint
		void SetDofMin(int which, float val, bool enabled = true);

		/*!
		 Set the limits (min,max) for the given constraint,
		 if the "loop" optional paramenter is true (default=false) values outside the given range are shifted to fit in the range.
		 If the last optional paramenter ("enabled") is true (default) enable the constraint.
		*/
		void SetDofRange(int which, float min, float max, bool loop = false, bool enabled = true);

		/*!
		 Set the snap value for the given constraint (values will be rounded to a multiple of it).
		 If the last optional paramenter ("enabled") is true (default) enable the constraint.
		*/
		void SetDofSnap(int which, float val, bool enabled = true);

		//! Restore the given constraint to its initial state (disabled, all zero/false)
		void ResetDof(int which);

		//@}

		/// @name Constraints application methods
		/// The first parameter is the input/output data
		/// (the original value in input and the result in output),
		/// the second one is the target final value.
		/// The first parameter is modified by the selected operation with the defined constraints (if not locked).
		/// The first parameter is passed as input and output, affected by changes (if any).
		//@{

		bool SetTranslation(Vec3& trs, const Vec3& newtrs)
		{
			return Translation.Apply(trs, newtrs);
		}
		bool SetRotation(Vec3& rot, const Vec3& newrot)
		{
			return Rotation.Apply(rot, newrot);
		}

		//@}

		//! Check if any constraint is active
		bool Active() { return Translation.Active() || Rotation.Active(); }

		/// Comparison operators
		//@{

		bool operator ==(const LocationConstraints& c) const
		{
			return c.Translation == Translation && c.Rotation == Rotation;
		}
		bool operator !=(const LocationConstraints& c) const
		{
			return !(c == *this);
		}

		//@}
	};

	//! Constraint for a complete transformation (position+rotation+scaling)
	class TransformConstraints : public LocationConstraints
	{

	public:

		/// @name Constraints data
		/// Use Set... methods to easier define this data.
		/// @see Vec3Constraint
		//@{

		Vec3Constraint Scaling;     ///< Default constructor

		//@}

		TransformConstraints(); //!< Default constructor

		/// @name Constraints definition methods extended for Scaling (see LocationConstraints)
		//@{

		void SetDofActive(int which, bool constrained = true);
		void SetDofLocked(int which, bool locked = true);
		void SetDofMax(int which, float val, bool enabled = true);
		void SetDofMin(int which, float val, bool enabled = true);
		void SetDofRange(int which, float min, float max, bool loop = false, bool enabled = true);
		void SetDofSnap(int which, float val, bool enabled = true);
		void ResetDof(int which);

		//@}

		/// @name Constraints application methods
		/// The first parameter is the input data (the original value), the second is the output after the selected operation with the defined constraints is applied.
		/// The same variable can be passed to the input and the output data, in that case changes (if any) will affect the given variable.
		//@{

		bool SetScaling(Vec3& scl, const Vec3& newscl)
		{
			return Scaling.Apply(scl, newscl);
		}

		//@}

		//! Check if any constraint is active
		bool Active() { return Translation.Active() || Rotation.Active() || Scaling.Active(); }

		/// Comparison operators
		//@{

		bool operator ==(const TransformConstraints& c) const
		{
			return c.Translation == Translation && c.Rotation == Rotation
				&& c.Scaling == Scaling;
		}
		bool operator !=(const TransformConstraints& c) const
		{
			return !(*this == c);
		}

		//@}
	};


	///@}
	///@}
	///@}

}
