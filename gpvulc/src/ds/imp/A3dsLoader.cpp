//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// Autodesk 3ds file loader


#include <gpvulc/path/PathInfo.h> // PathInfo
//#include <sys/gvruntime.h> // gvCallBusyCallBack()

#include <gpvulc/ds/imp/A3dsLoader.h>
#include <gpvulc/ds/Loader.h>
//#include <gfx/Shape.h>
#include <gpvulc/ds/math/mathutil.h>
#include <gpvulc/ds/util/ConsoleMessage.h>
#include <gpvulc/ds/util/BusyCallback.h>
#include <gpvulc/time/Chrono.h>
//#include <util/preproc.h>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif

namespace gpvulc
{

	const int gvA3ds_UpAxis = DS_Y;
	int VertexCounter = 0;
	DsLoaderFlags gvA3ds_LoaderFlags;

	//******************************************************************************
	// CONSTANTS USED TO IDENTIFY THE CHUNKS
	//******************************************************************************

	// the 3ds useful chunks, in a tree-like fashion as they're found inside the real file
#define   GV3DS_MAIN                                       0x4D4D
#define     GV3DS_EDITOR                                     0x3D3D
#define       GV3DS_EDITOR_OBJECT                        0x4000
#define         GV3DS_EDITOR_OBJECT_MESH                   0x4100
#define           GV3DS_EDITOR_OBJECT_MESH_VERTICES          0x4110
#define           GV3DS_EDITOR_OBJECT_MESH_FACES             0x4120
#define           GV3DS_EDITOR_OBJECT_MESH_MATERIAL          0x4130
#define           GV3DS_EDITOR_OBJECT_MESH_TEXTURE_COORDS    0x4140
#define           GV3DS_EDITOR_OBJECT_MESH_SMOOTHING_GROUPS  0x4150
#define           GV3DS_EDITOR_OBJECT_MESH_LOCAL_AXES        0x4160
#define       GV3DS_EDITOR_MATERIAL                            0xAFFF
#define         GV3DS_EDITOR_MATERIAL_NAME                       0xA000
#define         GV3DS_EDITOR_MATERIAL_AMBIENT                    0xA010
#define         GV3DS_EDITOR_MATERIAL_DIFFUSE                    0xA020
#define         GV3DS_EDITOR_MATERIAL_SPECULAR                   0xA030
#define         GV3DS_EDITOR_MATERIAL_GLOSSINESS                 0xA040
#define         GV3DS_EDITOR_MATERIAL_SHININESS                  0xA041
#define         GV3DS_EDITOR_MATERIAL_TRANSPARENCY               0xA050
//#define         GV3DS_EDITOR_MATERIAL_XP_FALL                    0xA052
//#define         GV3DS_EDITOR_MATERIAL_REFLECTION_BLUR            0xA053
#define         GV3DS_EDITOR_MATERIAL_2_SIDED                    0xA081
#define         GV3DS_EDITOR_MATERIAL_EMISSION                   0xA084
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_DIFFUSE            0xA200
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_OPACITY            0xA210
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_BUMP               0xA230
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_SPECULAR           0xA204
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_SHININESS          0xA33C
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_SELF_ILLUMINATION  0xA33D
#define         GV3DS_EDITOR_MATERIAL_TEXTURE_ENVIRONMENT        0xA220
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_FILE               0xA300
//#define           GV3DS_EDITOR_MATERIAL_TEXTURE_TILING     0xA353
//#define           GV3DS_EDITOR_MATERIAL_TEXTURE_FILTERING_BLUR_OLD 0xA352
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_FILTERING_BLUR     0xA353
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_U_SCALING          0xA354
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_V_SCALING          0xA356
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_U_OFFSET           0xA358
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_V_OFFSET           0xA35A
#define           GV3DS_EDITOR_MATERIAL_TEXTURE_ROTATION           0xA35C
#define     GV3DS_KEYFRAMER                          0xB000
#define       GV3DS_KEYFRAMER_OBJECT                   0xB002
//#define       GV3DS_KEYFRAMER_MESH                     0xB002
//#define         GV3DS_KEYFRAMER_SPOT_LIGHT               0xB007
//#define         GV3DS_KEYFRAMER_FRAMES_START_END         0xB008
#define         GV3DS_KEYFRAMER_OBJECT_HIERARCHY         0xB010
#define         GV3DS_KEYFRAMER_OBJECT_INSTANCE_NAME     0xB011
#define         GV3DS_KEYFRAMER_OBJECT_PRESCALE          0xB012
#define         GV3DS_KEYFRAMER_OBJECT_PIVOT_OFFSET      0xB013
#define         GV3DS_KEYFRAMER_OBJECT_TRANSLATION       0xB020
#define         GV3DS_KEYFRAMER_OBJECT_ROTATION          0xB021
#define         GV3DS_KEYFRAMER_OBJECT_SCALING           0xB022
#define         GV3DS_KEYFRAMER_OBJECT_NUMBER            0xB030
//#define         GV3DS_KEYFRAMER_OBJECT_HIERARCHY            0xB030
#define       GV3DS_KEYFRAMER_START_END_FRAMES           0xB008

// other useful 3ds chunks. these are present anywhere inside other chunks
#define   GV3DS_RGB_CHUNK    0x0011
#define   GV3DS_AMOUNT_OF    0x0030

// Frames per second in 3ds
#define   GV3DS_FPS    30.0

/*
struct gvA3ds_Flags {
  bool FileLoaded = false;
  bool SmoothingFlag=true;
  bool UseMipMapsFlag=false;
  bool TexCompFlag=false;
  bool TexCacheFlag=false;
  bool TexCacheClearFlag=false;
};
double LoadingTime=0;
double KeyFramerTimeScaling = 0.0333333333333333333;
*/


//*****************************************************************************
// DATA
//*****************************************************************************

// chunk structure
	struct gvA3ds_Chunk
	{

		unsigned short int ID;
		unsigned int Size;
	};


	//*****************************************************************************
	// BINARY DATA READING
	//*****************************************************************************

	// 32 - 16 - 8 bit values
	inline void gvA3ds_Load32Bit(FILE *fp, float &n) { fread(&n, 4, 1, fp); }
	inline void gvA3ds_Load32Bit(FILE *fp, long &n) { fread(&n, 4, 1, fp); }
	inline void gvA3ds_Load32Bit(FILE *fp, unsigned long &n) { fread(&n, 4, 1, fp); }
	inline void gvA3ds_Load16Bit(FILE *fp, short &n) { fread(&n, 2, 1, fp); }
	inline void gvA3ds_Load16Bit(FILE *fp, unsigned short &n) { fread(&n, 2, 1, fp); }
	inline void gvA3ds_Load8Bit(FILE *fp, char &n) { fread(&n, 1, 1, fp); }
	inline void gvA3ds_Load8Bit(FILE *fp, unsigned char &n) { fread(&n, 1, 1, fp); }


	// a generic float value
	inline bool gvA3ds_ParseFloat(FILE* fp, float& v)
	{

		// loads the float value
		gvA3ds_Load32Bit(fp, v);

		return true;
	}

	// a 3D vector
	inline bool gvA3ds_ParseVec3(FILE* fp, Vec3& v)
	{

		// loads the 3 values (4 byte each) in the format XYZ
		if (gvA3ds_UpAxis == DS_Y)
		{
			float x, y, z;
			gvA3ds_Load32Bit(fp, x);
			gvA3ds_Load32Bit(fp, y);
			gvA3ds_Load32Bit(fp, z);
			v.X = x;
			v.Y = z;
			v.Z = -y;
			//} else if ( gvA3ds_UpAxis==DS_X )...
		}
		else
		{
			gvA3ds_Load32Bit(fp, v.X);
			gvA3ds_Load32Bit(fp, v.Y);
			gvA3ds_Load32Bit(fp, v.Z);
		}

		return true;
	}


	// read a generic RGB triplet into a ColorRGBA
	inline void gvA3ds_ParseAmountOf(FILE* fp, float& val)
	{

		unsigned short v;

		// loads the 3 values (1 byte each) in the format R-G-B
		gvA3ds_Load16Bit(fp, v);
		val = (float)v;
	}



	// read a generic RGB triplet into a ColorRGBA
	inline bool gvA3ds_ParseRgbColor(FILE* fp, ColorRGBA& col)
	{

		unsigned char r, g, b;

		// loads the 3 values (1 byte each) in the format R-G-B
		gvA3ds_Load8Bit(fp, r); gvA3ds_Load8Bit(fp, g); gvA3ds_Load8Bit(fp, b);
		col.Set(r / 255.0f, g / 255.0f, b / 255.0f, col.A);

		return true;
	}

	// a generic short integer (2 bytes)

	bool gvA3ds_ParseString(FILE* fp, std::string& str)
	{
		// save the current position and calculates the length of the name
		long start = ftell(fp);
		unsigned int name_size = 0;
		do name_size++; while (fgetc(fp) != 0);

		// Resets the position into the file and creates the name array
		fseek(fp, start, SEEK_SET);
		char *name;
		if ((name = new char[name_size]) == nullptr)
		{
			return false;
		}

		// loads the name
		unsigned int i;
		for (i = 0; i < name_size; i++) name[i] = fgetc(fp);

		str = name;
		delete[] name;

		return true;
	}

	bool gvA3ds_ParseString(FILE* fp, TextBuffer& txt)
	{
		std::string str;
		if (!gvA3ds_ParseString(fp, str))
		{
			return false;
		}
		txt = str;
		return true;
	}

	//*****************************************************************************
	// CHUNK functions
	//*****************************************************************************

