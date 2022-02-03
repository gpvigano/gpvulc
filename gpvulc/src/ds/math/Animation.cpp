//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Animation classes and utilities.

#include <gpvulc/ds/math/Animation.h>
//#include <util/preproc.h>
#include <vector>
#include <set>

namespace gpvulc
{

	//---------------------------------------------------------------------
	// AnimTrack


	AnimTrack::AnimTrack()
	{
		ResetParams();
	}


	AnimTrack::~AnimTrack()
	{
		ResetKeyFrames();
	}


	AnimKeyFrame& AnimTrack::GetKeyFrame(double time)
	{
		int cur = 0;
		for (int i = 0; i < KeyFrames.Size(); i++)
		{

			// keyframe already existing:
			if (KeyFrames[i]->Time == time)
				return *(KeyFrames[i]);

			// keyframe not existing...

			// if the key frame is after the given time
			// (if it is before a new key frame must be added, see below)
			if (KeyFrames[i]->Time > time)
			{

				// if there is a previous key frame copy its tranformation
				if (i == 0)
					KeyFrames.Insert(i, new AnimKeyFrame(time));
				else
					KeyFrames.Insert(i, new AnimKeyFrame(time, KeyFrames[i - 1]->Value));
				UpdateInterval();
				return *(KeyFrames[i]);
			}
		}

		// all key frame times are before the given time,
		// no key frame available after/at the given time,
		// a new key frame must be added...

		// if there is a previous key frame copy its tranformation
		if (KeyFrames.Size())
			KeyFrames.Add(new AnimKeyFrame(time, KeyFrames[KeyFrames.Size() - 1]->Value));
		else
			KeyFrames.Add(new AnimKeyFrame(time));

		UpdateInterval();
		return *(KeyFrames.GetLast());
	}


	AnimKeyFrame* AnimTrack::GetKeyFrameAt(double time, AnimKeyFrame*& f1, AnimKeyFrame*& f2)
	{

		for (int i = 0; i < KeyFrames.Size(); i++)
		{

			// if the time match the key frame time return the key frame
			if (IsAlmostEqual(KeyFrames[i]->Time, time))
			{
				return KeyFrames[i];
			}

			// if the time is before the key frame time...
			if (time < KeyFrames[i]->Time)
			{
				// if this is not the first key frame set f1 to the previous key frame
				if (i > 0)
					f1 = KeyFrames[i - 1];
				// set f2 to the next key frame
				f2 = KeyFrames[i];
				// return nullptr as current key frame (its time didn't match the given one)
				return nullptr;
			}
		}

		// if the time passed the last key frame and there are key frames
		if (KeyFrames.Size() > 0)
		{
			f1 = KeyFrames.GetLast();
		}

		// return nullptr as current key frame (its time didn't match the given one)
		return nullptr;
	}



	float AnimTrack::Interpolate(const float t, const AnimKeyFrame& f1, const AnimKeyFrame& f2) const
	{

		float p1(f1.Value + f1.BezierParam), p2(f2.Value - f2.BezierParam);
		if (IsAlmostZero(f1.BezierParam) && IsAlmostZero(f2.BezierParam))
		{
			return LerpValues(t, f1.Value, f2.Value);
		}

		float a, b, c, d;
		float u(1 - t);
		float t2(t*t);
		float u2(u*u);
		a = u2 * u;
		b = 3 * t*u2;
		c = 3 * t2*u;
		d = t2 * t;

		return a*f1.Value + b*p1 + c*p2 + d*f2.Value;
	}


	const AnimKeyFrame* AnimTrack::GetKeyFrameByTime(double time) const
	{

		AnimKeyFrame *f1 = nullptr, *f2 = nullptr;
		AnimKeyFrame *f = ((AnimTrack*)this)->GetKeyFrameAt(time, f1, f2);
		return f;
	}


	bool AnimTrack::ChangeKeyFrameTime(double time, double new_time)
	{
		AnimKeyFrame *f1src = nullptr, *f2src = nullptr;
		AnimKeyFrame *src = GetKeyFrameAt(time, f1src, f2src);
		if (!src)
		{
			return false;
		}
		AnimKeyFrame *f1dst = nullptr, *f2dst = nullptr;
		AnimKeyFrame *dst = GetKeyFrameAt(new_time, f1dst, f2dst);
		if (dst)
		{
			return false;
		}
		int src_idx = KeyFrames.GetIndex(src);
		int new_idx = 0;
		if (f1dst) new_idx = KeyFrames.GetIndex(f1dst) + 1;
		src->Time = new_time;
		KeyFrames.Move(src_idx, new_idx);

		UpdateInterval();
		return true;
	}


