//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Data set

#include <gpvulc/ds/DataSet.h>

namespace gpvulc
{

	DsObject* FindDsObject(DsObject* dso, const std::string& name)
	{

		if (dso->Name == name)
		{
			return dso;
		}
		for (int i = 0; i < dso->Children.Size(); i++)
		{
			DsObject* found = FindDsObject(dso->Children[i], name);
			if (found)
				return found;
		}

		return nullptr;
	}




	//------------------------------------------------------------------------------
	// Classes implementation
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	//  DsMesh

	DsMesh::DsMesh()
	{

		_ConstructorInclude();
	}


	DsMesh::DsMesh(const DsMesh& _gvdsmesh)
	{

		_ConstructorInclude();
		_Copy(_gvdsmesh);
	}


	DsMeshType DsMesh::GetType()
	{

		return Type;
	}


	void DsMesh::SetType(DsMeshType _type)
	{

		Type = _type;
		MeshVerts = 0; // not defined
		switch (Type)
		{
		case DS_POINTS:    MeshVerts = 1; break;
		case DS_LINES:     MeshVerts = 2; break;
		case DS_TRIANGLES: MeshVerts = 3; break;
		case DS_QUADS:     MeshVerts = 4; break;
		default: break;
		}
	}


	int DsMesh::GetMeshVerts()
	{

		return MeshVerts;
	}


	int DsMesh::GetNumFaces()
	{

		if (MeshVerts == 0)
		{
			return 0;
		}
		return VertIdx.Size() / MeshVerts;
	}


	DsMesh::~DsMesh()
	{


	}


	void DsMesh::_Init()
	{

		Type = DS_TRIANGLES;
		MeshVerts = 3;
		MtlName.Clear();
	}


	void DsMesh::_ConstructorInclude()
	{

		_Init();
	}


	void DsMesh::_Copy(const DsMesh& obj)
	{

		Type = obj.Type;
		MeshVerts = obj.MeshVerts;
		VertIdx = obj.VertIdx;
		NormIdx = obj.NormIdx;
		ColorIdx = obj.ColorIdx;
		TexCoordIdx = obj.TexCoordIdx;
		MtlName = obj.MtlName;
		InverseTangentMatrix = obj.InverseTangentMatrix;
		SmoothingGroups = obj.SmoothingGroups;
		LocalMatrix = obj.LocalMatrix;
		AbsoluteMatrix = obj.AbsoluteMatrix;
	}


