//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Data set
/// @file DataSet.h
/// @author Giovanni Paolo Vigano'

//! Neutral 3D models data set definition.

#pragma once


/// @defgroup DataSet Data set
/// Neutral 3D models data set definition.
/// @author Giovanni Paolo Vigano'

/// @addtogroup DS
///@{

/// @addtogroup DataSet
///@{

#include "gpvulc/ds/MtlData.h"
#include "gpvulc/ds/GeoData.h"

#include <gpvulc/ds/math/Animation.h>
#include <gpvulc/ds/util/DynArray.h>
#include <gpvulc/text/TextBuffer.h>
#include <gpvulc/path/PathInfo.h>

namespace gpvulc
{
	// Forward class declarations
	class DsMesh;
	class DsGeom;
	class DsSkinJoint;
	class DsSkinJointWeight;
	class DsSkinVertex;
	class DsSkin;
	class DsObject;
	class DsDataSet;

	/*!
	 These values are equal to the corresponding OpenGL:
	 GL_POINTS == DS_POINTS
	 GL_LINES == DS_LINES
	 ...
	*/
	enum DsMeshType
	{
		DS_POINTS = 0, DS_LINES, DS_LINE_LOOP,
		DS_LINE_STRIP, DS_TRIANGLES, DS_TRIANGLE_STRIP, DS_TRIANGLE_FAN,
		DS_QUADS, DS_QUAD_STRIP, DS_POLYGON
	};

	enum _DsCopyModeEnum { DS_USE = 0, DS_CLONE, DS_COPY };

	//! Basic mesh (or set of meshes): a list of points, lines, faces
	class DsMesh
	{


	public:

		//! Type of OpenGL meshes: GL_POINTS,GL_LINES,GL_LINE_STRIP,GL_LINE_LOOP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_QUADS,GL_QUAD_STRIP,GL_POLYGON
		DsMeshType Type;

		//! Number of vertices, used only for GL_POINTS(1), GL_LINES (2), GL_TRIANGLES (3), GL_QUADS (4)
		int MeshVerts;
		DynArray< unsigned int > VertIdx;	// Vertices index
		DynArray< unsigned int > NormIdx;	// Normals index
		DynArray< unsigned int > ColorIdx;	// Colors index
		DynArray< unsigned int > TexCoordIdx;	// Texture coordinates index
		TextBuffer MtlName;	// Material used for this mesh
		DsMesh();
		DsMesh(const DsMesh& _gvdsmesh);

		//! Add a vertex (index) to this mesh
		void AddVertex(int idx) { VertIdx.Add(idx); }

		//! Add a normal (index) to this mesh
		void AddNormal(int idx) { NormIdx.Add(idx); }

		//! Add a color (index) to this mesh
		void AddColor(int idx) { ColorIdx.Add(idx); }

		//! Add a textute coordinate (index) to this mesh
		void AddTexCoord(int idx) { TexCoordIdx.Add(idx); }
		DsMeshType GetType();	//!< Get the mesh type
		void SetType(DsMeshType _type);	//!< Set the mesh type

		/*!
		 Get the number of vertices for each mesh element
		 or 0 if the mesh has not fixed sized elements
		*/
		int GetMeshVerts();
		DynArray< Mat4 > InverseTangentMatrix;	//!< Pre computed inverse tangent space matrices for normal mapping
		int GetNumFaces();
		DynArray< unsigned int > SmoothingGroups;	// Smoothing groups index
		Mat4 LocalMatrix;	//!< Here is stored an optional matrix for a local transformation

		//! Here is stored an optional matrix representing the absolute transformation (used in 3ds loader)
		Mat4 AbsoluteMatrix;
		virtual ~DsMesh();

	private:
		void _Init();	// [automatically generated]
		void _ConstructorInclude();	// [automatically generated]
		void _Copy(const DsMesh& obj);	// [automatically generated]

	public:
		DsMesh& operator =(const DsMesh& obj);	// [automatically generated]
	};





	//! Geometry (group of meshes)
	class DsGeom
	{

	public:
		TextBuffer Name;
		DynArray< DsMesh* > Meshes;	// List of meshes
		DsGeom(DsObject* _obj, const std::string& _name);

