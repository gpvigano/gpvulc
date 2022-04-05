//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Abstract class definition for a generic object loader.


#include <map>
#include <gpvulc/ds/Loader.h>
#include <gpvulc/fs/FileUtil.h>
#include <gpvulc/ds/util/BusyCallback.h> // gvCallBusyCallBack
#include <gpvulc/ds/util/ConsoleMessage.h> // GPVULC_NOTIFY

using namespace gpvulc;

DynArray< DsLoader* >* DsLoaderList = nullptr;


void DsInitLoaderList()
{
	static DynArray< DsLoader* > LoaderList;
	if (!DsLoaderList)
	{
		DsLoaderList = &LoaderList;
	}
}



// DsConv class implementation
//----------------------------------------

DsConv::DsConv()
{

	UpZtoUpYmat.SetRow(1, Vec4(0, 0, 1, 0));
	UpZtoUpYmat.SetRow(2, Vec4(0, -1, 0, 0));
	UpAxis = DS_Y;
}


DsConv::~DsConv()
{
}


bool DsConv::SupportedExt(const std::string& ext)
{

	for (int i = 0; i < ExtList.Size(); i++)
		if (ExtList[i].EqualTo(ext, true))
			return true;

	return false;
}


void DsConv::MapExt(const std::string& ext)
{

	ExtList.AddUnique(TextBuffer(ext));
}


// DsLoader class implementation
//----------------------------------------

DsLoader::DsLoader()
{

	DsInitLoaderList();
	DsLoaderList->AddUnique(this);
}


DsLoader::~DsLoader()
{

	if (!DsLoaderList)
	{
		return;
	}

	DsLoaderList->RemoveElem(this);
}


DsLoader* DsLoader::FindByExt(const std::string& ext)
{

	if (!DsLoaderList)
		return nullptr;

	// search backward to give higher priority to the last created loader
	for (int i = (*DsLoaderList).Size() - 1; i >= 0; i--)
		if ((*DsLoaderList)[i]->SupportedExt(ext))
		{
			return (*DsLoaderList)[i];
		}

	return nullptr;
}

//---------------------------------------------------------------------

namespace
{
	inline const char* GetValidString(const std::string& str)
	{
		return str.empty() ? "" : str.c_str();
	}
}


bool gpvulc::DsLoadFile(const std::string& file_name, DsDataSet& dataset, MtlLib& mtllib, DsLoaderFlags* flags)
{
	PathInfo f(file_name);
	DsLoader* loader = DsLoader::FindByExt(f.GetExt());
	if (loader == nullptr)
	{
		GPVULC_NOTIFY(LOG_WARN, "3D model type not supported: %s.\n", !f.GetExt().empty() ? f.GetExt() : "(no extension)");
		return false;
	}

	// load the data set

	dataset.FilePath = f;

	if(flags)
	{
		loader->Flags = *flags;
	}
	bool result = loader->LoadFile(f.GetFullPath(), &dataset, &mtllib);

	return result;
}


bool gpvulc::DsListIncludedFiles(DsDataSet* dataset, MtlLib* mtllib, DynArray<TextBuffer>& file_list)
{

	if (!dataset && !mtllib)
		return false;

	if (dataset)
	{
		//const std::string& file_path = dataset->FilePath.GetFullPath();
		//if( file_path && *file_path )
		//  file_list.Add( file_path );
		if (dataset->MtlLibName.Length())
		{
			PathInfo mlf(dataset->MtlLibName);
			mlf.SetRelativeToPath(dataset->FilePath.GetPath());
			file_list.AddUnique(TextBuffer(mlf.GetFullPath()));
		}
	}

	if (mtllib)
	{
		PathInfo mlf(mtllib->Name);
		if (!mlf.GetExt().empty())
		{
			if (dataset)
				mlf.SetRelativeToPath(dataset->FilePath.GetPath());
			if (!dataset || mlf != dataset->FilePath)
				file_list.AddUnique(TextBuffer(mlf.GetFullPath()));
		}
		for (int i = 0; i < mtllib->Materials.Size(); i++)
		{
			for (int j = 0; j < mtllib->Materials[i].Maps.Size(); j++)
			{
				if (mtllib->Materials[i].Maps[j].FileName.size())
				{
					PathInfo mf(mtllib->Materials[i].Maps[j].FileName);
					if (dataset)
						mf.SetRelativeToPath(dataset->FilePath.GetPath());
					file_list.AddUnique(TextBuffer(mf.GetFullPath()));
				}
			}
		}
	}

	return true;
}


