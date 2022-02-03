//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

//Transformation with position and orientation and scaling


#include <gpvulc/ds/math/transform.h>

namespace gpvulc
{

	Transformation::Transformation()
	{
		Init();
	}

	void Transformation::Init()
	{
		ComputeSclFromMatrix = false;
		Scaling.Set(1.0f, 1.0f, 1.0f);
	}


	Transformation& Transformation::operator =(const Transformation& t)
	{

		((Location&)*this) = ((const Location&)t);
		SetScaling(t.GetScaling());
		return *this;
	}


	bool Transformation::operator ==(const Transformation& t) const
	{

		if (((Location&)*this) != ((const Location&)t))
		{
			return false;
		}
		if (Scaling != t.GetScaling())
		{
			return false;
		}

		return true;
	}



	void Transformation::Reset()
	{

		Init();
		Location::Reset();
		NotifyChange(DS_SCL_CHANGE);
	}



	void Transformation::UpdateFromMatrix3() const
	{

		Mat4 m = Matrix;
		Vec3& sc = Scaling;
		int i;
		Vec4 axis;
		float len;
		for (i = 0; i < 3; i++)
		{
			axis = m.GetCol(i);
			//axis.Normalize();
			len = axis.Length();
			if (len)
			{
				if (ComputeSclFromMatrix)
					sc[i] = len;
				// ensure the rotation matrix does not contain scaling
				if (ComputeRotFromMatrix)
					axis /= len;
			}
			if (ComputeRotFromMatrix)
				m.SetCol(i, axis);
		}
		if (ComputeRotFromMatrix)
			Rotation.SetMatrix(m);

		ComputeSclFromMatrix = false;
		ComputeRotFromMatrix = false;
	}


	void Transformation::UpdateMatrix3() const
	{

		Mat4 m;
		Vec3 s = GetScaling();

		m.SetRotationQuat(Rotation);
		Matrix.SetCol(0, m.GetCol(0) * s.X);
		Matrix.SetCol(1, m.GetCol(1) * s.Y);
		Matrix.SetCol(2, m.GetCol(2) * s.Z);
		ComputeSubMatrix3 = false;
		ComputeInverseMatrix = true;
	}


	void Transformation::SetRotationMatrix(const Mat4& rot)
	{

		ComputeFromEulerAngles = false;
		Quat prevrot = GetRotationQuaternion();
		Rotation.SetMatrix(rot);
		ComputeRotFromMatrix = false;
		if (ComputeSclFromMatrix)
			UpdateFromMatrix3();
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		if (prevrot != Rotation)
			NotifyChange(DS_ROT_CHANGE);
	}


	bool Transformation::SetMatrix(const Mat4& mat)
	{

		bool result = Location::SetMatrix(mat);

		if (result)
		{

			// extracting and comparing the real rotation and scaling
			// is comparable to recalculating scaling from the matrix
			ComputeSclFromMatrix = ComputeRotFromMatrix;

			unsigned what = 0;
			if (ComputePos) what |= DS_POS_CHANGE;
			if (ComputeRotFromMatrix) what |= DS_ROT_CHANGE;
			if (ComputeSclFromMatrix) what |= DS_SCL_CHANGE;

			NotifyChange(what);
		}

		return result;
	}


	void Transformation::SetScaling(const Vec3& scl)
	{

		if (scl == GetScaling())
		{
			return;
		}
		Scaling = scl;
		ComputeSclFromMatrix = false;
		if (ComputeRotFromMatrix)
			UpdateFromMatrix3();
		ComputeSubMatrix3 = true;
		ComputeInverseMatrix = true;
		NotifyChange(DS_MAT_CHANGE);
	}


	Vec3 Transformation::GetScaling() const
	{

		if (ComputeSclFromMatrix)
			UpdateFromMatrix3();
		return Scaling;
	}








	//-------------------------------------------------------------------------

	PivotTransform::PivotTransform(int)
	{

		_Init();
	}


	void PivotTransform::_Init()
	{

		Changed = true;
		Translation = Vec3();
		Rotation = Vec3();
		Scaling = Vec3(1.0f, 1.0f, 1.0f);
		RotMatrix.Reset();
		Valid = false;
		ValidEulerAngles = false;
		EnablePivotCoordSys = false;
		EnableTranslation = false;
		EnableRotation = false;
		EnableScaling = false;
		EnableTransform = false;
		PivotCoordSys.MakeIdentity();
		InvPivotCoordSys.MakeIdentity();
		Constraints = nullptr;
	}


	bool PivotTransform::SetPivotMatrix(Mat4 piv)
	{
		//piv.SetCol(3, Vec4(0,0,0,1) );
		//piv.SetRow(3, Vec4(0,0,0,1) );
		if (PivotCoordSys == piv)
		{
			return false;
		}
		PivotCoordSys = piv;
		Valid = false;
		EnablePivotCoordSys = !(PivotCoordSys.IsIdentity());
		if (EnablePivotCoordSys)
			InvPivotCoordSys = PivotCoordSys.Inverted();
		else
			InvPivotCoordSys.MakeIdentity();
		Changed = true;

		return true;
	}


