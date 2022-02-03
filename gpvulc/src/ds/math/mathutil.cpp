//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// File I/O facility


#include <gpvulc/ds/math/mathutil.h>

namespace gpvulc
{

	// constrain values to a cyclic range from min to max
	void LimitValueRange(float& val, float min, float max)
	{

		float range = max - min;
		if (range == 0) { val = min; return; }
		if (range < 0)
		{
			float swap = min;
			min = max;
			max = swap;
			range = -range;
		}
		while (val < min) val += range;
		while (val >= max) val -= range;
	}

	//******************************************************************************
	// VECTORS
	//******************************************************************************

	// gets 2 points and returns the vector going from p1 to p2
	void VecFromTo(const float p1[3], const float p2[3], float v[3])
	{

		v[0] = p2[0] - p1[0];
		v[1] = p2[1] - p1[1];
		v[2] = p2[2] - p1[2];
	}

	// the usual cross product: result = u x v
	void VecCrossProduct(const float u[3], const float v[3], float result[3])
	{

		result[0] = u[1] * v[2] - u[2] * v[1];
		result[1] = u[2] * v[0] - u[0] * v[2];
		result[2] = u[0] * v[1] - u[1] * v[0];
	}

	// the scalar product u . v
	float VecDotProduct(const float u[3], const float v[3]) { return u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; }