bool gpvulc::DsListIncludedFiles(const std::string& file_name, DynArray<TextBuffer>& file_list)
{

	PathInfo f(file_name);
	DsLoader* loader = DsLoader::FindByExt(f.GetExt());
	if (loader == nullptr)
	{
		GPVULC_NOTIFY(LOG_WARN, "3D model type not supported: %s.\n", !f.GetExt().empty() ? f.GetExt() : "(no extension)");
		return false;
	}
	// load the data set
	DsDataSet dataset;

	// load the material library
	MtlLib mtllib;

	dataset.FilePath = f;

	DsLoaderFlags flags = loader->Flags;
	loader->Flags.NoGeometry = true;
	loader->Flags.NoHierarchy = true;
	loader->Flags.NoObjects = true;
	loader->Flags.NoTransform = true;
	bool result = loader->LoadFile(f.GetFullPath(), &dataset, &mtllib);
	loader->Flags = flags;

	if (!result)
		return false;

	return gpvulc::DsListIncludedFiles(&dataset, &mtllib, file_list);
}


bool gpvulc::DsCopyDatasetFiles(PathInfo src_file_name, PathInfo dst_file_name, bool overwrite_newer)
{

	if (!dst_file_name.Defined())
	{
		GPVULC_NOTIFY(LOG_WARN, "Destination path not defined.\n");
		return false;
	}

	if (!FileExists(src_file_name.GetFullPath()))
	{
		GPVULC_NOTIFY(LOG_WARN, "Source path not accessible: %s.\n", GetValidString(src_file_name.GetFullPath()));
		return false;
	}

	if (dst_file_name.GetName().empty())
		dst_file_name.SetFullName(src_file_name.GetFullName());
	gvCallBusyCallBack("parsing model file");
	DynArray<TextBuffer> file_names;
	if (!DsListIncludedFiles(src_file_name.GetFullPath(), file_names))
	{
		GPVULC_NOTIFY(LOG_WARN, "Failed to get file list for: %s.\n", GetValidString(src_file_name.GetFullPath()));
		return false;
	}
	gvCallBusyCallBack("copying files");
	if (!FileUpdate(src_file_name, dst_file_name, overwrite_newer))
	{
		return false;
	}

	////PathInfo icon_file;
	////icon_file.SetName(src_file_name.GetName(),"_icon",src_file_name.GetExt());
	////file_names.AddUnique(TextBuffer(icon_file.GetFullName()));


	//CreateDir(dst_file_name.GetPath(),true);
	//if( gvFileCompareByTime( src_file_name.GetFullPath(), dst_file_name.GetFullPath() )==0 ) {
	//  GPVULC_NOTIFY( LOG_DEBUG, "  File up-to-date: %s\n", dst_file_name.GetFullPath() );
	//} else {
	//  if( !gvFileCopy( src_file_name.GetFullPath(), dst_file_name.GetFullPath() ) ) {
	//    GPVULC_NOTIFY( LOG_WARN, "Failed to copy: %s\n         to: %s\n", src_file_name.GetFullPath(), dst_file_name.GetFullPath() );
	//    return false; // if the main file was not copied abort
	//  }
	//  GPVULC_NOTIFY( LOG_DEBUG, "Copied file: %s\nto:         %s\n\n", src_file_name.GetFullPath(), dst_file_name.GetFullPath() );
	//}
	for (int i = 0; i < file_names.Size(); i++)
	{
		PathInfo src(file_names[i]);
		PathInfo dst(file_names[i]);
		src.SetPath(src_file_name.GetPath(), src.GetPath());
		dst.SetPath(dst_file_name.GetPath(), dst.GetPath());
		if (!FileUpdate(src, dst, overwrite_newer))
		{
			continue;
		}
		//if( gvFileCompareByTime( src.GetFullPath(), dst.GetFullPath() )==0 ) {
		//  GPVULC_NOTIFY( LOG_DEBUG, "  File up-to-date: %s\n", dst.GetFullPath() );
		//  continue;// no need to update the file, go on
		//}
		//if( !gvFileCopy( src.GetFullPath(), dst.GetFullPath() ) ) {
		//  GPVULC_NOTIFY( LOG_WARN, "Copy failed: %s\n", src.GetFullPath() );
		//  continue;// even if a file was not copied go on
		//}
		GPVULC_NOTIFY(LOG_DEBUG, "  Copied file: %s\n  to:          %s\n\n",
			GetValidString(src.GetFullPath()), GetValidString(dst.GetFullPath()));
	}

	return true;
}



bool gpvulc::DsCopyFiles(const std::string& src_file_name, const std::string& dst_file_name, bool overwrite_newer)
{

	PathInfo src(src_file_name);
	PathInfo dst(dst_file_name);
	if (src.GetFullName().empty())
	{
		GPVULC_NOTIFY(LOG_WARN, "3D model file name is null.\n");
		return false;
	}
	DsLoader* loader = DsLoader::FindByExt(src.GetExt());
	if (loader == nullptr)
	{
		GPVULC_NOTIFY(LOG_NOTICE, "3D model type not supported: %s.\n", !src.GetExt().empty() ? src.GetExt() : "(no extension)");
		GPVULC_NOTIFY(LOG_NOTICE, "Copying only the given file: %s.\n", src.GetFullName().c_str());
		return FileUpdate(src_file_name, dst_file_name, overwrite_newer);
	}
	return DsCopyDatasetFiles(src, dst, overwrite_newer);
}