	bool AnimTrack::GetValueAt(double time, float& val) const
	{

		if (!Active || KeyFrames.Size() == 0)
		{
			return false;
		}

		double t = time + TimeDelay;

		// if stop is true deactivate the animation before returning
		bool stop = false;

		if (t > EndTime)
		{

			switch (ActionAfterEnd)
			{

			case GVANIM_DEACTIVATE:
				stop = true;
				break; // go on and update the last frame

			case GVANIM_RESTART:
			case GVANIM_REVERT:
				if (!IsAlmostZero(Duration))
				{
					//t = t-StartTime-((double)(int)(t/EndTime))*EndTime;
					t = t - ((double)(int)(t / Duration))*Duration;
				}
				break;

			case GVANIM_REWIND:
				t = StartTime; // if rewind return the to the first frame
				break;

			case GVANIM_STOP:
			default:
				break;
			}
		}

		bool loop = IsRepeating();
		bool bounce = (ActionAfterEnd == GVANIM_REVERT);
		//bool rewind = (ActionAfterEnd==GVANIM_REWIND);

		////if( loop && Duration ) {
		////  //t = t-StartTime-((double)(int)(t/EndTime))*EndTime;
		////  t = t-((double)(int)(t/Duration))*Duration;
		////}

		////if( rewind && t>EndTime )
		////  t = StartTime; // if rewind return the to the first frame

		bool rev = false;
		if (loop) rev = bounce && EndTime && ((int)(t / EndTime)) % 2;
		else       rev = bounce && Duration && ((int)(t / Duration)) % 2;
		if (Reverse) rev = !rev;

		if (rev)
		{
			if (loop) t = EndTime - t;
			else t = Duration - t;
		}


		AnimKeyFrame *f1 = nullptr, *f2 = nullptr;
		AnimKeyFrame *f = ((AnimTrack*)this)->GetKeyFrameAt(t, f1, f2);

		if (f)
		{ // keyframe found
			val = f->Value;
			if (stop) Active = false;
			return true;
		}

		if (loop && !f1)
		{ // between end frame and first frame
			f1 = KeyFrames[KeyFrames.Size() - 1];
			double trim = f2->Time ? t / f2->Time : 0.0f;

			val = Interpolate((float)trim, *f1, *f2);
			if (stop) Active = false;
			return true;
		}

		if (f1 && f2)
		{ // between two keyframes
			//float dv = (val - f1->Value) / (f2->Value - f1->Value);
			//double dt = (t - f1->Time) / (f2->Time - f1->Time);
			//float speed=dv/dt;

			double trim = (t - f1->Time) / (f2->Time - f1->Time);

			val = Interpolate((float)trim, *f1, *f2);
			if (stop)
			{
				Active = false;
			}
			return true;
		}
		//if( f1 ) { // passed the end
		//  if( Rewind )
		//    val = KeyFrames.GetFirst()->Value; // if rewind return the first frame
		//  else
		//    val = f1->Value;
		//  return true;
		//}
		//if( f2 ) { // before the beginning
		//  val = f2->Value;
		//  return true;
		//}

		if (stop)
		{
			Active = false;
		}

		return false;
	}


	void AnimTrack::UpdateInterval()
	{
		if (KeyFrames.Size())
		{
			StartTime = KeyFrames.GetFirst()->Time;
			EndTime = KeyFrames.GetLast()->Time;
			Duration = EndTime - StartTime;
		}
		else
		{
			StartTime = -1.0;
			EndTime = -1.0;
			Duration = 0.0;
		}
	}


	void AnimTrack::Reset()
	{
		ResetKeyFrames();
		ResetParams();
	}


	void AnimTrack::ResetParams()
	{

		Duration = 0.0;
		TimeDelay = 0.0;
		Active = false;
		Relative = false;
		Reverse = false;
		ActionAfterEnd = 0;
	}


	void AnimTrack::ResetKeyFrames()
	{

		for (int i = 0; i < KeyFrames.Size(); i++)
		{
			delete KeyFrames[i];
		}
		KeyFrames.RemoveAll(true);
		UpdateInterval();
	}


	bool AnimTrack::DeleteKeyFrame(double time)
	{

		for (int i = 0; i < KeyFrames.Size(); i++)
		{

			if (KeyFrames[i]->Time == time)
			{
				KeyFrames.RemoveIdx(i);
				UpdateInterval();
				return true;
			}
		}

		return false;
	}


	AnimTrack& AnimTrack::operator =(const AnimTrack& track)
	{

		if (&track == this)
		{
			// Make the operator robust.
			return *this;
		}
		Reset();
		AnimKeyFrame* kf = nullptr;
		KeyFrames.Allocate(track.NumKeyFrames());
		for (unsigned i = 0; i < track.NumKeyFrames(); i++)
		{
			kf = new AnimKeyFrame;
			*kf = *(track.GetKeyFrameByIndex(i));
			KeyFrames.Add(kf);
		}

		ActionAfterEnd = track.ActionAfterEnd;
		Active = track.Active;
		Relative = track.Relative;
		Reverse = track.Reverse;
		TimeDelay = track.TimeDelay;

		UpdateInterval();

		return *this;
	}


