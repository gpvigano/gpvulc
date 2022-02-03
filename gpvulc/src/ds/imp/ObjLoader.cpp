//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Wavefront OBJ file loader

#include <gpvulc/ds/imp/ObjLoader.h>
#include <gpvulc/ds/Loader.h>
#include <gpvulc/ds/util/ConsoleMessage.h>
#include <gpvulc/ds/util/BusyCallback.h>
#include <fstream>

namespace gpvulc
{

	TextBuffer _WObjLastMtlName;
	TextBuffer _WObjLastGrpName;
	DsGeom* _WObjLastGeo = nullptr;
	bool WObjMtlChanged = false;

	bool WObjObjectHasUV = false;
	bool WObjObjectHasNorms = false;

	DsLoaderFlags WObjLoaderSettings;

	void WObjResetLoader()
	{
		_WObjLastMtlName.Clear();
		WObjObjectHasUV = false;
		WObjObjectHasNorms = false;
		_WObjLastGeo = nullptr;
		_WObjLastMtlName.Clear();
		_WObjLastGrpName.Clear();
	}


	bool WObjReadName(const std::string& line, TextBuffer& name)
	{
		name.Set(line);
		name.Crop(" \n\t");
		return !name.IsEmpty();
	}


	bool WObjReadName(const std::string& line, std::string& name)
	{
		TextBuffer nameBuf;
		if (!WObjReadName(line, nameBuf))
		{
			return false;
		}
		name = nameBuf.StdString();
		return true;
	}


	bool WObjReadObjectInfo(DsObject* obj, const std::string& strline)
	{
		if (obj == nullptr)
		{
			return false;
		}
		if (strline.empty() || strline[0] != 'o')
		{
			return false;
		}
		// the dataset should be split into objects sharing the same lists
		if (!WObjReadName(&strline[1], obj->Name))
		{
			return false;
		}
		GPVULC_NOTIFY(LOG_DEBUG, "Object name set: %s\n", obj->Name.Get());
		return true;
	}


	bool WObjReadGroupInfo(DsObject* obj, const std::string& strline)
	{
		if (obj == nullptr)
		{
			return false;
		}
		if (strline.empty() || strline[0] != 'g')
			return false;
		TextBuffer name;
		if (!WObjReadName(&strline[1], name))
			return false;

		_WObjLastGeo = nullptr;
		DsGeom* geo = nullptr;
		for (int i = 0; i < obj->Geoms.Size(); i++)
		{
			if (obj->Geoms[i]->Name == name)
			{
				geo = obj->Geoms[i];
				_WObjLastGeo = geo;
				WObjMtlChanged = false;//(_WObjLastMtlName != geo->Meshes.GetLast()->MtlName);
				if (geo->Meshes.Size())
					_WObjLastMtlName = geo->Meshes.GetLast()->MtlName;
				else
					_WObjLastMtlName.Clear();
				break;
			}
		}

		_WObjLastGrpName = name;
		//if( !obj->AddGeom( name ) ) return false;
		//if( obj->Geoms.Size()==0 ) {
		if (!geo)
		{
			if (!obj->AddGeom(_WObjLastGrpName))
				return false;
			_WObjLastGeo = nullptr;//obj->Geoms.GetLast();
			GPVULC_NOTIFY(LOG_VERBOSE, "Added geometry %s\n", _WObjLastGrpName);
		}
		//if( _WObjLastMtlName.Length() ) obj->SetMtl( _WObjLastMtlName );
		//WObjObjectHasUV = false;
		//WObjObjectHasNorms = false;
		GPVULC_NOTIFY(LOG_DEBUG, "Group %s selected\n", name.Get());
		return true;
	}


	bool WObjReadMtlLibInfo(DsDataSet* data, const std::string& strline)
	{
		if (!WObjReadName(&strline[6], data->MtlLibName))
		{
			return false;
		}
		GPVULC_NOTIFY(LOG_DEBUG, "Material library %s set\n", data->MtlLibName.Get());
		return true;
	}


