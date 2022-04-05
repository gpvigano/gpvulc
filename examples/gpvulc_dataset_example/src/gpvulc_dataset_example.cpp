//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Dataset testing application

#include <gpvulc/ds/DsGeomGen.h>
#include <gpvulc/ds/imp/A3dsLoader.h>   // enable Autodesk 3ds loader
#include <gpvulc/ds/imp/ObjLoader.h>   // enable Wavefront OBJ loader
#include <gpvulc/ds/exp/ObjExporter.h> // enable Wavefront OBJ exporter
#include <gpvulc/fs/FileUtil.h>
#include <gpvulc/ds/util/ConsoleMessage.h>

using namespace gpvulc;

// dataset generation (create a box with a material)
bool build_dataset(DsDataSet* dataset, MtlLib* mtllib)
{

	DynArray<DsGeoPrim> prim_list;

	DsGeoPrim prim;
	prim.Type = DSPRIM_BOX;
	prim.ShapeAlignment = DS_ALIGN_CENTER;
	prim.Size.Set(2, 3, 4);

	Material& mtl = prim.Material;
	mtl.Name = "metal";
	mtl.Parameters.Specular.Set(0.2f, 0.2f, 0.2f, 1.0f);
	mtl.Parameters.Diffuse.Set(0.4f, 0.4f, 0.4f, 1.0f);
	mtl.Parameters.Ambient.Set(0.4f, 0.4f, 0.4f, 1.0f);
	mtl.Parameters.Shininess = 10;
	MtlTexMap tex;
	tex.FileName = "refl.jpg";
	tex.Type = TEX_ENVIRONMENT;
	mtl.Maps.Add(tex);

	//prim.Type = DSPRIM_CYLINDER;
	//prim.AxisOrientation = GPV_Z;
	//prim.Flip = true;
	//prim.Material.Parameters.Opacity = 0.5f;
	//prim_list.Add( prim );

	//return CreateGeomPrimList( prim_list );

	return DsGenDatasetFromPrim(dataset, mtllib, prim, "primitive");
}


bool export_dataset()
{

	DsDataSet dataset;
	MtlLib mtllib;

	build_dataset(&dataset, &mtllib);

	// export th datase to an Wavefront OBJ file
	return DsExport(&dataset, &mtllib, "output.obj");
}


// ****************************************************************************
// main

int main(int argc, char* argv[])
{
	// Change the current working directory to match the executable file path
	ChangeCurrDirToFilePath(argv[0]);

	// set the notification level for console messages
	//SetNotifyLevel(LOG_DEBUG);
	SetNotifyLevel(LOG_VERBOSE);

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			DsDataSet dataset;
			MtlLib mtllib;
			PathInfo inFile(argv[i]);
			if (DsLoadFile(inFile.GetFullPath(), dataset, mtllib))
			{
			GPVULC_NOTIFY(LOG_DEBUG, "Data set has %d objects.\n", dataset.Objects.Size());

				PathInfo outFile(argv[i]);
				outFile.SetExt("obj");
				if (outFile == inFile)
				{
					outFile.SetPath(inFile.GetPath(), "backup");
				}
				DsExport(&dataset, &mtllib, outFile.GetFullPath());
			}
		}
	}
	else if (export_dataset())
	{

		// copy model files to the given file path
		DsCopyDatasetFiles("output.obj", "backup/output_bak.obj", true);

		// copy model files to the given directory path
		//(the ending slash is optional, slashes are converted to backslashes)
		DsCopyDatasetFiles("output.obj", "backup/", true);
	}

	// after the first run only the generated OBJ will overwrite the destination file
	// other files are skipped

	system("pause");
	return EXIT_SUCCESS;
}