	// norm of a vector
	float VecNorm(const float v[3]) { return (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

	// normalizes vector v
	void VecNormalize(float v[3])
	{

		float vn = VecNorm(v);
		if (vn == 0)
		{
			v[0] = v[1] = v[2] = 0.0;
		}
		else
		{
			v[0] = v[0] / vn;
			v[1] = v[1] / vn;
			v[2] = v[2] / vn;
		}
	}

	/// returns in c the cross product (a "not normalized normal") at v1 (between v0->v1 and v1->v2)
	void VecCross(const float v0[3], const float v1[3], const float v2[3], float c[3])
	{

		Vec3 u, v;
		VecFromTo(v1, v2, u);
		VecFromTo(v1, v0, v);
		VecCrossProduct(u, v, c);
	}

	/// returns in n the normal at v1 (between v0->v1 and v1->v2)
	void VecNormal(const float v0[3], const float v1[3], const float v2[3], float normal[3])
	{

		Vec3 u, v;
		VecFromTo(v1, v2, u);
		VecFromTo(v1, v0, v);
		VecCrossProduct(u, v, normal);
		VecNormalize(normal);
	}

	// rotate a vector by the given angle (IN DEGREES) around an axis
	void VecRotate(const float p[3], float angle, const float axis[3], float result[3])
	{

		Vec3 res(p);

		// creates quaternion to rotate the points
		Quat q, q1, p1, p2;

		//angle /= 2;
		angle = DS_DEG2RAD*angle*0.5f;
		float sin_angle = sin(angle);
		q.S = cos(angle);
		q.V.X = sin_angle * axis[0];
		q.V.Y = sin_angle * axis[1];
		q.V.Z = sin_angle * axis[2];
		q1.S = q.S; q1.V.X = -q.V.X; q1.V.Y = -q.V.Y; q1.V.Z = -q.V.Z;

		p1.S = 0; p1.V.X = res[0]; p1.V.Y = res[1]; p1.V.Z = res[2];
		p2 = QuatMultiply(p1, q1); p1 = QuatMultiply(q, p2);
		res[0] = p1.V.X; res[1] = p1.V.Y; res[2] = p1.V.Z;

		result[0] = res[0];
		result[1] = res[1];
		result[2] = res[2];

	}

	// applies scaling, rotation and then translation to a point
	void VecTransform(const float p[3], const float scaling[3], float angle, const float axis[3], const float translation[3], float result[3])
	{

		result[0] = p[0]; result[1] = p[1]; result[2] = p[2];

		// creates quaternion to rotate the points
		Quat q, q1, p1, p2; bool rotate = false;
		if ((axis[0] != 0 || axis[1] != 0 || axis[2] != 0) && angle != 0)
		{

			rotate = true;

			//angle /= 2;
			angle = DS_DEG2RAD*angle*0.5f;
			float sin_angle = sin(angle);
			q.S = cos(angle);
			q.V.X = sin_angle * axis[0];
			q.V.Y = sin_angle * axis[1];
			q.V.Z = sin_angle * axis[2];
			q1.S = q.S; q1.V.X = -q.V.X; q1.V.Y = -q.V.Y; q1.V.Z = -q.V.Z;

		}

		result[0] *= scaling[0]; result[1] *= scaling[1]; result[2] *= scaling[2];

		if (rotate)
		{

			p1.S = 0; p1.V.X = result[0]; p1.V.Y = result[1]; p1.V.Z = result[2];
			p2 = QuatMultiply(p1, q1); p1 = QuatMultiply(q, p2);
			result[0] = p1.V.X; result[1] = p1.V.Y; result[2] = p1.V.Z;

		}

		result[0] += translation[0];
		result[1] += translation[1];
		result[2] += translation[2];

	}

	// applies the inverse of the given transformation (scaling, rotation and then translation) to a point (angle in DEGREES)
	void VecInverseTransform(const float p[3], const float scaling[3], float angle, const float axis[3], float translation[3], float result[3])
	{

		result[0] = p[0]; result[1] = p[1]; result[2] = p[2];

		// inverted translation
		result[0] -= translation[0]; result[1] -= translation[1]; result[2] -= translation[2];

		// creates quaternion to rotate the points
		Quat q, q1, p1, p2;
		if ((axis[0] != 0 || axis[1] != 0 || axis[2] != 0) && angle != 0)
		{

			//angle /= 2;
			angle = DS_DEG2RAD*angle*0.5f;
			// inverted angle
			angle = -angle;
			float sin_angle = sin(angle);
			q.S = cos(angle);
			q.V.X = sin_angle * axis[0];
			q.V.Y = sin_angle * axis[1];
			q.V.Z = sin_angle * axis[2];
			q1.S = q.S; q1.V.X = -q.V.X; q1.V.Y = -q.V.Y; q1.V.Z = -q.V.Z;

			p1.S = 0; p1.V.X = result[0]; p1.V.Y = result[1]; p1.V.Z = result[2];
			p2 = QuatMultiply(p1, q1); p1 = QuatMultiply(q, p2);
			result[0] = p1.V.X; result[1] = p1.V.Y; result[2] = p1.V.Z;

		}

		result[0] /= scaling[0]; result[1] /= scaling[1]; result[2] /= scaling[2];

	}


	// calculate the angle between two vectors
	float VecAngle(const float v1[3], const float v2[3])
	{

		float a = VecNorm(v1);
		if (a == 0)
		{
			return 0;
		}
		float b = VecNorm(v2);
		if (b == 0)
		{
			return 0;
		}
		float c = VecDotProduct(v1, v2);// c=a.b*cos(theta)

		float theta = DS_RAD2DEG*acos(c / (a*b));
		return theta;
	}


	float VecAngleOnPlane(const float v1[3], const float v2[3], const float n[3])
	{
		Vec3 vec1(v1);
		Vec3 vec2(v2);
		Vec3 normal(n);
		vec1.ProjectOnPlane(normal);
		vec2.ProjectOnPlane(normal);
		float angle = VecAngle(vec1, vec2);
		if (IsAlmostZero(angle))
		{
			return 0.0f;
		}
		Vec3 cross(vec1.Cross(vec2));
		if (cross.Dot(normal) < 0.0f)
		{
			return 360.0f - angle;
		}

		return angle;
	}



	// constrain angles in rot to a cyclic range from min to max
	void VecValueRange(float rot[3], float min, float max)
	{
		float range = max - min;
		if (range == 0)
		{
			rot[0] = rot[1] = rot[2] = min;
			return;
		}
		if (range < 0)
		{
			float swap = min;
			min = max;
			max = swap;
			range = -range;
		}
		for (int k = 0; k < 3; k++)
		{
			while (rot[k] < min)
				rot[k] += range;
			while (rot[k] >= max)
				rot[k] -= range;
		}
	}

	// Set rot1 angles according to the minor angles with rot2
	void VecMinorAngle(float rot1[3], const float rot2[3])
	{

		for (int k = 0; k < 3; k++)
		{
			while (rot2[k] - rot1[k]>180)
				rot1[k] += 360.0f;
			while (rot1[k] - rot2[k]>180)
				rot1[k] -= 360.0f;
		}
	}

	void SetMinorAngle(float& rot1, float rot2)
	{

		while (rot2 - rot1 > 180)
			rot1 += 360.0f;
		while (rot1 - rot2 > 180)
			rot1 -= 360.0f;
	}

	// Calculate the square distance between 2 points
	float VecSquareDistance(const float a[3], const float b[3])
	{

		//    Vec3 ab = Vec3(a)-Vec3(b);
		//    return (ab[0]*ab[0]+ab[1]*ab[1]+ab[2]*ab[2]);
		return ((a[0] - b[0])*(a[0] - b[0])
			+ (a[1] - b[1])*(a[1] - b[1]) + (a[2] - b[2])*(a[2] - b[2]));
	}

	// Calculate the distance between 2 points
	float VecDistance(const float a[3], const float b[3])
	{

		return sqrt((a[0] - b[0])*(a[0] - b[0])
			+ (a[1] - b[1])*(a[1] - b[1]) + (a[2] - b[2])*(a[2] - b[2]));
	}

	void VecFixEulerAngles(float angle[3])
	{
		if (IsAlmostEqual(fabs(angle[0]), 180.0f) && IsAlmostEqual(fabs(angle[2]), 180.0f) && gvSign(angle[0]) == gvSign(angle[2]))
		{
			angle[0] = angle[2] = 0.0f;
			angle[1] = 180.0f - angle[1];
			//if( angle[1]<0.0f ) angle[1] += 360.0f;
		}
		//for( int i=0; i<3; i++ ) if( angle[i]<0.0f ) angle[i] += 360.0f;
	}

	void VecGetEuler(float angle, const float ax[3], float euler_angles[3])
	{

		float m[16];
		memset(m, 0, 16 * sizeof(float));
		float x_angle, y_angle, z_angle;

		Vec3 axis(ax);
		VecNormalize(axis);

		angle *= DS_DEG2RAD;
		float cos_a = cos(angle); float sin_a = sin(angle);
		// have a look at the MatMakeEuler and MatMakeRotation functions to understand why it works this way
		m[0] = 1 + (1 - cos_a) * (axis[0] * axis[0] - 1);
		m[1] = axis[2] * sin_a + (1 - cos_a) * axis[0] * axis[1];
		m[2] = -axis[1] * sin_a + (1 - cos_a) * axis[0] * axis[2];
		m[4] = -axis[2] * sin_a + (1 - cos_a) * axis[0] * axis[1];
		m[5] = 1 + (1 - cos_a) * (axis[1] * axis[1] - 1);
		m[6] = axis[0] * sin_a + (1 - cos_a) * axis[1] * axis[2];
		m[10] = 1 + (1 - cos_a) * (axis[2] * axis[2] - 1);

		// m[2] is - sin_y - C is cos(y_angle)
		y_angle = asin(-m[2]); float C = cos(y_angle);

		// if C > 0 (and a delta) there's no gimbal lock
		if (fabs(C) > 0.001)
		{

			// m[6] is C * sin_x, m[10] is C * cos_x
			x_angle = atan2(m[6] / C, m[10] / C);

			// m[0] is C * cos_z, m[1] is C * sin_z
			z_angle = atan2(m[1] / C, m[0] / C);

			// otherwise there's a gimbal lock..
		}
		else
		{

			// we take x = 0, we use z for the rotation
			x_angle = 0;

			z_angle = atan2(m[4], m[5]);

		}

		x_angle *= DS_RAD2DEG;
		y_angle *= DS_RAD2DEG;
		z_angle *= DS_RAD2DEG;

		while (x_angle < 0.0f) x_angle += 360.0f;
		while (y_angle < 0.0f) y_angle += 360.0f;
		while (z_angle < 0.0f) z_angle += 360.0f;

		while (x_angle >= 360.0f) x_angle -= 360.0f;
		while (y_angle >= 360.0f) y_angle -= 360.0f;
		while (z_angle >= 360.0f) z_angle -= 360.0f;

		euler_angles[0] = x_angle;
		euler_angles[1] = y_angle;
		euler_angles[2] = z_angle;
	}


	void VecLookAtHP(const float pos[3], const float pnt[3], float rot[3])
	{

		float x = pnt[0] - pos[0];
		float y = pnt[1] - pos[1];
		float z = pnt[2] - pos[2];
		if (IsAlmostZero(x) && IsAlmostZero(y) && IsAlmostZero(z))
		{
			return;
		}
		if (IsAlmostZero(x) && IsAlmostZero(z))
		{
			rot[1] = 0; rot[0] = y > 0 ? 90.0f : -90.0f;
			return;
		}
		float h;
		if (x<0.0) h = 90.0f - DS_RAD2DEG*atan(z / x);
		else if (x>0.0) h = -90.0f - DS_RAD2DEG*atan(z / x);
		else h = z < 0 ? 0 : 180.0f;
		float d = sqrt(x*x + z*z);
		float p = DS_RAD2DEG*atan(y / d);

		rot[1] = h;
		rot[0] = p;
	}


	void VecLookAtPointAxisAngle(const Vec3& orig, const Vec3& dir, const Vec3& pnt, Vec3& axis, float& angle)
	{

		Vec3 op(pnt - orig); // vector from orig to pnt
		// calculate axis as cross product between the two vectors starting at orig
		axis = dir.Cross(op); // calculate axis as cross product between the two vectors starting at orig
		axis.Normalize();

		// calculate once the lengths of the vectors
		float d = dir.Length();
		float p = op.Length();

		// calculate the angle between the two vectors using the inverse cross product norm formula
		angle = (IsAlmostZero(d) || IsAlmostZero(p) ? 0.0f : DS_RAD2DEG*acos(1.0f / (d*p)));
	}


	void PositionToHpR(const float pos[3], float &h, float &p, float &r)
	{

		r = VecNorm(pos);
		float x = pos[0];
		float y = pos[1];
		float z = pos[2];
		if (r == 0)
			h = p = 0;
		else
		{
			h = (x == 0 && z == 0) ? 0 : DS_RAD2DEG*atan2(x, z);
			p = -DS_RAD2DEG*asin(y / r);//-90.0;
		}

	}


	void HpRToPosition(float h, float p, float dist, float pos[3])
	{

		float theta = DS_DEG2RAD*h;
		float phi = DS_DEG2RAD*(90.0f - p);
		float rho = dist;

		if (IsAlmostZero(phi))
		{
			pos[0] = rho*cos(theta);
			pos[1] = 0.0f;
			pos[2] = rho*sin(theta);
		}
		else
		{
			pos[0] = rho*sin(phi)*cos(theta);
			pos[1] = rho*sin(phi)*sin(theta);
			pos[2] = rho*cos(phi);
		}
	}



	//******************************************************************************
	// QUATERNIONS
	//******************************************************************************

	// multiplication between quaternions: result = q1 x q2
	Quat QuatMultiply(Quat q1, Quat q2)
	{

		Quat result;

		result.S = q1.S * q2.S - q1.V.X * q2.V.X - q1.V.Y * q2.V.Y - q1.V.Z * q2.V.Z;

		result.V.X = q1.S * q2.V.X + q2.S * q1.V.X + q1.V.Y * q2.V.Z - q1.V.Z * q2.V.Y;
		result.V.Y = q1.S * q2.V.Y + q2.S * q1.V.Y + q1.V.Z * q2.V.X - q1.V.X * q2.V.Z;
		result.V.Z = q1.S * q2.V.Z + q2.S * q1.V.Z + q1.V.X * q2.V.Y - q1.V.Y * q2.V.X;

		return result;
	}


	// makes a rotation quaternion given an angle and a generic axis
	void QuatFromAxisAngle(float axis[3], float angle, Quat &q)
	{
		// see http://www.gamedev.net/reference/articles/article1095.asp
	  //    If the axis of rotation is        (ax, ay, az)- must be a unit vector
	  //    and the angle is                  theta (radians)
	  //
	  //        w   =   cos(theta/2)
	  //        x   =   ax * sin(theta/2)
	  //        y   =   ay * sin(theta/2)
	  //        z   =   az * sin(theta/2)
	  //

		Vec3 a(axis);
		// If the axis is a zero vector (meaning there is no rotation), the quaternion should be set to the rotation identity quaternion.
		if (a.IsZero())
		{
			q.Reset();
			return;
		}
		// The axis must first be normalized.
		// a rotation quaternion is in the form: [cos(a), sin(a) * axis], where a = angle / 2
		angle = DS_DEG2RAD*angle / 2.0f;

		q.S = cos(angle);

		q.V = a * sin(angle);
	}


	void QuatFromEulerAngles(float angles[3], Quat &q)
	{
		// see http://www.gamedev.net/reference/articles/article1095.asp
		  //  If you have three Euler angles (rx, ry, rz), then you can form three independent quaternions
		//
		//    Qx = [ cos(rx/2), (sin(rx/2), 0, 0)]
		//    Qy = [ cos(ry/2), (0, sin(ry/2), 0)]
		//    Qz = [ cos(rz/2), (0, 0, sin(rz/2))]
		//
		//And the final quaternion is obtained by Qx * Qy * Qz.
		float hrx(DS_DEG2RAD*angles[0] / 2.0f), hry(DS_DEG2RAD*angles[1] / 2.0f), hrz(DS_DEG2RAD*angles[2] / 2.0f);
		Quat qx(cos(hrx), Vec3(sin(hrx), 0.0f, 0.0f));
		Quat qy(cos(hry), Vec3(0.0f, sin(hry), 0.0f));
		Quat qz(cos(hrz), Vec3(0.0f, 0.0f, sin(hrz)));
		//// Using NASA convention heading-pitch-roll
		q = qy*qx*qz;
		/*
		  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
		  double heading(DS_DEG2RAD*angles[1]);
		  double attitude(DS_DEG2RAD*angles[0]);
		  double bank(DS_DEG2RAD*angles[2]);
		  double C1 = cos(heading);
		  double C2 = cos(attitude);
		  double C3 = cos(bank);
		  double S1 = sin(heading);
		  double S2 = sin(attitude);
		  double S3 = sin(bank);

		  double w = sqrt(1.0 + C1 * C2 + C1*C3 - S1 * S2 * S3 + C2*C3) / 2.0;
		  q.S = w;

		  q.V.X = (C2 * S3 + C1 * S3 + S1 * S2 * C3) / (4.0 * w);
		  q.V.Y = (S1 * C2 + S1 * C3 + C1 * S2 * S3) / (4.0 * w);
		  q.V.Z = (-S1 * S3 + C1 * S2 * C3 + S2) /(4.0 * w);*/
	}


	Quat QuatFromMatrix(const float m[16])
	{

		Quat q;
		//const float M_EPSILON = (float) 1E-6;

		//float trace = m[0] + m[5] + m[10] + 1.0f;
		//if( trace > M_EPSILON ) {
		//  float s = 0.5f / sqrtf(trace);
		//  q.S = 0.25f / s;
		//  q.V.X = ( m[6] - m[9] ) * s;
		//  q.V.Y = ( m[8] - m[2] ) * s;
		//  q.V.Z = ( m[1] - m[4] ) * s;
		//} else {
		//  if ( m[0] > m[5] && m[0] > m[10] ) {
		//    float s = 2.0f * sqrtf( 1.0f + m[0] - m[5] - m[10]);
		//    q.S = (m[6] - m[9] ) / s;
		//    q.V.X = 0.25f * s;
		//    q.V.Y = (m[4] + m[1] ) / s;
		//    q.V.Z = (m[8] + m[2] ) / s;
		//  } else if (m[5] > m[10]) {
		//    float s = 2.0f * sqrtf( 1.0f + m[5] - m[0] - m[10]);
		//    q.S = (m[8] - m[2] ) / s;
		//    q.V.X = (m[4] + m[1] ) / s;
		//    q.V.Y = 0.25f * s;
		//    q.V.Z = (m[9] + m[6] ) / s;
		//  } else {
		//    float s = 2.0f * sqrtf( 1.0f + m[10] - m[0] - m[5] );
		//    q.S = (m[1] - m[4] ) / s;
		//    q.V.X = (m[8] + m[2] ) / s;
		//    q.V.Y = (m[9] + m[6] ) / s;
		//    q.V.Z = 0.25f * s;
		//  }
		//}
		//return q;


		//// Creates a quaternion from a rotation matrix.
		//// The algorithm used is from Allan and Mark Watt's "Advanced
		//// Animation and Rendering Techniques" (ACM Press 1992).

		//float s = 0.0f;
		//float q[4] = {0.0f};
		//float trace = m[0] + m[5] + m[10];

		//if (trace > DS_EPSILON) {

		//  s = sqrtf(trace + 1.0f);
		//  q[3] = s * 0.5f;
		//  s = 0.5f / s;
		//  q[0] = (m[9] - m[6]) * s;
		//  q[1] = (m[2] - m[8]) * s;
		//  q[2] = (m[4] - m[1]) * s;
		//} else {

		//  int nxt[3] = {1, 2, 0};
		//  int i = 0, j = 0, k = 0;

		//  if (m[5] > m[0])
		//      i = 1;

		//  if (m[10] > m[i+i*4])
		//      i = 2;

		//  j = nxt[i];
		//  k = nxt[j];
		//  s = sqrtf((m[i+i*4] - (m[j+j*4] + m[k+k*4])) + 1.0f);

		//  q[i] = s * 0.5f;
		//  s = 0.5f / s;
		//  q[3] = (m[j+k*4] - m[k+j*4]) * s;
		//  q[j] = (m[i+j*4] + m[j+i*4]) * s;
		//  q[k] = (m[i+k*4] + m[k+i*4]) * s;
		//}


		//q.S = sqrt(1.0 + m[0] + m[5] + m[10]) / 2.0;
		//double w4 = (4.0 * q.S);
		//q.V.X = (m[6] - m[9]) / w4 ;
		//q.V.Y = (m[8] - m[2]) / w4 ;
		//q.V.Z = (m[1] - m[4]) / w4 ;



		float trace = m[0] + m[5] + m[10];
		if (trace > DS_EPSILONf)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);

			q.S = 0.25f / s;
			q.V.X = (m[6] - m[9]) * s;
			q.V.Y = (m[8] - m[2]) * s;
			q.V.Z = (m[1] - m[4]) * s;
		}
		else
		{
			if (m[0] > m[5] && m[0] > m[10])
			{
				float s = 2.0f * sqrtf(1.0f + m[0] - m[5] - m[10]);
				q.S = (m[6] - m[9]) / s;
				q.V.X = 0.25f * s;
				q.V.Y = (m[4] + m[1]) / s;
				q.V.Z = (m[8] + m[2]) / s;
			}
			else if (m[5] > m[10])
			{
				float s = 2.0f * sqrtf(1.0f + m[5] - m[0] - m[10]);
				q.S = (m[8] - m[2]) / s;
				q.V.X = (m[4] + m[1]) / s;
				q.V.Y = 0.25f * s;
				q.V.Z = (m[9] + m[6]) / s;
			}
			else
			{
				float s = 2.0f * sqrtf(1.0f + m[10] - m[0] - m[5]);
				q.S = (m[1] - m[4]) / s;
				q.V.X = (m[8] + m[2]) / s;
				q.V.Y = (m[9] + m[6]) / s;
				q.V.Z = 0.25f * s;
			}
		}


		return q;
	}