		// Add a vertex to the last mesh
		void AddMeshVertex(int idx)
		{

			if (Meshes.Size() == 0) AddMesh();
			Meshes.GetLast()->AddVertex(idx);
		}

		// Add a normal to the last mesh
		void AddMeshNormal(int idx)
		{

			if (Meshes.Size() == 0) AddMesh();
			Meshes.GetLast()->AddNormal(idx);
		}

		// Add a color to the last mesh
		void AddMeshColor(int idx)
		{

			if (Meshes.Size() == 0) AddMesh();
			Meshes.GetLast()->AddColor(idx);
		}

		// Add a texture coordinate to the last mesh
		void AddMeshTexCoord(int idx)
		{

			if (Meshes.Size() == 0) AddMesh();
			Meshes.GetLast()->AddTexCoord(idx);
		}
		DsObject* Object;
		DsGeom();
		DsGeom(const DsGeom& _gvdsgroup);

		// Add a new (empty) mesh to this data set
		bool AddMesh(DsMeshType _type = DS_TRIANGLES)
		{

			DsMesh* mesh = new DsMesh();
			if (!mesh)
			{
				return false;
			}
			mesh->SetType(_type);
			Meshes.Add(mesh);
			return true;
		}
		void SetMtl(const std::string& _name);

		///@name Vertex info
		//@{

		DynArray< Vec3 > Vertices;	// Vertices coordinates
		DynArray< Vec3 > Normals;	// Normals list
		DynArray< ColorRGBA > Colors;	// Colors list
		DynArray< TexCoord > TexCoords;	//!< Texture coordinates for each texture unit
		DynArray< Mat4 > Transforms;	//!< Transformations applied to vertices

	  //@}

		void AddVertex(const Vec3& v) { Vertices.Add(v); }
		void AddNormal(const Vec3& n) { Normals.Add(n); }
		void AddColor(const ColorRGBA& c) { Colors.Add(c); }
		void AddTexCoord(const TexCoord& v, int unit = 0) { TexCoords.Add(v); }

		//  Update the link to the parent object, vertices, normals, etc.
		//  are shared or not according to the SharedData() state of the object
		bool LinkToObject(DsObject* obj = nullptr);
		virtual ~DsGeom();

	private:
		void _Init();	// [automatically generated]
		void _ConstructorInclude();	// [automatically generated]
		void _Copy(const DsGeom& obj);	// [automatically generated]

	public:
		DsGeom& operator =(const DsGeom& obj);	// [automatically generated]
	};






	//! Joint (name) with its inverse bind matrix
	class DsSkinJoint
	{


	public:
		TextBuffer Name;	// Name of the joint

		/*!
		 Inverse bind matrix: inverse of the matrix applied to the joint
		  before binding it to the shape
		*/
		Mat4 InvBindMatrix;
		Mat4 JointMat;	//!<  Precomputed joint matrix
	};




	//! Weights (for each joint) affecting vertex deformation
	class DsSkinVertex
	{


	public:
		DynArray< DsSkinJointWeight > JointWeights;	//!< List of weights (for each joint) affecting vertex deformation
	};




	//! Reference to a joint and weight applied
	class DsSkinJointWeight
	{


	public:
		TextBuffer JointName;	//!< Reference (name) to a joint
		float Weight;	//!< Weight applied for the named joint
		Mat4 JointMat;	//!< Transform matrix of the joint
		unsigned int JointIdx;	//!<  Index of the reference joint
		DsSkinJointWeight();	//!< Constructor
	};




	//! Skin data with references to vetices and skeleton joints
	class DsSkin
	{


	public:
		TextBuffer ShapeName;	//!< Name of the shape to which this skin is applied
		Mat4 ShapeBindMatrix;	//!< Matrix applied before the skin was bound to the shape
		DynArray< DsSkinVertex > ShapeVertices;	//!< List of weights (for each joint) affecting vertex deformation
		DynArray< TextBuffer > Skeleton;	//!< List of nodes (joint names) conntrolling this skin
		DynArray< DsSkinJoint > Joints;	//!< List of joints controlling this skin
	};




