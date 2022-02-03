//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Autodesk 3ds file format loader
/// @file A3dsLoader.h
/// @author Giovanni Paolo Vigano'


#ifndef __A3DSLOADER_H
#define __A3DSLOADER_H

#include "gpvulc/ds/DataSet.h"
#include "gpvulc/ds/Material.h"
#include "gpvulc/ds/Loader.h"
#include "gpvulc/ds/util/autoinit.h"

namespace gpvulc
{

	/// @addtogroup DS
	/// @{

	/// @addtogroup DsLoaders
	/// @{

	/*!
	 Load the Autodesk 3DS model file with the given path into a dataset and a material library
	 @param filename the path of the 3D model file to be loaded
	 @param data the dataset to be filled (if nullptr it will be discarded)
	 @param mtllib the material library to be filled (if nullptr it will be discarded)
	 @param flags loader settings, see DsLoaderFlags
	 @return true on success, false on error
	*/
	bool Load3dsFile(const std::string& filename, DsDataSet* data, MtlLib* mtllib, DsLoaderFlags flags);

	//! Autodesk 3DS file format loader
	class A3ds_Loader : public DsLoader
	{

	public:

		A3ds_Loader()
		{
			UpAxis = DS_Z;
			MapExt("3ds");
		}
		bool LoadFile(const std::string& file_name, DsDataSet* dataset, MtlLib* mtllib)
		{
			return Load3dsFile(file_name, dataset, mtllib, Flags);
		}
	};


	//void A3dsLoaderInit();
	//static struct _gvA3ds_LoaderInitializer {
	//  _gvA3ds_LoaderInitializer() {
	//    A3dsLoaderInit();
	//  }
	//} __gvA3ds_LoaderInitializer;

	void A3dsLoaderInit();
	GPVULC_AUTOINITFUNC(A3dsLoaderInit)
}

		/// @}
		/// @}

#endif//__A3DSLOADER_H