	const char* gvA3ds_UnknownChunkDescription(unsigned int id)
	{
		switch (id)
		{
		case 0x0010: return "color float";
		case 0x0100: return "editor: configuration 1";
		case 0x1100: return "editor: bitmap";
		case 0x1101: return "editor: use bitmap";
		case 0x1200: return "editor: solid background";
		case 0x1201: return "editor: use solid background";
		case 0x1300: return "editor: v gradient";
		case 0x1301: return "editor: use v gradient";
		case 0x1400: return "editor: lo shadow bias";
		case 0x1410: return "editor: hi shadow bias";
		case 0x1420: return "editor: shadow map size";
		case 0x1430: return "editor: shadow samples";
		case 0x1440: return "editor: shadow range";
		case 0x1450: return "editor: shadow filter";
		case 0x1460: return "editor: ray bias";
		case 0x1500: return "editor: consts";
		case 0x2000: return "editor: unknown";
		case 0x2100: return "editor: ambient light";
		case 0x2200: return "editor: fog";
		case 0x2201: return "editor: fog: use fog";
		case 0x2210: return "editor: fog: fog background";
		case 0x2300: return "editor: fog: distance cue";
		case 0x2302: return "editor: fog: layer fog";
		case 0x2303: return "editor: fog: use layer fog";
		case 0x2310: return "editor: dcue bgnd";
		case 0x2d2d: return "editor: smagic";
		case 0x2d3d: return "editor: lmagic";
		case 0x3e3d: return "editor: configuration 2";
		case 0x4600: return "editor: light";
		case 0x4620: return "editor: light: off";
		case 0x4610: return "editor: light: spot";
		case 0x465a: return "editor: light: unknown";
		case 0x4700: return "editor: camera";
		case 0x4710: return "editor: camera: see cone";
		case 0x4720: return "editor: camera: ranges";
		case 0x7012: return "editor: view: 1";
		case 0x7011: return "editor: view: 2";
		case 0x7020: return "editor: view: 3";
		case 0x7001: return "editor: view1";
		case 0xa052: return "editor: material: xp fall";
		case 0xa053: return "editor: material: reflection blur";
		case 0xa087: return "editor: material: wireframe size";
		case 0xa08a: return "editor: material: xp falling";
		case 0xa100: return "editor: material: shading";
		case 0xAFFF: return "editor: material: entry";
		case 0xB00A: return "editor: keyframe: HDR";
		case 0xB009: return "editor: keyframe: current frame";
		case 0xB011: return "editor: keyframe: dummy name";
		case 0xB013: return "editor: keyframe: pivot";
		case 0xB014: return "editor: keyframe: bounding box";
		case 0xB015: return "editor: keyframe: morph smooth";
		case 0xB021: return "editor: keyframe: rotation track tag";
		case 0xB022: return "editor: keyframe: scaling track tag";
		case 0xB020: return "editor: keyframe: position track tag";
		case 0x0002: return "editor: keyframe: viewport: bottom";
		case 0x3d3e: return "editor configuration main block";
		}

		return "unknown";
	}


	bool gvA3ds_LoadChunk(gvA3ds_Chunk& temp_chunk, FILE* fp)
	{

		size_t bytes_read;
		bytes_read = fread(&temp_chunk.ID, 1, 2, fp);
		bytes_read += fread(&temp_chunk.Size, 1, 4, fp);
		if (bytes_read != 6)
		{
			GPVULC_NOTIFY(LOG_DEBUG, "Error loading a chunk with id=%d\n", temp_chunk.ID);
			return false;
		}
		return true;
	}

	// simply jumps this chunk
	bool gvA3ds_SkipChunk(gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		if (CheckNotifyLevel(LOG_DEBUG))
		{
			unsigned short high, low;
			high = temp_chunk.ID / 256;
			low = temp_chunk.ID % 256;
			const std::string& descr = gvA3ds_UnknownChunkDescription(temp_chunk.ID);
			GPVULC_NOTIFY(LOG_VERBOSE, "...... skipping chunk (%s). Size=%d bytes, ID=0x%02x%02x\n",
				descr, temp_chunk.Size, high, low);
			GPVULC_LOG(LOG_DEBUG, "log/a3ds_errors.log",
				"Skipped unknown chunk ID 0x%02x%02x, %d bytes (%s)\n", high, low, temp_chunk.Size, descr);
		}

		// 6 bytes have been stripped yet: 2 bytes for the id and 4 for the size
		//temp_chunk.Size -= 6;

		for (unsigned int i = 0; i < temp_chunk.Size - 6; i++)
			if (fgetc(fp) == EOF)
				return true;
		//return (i == temp_chunk.Size-7);

		return true;
	}


	//*****************************************************************************
	// Mesh functions
	//*****************************************************************************

	DsMesh* gvA3ds_GetObjectMesh(DsObject* obj, DsGeom** g = nullptr)
	{
		if (obj == nullptr)
		{
			return nullptr;
		}
		if (obj->Geoms.Size() == 0 || obj->Geoms[0]->Meshes.Size() == 0)
		{
			return nullptr;
		}
		DsGeom* geom = obj->Geoms[0]; if (!geom)
		{
			return nullptr;
		}
		if (g) *g = geom;
		return geom->Meshes[0];
	}


	//*****************************************************************************
	// PARSER
	//*****************************************************************************

	//-----------------------------------------------------------------------------
	// EDITOR OBJECT MESH

	// the vertexes of the mesh
	bool gvA3ds_ParseEditorObjectMeshVertexes(DsGeom& geom, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		unsigned short vertexes_count;
		gvA3ds_Load16Bit(fp, vertexes_count);
		if (vertexes_count == 0)
		{
			return false;
		}

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading vertexes... %d vertexes found.\n", vertexes_count);
		VertexCounter += vertexes_count;

		// allocate space for the vertexes data that's right behind the door
		std::vector<Vec3> vertexes(vertexes_count);

		float x, y, z;
		// we load the vertexes: 3 * 4byte blocks of floating point coords
		if (gvA3ds_UpAxis == DS_Y)
		{
			for (int i = 0; i < vertexes_count; i++)
			{
				gvA3ds_Load32Bit(fp, x);
				gvA3ds_Load32Bit(fp, y);
				gvA3ds_Load32Bit(fp, z);
				vertexes[i].X = x;
				vertexes[i].Y = z;
				vertexes[i].Z = -y;
			}
			//} else if ( gvA3ds_UpAxis==DS_X )...
		}
		else
		{
			for (int i = 0; i < vertexes_count; i++)
			{
				gvA3ds_Load32Bit(fp, vertexes[i].X);
				gvA3ds_Load32Bit(fp, vertexes[i].Y);
				gvA3ds_Load32Bit(fp, vertexes[i].Z);
			}
		}

		// updates the object's vertexes data
		geom.Vertices.Copy(vertexes);

		return true;
	}


	// texture coords array
	bool gvA3ds_ParseEditorObjectMeshTextureCoords(DsGeom& geom, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// loads the number of texture coords
		unsigned short texture_coords_count;
		gvA3ds_Load16Bit(fp, texture_coords_count);

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading texture coordinates...\n");

		// allocates space for data
		geom.TexCoords.Allocate(texture_coords_count);

		// loads data and corrects values using coords scale and offset
		unsigned int i;
		for (i = 0; i < texture_coords_count; i++)
		{
			gvA3ds_Load32Bit(fp, geom.TexCoords[i].U);
			gvA3ds_Load32Bit(fp, geom.TexCoords[i].V);
		}

		return true;
	}


	// local coordinate system (center of the system, and the 3 axis)
	bool gvA3ds_ParseEditorObjectMeshLocalAxes(DsMesh& mesh, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		/*
		All object coordinates in 3ds are stored in world space, this is why you can just rip out the meshes and use/draw them without meddeling further
		Unfortunately, this gets a bit wonky with objects with pivot points (conjecture: PP support is retro fitted into the .3ds format and so doesn't fit perfectly?)

		Objects with pivot points have a position relative to their PP, so they have to undergo this transform:

		  invert the mesh matrix, apply this matrix to the object. This puts the object back at the origin
		  Transform the object by the nodes (nnegative) pivot point coords, this puts the PP at the origin
		  Tranform the node by the node matrix, which does the orientation about the pivot point,
		   (and currently) transforms the object back by a translation to the PP.
	  */

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading object coordinate system\n");


		// if the up axis is Y we must convert the local axes in this way:
		// X_axis_giove = X_axis_3ds
		// Z_axis_giove = -Y_axis_3ds
		// Y_axis_giove = Z_axis_3ds
		// also the coordinates read for each exis must be converted like this:
		// coord_giove.X = coord_4ds.X
		// coord_giove.Y = coord_4ds.Z
		// coord_giove.Z = -coord_4ds.Y

		Vec3 xaxis;
		Vec3 yaxis;
		Vec3 zaxis;
		Vec3 pivot;

		// loads the 3 axis values
		Vec3 pt;

		gvA3ds_Load32Bit(fp, pt.X); gvA3ds_Load32Bit(fp, pt.Y); gvA3ds_Load32Bit(fp, pt.Z);

		if (gvA3ds_UpAxis == DS_Y)
			xaxis = Vec3(pt.X, pt.Z, -pt.Y);
		////else if ( gvA3ds_UpAxis==DS_X ) ...
		else
			xaxis = pt;

		GPVULC_NOTIFY(LOG_VERBOSE, ".............. X axis: %g %g %g\n", pt.X, pt.Y, pt.Z);

		gvA3ds_Load32Bit(fp, pt.X); gvA3ds_Load32Bit(fp, pt.Y); gvA3ds_Load32Bit(fp, pt.Z);
		if (gvA3ds_UpAxis == DS_Y)
			zaxis = -Vec3(pt.X, pt.Z, -pt.Y);
		////else if ( gvA3ds_UpAxis==DS_X ) ...
		else
			yaxis = pt;

		GPVULC_NOTIFY(LOG_VERBOSE, ".............. Y axis: %g %g %g\n", pt.X, pt.Y, pt.Z);

		gvA3ds_Load32Bit(fp, pt.X); gvA3ds_Load32Bit(fp, pt.Y); gvA3ds_Load32Bit(fp, pt.Z);
		if (gvA3ds_UpAxis == DS_Y)
			yaxis = Vec3(pt.X, pt.Z, -pt.Y);
		////else if ( gvA3ds_UpAxis==DS_X ) ...
		else
			zaxis = pt;

		GPVULC_NOTIFY(LOG_VERBOSE, ".............. Z axis: %g %g %g\n", pt.X, pt.Y, pt.Z);

		// loads the center of the coordinate system
		gvA3ds_Load32Bit(fp, pt.X); gvA3ds_Load32Bit(fp, pt.Y); gvA3ds_Load32Bit(fp, pt.Z);
		if (gvA3ds_UpAxis == DS_Y)
			pivot = Vec3(pt.X, pt.Z, -pt.Y);
		////else if ( gvA3ds_UpAxis==DS_X ) ...
		else
			pivot = pt;

		GPVULC_NOTIFY(LOG_VERBOSE, ".............. Center: %g %g %g\n", pt.X, pt.Y, pt.Z);

		Mat4 mat;

		VecCopy(&mat[0], xaxis);
		VecCopy(&mat[4], yaxis);
		VecCopy(&mat[8], zaxis);
		VecCopy(&mat[12], pivot);
		//else if ( gvA3ds_UpAxis==DS_X ) ...

		mesh.AbsoluteMatrix = mat;

		return true;
	}