	/*!
	 Geometry data container class.
	 Lists in this class can be shared among different objects
	 using the Copy method with mode=DS_USE or DS_CLONE.
	*/
	class DsObject
	{


	public:
		Mat4 BakedMatrix;	//!< Here is stored an optional transformation applied by setup
		DsObject(DsDataSet* _ds, const std::string& _name);
		DsObject(const DsObject& _gvdsobject);
		void AddVertex(const Vec3& v)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->Vertices.Add(v);
		}
		void AddNormal(const Vec3& n)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->Normals.Add(n);
		}
		void AddColor(const ColorRGBA& c)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->Colors.Add(c);
		}
		void AddTexCoord(const TexCoord& v, int unit = 0)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->TexCoords.Add(v);
		}

		// Add a vertex to the last mesh
		void AddMeshVertex(int idx)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->AddMeshVertex(idx);
		}

		// Add a normal to the last mesh
		void AddMeshNormal(int idx)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->AddMeshNormal(idx);
		}

		// Add a color to the last mesh
		void AddMeshColor(int idx)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->AddMeshColor(idx);
		}

		// Add a texture coordinate to the last mesh
		void AddMeshTexCoord(int idx)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->AddMeshTexCoord(idx);
		}

		// Add a new (empty) mesh to this data set
		void AddMesh(DsMeshType _type = DS_TRIANGLES)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->AddMesh(_type);
		}
		bool AddGeom(const std::string& name = "")
		{

			TextBuffer geomName;
			if (name.empty())
			{
				geomName << Name << Geoms.Size();
			}
			else
			{
				geomName = name;
			}
			DsGeom* geom = new DsGeom(this, geomName);
			if (!geom)
			{
				return false;
			}
			Geoms.Add(geom);
			return true;
		}

		/*!
		 Copy a dataset in 3 different modes:
		 <BR> DS_USE:   Use directly all the data: any change in original geometry affects this object
		 <BR> DS_COPY:  Vertex lists are used direclty, meshes are copied
		 <BR> DS_CLONE: Create a complete copy of every data
		*/
		void CopyDS(int mode, DsObject* obj);
		DynArray< DsGeom* > Geoms;	// List of geometries
		TextBuffer Name;	//!< Name of the object (alphanumeric identifier)
		bool SetMtl(const std::string& _name);
		PivotTransform Transform;
		DsDataSet* DataSet;
		DsObject();
		int ID;	//!< Optional numeric identifier
		int ParentID;	///< Optional numeric identifier of the parent object.

	  ///@name Shared vertex info
	  //@{

		DynArray< Vec3 > SharedVertices;	// Vertices coordinates
		DynArray< Vec3 > SharedNormals;	// Shared normals list
		DynArray< ColorRGBA > SharedColors;	// shared colors list
		DynArray< TexCoord > SharedTexCoords;	//!< Shared texture coordinates for each texture unit
		DynArray< Mat4 > SharedTransforms;	//!< Shared transformations applied to vertices

	  //@}


		// If true the next added geometries will share the same vertices, normals, etc.
		bool _SharedData;
		void ShareData(bool __shareddata);
		bool SharedData();
		void UpdateGeoms();
		TransformAnimation* Animation;
		DsObject* Parent;	//  Parent object
		DynArray< DsObject* > Children;	//!<  List of children objects

		//!  A temporary vector to hold the pivot offset (used by 3ds loader)
		Vec3 PivotOffset;

		//!  Mark this object as a $$$DUMMY to be processed as an instance (used by 3ds loader)
		bool IsDummy;
		virtual ~DsObject();
		DsSkin* Skin;
		void AllocateVertices(unsigned int n)
		{

			if (Geoms.Size() == 0) AddGeom();
			Geoms.GetLast()->Vertices.Allocate(n);
		}
		void AllocateMeshVertices(unsigned int n)
		{

			if (Geoms.Size() == 0) AddGeom();
			if (Geoms.GetLast()->Meshes.Size() == 0) AddMesh();
			Geoms.GetLast()->Meshes.GetLast()->VertIdx.Allocate(n);
		}
		bool IsJoint;	//!< Mark this object as a joint to be processed for skinning

	private:
		void _Init();	// [automatically generated]
		void _ConstructorInclude();	// [automatically generated]
		void _Copy(const DsObject& obj);	// [automatically generated]

	public:
		DsObject& operator =(const DsObject& obj);	// [automatically generated]
	};





	/*!
	 Geometry data container class.
	 Lists in this class can be shared among different objects
	 using the Copy method with mode=DS_USE or DS_CLONE.
	 TODO: add SetCurrentNNN to add data to the selected item instead of the last one.
	*/
	class DsDataSet
	{


	public:
		Mat4 ConversionMatrix;	//!< Here is stored an optional transformation applied by setup
		TextBuffer MtlLibName;	// Material library name
		DsDataSet() { _ConstructorInclude(); }
		DsDataSet(const DsDataSet& _gvdsdataset);
		void AddVertex(const Vec3& v)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddVertex(v);
		}
		void AddNormal(const Vec3& n)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddNormal(n);
		}
		void AddColor(const ColorRGBA& c)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddColor(c);
		}
		void AddTexCoord(const TexCoord& v)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddTexCoord(v);
		}

		// Add a vertex to the last mesh
		void AddMeshVertex(int idx)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddMeshVertex(idx);
		}

		// Add a normal to the last mesh
		void AddMeshNormal(int idx)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddMeshNormal(idx);
		}

		// Add a color to the last mesh
		void AddMeshColor(int idx)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddMeshColor(idx);
		}

		// Add a texture coordinate to the last mesh
		void AddMeshTexCoord(int idx)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddMeshTexCoord(idx);
		}

		// Add a new (empty) mesh to this data set
		void AddMesh(DsMeshType _type = DS_TRIANGLES)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AddMesh(_type);
		}
		bool AddGeom(const std::string& name = "default")
		{

			if (Objects.Size() == 0) AddObject();
			return Objects.GetLast()->AddGeom(name);
		}
		DynArray< DsObject* > Objects;	//!< List of objects
		TextBuffer Name;

		/*!
		 Set the material name to the last mesh of the last geometry of the last object added.
		 If the given string is null or empty immediately returns false.
		 Returns true if the material name was successfully set.
		*/
		bool SetMtl(const std::string& _name);

		bool AddObject(const std::string& name = "")
		{

			TextBuffer objName;
			if (name.empty())
			{
				objName << Name << Objects.Size();
			}
			else
			{
				objName = name;
			}
			DsObject* obj = new DsObject(this, objName);
			if (!obj)
			{
				return false;
			}
			Objects.Add(obj);
			return true;
		}
		PathInfo FilePath;
		DsObject* FindObjectByName(const std::string& name);	//!<  Find an object in this dataset given its name
		DsObject* FindObjectByID(int id);	//!<  Find an object in this dataset given its name

		//!  Root objects, objects that have no parent (one or more from the Objects list)
		DynArray< DsObject* > RootObjects;

		//  Reset a possible previous hierarchy, call this method if you want to rebuild the hierarchy
		void ResetHierarchy();

		//  Rebuild the hierarchy of the objects from their ID and ParentID members.
		//  If a child has a ParentID<0 it is added to the RootObjects list.
		void BuildHierarchy();
		virtual ~DsDataSet();

		//!  Allocate memory for the given number of vertices before using AddVertex(), this speeds up the addition of many vertices
		void AllocateVertices(unsigned int n)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AllocateVertices(n);
		}

		//  Allocate memory for the given number of vertex indices before using AddMeshVertex(), this speeds up the addition of many vertices
		void AllocateMeshVertices(unsigned int n)
		{

			if (Objects.Size() == 0) AddObject();
			Objects.GetLast()->AllocateMeshVertices(n);
		}

	private:
		void _Init();	// [automatically generated]
		void _ConstructorInclude();	// [automatically generated]
		void _Copy(const DsDataSet& obj);	// [automatically generated]

	public:
		DsDataSet& operator =(const DsDataSet& obj);	// [automatically generated]
	};





	DsObject* FindDsObject(DsObject* dso, const std::string& name);



	///@} (DataSet)

	///@} (DS)

} // namespace gpvulc
