//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Basic geometry data


#include <gpvulc/ds/GeoData.h>
#include <gpvulc/text/TextParser.h>
#include <gpvulc/ds/util/ConsoleMessage.h>

namespace gpvulc
{


	bool CalcFaceNormals(std::vector<Vec3>& verts, std::vector<TriangleIndices>& faces, std::vector<Vec3>& faces_normals)
	{

		// error checking
		if (verts.empty() || faces.empty())
		{
			GPVULC_NOTIFY(LOG_WARN, "CalcFaceNormals - %s %s\n", verts.empty() ? "no vertices" : "", faces.empty() ? "no faces" : "");
			return false;
		}
		size_t num_verts = verts.size();
		size_t num_faces = faces.size();
		if (num_verts != 3 * num_faces)
		{
			GPVULC_NOTIFY(LOG_WARN, "CalcFaceNormals - Cannot apply to indexed geometry\n");
			return false;
		}

			faces_normals.resize(num_verts);

		//  GPVULC_NOTIFY( LOG_DEBUG,"CalcFaceNormals - calculating normals...\n");

		unsigned int i;
		Vec3 n;

		// fills in the array
		for (i = 0; i < num_faces; i++)
		{

			// normal: p01 x p02
			VecNormal(
				verts[faces[i].VertsIndices[2]],
				verts[faces[i].VertsIndices[0]],
				verts[faces[i].VertsIndices[1]],
				n);
			faces_normals[faces[i].VertsIndices[0]] = n;
			faces_normals[faces[i].VertsIndices[1]] = n;
			faces_normals[faces[i].VertsIndices[2]] = n;
		}
		//  GPVULC_NOTIFY( LOG_DEBUG,"CalcFaceNormals - done.\n");

		return true;
	}


	bool CalcVertNormals(const std::vector<Vec3>& verts, std::vector<TriangleIndices>& faces, std::vector<Vec3>& vert_normals, unsigned int vertsbeg, unsigned int vertscount, unsigned int facesbeg, unsigned int facescount)
	{
		size_t num_verts = verts.size();
		size_t num_faces = faces.size();

		// error checking
		if (verts.empty() || faces.empty())
		{
			GPVULC_NOTIFY(LOG_WARN, "CalcFaceNormals - %s %s\n", verts.empty() ? "no vertices" : "", faces.empty() ? "no faces" : "");
			return false;
		}

		//  if(vertsend==0) vertsend=num_verts-1;
		//  if(facesend==0) facesend=num_faces-1;
		//  unsigned int vertsrange=vertsend-vertsbeg+1;
		//  unsigned int facesrange=facesend-facesbeg+1;
		if (vertscount == 0) vertscount = (unsigned int)num_verts;
		if (facescount == 0) facescount = (unsigned int)num_faces;
		unsigned int vertsend = vertsbeg + vertscount - 1;
		unsigned int facesend = facesbeg + facescount - 1;

		vert_normals.resize(num_verts);

		// allocate cross product list
		std::vector<Vec3> cross_prod(facescount);

		//  GPVULC_NOTIFY( LOG_DEBUG,"CalcVertNormals - calculating normals v[%d-%d],f[%d-%d]...\n",
		//                      vertsbeg,vertsend,facesbeg,facesend);

		unsigned int i, k;

		// fills in the array
		for (i = facesbeg; i <= facesend; i++)
		{
			// normal: p01 x p02
			VecCross(
				verts[faces[i].VertsIndices[2]],
				verts[faces[i].VertsIndices[0]],
				verts[faces[i].VertsIndices[1]],
				cross_prod[i - facesbeg]);
		}

		//////////////// Now Get The Vertex Normals /////////////////

		unsigned int idx = 0;
		// create a list of cross product sums and shared counters
		std::vector<Vec3> crossprodsum(vertscount);
		std::vector<unsigned int> shared(vertscount);

		// sum cross products and count shared vertices for each face
		for (i = facesbeg; i <= facesend; i++)
		{     // Go through all of the faces
			for (k = 0; k < 3; k++)
			{
				idx = faces[i].VertsIndices[k] - vertsbeg;
				crossprodsum[idx] += cross_prod[i - facesbeg];
				shared[idx]++;
			}
		}

		for (i = vertsbeg; i <= vertsend; i++)
		{ // Go through all of the vertices

// Get the normal by dividing the sum by the shared.
			vert_normals[i] = crossprodsum[i - vertsbeg] / (float)shared[i - vertsbeg];

			// Normalize the normal for the final vertex normal
			vert_normals[i].Normalize();
		}

		//  GPVULC_NOTIFY( LOG_DEBUG,"CalcVertNormals - done.\n");

		return true;
	}