	// faces of the mesh (indexes of the mesh)
	bool gvA3ds_ParseEditorObjectMeshFaces(DsMesh& mesh, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		unsigned short fcount;
		gvA3ds_Load16Bit(fp, fcount);
		unsigned int faces_count = fcount;

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading faces... %d faces found.\n", faces_count);
		//FaceCounter+=faces_count;

		mesh.SetType(DS_TRIANGLES);
		mesh.VertIdx.Allocate(faces_count * 3);
		mesh.VertIdx.RemoveAll(false);

		unsigned short vertidx;
		// we load the faces: 3 * 2byte blocks of floating point coords,
	  //     then 2byte flags for visibility
		for (unsigned int i = 0; i < faces_count; i++)
		{

			for (unsigned int j = 0; j < 3; j++)
			{
				gvA3ds_Load16Bit(fp, vertidx);
				mesh.AddVertex(vertidx);
				//mesh.VertIdx[i] = vertidx;
			}

			// skips the flag bytes
			fseek(fp, ftell(fp) + 2, SEEK_SET);
		}

		return true;
	}


	// material parse: list of faces using a material, identified by name
	bool gvA3ds_ParseEditorObjectMeshMaterial(DsMesh& mesh, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading mesh material name: ");

		// loads the name of the material
		unsigned int material_name_size = 0;
		char *material_name;
		long start = ftell(fp);
		do material_name_size++; while (fgetc(fp) != 0);
		fseek(fp, start, SEEK_SET);
		if ((material_name = new char[material_name_size]) == nullptr)
		{
			return false;
		}
		unsigned int i;
		for (i = 0; i < material_name_size; i++) material_name[i] = fgetc(fp);

		mesh.MtlName = material_name;
		delete[] material_name;

		GPVULC_NOTIFY(LOG_VERBOSE, "%s\n", mesh.MtlName.Get());

		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading mesh material faces: ");

		// loads the number of faces using this material
		unsigned short face;
		unsigned short faces_count;
		gvA3ds_Load16Bit(fp, faces_count);
		GPVULC_NOTIFY(LOG_VERBOSE, "%d faces\n", faces_count);

		// sets faces material
		for (i = 0; i < faces_count; i++)
		{
			gvA3ds_Load16Bit(fp, face);
			//...
		}


		return true;
	}

	// faces smoothing groups
	bool gvA3ds_ParseEditorObjectMeshSmoothingGroups(DsMesh& mesh, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// get number of faces
		unsigned int faces_count;
		mesh.GetMeshVerts();
		if (mesh.GetMeshVerts() == 0)
		{
			return false;
		}
		faces_count = mesh.VertIdx.Size() / mesh.GetMeshVerts();
		if (faces_count == 0)
		{
			return false;
		}
		//unsigned char *smoothing_groups; if ((smoothing_groups = new unsigned char[faces_count]) == nullptr) return false;
		mesh.SmoothingGroups.Allocate(faces_count);
		GPVULC_NOTIFY(LOG_VERBOSE, "............ loading smoothing groups...\n");

		// reads faces smoothing group and updates the values into object's structures
		unsigned int i;
		unsigned long group_bits = 0;
		unsigned int group;
		for (i = 0; i < faces_count; i++)
		{

			// read the long int containing the flags
			gvA3ds_Load32Bit(fp, group_bits);
			// calculates the logarithm (i-th group = bit 2^i set to 1)
			for (group = 0; (((group_bits & 0x01) == 0) && (group < 32)); group++, group_bits = group_bits >> 1);

			// updates the value
			mesh.SmoothingGroups[i] = group;
		}

		return true;

	}



	//-----------------------------------------------------------------------------
	// EDITOR OBJECT


