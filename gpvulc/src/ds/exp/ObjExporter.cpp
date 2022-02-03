//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Wavefront OBJ file exporter

#include <gpvulc/ds/MtlData.h>
#include <gpvulc/ds/GeoData.h>
#include <gpvulc/ds/math/mathdefs.h>
#include <gpvulc/ds/util/ConsoleMessage.h>
#include <gpvulc/ds/ver.h>

#include <gpvulc/ds/exp/ObjExporter.h>

namespace gpvulc
{


	WObjExporter::WObjExporter()
	{
		UpAxis = DS_Y;
		MapExt("obj");
		Init();
	}


	void WObjExporter::Init()
	{
		DataSet = nullptr;
		MaterialLib = nullptr;
		VertCount = 0;
		TexcCount = 0;
		NormCount = 0;
		VertCountOffset = 0;
		TexcCountOffset = 0;
		NormCountOffset = 0;
	}


	WObjExporter::~WObjExporter()
	{
	}


	bool WObjExporter::SaveFile(DsDataSet* dataset, MtlLib* mtllib, const std::string& file_name)
	{

		PathInfo file_path(file_name);
		PathInfo lib_path(file_name);
		lib_path.SetExt("mtl");

		if (!SaveMtlFile(mtllib, lib_path.GetFullPath()))
			return false;
		if (!SaveObjFile(dataset, file_path.GetFullPath(), lib_path.GetFullName()))
			return false;

		return true;
	}


	bool WObjExporter::SaveMtlFile(MtlLib* mtllib, const std::string& file_name)
	{

		Init();

		OutFile.open(file_name, std::ios::out);
		if (!OutFile)
		{
			GPVULC_NOTIFY(LOG_WARN, "Failed to open MTL output file: %s\n", file_name);
			return false;
		}

		OutFile << "\
# \n\
# Wavefront OBJ material library file\n\
# Exported by GPVULC-DS " << DS_CURRENT_VERSION << "\n\
# CNR-STIIMA\n\
# http://www.stiima.cnr.it/\n\
# \n";

		OutFile << "# material library " << mtllib->Name << std::endl;

		int nmat = mtllib->Materials.Size();
		int matcount = 0;
		for (int i = 0; i < nmat; i++)
		{
			OutFile << "newmtl " << mtllib->Materials[i].Name << std::endl;
			ExportMtlParams(mtllib->Materials[i].Parameters);
			for (int j = 0; j < mtllib->Materials[i].Maps.Size(); j++)
			{
				ExportMtlMap(mtllib->Materials[i].Maps[j]);
			}
		}

		OutFile.close();
		return true;
	}

	bool WObjExporter::ExportMtlParams(const MtlParams& mtlpar)
	{
		OutFile << "Ka " << mtlpar.Ambient.R << " " << mtlpar.Ambient.G << " " << mtlpar.Ambient.B << std::endl;
		OutFile << "Kd " << mtlpar.Diffuse.R << " " << mtlpar.Diffuse.G << " " << mtlpar.Diffuse.B << std::endl;
		OutFile << "Ks " << mtlpar.Specular.R << " " << mtlpar.Specular.G << " " << mtlpar.Specular.B << std::endl;
		OutFile << "Ns " << mtlpar.Shininess << std::endl;
		if (mtlpar.Opacity < 1.0f)
		{
			OutFile << "d " << mtlpar.Opacity << std::endl;
		}
		return true;
	}


	bool WObjExporter::ExportMtlMap(const MtlTexMap& mtlmap)
	{

		std::string maptype;
		switch (mtlmap.Type)
		{
		case TEX_DIFFUSE:
			maptype = "map_Kd";
			break;
		case TEX_OPACITY:
			//maptype = "map_opacity";
			maptype = "map_D";
			break;
		case TEX_BUMP:
			maptype = "map_Bump";
			break;
		case TEX_SPECULAR:
			maptype = "map_Ks";
			break;
		case TEX_SHININESS:
			maptype = "map_Ns";
			break;
		case TEX_REFL_LEV:
			maptype = "map_Ka";
			break;
		case TEX_ENVIRONMENT:
			maptype = "refl";
			break;
		}
		if (!maptype.empty())
		{
			OutFile << maptype << " " << mtlmap.FileName << std::endl;
		}
		return true;
	}





