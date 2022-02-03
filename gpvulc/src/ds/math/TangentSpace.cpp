//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#include <gpvulc/ds/math/TangentSpace.h>

namespace gpvulc
{

	namespace
	{
		// Function CalculateTangentArray
		// ORIGINAL CODE FROM:
		// http://www.opengl.org/discussion_boards/ubb/Forum3/HTML/011349.html

		/* TODO: The tangent list must work also with shared vertices
						  and TangentBasis is redundant, can be replaced with
						  Vec4* */


		void _CalculateTangentArray(long numTriangles,
			TriangleData* triangleList, TangentBasis* tangentBasisList)
		{

			/*INPUT NEEDED:

		   --triangleList: list of the triangles of the geometry.
			--Every element of triangleList is a single triangle, called Triangle.

			--Every Triangle has its own members:

			   Vertex1, Vertex2, Vertex3 (3D vectors)
				Normal[0], Normal[1], Normal[2] (3D vectors)
				Texcoord1, Texcoord2, Texcoord3 (2D vectors)
				  ATTENTION: the Texcoord are simply the texture coordinate of the diffusive texture.

			--tangentBasisList: the empty array to fill in with the output


			OUTPUT PRODUCED:

		   --tangentBasisList: list of the tangent bases related to the triangles of the geometry.
			--Every element of TangeBasisList is called TangentBasis.

			--Every TangentBasis has its own members:

			  Tangent[0], Tangent[1], Tangent[2] (3D vectors)

			  ATTENTION:
				note that the tangent basis is a basis of a 3-dimensional space
			  so it is a 3x3 matrix.
				It is composed by:
				-the Tangent vector
				-the Binormal vector
				-the Normal vector

			   We need to compute only the Tangent vector (and we do it with this function)
				since the Normal vector is the vertex normal and we already have it
				and since the Binormal is the cross product between the Normal and the Tangent.
				The Binormal is computed in the vertex shader as "cross (gl_Normal, Tangent)",
				where "Tangent" is what we are going to compute here. */





			long i = 0;
			long j = 0;
			Vec3 sdir, tdir;
			Vec3* tan1 = new Vec3[numTriangles * 3];
			Vec3* tan2 = new Vec3[numTriangles * 3];

			for (i = 0; i < numTriangles; i++)
			{
				// Shortcut to the i-th triangle
				TriangleData& Triangle = triangleList[i];

				// Shortcut to the vertexes of the i-th triangle
				Vec3& Vertex1 = Triangle.Vertex[0];
				Vec3& Vertex2 = Triangle.Vertex[1];
				Vec3& Vertex3 = Triangle.Vertex[2];

				// Shortcut to the texture coordinates of the vertices of the i-th triangle
				// ATTENTION: the Texcoord are simply the texture coordinate of the diffusive texture.
				Vec2& Texcoord1 = Triangle.Texcoord[0];
				Vec2& Texcoord2 = Triangle.Texcoord[1];
				Vec2& Texcoord3 = Triangle.Texcoord[2];

				float x1 = Vertex2.X - Vertex1.X;
				float x2 = Vertex3.X - Vertex1.X;

				float y1 = Vertex2.Y - Vertex1.Y;
				float y2 = Vertex3.Y - Vertex1.Y;

				float z1 = Vertex2.Z - Vertex1.Z;
				float z2 = Vertex3.Z - Vertex1.Z;

				float s1 = Texcoord2.X - Texcoord1.X;
				float s2 = Texcoord3.X - Texcoord1.X;

				float t1 = Texcoord2.Y - Texcoord1.Y;
				float t2 = Texcoord3.Y - Texcoord1.Y;

				float r = 1.0F / (s1*t2 - s2*t1);

				sdir = Vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

				tdir = Vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				for (j = 0; j < 3; j++) tan1[i * 3 + j] += sdir;

				for (j = 0; j < 3; j++) tan2[i * 3 + j] += tdir;
			}





			for (i = 0; i < numTriangles; i++)
			{
				for (j = 0; j < 3; j++)
				{
					Vec3& normal = triangleList[i].Normal[j];
					Vec4& tangent = tangentBasisList[i].Tangent[j];
					Vec3& t = tan1[i * 3 + j];

					// THE OPERATOR "%" IS THE VECTOR CROSS PRODUCT
					// THE OPERATOR "*" IS THE VECTOR SCALAR PRODUCT (DOT PRODUCT)
					tangent = Vec4(
						(t - normal.Dot(normal.Dot(t))).Normalized());
					tangent.W = (VecDotProduct((normal.Cross(t)), tan2[i * 3 + j]) < 0.0F) ? -1.0F : 1.0F;
				}
			}

			delete[] tan1;
			delete[] tan2;
		}
	}