	bool PivotTransform::SetPivot(Vec3 piv)
	{
		if (PivotCoordSys.GetTranslation() == piv)
		{
			return false;
		}
		PivotCoordSys.SetTranslation(piv);
		InvPivotCoordSys.SetTranslation(-piv);
		Valid = false;
		EnablePivotCoordSys = !(PivotCoordSys.IsIdentity());
		Changed = true;

		return true;
	}


	void PivotTransform::GetPivotCoordSys(Vec3& xaxis, Vec3& yaxis, Vec3& zaxis) const
	{
		xaxis = PivotCoordSys.GetCol(0);
		yaxis = PivotCoordSys.GetCol(1);
		zaxis = PivotCoordSys.GetCol(2);
	}


	bool PivotTransform::SetPivotCoordSys(Vec3 xaxis, Vec3 yaxis, Vec3 zaxis)
	{
		Mat4 m;
		m.SetCol(0, xaxis);
		m.SetCol(1, yaxis);
		m.SetCol(2, zaxis);
		m.SetCol(3, PivotCoordSys.GetCol(3));
		return SetPivotMatrix(m);
	}


	/// Return the translation
	//Vec3 GetTranslation() { return Translation;}
	/// Set the translation [0,0,0]=disabled
	bool PivotTransform::SetTranslation(Vec3 pos, unsigned changemask)
	{

		Vec3 tr(Translation);
		tr.Set(pos, changemask);
		if (Translation == tr)
		{
			return false;
		}
		if (Constraints)
		{
			if (!Constraints->SetTranslation(Translation, tr))
			{
				return false;
			}
		}
		else Translation = tr;
		Valid = false;
		EnableTranslation = (!Translation.IsZero());
		EnableTransform = EnableTranslation || EnableRotation || EnableScaling;
		Changed = true;

		return true;
	}


	void PivotTransform::_UpdateRotation() const
	{
		if (!ValidEulerAngles) Rotation = RotMatrix.GetEuler();
		ValidEulerAngles = true;
	}


	void PivotTransform::_CheckRotMatrix()
	{

		Valid = false;
		EnableRotation = (!RotMatrix.IsIdentity());
		EnableTransform = EnableTranslation || EnableRotation || EnableScaling;
		Changed = true;
	}


	/// Return the rotation (Euler angles)
	//Vec3 GetRotation() { return Rotation;}
	/// Set the rotation (Euler angles) [0,0,0]=disabled
	bool PivotTransform::SetRotation(Vec3 r, unsigned changemask)
	{

		_UpdateRotation();
		Vec3 rot(Rotation);
		rot.Set(r, changemask);
		VecFixEulerAngles(rot);
		////if( ValidEulerAngles && Rotation==rot )
		{
			return true;
		}

		if (Constraints)
		{
			if (!Constraints->SetRotation(Rotation, rot))
				return false;
		}
		else Rotation = rot;
		ValidEulerAngles = true;
		Mat4 rotmat;
		rotmat.MakeEuler(Rotation.X, Rotation.Y, Rotation.Z);
		if (RotMatrix == rotmat)
		{
			return false;
		}
		RotMatrix = rotmat;
		_CheckRotMatrix();

		return true;
	}


	/// Set the rotation (angle around an axis) [0,[0,0,0]]=disabled
	bool PivotTransform::SetRotation(float angle, Vec3 axis)
	{

		if (Constraints)
		{
			Vec3 rot;
			VecGetEuler(angle, axis, rot);
			return SetRotation(rot);
		}
		Mat4 rotmat;
		rotmat.MakeRotation(angle, axis);
		if (RotMatrix == rotmat)
		{
			return false;
		}
		RotMatrix = rotmat;
		ValidEulerAngles = false;
		_CheckRotMatrix();

		return true;
	}


	bool PivotTransform::SetRotationMatrix(Mat4 rotmat)
	{

		if (Constraints)
		{
			Vec3 rot = rotmat.GetEuler();
			return SetRotation(rot);
		}
		if (RotMatrix == rotmat)
		{
			return false;
		}
		RotMatrix = rotmat;
		ValidEulerAngles = false;
		_CheckRotMatrix();
		return true;
	}


	bool PivotTransform::SetReferenceSystem(Vec3 xAxis, Vec3 yAxis, Vec3 zAxis)
	{

		Mat4 mat;
		mat.SetCol(0, Vec4(xAxis, 0));
		mat.SetCol(1, Vec4(yAxis, 0));
		mat.SetCol(2, Vec4(zAxis, 0));

		bool changed = false;
		Mat4 r;
		if (SetScaling(mat.GetRotScaling(r)))
			changed = true;

		if (SetRotationMatrix(r))
			changed = true;

		return changed;
	}


