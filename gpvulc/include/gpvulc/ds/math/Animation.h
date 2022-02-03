//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Animation classes and utilities.
/// @file Animation.h
/// @author Giovanni Paolo Vigano'



#pragma once


//#include <core/gvdefs.h>
#include "gpvulc/ds/math/mathutil.h"
#include "gpvulc/ds/math/Transform.h"

#include <gpvulc/ds/util/DynArray.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <set>


//*********************************************************************
// ANIMATION
//*********************************************************************


namespace gpvulc
{
	/// @defgroup Animation Animation
	/// Animation facility
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{

	/// @addtogroup Mathematics
	///@{ 

	/// @addtogroup Animation
	///@{ 

	//#define GVANIM_TIME_CONST_SPEED (-1.0)
	//#define GVANIM_TIME_UNIFORM (-2.0)

		//! Actions set after the end of animation
	enum AnimEndActionEnum
	{
		//! Stop the animation when passed the last key frame
		GVANIM_STOP = 0,
		//! Loop the animation when passed the last key frame
		GVANIM_RESTART,
		//! Loop and repeat the animation in reverse order for each odd cycle
		GVANIM_REVERT,
		//! If looping is disabled at the end of the animation return to the first frame (instead of the last one)
		GVANIM_REWIND,
		//! Stop and deactivate the animation when passed the last key frame
		GVANIM_DEACTIVATE
	};





	//!  Information about a single key frame in animation
	class AnimKeyFrame
	{
	public:

		//! Time at which this frame is assigned
		double Time;

		//! Bezier control point component for the curve, if 0 linear interpolation is used
		float BezierParam;

		//! Value related to this frame
		float Value;

		AnimKeyFrame(double time = 0.0, const float val = 0.0f, const float b = 0.0f)
		{
			Time = time; Value = val;
			BezierParam = b;
		}
	};



	struct AnimKeyFrameSet
	{
		double Time;
		const AnimKeyFrame* KeyFrame[9];
		AnimKeyFrameSet(double t = 0.0) : Time(t)
		{
			memset(KeyFrame, 0, 9 * sizeof(const AnimKeyFrame*));
		}

		AnimKeyFrameSet& operator =(const AnimKeyFrameSet& kfs)
		{
			Time = kfs.Time;
			memcpy(KeyFrame, kfs.KeyFrame, 9 * sizeof(const AnimKeyFrame*));
			return *this;
		}
	};

	//float gvAnimInterp_Linear( const float t, const float v1, const float v2 );

	//!  Animation track
	class AnimTrack
	{

	protected:

		float Interpolate(const float t, const AnimKeyFrame& f1, const AnimKeyFrame& f2) const;

		DynArray< AnimKeyFrame* > KeyFrames;

		/*!
		 return the keyframe at the given time if a keyframe exists at that time,
		 otherwise create the keyframe and return it
		*/
		AnimKeyFrame& GetKeyFrame(double time);

		/*!
		 Return the keyframe at the given time if a keyframe exists at that time,
		 otherwise return nullptr and set the pointers to the nearest keyframes
		*/
		AnimKeyFrame* GetKeyFrameAt(double time, AnimKeyFrame*& f1, AnimKeyFrame*& f2);

		double StartTime;  //!< Internal member storing the first time in the key frames list
		double EndTime;    //!< Internal member storing the last time in the key frames list
		double Duration;   //!< Internal member storing the difference between the last and the first time in the key frames list

		void UpdateInterval();

	public:


		//! What to do when the animation is ended (see AnimEndActionEnum)
		int ActionAfterEnd;

		//! Flag used externally to store if the animation is active or not (all the methods work anyway in the same way)
		mutable bool Active;

		//! Flag used externally to store if the animation is relative to the initial position (all the methods work anyway in the same way)
		bool Relative;

		//! Reverse the animation (time reverted, key frames order does not change)
		bool Reverse;

		//! Time delay added when calculating the value with GetValueAt(), it can also be negative (intially it is 0.0)
		double TimeDelay;

		AnimTrack();
		~AnimTrack();

		//! Check if the animation is defined (at least 1 keyframe)
		bool IsDefined() const { return KeyFrames.Size() > 0; }//&& Duration>DS_EPSILONd; }

		//! Check if the animation is defined and active
		bool IsActive() const { return Active && IsDefined(); }

		//! Check if the animation is repeating
		bool IsRepeating() const { return (ActionAfterEnd == GVANIM_RESTART || ActionAfterEnd == GVANIM_REVERT); }

		//! Copy the given keyframe to the one at the corresponding time or to a new one if not present
		void SetKeyFrame(const AnimKeyFrame& kf) { GetKeyFrame(kf.Time) = kf; }

		//! Copy the given keyframe value to the one at the corresponding time or to a new one if not present
		void SetKeyFrameValue(double time, float val, float b = 0.0f)
		{
			AnimKeyFrame& kf = GetKeyFrame(time);
			kf.Value = val;
			kf.BezierParam = b;
		}

		//! Delete the keyframe at the givewn time
		bool DeleteKeyFrame(double time);

		/*!
		 Change the time of a keyframe, keep the time order
		 @return false if no keyframe is found at the given time or if there is an existing keyframe at the given new time.
		*/
		bool ChangeKeyFrameTime(double time, double new_time);

		//! Reset animation parameters and keyframes
		void Reset();

		//! Reset animation parameters
		void ResetParams();

		//! Reset animation keyframes
		void ResetKeyFrames();

		bool GetValueAt(double time, float& val) const;
		unsigned NumKeyFrames() const { return KeyFrames.Size(); }
		const AnimKeyFrame* GetKeyFrameByIndex(unsigned i) const { return i < NumKeyFrames() ? KeyFrames[i] : 0; }
		const AnimKeyFrame* GetKeyFrameByTime(double t) const;

		double GetStartTime() const { return StartTime; }
		double GetDuration() const { return Duration; }
		double GetEndTime() const { return EndTime; }

		AnimTrack& operator =(const AnimTrack& track);
	};


	//! Vector animation, this class aggregates 3 animation tracks (AnimTrack)
	class VecAnimation
	{

	public:
		AnimTrack X;
		AnimTrack Y;
		AnimTrack Z;

		VecAnimation() {}

		/// Set flags for all the 3 tracks
		///@{

		void SetTimeDelay(double delay)
		{
			X.TimeDelay = Y.TimeDelay = Z.TimeDelay = delay;
		}

		bool SetActionAfterEnd(int action)
		{
			bool changed = X.ActionAfterEnd != action || Y.ActionAfterEnd != action || Z.ActionAfterEnd != action;
			X.ActionAfterEnd = Y.ActionAfterEnd = Z.ActionAfterEnd = action;
			return changed;
		}

		bool SetRelative(bool on)
		{
			bool changed = X.Relative != on || Y.Relative != on || Z.Relative != on;
			X.Relative = Y.Relative = Z.Relative = on;
			return changed;
		}

		bool SetReverse(bool on)
		{
			bool changed = X.Reverse != on || Y.Reverse != on || Z.Reverse != on;
			X.Reverse = Y.Reverse = Z.Reverse = on;
			return changed;
		}

		/*!
		 Set the animation as active (true) or not active (false)
		 @return true if the state (see IsActive()) was changed
		*/
		bool SetActive(bool on, unsigned usemask = DS_USEXYZ)
		{
			bool was_active = IsActive();
			if (usemask&DS_USEX) X.Active = on;
			if (usemask&DS_USEY) Y.Active = on;
			if (usemask&DS_USEZ) Z.Active = on;
			return was_active != IsActive();
		}

		///@}

		//! Check if the animation is relative
		bool IsRelative() const { return X.Relative || Y.Relative || Z.Relative; }

		//! Check if the animation is defined
		bool IsDefined() const { return X.IsDefined() || Y.IsDefined() || Z.IsDefined(); }

		//! Check if the animation is defined and active
		bool IsActive() const { return X.IsActive() || Y.IsActive() || Z.IsActive(); }

		//! Check if the animation is repeating
		bool IsRepeating() const { return X.IsRepeating() || Y.IsRepeating() || Z.IsRepeating(); }

		//! Reset animation parameters and keyframes
		void Reset() { X.Reset(); Y.Reset(); Z.Reset(); }

		//! Reset animation parameters
		void ResetParams() { X.ResetParams(); Y.ResetParams(); Z.ResetParams(); }

		//! Reset animation keyframes
		void ResetKeyFrames() { X.ResetKeyFrames(); Y.ResetKeyFrames(); Z.ResetKeyFrames(); }

		//! Store in the given set all the keyframe times
		int CollectKeyFrameTimes(std::set<double>& kf_time) const;

		/*!
		 Store in the given vectors the keyframes and a set of sampled points. Please note that:
		 @li the given vectors are not cleared
		 @li the first parameter is mandatory (it can be nullptr), the others are optional
		 @li you can pass nullptr to each parameter you are not interested in (including the first one)
		 @param keyframes (output) list of keyframe values
		 @param kf_coords (output) list of coordinates usage (or-ed bitmask of DS_USEX, DS_USEY, DS_USEZ)
		 @param kf_time (output) list of time for each keyframe
		 @param points (output) list of points along the path sampled with the given resolution
		 @param samples (input) number of samples used to build the list of points along the path
		 @param tan_vec (output) list of tangent vectors defined for each frame (if not defined they are zero-length vectors)
		 @return false if no animation is defined
		*/
		bool CreatePath(
			std::vector<Vec3>* keyframes,
			std::vector<unsigned>* kf_coords = nullptr,
			std::vector<double>* kf_time = nullptr,
			std::vector<Vec3>* points = nullptr,
			unsigned samples = 50,
			std::vector<Vec3>* tan_vec = nullptr
			) const;

		//! Set the vector value at the given time
		void SetValueAt(double time, const Vec3& vec, unsigned usemask = DS_USEXYZ, const Vec3& b = Vec3())
		{
			if ((usemask&DS_USEX) != 0) X.SetKeyFrameValue(time, vec.X, b.X);
			if ((usemask&DS_USEY) != 0) Y.SetKeyFrameValue(time, vec.Y, b.Y);
			if ((usemask&DS_USEZ) != 0) Z.SetKeyFrameValue(time, vec.Z, b.Z);
		}

		//! Calculate the vector at the given time
		bool GetValueAt(double time, Vec3& vec, unsigned usemask = DS_USEXYZ, unsigned* usedmask = nullptr) const;

		//! Get the combined start time
		double GetStartTime() const
		{
			double t = std::numeric_limits<double>::max();
			if (X.IsDefined()) t = std::min(t, X.GetStartTime());
			if (Y.IsDefined()) t = std::min(t, Y.GetStartTime());
			if (Z.IsDefined()) t = std::min(t, Z.GetStartTime());
			return t;
		}

		//! Get the combined duration
		double GetDuration() const
		{
			return GetEndTime() - GetStartTime();
		}

		//! Get the combined end time
		double GetEndTime() const
		{
			double t = -1.0;
			if (X.IsDefined()) t = std::max(t, X.GetEndTime());
			if (Y.IsDefined()) t = std::max(t, Y.GetEndTime());
			if (Z.IsDefined()) t = std::max(t, Z.GetEndTime());
			return t;
		}
	};


	//! Transform animation, this class aggregates 3 vector animations (VecAnimation)
	class TransformAnimation
	{
	public:
		VecAnimation Position;  //!< Position track
		VecAnimation Rotation;  //!< Rotation track
		VecAnimation Scaling;   //!< Scaling track

		TransformAnimation() {}

		void SetTimeDelay(double delay)
		{
			Position.SetTimeDelay(delay);
			Rotation.SetTimeDelay(delay);
			Scaling.SetTimeDelay(delay);
		}

		//! Check if the animation is defined
		bool IsDefined() const { return Position.IsDefined() || Rotation.IsDefined() || Scaling.IsDefined(); }

		//! Check if the animation is defined and active
		bool IsActive() const { return Position.IsActive() || Rotation.IsActive() || Scaling.IsActive(); }

		//! Check if the animation is repeated
		bool IsRepeating() const { return Position.IsRepeating() || Rotation.IsRepeating() || Scaling.IsRepeating(); }

		//! Set the Position vector value at the given time
		void SetPosition(double time, const Vec3& pos, unsigned usemask = DS_USEXYZ, const Vec3& b = Vec3())
		{
			Position.SetValueAt(time, pos, usemask, b);
		}

		//! Set the Rotation vector value at the given time
		void SetRotation(double time, const Vec3& pos, unsigned usemask = DS_USEXYZ, const Vec3& b = Vec3())
		{
			Rotation.SetValueAt(time, pos, usemask, b);
		}

		//! Set the Scaling vector value at the given time
		void SetScaling(double time, const Vec3& pos, unsigned usemask = DS_USEXYZ, const Vec3& b = Vec3())
		{
			Scaling.SetValueAt(time, pos, usemask, b);
		}

		//! Calculate the transformation at the given time
		bool GetTransformAt(double time, Transformation& tr) const;

		//! Get the combined start time
		double GetStartTime() const
		{
			double t = std::numeric_limits<double>::max();
			if (Position.IsDefined()) t = std::min(t, Position.GetStartTime());
			if (Rotation.IsDefined()) t = std::min(t, Rotation.GetStartTime());
			if (Scaling.IsDefined()) t = std::min(t, Scaling.GetStartTime());
			return t;
		}

		//! Get the combined duration
		double GetDuration() const { return GetEndTime() - GetStartTime(); }

		//! Get the combined end time
		double GetEndTime() const
		{
			double t = -1.0;
			if (Position.IsDefined()) t = std::max(t, Position.GetEndTime());
			if (Rotation.IsDefined()) t = std::max(t, Rotation.GetEndTime());
			if (Scaling.IsDefined()) t = std::max(t, Scaling.GetEndTime());
			return t;
		}

		//! Reset animation parameters and keyframes
		void Reset() { Position.Reset(); Rotation.Reset(); Scaling.Reset(); }

		//! Reset animation parameters
		void ResetParams() { Position.ResetParams(); Rotation.ResetParams(); Scaling.ResetParams(); }

		//! Reset animation keyframes
		void ResetKeyFrames() { Position.ResetKeyFrames(); Rotation.ResetKeyFrames(); Scaling.ResetKeyFrames(); }

		//! Store in the given set all the keyframe times.
		int CollectKeyFrameTimes(std::set<double>& kf_time) const;

		//! Store in the given vector all the keyframes.
		int CollectKeyFrames(std::vector< AnimKeyFrameSet >& keyframe_set) const;
	};


	/*!
	 @brief Time controller for playing animations
	 @details Object used to control the time used as parameter for animation,
	 given the real time it computes the correct animation time.
	*/
	class AnimTimeController
	{

		bool mStarted;
		bool mPaused;
		double mTimeStart;
		double mTimePause;
		double mTimeScaling;
		double mTimeOffset;

	public:
		AnimTimeController();

		//! Start the animation
		void Start(double real_time);

		//! Get the start time
		double GetStartTime() { return mTimeStart; }

		//! Set a delay (change the start time of the animation), use negative values to move forward in time
		void Delay(double time_delay);

		//! Stop the animation
		void Stop();

		//! Restart the animation
		void Restart(double real_time) { Stop(); Start(real_time); }

		//! Pause the animation
		void Pause(double real_time);

		//! Check if the animation is paused
		bool Paused()
		{
			return mPaused;
		}

		//! Get the pause time
		double GetPauseTime() { return mTimePause; }

		//! Check if the animation is started
		bool Started()
		{
			return mStarted;
		}

		//! Check if the animation is started and not paused
		bool Running()
		{
			return mStarted && !mPaused;
		}

		//! Get the computed time
		double Time(double real_time);

		//! Scale the time to the given factor (the offset is automatically adjusated to match the same output time)
		void Scale(double real_time, double scale);

		//! Get current time scaling
		double GetScaling() { return mTimeScaling; }

		//! Shift the time by the given factor
		void ShiftTime(double time_shift);

		//! Get the current time shift
		double TimeShift() { return mTimeOffset; }
	};


	//@} (Animation)
	//@} (Mathematics)
	//@} (DS)

} // namespace gpvulc
