//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Abstract class definition for a generic object loader.
/// @file Loader.h
/// @author Giovanni Paolo Vigano'


#pragma once

#include "gpvulc/ds/DataSet.h"
#include "gpvulc/ds/Material.h"

namespace gpvulc
{

	/// @defgroup DsLoaders Loaders
	/// @brief Abstract class definition for object loaders.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{



	/*!
	 Abstract class for a generic object loader or exporter.
	@author Giovanni Paolo Vigano'
	*/
	class DsConv
	{

		//! List of supported file extensions
		DynArray< TextBuffer > ExtList;

	protected:

		//!< The "up" axis of the world. It can be DS_X, DS_Y OR DS_Z (default = DS_Y)
		int UpAxis;

		/*!
		 Internal matrix used to convert a Z-up system into a Y-up one:
		 1  0  0  0 <BR>
		 0  0  1  0 <BR>
		 1 -1  0  0 <BR>
		 1  0  0  1 <BR>
		*/
		Mat4 UpZtoUpYmat;

	public:

		/*!
		 Constructor.
		 initialize UpZtoUpYmat and set UpAxis to DS_Y
		*/
		DsConv();

		//! Destructor.
		virtual ~DsConv();

		//void SetUpAxis( int axis ) { if( axis==DS_X || axis==DS_Y || axis==DS_Z ) UpAxis = axis; }

		//! Get the current up axis (default = DS_Y)
		int GetUpAxis() { return UpAxis; }

		//! Check if the given file extension is supported by this loader
		bool SupportedExt(const std::string& ext);

		//! Map the given extension to this loader, call it in constructor for each supported extension
		void MapExt(const std::string& ext);

	};


	/// @addtogroup DsLoaders
	/// @{


	//! Generic flags for loaders
	struct DsLoaderFlags
	{

		/*!
			 Set the	method used to convert the dataset
		   DS_USE	      -	use the	dataset	directly (it can be used only once in the scene)
		   DS_COPY      -	an exact duplicate of the dataset is used
		   [DS_CLONE]   -	the hierarchy is copied	and the	geometry is referenced directly
		*/
		int ModelUsage;

		//!	Do not load objects
		bool NoObjects;

		//!	Do not load geometry
		bool NoGeometry;

		//!	Do not load hierarchy information
		bool NoHierarchy;

		//!	Do not load materials and textures
		bool NoMaterials;

		//!	Do not load textures
		bool NoTextures;

		//!	Try to build mipmaps for textures
		bool BuildMipmaps;

		//!	Do not load normals
		bool NoNormals;

		//!	Do not load normals when RTT diffuse maps are present
		bool NoNormalsIfRTT;

		//!	Do not load transformations
		bool NoTransform;

		//!	Bake pivot transformations (leave pivot offset)
		bool BakePivotTransform;

		//!	Do not load animations
		bool NoAnimation;

		DsLoaderFlags()
		{
			ModelUsage = DS_CLONE;
			NoObjects = false;
			NoGeometry = false;
			NoHierarchy = false;
			NoMaterials = false;
			NoTextures = false;
			BuildMipmaps = false;
			NoNormals = false;
			NoNormalsIfRTT = false;
			NoTransform = false;
			NoAnimation = false;
			BakePivotTransform = false;
		}
	};



	/*!
	 This is the abstract class for a generic object loader.
	@author Giovanni Paolo Vigano'
	*/
	class DsLoader : public DsConv
	{

	public:

		//! Flags for the behaviour of the loader
		DsLoaderFlags Flags;

		/*!
		 Constructor.
		 Add the current object to the loader list,
		 initialize UpZtoUpYmat and set UpAxis to DS_Y
		*/
		DsLoader();

		/*!
		 Destructor.
		 Remove the current object from the loader list.
		*/
		virtual ~DsLoader();

		//! Find the loader supporting the given file extension, return nullptr if not found
		static DsLoader* FindByExt(const std::string& ext);

		/*!
		 Virtual method (generic interface) to load a file. It must be re-implemented in subclasses.
		 Load a model from a file using the given model pointer (or creating a new one if omitted)
		 @param file_name (input) path name of the file to load
		 @param dataset (output) dataset to be loaded
		 @param mtllib (output) related material library to be loaded
		 @return true on success or false on error
		*/
		virtual bool LoadFile(const std::string& file_name, DsDataSet* dataset, MtlLib* mtllib) = 0;
	};

	/*!
	 Get a list of files included by the given 3D model file (excluding the file itself).
	 Paths are relative to the given file path.
	*/
	bool DsLoadFile(const std::string& file_name, DsDataSet& dataset, MtlLib& mtllib, DsLoaderFlags* flags = nullptr);


	//@}

	/*!
	 Get a list of files included by the given dataset and the given library (including the library, if present)
	 Paths are relative to the given file path.
	*/
	bool DsListIncludedFiles(DsDataSet* dataset, MtlLib* mtllib, DynArray<TextBuffer>& file_list);

	/*!
	 Get a list of files included by the given 3D model file (excluding the file itself).
	 Paths are relative to the given file path.
	*/
	bool DsListIncludedFiles(const std::string& file_name, DynArray<TextBuffer>& file_list);

	/*!
	 Copy a dataset file with all included files to another path
	 @param src_file_path path of the file to copy
	 @param dst_file_path path name of the destination
	 @param overwrite_newer overwrite also newer files
	*/
	bool DsCopyDatasetFiles(PathInfo src_file_path, PathInfo dst_file_path, bool overwrite_newer);

	/*!
	 Copy a dataset file with all included files to another path
	 @param src_file_path path of the file to copy
	 @param dst_file_path path name of the destination
	 @param overwrite_newer overwrite also newer files
	*/
	inline bool DsCopyDatasetFiles(const std::string& srcFile, const std::string& dstFile, bool overwriteNewer)
	{
		PathInfo src_file_path(srcFile);
		PathInfo dst_file_path(dstFile);
		return DsCopyDatasetFiles(src_file_path, dst_file_path, overwriteNewer);
	}

	/*!
	 Copy a dataset file with all included files to another path
	 or only the file itself if it is not a supported 3D format,
	 create intermediate path if needed, update only if the destination file is older,
	 return false on error.
	 @param src_file_path path of the file to copy
	 @param dst_file_path path name of the destination
	 @param overwrite_newer overwrite also newer files
	*/
	bool DsCopyFiles(const std::string& src_file_path, const std::string& dst_file_path, bool overwrite_newer);

	/// @}
	/// @}

} // namespace gpvulc