	void PivotTransform::GetReferenceSystem(Vec3* xAxis, Vec3* yAxis, Vec3* zAxis)
	{

		GetTransformMatrix(); // update the transform matrix
		if (xAxis) *xAxis = TransformMatrix.GetCol(0).XYZ();
		if (yAxis) *yAxis = TransformMatrix.GetCol(1).XYZ();
		if (zAxis) *zAxis = TransformMatrix.GetCol(2).XYZ();
	}


	/// Get the (x,y,z) scaling
	//Vec3 GetScaling() { return Scaling;}
	/// Set the (x,y,z) scaling [1,1,1]=disabled
	bool PivotTransform::SetScaling(Vec3 scl, unsigned changemask)
	{

		Vec3 sc(Scaling);
		sc.Set(scl, changemask);
		if (Scaling == sc)
			return false;
		if (Constraints)
		{
			if (!Constraints->SetScaling(Scaling, sc))
				return false;
		}
		else Scaling = sc;
		Valid = false;
		EnableScaling = (sc != Vec3(1, 1, 1));
		EnableTransform = EnableTranslation || EnableRotation || EnableScaling;
		Changed = true;

		return true;
	}


	bool PivotTransform::SetTransformMatrix(Mat4 mat)
	{

		if (TransformMatrix == mat)
			return false;

		Changed = true;
		Valid = false;
		ValidEulerAngles = false;
		EnableTranslation = false;
		EnableRotation = false;
		EnableScaling = false;
		EnableTransform = false;
		Translation = Vec3();
		Rotation = Vec3();
		Scaling = Vec3(1.0f, 1.0f, 1.0f);
		RotMatrix.Reset();

		TransformMatrix = mat;
		//Constraints = constraints;
		EnableTransform = !TransformMatrix.IsIdentity();
		SetTranslation(mat.GetTranslation());
		Mat4 r;
		SetScaling(mat.GetRotScaling(r));
		SetRotationMatrix(r);

		if (!PivotCoordSys.IsIdentity())
		{
			FinalMatrix = PivotCoordSys*TransformMatrix*InvPivotCoordSys;
		}
		else FinalMatrix = TransformMatrix;
		Valid = true;

		return true;
	}


	Mat4 PivotTransform::GetTransformMatrix() const
	{
		if (!Valid) ((PivotTransform&)(*this))._ComputeMatrix();
		return TransformMatrix;
	}


	Mat4 PivotTransform::GetMatrix() const
	{
		if (!Valid) _ComputeMatrix();
		return FinalMatrix;
	}


	// Assignment operators

	PivotTransform& PivotTransform::operator =(const PivotTransform& t)
	{

		SetPivotMatrix(t.GetPivotMatrix());
		SetTranslation(t.GetTranslation());
		SetRotation(t.GetRotation());
		SetScaling(t.GetScaling());
		SetConstraints(t.GetConstraints());
		return *this;
	}


	PivotTransform& PivotTransform::operator =(const Transformation& t)
	{

		//SetMatrix(     t.GetMatrix()     );
		SetTranslation(t.GetPosition());
		SetRotation(t.GetRotationEulerAngles());
		SetScaling(t.GetScaling());
		return *this;
	}


	void PivotTransform::_ComputeMatrix() const
	{

		TransformMatrix.MakeIdentity();
		if (EnableTransform)
		{

			Mat4 matsc, mat;
			if (EnableScaling) { MatMakeScaling(Scaling, TransformMatrix); }
			if (EnableRotation) { TransformMatrix = RotMatrix*TransformMatrix; }
			if (EnableTranslation) TransformMatrix += Translation;
		}

		//if( EnablePivotCoordSys ) FinalMatrix *= InvPivotCoordSys;
		//if( EnableScaling ) { MatMakeScaling(Scaling,matsc); FinalMatrix = matsc*FinalMatrix; }
		//if( EnableRotation ) { FinalMatrix = RotMatrix*FinalMatrix; }
		//if( EnableTranslation ) FinalMatrix += Translation;
		//if( EnablePivotCoordSys ) FinalMatrix *= PivotCoordSys;
		if (EnableTransform)
		{
			if (EnablePivotCoordSys)
				FinalMatrix = PivotCoordSys*TransformMatrix*InvPivotCoordSys;
			else
				FinalMatrix = TransformMatrix;
		}
		else
			FinalMatrix.MakeIdentity();

		Valid = true;
	}


	void PivotTransform::Reset()
	{
		_Init();
	}


	bool PivotTransform::operator ==(const PivotTransform& t) const
	{
		if (PivotCoordSys != t.GetPivotMatrix()) return false;
		if (Translation != t.GetTranslation()) return false;
		if (RotMatrix != t.GetRotationMatrix()) return false;
		if (Scaling != t.GetScaling()) return false;
		if ((Constraints == nullptr) != (t.GetConstraints() == nullptr)) return false;
		//if( Constraints && t.GetConstraints() && ( *Constraints!=*(t.GetConstraints()) ) ) return false;
		return true;
	}

} // namespace gpvulc