	bool WObjReadMtlInfo(DsObject* obj, const std::string& strline)
	{
		if (obj == nullptr)
		{
			return false;
		}
		TextBuffer name;
		if (!WObjReadName(&strline[6], name))
		{
			return false;
		}
		//if( !obj->SetMtl( name ) ) return false;
		GPVULC_NOTIFY(LOG_DEBUG, "Material %s set\n", name.Get());
		WObjMtlChanged = (_WObjLastMtlName != name);
		_WObjLastMtlName = name;

		return true;
	}


#ifdef _MSC_VER
#pragma warning( push )
#pragma warning ( disable : 4996 )
#endif
	bool WObjReadVertexInfo(DsObject* obj, const std::string& strline)
	{
		if (obj == nullptr)
		{
			return false;
		}
		//if( !strline || strlen(strline)<6 || strline[0]!='v' ) return false;
		char ch = strline[1];
		float x, y, z;
		switch (ch)
		{
		case ' ':
		case '\t': // vertex coordinates
			sscanf(&strline[2], "%f%f%f", &x, &y, &z);
			GPVULC_NOTIFY(LOG_VERBOSE, "v %f %f %f\n", x, y, z);
			obj->SharedVertices.Add(Vec3(x, y, z));
			//obj->AddVertex(Vec3(x,y,z));
			break;
		case 'n': // normal
			sscanf(&strline[2], "%f%f%f", &x, &y, &z);
			GPVULC_NOTIFY(LOG_VERBOSE, "vn %f %f %f\n", x, y, z);
			obj->SharedNormals.Add(Vec3(x, y, z));
			//obj->AddNormal(Vec3(x,y,z));
			WObjObjectHasNorms = true;
			break;
		case 't': // texture coordinate
			sscanf(&strline[2], "%f%f", &x, &y);
			GPVULC_NOTIFY(LOG_VERBOSE, "tn %f %f\n", x, y);
			obj->SharedTexCoords.Add(TexCoord(x, y));
			//obj->AddTexCoord(TexCoord(x,y));
			WObjObjectHasUV = true;
			break;
		}
		return true;
	}
#ifdef _MSC_VER
#pragma warning( pop )
#endif