	Mat4 QuatToMatrix(const Quat& q)
	{
		Mat4 m;
		MatMakeRotFromQuat(q, m);
		return m;
	}


	void QuatToAxisAngle(const Quat& quat, Vec3& axis, float& angle)
	{
		Quat q(quat);
		// see: http://www.gamedev.net/reference/articles/article1095.asp
		// If the axis of rotation is         (ax, ay, az)
		// and the angle is                   theta (radians)
		// then the                           angle= 2 * acos(w)
		//
		//           ax= x / scale
		//           ay= y / scale
		//           az= z / scale
		//
		// where scale = sqrt (x2 + y2 + z2)


		if (q.S > 1.0f) q.Normalize(); // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
		float scale = q.V.Length();
		if (scale < DS_EPSILONf)
		{ // test to avoid divide by zero, scale is always positive due to sqrt
	// if scale close to zero then direction of axis not important
			axis.Set(1, 0, 0);// = q.V/scale;
			angle = 0.0f;
		}
		else
		{
			axis = q.V / scale; // normalise axis
			angle = DS_RAD2DEG * 2.0f * acos(q.S);
		}
	}


	// Returns the euler angles given a quaternion
	void QuatToEulerAngles(const Quat& q, Vec3& euler_angles)
	{
		//float angle;
		//Vec3 axis;
		//QuatToAxisAngle( q, axis, angle );
		//VecGetEuler( angle, axis, euler_angles );

		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
		double test = q.V.X*q.V.Y + q.V.Z*q.S;
		if (test > 0.499)
		{ // singularity at north pole
			euler_angles.Y = DS_RAD2DEG * 2.0f * atan2(q.V.X, q.S);
			euler_angles.X = 90.0f;
			euler_angles.Z = 0;
			return;
		}
		if (test < -0.499)
		{ // singularity at south pole
			euler_angles.Y = DS_RAD2DEG * -2.0f * atan2(q.V.X, q.S);
			euler_angles.X = -90.0f;
			euler_angles.Z = 0;
			return;
		}
		double sqx = q.V.X*q.V.X;
		double sqy = q.V.Y*q.V.Y;
		double sqz = q.V.Z*q.V.Z;
		euler_angles.Y = DS_RAD2DEG * (float)atan2(2.0f*q.V.Y*q.S - 2.0f*q.V.X*q.V.Z, 1.0f - 2.0f*sqy - 2.0f*sqz);
		euler_angles.X = DS_RAD2DEG * (float)asin(2.0f*test);
		euler_angles.Z = DS_RAD2DEG * (float)atan2(2.0f*q.V.X*q.S - 2.0f*q.V.Y*q.V.Z, 1.0f - 2.0f*sqx - 2.0f*sqz);
	}


	void QuatRotate(float p[3], float axis[3], float angle, float result[3])
	{

		// 2 quaternions are needed for a rotation: q and its inverse one
		Quat q, q1, p1, p2;
		if ((axis[0] != 0 || axis[1] != 0 || axis[2] != 0) && angle != 0)
		{

			angle = angle*DS_DEG2RAD / -2;
			float sin_angle = sin(angle);

			q.S = cos(angle); q.V.X = sin_angle * axis[0]; q.V.Y = sin_angle * axis[1]; q.V.Z = sin_angle * axis[2];
			q1.S = q.S; q1.V.X = -q.V.X; q1.V.Y = -q.V.Y; q1.V.Z = -q.V.Z;

			p1.S = 0; p1.V.X = p[0]; p1.V.Y = p[1]; p1.V.Z = p[2];
			p2 = QuatMultiply(p1, q1); p1 = QuatMultiply(q, p2);
			result[0] = p1.V.X; result[1] = p1.V.Y; result[2] = p1.V.Z;
		}
	}


	Quat QuatNegate(Quat q)
	{

		Quat result;
		result[0] = -q[0];
		result[1] = -q[1];
		result[2] = -q[2];
		result[3] = q[3];

		return result;
	}


	Quat QuatInvert(Quat q)
	{

		float& x = q.V.X;
		float& y = q.V.Y;
		float& z = q.V.Z;
		float& w = q.S;
		float fNorm = w*w + x*x + y*y + z*z;
		if (fNorm > 0.0)
		{
			float fInvNorm = 1.0f / fNorm;
			return Quat(w*fInvNorm, Vec3(-x*fInvNorm, -y*fInvNorm, -z*fInvNorm));
		}

		// return an invalid result to flag the error

		return Quat(0, Vec3());
	}


	Quat QuatNormalize(const Quat& q)
	{

		Quat result;
		/*
		 * Quaternions always obey:  x^2 + y^2 + z^2 + w^2 = 1.0
		 * If they don't add up to 1.0, dividing by their magnitude will
		 * renormalize them.
		*/
		int i;
		float w;

		w = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
		// If the axis is a zero vector (meaning there is no rotation), the quaternion should be set to the rotation identity quaternion.
		if (IsAlmostZero(w))
		{
			result[0] = 1.0f;
			result[1] = 0.0f;
			result[2] = 0.0f;
			result[3] = 0.0f;
			return result;
		}
		for (i = 0; i < 4; i++) result[i] = q[i] / w;

		return result;
	}


	Quat QuatSlerp(const Quat& q1, float t, const Quat& q2)
	{

		// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
		Quat q;

		float dot = q1.V.Dot(q2.V) + q1.S*q2.S;
		const float threshold = 0.9995f;
		if (dot > threshold)
		{
			// If the inputs are too close for comfort, linearly interpolate
			// and normalize the result.

			q = q1 + (q2 - q1)*t;
			q.Normalize();
			return q;
		}
		gvClip(dot, -1.0f, 1.0f);           // Robustness: Stay within domain of acos()
		float theta_0 = acos(dot);  // theta_0 = angle between input vectors
		float theta = theta_0*t;    // theta = angle between v0 and result 

		Quat q0(q2 - q1*dot);
		q0.Normalize();              // { v0, v2 } is now an orthonormal basis

		q = q1*cos(theta) + q0*sin(theta);
		return q;
	}

	//******************************************************************************
	// BOXES
	//******************************************************************************


	void ResetBoundingBox(BoundingBox &b)
	{

		memset(&b, 0, sizeof(BoundingBox));
		b.Valid = false;

	}


	bool SetBoundingBox(BoundingBox &b, float min_x, float max_x, float min_y, float max_y, float min_z, float max_z)
	{

		if (max_x < min_x) std::swap(max_x, min_x);
		if (max_y < min_y) std::swap(max_y, min_y);
		if (max_z < min_z) std::swap(max_z, min_z);

		b.xyz = Vec3(min_x, min_y, min_z);
		b.xyZ = Vec3(min_x, min_y, max_z);
		b.xYz = Vec3(min_x, max_y, min_z);
		b.xYZ = Vec3(min_x, max_y, max_z);
		b.Xyz = Vec3(max_x, min_y, min_z);
		b.XyZ = Vec3(max_x, min_y, max_z);
		b.XYz = Vec3(max_x, max_y, min_z);
		b.XYZ = Vec3(max_x, max_y, max_z);

		b.Center.X = (min_x + max_x) / 2.0f;
		b.Center.Y = (min_y + max_y) / 2.0f;
		b.Center.Z = (min_z + max_z) / 2.0f;

		b.Size.X = max_x - min_x;
		b.Size.Y = max_y - min_y;
		b.Size.Z = max_z - min_z;

		b.Valid = true;

		return true;

	}