	// Class VecAnimation implementation
	//-------------------------------------

	int VecAnimation::CollectKeyFrameTimes(std::set<double>& kf_time) const
	{

		unsigned k = 0;
		for (k = 0; k < X.NumKeyFrames(); k++)
		{
			kf_time.insert(X.GetKeyFrameByIndex(k)->Time);
		}
		for (k = 0; k < Y.NumKeyFrames(); k++)
		{
			kf_time.insert(Y.GetKeyFrameByIndex(k)->Time);
		}
		for (k = 0; k < Z.NumKeyFrames(); k++)
		{
			kf_time.insert(Z.GetKeyFrameByIndex(k)->Time);
		}

		return (int)kf_time.size();
	}


	bool VecAnimation::CreatePath(
		std::vector<Vec3>* keyframes, std::vector<unsigned>* kf_coords,
		std::vector<double>* kf_time,
		std::vector<Vec3>* points, unsigned samples,
		std::vector<Vec3>* tan_vec) const
	{
		if (!IsDefined())
			return false;
		if (points && samples)
		{
			double t_start(GetStartTime());
			double t_end(GetEndTime());
			if (X.IsRepeating() || Y.IsRepeating() || Z.IsRepeating()) t_start = 0.0;
			double t_inc((t_end - t_start) / (float)samples);
			if (IsAlmostZero(t_inc)) t_inc = 1.0; // set an arbitrary value to break the loop
			for (double t = t_start; t <= t_end; t += t_inc)
			{
				Vec3 pos;
				if (GetValueAt(t, pos))
				{
					points->push_back(pos);
				}
			}
		}

		// fill keyframes array with keyframes from all the 3 tracks sorted by time
		const AnimKeyFrame* kf(0);
		const AnimKeyFrame* kfs[3] = { 0,0,0 };
		std::set<double> kft;
		CollectKeyFrameTimes(kft);
		////size_t k=0;
		////for( k=0; k<X.NumKeyFrames(); k++ ) {
		////  kft.insert(X.GetKeyFrameByIndex(k)->Time);
		////}
		////for( k=0; k<Y.NumKeyFrames(); k++ ) {
		////  kft.insert(Y.GetKeyFrameByIndex(k)->Time);
		////}
		////for( k=0; k<Z.NumKeyFrames(); k++ ) {
		////  kft.insert(Z.GetKeyFrameByIndex(k)->Time);
		////}

		// now kft contains a set of sorted and unique time values
		std::set<double>::iterator it;
		for (it = kft.begin(); it != kft.end(); ++it)
		{
			double t = *(it);
			kfs[0] = X.GetKeyFrameByTime(t);
			kfs[1] = Y.GetKeyFrameByTime(t);
			kfs[2] = Z.GetKeyFrameByTime(t);

			Vec3 pos;
			unsigned c(0);
			Vec3 b;
			if (kfs[0]) { c |= DS_USEX; b.X = kfs[0]->BezierParam; pos.X = kfs[0]->Value; }
			if (kfs[1]) { c |= DS_USEY; b.Y = kfs[1]->BezierParam; pos.Y = kfs[1]->Value; }
			if (kfs[2]) { c |= DS_USEZ; b.Z = kfs[2]->BezierParam; pos.Z = kfs[2]->Value; }

			if (kf_time)
				kf_time->push_back(t);
			if (keyframes)
				keyframes->push_back(pos);
			if (kf_coords)
				kf_coords->push_back(c);
			if (tan_vec)
				tan_vec->push_back(b);
		}


		return true;
	}


	bool VecAnimation::GetValueAt(double time, Vec3& vec, unsigned usemask, unsigned* usedmask) const
	{

		bool xchanged = (usemask&DS_USEX) != 0 && X.GetValueAt(time, vec.X);
		bool ychanged = (usemask&DS_USEY) != 0 && Y.GetValueAt(time, vec.Y);
		bool zchanged = (usemask&DS_USEZ) != 0 && Z.GetValueAt(time, vec.Z);

		unsigned changed = 0;
		if (xchanged) changed |= DS_USEX;
		if (ychanged) changed |= DS_USEY;
		if (zchanged) changed |= DS_USEZ;

		if (usedmask)
		{
			*usedmask = changed;
		}

		return changed != 0;
	}


	//---------------------------------------------------------------------------
	// TransformAnimation class implementation

