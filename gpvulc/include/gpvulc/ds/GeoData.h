//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Basic geometry data
/// @file GeoData.h
/// @author Giovanni Paolo Vigano'

//! Neutral basic data strucures for geometry definition.

#pragma once


/// @addtogroup DS
///@{

/// @addtogroup DataSet
///@{

#include "gpvulc/ds/MtlData.h"
#include <gpvulc/ds/util/DynArray.h>

namespace gpvulc
{
	//! Texture coordinate structure
	struct TexCoord
	{

		float U, V;

		TexCoord() { this->U = this->V = 0.0; }
		TexCoord(float U, float V) { this->U = U; this->V = V; }
		TexCoord(float f[2]) { this->U = f[0]; this->V = f[1]; }

		TexCoord& operator =(const TexCoord& p1) { memcpy(this, (void*)(&p1), sizeof(TexCoord)); return *this; }
		TexCoord operator +(TexCoord p1) { return TexCoord(this->U + p1.U, this->V + p1.V); }
		TexCoord operator -(TexCoord p1) { return TexCoord(this->U - p1.U, this->V - p1.V); }
		TexCoord operator *(float f) { return TexCoord(this->U * f, this->V * f); }
		TexCoord operator /(float f) { return TexCoord(this->U / f, this->V / f); }
		TexCoord operator +=(TexCoord p1) { this->U += p1.U; this->V += p1.V; return *this; }
		TexCoord operator -=(TexCoord p1) { this->U -= p1.U; this->V -= p1.V; return *this; }
		TexCoord operator *=(float f) { this->U *= f; this->V *= f; return *this; }
		TexCoord operator /=(float f) { this->U /= f; this->V /= f; return *this; }

		operator float*() { return (float*)this; }
		operator Vec2() { return (Vec2&)*this; }
	};

	///@name Lists
	//@{

	//! List of colors
	typedef DynArray< ColorRGBA > MColor;
	//! List of texture coordinates
	typedef DynArray< TexCoord > MTexCoord;

	//@}


	/*

	///@name Conversion functions - covert a type to/from a string
	//@{

	std::string ToString(ColorRGBA val);
	bool FromString(const std::string&s, ColorRGBA& val);
	std::string ToString(const MColor& val);
	bool FromString(const std::string&s, MColor& val);

	std::string ToString(const TexCoord& val);
	bool FromString(const std::string&s, TexCoord& val);
	std::string ToString(const MTexCoord& val);
	bool FromString(const std::string&s, MTexCoord& val);

	//@}

	*/


	/*!
	  Calculate faces normals for a list of faces (use SplitVerts() if you have shared vertices).
	 @param verts          list of vertices
	 @param faces          list of faces
	 @param faces_normals  (output) list of normals: if a non-empty list is provided it will be filled, otherwise a new list is created
	 @return true on success or false on error (any pointer null, any counter==0 or memory allocation failure).
	*/
	bool CalcFaceNormals(std::vector<Vec3>& verts, std::vector<TriangleIndices>& faces, std::vector<Vec3>& faces_normals);

	/*!
	  Calculate vertex normals for a list of faces (interpolated).
	 @param verts          list of vertices
	 @param num_verts      number of vertices
	 @param faces          list of faces
	 @param num_faces      number of faces
	 @param vert_normals   (output) list of normals: if a non-null list is provided it will be filled, otherwise a new list is created
	 @param vertsbeg       first index in vertices subrange (default=0, first vertex)
	 @param vertscount     number of vertices in the subrange (default=0, until the last vertex)
	 @param facesbeg       first index in faces subrange (default=0, first face)
	 @param facescount     number of faces in the subrange (default=0, until the last face)
	 @return true on success or false on error (any pointer null, any counter==0 or memory allocation failure).
	*/
	bool CalcVertNormals(const std::vector<Vec3>& verts, std::vector<TriangleIndices>& faces, std::vector<Vec3>& vert_normals, unsigned int vertsbeg = 0, unsigned int vertscount = 0, unsigned int facesbeg = 0, unsigned int facescount = 0);

	/*!
	  Split vertices shared by a list of faces.
	 @param verts          list of vertices
	 @param new_verts      (output) pointer for the new list of vertices
	 @param faces          list of faces
	 @param normals        list of normals: if a null list is provided it will be ignored
	 @param new_normals    (output) pointer for the new list of normals
	 @param tex_coords     list of texture coordinates: if a null list is provided it will be ignored
	 @param new_tex_coords (output) pointer for the new list of texture coordinates
	 @param deletelists    delete old lists (vertices,normals,texture coords)
	*/
	bool SplitVerts(
		const std::vector<Vec3>& verts,
		std::vector<Vec3>& new_verts,
		std::vector<TriangleIndices>& faces,
		std::vector<Vec3>& normals,
		std::vector<Vec3>& new_normals,
		const std::vector<TexCoord>& tex_coords,
		std::vector<TexCoord>& new_tex_coords);
}

///@} (DataSet)

///@} (DS)


