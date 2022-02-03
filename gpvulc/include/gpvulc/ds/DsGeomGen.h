//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Geometry generation for data sets
/// @file DsGeomGen.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include "gpvulc/ds/DataSet.h"
#include "gpvulc/ds/Material.h"

#include <string>

/// @addtogroup DS
///@{

namespace gpvulc
{

	///@name Geometry generation
	///@{

	//! Type of geometric primitive
	enum DsGeoPrimType
	{
		DSPRIM_UNDEF = 0,   //!< Undefined [default]
		DSPRIM_TRIANGLE,  //!< Triangle
		DSPRIM_RHOMBUS,   //!< Rhombus
		DSPRIM_RECT,      //!< Rectangle
		DSPRIM_CIRCLE,    //!< Circle
		DSPRIM_ELLIPSE,   //!< Ellipse
		DSPRIM_BOX,       //!< Box
		DSPRIM_SPHERE,    //!< Sphere
		DSPRIM_NORTH_EMISPHERE, //!< North emisphere
		DSPRIM_SOUTH_EMISPHERE, //!< South emisphere
		DSPRIM_CYLSURF,   //!< Cylindrical surface
		DSPRIM_CYLINDER,  //!< Cylinder
		DSPRIM_CAPSULE,   //!< Capsule
		DSPRIM_CONESURF,  //!< Cone surface
		DSPRIM_CONE,      //!< Cone
	};


	std::string ToString(DsGeoPrimType  e);

	bool FromString(const std::string& s, DsGeoPrimType & e);


	//! Geometric primitive descriptor
	struct DsGeoPrim
	{

		//! Primitive type (see DsGeoPrimType)
		DsGeoPrimType Type;

		//! Placement of the origin of the vertices coordinates (DS_ALIGN_BOTTOMCENTER,DS_ALIGN_CENTER,...)
		int ShapeAlignment;

		//! Offset of the primitive shape
		Vec3 Offset;

		//! Size of the primitive shape along cartesian axes
		Vec3 Size;

		//! Alignmentof the main simmetry axis (if any) along cartesian axes (DS_X,DS_Y,DS_Z)
		int AxisOrientation;

		unsigned Slices; //!< Slices of (rounded) surface around the main axis (default=24)
		unsigned Quality; //!< Number of faces of spherical surfaces, 0=minimum allowed (default=20)

		bool Flip; //!< Revert vertices order to flip faces

		// /// Matrix used to transform vertices after the primitive has been created
		// Mat4 Matrix;

		//! Material definition to be used for this shape
		Material Material;

		DsGeoPrim();
	};

	//! Generate a geometry from a geometric primitive descriptor
	bool DsGenGeomFromPrim(DsGeom* geom, DsGeoPrim& prim);

	//! Generate an object from a geometric primitive descriptor
	bool DsGenObjFromPrim(DsObject* dsobj, DsGeoPrim& prim);

	//! Generate a dataset from a geometric primitive descriptor
	bool DsGenDatasetFromPrim(DsDataSet* dataset, MtlLib* mtllib, DsGeoPrim& prim, const std::string& name = "");

	//! Generate a dataset from a list of geometric primitive descriptor
	bool DsGenDatasetFromPrimList(DsDataSet* dataset, MtlLib* mtllib, DynArray<DsGeoPrim>& prim_list);



	/*!
	 Generate a cylinder
	 @param geom target geometry
	 @param align alignment of the main axis (DS_X, DS_Y, DS_Z)
	 @param aabb axis aligned bounding box defining the box extension
	 @param num_sides number of sides for the lateral surface
	 @param centered cylinder origin is in its center (default)
	 @param mtl_name material name to be set to geometry meshes
	 @return false if the given geometry is nullptr, true otherwise
	*/
	bool DsGenCylinder(DsGeom* geom, int align, const AABBox& aabb, unsigned num_sides, const std::string& mtl_name);

	/*!
	 Generate a box
	 @param geom target geometry
	 @param aabb axis aligned bounding box defining the box extension
	 @param mtl_name material name to be set to geometry meshes
	 @return false if the given geometry is nullptr, true otherwise
	*/
	bool DsGenBox(DsGeom* geom, const AABBox& aabb, const std::string& mtl_name = "");

	/*!
	 Generate an axis aligned bounding box
	 @param geom target geometry
	 @param aabb axis aligned bounding box
	 @param mtl_name material name to be set to geometry meshes
	 @return false if the given geometry is nullptr, true otherwise
	*/
	bool DsGenAABB(DsGeom* geom, const AABBox& aabb, const std::string& mtl_name = "");

	/*!
	 Generate a sphere
	 @param geom target geometry
	 @param align alignment of the main axis (DS_X, DS_Y, DS_Z)
	 @param aabb axis aligned bounding box defining the box extension
	 @param quality number of faces
	 @param hemisphere build only an hemisphere (+1 for positive side, -1 for negative side of the axis)
	 @param flip flip faces
	 @param mtl_name material name to be set to geometry meshes
	 @return false if the given geometry is nullptr, true otherwise
	*/
	bool DsGenSphere(DsGeom* geom, int align, const AABBox& aabb, unsigned quality, int hemisphere, bool flip, const std::string& mtl_name);

	///@} {Geometry generation}
///@} (DataSet)

}
