//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Wavefront OBJ file exporter
/// @file ObjLoader.h
/// @author Giovanni Paolo Vigano'

#ifndef OBJEXPORTER_H_
#define OBJEXPORTER_H_

#include <gpvulc/ds/DataSet.h>
#include <gpvulc/ds/Material.h>
#include <gpvulc/ds/Exporter.h>
#include <gpvulc/ds/util/autoinit.h>
#include <fstream>

namespace gpvulc
{

	/// @addtogroup DS
	/// @{

	/// @addtogroup DsExporters
	/// @{

	//! Wavefront OBJ file format exporter
	class WObjExporter : public DsExporter
	{

		void Init();
	protected:

		std::ofstream OutFile;
		DsDataSet* DataSet;
		MtlLib* MaterialLib;
		unsigned VertCount;
		unsigned TexcCount;
		unsigned NormCount;
		unsigned VertCountOffset;
		unsigned TexcCountOffset;
		unsigned NormCountOffset;

		bool ExportMtlParams(const MtlParams& mtlpar);
		bool ExportMtlMap(const MtlTexMap& mtlmap);

		bool ExportObject(DsObject* dsobj, const Mat4& parent_mat);
		bool ExportVertCoords(const DynArray<Vec3>& Vertices,
			const DynArray<Vec3>& Normals,
			const DynArray<TexCoord>& TexCoords,
			const Mat4& parent_mat);
		bool ExportGeometry(DsGeom* dsgeom, const Mat4& parent_mat);
		bool ExportMesh(DsGeom* dsgeom, DsMesh* dsmesh);
		bool ExportVert(DsMesh* dsmesh, int i, bool use_vert_as_norms, bool use_vert_as_texc);

	public:
		WObjExporter();
		virtual ~WObjExporter();

		bool SaveFile(DsDataSet* dataset, MtlLib* mtllib, const std::string& file_name);
		bool SaveObjFile(DsDataSet* dataset, const std::string& file_name, const std::string& lib_name);
		bool SaveMtlFile(MtlLib* mtllib, const std::string& file_name);
	};

	void WObjExpInit();
	GPVULC_AUTOINITFUNC(WObjExpInit)

		/// @}
		/// @}

}

#endif//OBJEXPORTER_H_