	bool WObjExporter::SaveObjFile(DsDataSet* dataset, const std::string& file_name, const std::string& lib_name)
	{

		Init();

		OutFile.open(file_name, std::ios::out);
		if (!OutFile)
		{
			GPVULC_NOTIFY(LOG_WARN, "Failed to open OBJ output file: %s\n", file_name);
			return false;
		}

		OutFile << "\
# \n\
# Wavefront OBJ file\n\
# Exported by GPVULC-DS " << DS_CURRENT_VERSION << "\n\
# CNR-STIIMA\n\
# http://www.stiima.cnr.it/\n\
# \n";

		OutFile << "mtllib " << lib_name << std::endl;

		bool result = true;
		int i;
		for (i = 0; i < dataset->Objects.Size(); i++)
		{
			DsObject* dsobj = dataset->Objects[i];
			if (!ExportObject(dsobj, dsobj->Transform.GetMatrix()))
				result = false;
		}
		OutFile.close();

		return result;
	}


	bool WObjExporter::ExportObject(DsObject* dsobj, const Mat4& parent_mat)
	{
		int i;
		Mat4 mat(parent_mat);
		if (dsobj->Transform.IsActive())
			mat = parent_mat*dsobj->Transform.GetMatrix();
		OutFile << "# object " << dsobj->Name << std::endl;

		if (dsobj->SharedVertices.Linked())
			ExportVertCoords(dsobj->SharedVertices, dsobj->SharedNormals, dsobj->SharedTexCoords, parent_mat);
		for (i = 0; i < dsobj->Geoms.Size(); i++)
		{
			ExportGeometry(dsobj->Geoms[i], mat);
		}

		for (i = 0; i < dsobj->Children.Size(); i++)
		{
			ExportObject(dsobj->Children[i], mat);
		}
		return true;
	}


	bool WObjExporter::ExportGeometry(DsGeom* dsgeom, const Mat4& parent_mat)
	{

		OutFile << "g " << dsgeom->Name << std::endl;
		if (!dsgeom->Vertices.Linked())
			ExportVertCoords(dsgeom->Vertices, dsgeom->Normals, dsgeom->TexCoords, parent_mat);

		for (int j = 0; j < dsgeom->Meshes.Size(); j++)
		{

			ExportMesh(dsgeom, dsgeom->Meshes[j]);
		}

		return true;
	}


	bool WObjExporter::ExportVertCoords(const DynArray<Vec3>& vertices,
		const DynArray<Vec3>& normals,
		const DynArray<TexCoord>& texCoords,
		const Mat4& parent_mat)
	{
		int i = 0;
		Vec3 v, n, t;

		Mat4 mat(parent_mat);
		//if ( dsgeom->Meshes.Size() && !dsgeom->Meshes[0]->AbsoluteMatrix.IsIdentity() )
		//  mat = dsgeom->Meshes[0]->AbsoluteMatrix.Inverted();
		Mat4 rot;
		if (!mat.IsIdentity())
			rot = mat.GetRotMat();
		bool mat_id = mat.IsIdentity();
		bool rot_id = rot.IsIdentity();
		VertCountOffset += VertCount;
		TexcCountOffset += TexcCount;
		NormCountOffset += NormCount;


		if (vertices.Size())
		{
			OutFile << "# " << vertices.Size() << " vertices" << std::endl;
			for (i = 0; i < vertices.Size(); i++)
			{
				if (mat_id)
					v = vertices[i];
				else
					MatPointTransform(vertices[i], mat, v);
				OutFile << "v " << v.X << " " << v.Y << " " << v.Z << std::endl;
				VertCount++;
			}
		}

		if (normals.Size())
		{
			OutFile << "# " << vertices.Size() << " normals" << std::endl;
			for (i = 0; i < normals.Size(); i++)
			{
				if (rot_id)
					n = normals[i];
				else
					n = rot*normals[i];
				OutFile << "vn " << n.X << " " << n.Y << " " << n.Z << std::endl;
				NormCount++;
			}
		}

		if (texCoords.Size())
		{
			OutFile << "# " << vertices.Size() << " texture coordinates" << std::endl;
			for (i = 0; i < texCoords.Size(); i++)
			{
				OutFile << "vt " << texCoords[i].U << " " << texCoords[i].V << std::endl;
				TexcCount++;
			}
		}

		return true;
	}


