//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Movable location with position and orientation


#include <gpvulc/ds/math/Location.h>

namespace gpvulc
{

	// Movable location
	// class Location implementation

	Location::Location()
	{

		Init();
		_DisableChangesNotification = false;
	}


	void Location::Init()
	{
		ComputePos = false;
		ComputeRotFromMatrix = false;
		ComputeSubMatrixPos = false;
		ComputeSubMatrix3 = false;
		ComputeInverseMatrix = false;
		ComputeFromEulerAngles = false;
	}


	void Location::Reset()
	{
		Position.Set(0, 0, 0);
		Rotation.Reset();
		EulerAngles.Set(0, 0, 0);
		Matrix.MakeIdentity();
		InverseMatrix = Matrix;
		Init();
		NotifyChange(DS_POS_CHANGE | DS_ROT_CHANGE | DS_MAT_CHANGE);
	}


	Location& Location::operator =(const Location& loc)
	{
		SetMatrix(loc.GetMatrix());
		if (loc.EulerAnglesAvailable())
		{
			EulerAngles = loc.GetRotationEulerAngles();
			ComputeFromEulerAngles = true;
			Rotation.SetEulerAngles(EulerAngles);
			ComputeRotFromMatrix = false;
		}
		return *this;
	}


	void Location::UpdateFromMatrix3() const
	{

		if (ComputeRotFromMatrix)
			Rotation.SetMatrix(Matrix);

		ComputeRotFromMatrix = false;
	}


	void Location::UpdatePositionFromMatrix() const
	{

		Position = Matrix.GetTranslation();
		ComputePos = false;
	}


	void Location::UpdateMatrixPos() const
	{

		Matrix.SetTranslation(Position);
		ComputeSubMatrixPos = false;
		ComputeInverseMatrix = true;
	}


	void Location::UpdateMatrix3() const
	{

		Mat4 m;

		Matrix.SetRotationQuat(Rotation);

		ComputeSubMatrix3 = false;
		ComputeInverseMatrix = true;
	}


	void Location::UpdateInvMatrix() const
	{

		Mat4& invmat = InverseMatrix;
		invmat = GetMatrix();
		invmat.Invert();
		ComputeInverseMatrix = false;
	}


	void Location::SetPosition(const Vec3& pos)
	{

		ComputeSubMatrixPos = (pos != Position);
		Position = pos;
		ComputePos = false;
		ComputeInverseMatrix = true;
		NotifyChange(DS_POS_CHANGE);
	}


	void Location::SetRotationEulerAngles(const Vec3& rot)
	{

		Quat prevrot = GetRotationQuaternion();
		Rotation.SetEulerAngles(rot);
		ComputeRotFromMatrix = false;
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;

		EulerAngles = rot;
		ComputeFromEulerAngles = true;

		NotifyChange(DS_ROT_CHANGE);
	}


	void Location::SetRotationAxisAngle(const Vec3& axis, float angle)
	{

		ComputeFromEulerAngles = false;
		Quat prevrot = GetRotationQuaternion();
		Rotation.SetAxisAngle(axis, angle);
		ComputeRotFromMatrix = false;
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		if (prevrot != Rotation)
			NotifyChange(DS_ROT_CHANGE);
	}


	void Location::SetRotationQuaternion(const Quat& rot)
	{

		if (rot == GetRotationQuaternion())
		{
			return;
		}
		ComputeFromEulerAngles = false;
		Rotation = rot;
		ComputeRotFromMatrix = false;
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		NotifyChange(DS_ROT_CHANGE);
	}


	void Location::SetRotationMatrix(const Mat4& rot)
	{

		ComputeFromEulerAngles = false;
		Quat prevrot = GetRotationQuaternion();
		Rotation.SetMatrix(rot);
		ComputeRotFromMatrix = false;
		Matrix.CopySubMatrix3(rot);
		ComputeSubMatrix3 = false;
		ComputeInverseMatrix = true;
		if (prevrot != Rotation)
			NotifyChange(DS_ROT_CHANGE);
	}