	// a chunk containing all the informations for a triangle mesh
	bool gvA3ds_ParseEditorObjectMesh(DsObject& object, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;

		GPVULC_NOTIFY(LOG_VERBOSE, "......... found a mesh! this chunk is %d bytes long.\n", size);
		if (!object.AddGeom(object.Name))
		{
			return false;
		}
		DsGeom& geom = *(object.Geoms.GetLast());
		geom.AddMesh();
		DsMesh& mesh = *(geom.Meshes.GetLast());

		//gvCallBusyCallBack("loading faces");

		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			if (gvA3ds_LoaderFlags.NoGeometry)
			{
				if (!gvA3ds_LoaderFlags.NoMaterials)
				{
					switch (temp_chunk.ID)
					{

					case GV3DS_EDITOR_OBJECT_MESH_MATERIAL:
						if (!gvA3ds_ParseEditorObjectMeshMaterial(mesh, temp_chunk, fp))
						{
							return false;
						}
						break;

					default:
						if (!gvA3ds_SkipChunk(temp_chunk, fp))
						{
							return false;
						}
						break;
					}
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}

				continue;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR_OBJECT_MESH_VERTICES:
				if (!gvA3ds_ParseEditorObjectMeshVertexes(geom, temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_OBJECT_MESH_TEXTURE_COORDS:
				if (!gvA3ds_ParseEditorObjectMeshTextureCoords(geom, temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_OBJECT_MESH_LOCAL_AXES:
				if (!gvA3ds_ParseEditorObjectMeshLocalAxes(mesh, temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_OBJECT_MESH_FACES:
				if (!gvA3ds_ParseEditorObjectMeshFaces(mesh, temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_OBJECT_MESH_MATERIAL:
				if (!gvA3ds_LoaderFlags.NoMaterials)
				{
					if (!gvA3ds_ParseEditorObjectMeshMaterial(mesh, temp_chunk, fp))
					{
						return false;
					}
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_OBJECT_MESH_SMOOTHING_GROUPS:
				if (!gvA3ds_LoaderFlags.NoNormals)
				{
					if (!gvA3ds_ParseEditorObjectMeshSmoothingGroups(mesh, temp_chunk, fp))
					{
						return false;
					}
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			}

		}

		return true;
	}

	// object parser: reads an object data (may be a mesh, a light, a camera.. we only consider meshes)
	bool gvA3ds_ParseEditorObject(DsDataSet* data, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;

		GPVULC_NOTIFY(LOG_VERBOSE, "...... found an object! this chunk is %d bytes long.\n", size);
		gvCallBusyCallBack("loading objects");

		// we load the name of the object we've found
		TextBuffer object_name(16, true);
		int ch;
		while ((ch = fgetc(fp)) != 0)
		{
			object_name << (char)ch;
		}

		GPVULC_NOTIFY(LOG_VERBOSE, "....... its name is %s\n", (const std::string&)object_name);
		//gvCallBusyCallBack("loading objects");


		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR_OBJECT_MESH:
				// this is a 3DS object mesh, create a new dataset geometry object (DsObject class)
				// and add it to dataset (method "AddObject" do all of that)
				if (!data->AddObject(object_name))
					return false;

				if (!gvA3ds_ParseEditorObjectMesh(*(data->Objects.GetLast()), temp_chunk, fp))
					return false;
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					GPVULC_NOTIFY(LOG_VERBOSE, "....... not a mesh object, skipped. %s\n", object_name.Get());
					return false;
				}
				break;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	// EDITOR MATERIAL

	  // PARAMETER AMOUNT OF
	bool gvA3ds_ParseParamAmountOf(float& val, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;
		unsigned short amount;


		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_AMOUNT_OF:
				gvA3ds_Load16Bit(fp, amount);
				val = (float)amount;
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;
			}
		}
		return true;
	}

	// PARAMETER COLOR
	bool gvA3ds_ParseEditorMaterialParamColor(ColorRGBA& color, gvA3ds_Chunk temp_chunk, FILE* fp)
	{
		// color chunks: ambient - diffuse - specular colors

		  // this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;
		long end = (long)(start + size);

		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < end)
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_RGB_CHUNK:
				if (!gvA3ds_ParseRgbColor(fp, color))
				{
					return false;
				}
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;
			}
		}
		return true;
	}


	bool gvA3ds_ParseEditorMaterialTexture(int type, Material& material, gvA3ds_Chunk temp_chunk, FILE* fp)
	{
		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;
		long end = start + size;
		float val = 0.0f; // buffer for floating point values

		GPVULC_NOTIFY(LOG_VERBOSE, "...... found texture information\n", size);
		//gvCallBusyCallBack("loading textures...");

		material.Maps.Add(MtlTexMap());
		MtlTexMap& map = material.Maps.GetLast();
		map.Type = type;

		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < end)
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR_MATERIAL_TEXTURE_FILE:
				if (!(gvA3ds_ParseString(fp, map.FileName)))
				{
					return false;
				}
				GPVULC_NOTIFY(LOG_VERBOSE, "....... map file: %s\n", map.FileName.c_str());
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_U_SCALING:
				if (!gvA3ds_ParseFloat(fp, map.UScale))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_V_SCALING:
				if (!gvA3ds_ParseFloat(fp, map.VScale))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_U_OFFSET:
				if (!gvA3ds_ParseFloat(fp, map.UOffset))
				{
					return false;
				}
				break;

				// this one needs inversion!
			case GV3DS_EDITOR_MATERIAL_TEXTURE_V_OFFSET:
				if (!gvA3ds_ParseFloat(fp, map.VOffset))
				{
					return false;
				}
				map.VOffset = -map.VOffset;
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_ROTATION:
				if (!gvA3ds_ParseFloat(fp, map.Rotation))
				{
					return false;
				}
				break;

			case GV3DS_AMOUNT_OF:
				gvA3ds_ParseAmountOf(fp, val);
				map.AmountOf = val*0.01f;
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_FILTERING_BLUR:
				if (!gvA3ds_ParseFloat(fp, val))
				{
					return false;
				}
				if (val)
				{
					map.BlurFactor = 0.01f*val;
					GPVULC_NOTIFY(LOG_VERBOSE, "....... filtering blur = %g\n", map.BlurFactor);
				}
				break;

			default:
				if (!(gvA3ds_SkipChunk(temp_chunk, fp)))
				{
					return false;
				}
				break;

			}

		}

		return true;
	}

	// the material chunk: contains all informations concerning a material (colors and textures)
	bool gvA3ds_ParseEditorMaterial(MtlLib* mtllib, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		mtllib->Materials.Add(Material());
		Material& material = mtllib->Materials.GetLast();
		material.Parameters.DiffuseOW = true;

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;
		long end = start + size;
		float val = 0.0f; // temporary buffer for values read (not reset at each loop)

		GPVULC_NOTIFY(LOG_VERBOSE, "... got into material parser! this chunk is %d bytes long.\n", size);

		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < end)
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR_MATERIAL_NAME:
				if (!(gvA3ds_ParseString(fp, material.Name)))
				{
					return false;
				}
				GPVULC_NOTIFY(LOG_VERBOSE, "...... material name is %s\n", material.Name.c_str());
				break;

			case GV3DS_EDITOR_MATERIAL_AMBIENT:
				if (!(gvA3ds_ParseEditorMaterialParamColor(material.Parameters.Ambient, temp_chunk, fp)))
				{
					return false;
				}
				GPVULC_NOTIFY(LOG_VERBOSE, "...... ambient color: %x\n", material.Parameters.Ambient.GetHex());
				break;

			case GV3DS_EDITOR_MATERIAL_DIFFUSE:
				if (!(gvA3ds_ParseEditorMaterialParamColor(material.Parameters.Diffuse, temp_chunk, fp)))
				{
					return false;
				}
				//material.Parameters.Color = material.Parameters.Diffuse;
				GPVULC_NOTIFY(LOG_VERBOSE, "...... diffuse color: %x\n", material.Parameters.Diffuse.GetHex());
				break;

			case GV3DS_EDITOR_MATERIAL_SPECULAR:
				if (!(gvA3ds_ParseEditorMaterialParamColor(material.Parameters.Specular, temp_chunk, fp)))
				{
					return false;
				}
				GPVULC_NOTIFY(LOG_VERBOSE, "...... specular color: %x\n", material.Parameters.Specular.GetHex());
				break;

			case GV3DS_EDITOR_MATERIAL_EMISSION:
				if (!(gvA3ds_ParseEditorMaterialParamColor(material.Parameters.Emission, temp_chunk, fp)))
				{
					return false;
				}
				GPVULC_NOTIFY(LOG_VERBOSE, "...... emission color: %x\n", material.Parameters.Emission.GetHex());
				break;

			case GV3DS_EDITOR_MATERIAL_SHININESS:
				if (!(gvA3ds_ParseParamAmountOf(material.Parameters.SpecularLevel, temp_chunk, fp)))
				{
					return false;
				}
				material.Parameters.SpecularLevel = 0.01f*val;
				GPVULC_NOTIFY(LOG_VERBOSE, "...... specular level (shininess): %f\n", material.Parameters.SpecularLevel);
				break;

			case GV3DS_EDITOR_MATERIAL_TRANSPARENCY:
				if (!(gvA3ds_ParseParamAmountOf(val, temp_chunk, fp)))
				{
					return false;
				}
				material.Parameters.Opacity = 1.0f - 0.01f*val;
				GPVULC_NOTIFY(LOG_VERBOSE, "...... opacity: %f\n", material.Parameters.Opacity);
				break;

				////      case GV3DS_EDITOR_MATERIAL_REFLECTION_BLUR:
				////        if (!(ParseEditorMaterialReflectionBlur(temp_chunk, Materials[MaterialsCount-1]))) return false;
				////        break;
				////
				////      case GV3DS_EDITOR_MATERIAL_XP_FALL:
				////        if (!(ParseEditorMaterialXpFall(temp_chunk, Materials[MaterialsCount-1]))) return false;
				////        break;
				////
			case GV3DS_EDITOR_MATERIAL_GLOSSINESS:
				if (!(gvA3ds_ParseParamAmountOf(val, temp_chunk, fp)))
				{
					return false;
				}
				if (val > 64.0f) val = 64.0f;
				material.Parameters.Shininess = val * 1.28f;
				GPVULC_NOTIFY(LOG_VERBOSE, "...... shininess (glossiness): %f\n", material.Parameters.Shininess);
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_DIFFUSE:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_DIFFUSE, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_OPACITY:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_OPACITY, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_BUMP:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_BUMP, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_SPECULAR:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_SPECULAR, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_SHININESS:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_SHININESS, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_SELF_ILLUMINATION:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_SELF_ILLUMINATION, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_TEXTURE_ENVIRONMENT:
				if (!(gvA3ds_ParseEditorMaterialTexture(TEX_ENVIRONMENT, material, temp_chunk, fp)))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL_2_SIDED:
				material.Parameters.TwoSided = true;
				GPVULC_NOTIFY(LOG_VERBOSE, "...... found two sided flag\n");
				break;

			default:
				if (!(gvA3ds_SkipChunk(temp_chunk, fp)))
				{
					return false;
				}
				break;

			}

		}

		//material.TexDiffuse.BlurFactor=
		//material.TexOpacity.BlurFactor=
		//material.TexBump.BlurFactor=
		//material.TexSpecularColor.BlurFactor=
		//material.TexSpecularLevel.BlurFactor=
		//material.TexSelfIllumination.BlurFactor=
		//material.TexReflection.BlurFactor=0.01f*material.Blur;

		return true;

	}


	//-----------------------------------------------------------------------------
	// EDITOR

	// editor chunk: contains all geometry, light and cameras (ignored) and materials
	bool gvA3ds_ParseEditor(DsDataSet* data, MtlLib* mtllib, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;

		GPVULC_NOTIFY(LOG_VERBOSE, "... got into 3d editor parser! this chunk is %d bytes long.\n", size);

		// while we're not out of the chunk we keep reading chunks
		while (ftell(fp) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR_OBJECT:
				if (!gvA3ds_LoaderFlags.NoObjects)
				{
					if (!gvA3ds_ParseEditorObject(data, temp_chunk, fp))
					{
						return false;
					}
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_EDITOR_MATERIAL:
				if (!gvA3ds_LoaderFlags.NoMaterials)
				{
					if (!gvA3ds_ParseEditorMaterial(mtllib, temp_chunk, fp))
					{
						return false;
					}
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;
			}
		}

		return true;
	}


	//-----------------------------------------------------------------------------
	// KEYFRAMER

	bool gvA3ds_ParseKeyFramerObjectTranslation(TransformAnimation& anim, gvA3ds_Chunk temp_chunk, FILE* fp)
	{
		//14+18*(num_frames) POS_TRACK_TAG
		//14 for the header:
		//2 short flags;
		//8 short unknown[4];
		//2 short keys;
		//2 short unknown;
		//18 for each keyframe:
		//struct {
		//2 short framenum;
		//4 long unknown;
		//12 float pos_x, pos_y, pos_z;
		//} pos[keys];

		GPVULC_NOTIFY(LOG_VERBOSE, "......... loading object's translation\n");

		unsigned int size = temp_chunk.Size - 6;
		// skips 14 bytes
		fseek(fp, ftell(fp) + 14, SEEK_SET);

		// loads the keyframes
		unsigned int i = 14;
		while (i < size)
		{

			Vec3 tr;
			// loads the time (and skips the next 4 bytes)
			unsigned short time;
			gvA3ds_Load16Bit(fp, time);
			// skip "long unknown"
			fseek(fp, ftell(fp) + 4, SEEK_SET);

			// loads the vector
			gvA3ds_Load32Bit(fp, tr.X); gvA3ds_Load32Bit(fp, tr.Y); gvA3ds_Load32Bit(fp, tr.Z);

			GPVULC_NOTIFY(LOG_VERBOSE, ".......... translation[%d]: (%g %g %g)\n", time, tr.X, tr.Y, tr.Z);

			//if ( UpAxis==DS_Y )
			tr = Vec3(tr.X, tr.Z, -tr.Y);
			//else if ( UpAxis==DS_X ) ...

			// adds a keyframe
			anim.SetPosition(time / GV3DS_FPS, tr);
			if (anim.Position.IsDefined()) anim.Position.SetActive(true);

			i += 18; // jump to the next keyframe
		}

		return true;
	}

	bool gvA3ds_ParseKeyFramerObjectRotation(TransformAnimation& anim, gvA3ds_Chunk temp_chunk, FILE* fp)
	{
		//14+22*(num_frames) POS_TRACK_TAG
		//14 for the header:
		//2 short flags;
		//8 short unknown[4];
		//2 short keys;
		//2 short unknown;
		//18 for each keyframe:
		//struct {
		//2 short framenum;
		//4 long unknown;
		//4 float rotation_rad;
		//12 float axis_x, axis_y, axis_z;
		//} pos[keys];

		GPVULC_NOTIFY(LOG_VERBOSE, "......... loading object's rotation\n");

		unsigned int size = temp_chunk.Size - 6;
		// skips 14 bytes
		fseek(fp, ftell(fp) + 14, SEEK_SET);

		// loads the keyframes
		unsigned int i = 14;
		while (i < size)
		{

			// loads the time (and skips the next 4 bytes)
			unsigned short time;
			gvA3ds_Load16Bit(fp, time);
			// skip "long unknown"
			fseek(fp, ftell(fp) + 4, SEEK_SET);

			Vec3 axis;
			float angle;
			// loads the angle
			gvA3ds_Load32Bit(fp, angle);

			// loads the axis
			gvA3ds_Load32Bit(fp, axis.X); gvA3ds_Load32Bit(fp, axis.Y); gvA3ds_Load32Bit(fp, axis.Z);

			GPVULC_NOTIFY(LOG_VERBOSE, ".......... rotation[%d]: %g (%g %g %g)\n", time, angle, axis.X, axis.Y, axis.Z);

			//if ( UpAxis==DS_Y )
			//else if ( UpAxis==DS_X ) ...


			// convert rotation into Euler angles

			Vec3 rot;
			unsigned rotbits = 0u;
			if (!axis.IsZero())
			{
				angle *= DS_RAD2DEG;
				//if      ((axis-Vec3(1,0,0)).IsZero()) rot.X = angle;
				//else if ((axis-Vec3(0,1,0)).IsZero()) rot.Z = -angle;
				//else if ((axis-Vec3(0,0,1)).IsZero()) rot.Y = angle;
				//else if ((axis-Vec3(-1,0,0)).IsZero()) rot.X = -angle;
				//else if ((axis-Vec3(0,-1,0)).IsZero()) rot.Z = angle;
				//else if ((axis-Vec3(0,0,-1)).IsZero()) rot.Y = -angle;
				//else {
				Mat4 rotmat;
				rotmat.MakeRotation(angle, axis);
				rotmat = rotmat.ConvertZupYup();
				rot = rotmat.GetEuler();
				//}
				if (!IsAlmostZero(axis.Dot(Vec3(1, 0, 0)))) rotbits |= DS_USEX;
				else if (!IsAlmostZero(axis.Dot(Vec3(0, 1, 0)))) rotbits |= DS_USEZ;
				else if (!IsAlmostZero(axis.Dot(Vec3(0, 0, 1)))) rotbits |= DS_USEY;
			}
			else rotbits = DS_USEXYZ;

			// adds a keyframe
			//anim.SetRotation( time, angle, axis );
			anim.SetRotation(time / GV3DS_FPS, rot, rotbits);
			if (anim.Rotation.IsDefined()) anim.Rotation.SetActive(true);

			i += 22; // jump to the next keyframe
		}

		return true;
	}

	bool gvA3ds_ParseKeyFramerObjectScaling(TransformAnimation& anim, gvA3ds_Chunk temp_chunk, FILE* fp)
	{
		//14+18*(num_frames) POS_TRACK_TAG
		//14 for the header:
		//2 short flags;
		//8 short unknown[4];
		//2 short keys;
		//2 short unknown;
		//18 for each keyframe:
		//struct {
		//2 short framenum;
		//4 long unknown;
		//12 float scale_x, scale_y, scale_z;
		//} pos[keys];

		GPVULC_NOTIFY(LOG_VERBOSE, "......... loading object's scaling factors\n");

		unsigned int size = temp_chunk.Size - 6;
		// skips 14 bytes
		fseek(fp, ftell(fp) + 14, SEEK_SET);

		// loads the keyframes
		unsigned int i = 14;
		while (i < size)
		{

			Vec3 sc;
			// loads the time (and skips the next 4 bytes)
			unsigned short time;
			gvA3ds_Load16Bit(fp, time);
			// skip "long unknown"
			fseek(fp, ftell(fp) + 4, SEEK_SET);

			// loads the vector
			gvA3ds_Load32Bit(fp, sc.X); gvA3ds_Load32Bit(fp, sc.Y); gvA3ds_Load32Bit(fp, sc.Z);

			GPVULC_NOTIFY(LOG_VERBOSE, ".......... scaling[%d]: (%g %g %g)\n", time, sc.X, sc.Y, sc.Z);

			sc = Vec3(sc.X, sc.Z, sc.Y);
			//if ( UpAxis==DS_Y )
			  //sc = Vec3(sc.X,sc.Z,-sc.Y);
			//else if ( UpAxis==DS_X ) ...

			// adds a keyframe
			anim.SetScaling(time / GV3DS_FPS, sc);
			if (anim.Scaling.IsDefined()) anim.Scaling.SetActive(true);

			i += 18; // jump to the next keyframe
		}

		return true;
	}

	// editor chunk: contains all geometry, light and cameras (ignored) and materials
	bool gvA3ds_ParseKeyFramerObject(DsDataSet* data, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		unsigned int size = temp_chunk.Size - 6;

		//StartKeyFrame = nullptr;

		GPVULC_NOTIFY(LOG_VERBOSE, "...... found a keyframer object! this chunk is %d bytes long.\n", size);
		//gvCallBusyCallBack("loading node transformation...");

		unsigned short number = 0;
		TextBuffer object_name;
		TextBuffer instance_name;
		short hierarchy;
		Vec3 pivot;
		Vec3 translation;
		TransformAnimation* anim = new TransformAnimation;

		while (ftell(fp) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
				return false;

			switch (temp_chunk.ID)
			{

			case GV3DS_KEYFRAMER_OBJECT_NUMBER:
				gvA3ds_Load16Bit(fp, number);
				GPVULC_NOTIFY(LOG_VERBOSE, "......... loading group/object's number... %d\n", number);
				break;

			case GV3DS_KEYFRAMER_OBJECT_HIERARCHY:
				if (!gvA3ds_ParseString(fp, object_name))
					return false;
				// skips unknown 4 bytes
				fseek(fp, ftell(fp) + 4, SEEK_SET);
				// loads hierarchy
				gvA3ds_Load16Bit(fp, hierarchy);
				GPVULC_NOTIFY(LOG_VERBOSE, "......... hierarchy level for object %s (father's number)=%d\n", (const std::string&)object_name, hierarchy);
				break;

			case GV3DS_KEYFRAMER_OBJECT_INSTANCE_NAME:
				if (!gvA3ds_ParseString(fp, instance_name))
					return false;
				GPVULC_NOTIFY(LOG_VERBOSE, "......... instance name: %s\n", (const std::string&)instance_name);
				break;

			case GV3DS_KEYFRAMER_OBJECT_PIVOT_OFFSET:
				// loads the pivot offset
				gvA3ds_Load32Bit(fp, pivot.X);
				gvA3ds_Load32Bit(fp, pivot.Y);
				gvA3ds_Load32Bit(fp, pivot.Z);
				GPVULC_NOTIFY(LOG_VERBOSE, "......... loading object's pivot offset (%g,%g,%g)\n", pivot.X, pivot.Y, pivot.Z);
				break;

			case GV3DS_KEYFRAMER_OBJECT_TRANSLATION:
				if (!gvA3ds_LoaderFlags.NoTransform)
				{
					if (!gvA3ds_ParseKeyFramerObjectTranslation(*anim, temp_chunk, fp))
						return false;
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;

			case GV3DS_KEYFRAMER_OBJECT_ROTATION:
				if (!gvA3ds_LoaderFlags.NoTransform)
				{
					if (!gvA3ds_ParseKeyFramerObjectRotation(*anim, temp_chunk, fp))
						return false;
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
				{
					return false;
				}
				break;

			case GV3DS_KEYFRAMER_OBJECT_SCALING:
				if (!gvA3ds_LoaderFlags.NoTransform && object_name != "$$$DUMMY")
				{
					if (!gvA3ds_ParseKeyFramerObjectScaling(*anim, temp_chunk, fp))
						return false;
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;
			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;
			}
		}

		DsObject* object = nullptr;
		if (object_name == "$$$DUMMY")
		{
			GPVULC_NOTIFY(LOG_VERBOSE, "......... Object %s is a $$$DUMMY\n", (const std::string&)instance_name);
			if (!data->AddObject(instance_name))
				return false;
			object = data->Objects.GetLast();
			object->IsDummy = true;
		}
		else
		{
			if (!object)
				object = data->FindObjectByName(object_name);
			if (!object)
			{
				GPVULC_NOTIFY(LOG_WARN, ". Referenced object %s not found\n", (const std::string&)object_name);
				if (data->AddObject(object_name))
					object = data->Objects.GetLast();
			}
		}

		if (!object)
		{
			GPVULC_NOTIFY(LOG_WARN, ". referenced object not found\n");
			return false;
		}

		// sets loader variable to remember last number read
		object->ID = number;
		object->ParentID = hierarchy;
		if (anim->IsDefined())
		{
			object->Animation = anim;
			//object->Animation->Position.SetActionAfterEnd(GVANIM_RESTART);
			//object->Animation->Rotation.SetActionAfterEnd(GVANIM_RESTART);
			//object->Animation->Scaling.SetActionAfterEnd(GVANIM_RESTART);
			Transformation tr;
			if (anim->GetTransformAt(0, tr))   object->Transform = tr;
		}
		else
			delete anim;

		//if ( UpAxis==DS_Y )
		pivot = Vec3(pivot.X, pivot.Z, -pivot.Y);
		//else if ( UpAxis==DS_X ) ...

		object->PivotOffset = pivot;


		/* TODO (GPV#1#): Setup pivots */
	//  ReorderGroupKeyFrames(LastElement->Ref);

		return true;
	}

	// keyframer chunk: contains the scene graph, and transformations informations
	bool gvA3ds_ParseKeyFramer(DsDataSet* data, gvA3ds_Chunk temp_chunk, FILE* fp)
	{

		// this is needed to check when we're over with this chunk
		long start = ftell(fp);
		long chunk_start = 0;
		unsigned int size = temp_chunk.Size - 6;

		long start_frame, end_frame; // read, but not used for now

		GPVULC_NOTIFY(LOG_VERBOSE, "... got into keyframer parser! this chunk is %d bytes long.\n", size);

		// while we're not out of the chunk we keep reading chunks
		while ((chunk_start = ftell(fp)) < (long)(start + size))
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
				return false;

			switch (temp_chunk.ID)
			{
				/*
				b00aH   KFHDR
				followed by viewport_layout, kfseg, kfcurtime, object_node_tag, light_node_tag, target_node_tag, camera_node_tag, l_target_node_tag, spotlight_node_tag, ambient_node_tag...
				short revision;
				cstr filename;
				short animlen;
				*/
			case GV3DS_KEYFRAMER_OBJECT:
				if (!gvA3ds_LoaderFlags.NoObjects || !gvA3ds_LoaderFlags.NoGeometry || !gvA3ds_LoaderFlags.NoHierarchy)
				{
					if (!gvA3ds_ParseKeyFramerObject(data, temp_chunk, fp))
						return false;
				}
				else if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;

			case GV3DS_KEYFRAMER_START_END_FRAMES:
				// loads the start and end frames
				gvA3ds_Load32Bit(fp, start_frame); gvA3ds_Load32Bit(fp, end_frame);
				// skips other eventual bytes..
				fseek(fp, chunk_start + temp_chunk.Size, SEEK_SET);
				GPVULC_NOTIFY(LOG_VERBOSE, "...... keyframer start-end: %d - %d.\n", start_frame, end_frame);
				break;

			default:
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;

			}

		}

		return true;
	}


	//-----------------------------------------------------------------------------
	// FILE

	bool gvA3ds_ParseFile(DsDataSet* data, MtlLib* mtllib, FILE* fp)
	{
		// 2 bytes for the id and 4 bytes for the size
		gvA3ds_Chunk temp_chunk;
		if (!gvA3ds_LoadChunk(temp_chunk, fp))
		{

			GPVULC_NOTIFY(LOG_WARN, "Error: can't load main chunk!\n");
			return false;
		}

		// file type checking
		if (temp_chunk.ID != GV3DS_MAIN)
		{

			GPVULC_NOTIFY(LOG_WARN, "Error: can't load main chunk! This is not a valid 3ds file!\n");
			return false;

		}
		else
		{

			GPVULC_NOTIFY(LOG_VERBOSE, "\nSuccessfully loaded main chunk! It's %d bytes long.\n", temp_chunk.Size);
		}

		// to understand when we're over with this chunk
		long size = temp_chunk.Size - 6;

		while (ftell(fp) < size)
		{

			if (!gvA3ds_LoadChunk(temp_chunk, fp))
			{
				return false;
			}

			switch (temp_chunk.ID)
			{

			case GV3DS_EDITOR:
				GPVULC_NOTIFY(LOG_VERBOSE, ". Chunk: EDITOR\n");
				if (!gvA3ds_ParseEditor(data, mtllib, temp_chunk, fp))
					return false;
				break;

			case GV3DS_KEYFRAMER:
				GPVULC_NOTIFY(LOG_VERBOSE, ". Chunk: KEYFRAMER\n");
				if (!gvA3ds_ParseKeyFramer(data, temp_chunk, fp))
					return false;
				break;

			default:
				GPVULC_NOTIFY(LOG_VERBOSE, ". Chunk: unknown\n");
				if (!gvA3ds_SkipChunk(temp_chunk, fp))
					return false;
				break;
			}
		}


		return true;
	}


	//-----------------------------------------------------------------------------
	//  SETUP DATA

	// GLOBAL FUNCTION THAT CALCULATES THE FACE NORMAL VECTORS GIVEN THE SMOOTHING GROUPS
	// - first every face's vertex' normal is calculated
	// - then for each smoothing group normals are interpolated, and if there are faces without
	//   any smoothing group their normals are kept as they are
	bool gvA3ds_CalculateNormals(
		const std::vector<Vec3>& old_vertexes,
		const std::vector<TexCoord> old_texture_coords,
		std::vector<TriangleIndices>& faces,
		std::vector<unsigned int> smoothing_groups,
		std::vector<Vec3>& new_vertexes,
		std::vector<Vec3>& normals,
		std::vector<TexCoord> new_texture_coords)
	{

		// error check
		if (old_vertexes.empty())
		{
			return false;
		}
		if (faces.empty())
		{
			return false;
		}
		unsigned int old_vertexes_count = (unsigned int)old_vertexes.size();
		unsigned int faces_count = (unsigned int)faces.size();
		unsigned int i, j, k;
		bool only_one_sg = true;

		// with smoothing groups: normals one the same vertex are interpolated together
		if (!smoothing_groups.empty())
		{

			only_one_sg = true;
			// check if there is only one smoothing group
			for (i = 1; i < faces_count; i++)
				if (smoothing_groups[i] != smoothing_groups[0]) { only_one_sg = false; break; }
			//    only_one_sg=true;

			if (only_one_sg)
			{
				GPVULC_NOTIFY(LOG_VERBOSE, "...... single smoothing group: interpolating normals...\n");
				// creates an array to temporary hold faces normals
				normals.resize(old_vertexes_count);

				CalcVertNormals(old_vertexes, faces, normals);

				new_vertexes = old_vertexes;
				new_texture_coords = old_texture_coords;

				GPVULC_NOTIFY(LOG_VERBOSE, "...... single smoothing group: normals interpolated.\n");

			}
			else
			{  // more than one smoothing group

	 // To calculate normals for each smoothing group we need to split the geometry
	 // into separate parts, one for each smoothing group. Once the geometry data
	 // (vertices, faces, texture ccordinates) are splitted and grouped we can
	 // calculate normals for the entire geometry and we obtain the expected result.

				unsigned int sg_faces_count[256] = { 0 };
				unsigned int sg_max = 0;
				bool used_sg[256];
				unsigned int sg_verts_count[256] = { 0 };
				unsigned int idx = 0;

				memset(used_sg, 0, 256);

				// find the maximum value for a smoothing group
				for (i = 0; i < faces_count; i++)
				{
					if (sg_max < smoothing_groups[i])
					{
						sg_max = smoothing_groups[i];
					}
					used_sg[smoothing_groups[i]] = true;
				}

				// remap smoothing groups in a continuous sequence from 0 to sg_max
				unsigned char sg_map[256];
				unsigned int sg_map_counter = 0;
				for (i = 0; i <= sg_max; i++)
				{
					if (used_sg[i])
					{
						sg_map[i] = sg_map_counter;
						sg_map_counter++;
					}
				}

				sg_max = sg_map[sg_max];
				unsigned int num_sg = sg_max + 1;
				for (i = 0; i < faces_count; i++)
				{
					smoothing_groups[i] = sg_map[smoothing_groups[i]];
				}

				// allocate the usage flags [verts x smooth_g] matrix
				std::vector<bool> sg_used_verts(old_vertexes_count*num_sg);

				// count the faces for each smoothing group
				for (i = 0; i < faces_count; i++)
				{
					sg_faces_count[smoothing_groups[i]]++;
					// set smoothing groups for each vertex
					for (k = 0; k < 3; k++)
					{
						idx = faces[i].VertsIndices[k];
						// check smoothing groups for each vertex ([idx][smoothing_groups[i]])
						sg_used_verts[idx*num_sg + smoothing_groups[i]] = true;
					}
				}

				// calculate the new number of vertices
				// (vertices used by more smoothing groups will be duplicated)
				size_t new_vertexes_count = 0;
				for (i = 0; i < num_sg; i++)
				{
					for (j = 0; j < old_vertexes_count; j++)
					{
						if (sg_used_verts[j*num_sg + i])
						{
							new_vertexes_count++;
							// count vertices for each smoothing group
							sg_verts_count[i]++;
						}
					}
				}

				// creates the new vertex array
				new_vertexes.resize(new_vertexes_count);

				// create a matrix to map old_vertexes to new_vertexes

				// vertices remapping matrix
				std::vector<unsigned int> verts_map(old_vertexes_count*num_sg);

				// handle texture coordinates
				if (!old_texture_coords.empty())
				{
					new_texture_coords.resize(new_vertexes_count);
				}

				// copy vertices in the new list ordered by smoothing group
				unsigned int new_vert_idx = 0;
				for (i = 0; i < num_sg; i++)
				{
					for (j = 0; j < old_vertexes_count; j++)
					{
						if (sg_used_verts[j*num_sg + i])
						{
							new_vertexes[new_vert_idx] = old_vertexes[j];
							// copy texture coordinates if needed
							if (!new_texture_coords.empty())
							{
								new_texture_coords[new_vert_idx] = old_texture_coords[j];
							}
							// map vertex indices to the new array
							verts_map[j*num_sg + i] = new_vert_idx;
							new_vert_idx++; // next index in the new list
						}
					}
				}

				// create a copy of faces list
				std::vector<TriangleIndices> temp_faces(faces); // temporary list of faces

				// map faces indices (by smoothing groups)
				std::vector<unsigned int> sg_faces_idx(faces_count);
				sg_faces_idx[0] = 0;
				unsigned int newfaceidx;
				newfaceidx = 0;
				for (i = 0; i < num_sg; i++)
				{
					for (j = 0; j < faces_count; j++)
					{
						if (smoothing_groups[j] == i) sg_faces_idx[j] = newfaceidx++;
					}
				}

				// remap faces on new_vertexes
				// sorting faces by smoothing group
				unsigned int newidx;
				for (i = 0; i < faces_count; i++)
				{
					newfaceidx = sg_faces_idx[i];
					for (k = 0; k < 3; k++)
					{
						idx = temp_faces[i].VertsIndices[k];
						newidx = verts_map[idx*num_sg + smoothing_groups[i]];
						faces[newfaceidx].VertsIndices[k] = newidx;
					}
				}

				// build and smooth normals by smoothing group

				// creates the vertex and normals array
				normals.resize(new_vertexes_count);

				// calculate vertex normals
		  //      CalcVertNormals(new_vertexes, new_vertexes_count,
		  //                       faces, faces_count,
		  //                       normals);

				unsigned int sg_vertsbeg = 0, sg_facesbeg = 0;
				for (i = 0; i < num_sg; i++)
				{
					GPVULC_NOTIFY(LOG_VERBOSE, "....... smoothing group %d: interpolating normals (faces %d-%d)...\n", i, sg_facesbeg, sg_facesbeg + sg_faces_count[i] - 1);
					gvCallBusyCallBack("calculating normals - processing smoothing groups");
					CalcVertNormals(new_vertexes,
						faces,
						normals,
						sg_vertsbeg, sg_verts_count[i],
						sg_facesbeg, sg_faces_count[i]);
					sg_vertsbeg += sg_verts_count[i];
					sg_facesbeg += sg_faces_count[i];
				}

				GPVULC_NOTIFY(LOG_VERBOSE, "...... normals built.\n");

			}
			// without smoothing groups: for each face we use 3 distinct vertices and normals (no interpolation)
		}
		else
		{  // smoothing_groups==nullptr
			GPVULC_NOTIFY(LOG_VERBOSE, "...... calculating normals (no smoothing group)...\n");

			normals.clear();

			std::vector<Vec3> noNormals;
			// split faces: redirect faces vertices
			SplitVerts(old_vertexes,
				new_vertexes,
				faces,
				noNormals, normals,
				old_texture_coords, new_texture_coords);
			// creates the normals array
	  //      normals = new Vec3[new_vertexes_count];
	  //      if (normals==nullptr) {
	  //        GPVULC_NOTIFY( LOG_FATAL,"gvA3ds_CalculateNormals - Memory allocation failed (normals)\n");
	  //        return false;
	  //      }

			CalcFaceNormals(new_vertexes, faces, normals);
			//printf("old_vertexes_count=%d faces_count=%d new_vertexes_count=%d  \n",old_vertexes_count,faces_count,new_vertexes_count);
		}

		return true;

	}



	// Process the smoothing groups of a mesh calculating the normals
	bool gvA3ds_ProcessSmoothingGroups(DsGeom* geom, DsMesh* mesh)
	{

		// get number of faces
		unsigned int faces_count = mesh->GetNumFaces();
		if (faces_count == 0)
		{
			GPVULC_NOTIFY(LOG_WARN, "No face for this smoothing group\n");
			return false;
		}
		if (geom->TexCoords.Size() && geom->Vertices.Size() != geom->TexCoords.Size())
		{
			GPVULC_NOTIFY(LOG_WARN, "Vertices and texture coordinates do not match in this smoothing group\n");
			return false;
		}

		// calculates object normals

		std::vector<TriangleIndices> faces(mesh->VertIdx.Size());

		TriangleIndices* tris = (TriangleIndices*)(&(mesh->VertIdx.GetData()[0]));
		memcpy(&faces[0], tris, faces.size()*sizeof(TriangleIndices));
		//TriangleIndices* faces = new TriangleIndices[faces_count];
		//int i;
		//for( i=0; i<faces_count; i++ ) {
		//  faces[i] = TriangleIndices(mesh->VertIdx[i*3+0],mesh->VertIdx[i*3+1],mesh->VertIdx[i*3+2]);
		//}

		GPVULC_NOTIFY(LOG_VERBOSE, ".... calculating normals...\n");
		gvCallBusyCallBack("calculating normals");

		if (!(gvA3ds_CalculateNormals(geom->Vertices.GetData(), geom->TexCoords.GetData(), faces,
			mesh->SmoothingGroups.GetData(),
			//SmoothingFlag?smoothing_groups:nullptr,
			geom->Vertices.GetData(), geom->Normals.GetData(), geom->TexCoords.GetData())))
		{

			return false;
		}

		return true;
	}


	bool gvA3ds_ProcessMesh(Vec3 piv, DsGeom* geom, DsMesh* mesh, bool do_not_flip)
	{

		// Apply the geometry pre-transformation
		//--------------------------------------
		Mat4 mesh_mat = mesh->AbsoluteMatrix;//GetPivotMatrix();
		Mat4 mesh_invmat = mesh_mat.Inverted();

		int i;
		bool flip = false;
		if (!do_not_flip && mesh_mat.Determinant() < 0.0f)
			flip = true;

		//DynArray< int > vert_idx;
		//vert_idx.Allocate(mesh->VertIdx.Size());
		//for( i=0; i<mesh->VertIdx.Size(); i++ ) {
		//  vert_idx.AddUnique( mesh->VertIdx[i] );
		//}

		//for( i=0; i<vert_idx.Size(); i++ ) {
		//  Vec3& v = geom->Vertices[vert_idx[i]];
		for (i = 0; i < geom->Vertices.Size(); i++)
		{
			Vec3& v = geom->Vertices[i];
			v = mesh_invmat * v;
			v -= piv;
			if (flip) v.X = -v.X;
			if (gvA3ds_LoaderFlags.NoTransform) v = mesh_mat * v;
			else v = mesh->LocalMatrix * v;
		}
		if (!gvA3ds_LoaderFlags.NoTransform)
			mesh->LocalMatrix.Reset();

		// Flip X coordinate of vertices if mesh matrix has negative determinant
		if (flip)
		{

			for (i = 0; i < mesh->VertIdx.Size(); i += 3)
			{
				std::swap(mesh->VertIdx[i + 1], mesh->VertIdx[i + 2]);
			}
		}

		return true;
	}

	// Setup the data set and its materials
	bool gvA3ds_ProcessMatrices(DsObject* obj, Mat4 upper_matrix)
	{

		if (!obj)
			return false;

		int i;
		GPVULC_NOTIFY(LOG_VERBOSE, ". processing matrices for object %s...\n", obj->Name.Get());

		//bool is_leaf = (obj->Children.Size()==0);

		//Vec3 flip_scaling(1.0f);
		//Vec3 scaling = obj->Transform.GetScaling();
		//if ( scaling.X<0.0f ) { scaling.X = -scaling.X; flip_scaling.X = -1.0f; }
		//if ( scaling.Y<0.0f ) { scaling.Y = -scaling.Y; flip_scaling.Y = -1.0f; }
		//if ( scaling.Z<0.0f ) { scaling.Z = -scaling.Z; flip_scaling.Z = -1.0f; }
		//obj->Transform.SetScaling(scaling);

		DsMesh* mesh = gvA3ds_GetObjectMesh(obj);

		if (mesh)
		{
			// it seems that the pivot point must be scaled according to the mesh matrix, but this should be verified...
			Mat4 mesh_mat = mesh->AbsoluteMatrix;
			Vec3 xaxis(mesh_mat.GetCol(0));
			Vec3 yaxis(mesh_mat.GetCol(1));
			Vec3 zaxis(mesh_mat.GetCol(2));
			Vec3 pivot_scaling = Vec3(xaxis.Length(), yaxis.Length(), zaxis.Length());
			Vec3 fixed_pivot = obj->Transform.GetPivot()*pivot_scaling;

			Mat4 mat;
			VecCopy(&mat[0], xaxis.Normalized());
			VecCopy(&mat[4], yaxis.Normalized());
			VecCopy(&mat[8], zaxis.Normalized());
			VecCopy(&mat[12], fixed_pivot);
			//VecCopy( &mat[12], Vec3());
			// set pivot rotation (only) matrix
			obj->Transform.SetPivotMatrix(mat);

			//obj->Transform.SetPivot(fixed_pivot);
		}

		Mat4 loc_matrix = obj->Transform.GetMatrix();
		Mat4 abs_matrix;

		if (mesh)
		{
			Mat4 inv_matrix;
			//if( is_leaf ) {
			//  Mat4 save_pivot_mat = obj->Transform.GetPivotMatrix();
			//  Vec3 save_pivot = obj->Transform.GetPivot();
			//  inv_matrix = upper_matrix.Inverted();
			//  mesh->LocalMatrix = inv_matrix * mesh->AbsoluteMatrix;
			//  obj->Transform.Reset();
			//  //obj->Transform.SetPivotMatrix(save_pivot_mat);
			//  //obj->Transform.SetPivot(save_pivot);
			//} else {
			abs_matrix = upper_matrix * loc_matrix;
			inv_matrix = abs_matrix.Inverted();
			mesh->LocalMatrix = inv_matrix * mesh->AbsoluteMatrix;
			//}
		//****************************************************************************
		//       when I will understand when the following condition must be true
		//       I will solve the last problem...
		//***************************************************************************
		//      bool postmult = false;
		//      if( postmult )
			  //mesh->LocalMatrix *= obj->Transform.GetPivotInverseMatrix();
		}
		else
		{
			abs_matrix = upper_matrix * loc_matrix;
		}

		for (i = 0; i < obj->Children.Size(); i++)
		{
			gvA3ds_ProcessMatrices(obj->Children[i], abs_matrix);
		}

		return true;
	}

	// Setup the data set and its materials
	bool gvA3ds_SetupDataSet(DsDataSet* data, MtlLib* mtllib)
	{

		if (!data)
		{
			return true;
		} // nothing to do

		int o;//,g,m;
		GPVULC_NOTIFY(LOG_VERBOSE, ". building objects hierarchy...");
		if (!gvA3ds_LoaderFlags.NoHierarchy) data->BuildHierarchy();
		else
		{
			for (int i = 0; i < data->Objects.Size(); i++)
				data->RootObjects.AddUnique(data->Objects[i]);
		}
		GPVULC_NOTIFY(LOG_VERBOSE, " done.\n");

		if (!gvA3ds_LoaderFlags.NoTransform) for (o = 0; o < data->RootObjects.Size(); o++)
		{
			gvA3ds_ProcessMatrices(data->RootObjects[o], Mat4());
		}

		bool calculate_normals = !gvA3ds_LoaderFlags.NoNormals;

		for (o = 0; o < data->Objects.Size(); o++)
		{
			DsObject& obj = *(data->Objects[o]);
			GPVULC_NOTIFY(LOG_VERBOSE, ". processing object %s...\n", obj.Name.Get());
			gvCallBusyCallBack("processing objects");
			bool compute_normals = calculate_normals;

			if (gvA3ds_LoaderFlags.NoTransform && obj.IsDummy) obj.Transform.Reset();
			DsGeom* geom = nullptr;
			DsMesh* mesh = gvA3ds_GetObjectMesh(&obj, &geom);
			if (!mesh) continue;

			// check the faces indices
			bool invalid_mesh = false;
			std::vector<int> invalid_mesh_face;
			std::vector<int> invalid_mesh_face_vert;
			for (int i = 0; i < mesh->GetNumFaces(); i++)
			{
				for (int k = 0; k < 3; k++)
				{
					int idx = mesh->VertIdx[i * 3 + k];
					if (idx >= geom->Vertices.Size())
					{
						invalid_mesh_face.push_back(i);
						invalid_mesh_face_vert.push_back(k);
						invalid_mesh = true;
					}
				}
			}
			if (invalid_mesh)
			{

				GPVULC_NOTIFY(LOG_WARN, "Invalid mesh %s (material=%s), discarded\n", geom->Name.Get(), mesh->MtlName.IsEmpty() ? "<none>" : mesh->MtlName.Get());
				//TextBuffer err_list(2048,false);
				//for (size_t i = 0; i < invalid_mesh_face.size(); i++) {
				//  err_list << "  - face n." << invalid_mesh_face[i]
				//    << " vert n." << invalid_mesh_face_vert[i] << "\n";
				//}
				GPVULC_LOG(LOG_DEBUG, "log/a3ds_errors.log",
					"Invalid mesh %s (material=%s): %d invalid vertices\n", geom->Name.Get(), mesh->MtlName.IsEmpty() ? "<none>" : mesh->MtlName.Get(), invalid_mesh_face_vert.size());
				geom->Meshes.RemoveElem(mesh);
				continue;
			}


			GPVULC_NOTIFY(LOG_VERBOSE, ".. processing mesh transform for geometry %s (material=%s)\n", geom->Name.Get(), mesh->MtlName.IsEmpty() ? "<none>" : mesh->MtlName.Get());

			// do not flip objects if the parent is a dummy object
			bool dont_flip = obj.Parent && obj.Parent->IsDummy;

			if (compute_normals && gvA3ds_LoaderFlags.NoNormalsIfRTT && mtllib)
			{
				Material& mtl = mtllib->GetMaterial(mesh->MtlName);
				// check if spherical mapping is present (normals needed)
				bool env = false;
				for (int k = 0; k < mtl.Maps.Size(); k++)
				{
					if (mtl.Maps[k].Type == TEX_ENVIRONMENT || mtl.Maps[k].Type == TEX_CUBE_MAP)
					{
						env = true;
						break;
					}
				}
				if (!env) for (int k = 0; k < mtl.Maps.Size(); k++)
				{
					if (mtl.Maps[k].Type == TEX_DIFFUSE)
					{
						compute_normals = false;
						break;
					}
				}
			}

			//mesh->LocalMatrix = obj.Transform.GetMatrix().Inverted();
			  //mesh->LocalMatrix = obj.Transform.GetMatrix().Inverted() * mesh->AbsoluteMatrix;
			if (!mesh->AbsoluteMatrix.IsIdentity() || obj.Transform.IsActive())
				gvA3ds_ProcessMesh(obj.PivotOffset, geom, mesh, dont_flip);
			GPVULC_NOTIFY(LOG_VERBOSE, ".. processing smoothing groups for geometry %s\n", geom->Name.Get());
			if (compute_normals && !gvA3ds_ProcessSmoothingGroups(geom, mesh))
			{
				GPVULC_NOTIFY(LOG_WARN, "Error processing smoothing groups for geometry %s\n", geom->Name.Get());
				//return false;
			}
			// mesh->LocalMatrix = obj.Transform.GetMatrix();


			//for( g=0; g<obj.Geoms.Size(); g++ ) {
			//  DsGeom& geom = *(obj.Geoms[g]);
			//  GPVULC_NOTIFY( LOG_VERBOSE,".. processing geometry %s...\n", geom.Name.Get() );
			//  for( m=0; m<geom.Meshes.Size(); m++ ) {
			//    DsMesh& mesh = *(geom.Meshes[m]);
			//    GPVULC_NOTIFY( LOG_VERBOSE,"... processing mesh %d (material=%s)\n", m, mesh.MtlName.IsEmpty() ? "<none>" : mesh.MtlName.Get() );
			//    if( !mesh.AbsoluteMatrix.IsIdentity() || obj.Transform.IsActive() )
			//      gvA3ds_ProcessMesh( obj.PivotOffset, &geom, &mesh, dont_flip );
			//    GPVULC_NOTIFY( LOG_VERBOSE,".... processing smoothing groups for geometry %s, mesh %d\n", geom.Name.Get(), m );
			//    if( !gvA3ds_ProcessSmoothingGroups( &geom, &mesh ) ){
			//      GPVULC_NOTIFY( LOG_WARN,"Error processing smoothing groups for geometry %s, mesh %d\n", geom.Name.Get(), m );
			//      return false;
			//    }
			//  }
			//}
			// **************************

			if (gvA3ds_LoaderFlags.NoTransform) obj.Transform.Reset();
		}

		if (!gvA3ds_LoaderFlags.NoTransform)
		{
			gvCallBusyCallBack("baking objects transforms");
			for (o = 0; o < data->Objects.Size(); o++)
			{
				DsObject& obj = *(data->Objects[o]);

				// delete useless animations
				if (obj.Animation)
				{
					bool freeze = gvA3ds_LoaderFlags.NoAnimation;
					// freeze if only one keyframe
					if (!freeze) freeze = (obj.Animation->Position.X.NumKeyFrames() <= 1
						&& obj.Animation->Rotation.X.NumKeyFrames() <= 1
						&& obj.Animation->Scaling.X.NumKeyFrames() <= 1
						&& obj.Animation->GetDuration() == 0.0);

					// freeze if all the keyframes are equal
					if (!freeze)
					{
						unsigned i = 0;

						// collect all the keyframe times for all the tracks
						DynArray<double> t;
						for (i = 0; i < obj.Animation->Position.X.NumKeyFrames(); i++)
						{
							t.AddUnique(obj.Animation->Position.X.GetKeyFrameByIndex(i)->Time);
						}
						for (i = 0; i < obj.Animation->Rotation.X.NumKeyFrames(); i++)
						{
							t.AddUnique(obj.Animation->Rotation.X.GetKeyFrameByIndex(i)->Time);
						}
						for (i = 0; i < obj.Animation->Scaling.X.NumKeyFrames(); i++)
						{
							t.AddUnique(obj.Animation->Scaling.X.GetKeyFrameByIndex(i)->Time);
						}

						freeze = true;
						if (t.Size() > 1)
						{
							Transformation tr;
							Transformation prev_tr;
							obj.Animation->GetTransformAt(t[0], prev_tr);
							// test if all transformations are equal
							for (int j = 1; j < t.Size(); j++)
							{
								obj.Animation->GetTransformAt(t[j], tr);
								if (tr != prev_tr)
								{
									freeze = false;
									break;
								}
								prev_tr = tr;
							}
						}
					}
					if (freeze)
					{
						Transformation tr;
						obj.Animation->GetTransformAt(obj.Animation->GetStartTime(), tr);
						obj.Transform = tr;
						delete obj.Animation;
						obj.Animation = nullptr;
					}
				} // (end: delete useless animations)

		  // TODO: "","","correggere anche le animazioni")
				if (gvA3ds_LoaderFlags.BakePivotTransform)
				{
					Mat4 piv_mat = obj.Transform.GetPivotMatrix();
					piv_mat.SetTranslation(Vec3());
					if (!piv_mat.IsIdentity() && !obj.Animation)
					{
						GPVULC_NOTIFY(LOG_VERBOSE, ". baking transform for object %s...\n", obj.Name.Get());
						Mat4 mat = obj.Transform.GetMatrix();
						Mat4 piv;
						piv.SetTranslation(obj.Transform.GetPivot());
						mat = piv.Inverted()*mat*piv;
						obj.Transform.SetPivotMatrix(piv);
						obj.Transform.SetMatrix(mat);
					}
				}

			}
		}

		return true;
	}

	// Setup the data set materials
	bool gvA3ds_SetupMtlLib(MtlLib* mtllib)
	{

		if (!mtllib)
		{
			return true;
		} // nothing to do


		return true;
	}

	// Setup the data set and its materials
	bool gvA3ds_SetupData(DsDataSet* data, MtlLib* mtllib)
	{

		if (!gvA3ds_SetupDataSet(data, mtllib))
		{
			return false;
		}
		if (!gvA3ds_SetupMtlLib(mtllib))
		{
			return false;
		}
		if (data && mtllib) mtllib->Name = data->Name;

		return true;
	}

	//-----------------------------------------------------------------------------
	//  Main loader function
	//******************************************************************************
	bool Load3dsFile(const std::string& filename, DsDataSet* data, MtlLib* mtllib, DsLoaderFlags flags)
	{

		if (data == nullptr && mtllib == nullptr)
		{
			return false;
		}

		VertexCounter = 0;
		PathInfo a3dsfile(filename);

		FILE* fp = fopen(a3dsfile.GetFullPath().c_str(), "rb");
		if (!fp)
		{
			GPVULC_NOTIFY(LOG_WARN, "Could not open file %s!\n", a3dsfile.GetFullPath());
			return false;
		}
		GPVULC_NOTIFY(LOG_INFO, "-----------------------------\n");
		GPVULC_NOTIFY(LOG_INFO, "Model loader for Autodesk 3ds\n");
		GPVULC_NOTIFY(LOG_INFO, "-----------------------------\n");
		GPVULC_NOTIFY(LOG_INFO, "Loading 3ds file \"%s\"...\n", a3dsfile.GetFullPath());

		Chrono chrono;
		chrono.Start();

		gvA3ds_LoaderFlags = flags;

		if (!gvA3ds_ParseFile(data, mtllib, fp))
		{
			GPVULC_NOTIFY(LOG_WARN, "Could not parse the file %s!\n", a3dsfile.GetFullPath());
			fclose(fp);
			return false;
		}
		fclose(fp);

		if (data->Name.IsEmpty())
		{
			data->Name = a3dsfile.GetFullPath();
		}

		if (!gvA3ds_SetupData(data, mtllib))
		{
			GPVULC_NOTIFY(LOG_WARN, "Failed to setup data from file %s!\n", a3dsfile.GetFullPath());
			return false;
		}

		GPVULC_NOTIFY(LOG_INFO, "\nModel loader for Autodesk 3ds: file successfully loaded in %g seconds.\n", chrono.GetElapsedTime());

		return true;
	}

	//A3ds_Loader* _A3dsLoader = nullptr;

	void A3dsLoaderInit()
	{
		//if( !_A3dsLoader )
		//  _A3dsLoader = new A3ds_Loader;
		static A3ds_Loader A3dsLoader;
		//_A3dsLoader = &A3dsLoader;
	}

	//void gvA3ds_LoaderDeinit() {
	//  if( _A3dsLoader )
	//    delete _A3dsLoader;
	//  _A3dsLoader = nullptr;
	//}

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