	bool WObjExporter::ExportMesh(DsGeom* dsgeom, DsMesh* dsmesh)
	{

		int num_verts = dsmesh->VertIdx.Size();
		int prim_verts = dsmesh->GetMeshVerts();
		if (prim_verts == 0) prim_verts = num_verts;
		int num_prims = num_verts / prim_verts;
		bool use_vert_as_norms = dsmesh->NormIdx.Size() == 0 && dsgeom->Normals.Size() == dsgeom->Vertices.Size();
		bool use_vert_as_texc = dsmesh->TexCoordIdx.Size() == 0 && dsgeom->TexCoords.Size() == dsgeom->Vertices.Size();

		if (!dsmesh->MtlName.IsEmpty())
		{
			OutFile << "usemtl " << dsmesh->MtlName << std::endl;
		}

		std::string typetag;
		switch (dsmesh->GetType())
		{
		case DS_TRIANGLE_FAN:
			prim_verts = 3;
			typetag = "f";
			break;
		case DS_TRIANGLES:
		case DS_QUADS:
		case DS_POLYGON:
			typetag = "f";
			break;
		case DS_LINE_LOOP:
		case DS_LINES:
		case DS_LINE_STRIP:
			typetag = "l";
			break;
		case DS_POINTS:
			typetag = "p";
			break;
		case DS_TRIANGLE_STRIP:
			// TODO: faces must be splitted
			GPVULC_NOTIFY(LOG_WARN, "OBJ exporter: triangle strips not yet supported!\n");
			return false;
		case DS_QUAD_STRIP:
			// TODO: faces must be splitted
			GPVULC_NOTIFY(LOG_WARN, "OBJ exporter: quad strips not yet supported!\n");
			return false;
		default:
			// TODO: faces must be splitted
			GPVULC_NOTIFY(LOG_WARN, "OBJ exporter: unknown mesh type!\n");
			return false;
		}

		// skip the center of the triangle fan
		if (dsmesh->GetType() == DS_TRIANGLE_FAN)
		{
			OutFile << "# triangle fan" << std::endl;
			OutFile << typetag;
			for (int i = 1; i < num_verts - 1; i++)
			{
				ExportVert(dsmesh, 0, use_vert_as_norms, use_vert_as_texc);
				ExportVert(dsmesh, i, use_vert_as_norms, use_vert_as_texc);
				ExportVert(dsmesh, i + 1, use_vert_as_norms, use_vert_as_texc);
			}
			ExportVert(dsmesh, 0, use_vert_as_norms, use_vert_as_texc);
			ExportVert(dsmesh, num_verts - 1, use_vert_as_norms, use_vert_as_texc);
			ExportVert(dsmesh, 1, use_vert_as_norms, use_vert_as_texc);
			OutFile << std::endl;
		}
		else
		{
			for (int i = 0; i < num_verts; )
			{
				int start(i), end(i + prim_verts - 1);
				//export_obj_prim( OutFile, dsmesh, start, end );
				OutFile << typetag;
				for (int j = start; j <= end; j++, i++)
				{

					ExportVert(dsmesh, j, use_vert_as_norms, use_vert_as_texc);
				}
				// repeat last vertex in case of line loop
				if (dsmesh->GetType() == DS_LINE_LOOP)
				{
					ExportVert(dsmesh, start, use_vert_as_norms, use_vert_as_texc);
				}

				OutFile << std::endl;
			}
		}
		return true;
	}


	bool WObjExporter::ExportVert(DsMesh* dsmesh, int i, bool use_vert_as_norms, bool use_vert_as_texc)
	{

		OutFile << " " << (dsmesh->VertIdx[i] + VertCountOffset + 1);
		if (dsmesh->TexCoordIdx.Size())
		{
			OutFile << "/" << (dsmesh->TexCoordIdx[i] + TexcCountOffset + 1);
		}
		else if (use_vert_as_texc)
		{
			OutFile << "/" << (dsmesh->VertIdx[i] + VertCountOffset + 1);
		}

		if (use_vert_as_norms)
		{
			OutFile << "/" << (dsmesh->VertIdx[i] + VertCountOffset + 1);
		}
		else if (dsmesh->NormIdx.Size())
		{
			if (!dsmesh->TexCoordIdx.Size()) OutFile << "/";
			OutFile << "/" << (dsmesh->NormIdx[i] + NormCountOffset + 1);
		}
		return true;
	}



	void WObjExpInit()
	{
		static WObjExporter WObjExporter;
	}

}
