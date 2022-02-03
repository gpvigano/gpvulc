//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Constraints for vectors, location and transformations


#include <gpvulc/ds/math/Constraints.h>

namespace gpvulc
{

	//******************************************************************************
	// CONSTRAINTS
	//******************************************************************************

	LocationConstraints::LocationConstraints()
	{
	}

	void LocationConstraints::ResetDof(int which)
	{
		if (which & DS_POS_X) Translation.X.Reset();
		if (which & DS_POS_Y) Translation.Y.Reset();
		if (which & DS_POS_Z) Translation.Z.Reset();
		if (which & DS_ROT_X) Rotation.X.Reset();
		if (which & DS_ROT_Y) Rotation.Y.Reset();
		if (which & DS_ROT_Z) Rotation.Z.Reset();
	}

	void LocationConstraints::SetDofActive(int which, bool active)
	{
		if (which & DS_POS_X) Translation.X.Active = active;
		if (which & DS_POS_Y) Translation.Y.Active = active;
		if (which & DS_POS_Z) Translation.Z.Active = active;
		if (which & DS_ROT_X) Rotation.X.Active = active;
		if (which & DS_ROT_Y) Rotation.Y.Active = active;
		if (which & DS_ROT_Z) Rotation.Z.Active = active;
	}

	void LocationConstraints::SetDofLocked(int which, bool locked)
	{
		if (which & DS_POS_X) Translation.X.Locked = locked;
		if (which & DS_POS_Y) Translation.Y.Locked = locked;
		if (which & DS_POS_Z) Translation.Z.Locked = locked;
		if (which & DS_ROT_X) Rotation.X.Locked = locked;
		if (which & DS_ROT_Y) Rotation.Y.Locked = locked;
		if (which & DS_ROT_Z) Rotation.Z.Locked = locked;
		if (locked) SetDofActive(which, true);
	}

	void LocationConstraints::SetDofMax(int which, float val, bool enabled)
	{
		if (which & DS_POS_X) Translation.X.Max.Set(enabled, val);
		if (which & DS_POS_Y) Translation.Y.Max.Set(enabled, val);
		if (which & DS_POS_Z) Translation.Z.Max.Set(enabled, val);
		if (which & DS_ROT_X) Rotation.X.Max.Set(enabled, val);
		if (which & DS_ROT_Y) Rotation.Y.Max.Set(enabled, val);
		if (which & DS_ROT_Z) Rotation.Z.Max.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

	void LocationConstraints::SetDofMin(int which, float val, bool enabled)
	{
		if (which & DS_POS_X) Translation.X.Min.Set(enabled, val);
		if (which & DS_POS_Y) Translation.Y.Min.Set(enabled, val);
		if (which & DS_POS_Z) Translation.Z.Min.Set(enabled, val);
		if (which & DS_ROT_X) Rotation.X.Min.Set(enabled, val);
		if (which & DS_ROT_Y) Rotation.Y.Min.Set(enabled, val);
		if (which & DS_ROT_Z) Rotation.Z.Min.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

	void LocationConstraints::SetDofRange(int which, float min, float max, bool loop, bool enabled)
	{
		if (which & DS_POS_X) Translation.X.SetRange(min, max, loop, enabled);
		if (which & DS_POS_Y) Translation.Y.SetRange(min, max, loop, enabled);
		if (which & DS_POS_Z) Translation.Z.SetRange(min, max, loop, enabled);
		if (which & DS_ROT_X) Rotation.X.SetRange(min, max, loop, enabled);
		if (which & DS_ROT_Y) Rotation.Y.SetRange(min, max, loop, enabled);
		if (which & DS_ROT_Z) Rotation.Z.SetRange(min, max, loop, enabled);
		if (enabled) SetDofActive(which, true);
	}

	void LocationConstraints::SetDofSnap(int which, float val, bool enabled)
	{
		if (which & DS_POS_X) Translation.X.Snap.Set(enabled, val);
		if (which & DS_POS_Y) Translation.Y.Snap.Set(enabled, val);
		if (which & DS_POS_Z) Translation.Z.Snap.Set(enabled, val);
		if (which & DS_ROT_X) Rotation.X.Snap.Set(enabled, val);
		if (which & DS_ROT_Y) Rotation.Y.Snap.Set(enabled, val);
		if (which & DS_ROT_Z) Rotation.Z.Snap.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

	// TransformConstraints
	//-------------------------
	TransformConstraints::TransformConstraints()
	{
	}


	void TransformConstraints::ResetDof(int which)
	{
		LocationConstraints::ResetDof(which);
		if (which & DS_SCALE_X) Scaling.X.Reset();
		if (which & DS_SCALE_Y) Scaling.Y.Reset();
		if (which & DS_SCALE_Z) Scaling.Z.Reset();
	}

	void TransformConstraints::SetDofActive(int which, bool active)
	{
		LocationConstraints::SetDofActive(which, active);
		if (which & DS_SCALE_X) Scaling.X.Active = active;
		if (which & DS_SCALE_Y) Scaling.Y.Active = active;
		if (which & DS_SCALE_Z) Scaling.Z.Active = active;
	}

	void TransformConstraints::SetDofLocked(int which, bool locked)
	{
		LocationConstraints::SetDofLocked(which, locked);
		if (which & DS_SCALE_X) Scaling.X.Locked = locked;
		if (which & DS_SCALE_Y) Scaling.Y.Locked = locked;
		if (which & DS_SCALE_Z) Scaling.Z.Locked = locked;
		if (locked) SetDofActive(which, true);
	}

	void TransformConstraints::SetDofMax(int which, float val, bool enabled)
	{
		LocationConstraints::SetDofMax(which, val, enabled);
		if (which & DS_SCALE_X) Scaling.X.Max.Set(enabled, val);
		if (which & DS_SCALE_Y) Scaling.Y.Max.Set(enabled, val);
		if (which & DS_SCALE_Z) Scaling.Z.Max.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

	void TransformConstraints::SetDofMin(int which, float val, bool enabled)
	{
		LocationConstraints::SetDofMin(which, val, enabled);
		if (which & DS_SCALE_X) Scaling.X.Min.Set(enabled, val);
		if (which & DS_SCALE_Y) Scaling.Y.Min.Set(enabled, val);
		if (which & DS_SCALE_Z) Scaling.Z.Min.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

	void TransformConstraints::SetDofRange(int which, float min, float max, bool loop, bool enabled)
	{
		LocationConstraints::SetDofRange(which, min, max, loop, enabled);
		if (which & DS_SCALE_X) Scaling.X.SetRange(min, max, loop, enabled);
		if (which & DS_SCALE_Y) Scaling.Y.SetRange(min, max, loop, enabled);
		if (which & DS_SCALE_Z) Scaling.Z.SetRange(min, max, loop, enabled);
		if (enabled) SetDofActive(which, true);
	}

	void TransformConstraints::SetDofSnap(int which, float val, bool enabled)
	{
		LocationConstraints::SetDofSnap(which, val, enabled);
		if (which & DS_SCALE_X) Scaling.X.Snap.Set(enabled, val);
		if (which & DS_SCALE_Y) Scaling.Y.Snap.Set(enabled, val);
		if (which & DS_SCALE_Z) Scaling.Z.Snap.Set(enabled, val);
		if (enabled) SetDofActive(which, true);
	}

} // namespace gpvulc