	DsMesh& DsMesh::operator =(const DsMesh& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  DsGeom

	DsGeom::DsGeom(DsObject* _obj, const std::string& _name)
	{

		_ConstructorInclude();
		Object = _obj;
		Name = _name;
		LinkToObject();
	}


	DsGeom::DsGeom()
	{

		_ConstructorInclude();
	}


	DsGeom::DsGeom(const DsGeom& _gvdsgroup)
	{

		_ConstructorInclude();
		_Copy(_gvdsgroup);
	}


	void DsGeom::SetMtl(const std::string& _name)
	{

		if (_name.empty())
		{
			return;
		}
		if (Meshes.Size() == 0) AddMesh();
		if (Meshes.GetLast()->VertIdx.Size()) AddMesh();
		Meshes.GetLast()->MtlName = _name;
	}


	bool DsGeom::LinkToObject(DsObject* obj)
	{

		if (obj) Object = obj;
		if (!Object)
		{
			return false;
		}
		if (Object->SharedData())
		{
			Vertices.LinkTo(Object->SharedVertices);
			Normals.LinkTo(Object->SharedNormals);
			Colors.LinkTo(Object->SharedColors);
			TexCoords.LinkTo(Object->SharedTexCoords);
			Transforms.LinkTo(Object->SharedTransforms);
		}
		else
		{
			Vertices.Unlink();
			Normals.Unlink();
			Colors.Unlink();
			TexCoords.Unlink();
			Transforms.Unlink();
		}
		return true;
	}


	DsGeom::~DsGeom()
	{

	}


	void DsGeom::_Init()
	{

		Name.Clear();
		Object = nullptr;
	}


	void DsGeom::_ConstructorInclude()
	{

		Object = nullptr;
		_Init();
	}


	void DsGeom::_Copy(const DsGeom& obj)
	{

		Name = obj.Name;
		Meshes = obj.Meshes;
		Object = obj.Object;
		Vertices = obj.Vertices;
		Normals = obj.Normals;
		Colors = obj.Colors;
		TexCoords = obj.TexCoords;
		Transforms = obj.Transforms;
	}


	DsGeom& DsGeom::operator =(const DsGeom& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  DsSkinJointWeight

	DsSkinJointWeight::DsSkinJointWeight()
	{

		Weight = 0.0f;
		JointIdx = 0;
	}




	//------------------------------------------------------------------------------
	//  DsObject

	DsObject::DsObject(DsDataSet* _ds, const std::string& _name)
	{

		_ConstructorInclude();
		DataSet = _ds;
		Name = _name;
	}


	DsObject::DsObject(const DsObject& _gvdsobject)
	{

		_ConstructorInclude();
		_Copy(_gvdsobject);
	}


	void DsObject::CopyDS(int mode, DsObject* obj)
	{

		DsObject& ds = *obj;

		switch (mode)
		{
		case DS_USE:
		case DS_COPY:
			//Vertices.LinkTo( ds.Vertices );
			//Normals.LinkTo( ds.Normals );
			//Colors.LinkTo( ds.Colors );
			//TexCoords.LinkTo( ds.TexCoords );
			switch (mode)
			{
			case DS_USE:
				Geoms.LinkTo(ds.Geoms);
				break;
			case DS_COPY:
				Geoms.Copy(ds.Geoms.GetData());
				break;
			}
			break;
		case DS_CLONE:
			(*this) = ds;
			break;
		}
	}


	bool DsObject::SetMtl(const std::string& _name)
	{

		if (_name.empty())
		{
			return false;
		}
		if (Geoms.Size() == 0) AddGeom();
		Geoms.GetLast()->SetMtl(_name);
		return true;
	}


	DsObject::DsObject()
	{

		_ConstructorInclude();
	}


	void DsObject::ShareData(bool __shareddata)
	{

		_SharedData = __shareddata;
		for (int i = 0; i < Geoms.Size(); i++)
			Geoms[i]->LinkToObject(this);
	}


	bool DsObject::SharedData()
	{

		return _SharedData;
	}


	void DsObject::UpdateGeoms()
	{

		for (int i = 0; i < Geoms.Size(); i++)
			Geoms[i]->Object = this;
	}


	DsObject::~DsObject()
	{

	}


	void DsObject::_Init()
	{

		Name.Clear();
		DataSet = nullptr;
		ID = -1;
		ParentID = -1;
		_SharedData = false;
		Animation = nullptr;
		Parent = nullptr;
		IsDummy = false;
		Skin = nullptr;
		IsJoint = false;
	}


	void DsObject::_ConstructorInclude()
	{

		DataSet = nullptr;

		Animation = nullptr;

		Parent = nullptr;

		Skin = nullptr;
		_Init();
	}


	void DsObject::_Copy(const DsObject& obj)
	{

		BakedMatrix = obj.BakedMatrix;
		Geoms = obj.Geoms;
		Name = obj.Name;
		Transform = obj.Transform;
		DataSet = obj.DataSet;
		ID = obj.ID;
		ParentID = obj.ParentID;
		SharedVertices = obj.SharedVertices;
		SharedNormals = obj.SharedNormals;
		SharedColors = obj.SharedColors;
		SharedTexCoords = obj.SharedTexCoords;
		SharedTransforms = obj.SharedTransforms;
		_SharedData = obj._SharedData;
		Animation = obj.Animation;
		Parent = obj.Parent;
		Children = obj.Children;
		PivotOffset = obj.PivotOffset;
		IsDummy = obj.IsDummy;
		Skin = obj.Skin;
		IsJoint = obj.IsJoint;
	}


	DsObject& DsObject::operator =(const DsObject& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  DsDataSet

	DsDataSet::DsDataSet(const DsDataSet& _gvdsdataset)
	{

		_ConstructorInclude();
		_Copy(_gvdsdataset);
	}


	bool DsDataSet::SetMtl(const std::string& _name)
	{

		if (_name.empty())
		{
			return false;
		}
		if (Objects.Size() == 0)
		{
			if (!AddObject())
			{
				return false;
			}
		}
		Objects.GetLast()->SetMtl(_name);
		return true;
	}


	DsObject* DsDataSet::FindObjectByName(const std::string& name)
	{

		for (int i = 0; i < Objects.Size(); i++)
		{
			if (Objects[i]->Name == name)
				return Objects[i];
		}
		return nullptr;
	}


	DsObject* DsDataSet::FindObjectByID(int id)
	{

		for (int i = 0; i < Objects.Size(); i++)
		{
			if (Objects[i]->ID == id)
				return Objects[i];
		}
		return nullptr;
	}


	void DsDataSet::ResetHierarchy()
	{

		RootObjects.RemoveAll(true);
		int i;

		for (i = 0; i < Objects.Size(); i++)
		{
			Objects[i]->Children.RemoveAll(true);
			Objects[i]->Parent = nullptr;
		}
	}


	void DsDataSet::BuildHierarchy()
	{

		// if the RootObject list is already filled the hierarchy was already built (maybe also externally)
		if (RootObjects.Size())
		{
			return;
		}

		int i, j;

		for (i = 0; i < Objects.Size(); i++)
		{
			DsObject* dsobj = Objects[i];
			bool is_root = true;
			for (j = 0; j < Objects.Size(); j++)
			{
				if (i != j && Objects[j]->Children.Has(dsobj))
				{
					is_root = false;
					break;
				}
			}
			// if ID < 0 the node is the root
			if (!is_root) continue;
			if (dsobj->ParentID < 0)
				RootObjects.AddUnique(dsobj);
			// else attach it to the object with the id equal to the parent id of this object
			else
			{
				for (j = 0; j < Objects.Size(); j++)
				{
					if (i != j && dsobj->ParentID == Objects[j]->ID)
					{
						Objects[j]->Children.AddUnique(dsobj);
						dsobj->Parent = Objects[j];
						break;
					}
				}
			}
		}
	}

	unsigned DsDataSetCount_ = 0;

	DsDataSet::~DsDataSet()
	{

		DsDataSetCount_--;
	}


	void DsDataSet::_Init()
	{

		MtlLibName.Clear();
		Name.Clear();
		RootObjects.Clear();
	}

	void DsDataSet::_ConstructorInclude()
	{

		_Init();
		DsDataSetCount_++;
	}


	void DsDataSet::_Copy(const DsDataSet& obj)
	{

		ConversionMatrix = obj.ConversionMatrix;
		MtlLibName = obj.MtlLibName;
		Objects = obj.Objects;
		Name = obj.Name;
		FilePath = obj.FilePath;
		RootObjects = obj.RootObjects;
	}


	DsDataSet& DsDataSet::operator =(const DsDataSet& obj)
	{

		_Copy(obj);
		return *this;
	}

} // namespace gpvulc