	void GetBoundingBoxValues(const BoundingBox &bb, float &x_min, float &x_max, float &y_min, float &y_max, float &z_min, float &z_max)
	{

		BoundingBox &b = (BoundingBox&)bb;
		x_min = x_max = b.xyz.X;
		y_min = y_max = b.xyz.Y;
		z_min = z_max = b.xyz.Z;
		int i;
		for (i = 0; i < 8; i++)
		{
			if (b[i].X < x_min) x_min = b[i].X;
			if (b[i].X > x_max) x_max = b[i].X;
			if (b[i].Y < y_min) y_min = b[i].Y;
			if (b[i].Y > y_max) y_max = b[i].Y;
			if (b[i].Z < z_min) z_min = b[i].Z;
			if (b[i].Z > z_max) z_max = b[i].Z;
		}
	}


	void GetBoundingBoxCenter(BoundingBox &bb, float p[3])
	{

		Vec3 v;
		for (int i = 0; i < 8; i++) v += bb[i];
		v /= 8.0f;
		p[0] = v.X;
		p[1] = v.Y;
		p[2] = v.Z;
	}


	void GetBoundingBoxSize(BoundingBox &bb, float p[3])
	{

		float x_min, x_max, y_min, y_max, z_min, z_max;
		GetBoundingBoxValues(bb, x_min, x_max, y_min, y_max, z_min, z_max);
		p[0] = fabs(x_max - x_min);
		p[1] = fabs(y_max - y_min);
		p[2] = fabs(z_max - z_min);
	}