	bool SplitVerts(
		const std::vector<Vec3>& verts,
		std::vector<Vec3>& new_verts,
		std::vector<TriangleIndices>& faces,
		std::vector<Vec3>& normals,
		std::vector<Vec3>& new_normals,
		const std::vector<TexCoord>& tex_coords,
		std::vector<TexCoord>& new_tex_coords
		)
	{

		// error checking
		if (verts.empty() || faces.empty())
		{
			GPVULC_NOTIFY(LOG_WARN, "CalcFaceNormals - %s %s\n", verts.empty() ? "no vertices" : "", faces.empty() ? "no faces" : "");
			return false;
		}

		size_t num_verts = verts.size();
		size_t num_faces = faces.size();
		unsigned int new_num_verts = (unsigned int)num_faces * 3;

		new_verts.resize(new_num_verts);
		if (!normals.empty())
		{
			new_normals.resize(new_num_verts);
		}
		else
		{
			new_normals.clear();
		}
		if (!tex_coords.empty())
		{
			new_tex_coords.resize(new_num_verts);
		}
		else
		{
			new_tex_coords.clear();
		}

		num_verts = new_num_verts;
		unsigned int i, k, idx;
		unsigned int count = 0;

		// split shared verices
		for (i = 0; i < num_faces; i++)
		{     // Go through all of the faces
			for (k = 0; k < 3; k++)
			{
				idx = faces[i].VertsIndices[k];
				faces[i].VertsIndices[k] = count;
				new_verts[count] = verts[idx];
				if (!normals.empty()) new_normals[count] = normals[idx];
				if (!tex_coords.empty()) new_tex_coords[count] = tex_coords[idx];
				count++;
				if (count > new_num_verts)
				{
					GPVULC_NOTIFY(LOG_WARN, "gvSplitTriVerts - vertex list overflow\n");
					break;
				}
			}
		}

		return true;
	}

	/*
	//------------------------------------------------------------------------------
	// Conversion from/to string
	//------------------------------------------------------------------------------


	bool FromString( const std::string&s, TexCoord& val ) {
	  Vec2 v = val;
	  if( !FromString( s, v ) ) return false;
	  val = TexCoord(v);
	  return true;
	}

	TextBuffer ToString( const TexCoord& val ) {
	  Vec2 v(val.U,val.V);
	  return ToString( v );
	}


	template < class itemType, class listType >
	bool gvListFromString(const std::string&s, listType& val) {
	  if (s==nullptr)
	  {
		return false;
	  }
	  TextParser p;
	  p.SetText(s);
	  const std::string& t = nullptr;
	  itemType c;
	  while( !p.Complete() ) {
		t = p.GetToken(", ");
		if( FromString( t, c ) ) val.Add( c );
	  }
	  return true;
	}


	template < class itemType >
	TextBuffer gvListToString(const itemType& mc) {
	  TextBuffer s;
	  int i;
	  for( i=0; i<mc.Size(); i++ ) {
		s << ToString( mc[i] ) << ", ";
	  }
	  return s;
	}


	bool FromString(const std::string&s, MColor& val) {
	  return gvListFromString<ColorRGBA,MColor>(s,val);
	}

	TextBuffer ToString(const MColor& mc) {
	  return gvListToString<MColor>(mc);
	}

	bool FromString(const std::string&s, MTexCoord& val) {
	  return gvListFromString<TexCoord,MTexCoord>(s,val);
	}

	TextBuffer ToString(const MTexCoord& mc) {
	  return gvListToString<MTexCoord>(mc);
	}

	*/

}
