//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Tangent space calculation
/// @file TangentSpace.h
/// @author Giovanni Paolo Vigano'

#pragma once

/// @defgroup TanSpace Tangent space
/// @brief Tangent space calculation.
/// @author Giovanni Paolo Vigano'
  
/// @addtogroup DS
/// @{

/// @addtogroup Mathematics
/// @{

/// @addtogroup TanSpace
/// @{


#include "gpvulc/ds/math/mathutil.h"

namespace gpvulc
{

	struct TriangleData
	{
		Vec3 Vertex[3];
		Vec3 Normal[3];
		Vec2 Texcoord[3];
		TriangleData() {}
		TriangleData(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 n1, Vec3 n2, Vec3 n3, Vec2 t1, Vec2 t2, Vec2 t3)
		{
			Vertex[0] = v1;
			Vertex[1] = v2;
			Vertex[2] = v3;
			Normal[0] = n1;
			Normal[1] = n2;
			Normal[2] = n3;
			Texcoord[0] = t1;
			Texcoord[1] = t2;
			Texcoord[2] = t3;
		}
	};


	struct TangentBasis
	{
		Vec4 Tangent[3];
	};


	/*!
	Calculate the tangent list for normal mapping.
	* @param[in] numFaces number of faces
	* @param[in] faceList list of faces (indices of vertices)
	* @param[in] vertexList list of vertices
	* @param[in] normals  list of normals (one for each vertex)
	* @param[in] texCoords list of texture coordinates (one for each vertex)
	* @param[out] tangents previous list (if any) of tangents (one for each vertex),
			  if is nullptr (default) a new list is created
	* @return a list of vectors representing tangents for each vertex
	*/
	Vec4* CalculateTangentArray(
		unsigned int numFaces,
		const TriangleIndices* faceList,
		const Vec3* vertexList,
		const Vec3* normals,
		const Vec2* texCoords,
		Vec4* tangents = nullptr
		);


	/*!
	Given a vertex (v1) and two other vertices (v2 & v3) which define a
	triangle, this function will return Tangent, BiNormal, and Normal,
	vectors which can be used to define the tangent matrix for the first
	vertex's position (v1).<BR>
	Note: This function is based on an article by By Jakob Gath and Sbren Dreijer.<BR>
	http://www.blacksmith-studios.dk/projects/downloads/tangent_matrix_derivation.php

	@param[in] v1        vertex 1
	@param[in] v2        vertex 2
	@param[in] v3        vertex 3
	@param[in] v1tc      texture-coordinates of vertex 1
	@param[in] v2tc      texture-coordinates of vertex 2
	@param[in] v3tc      texture-coordinates of vertex 3
	@param[out] vTangent  if not null, this will be set as the tangent vector
	@param[out] vBiNormal if not null, this will be set as the binormal vector
	@param[out] vNormal   if not null, this will be set as the normal vector
	@param[in] normal    if not null it will be directly used instead of calculating it as perpendicular to the face
	*/

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
		const Vec3 *normal = nullptr);

	/*!
	 Compute the inverse tangent matrix for the vertices of a polygon.
	 Note that no check is made on parameters, you must check that arrays are not null and of the correct size.
	 @param[in] vertsPerPoly   number of verices per polygon (e.g. triangle=3)
	 @param[in] offset           starting index for the given arrays (e.g. 1st triangle=0, 2nd triangle=3, 3rd triangle = 6, ...)
	 @param[out] itmat            array of inverse tangent matrices to be calculated
	 @param[in] verts            array of vertices
	 @param[in] texcoords        array of texture coordinates
	 @param[in] norms            optional array of normals
	 @param[in] vertIndices     optional array of vertex indices. If provided this will be used for indirectly indexing the given array
	 @param[in] tcIndices       optional array of texture coordinate indices. If provided this will be used for indirectly indexing the given array, otherwise vertIndices is used if provided
	 @param[in] normIndices     optional array of normal indices. If provided this will be used for indirectly indexing the given array
	*/
	void ComputeVertInvTangentMatrixPerPoly(
		int vertsPerPoly,
		int offset,
		Mat4* itmat,
		const Vec3* verts,
		const Vec2* texcoords,
		const Vec3* norms = nullptr,
		const unsigned int* vertIndices = nullptr,
		const unsigned int* tcIndices = nullptr,
		const unsigned int* normIndices = nullptr);

	/// @}
	/// @}
	/// @}

} // namespace gpvulc