	bool WObjReadFaceInfo(DsObject* obj, const std::string& strLine)
	{

		if (obj == nullptr)
		{
			return false;
		}
		if (strLine.empty() || strLine.size() < 3)
		{
			return false;
		}
		char facetype = strLine[0];
		size_t start = strLine.find_first_not_of(" \t");
		TextBuffer strline;
		strline.Crop(" \t", " \t\r\n");
		int numverts = 0;

		std::vector<std::string> params = strline.Split(" \t");
		numverts = (int)params.size();

		DsMeshType mt;
		switch (facetype)
		{
		case 'f':
			switch (numverts)
			{
			case 3:
				mt = DS_TRIANGLES;
				break;
			case 4:
				mt = DS_QUADS;
				break;
			default:
				mt = DS_POLYGON;
				break;
			}
			break;
		case 'l':
			mt = DS_LINE_STRIP;
			break;
		case 'p':
			mt = DS_POINTS;
			break;
		default:
			return false;
		}

		if (obj->Geoms.Size() == 0)
		{
			if (!obj->AddGeom(_WObjLastGrpName))
				return false;
			GPVULC_NOTIFY(LOG_VERBOSE, "Added geometry to group %s\n", _WObjLastGrpName.Get());
		}

		DsGeom* geom = _WObjLastGeo;
		if (!geom)
			_WObjLastGeo = geom = obj->Geoms.GetLast();
		if (!geom)
			return false;

		//bool copy_mtl = ( geom->Meshes.Size() &&
		//    geom->Meshes.GetLast()->GetMeshVerts() &&
		//    geom->Meshes.GetLast()->GetMeshVerts() != numverts );
		if (geom->Meshes.Size() == 0 || WObjMtlChanged || mt != geom->Meshes.GetLast()->GetType() || geom->Meshes.GetLast()->GetMeshVerts() != numverts)
		{
			GPVULC_NOTIFY(LOG_VERBOSE, "Added mesh %d to geometry %s, group %s\n",
				geom->Meshes.Size(), geom->Name.Get(), _WObjLastGrpName.Get());
			DsMesh* mesh(0);
			if (geom->Meshes.Size())
			{
				mesh = geom->Meshes.GetLast();
				if (mesh)
				{
					mesh->VertIdx.ShrinkToFit();
					mesh->NormIdx.ShrinkToFit();
					mesh->TexCoordIdx.ShrinkToFit();
				}
			}
			geom->AddMesh();
			mesh = geom->Meshes.GetLast();
			if (mesh)
			{
				mesh->VertIdx.Allocate(numverts * geom->Vertices.Size());
				mesh->NormIdx.Allocate(numverts * geom->Vertices.Size());
				mesh->TexCoordIdx.Allocate(numverts * geom->Vertices.Size());
			}
			if (_WObjLastMtlName.Length())
			{
				geom->SetMtl(_WObjLastMtlName);
				//geom->Meshes.GetLast()->MtlName = _WObjLastMtlName;
			}
			GPVULC_NOTIFY(LOG_VERBOSE, "Assigned material %s to mesh\n", _WObjLastMtlName.Get());
			WObjMtlChanged = false;
		}

		//printf("NUMVERTS=%d\n",numverts);Sleep(100);
		DsMesh* mesh = geom->Meshes.GetLast();
		if (!mesh)
			return false;

		// parse vertices
		//---------------
		const unsigned char  vc_bit = 0x01;
		const unsigned char  tc_bit = 0x02;
		const unsigned char  nc_bit = 0x04;
		const unsigned char  vcnc_bit = vc_bit | nc_bit;
		const unsigned char  vctc_bit = vc_bit | tc_bit;
		const unsigned char  vctcnc_bit = vc_bit | tc_bit | nc_bit;
		unsigned char  pattern = vc_bit;
		bool use_t(false), use_n(false);

		if (WObjObjectHasNorms && strline.Contains("//"))
		{
			pattern = vcnc_bit;
			use_n = true;
		}
		else if (WObjObjectHasUV && WObjObjectHasNorms)
		{
			pattern = vctcnc_bit;
			use_t = use_n = true;
		}
		else if (WObjObjectHasUV)
		{
			pattern = vctc_bit;
			use_t = true;
		}

		int v, t, n;
		for (int i = 0; i < numverts; i++)
		{
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
			switch (pattern)
			{
			case vc_bit:
				//v = std::stoi(params[i]);
				sscanf(params[i].c_str(), "%d", &v);
				break;
			case vcnc_bit:
				sscanf(params[i].c_str(), "%d//%d", &v, &n);
				break;
			case vctcnc_bit:
				sscanf(params[i].c_str(), "%d/%d/%d", &v, &t, &n);
				break;
			case vctc_bit:
				sscanf(params[i].c_str(), "%d/%d", &v, &t);
				break;
			}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

			if (v < 0) v = obj->SharedVertices.Size() + v;// geom->Vertices.Size()+v;
			else v--;

			if (mesh->VertIdx.Size() == mesh->VertIdx.Dimension())
				mesh->VertIdx.Allocate(2 * mesh->VertIdx.Size());
			mesh->VertIdx.Add(v);

			if (use_t)
			{
				if (t < 0) t = obj->SharedTexCoords.Size() + t;// geom->TexCoords.Size()+t;
				else t--;
				if (mesh->TexCoordIdx.Size() == mesh->TexCoordIdx.Dimension())
					mesh->TexCoordIdx.Allocate(2 * mesh->TexCoordIdx.Size());
				mesh->TexCoordIdx.Add(t);
			}
			if (use_n)
			{
				if (n < 0) n = obj->SharedNormals.Size() + n;// geom->Normals.Size()+n;
				else n--;
				if (mesh->NormIdx.Size() == mesh->NormIdx.Dimension())
					mesh->NormIdx.Allocate(2 * mesh->NormIdx.Size());
				mesh->NormIdx.Add(n);
			}
		}

		mesh->SetType(mt);
		return true;
	}