	bool Location::SetMatrix(const Mat4& mat)
	{

		ComputeFromEulerAngles = false;
		Mat4 m(mat);
		// remove scaling

		bool mat_changed = (m != GetMatrix());
		if (!mat_changed)
		{
			return false;
		}

		ComputePos = m.GetTranslation() != Position;

		// compare the first 3 columns (scaling included)
		ComputeRotFromMatrix =
			Matrix.GetCol(0) != m.GetCol(0)
			|| Matrix.GetCol(1) != m.GetCol(1)
			|| Matrix.GetCol(2) != m.GetCol(2);
		ComputeSubMatrixPos = false;
		ComputeSubMatrix3 = false;
		ComputeInverseMatrix = true;

		Matrix = m;

		unsigned what = 0;
		if (ComputePos) what |= DS_POS_CHANGE;
		if (ComputeRotFromMatrix) what |= DS_ROT_CHANGE;

		NotifyChange(what);

		return true;
	}


	Vec3 Location::GetPosition() const
	{
		if (ComputePos)
			UpdatePositionFromMatrix();
		return Position;
	}


	Vec3 Location::GetRotationEulerAngles() const
	{

		if (ComputeRotFromMatrix)
			return Matrix.GetEuler();
		if (ComputeFromEulerAngles)
			return EulerAngles;
		return Rotation.GetEulerAngles();
	}


	void Location::GetRotationAxisAngle(Vec3& axis, float& angle) const
	{

		if (ComputeRotFromMatrix)
			UpdateFromMatrix3();
		return Rotation.GetAxisAngle(axis, angle);
	}


	Quat Location::GetRotationQuaternion() const
	{

		if (ComputeRotFromMatrix)
			UpdateFromMatrix3();
		return Rotation;
	}


	Mat4 Location::GetRotationMatrix() const
	{

		Mat4 rot;
		if (ComputeSubMatrix3)
			UpdateMatrix3();
		rot.CopySubMatrix3(Matrix);
		return rot;
	}


	Mat4 Location::GetMatrix() const
	{

		if (ComputeSubMatrix3)
		{
			UpdateMatrix3();
		}
		if (ComputeSubMatrixPos)
		{
			UpdateMatrixPos();
		}
		return Matrix;
	}


	Mat4 Location::GetInverseMatrix() const
	{

		if (ComputeInverseMatrix) UpdateInvMatrix();
		return InverseMatrix;
	}


	void Location::MoveBy(const Vec3& pos)
	{

		if (pos.IsZero())
		{
			return;
		}
		ComputeSubMatrixPos = true;
		Position = GetPosition() + pos;
		ComputePos = false;
		ComputeInverseMatrix = true;
		NotifyChange(DS_POS_CHANGE);
	}


	Vec3 Location::GetSlide(const Vec3& vec)
	{
		if (vec.IsZero())
		{
			return Vec3();
		}
		Vec3 offset;
		MatVecTransform(vec, GetMatrix(), offset);
		return offset;
	}


	void Location::Slide(const Vec3& vec)
	{
		if (vec.IsZero())
		{
			return;
		}
		Vec3 offset(vec);
		MatVecTransform(offset, GetMatrix(), offset);
		MoveBy(offset);
	}

	void Location::RotateBy(float angle, const Vec3& axis)
	{
		if (IsAlmostZero(angle) || axis.IsZero())
		{
			return;
		}
		Quat q(axis, angle);
		Rotation = q*GetRotationQuaternion();
		ComputeRotFromMatrix = false;
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		NotifyChange(DS_ROT_CHANGE);
	}


	void Location::Spin(float angle, const Vec3& axis)
	{

		if (IsAlmostZero(angle) || axis.IsZero())
		{
			return;
		}
		Quat q(axis, angle);
		Rotation = GetRotationQuaternion()*q;
		ComputeRotFromMatrix = false;
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		NotifyChange(DS_ROT_CHANGE);
	}


	void Location::Orbit(const Vec3& pnt, float angle, const Vec3& axis)
	{

		if (IsAlmostZero(angle) || axis.IsZero())
		{
			return;
		}

		Quat q(axis, angle);
		// set position relative to the reference point
		Vec3 p1 = GetPosition() - pnt;
		Vec3 p2;
		// rotate position around the point
		MatVecTransform(p1, q.GetMatrix(), p2);
		// set position adding the reference point to transformed point
		Position = pnt + p2;

		ComputePos = false;
		ComputeSubMatrixPos = true;
		ComputeInverseMatrix = true;
		NotifyChange(DS_POS_CHANGE);
	}

} // namespace gpvulc