	bool TransformAnimation::GetTransformAt(double time, Transformation& tr) const
	{
		Vec3 pos(tr.GetPosition());
		Vec3 rot(tr.GetRotationEulerAngles());
		Vec3 scl(tr.GetScaling());
		bool poschanged = Position.GetValueAt(time, pos);
		bool rotchanged = Rotation.GetValueAt(time, rot);
		bool sclchanged = Scaling.GetValueAt(time, scl);
		if (poschanged) tr.SetPosition(pos);
		if (rotchanged) tr.SetRotationEulerAngles(rot);
		if (sclchanged) tr.SetScaling(scl);
		return poschanged || rotchanged || sclchanged;
	}


	int TransformAnimation::CollectKeyFrameTimes(std::set<double>& kf_time) const
	{

		size_t num = kf_time.size();
		int pos_kf = Position.CollectKeyFrameTimes(kf_time);
		int rot_kf = Rotation.CollectKeyFrameTimes(kf_time);
		int scl_kf = Scaling.CollectKeyFrameTimes(kf_time);

		return (int)(kf_time.size() - num);
	}


	int TransformAnimation::CollectKeyFrames(std::vector< AnimKeyFrameSet >& keyframe_set) const
	{

		size_t num = keyframe_set.size();
		std::set<double> kft;
		CollectKeyFrameTimes(kft);

		keyframe_set.reserve(num + kft.size());

		// now kft contains a set of sorted and unique time values
		std::set<double>::iterator it;
		for (it = kft.begin(); it != kft.end(); ++it)
		{
			double t = *(it);

			AnimKeyFrameSet kfs(t);

			kfs.KeyFrame[DS_X] = Position.X.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_Y] = Position.Y.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_Z] = Position.Z.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_RX] = Rotation.X.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_RY] = Rotation.Y.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_RZ] = Rotation.Z.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_SX] = Scaling.X.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_SY] = Scaling.Y.GetKeyFrameByTime(t);
			kfs.KeyFrame[DS_SZ] = Scaling.Z.GetKeyFrameByTime(t);

			keyframe_set.push_back(kfs);
		}

		return (int)(keyframe_set.size() - num);
	}


	//---------------------------------------------------------------------------
	// AnimTimeController class implementation


	AnimTimeController::AnimTimeController()
	{

		mTimeStart = 0.0;
		mTimePause = 0.0;
		mTimeScaling = 1.0;
		mTimeOffset = 0.0;
		mStarted = false;
		mPaused = false;
	}


	void AnimTimeController::Start(double real_time)
	{

		//if( !mStarted ) mTimeOffset = 0.0;
		if (mPaused)
		{
			mTimeStart += real_time - mTimePause;
			mPaused = false;
		}
		else mTimeStart = real_time;

		mStarted = true;
	}


	void AnimTimeController::Delay(double time_delay)
	{
		if (!mStarted)
		{
			return;
		}
		if (!IsAlmostZero(mTimeScaling)) mTimeStart += time_delay / mTimeScaling;
		if (mPaused && mTimePause < mTimeStart) mTimePause = mTimeStart;
	}


	void AnimTimeController::Stop()
	{
		mStarted = false;
		mPaused = false;
		mTimeOffset = 0.0;
	}

	/// Pause the animation
	void AnimTimeController::Pause(double real_time)
	{
		if (!mStarted)
		{
			return;
		}
		mTimePause = real_time;
		mPaused = true;
	}

	double AnimTimeController::Time(double real_time)
	{
		if (!mStarted)
		{
			return 0.0;
		}
		if (mPaused)
		{
			real_time = mTimePause;
		}
		return mTimeScaling*(real_time - mTimeStart) + mTimeOffset;
	}


	void AnimTimeController::Scale(double real_time, double scale)
	{
		if (IsAlmostEqual(mTimeScaling, scale))
		{
			return;
		}
		if (IsAlmostZero(scale))
		{
			Pause(real_time);
			return;
		}
		//// move the starting point to match the current time
		//if( IsAlmostZero(scale) ) mTimeStart = 0.0;
		//else mTimeStart = ((scale-mTimeScaling)*real_time+mTimeStart)/scale;
		if (mPaused) real_time = mTimePause;
		//double time_shift = real_time-mTimeStart;
		//mTimeOffset = fabs(mTimeScaling*time_shift+mTimeOffset-scale*time_shift);
		mTimeStart = real_time - mTimeScaling / scale*(real_time - mTimeStart);
		mTimeScaling = scale;
		if (mPaused && mTimePause < mTimeStart)
		{
			mTimePause = mTimeStart;
		}
	}

	void AnimTimeController::ShiftTime(double time_shift)
	{
		mTimeOffset = time_shift;
	}


} // namespace gpvulc