	bool WObjMtlReadMap(const std::string& name, int type, MtlTexMap& m)
	{
		if (!WObjReadName(name, m.FileName))
			return false;
		if (m.FileName.empty())
			return false;
		if (m.FileName[0] == '/') m.FileName.insert(0, ".");
		m.Type = type;
		return true;;
	}


	bool WObjLoadMtlLib(const std::string& filename, MtlLib* mtllib)
	{
		if (filename.empty())
			return false;
		if (mtllib == nullptr)
			return false;
		std::ifstream inStr(filename);
		if (!inStr.good())
			return false;
		PathInfo f(filename);
		mtllib->Name = f.GetFullName();
		GPVULC_NOTIFY(LOG_DEBUG, "Material library: %s.\n", (const std::string&)(mtllib->Name));
		char buf[512] = { 0 };
		TextBuffer strline;

		gvCallBusyCallBack("loading material library");

		Material* currmtl = &(MtlLib::DefaultMaterial);
		MtlTexMap m;
		float a = 1.0f;
		while (inStr.good())
		{

			float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f, s = 0.0f;
			strline.ReadLine(inStr);
			strline.Crop();
			if (!inStr.good() && strline.IsEmpty())
				break;

			switch (strline[0])
			{
			case 'n':
				if (strline.StartsWith("newmtl"))
				{
					gvCallBusyCallBack("loading materials");

					TextBuffer name;
					if (!WObjReadName(&strline[6], name)) break;
					Material m;
					m.Name = name;
					mtllib->Materials.Add(m);
					currmtl = &(mtllib->Materials.GetLast());
					currmtl->Parameters.Ambient.A = 1;
					currmtl->Parameters.Diffuse.A = 1;
					currmtl->Parameters.Specular.A = 1;
					currmtl->Parameters.TwoSided = true;//false; // to be checked
					GPVULC_NOTIFY(LOG_DEBUG, "Material %s added.\n", (const std::string&)(m.Name));
				}
				break;

			case 'm':
				if (currmtl && strline.StartsWith("map_"))
				{
					switch (strline[4])
					{
					case 'N':
						switch (strline[5])
						{
						case 's': // specular exponent map
							if (WObjMtlReadMap(&strline[6], TEX_SHININESS, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Specular exp map %s added.\n", (const std::string&)(m.FileName));
							}
							break;
						default:
							break;
						}
						break;
					case 'K':
					case 'k':
						switch (strline[5])
						{
						case 'a': //ambient map
						case 'A': //ambient map
							if (WObjMtlReadMap(&strline[6], TEX_REFL_LEV, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Ambient map %s added.\n", (const std::string&)(m.FileName));
							}
							break;
						case 'D': //diffuse map
						case 'd': //diffuse map
							if (WObjMtlReadMap(&strline[6], TEX_DIFFUSE, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Diffuse map %s added.\n", (const std::string&)(m.FileName));
							}
							break;
						case 'S': //specular map
						case 's': //specular map
							if (WObjMtlReadMap(&strline[6], TEX_SPECULAR, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Specular map %s added.\n", (const std::string&)(m.FileName));
							}
							break;
						default:
							break;
						}
						break;
					case 'd':
					case 'D': //opacity map
						if (WObjMtlReadMap(&strline[5], TEX_OPACITY, m))
						{
							currmtl->Maps.Add(m);
							GPVULC_NOTIFY(LOG_DEBUG, "Opacity map %s added.\n", (const std::string&)(m.FileName));
						}
						break;
					case 'o': //opacity map
						if (strline.StartsWith("map_opacity"))
						{
							if (WObjMtlReadMap(&strline[11], TEX_OPACITY, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Opacity map %s added.\n", (const std::string&)(m.FileName));
							}
						}
						break;
					case 'b':
					case 'B': //bump map
						if (strline.StartsWith("map_bump", true))
						{
							if (WObjMtlReadMap(&strline[8], TEX_BUMP, m))
							{
								currmtl->Maps.Add(m);
								GPVULC_NOTIFY(LOG_DEBUG, "Bump map %s added.\n", (const std::string&)(m.FileName));
							}
						}
						break;
					default:
						break;
					}
				}
				break;
			case 'r': //reflection map
				if (strline.StartsWith("refl"))
				{
					if (WObjMtlReadMap(&strline[4], TEX_ENVIRONMENT, m))
					{
						currmtl->Maps.Add(m);
						GPVULC_NOTIFY(LOG_DEBUG, "Reflection map %s added.\n", (const std::string&)(m.FileName));
					}
				}
				break;


#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
			case 'd': //alpha
				if (currmtl)
				{
					sscanf(&strline[1], "%f", &a);
					currmtl->Parameters.Opacity = a;
					currmtl->Parameters.Ambient.A = a;
					currmtl->Parameters.Diffuse.A = a;
					currmtl->Parameters.Specular.A = a;
				}
				break;

			case 'K':
				switch (strline[1])
				{
				case 'a': //ambient
					if (currmtl)
					{
						sscanf(&strline[2], "%f%f%f", &r, &g, &b);
						GPVULC_NOTIFY(LOG_VERBOSE, "Ka %f %f %f\n", r, g, b);
						currmtl->Parameters.Ambient.Set(r, g, b, 1);//currmtl->Parameters.Opacity);
					}
					break;
				case 'd': //diffuse
					if (currmtl)
					{
						sscanf(&strline[2], "%f%f%f", &r, &g, &b);
						GPVULC_NOTIFY(LOG_VERBOSE, "Kd %f %f %f\n", r, g, b);
						currmtl->Parameters.Diffuse.Set(r, g, b, 1);//currmtl->Parameters.Opacity);
					}
					break;
				case 's': //specular
					if (currmtl)
					{
						sscanf(&strline[2], "%f%f%f", &r, &g, &b);
						GPVULC_NOTIFY(LOG_VERBOSE, "Ks %f %f %f\n", r, g, b);
						currmtl->Parameters.Specular.Set(r, g, b, 1);//currmtl->Parameters.Opacity);
					}
					break;
				default:
					break;
				}
				break;
			case 'N':
				switch (strline[1])
				{
				case 's': //shininess
					if (currmtl)
					{
						sscanf(&strline[2], "%f", &s);
						GPVULC_NOTIFY(LOG_VERBOSE, "Ns %f\n", s);
						currmtl->Parameters.Shininess = s;
					}
					break;
				}
				break;
#ifdef _MSC_VER
#pragma warning( pop )
#endif
			default:
				break;
			}
		}

		inStr.close();

		return true;
	}


	bool WObjLoadModel(const std::string& filename, DsDataSet* data)
	{
		if (data == nullptr)
			return false;
		std::ifstream inStr(filename);
		if (!inStr.good())
			return false;

		WObjResetLoader();

		TextBuffer strline;
		char ch = 0;

		// compute the vertex data arrays size to speed up memory allocation
		unsigned int verts_count = 0, norms_count = 0, texs_count = 0, tot_faces_count = 0;

		while (strline.ReadSplitLine(inStr))
		{

			switch (strline[0])
			{
			case 'v':

				switch (strline[1])
				{
				case ' ':
				case '\t':
					verts_count++;
					break;
				case 'n':
					norms_count++;
					break;
				case 't':
					texs_count++;
					break;
				}
				break;
			case 'f':
			case 'l':
			case 'p':
				tot_faces_count++;
				break;

			default:
				break;
			}
		}
		inStr.seekg(0, std::ios::beg);


		data->AddObject();

		DsObject& obj = *(data->Objects.GetLast());
		obj.ShareData(true);

		obj.SharedVertices.Allocate(verts_count);
		obj.SharedNormals.Allocate(norms_count);
		if (texs_count)
		{
			obj.SharedTexCoords.Allocate(texs_count);
		}
		GPVULC_NOTIFY(LOG_DEBUG, "Data set has %d vertices, %d normals, %d UV coords, %d faces\n",
			verts_count, norms_count, texs_count, tot_faces_count);

		TextBuffer msg(80, false);
		unsigned faces_counter = 0;
		unsigned verts_counter = 0;
		// Parse the .obj file
		while (strline.ReadSplitLine(inStr))
		{

			// Get the beginning character of the current line in the .obj file
			ch = strline[0];

			switch (ch)
			{

			case 'v':           // (vertex/normal/texture_coord)

			  // Decipher this line to see if it's a vertex ("v"), normal ("vn"), or UV coordinate ("vt")
				WObjReadVertexInfo(&obj, strline);
				verts_counter++;
				if (verts_counter % 1000)
				{
					msg.Clear();
					msg << "loading vertices: " << verts_counter;
					gvCallBusyCallBack(msg.Get());
				}
				break;

			case 'o':           // (object)

				gvCallBusyCallBack("loading objects");
				WObjReadObjectInfo(&obj, strline);
				break;

			case 'g':           // (group)

				gvCallBusyCallBack("loading groups");
				WObjReadGroupInfo(&obj, strline);
				break;

			case 'u':           // (use material)

				if (strline.StartsWith("usemtl")) WObjReadMtlInfo(&obj, strline);
				break;

			case 'm':           // (material library)

				if (strline.StartsWith("mtllib")) WObjReadMtlLibInfo(data, strline);
				break;

			case 'f':           // face
			case 'l':           // line
			case 'p':           // point
				WObjReadFaceInfo(&obj, strline);
				faces_counter++;
				if (faces_counter % 1000)
				{
					msg.Clear();
					msg << "loading faces: " << faces_counter;
					gvCallBusyCallBack(msg.Get());
				}
				break;

				//case '\n':

				//  // If we read in a newline character, we've encountered a blank line in
				//  // the .obj file.  We don't want to do the default case and skip another
				//  // line, so we just break and do nothing.
				//  break;

			default:

				// If we get here then we don't care about the line being read, so read past it.
				break;
			}

		}

		inStr.close();
		//if( data->Name.IsEmpty() ) {
		//  PathInfo objfile (filename);
		//  data->Name = objfile.GetFullName();
		//}
		data->FilePath.SetFullPath(filename);
		//obj.ShareData( false );

		return true;
	}


	// Setup the data set and its materials
	bool WObjSetupData(DsDataSet* data, MtlLib* mtllib)
	{

		gvCallBusyCallBack("setting up object");
		//if( !WObjSetupDataSet( data ) ) return false;
		//if( !WObjSetupMtlLib( mtllib ) ) return false;
		//if( data && mtllib ) mtllib->Name = data->Name;
		for (int i = 0; i < data->Objects.Size(); i++)
		{
			DsObject* object = data->Objects[i];
			data->RootObjects.Add(object);
			for (int j = 0; j < object->Geoms.Size(); j++)
			{
				DsGeom* geom = object->Geoms[j];
				geom->Vertices.LinkTo(object->SharedVertices);
				if (object->SharedNormals.Size())
					geom->Normals.LinkTo(object->SharedNormals);
				if (object->SharedTexCoords.Size())
					geom->TexCoords.LinkTo(object->SharedTexCoords);
				if (object->SharedColors.Size())
					geom->Colors.LinkTo(object->SharedColors);
			}
		}

		return true;
	}


	bool WObjLoadFile(const std::string& filename, DsDataSet* data, MtlLib* mtllib, DsLoaderFlags flags)
	{

		WObjLoaderSettings = flags;
		if (data && !WObjLoadModel(filename, data))
		{
			GPVULC_NOTIFY(LOG_WARN, "Wavefront OBJ loader: failed to load model: %s.\n", filename);
			return false;
		}
		if (!WObjLoaderSettings.NoMaterials && mtllib && data->MtlLibName.Length())
		{
			PathInfo f(filename);
			f.SetFullName(data->MtlLibName);
			if (!WObjLoadMtlLib(f.GetFullPath(), mtllib))
			{
				GPVULC_NOTIFY(LOG_WARN, "Wavefront OBJ loader: failed to load material library: %s.\n", f.GetFullPath());
				return false;
			}
		}

		if (!WObjSetupData(data, mtllib))
			return false;
		return true;
	}


	void WObjInit()
	{
		static WObjLoader gvWObjLoader;
	}

}