	Vec4* CalculateTangentArray(
		unsigned int NumFaces,
		const TriangleIndices* faceList,
		const Vec3* vertexList,
		const Vec3* normals,
		const Vec2* texCoords,
		Vec4* tangents
		)
	{

		if (faceList == nullptr || vertexList == nullptr || normals == nullptr || texCoords == nullptr)
		{
			return nullptr;
		}

		//  Vec4* tangents = nullptr;

		long numTriangles = NumFaces;
		TriangleData* triangleList = new TriangleData[numTriangles];
		if (triangleList == nullptr)
		{
			return nullptr;
		}
		TangentBasis* tangentBasisList = new TangentBasis[numTriangles];
		if (tangentBasisList == nullptr)
		{
			delete[] triangleList;
			return nullptr;
		}

		unsigned int i = 0;
		unsigned int j = 0;
		for (i = 0; i < NumFaces; i++)
		{
			triangleList[i] = TriangleData(
				vertexList[faceList[i].VertsIndices[0]],
				vertexList[faceList[i].VertsIndices[1]],
				vertexList[faceList[i].VertsIndices[2]],
				normals[faceList[i].VertsIndices[0]],
				normals[faceList[i].VertsIndices[1]],
				normals[faceList[i].VertsIndices[2]],
				texCoords[faceList[i].VertsIndices[0]],
				texCoords[faceList[i].VertsIndices[1]],
				texCoords[faceList[i].VertsIndices[2]]
				);
		}

		_CalculateTangentArray(numTriangles, triangleList, tangentBasisList);

		if (tangents == nullptr)
		{
			tangents = new Vec4[NumFaces * 3];
		}
		if (tangents == nullptr)
		{
			delete[] triangleList;
			delete[] tangentBasisList;
			return nullptr;
		}
		for (i = 0; i < NumFaces; i++)
		{
			for (j = 0; j < 3; j++)
			{
				tangents[i * 3 + j] = tangentBasisList[i].Tangent[j];
			}
		}

		delete[] triangleList;
		delete[] tangentBasisList;

		return tangents;
	}






	//-----------------------------------------------------------------------------
	// Name: CreateTangentSpaceVectors()
	// Desc: Given a vertex (v1) and two other vertices (v2 & v3) which define a 
	//       triangle, this function will return Tangent, BiNormal, and Normal, 
	//       vectors which can be used to define the tangent matrix for the first 
	//       vertex's position (v1).
	//
	// Input:
	//       vertex1, vertex2, vertex3        - vertices 1,2,3
	//       vertex1tc, vertex2tc, vertex3tc  - texture-coordinates of vertices 1,2,3
	// Output:
	//       vTangent  - tangent vector
	//       vBiNormal - binormal vector
	//       vNormal   - normal vector
	//
	// Note: This function is based on http://www.terathon.com/code/tangent.html
	//------------------------------------------------------------------------------

