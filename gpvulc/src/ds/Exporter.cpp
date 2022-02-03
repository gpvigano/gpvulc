//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Abstract class definition for a generic object exporter.


#include <map>
#include <gpvulc/ds/Exporter.h>

namespace
{

	gpvulc::DynArray< gpvulc::DsExporter* >* DsExporterList = nullptr;

	void DsInitExporterList()
	{
		static gpvulc::DynArray< gpvulc::DsExporter* > ExporterList;
		if (!DsExporterList) DsExporterList = &ExporterList;
	}
}

namespace gpvulc
{

	// DsExporter class implementation
	//----------------------------------------

	DsExporter::DsExporter()
	{
		DsInitExporterList();
		DsExporterList->AddUnique(this);
	}


	DsExporter::~DsExporter()
	{
		if (!DsExporterList)
		{
			return;
		}

		DsExporterList->RemoveElem(this);
	}



	DsExporter* DsExporter::FindByExt(const std::string& ext)
	{
		if (!DsExporterList)
		{
			return nullptr;
		}

		// search backward to give higher priority to the last created loader
		for (int i = (*DsExporterList).Size() - 1; i >= 0; i--)
		{
			if ((*DsExporterList)[i]->SupportedExt(ext))
			{
				return (*DsExporterList)[i];
			}
		}

		return nullptr;
	}


	bool DsExport(DsDataSet* dataset, MtlLib* mtllib, const std::string& file_name)
	{
		PathInfo f(file_name);

		DsExporter* exporter = DsExporter::FindByExt(f.GetExt());
		if (!exporter)
		{
			return false;
		}
		return exporter->SaveFile(dataset, mtllib, f.GetFullPath());
	}

} // namespace gpvulc

