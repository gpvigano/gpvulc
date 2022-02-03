//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Wavefront OBJ file loader
/// @file ObjLoader.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <gpvulc/ds/DataSet.h>
#include <gpvulc/ds/Material.h>
#include <gpvulc/ds/Loader.h>
#include <gpvulc/ds/util/autoinit.h>

namespace gpvulc
{

	/// @addtogroup DS
	/// @{

	/// @addtogroup DsLoaders
	/// @{

	/*!
	 Load the Wavefront OBJ model file with the given path into a dataset
	 @param filename the path of the 3D model file (.obj) to be loaded
	 @param data the dataset to be filled (if nullptr the function returns false)
	 @return true on success, false on error
	*/
	bool WObjLoadModel(const std::string& filename, DsDataSet* data);


	/*!
	 Load the Wavefront OBJ material library file with the given path into a material library
	 @param filename the path of the material library file (.mtl) to be loaded
	 @param mtllib the material library to be filled (if nullptr the function returns false)
	 @return true on success, false on error
	*/
	bool WObjLoadMtlLib(const std::string& filename, MtlLib* mtllib);


	/*!
	 Load the Wavefront OBJ model file and material library with the given path into the given dataset and material library
	 @param filename the path of the 3D model file to be loaded
	 @param data the dataset to be filled (if nullptr it will be discarded)
	 @param mtllib the material library to be filled (if nullptr it will be discarded)
	 @param flags loader settings, see DsLoaderFlags
	 @return true on success, false on error
	*/
	bool WObjLoadFile(const std::string& filename, DsDataSet* data, MtlLib* mtllib, DsLoaderFlags flags);


	//! Wavefront OBJ file format loader
	class WObjLoader : public DsLoader
	{

	public:

		WObjLoader()
		{
			UpAxis = DS_Y;
			MapExt("obj");
		}

		bool LoadFile(const std::string& filename, DsDataSet* data, MtlLib* mtllib)
		{
			return WObjLoadFile(filename, data, mtllib, Flags);
		}

	};

	void WObjInit();
	GPVULC_AUTOINITFUNC(WObjInit)

		/// @}
		/// @}

}