	bool IsVec3InBox(Vec3 p, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
	{

		return ((p.X >= x_min && p.X <= x_max) && (p.Y >= y_min && p.Y <= y_max) && (p.Z >= z_min && p.Z <= z_max));

	}


	bool AreRangesIntersecting(float x_min_1, float x_max_1, float y_min_1, float y_max_1, float z_min_1, float z_max_1,
		float x_min_2, float x_max_2, float y_min_2, float y_max_2, float z_min_2, float z_max_2)
	{

		return ((((x_min_2 <= x_max_1) && (x_min_2 >= x_min_1)) || ((x_max_2 >= x_min_1) && (x_max_2 <= x_max_1)) ||
			((x_min_1 <= x_max_2) && (x_min_1 >= x_min_2)) || ((x_max_1 >= x_min_2) && (x_max_1 <= x_max_2))) &&
			(((y_min_2 <= y_max_1) && (y_min_2 >= y_min_1)) || ((y_max_2 >= y_min_1) && (y_max_2 <= y_max_1)) ||
				((y_min_1 <= y_max_2) && (y_min_1 >= y_min_2)) || ((y_max_1 >= y_min_2) && (y_max_1 <= y_max_2))) &&
			(((z_min_2 <= z_max_1) && (z_min_2 >= z_min_1)) || ((z_max_2 >= z_min_1) && (z_max_2 <= z_max_1)) ||
				((z_min_1 <= z_max_2) && (z_min_1 >= z_min_2)) || ((z_max_1 >= z_min_2) && (z_max_1 <= z_max_2))));

	}


	// Axis Aligned Bounding Box (AABB) structure
	//*********************************************************************

	AABBox::AABBox()
	{
		Reset();
	}


	AABBox::AABBox(const AABBox& bb)
	{
		memcpy(this, &bb, sizeof(AABBox));
	}


	void AABBox::Reset()
	{
		Center = Size = Vec3(0, 0, 0);
		Xmin = Xmax = Ymin = Ymax = Zmin = Zmax = 0;
		Valid = false;
	}


	void AABBox::Set(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
	{

		Xmin = x_min;
		Xmax = x_max;
		Ymin = y_min;
		Ymax = y_max;
		Zmin = z_min;
		Zmax = z_max;

		Update();
	}


	void AABBox::Set(Vec3 min_pnt, Vec3 max_pnt)
	{

		Xmin = min_pnt.X;
		Xmax = max_pnt.X;
		Ymin = min_pnt.Y;
		Ymax = max_pnt.Y;
		Zmin = min_pnt.Z;
		Zmax = max_pnt.Z;

		Update();
	}


	void AABBox::Constrain(Vec3& pnt)
	{
		if (!Valid)
		{
			return;
		}
		if (pnt.X < Xmin) pnt.X = Xmin;
		if (pnt.X > Xmax) pnt.X = Xmax;
		if (pnt.Y < Ymin) pnt.Y = Ymin;
		if (pnt.Y > Ymax) pnt.Y = Ymax;
		if (pnt.Z < Xmin) pnt.Z = Zmin;
		if (pnt.Z > Xmax) pnt.Z = Zmax;
	}


	bool AABBox::Including(Vec3 pnt)
	{
		// if the box is not valid no constraints are defined
		if (!Valid)
		{
			return true;
		}
		if (pnt.X<Xmin || pnt.X>Xmax
			|| pnt.Y<Ymin || pnt.Y>Ymax
			|| pnt.Z<Zmin || pnt.Z>Zmax)
		{
			return false;
		}
		return true;
	}


	bool AABBox::IncludingBox(AABBox bb)
	{
		// if the box is not valid no constraints are defined
		if (!Valid || !bb.Valid)
		{
			return true;
		}
		if (bb.Xmin<Xmin || bb.Xmax>Xmax
			|| bb.Ymin<Ymin || bb.Ymax>Ymax
			|| bb.Zmin<Zmin || bb.Zmax>Zmax)
		{
			return false;
		}
		return true;
	}


	void AABBox::IncludePoint(Vec3 pnt)
	{

		if (!Valid)
		{
			Xmin = Xmax = pnt.X;
			Ymin = Ymax = pnt.Y;
			Zmin = Zmax = pnt.Z;
			Update();
			return;
		}
		if (pnt.X < Xmin) Xmin = pnt.X;
		if (pnt.Y < Ymin) Ymin = pnt.Y;
		if (pnt.Z < Zmin) Zmin = pnt.Z;
		if (pnt.X > Xmax) Xmax = pnt.X;
		if (pnt.Y > Ymax) Ymax = pnt.Y;
		if (pnt.Z > Zmax) Zmax = pnt.Z;
		Update();
	}


	void AABBox::EmbodyBox(const AABBox& bb)
	{
		if (!bb.Valid)
		{
			return;
		}
		// if this is not defined set this = bb
		if (!Valid)
		{
			*this = bb;
			return;
		}
		if (bb.Xmin < Xmin) Xmin = bb.Xmin;
		if (bb.Xmax > Xmax) Xmax = bb.Xmax;
		if (bb.Ymin < Ymin) Ymin = bb.Ymin;
		if (bb.Ymax > Ymax) Ymax = bb.Ymax;
		if (bb.Zmin < Zmin) Zmin = bb.Zmin;
		if (bb.Zmax > Zmax) Zmax = bb.Zmax;
	}


	void AABBox::SetSize(Vec3 s)
	{
		SetCenterSize(Center, s);
	}


	void AABBox::SetCenter(Vec3 c)
	{
		SetCenterSize(c, Size);
	}


	void AABBox::SetCenterSize(Vec3 c, Vec3 s)
	{
		Center = c;
		Size = s;
		Vec3 hs = s*0.5f;
		Xmin = c.X - hs.X;
		Xmax = c.X + hs.X;
		Ymin = c.Y - hs.Y;
		Ymax = c.Y + hs.Y;
		Zmin = c.Z - hs.Z;
		Zmax = c.Z + hs.Z;
		Valid = true;
	}


	void AABBox::Update()
	{

		if (Xmin > Xmax) std::swap(Xmin, Xmax);
		if (Ymin > Ymax) std::swap(Ymin, Ymax);
		if (Zmin > Zmax) std::swap(Zmin, Zmax);
		Origin = Vec3(Xmin, Ymin, Zmin);
		Center = Vec3((Xmin + Xmax)*0.5f, (Ymin + Ymax)*0.5f, (Zmin + Zmax)*0.5f);
		Size = Vec3(fabs(Xmax - Xmin), fabs(Ymax - Ymin), fabs(Zmax - Zmin));
		MaxSize = 0;
		for (int i = 0; i < 3; i++)
		{
			if (MaxSize < Size[i])
			{
				MaxSize = Size[i];
			}
		}
		Valid = true;
	}


	float AABBox::operator[] (int idx)
	{
		switch (idx)
		{
		case 0: return Xmin;
		case 1: return Xmax;
		case 2: return Ymin;
		case 3: return Ymax;
		case 4: return Zmin;
		case 5: return Zmax;
		}
		return 0;
	}


	AABBox& AABBox::operator =(const AABBox& bb)
	{
		memcpy(this, &bb, sizeof(AABBox));
		return *this;
	}


	void AABBox::SetSizeOffsetAlignment(Vec3 size, Vec3 offset, int align)
	{
		SetCenterSize(size / 2.0f, size);

		Vec3 align_offset = -GetAlignPos(align);

		Vec3 min_corner(offset + align_offset);
		Vec3 max_corner(min_corner + size);

		Xmin = min_corner.X;
		Xmax = max_corner.X;
		Ymin = min_corner.Y;
		Ymax = max_corner.Y;
		Zmin = min_corner.Z;
		Zmax = max_corner.Z;

		Update();
	}


	Vec3 AABBox::GetAlignPos(int align)
	{

		Update();
		Vec3 align_offset;

		switch (align&DS_XALIGN)
		{
		case DS_XALIGN_LEFT:   align_offset.X = Xmin; break;
		case DS_XALIGN_CENTER: align_offset.X = Center.X; break;
		case DS_XALIGN_RIGHT:  align_offset.X = Xmax; break;
		}
		switch (align&DS_YALIGN)
		{
		case DS_YALIGN_BOTTOM: align_offset.Y = Ymin; break;
		case DS_YALIGN_CENTER: align_offset.Y = Center.Y; break;
		case DS_YALIGN_TOP:    align_offset.Y = Ymax; break;
		}
		switch (align&DS_ZALIGN)
		{
		case DS_ZALIGN_FRONT:  align_offset.Z = Zmin; break;
		case DS_ZALIGN_CENTER: align_offset.Z = Center.Z; break;
		case DS_ZALIGN_BACK:   align_offset.Z = Zmax; break;
		}
		return align_offset;
	}


	// bounding box structure
	//*********************************************************************

	BoundingBox::BoundingBox()
	{
		Reset();
	}


	void BoundingBox::Reset()
	{
		Center = Size = xyz = xyZ = xYz = xYZ = Xyz = XyZ = XYz = XYZ = Vec3(0, 0, 0);
		MaxSize = 0;
		Valid = false;
	}


	void BoundingBox::Set(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
	{
		SetBoundingBox(*this, x_min, x_max, y_min, y_max, z_min, z_max);
		Update();
	}


	void BoundingBox::Update()
	{
		Valid = false;
		GetBoundingBoxCenter(*this, Center);
		GetBoundingBoxSize(*this, Size);
		MaxSize = 0;
		for (int i = 0; i < 3; i++)
			if (MaxSize < Size[i])
				MaxSize = Size[i];
		Valid = true;
	}


	Vec3& BoundingBox::GetCenter()
	{
		GetBoundingBoxCenter(*this, Center);
		return Center;
	}


	AABBox BoundingBox::GetAABB() const
	{
		AABBox aabb;
		GetBoundingBoxValues(*this, aabb.Xmin, aabb.Xmax, aabb.Ymin, aabb.Ymax, aabb.Zmin, aabb.Zmax);
		aabb.Update();
		return aabb;
	}


	void BoundingBox::EmbodyBox(const BoundingBox& bb)
	{
		if (!bb.Valid)
		{
			return;
		}
		if (!Valid)
		{
			*this = bb;
			return;
		}
		AABBox aabb = GetAABB();
		AABBox aabb2 = bb.GetAABB();
		aabb.EmbodyBox(aabb2);
		*this = aabb;
	}


	void BoundingBox::WrapPoints(Vec3* points, int count)
	{

		if (points == nullptr || count == 0)
		{
			Reset();
			return;
		}

		// values from first vertex
		float min_x = points[0].X;
		float max_x = points[0].X;
		float min_y = points[0].Y;
		float max_y = points[0].Y;
		float min_z = points[0].Z;
		float max_z = points[0].Z;

		for (int i = 0; i < count; i++)
		{

			Vec3& v = points[i];

			if (v.X < min_x) min_x = v.X;
			else if (v.X > max_x) max_x = v.X;

			if (v.Y < min_y) min_y = v.Y;
			else if (v.Y > max_y) max_y = v.Y;

			if (v.Z < min_z) min_z = v.Z;
			else if (v.Z > max_z) max_z = v.Z;
		}
		Set(min_x, max_x, min_y, max_y, min_z, max_z);
		Update();
	}


	void BoundingBox::IncludePoints(Vec3* points, int count, Mat4* matrix)
	{

		if (points == nullptr || count == 0)
		{
			return;
		}

		Vec3 v;
		if (matrix) v = *matrix * points[0];
		else v = points[0];
		// values from first vertex
		float min_x = v.X;
		float max_x = v.X;
		float min_y = v.Y;
		float max_y = v.Y;
		float min_z = v.Z;
		float max_z = v.Z;
		if (Valid)
		{
			Update();
			Vec3 hsiz(Size / 2.0f);
			min_x = Center.X - hsiz.X;
			max_x = Center.X + hsiz.X;
			min_y = Center.Y - hsiz.Y;
			max_y = Center.Y + hsiz.Y;
			min_z = Center.Z - hsiz.Z;
			max_z = Center.Z + hsiz.Z;
		}
		for (int i = 0; i < count; i++)
		{
			if (matrix) v = *matrix * points[i];
			else v = points[i];

			if (v.X < min_x) min_x = v.X;
			else if (v.X > max_x) max_x = v.X;

			if (v.Y < min_y) min_y = v.Y;
			else if (v.Y > max_y) max_y = v.Y;

			if (v.Z < min_z) min_z = v.Z;
			else if (v.Z > max_z) max_z = v.Z;
		}
		Set(min_x, max_x, min_y, max_y, min_z, max_z);
		Update();
	}


	Vec3& BoundingBox::operator[] (int idx)
	{
		switch (idx)
		{
		case 0: return xyz;
		case 1: return xyZ;
		case 2: return xYz;
		case 3: return xYZ;
		case 4: return Xyz;
		case 5: return XyZ;
		case 6: return XYz;
		case 7: return XYZ;
		}
		return Center;
	}


	BoundingBox& BoundingBox::operator =(const BoundingBox& bb)
	{
		memcpy(this, &bb, sizeof(BoundingBox));
		return *this;
	}


	BoundingBox& BoundingBox::operator =(const AABBox& aabb)
	{
		xyz = Vec3(aabb.Xmin, aabb.Ymin, aabb.Zmin);
		xyZ = Vec3(aabb.Xmin, aabb.Ymin, aabb.Zmax);
		xYz = Vec3(aabb.Xmin, aabb.Ymax, aabb.Zmin);
		xYZ = Vec3(aabb.Xmin, aabb.Ymax, aabb.Zmax);
		Xyz = Vec3(aabb.Xmax, aabb.Ymin, aabb.Zmin);
		XyZ = Vec3(aabb.Xmax, aabb.Ymin, aabb.Zmax);
		XYz = Vec3(aabb.Xmax, aabb.Ymax, aabb.Zmin);
		XYZ = Vec3(aabb.Xmax, aabb.Ymax, aabb.Zmax);
		return *this;
	}


	bool BoundingBox::operator ==(const BoundingBox& bb)
	{
		return memcmp(this, &bb, sizeof(BoundingBox)) == 0;
	}


	// Bounding sphere
	//*********************************************************************

	BoundingSphere::BoundingSphere()
	{
		Radius = 0.0f;
	}


	BoundingSphere::BoundingSphere(const BoundingBox& bb)
	{
		*this = bb;
	}


	BoundingSphere::BoundingSphere(Vec3 c, float r)
	{
		Center = c;
		Radius = r;
	}


	BoundingSphere& BoundingSphere::EmbodyBoundingSphere(const BoundingSphere& bs)
	{
		if (bs.Radius < 0)
		{
			return *this;
		}
		if (Radius < 0)
		{
			*this = bs;
			return *this;
		}
		Vec3 dist = bs.Center - Center;
		Vec3 start = Center - dist.Normalized()*Radius;
		Vec3 end = bs.Center + dist.Normalized()*bs.Radius;
		Center = (start + end)*0.5f;
		Radius = (end - start).Length()*0.5f;
		return *this;
	}


	void BoundingSphere::Reset()
	{
		Center = Vec3();
		Radius = 0.0f;
	}


	void BoundingSphere::IncludePoints(Vec3* points, int count)
	{
		//Radius = 0;

		for (int i = 0; i < count; i++)
		{
			float r = Center.Distance(points[i]);

			if (Radius < r)
			{
				Radius = r;
			}
		}
	}


	void BoundingSphere::WrapPoints(Vec3* points, int count)
	{

		if (points == nullptr || count == 0)
		{
			Reset();
			return;
		}

		BoundingBox bb;
		bb.WrapPoints(points, count);
		Center = bb.Center;

		IncludePoints(points, count);
	}


	//******************************************************************************
	// PATH
	//******************************************************************************

	PosRot& PosRotPath::operator[] (unsigned int idx)
	{
		if (idx >= WayPoint.size())
		{
			WayPoint.resize(idx + 1);
		}
		return WayPoint[idx];
	}


	float PosRotPath::Distance() const
	{
		float dist = 0.0f;
		for (size_t i = 0; i + 1 < WayPoint.size(); i++)
		{
			dist += WayPoint[i].Pos.Distance(WayPoint[i + 1].Pos);
		}
		return dist;
	}


	void PosRotPath::Insert(const PosRot& p, unsigned int idx)
	{
		WayPoint.insert(WayPoint.begin() + idx, p);
	}


	void PosRotPath::Remove(unsigned int idx)
	{
		WayPoint.erase(WayPoint.begin() + idx);
	}


	void PosRotPath::Reset()
	{
		WayPoint.clear();
	}

	//******************************************************************************
	// MATRICES
	//******************************************************************************

	// TODO: "","GPV","test ConvertZupYup/ConvertZupYup implementation in Mat4"

	bool MatIsIdentity(const float m[16])
	{
		//float id[16];
		//MatMakeIdentity(id);
		//return memcmp(m,id,16*sizeof(float)) == 0;
#define IS_0_(x) IsAlmostZero(x)
#define IS_1_(x) IsAlmostEqual(x,1.0f)
		return IS_1_(m[ 0]) && IS_0_(m[ 1]) && IS_0_(m[ 2]) && IS_0_(m[ 3])
			&& IS_0_(m[ 4]) && IS_1_(m[ 5]) && IS_0_(m[ 6]) && IS_0_(m[ 7])
			&& IS_0_(m[ 8]) && IS_0_(m[ 9]) && IS_1_(m[10]) && IS_0_(m[11])
			&& IS_0_(m[12]) && IS_0_(m[13]) && IS_0_(m[14]) && IS_1_(m[15]);
#undef IS_0_
#undef IS_1_
	}


	void MatMakeIdentity(float m[16])
	{
		memset(m, 0, 16 * sizeof(float));
		m[0] = m[5] = m[10] = m[15] = 1.0;
	}


	void MatMakeTranslation(float t[3], float m[16])
	{
		MatMakeIdentity(m);
		m[12] = t[0]; m[13] = t[1]; m[14] = t[2];
	}


	void MatMakeScaling(float s[3], float m[16])
	{
		memset(m, 0, 16 * sizeof(float));
		m[0] = s[0]; m[5] = s[1]; m[10] = s[2]; m[15] = 1.0;
	}


	void MatMakeEuler(Vec3 angles, float m[16])
	{
		MatMakeIdentity(m);
		MatSetRotation(m, angles);
	}


	struct gvRotMatAngle
	{
		float Angle[3];
		float MatCol[3][3];
	};


	const gvRotMatAngle MatAngleLookUpTable[] = {

		// identity
		  { {  0.0f,  0.0f,  0.0f},  { { 1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },

		  // rotations around Y
			{ {  0.0f, 90.0f,  0.0f},  { { 0.0f, 0.0f,-1.0f}, { 0.0f, 1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f} } },
			{ {  0.0f,-90.0f,  0.0f},  { { 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} } },
			{ {  0.0f,180.0f,  0.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f}, { 0.0f, 0.0f,-1.0f} } },

		// rotations around X
		  { { 90.0f,  0.0f,  0.0f},  { { 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { 0.0f,-1.0f, 0.0f} } },
		  { {-90.0f,  0.0f,  0.0f},  { { 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f,-1.0f}, { 0.0f, 1.0f, 0.0f} } },
		  { {180.0f,  0.0f,  0.0f},  { { 1.0f, 0.0f, 0.0f}, { 0.0f,-1.0f, 0.0f}, { 0.0f, 0.0f,-1.0f} } },

		// rotations around Z
		  { {  0.0f,  0.0f, 90.0f},  { { 0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },
		  { {  0.0f,  0.0f,-90.0f},  { { 0.0f,-1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },
		  { {  0.0f,  0.0f,180.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f,-1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },

		// rotations around X and Y
		  { { 90.0f, 90.0f,  0.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f,-1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },
		  { { 90.0f,-90.0f,  0.0f},  { { 0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, { 0.0f,-1.0f, 0.0f} } },
		  { { 90.0f,180.0f,  0.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f,-1.0f}, { 0.0f,-1.0f, 0.0f} } },
		  { {-90.0f, 90.0f,  0.0f},  { { 0.0f, 0.0f,-1.0f}, {-1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f} } },
		  { {-90.0f,-90.0f,  0.0f},  { { 0.0f, 0.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f} } },
		  { {-90.0f,180.0f,  0.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f, 0.0f} } },
		  { {180.0f, 90.0f,  0.0f},  { { 0.0f, 0.0f,-1.0f}, { 0.0f,-1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} } },
		  { {180.0f,-90.0f,  0.0f},  { { 0.0f, 0.0f, 1.0f}, { 0.0f,-1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f} } },
		  { {180.0f,180.0f,  0.0f},  { {-1.0f, 0.0f, 0.0f}, { 0.0f,-1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f} } },

	};


	bool MatAngleLookUp(const float m[16], Vec3* angles)
	{

		int i, j, k;
		bool found;
		int n = sizeof(MatAngleLookUpTable) / sizeof(gvRotMatAngle);
		for (i = 0; i < n; i++)
		{
			found = true;
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					if (!IsAlmostEqual(m[4 * j + k], MatAngleLookUpTable[i].MatCol[j][k]))
					{
						found = false;
					}
					if (!found)
					{
						break;
					}
				}
				if (!found)
				{
					break;
				}
			}

			if (found)
			{
				if (angles) memcpy(&(angles->X), MatAngleLookUpTable[i].Angle, 3 * sizeof(float));
				return true;
			}
		}
		return false;
	}


	// have a look at the MatMakeEuler function to understand why this group of functions works in this way

	Vec3 MatGetEuler(const float m[16])
	{
		Vec3 angle;

		// try to use lookup table to avoid singularities
		if (MatAngleLookUp(m, &angle))
		{
			return angle;
		}

		// manage some singular cases:

		//  0  (0) -1
		//  0  (1)  0
		// [1]  0   0
		// y=-90, x=z=0

		//  0  (0)  1
		//  0  (1)  0
		//[-1]  0   0
		// y=90, x=z=0


		// m[2] is -sin_y, C is cos(angle.Y)
		angle.Y = asin(-m[2]); float Cy = cos(angle.Y);

		// if Cy > 0 (and a delta) there's no gimbal lock
		if (fabs(Cy) > 0.001)
		{

			float Cx = m[10] / Cy;
			float Cz = m[0] / Cy;

			// m[6] is Cy * sin_x, m[10] is Cy * cos_x
			angle.X = atan2(m[6] / Cy, m[10] / Cy);

			// m[0] is Cy * cos_z, m[1] is Cy * sin_z
			angle.Z = atan2(m[1] / Cy, m[0] / Cy);

			// otherwise there's a gimbal lock..
		}
		else
		{
			// we can combine a rotation along the Y axis with a rotation along the X axis
			// sy==+-1 rot_y=+-90
			// let's set rot_z=0, thus cz=1 sz=0

			//// First column
			//mat[0]=0;
			//mat[1]=0;
			//mat[2]=-+1;

			//// Second column
			//mat[4]=+-sx;
			//mat[5]= cx;
			//mat[6]=0;

			//// Third column
			//mat[8]=+-cx;
			//mat[9]=-sx;
			//mat[10]=0;

			float sx;
			float cx;
			if (m[2] < 0.0)
			{
				// sy==+1 rot_y=90

				//mat[4]=+sx;
				//mat[5]= cx;
				sx = m[4];
				cx = m[5];

				//mat[8]=+cx;
				//mat[9]=-sx;

				angle.Y = DS_HPI;
			}
			else
			{
				// sy==-1 rot_y=-90

				//mat[4]=-sx;
				//mat[5]= cx;
				sx = -m[4];
				cx = m[5];

				//mat[8]=-cx;
				//mat[9]=-sx;

				angle.Y = -DS_HPI;
			}
			angle.X = asin(sx);

			// asin() returns an angle in the range -pi/2,pi/2
			// find the correct angle checking the quadrant
			if (sx >= 0.0f)
			{
				if (cx < 0.0f)
					angle.X = DS_PI - angle.X;
				// else ok
			}
			else
			{
				if (cx < 0.0f)
					angle.X = -DS_PI - angle.X;
				// else ok
			}
		}

		//while (angle.X < 0) angle.X += 2 * DS_PI;
		//while (angle.Y < 0) angle.Y += 2 * DS_PI;
		//while (angle.Z < 0) angle.Z += 2 * DS_PI;

		//while (angle.X >= DS_PI) angle.X -= 2 * DS_PI;
		//while (angle.Y >= DS_PI) angle.Y -= 2 * DS_PI;
		//while (angle.Z >= DS_PI) angle.Z -= 2 * DS_PI;

		angle.X *= DS_RAD2DEG;
		angle.Y *= DS_RAD2DEG;
		angle.Z *= DS_RAD2DEG;

		VecFixEulerAngles(angle);

		return angle;
	}


	Vec4 MatGetAxisAngle(const float m[16])
	{
		double angle, x, y, z; // variables for result
		double epsilon = 0.01; // margin to allow for rounding errors
		double epsilon2 = 0.1; // margin to distinguish between 0 and 180 degrees
		// (put optional check that input is pure rotation here)
		if ((fabs(m[4] - m[1]) < epsilon)
			&& (fabs(m[8] - m[2]) < epsilon)
			&& (fabs(m[9] - m[6]) < epsilon))
		{
			// singularity found
			// first check for identity matrix which must have +1 for all terms
			//  in leading diagonaland zero in other terms
			if ((fabs(m[4] + m[1]) < epsilon2)
				&& (fabs(m[8] + m[2]) < epsilon2)
				&& (fabs(m[9] + m[6]) < epsilon2)
				&& (fabs(m[0] + m[5] + m[10] - 3) < epsilon2))
			{
				// this singularity is identity matrix so angle = 0
				return Vec4(1, 0, 0, 0); // zero angle, arbitrary axis
			}
			// otherwise this singularity is angle = 180
			angle = M_PI;
			double xx = (m[0] + 1) / 2;
			double yy = (m[5] + 1) / 2;
			double zz = (m[10] + 1) / 2;
			double xy = (m[4] + m[1]) / 4;
			double xz = (m[8] + m[2]) / 4;
			double yz = (m[9] + m[6]) / 4;
			if ((xx > yy) && (xx > zz))
			{ // m[0] = m(0,0) is the largest diagonal term
				if (xx< epsilon)
				{
					x = 0;
					y = 0.7071;
					z = 0.7071;
				}
				else
				{
					x = sqrt(xx);
					y = xy / x;
					z = xz / x;
				}
			}
			else if (yy > zz)
			{ // m[5] = m(1,1) is the largest diagonal term
				if (yy < epsilon)
				{
					x = 0.7071;
					y = 0;
					z = 0.7071;
				}
				else
				{
					y = sqrt(yy);
					x = xy / y;
					z = yz / y;
				}
			}
			else
			{ // m[10] = m(2,2) is the largest diagonal term so base result on this
				if (zz < epsilon)
				{
					x = 0.7071;
					y = 0.7071;
					z = 0;
				}
				else
				{
					z = sqrt(zz);
					x = xz / z;
					y = yz / z;
				}
			}
			return Vec4((float)x, (float)y, (float)z, (float)angle); // return 180 deg rotation
		}
		// as we have reached here there are no singularities so we can handle normally
		double s = sqrt((m[6] - m[9])*(m[6] - m[9])
			+ (m[8] - m[2])*(m[8] - m[2])
			+ (m[1] - m[4])*(m[1] - m[4])); // used to normalise
		if (fabs(s) < 0.001) s = 1;
		// prevent divide by zero, should not happen if matrix is orthogonal and should be
		// caught by singularity test above, but I've left it in just in case
		angle = acos((m[0] + m[5] + m[10] - 1) / 2);
		x = (m[6] - m[9]) / s;
		y = (m[8] - m[2]) / s;
		z = (m[1] - m[4]) / s;
		return Vec4((float)x, (float)y, (float)z, DS_RAD2DEG*(float)angle);
	}


	void MatMakeRotFromQuat(const Quat& q, float m[16], bool reset_translation)
	{
		double w2 = q.S*q.S;
		double x2 = q.V.X*q.V.X;
		double y2 = q.V.Y*q.V.Y;
		double z2 = q.V.Z*q.V.Z;

		double den = x2 + y2 + z2 + w2;
		if (IsAlmostZero(den))
		{
			// degenerate quaternion, make no rotation
			m[0] = m[5] = m[10] = m[15] = 1.0;
			m[1] = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0;
		}
		else
		{
			// invSL (inverse square length) is only required if quaternion is not already normalised
			double invSL = 1 / den;
			m[0] = (float)((x2 - y2 - z2 + w2)*invSL); // since w2 + x2 + y2 + z2 =1/invSL*invSL
			m[5] = (float)((-x2 + y2 - z2 + w2)*invSL);
			m[10] = (float)((-x2 - y2 + z2 + w2)*invSL);

			double xy = q.V.X*q.V.Y;
			double zs = q.V.Z*q.S;
			m[1] = (float)(2.0 * (xy + zs)*invSL);
			m[4] = (float)(2.0 * (xy - zs)*invSL);

			double xz = q.V.X*q.V.Z;
			double ys = q.V.Y*q.S;
			m[2] = (float)(2.0 * (xz - ys)*invSL);
			m[8] = (float)(2.0 * (xz + ys)*invSL);
			double yz = q.V.Y*q.V.Z;
			double xs = q.V.X*q.S;
			m[6] = (float)(2.0 * (yz + xs)*invSL);
			m[9] = (float)(2.0 * (yz - xs)*invSL);
		}


		if (reset_translation)
		{
			m[3] = 0.0f;
			m[7] = 0.0f;
			m[11] = 0.0f;

			m[12] = 0.0f;
			m[13] = 0.0f;
			m[14] = 0.0f;
		}
		m[15] = 1.0f;
	}


	void MatMakeRotation(float angle, float axis[3], float m[16])
	{

		Quat q(axis, angle);
		MatMakeRotFromQuat(q, m);
		/*
		  memset(m, 0, 16 * sizeof(float));
		  VecNormalize(axis);

		  angle *= DS_DEG2RAD;
		  float cos_a = cos(angle); float sin_a = sin(angle);

		  m[0] = 1 + (1 - cos_a) * (axis[0] * axis[0] - 1);
		  m[1] = axis[2] * sin_a + (1 - cos_a) * axis[0] * axis[1];
		  m[2] = - axis[1] * sin_a + (1 - cos_a) * axis[0] * axis[2];

		  m[4] = - axis[2] * sin_a + (1 - cos_a) * axis[0] * axis[1];
		  m[5] = 1 + (1 - cos_a) * (axis[1] * axis[1] - 1);
		  m[6] = axis[0] * sin_a + (1 - cos_a) * axis[1] * axis[2];

		  m[8] = axis[1] * sin_a + (1 - cos_a) * axis[0] * axis[2];
		  m[9] = - axis[0] * sin_a + (1 - cos_a) * axis[1] * axis[2];
		  m[10] = 1 + (1 - cos_a) * (axis[2] * axis[2] - 1);

		  m[15] = 1.0;
		*/
	}


	void MatProduct(const float m1[16], const float m2[16], float r[16])
	{

		float temp1[16];
		float temp2[16];

		memcpy(temp1, m1, 16 * sizeof(float));
		memcpy(temp2, m2, 16 * sizeof(float));
		unsigned int i, j;
		for (i = 0; i < 4; i++) for (j = 0; j < 4; j++) r[i * 4 + j] = MatRowColumnProduct(temp1, j, temp2, i);
	}


	float MatRowColumnProduct(float m1[16], unsigned int row, float m2[16], unsigned int column)
	{

		return (m1[row + 0] * m2[0 + column * 4] + m1[row + 4] * m2[1 + column * 4] +
			m1[row + 8] * m2[2 + column * 4] + m1[row + 12] * m2[3 + column * 4]);

	}


	bool MatProduct(float *m1, float *m2, float *r, unsigned int n)
	{

		float *temp1;
		if ((temp1 = new float[n*n]) == nullptr)
		{
			return false;
		}
		memcpy(temp1, m1, n * n * sizeof(float));
		float *temp2;
		if ((temp2 = new float[n*n]) == nullptr)
		{
			return false;
		}
		memcpy(temp2, m2, n * n * sizeof(float));

		unsigned int i, j;
		for (i = 0; i < n; i++) for (j = 0; j < n; j++)
		{
			r[i*n + j] = MatRowColumnProduct(temp1, j, temp2, i, n);
		}

		delete[] temp2; delete[] temp1;

		return true;

	}


	float MatRowColumnProduct(float *m1, unsigned int row, float *m2, unsigned int column, unsigned int n)
	{

		float result = 0.0;

		unsigned int i;
		for (i = 0; i < n; i++) result += m1[i*n + row] * m2[column*n + i];

		return result;

	}


	void MatPointTransform(const float vector[3], const float matrix[16], float result[3])
	{

		float r[3];
		// we suppose vector to be [x y z 1]
		r[0] = vector[0] * matrix[0] + vector[1] * matrix[4] + vector[2] * matrix[8] + 1.0f * matrix[12];
		r[1] = vector[0] * matrix[1] + vector[1] * matrix[5] + vector[2] * matrix[9] + 1.0f * matrix[13];
		r[2] = vector[0] * matrix[2] + vector[1] * matrix[6] + vector[2] * matrix[10] + 1.0f * matrix[14];
		result[0] = r[0];
		result[1] = r[1];
		result[2] = r[2];
	}


	void MatVecTransform(const float vector[3], const float matrix[16], float result[3])
	{

		float r[3];
		// we suppose vector to be [x y z 0]
		r[0] = vector[0] * matrix[0] + vector[1] * matrix[4] + vector[2] * matrix[8]; // + 0.0f * matrix[12];
		r[1] = vector[0] * matrix[1] + vector[1] * matrix[5] + vector[2] * matrix[9]; // + 0.0f * matrix[13];
		r[2] = vector[0] * matrix[2] + vector[1] * matrix[6] + vector[2] * matrix[10]; // + 0.0f * matrix[14];
		result[0] = r[0];
		result[1] = r[1];
		result[2] = r[2];
	}


	void MatBoxTransform(BoundingBox b, float matrix[16], BoundingBox &result)
	{

		int i;
		for (i = 0; i < 8; i++) MatPointTransform(b[i], matrix, result[i]);

		//  MatPointTransform(b.Xyz, matrix, result.Xyz);
		//  MatPointTransform(b.XyZ, matrix, result.XyZ);
		//  MatPointTransform(b.Xyz, matrix, result.Xyz);
		//  MatPointTransform(b.XyZ, matrix, result.XyZ);
		//  MatPointTransform(b.xYz, matrix, result.xYz);
		//  MatPointTransform(b.XYZ, matrix, result.XYZ);
		//  MatPointTransform(b.XYz, matrix, result.XYz);
		//  MatPointTransform(b.XYZ, matrix, result.XYZ);
		result.Valid = false;
		//  MatPointTransform(b.Center, matrix, result.Center);
		result.Update();
		result.Valid = b.Valid;
	}


	void MatSphereTransform(BoundingSphere b, float matrix[16], BoundingSphere &result)
	{

		MatPointTransform(b.Center, matrix, result.Center);
		Vec3 r;
		MatVecTransform(Vec3(b.Radius, 0, 0), matrix, r);
		result.Radius = r.Length();
	}


	bool MatTranspose(float *m, unsigned int n)
	{

		unsigned int i, j;
		float swap = 0.0f;
		for (i = 0; i < n - 1; i++) for (j = i + 1; j < n; j++)
		{
			swap = m[i*n + j];
			m[i*n + j] = m[j*n + i];
			m[j*n + i] = swap;
		}

		return true;
	}


	bool MatInvert(float *m, unsigned int n)
	{

		float det = MatDeterminant(m, n);
		if (det == 0)
		{
			return false;
		}

		unsigned int i, j;
		float *result, *temp;
		if ((result = new float[n*n]) == nullptr)
		{
			return false;
		}
		if ((temp = new float[(n - 1)*(n - 1)]) == nullptr)
		{
			return false;
		}

		for (i = 0; i < n; i++) for (j = 0; j < n; j++)
		{
			MatGetSubMatrix(m, temp, n, i, j);
			result[j*n + i] = gvMinusOneExp(i) * gvMinusOneExp(j) * MatDeterminant(temp, n - 1) / det;
		}

		for (i = 0; i < n*n; i++) m[i] = result[i];

		delete[] result;
		delete[] temp;

		return true;

	}


	float MatDeterminant(const float *m, unsigned int n)
	{

		if (n < 1)
		{
			return 0;
		}
		if (n == 1)
		{
			return m[0];
		}

		unsigned int i;
		float *m2;
		if ((m2 = new float[(n - 1)*(n - 1)]) == nullptr)
		{
			return 0;
		}

		float det = 0.0;
		for (i = 0; i < n; i++)
		{
			MatGetSubMatrix(m, m2, n, 0, i);
			det += gvMinusOneExp(i) * m[i] * MatDeterminant(m2, n - 1);
		}

		delete[] m2;

		return det;

	}


	void MatGetSubMatrix(const float *src, float *dst, unsigned int n, unsigned int row_to_skip, unsigned int column_to_skip)
	{

		unsigned int i, j;
		for (i = 0; i < n; i++) for (j = 0; j < n; j++)
		{
			if (i < row_to_skip && j < column_to_skip) dst[i*(n - 1) + j] = src[i*n + j];
			else if (i < row_to_skip && j > column_to_skip) dst[i*(n - 1) + j - 1] = src[i*n + j];
			else if (i > row_to_skip && j < column_to_skip) dst[(i - 1)*(n - 1) + j] = src[i*n + j];
			else if (i > row_to_skip && j > column_to_skip) dst[(i - 1)*(n - 1) + j - 1] = src[i*n + j];
		}

	}


	void MatSetRotation(float mat[16], float rot[3])
	{
		//float sx, sy, sz;
		//float cx, cy, cz;
		//float rrr[3];

		//rrr[0]=DS_DEG2RAD*rot[0];
		//rrr[1]=DS_DEG2RAD*rot[1];
		//rrr[2]=DS_DEG2RAD*rot[2];

		//sx=sin (rrr[0]);
		//sy=sin (rrr[1]);
		//sz=sin (rrr[2]);

		//cx=cos (rrr[0]);
		//cy=cos (rrr[1]);
		//cz=cos (rrr[2]);

		//mat[0]=cz*cy;
		//mat[1]=sz*cy;
		//mat[2]=-sy;

		//mat[4]=cz*sy*sx-sz*cx;
		//mat[5]=sz*sy*sx+cz*cx;
		//mat[6]=cy*sx;

		//mat[8]=cz*sy*cx+sz*sx;
		//mat[9]=sz*sy*cx-cz*sx;
		//mat[10]=cy*cx;
	  /*
		float headDegrees = DS_DEG2RAD*rot[1];
		float pitchDegrees = DS_DEG2RAD*rot[0];
		float rollDegrees = DS_DEG2RAD*rot[2];

		float cosH = cosf(headDegrees);
		float cosP = cosf(pitchDegrees);
		float cosR = cosf(rollDegrees);
		float sinH = sinf(headDegrees);
		float sinP = sinf(pitchDegrees);
		float sinR = sinf(rollDegrees);

		mat[0] = cosR * cosH - sinR * sinP * sinH;
		mat[1] = -sinR * cosP;
		mat[2] = cosR * sinH + sinR * sinP * cosH;

		mat[4] = sinR * cosH + cosR * sinP * sinH;
		mat[5] = cosR * cosP;
		mat[6] = sinR * sinH - cosR * sinP * cosH;

		mat[8] = -cosP * sinH;
		mat[9] = sinP;
		mat[10] = cosP * cosH;
	  */

		float sx, sy, sz;
		float cx, cy, cz;
		//  float pi=3.1415926535f;
		float rotation[3];


		// The matrix is column-major according to OpenGL standard

		// mat is an homougeneous matrix containing:
		//    3x3 upper left submatrix (scaling/rotation):
		//            CrzCry  CrzSrySrx-SrzCrx   CrzSryCrx+SrzSrx
		//        SrzCry  SrzSrySrx+CrzCrx   SrzSryCrx-CrzSrx
		//        -Sry    CrySrx             CryCrx
		//      being: obs_rotx, obs_roty, obs_rotz=rotation around axes (x, y, z)
		//      C*=cos (*)    S*=sin (*)

		//    first 3 elements of fourth column:
		//              translation in x,y,z
		//    last row = (0, 0, 0, 1).
		//
		// Compute values for cx, cy, cz and sx, sy, sz
		// WARNING: sin and cos use radiants (rotation will be in radians),

		rotation[0] = DS_DEG2RAD*rot[0];
		rotation[1] = DS_DEG2RAD*rot[1];
		rotation[2] = DS_DEG2RAD*rot[2];

		//  Sin of rotation around x, y, z
		sx = sin(rotation[0]);
		sy = sin(rotation[1]);
		sz = sin(rotation[2]);

		//  Cos of rotation around x, y, z
		cx = cos(rotation[0]);
		cy = cos(rotation[1]);
		cz = cos(rotation[2]);


		// 3x3 upper left submatrix

		// First column
		mat[0] = cz*cy;
		mat[1] = sz*cy;
		mat[2] = -sy;

		// Second column
		mat[4] = cz*sy*sx - sz*cx;
		mat[5] = sz*sy*sx + cz*cx;
		mat[6] = cy*sx;

		// Third column
		mat[8] = cz*sy*cx + sz*sx;
		mat[9] = sz*sy*cx - cz*sx;
		mat[10] = cy*cx;
	}


	void MatSetTranslation(float mat[16], const float pos[3])
	{
		mat[12] = pos[0];
		mat[13] = pos[1];
		mat[14] = pos[2];
	}


	void MatGetTranslation(const float mat[16], float pos[3])
	{
		pos[0] = mat[12];
		pos[1] = mat[13];
		pos[2] = mat[14];
	}



	void MatMakeHomogeneous(float mat[16], float pos[3], float rot[3])
	{

		MatSetRotation(mat, rot);
		MatSetTranslation(mat, pos);
		/*
		  float sx, sy, sz;
		  float cx, cy, cz;
		//  float pi=3.1415926535f;
		  float rotation[3];


		// The matrix is column-major according to OpenGL standard

		// mat is an homougeneous matrix containing:
		//    3x3 upper left submatrix (scaling/rotation):
		//            CrzCry  CrzSrySrx-SrzCrx   CrzSryCrx+SrzSrx
		//        SrzCry  SrzSrySrx+CrzCrx   SrzSryCrx-CrzSrx
		//        -Sry    CrySrx             CryCrx
		//      being: obs_rotx, obs_roty, obs_rotz=rotation around axes (x, y, z)
		//      C*=cos (*)    S*=sin (*)

		//    first 3 elements of fourth column:
		//              translation in x,y,z
		//    last row = (0, 0, 0, 1).
		//
		// Compute values for cx, cy, cz and sx, sy, sz
		// WARNING: sin and cos use radiants (rotation will be in radians),

		  rotation[0]=DS_DEG2RAD*rot[0];
		  rotation[1]=DS_DEG2RAD*rot[1];
		  rotation[2]=DS_DEG2RAD*rot[2];

		  //  Sin of rotation around x, y, z
		  sx=sin (rotation[0]);
		  sy=sin (rotation[1]);
		  sz=sin (rotation[2]);

		  //  Cos of rotation around x, y, z
		  cx=cos (rotation[0]);
		  cy=cos (rotation[1]);
		  cz=cos (rotation[2]);


		  // 3x3 upper left submatrix

		  // First column
		  mat[0]=cz*cy;
		  mat[1]=sz*cy;
		  mat[2]=-sy;

		  // Second column
		  mat[4]=cz*sy*sx-sz*cx;
		  mat[5]=sz*sy*sx+cz*cx;
		  mat[6]=cy*sx;

		  // Third column
		  mat[8]=cz*sy*cx+sz*sx;
		  mat[9]=sz*sy*cx-cz*sx;
		  mat[10]=cy*cx;

		  // First 3 elements of the third column

		  mat[12]=pos[0];
		  mat[13]=pos[1];
		  mat[14]=pos[2];

		  // Last row (0, 0, 0, 1)
		  mat[3]=0.0;
		  mat[7]=0.0;
		  mat[11]=0.0;
		*/
		mat[15] = 1.0;
	}


	bool MatIsRotation(float m[16])
	{
		// The condition for a pure rotation matrix is:	 m' * m = I  and  det(m)==1.
		double epsilon = 0.01; // margin to allow for rounding errors
		if (fabs(m[0] * m[4] + m[4] * m[5] + m[8] * m[9]) > epsilon) return false;
		if (fabs(m[0] * m[2] + m[4] * m[6] + m[8] * m[10]) > epsilon) return false;
		if (fabs(m[1] * m[2] + m[5] * m[6] + m[9] * m[10]) > epsilon) return false;
		if (fabs(m[0] * m[0] + m[4] * m[4] + m[8] * m[8] - 1) > epsilon) return false;
		if (fabs(m[1] * m[1] + m[5] * m[5] + m[9] * m[9] - 1) > epsilon) return false;
		if (fabs(m[2] * m[2] + m[6] * m[6] + m[10] * m[10] - 1) > epsilon) return false;
		return (fabs(MatDeterminant(m) - 1) < epsilon);
	}


	//******************************************************************************
	// Curves
	//******************************************************************************

	Vec4 GetBezier4params(double mu)
	{

		double mum1(1 - mu);
		double a, b, c, d;
		a = mum1 * mum1 * mum1;
		b = 3 * mu*mum1*mum1;
		c = 3 * mu*mu*mum1;
		d = mu * mu * mu;
		return Vec4((float)a, (float)b, (float)c, (float)d);
	}


	Vec3 VecBezier4(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, double mu)
	{

		double mum1(1 - mu);
		Vec3 p;

		double a, b, c, d;
		a = mum1 * mum1 * mum1;
		b = 3 * mu*mum1*mum1;
		c = 3 * mu*mu*mum1;
		d = mu * mu * mu;

		p.X = (float)(a*p1.X + b*p2.X + c*p3.X + d*p4.X);
		p.Y = (float)(a*p1.Y + b*p2.Y + c*p3.Y + d*p4.Y);
		p.Z = (float)(a*p1.Z + b*p2.Z + c*p3.Z + d*p4.Z);

		return p;
	}


	Vec3 VecGetEulerFromAngleAxis(float angle, Vec3 axis)
	{
		float m[16];
		MatMakeRotation(angle, axis, m);
		return MatGetEuler(m);
	}



	//******************************************************************************
	// LINEAR SYSTEMS
	//******************************************************************************

	// M x a = b , 'a' contains all the unknown values. M is a n x n matrix, a and b are n x 1 vectors

	bool GaussResolve(float *M, float *a, float *b, unsigned int n)
	{

		unsigned int i, j, k;

		// error check
		if (n == 0)
		{
			return false;
		}

		// creates temporary matrix and array, and gaussian matrix
		float *temp_m;
		if ((temp_m = new float[n*n]) == nullptr)
		{
			return false;
		}
		float *temp_b;
		if ((temp_b = new float[n]) == nullptr)
		{
			return false;
		}
		float *G;
		if ((G = new float[n*n]) == nullptr)
		{
			return false;
		}

		// turns the matrix into an upper triangular one
		for (i = 0; i < n - 1; i++)
		{

			// finds the row with a maximum value
			float max = 0.0; unsigned int max_index = 0;
			for (j = i; j < n; j++)
			{
				if (fabs(M[i*n + j]) > max) { max = fabs(M[i*n + j]); max_index = j; }
			}

			// singular matrix
			if (max == 0) { delete[] temp_m; delete[] temp_b; delete[] G; return false; }

			// stores modified matrix
			memcpy(temp_m, M, n * n * sizeof(float));
			for (j = 0; j < n; j++)
			{

				// values to flip: i-th row and max_index-th row
				temp_m[j*n + i] = M[j*n + max_index];
				temp_m[j*n + max_index] = M[j*n + i];

			}

			// stores modified array
			memcpy(temp_b, b, n * sizeof(float));
			temp_b[i] = b[max_index];
			temp_b[max_index] = b[i];

			// gaussian matrix: on the i-th column, row j-th there's - M[j][i] / M[i][i]
			memset(G, 0, n * n * sizeof(float));
			for (j = 0; j < n; j++) G[j*n + j] = 1.0;
			for (j = i + 1; j < n; j++) G[i*n + j] = -temp_m[i*n + j] / temp_m[i*n + i];

			// multiplication
			MatProduct(G, temp_m, M, n);

			for (j = 0; j < n; j++)
			{
				b[j] = 0.0;
				for (k = 0; k < n; k++) b[j] += G[k*n + j] * temp_b[k];
			}

		}

		// free up some memory
		delete[] G;
		delete[] temp_b;
		delete[] temp_m;

		// rock'n'roll! we solve the system
		float coeff;
		int index;
		for (index = n - 1; index >= 0; index--)
		{

			coeff = b[index];
			for (j = index + 1; j < n; j++) coeff -= M[j*n + index] * a[j];
			a[index] = coeff / M[index*n + index];

		}

		return true;

	}

	//******************************************************************************
	// INTERSECTION
	//******************************************************************************


	float RayPlaneIntersectionLength(Vec3 rayOrigin, Vec3 rayDirection,
		Vec3 planeOrigin, Vec3 planeNormal, Vec3* intersection)
	{
		// Locals
		float numer, denom, d;

		// Calculate the distance to the intersection point
		d = planeNormal.Dot(planeOrigin);
		numer = -planeNormal.Dot(rayOrigin) + d;
		denom = planeNormal.Dot(rayDirection);
		if (denom == 0)
		{
			return -1;
		}
		d = numer / denom;
		if (d < 0)
		{
			return -1;
		}

		// Store the intersection point
		if (intersection)
		{
			*intersection = rayOrigin + rayDirection * d;
		}

		// Return the distance
		return d;
	}

} // namespace gpvulc