	void CreateTangentSpaceVectors(
		const Vec3& vertex1,
		const Vec3& vertex2,
		const Vec3& vertex3,
		const Vec2& vertex1tc,
		const Vec2& vertex2tc,
		const Vec2& vertex3tc,
		Vec3 *vTangent,
		Vec3 *vBiNormal,
		Vec3 *vNormal,
		const Vec3* normal)
	{

		float u0 = vertex1tc.X;
		float v0 = vertex1tc.Y;
		float u1 = vertex2tc.X;
		float v1 = vertex2tc.Y;
		float u2 = vertex3tc.X;
		float v2 = vertex3tc.Y;

		Vec3 Q1 = vertex2 - vertex1;
		Vec3 Q2 = vertex3 - vertex1;

		float s1 = u1 - u0;
		float t1 = v1 - v0;
		float s2 = u2 - u0;
		float t2 = v2 - v0;

		float d = (s1*t2 - s2*t1);

		if (d < 0.0001f && d > -0.0001f)
		{
			// We're too close to zero and we're at risk of a divide-by-zero! 
			// Set the tangent matrix to the identity matrix and do nothing.
			if (vTangent) *vTangent = Vec3(1.0f, 0.0f, 0.0f);
			if (vBiNormal) *vBiNormal = Vec3(0.0f, 1.0f, 0.0f);
			if (vNormal) *vNormal = Vec3(0.0f, 0.0f, 1.0f);
			return;
		}

		Mat4 ist;
		ist.SetRow(0, Vec4(t2, -t1, 0, 0));
		ist.SetRow(1, Vec4(-s2, s1, 0, 0));
		ist.SetRow(2, Vec4(0, 0, 0, 0));
		ist.SetRow(3, Vec4(0, 0, 0, 0));

		Mat4 Q;
		Q.SetRow(0, Vec4(Q1.X, Q1.Y, Q1.Z, 0));
		Q.SetRow(1, Vec4(Q2.X, Q2.Y, Q2.Z, 0));
		Q.SetRow(2, Vec4(0, 0, 0, 0));
		Q.SetRow(3, Vec4(0, 0, 0, 0));

		Mat4 TB = ist * Q / d;


		Vec3 T = TB.GetRow(0);
		Vec3 B = TB.GetRow(1);
		Vec3 N;


		if (normal)
		{
			N = *normal;

			// project T and B on the same plane as N
			T.ProjectOnPlane(N);
			B.ProjectOnPlane(N);
		}
		else
		{
			N = T.Cross(B);
		}


		Vec3 T1 = T - N * (N.Dot(T));
		Vec3 B1 = B - N * (N.Dot(B)) - T1 * (T1.Dot(B));
		T1.Normalize();
		B1.Normalize();
		N.Normalize();

		if (vTangent)
		{
			(*vTangent) = T1;
		}

		if (vBiNormal)
		{
			(*vBiNormal) = B1;
		}

		if (vNormal)
		{
			(*vNormal) = N;
		}

	}



	void ComputeVertInvTangentMatrixPerPoly(
		int vertsPerPrim,
		int offset,
		Mat4* itmat,
		const Vec3* verts,
		const Vec2* texCoords,
		const Vec3* norms,
		const unsigned int* vertIndices,
		const unsigned int* tcIndices,
		const unsigned int* normIndices)
	{
		Vec3 vTangent;
		Vec3 vBiNormal;
		Vec3 vNormal;
		for (int i = 0; i < vertsPerPrim; i++)
		{
			//
			// Vertex 0 of a quadrilateral face...
			//
			//  v3
			//    3----2
			//    |    |
			//    |    |
			//    0----1
			//  v1      v2
			//
			int v1 = i;
			int v2 = i + 1;
			int v3 = i - 1;
			if (v3 < 0) v3 = vertsPerPrim - 1;
			if (v2 >= vertsPerPrim) v2 = 0;
			// apply the offset after identifying the vertices
			v1 += offset;
			v2 += offset;
			v3 += offset;

			int v1v, v2v, v3v;
			int v1t, v2t, v3t;
			int v1n, v2n, v3n;

			if (vertIndices)
			{
				v1v = vertIndices[v1];
				v2v = vertIndices[v2];
				v3v = vertIndices[v3];
			}
			else
			{
				v1v = v1;
				v2v = v2;
				v3v = v3;
			}

			if (tcIndices)
			{
				v1t = tcIndices[v1];
				v2t = tcIndices[v2];
				v3t = tcIndices[v3];
			}
			else
			{
				if (vertIndices)
				{
					v1t = vertIndices[v1];
					v2t = vertIndices[v2];
					v3t = vertIndices[v3];
				}
				else
				{
					v1t = v1;
					v2t = v2;
					v3t = v3;
				}
			}
			if (normIndices)
			{
				v1n = normIndices[v1];
				v2n = normIndices[v2];
				v3n = normIndices[v3];
			}
			else
			{
				if (vertIndices)
				{
					v1n = vertIndices[v1];
					v2n = vertIndices[v2];
					v3n = vertIndices[v3];
				}
				else
				{
					v1n = v1;
					v2n = v2;
					v3n = v3;
				}
			}
			if (norms)
			{
				vNormal = norms[v1n];
			}

			CreateTangentSpaceVectors(
				verts[v1v], verts[v2v], verts[v3v],
				texCoords[v1t], texCoords[v2t], texCoords[v3t],
				&vTangent, &vBiNormal, &vNormal,
				norms ? &norms[v1n] : nullptr);
			itmat[v1].SetRow(0, Vec4(vTangent, 0.0f));
			itmat[v1].SetRow(1, Vec4(vBiNormal, 0.0f));
			itmat[v1].SetRow(2, Vec4(vNormal, 0.0f));
		}
	}

} // namespace gpvulc
