//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Abstract class definition for a generic object exporter.
/// @file Exporter.h
/// @author Giovanni Paolo Vigano'


#pragma once

#include "gpvulc/ds/DataSet.h"
#include "gpvulc/ds/Material.h"
#include "gpvulc/ds/Loader.h"

namespace gpvulc
{
	/// @defgroup DsExporters Exporters
	/// @brief Abstract class definition for object exporters.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup DS
	/// @{

	/// @addtogroup DsExporters
	/// @{


	//! Generic flags for exporters
	struct DsExporterFlags
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

		//!	Do not load normals
		bool NoNormals;

		//!	Do not load transformations
		bool NoTransform;

		DsExporterFlags()
		{
			ModelUsage = DS_CLONE;
			NoObjects = false;
			NoGeometry = false;
			NoHierarchy = false;
			NoMaterials = false;
			NoTextures = false;
			NoNormals = false;
			NoTransform = false;
		}
	};


	/*!
	 This is the abstract class for a generic object exporter.
	@author Giovanni Paolo Vigano'
	*/
	class DsExporter : public DsConv
	{

	public:

		//! Flags for the behaviour of the exporter
		DsExporterFlags Flags;

		/*!
		 Constructor.
		 Add the current object to the exporters list,
		*/
		DsExporter();

		/*!
		 Destructor.
		 Remove the current object from the exporters list.
		*/
		virtual ~DsExporter();

		//! Find the exporter supporting the given file extension, return nullptr if not found
		static DsExporter* FindByExt(const std::string& ext);

		/*!
		 Virtual method (generic interface) to save a file. It must be re-implemented in subclasses.
		 Save a dataset and its material library to a file using the file path
		 @param dataset (input) dataset to be saved
		 @param mtllib (input) related material library to be saved
		 @param file_name (input) path name of the file to save
		 @return true on success or false on error
		*/
		virtual bool SaveFile(DsDataSet* dataset, MtlLib* mtllib, const std::string& file_name) = 0;
	};


	/*!
	 Export a data set to a 3D model file
	 @param dataset (input) dataset to be saved
	 @param mtllib (input) related material library to be saved
	 @param file_name file path of the model to be saved
	 @return true on success or false on error
	*/
	bool DsExport(DsDataSet* dataset, MtlLib* mtllib, const std::string& file_name);

	/// @}
	/// @}
}
