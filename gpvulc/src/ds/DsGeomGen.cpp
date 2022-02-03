//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Geometry generation for data sets


#include <gpvulc/ds/DsGeomGen.h>


/// Maximum quality allowed for primitive generation (3,200,000 triangles)
#define DS_MAXPRIMQUALITY 3200000


  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
# define DS_COS45 0.707106781186547524f

namespace gpvulc
{
	std::string ToString(DsGeoPrimType  e)
	{

		switch (e)
		{
		case DSPRIM_TRIANGLE: return "triangle";
		case DSPRIM_RECT: return "rectangle";
		case DSPRIM_CIRCLE: return "circle";
		case DSPRIM_ELLIPSE: return "ellipse";
		case DSPRIM_BOX: return "box";
		case DSPRIM_SPHERE: return "sphere";
		case DSPRIM_SOUTH_EMISPHERE: return "south emisphere";
		case DSPRIM_NORTH_EMISPHERE: return "north emisphere";
		case DSPRIM_CYLSURF: return "cylindrical surface";
		case DSPRIM_CYLINDER: return "cylinder";
		case DSPRIM_CAPSULE: return "capsule";
		case DSPRIM_CONESURF: return "conic surface";
		case DSPRIM_CONE: return "cone";
		}
		return "";
	}


	bool FromString(const std::string& s, DsGeoPrimType & e)
	{

		TextBuffer str(s);
		int val = -1;
		if (str == "triangle") val = e = DSPRIM_TRIANGLE;
		else if (str == "rectangle") val = e = DSPRIM_RECT;
		else if (str == "circle") val = e = DSPRIM_CIRCLE;
		else if (str == "ellipse") val = e = DSPRIM_ELLIPSE;
		else if (str == "box") val = e = DSPRIM_BOX;
		else if (str == "sphere") val = e = DSPRIM_SPHERE;
		else if (str == "north emisphere") val = e = DSPRIM_NORTH_EMISPHERE;
		else if (str == "south emisphere") val = e = DSPRIM_SOUTH_EMISPHERE;
		else if (str == "cylindrical surface") val = e = DSPRIM_CYLSURF;
		else if (str == "cylinder") val = e = DSPRIM_CYLINDER;
		else if (str == "capsule") val = e = DSPRIM_CAPSULE;
		else if (str == "conic surface") val = e = DSPRIM_CONESURF;
		else if (str == "cone") val = e = DSPRIM_CONE;
		else val = e = DSPRIM_UNDEF;

		return (val >= 0);
	}



	// DsGeoPrim implementation
	DsGeoPrim::DsGeoPrim() : Type(DSPRIM_UNDEF), Size(1.0f), AxisOrientation(DS_X), ShapeAlignment(DS_ALIGN_CENTER)
	{
		Slices = 12;
		Quality = 20;
		Flip = false;
		//Material.Name = "prim_default";
		Material.Parameters.Specular.Set(0.2f, 0.2f, 0.2f, 1.0f);
		Material.Parameters.Diffuse.Set(0.4f, 0.4f, 0.4f, 1.0f);
		Material.Parameters.Ambient.Set(0.1f, 0.1f, 0.1f, 1.0f);
		Material.Parameters.Shininess = 10.0f;
	}

	// primitive generation
	namespace
	{
		/// Remap coordinates along the given axis (X axis = no change)
		inline Vec3 _gvRemapCoords(int align, float x, float y, float z)
		{
			Vec3 r;
			switch (align)
			{
			case DS_X:
				r.Set(x, y, z);
				break;
			case DS_Y:
				r.Set(z, x, y);
				break;
			case DS_Z:
				r.Set(y, z, x);
				break;
			}
			return r;
		}
		inline Vec3 _gvRemapCoords(int align, Vec3 v)
		{
			return _gvRemapCoords(align, v.X, v.Y, v.Z);
		}


		/// compute the original coordinates after a remapping (see _gvRemapCoords())
		inline Vec3 _gvUnremapCoords(int align, float x, float y, float z)
		{
			Vec3 r;
			switch (align)
			{
			case DS_X:
				r.Set(x, y, z);
				break;
			case DS_Y:
				r.Set(y, z, x);
				break;
			case DS_Z:
				r.Set(z, x, y);
				break;
			}
			return r;
		}


		inline AABBox _GetPrimMinMax(const DsGeoPrim& prim)
		{
			AABBox aabb;
			aabb.SetSizeOffsetAlignment(prim.Size, prim.Offset, prim.ShapeAlignment);
			Vec3 min_pnt, max_pnt;
			if (prim.Flip)
			{
				max_pnt.Set(aabb.Xmin, aabb.Ymin, aabb.Zmin);
				min_pnt.Set(aabb.Xmax, aabb.Ymax, aabb.Zmax);
			}
			else
			{
				min_pnt.Set(aabb.Xmin, aabb.Ymin, aabb.Zmin);
				max_pnt.Set(aabb.Xmax, aabb.Ymax, aabb.Zmax);
			}
			aabb.Set(min_pnt, max_pnt);

			return aabb;
		}



		static std::vector<Vec3> _DsGenPyramidVerts = {
			// vertex
			{1, 0, 0},
			// base
			{0, -DS_COS45,-DS_COS45},
			{0, -DS_COS45, DS_COS45},
			{0,  DS_COS45, DS_COS45},
			{0,  DS_COS45,-DS_COS45},
			// downvertex
			{-1, 0, 0},
		};

		static std::vector< std::vector<unsigned> > _DsGenPyramidFaces = {
			// sides
			{0, 2, 1},    {0, 3, 2},
			{0, 4, 3},    {0, 1, 4},
			// base
			{3, 4, 1},    {1, 4, 2},
		};


		static std::vector< std::vector<unsigned> > _DsGenDownPyramidFaces = {
			// sides
			{5, 1, 2},    {5, 2, 3},
			{5, 3, 4},    {5, 4, 1},
			// base
			{3, 1, 4},    {1, 2, 4},
		};


		static std::vector<Vec3> _DsGenIcosahedronVerts = {
		  {-ICX, 0, ICZ},
		  {ICX, 0, ICZ},
		  {-ICX, 0, -ICZ},
		  {ICX, 0, -ICZ},
		  {0, ICZ, ICX},
		  {0, ICZ, -ICX},
		  {0, -ICZ, ICX},
		  {0, -ICZ, -ICX},
		  {ICZ, ICX, 0},
		  {-ICZ, ICX, 0},
		  {ICZ, -ICX, 0},
		  {-ICZ, -ICX, 0}
		};


		static std::vector< std::vector<unsigned> > _DsGenIcosahedronFaces = {
		  {0, 4, 1},    {0, 9, 4},
		  {9, 5, 4},    {4, 5, 8},
		  {4, 8, 1},    {8, 10, 1},
		  {8, 3, 10},   {5, 3, 8},
		  {5, 2, 3},    {2, 7, 3},
		  {7, 10, 3},   {7, 6, 10},
		  {7, 11, 6},   {11, 0, 6},
		  {0, 1, 6},    {6, 1, 10},
		  {9, 0, 11},   {9, 11, 2},
		  {9, 2, 5},    {7, 2, 11},
		};


		// generate recursively a sphere of radius 1 centered in 0,0,0
		// level=number of iterations
		void DsGenSpherePatch(DynArray<Vec3>& verts, DynArray<unsigned>& indices,
			float p1[3], float p2[3], float p3[3],
			unsigned i1, unsigned i2, unsigned i3,
			unsigned level)
		{
			int i;
			if (level > 0)
			{
				float q1[3], q2[3], q3[3];     // sub-vertices
				for (i = 0; i < 3; i++)
				{
					q1[i] = 0.5f*(p1[i] + p2[i]);
					q2[i] = 0.5f*(p2[i] + p3[i]);
					q3[i] = 0.5f*(p3[i] + p1[i]);
				}
				float length1 = (float)(1.0 / sqrt(q1[0] * q1[0] + q1[1] * q1[1] + q1[2] * q1[2]));
				float length2 = (float)(1.0 / sqrt(q2[0] * q2[0] + q2[1] * q2[1] + q2[2] * q2[2]));
				float length3 = (float)(1.0 / sqrt(q3[0] * q3[0] + q3[1] * q3[1] + q3[2] * q3[2]));
				for (i = 0; i < 3; i++)
				{
					q1[i] *= length1;
					q2[i] *= length2;
					q3[i] *= length3;
				}
				unsigned iq1 = verts.Size();
				unsigned iq2 = iq1 + 1;
				unsigned iq3 = iq2 + 1;
				verts.Add(q1);
				verts.Add(q2);
				verts.Add(q3);
				DsGenSpherePatch(verts, indices, p1, q1, q3, i1, iq1, iq3, level - 1);
				DsGenSpherePatch(verts, indices, q1, p2, q2, iq1, i2, iq2, level - 1);
				DsGenSpherePatch(verts, indices, q1, q2, q3, iq1, iq2, iq3, level - 1);
				DsGenSpherePatch(verts, indices, q3, q2, p3, iq3, iq2, i3, level - 1);
			}
			else
			{
				indices.Add(i1);
				indices.Add(i2);
				indices.Add(i3);
			}
		}

	}

	bool DsGenBox(DsGeom* geom, const AABBox& aabb, const std::string& mtl_name)
	{

		if (!geom)
			return false;

		int nv = geom->Vertices.Size();
		int nn = geom->Normals.Size();

		float xmin(aabb.Xmin), xmax(aabb.Xmax);
		float ymin(aabb.Ymin), ymax(aabb.Ymax);
		float zmin(aabb.Zmin), zmax(aabb.Zmax);

		geom->AddVertex(Vec3(xmin, ymin, zmin)); //0
		geom->AddVertex(Vec3(xmin, ymin, zmax)); //1
		geom->AddVertex(Vec3(xmin, ymax, zmin)); //2
		geom->AddVertex(Vec3(xmin, ymax, zmax)); //3
		geom->AddVertex(Vec3(xmax, ymin, zmin)); //4
		geom->AddVertex(Vec3(xmax, ymin, zmax)); //5
		geom->AddVertex(Vec3(xmax, ymax, zmin)); //6
		geom->AddVertex(Vec3(xmax, ymax, zmax)); //7

		geom->AddNormal(Vec3(-1, 0, 0)); //0
		geom->AddNormal(Vec3(1, 0, 0)); //1
		geom->AddNormal(Vec3(0, -1, 0)); //2
		geom->AddNormal(Vec3(0, 1, 0)); //3
		geom->AddNormal(Vec3(0, 0, -1)); //4
		geom->AddNormal(Vec3(0, 0, 1)); //5

		// create the box mesh
		DsMesh* box_mesh = new DsMesh;
		geom->Meshes.Add(box_mesh);
		box_mesh->MtlName = mtl_name;
		box_mesh->SetType(DS_QUADS);

		int nidx = 0;

		// xmin
		box_mesh->AddVertex(nv + 0);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 1);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 3);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 2);
		box_mesh->AddNormal(nn + nidx);
		nidx++;

		// xmax
		box_mesh->AddVertex(nv + 4);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 6);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 7);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 5);
		box_mesh->AddNormal(nn + nidx);
		nidx++;

		// ymin
		box_mesh->AddVertex(nv + 0);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 4);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 5);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 1);
		box_mesh->AddNormal(nn + nidx);
		nidx++;

		// ymax
		box_mesh->AddVertex(nv + 2);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 3);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 7);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 6);
		box_mesh->AddNormal(nn + nidx);
		nidx++;

		// zmin
		box_mesh->AddVertex(nv + 0);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 2);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 6);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 4);
		box_mesh->AddNormal(nn + nidx);
		nidx++;

		// zmax
		box_mesh->AddVertex(nv + 1);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 5);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 7);
		box_mesh->AddNormal(nn + nidx);
		box_mesh->AddVertex(nv + 3);
		box_mesh->AddNormal(nn + nidx);

		box_mesh->MtlName = mtl_name;

		return true;
	}


	bool DsGenAABB(DsGeom* geom, const AABBox& aabb, const std::string& mtl_name)
	{
		return DsGenBox(geom, aabb, mtl_name);
	}


	bool DsGenSphere(DsGeom* geom, int align, const AABBox& aabb, unsigned quality, int hemisphere, bool flip, const std::string& mtl_name)
	{
		if (!geom)
			return false;

		int i = 0;
		DynArray<Vec3> verts;
		DynArray<unsigned> indices;
		// limit iterations to a reasonable number
		if (quality > DS_MAXPRIMQUALITY)
			quality = DS_MAXPRIMQUALITY;
		unsigned num_verts = 12;
		unsigned num_faces = 20;
		unsigned level = 0;
		if (hemisphere)
		{
			num_verts = 5;
			num_faces = 4;
		}
		while (num_faces * 4 <= quality)
		{
			num_faces *= 4;
			++level;
		}
		if (hemisphere)
		{
			num_verts = num_faces + 1; // vertices shared among 3 faces + 1 shared among 4
		}
		else
		{
			if (num_faces > 12) num_verts = num_faces + 40; // vertices shared among 3 faces + 40 shared among 4 or more
		}

		if (hemisphere)
		{
			verts.Allocate(num_verts);
			indices.Allocate(num_faces * 3);
			if (hemisphere < 0)
				verts.Copy(_DsGenPyramidVerts, 1, (unsigned)_DsGenPyramidVerts.size() - 1);
			else
				verts.Copy(_DsGenPyramidVerts, (unsigned)_DsGenPyramidVerts.size() - 1);
			// use only pyramid sides (4 instead of 6)
			for (i = 0; i < 4; i++)
			{

				unsigned i1, i2, i3;

				if (flip)
				{
					i1 = 2; i2 = 1; i3 = 0;
				}
				else
				{
					i1 = 0; i2 = 1; i3 = 2;
				}
				unsigned v1, v2, v3;
				if (hemisphere < 0)
				{
					v1 = _DsGenDownPyramidFaces[i][i1] - 1;
					v2 = _DsGenDownPyramidFaces[i][i2] - 1;
					v3 = _DsGenDownPyramidFaces[i][i3] - 1;
				}
				else
				{
					v1 = _DsGenPyramidFaces[i][i1];
					v2 = _DsGenPyramidFaces[i][i2];
					v3 = _DsGenPyramidFaces[i][i3];
				}
				DsGenSpherePatch(verts, indices,
					verts[v1], verts[v2], verts[v3],
					v1, v2, v3,
					level);
			}
		}
		else
		{
			verts.Copy(_DsGenIcosahedronVerts);
			//for ( i=0; i<20; i++) {
			//  verts.Add( _DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][0]] );
			//  verts.Add( _DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][1]] );
			//  verts.Add( _DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][2]] );
			//}
			for (i = 0; i < 20; i++)
			{
				DsGenSpherePatch(verts, indices,
					_DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][2]],
					_DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][1]],
					_DsGenIcosahedronVerts[_DsGenIcosahedronFaces[i][0]],

					_DsGenIcosahedronFaces[i][2],
					_DsGenIcosahedronFaces[i][1],
					_DsGenIcosahedronFaces[i][0],

					level);
			}
		}


		// the axis aligned bounding box corners are relative to the final primitive
		// so let's get them relative to the starting (X-axis aligned) primitive
		Vec3 min_pnt = _gvUnremapCoords(align, aabb.Xmin, aabb.Ymin, aabb.Zmin);
		Vec3 max_pnt = _gvUnremapCoords(align, aabb.Xmax, aabb.Ymax, aabb.Zmax);

		Vec3 radius = (max_pnt - min_pnt) / 2.0f;
		Vec3 center = (max_pnt + min_pnt) / 2.0f;
		if (hemisphere)
		{
			radius.X = max_pnt.X - min_pnt.X;
			if (hemisphere < 0)
				center.X = max_pnt.X;
			else
				center.X = min_pnt.X;
		}
		radius = radius.AbsValues();

		int nv = geom->Vertices.Size();
		int nn = geom->Normals.Size();
		int nt = geom->TexCoords.Size();

		Vec3 v, n;

		for (i = 0; i < verts.Size(); i++)
		{
			const Vec3& vi = verts[i];
			v = center + radius * vi;
			n = flip ? -vi : vi;
			float h = DS_RAD2DEG * acos(vi.X);
			if (vi.Z < 0.0f) h = -h;
			float p = DS_RAD2DEG * asin(vi.Y);
			TexCoord tc(0.5f + h / 360.0f, 0.5f + p / 180.0f);
			geom->Vertices.Add(_gvRemapCoords(align, v));
			geom->Normals.Add(_gvRemapCoords(align, n));
			geom->TexCoords.Add(tc);
			// TODO: "24/06/2016","GPV","Texture coordinates are incorrect if using shared vertices")
		}

		// create the mesh
		DsMesh* mesh = new DsMesh;
		geom->Meshes.Add(mesh);
		mesh->MtlName = mtl_name;
		mesh->SetType(DS_TRIANGLES);

		int start, stop, inc;
		if (flip)
			start = indices.Size() - 1, stop = -1, inc = -1;
		else
			start = 0, stop = indices.Size(), inc = 1;

		for (i = start; i != stop; i += inc)
		{
			//for ( i=0; i<indices.Size(); i++) {
			mesh->VertIdx.Add(nv + indices[i]);
			mesh->NormIdx.Add(nn + indices[i]);
			mesh->TexCoordIdx.Add(nt + indices[i]);
		}

		return true;
	}


	bool DsGenConeSurf(DsGeom* geom, int align, const AABBox& aabb, unsigned num_sides, bool cone, bool base, bool flip, const std::string& mtl_name)
	{

		if (!geom)
			return false;

		int nv = geom->Vertices.Size();
		int nn = geom->Normals.Size();

		int i;

		// the axis aligned bounding box corners are relative to the final primitive
		// so let's get them relative to the starting (X-axis aligned) primitive
		Vec3 min_pnt = _gvUnremapCoords(align, aabb.Xmin, aabb.Ymin, aabb.Zmin);
		Vec3 max_pnt = _gvUnremapCoords(align, aabb.Xmax, aabb.Ymax, aabb.Zmax);

		float yradius = (max_pnt.Y - min_pnt.Y) / 2.0f;
		float zradius = (max_pnt.Z - min_pnt.Z) / 2.0f;
		float ycenter = (max_pnt.Y + min_pnt.Y) / 2.0f;
		float zcenter = (max_pnt.Z + min_pnt.Z) / 2.0f;

		Vec3 vert;
		Vec3 generatrix;
		Vec3 apex(cone ? max_pnt.X : min_pnt.X, ycenter, zcenter);
		Vec3 height(ValuesDistance(max_pnt.X, min_pnt.X), 0, 0);
		Vec3 n;

		vert.X = min_pnt.X;

		float a = 0.0;
		float sa = 0.0;
		float ca = 0.0;
		float step = (float)(DS_2PId / (double)num_sides);

		for (i = 0; i < (int)num_sides; i++)
		{
			a += step;
			sa = sin(a);
			ca = cos(a);
			vert.Y = ycenter + yradius*sa;
			vert.Z = zcenter + zradius*ca;
			geom->Vertices.Add(_gvRemapCoords(align, vert));
			if (cone)
			{
				generatrix = vert - apex;
				n = height + generatrix;
				n.Normalize();
				geom->Normals.Add(_gvRemapCoords(align, n));
			}
			else
			{
				geom->Normals.Add(_gvRemapCoords(align, 0, sa, ca));
			}
		}

		geom->AddVertex(_gvRemapCoords(align, apex));
		// unique normal
		geom->AddNormal(_gvRemapCoords(align, 1, 0, 0));
		if (base)
		{
			geom->AddVertex(_gvRemapCoords(align, min_pnt.X, ycenter, zcenter));
			geom->AddNormal(_gvRemapCoords(align, -1, 0, 0));
		}

		// create the mesh
		DsMesh* ellipse_mesh = new DsMesh;
		geom->Meshes.Add(ellipse_mesh);
		ellipse_mesh->MtlName = mtl_name;
		ellipse_mesh->SetType(DS_TRIANGLES);

		if (flip)
		{
			for (i = nn; i < geom->Normals.Size(); i++)
			{
				geom->Normals[i] = -geom->Normals[i];
			}
		}

		// ellipse / conic surface
		for (i = 0; i < (int)num_sides; i++)
		{
			ellipse_mesh->AddVertex(nv + num_sides);
			ellipse_mesh->AddNormal(nn + num_sides);
			if (!cone)
			{
				ellipse_mesh->AddNormal(nn + num_sides);
				ellipse_mesh->AddNormal(nn + num_sides);
			}
			int pv = (i == 0) ? num_sides - 1 : i - 1;
			if (flip)
			{
				ellipse_mesh->AddVertex(nv + pv);
				if (cone) ellipse_mesh->AddNormal(nn + pv);
				ellipse_mesh->AddVertex(nv + i);
				if (cone) ellipse_mesh->AddNormal(nn + i);
			}
			else
			{
				ellipse_mesh->AddVertex(nv + i);
				if (cone) ellipse_mesh->AddNormal(nn + i);
				ellipse_mesh->AddVertex(nv + pv);
				if (cone) ellipse_mesh->AddNormal(nn + pv);
			}
		}

		// cone base
		if (base)
		{
			for (i = 0; i < (int)num_sides; i++)
			{
				ellipse_mesh->AddVertex(nv + num_sides + 1);
				ellipse_mesh->AddNormal(nn + num_sides + 1);
				ellipse_mesh->AddNormal(nn + num_sides + 1);
				ellipse_mesh->AddNormal(nn + num_sides + 1);
				int pv = (i == 0) ? num_sides - 1 : i - 1;
				if (flip)
				{
					ellipse_mesh->AddVertex(nv + i);
					ellipse_mesh->AddVertex(nv + pv);
				}
				else
				{
					ellipse_mesh->AddVertex(nv + pv);
					ellipse_mesh->AddVertex(nv + i);
				}
			}
		}

		return true;
	}


	bool DsGenCylSurf(DsGeom* geom, int align, const AABBox& aabb, unsigned num_sides, const std::string& mtl_name)
	{

		if (!geom)
			return false;

		int nv = geom->Vertices.Size();
		int nn = geom->Normals.Size();

		int i;

		Vec3 min_pnt = _gvUnremapCoords(align, aabb.Xmin, aabb.Ymin, aabb.Zmin);
		Vec3 max_pnt = _gvUnremapCoords(align, aabb.Xmax, aabb.Ymax, aabb.Zmax);

		float xmin = min_pnt.X;
		float xmax = max_pnt.X;

		float yradius = (max_pnt.Y - min_pnt.Y) / 2.0f;
		float zradius = (max_pnt.Z - min_pnt.Z) / 2.0f;
		float ycenter = (max_pnt.Y + min_pnt.Y) / 2.0f;
		float zcenter = (max_pnt.Z + min_pnt.Z) / 2.0f;

		float x, y, z;

		x = xmin;

		float a = 0.0;
		float sa = 0.0;
		float ca = 0.0;
		float step = (float)(DS_2PId / (double)num_sides);

		for (i = 0; i < (int)num_sides; i++)
		{
			a += step;
			sa = sin(a);
			ca = cos(a);
			y = ycenter + yradius*sa;
			z = zcenter + zradius*ca;
			geom->AddVertex(_gvRemapCoords(align, x, y, z));
			geom->AddNormal(_gvRemapCoords(align, 0, sa, ca));
		}

		x = xmax;
		for (i = 0; i < (int)num_sides; i++)
		{
			a += step;
			sa = sin(a);
			ca = cos(a);
			y = ycenter + yradius*sa;
			z = zcenter + zradius*ca;
			geom->AddVertex(_gvRemapCoords(align, x, y, z));
			geom->AddNormal(_gvRemapCoords(align, 0, sa, ca));
		}
		// axis segment points (for triangle fans)
		geom->AddVertex(_gvRemapCoords(align, xmin, 0, 0));
		geom->AddNormal(_gvRemapCoords(align, -1, 0, 0));
		geom->AddVertex(_gvRemapCoords(align, xmax, 0, 0));
		geom->AddNormal(_gvRemapCoords(align, 1, 0, 0));

		// create the cylindrical surface
		DsMesh* cyl_mesh = new DsMesh;
		geom->Meshes.Add(cyl_mesh);
		cyl_mesh->MtlName = mtl_name;
		cyl_mesh->SetType(DS_QUADS);
		for (i = 0; i < (int)num_sides - 1; i++)
		{
			cyl_mesh->AddVertex(nv + i);
			cyl_mesh->AddVertex(nv + num_sides + i);
			cyl_mesh->AddVertex(nv + num_sides + i + 1);
			cyl_mesh->AddVertex(nv + i + 1);
			cyl_mesh->AddNormal(nn + i);
			cyl_mesh->AddNormal(nn + num_sides + i);
			cyl_mesh->AddNormal(nn + num_sides + i + 1);
			cyl_mesh->AddNormal(nn + i + 1);
		}

		// close the surface joining the first and the last facet
		i = num_sides - 1;
		cyl_mesh->AddVertex(nv + i);
		cyl_mesh->AddVertex(nv + num_sides + i);
		cyl_mesh->AddVertex(nv + num_sides);
		cyl_mesh->AddVertex(nv + 0);
		cyl_mesh->AddNormal(nn + i);
		cyl_mesh->AddNormal(nn + num_sides + i);
		cyl_mesh->AddNormal(nn + num_sides);
		cyl_mesh->AddNormal(nn + 0);

		return true;
	}

	/// Generate a cylinder
	/// @param geom target geometry
	/// @param align cylinder alignment along reference axes: DS_X, DS_Y, DS_Z
	/// @param aabb axis-aligned bounding box of the cylinder
	/// @param num_sides number of segments approximating the cylider surface
	/// @param mtl_name material name to be set to geometry meshes
	/// @return It returns false if the given geometry is nullptr, true otherwise
	bool DsGenCylinder(DsGeom* geom, int align, const AABBox& aabb, unsigned num_sides, const std::string& mtl_name)
	{
		if (!geom)
			return false;

		int nv = geom->Vertices.Size();
		int nn = geom->Normals.Size();

		DsGenCylSurf(geom, align, aabb, num_sides, mtl_name);

		int i;

		int nnc = geom->Normals.Size();

		// axis segment points (for triangle fans)
		geom->AddNormal(_gvRemapCoords(align, -1, 0, 0));
		geom->AddNormal(_gvRemapCoords(align, 1, 0, 0));

		// create the first circle
		DsMesh* cyl_mesh_cap_min = new DsMesh;
		geom->Meshes.Add(cyl_mesh_cap_min);
		cyl_mesh_cap_min->MtlName = mtl_name;
		cyl_mesh_cap_min->SetType(DS_POLYGON);
		for (i = 0; i < (int)num_sides; i++)
		{
			cyl_mesh_cap_min->AddVertex(nv + i);
			cyl_mesh_cap_min->AddNormal(nnc);
		}

		// create the second circle (revert order)
		DsMesh* cyl_mesh_cap_max = new DsMesh;
		geom->Meshes.Add(cyl_mesh_cap_max);
		cyl_mesh_cap_max->MtlName = mtl_name;
		cyl_mesh_cap_max->SetType(DS_POLYGON);
		for (i = (int)num_sides - 1; i >= 0; i--)
		{
			cyl_mesh_cap_max->AddVertex(nv + num_sides + i);
			cyl_mesh_cap_max->AddNormal(nnc + 1);
		}
		return true;
	}


	bool DsGenGeomFromPrimBox(DsGeom* geom, const DsGeoPrim& prim)
	{

		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		return DsGenBox(geom, _GetPrimMinMax(prim), prim.Material.Name);
	}


	bool DsGenGeomFromPrimEllipse(DsGeom* geom, const DsGeoPrim& prim)
	{
		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		return DsGenConeSurf(geom, prim.AxisOrientation, _GetPrimMinMax(prim), prim.Slices, false, false, prim.Flip, prim.Material.Name);
	}


	bool DsGenGeomFromPrimCone(DsGeom* geom, const DsGeoPrim& prim)
	{
		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		bool cone = false;
		bool base = false;
		switch (prim.Type)
		{
		case DSPRIM_CONE:
			base = true;
		case DSPRIM_CONESURF:
			cone = true;
			break;
		}
		return DsGenConeSurf(geom, prim.AxisOrientation, _GetPrimMinMax(prim), prim.Slices, cone, base, prim.Flip, prim.Material.Name);
	}


	bool DsGenGeomFromPrimSphereSurf(DsGeom* geom, const DsGeoPrim& prim)
	{

		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		int hemisphere;
		switch (prim.Type)
		{
		case DSPRIM_NORTH_EMISPHERE: hemisphere = 1; break;
		case DSPRIM_SOUTH_EMISPHERE: hemisphere = -1; break;
		case DSPRIM_SPHERE: hemisphere = 0; break;
		default:
			return false;
		}

		return DsGenSphere(geom, prim.AxisOrientation, _GetPrimMinMax(prim), prim.Quality, hemisphere, prim.Flip, prim.Material.Name);
	}


	bool DsGenGeomFromPrimCylSurf(DsGeom* geom, const DsGeoPrim& prim)
	{

		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		return DsGenCylSurf(geom, prim.AxisOrientation, _GetPrimMinMax(prim), prim.Slices, prim.Material.Name);
	}


	bool DsGenGeomFromPrimCyl(DsGeom* geom, const DsGeoPrim& prim)
	{

		if (!geom || prim.Type == DSPRIM_UNDEF)
			return false;

		return DsGenCylinder(geom, prim.AxisOrientation, _GetPrimMinMax(prim), prim.Slices, prim.Material.Name);
	}


	//bool DsGenGeomFromPrimCapsule( DsGeom* geom, const DsGeoPrim& prim ) {
	//  if( !geom || prim.Type==DSPRIM_UNDEF )
	//    return false;
	//
	//  if( !DsGenCylSurf( geom, prim.AxisOrientation, _GetPrimMinMax( prim ), prim.Slices, prim.Material.Name ) )
	//    return false;
	//  if( !DsGenSphere( geom, prim.AxisOrientation, _GetPrimMinMax( prim ), prim.Slices, prim.Material.Name ) )
	//    return false;
	//
	//  return true;
	//}





	bool DsGenGeomFromPrim(DsGeom* geom, DsGeoPrim& prim)
	{


		// TODO: "","","implement DsGenGeomFromPrim*()")
		switch (prim.Type)
		{
		case DSPRIM_BOX:
			return DsGenGeomFromPrimBox(geom, prim);

		case DSPRIM_TRIANGLE:
			prim.Slices = 3;
			return DsGenGeomFromPrimEllipse(geom, prim);
			//case DSPRIM_RECT:
		case DSPRIM_RHOMBUS:
			prim.Slices = 4;
			return DsGenGeomFromPrimEllipse(geom, prim);
			//case DSPRIM_CIRCLE:
		case DSPRIM_ELLIPSE:
			return DsGenGeomFromPrimEllipse(geom, prim);
		case DSPRIM_SPHERE:
		case DSPRIM_NORTH_EMISPHERE:
		case DSPRIM_SOUTH_EMISPHERE:
			return DsGenGeomFromPrimSphereSurf(geom, prim);
		case DSPRIM_CYLSURF:
			return DsGenGeomFromPrimCylSurf(geom, prim);
		case DSPRIM_CYLINDER:
			return DsGenGeomFromPrimCyl(geom, prim);
		case DSPRIM_CONESURF:
		case DSPRIM_CONE:
			return DsGenGeomFromPrimCone(geom, prim);
			//case DSPRIM_CAPSULE:
		case DSPRIM_UNDEF:
		default:
			// primitive not implemented
			return false;
		}

		return true;
	}


	bool DsGenObjFromPrim(DsObject* dsobj, DsGeoPrim& prim)
	{

		if (!dsobj)
		{
			return false;
		}
		if (prim.Material.Name.empty())
			prim.Material.Name = "prim_mtl";
		DsGeom* geom = new DsGeom(nullptr, TextBuffer("geo_") + prim.Material.Name);

		if (!DsGenGeomFromPrim(geom, prim))
		{
			delete geom;
			return false;
		}

		for (int i = 0; i < geom->Meshes.Size(); i++)
		{
			geom->Meshes[i]->MtlName = prim.Material.Name;
		}
		dsobj->Geoms.Add(geom);

		return true;
	}


	bool DsGenDatasetFromPrim(DsDataSet* dataset, MtlLib* mtllib, DsGeoPrim& prim, const std::string& name)
	{

		if (!dataset || !mtllib)
		{
			return false;
		}
		if (dataset->Name.IsEmpty())
			dataset->Name = name;
		if (mtllib->Name.empty())
			mtllib->Name = dataset->Name;
		if (prim.Material.Name.empty())
			prim.Material.Name = "prim_default";

		DsObject* dsobj = new DsObject(dataset, name.empty() ? "prim" : name);
		mtllib->Materials.Add(prim.Material);
		DsGenObjFromPrim(dsobj, prim);
		dataset->Objects.Add(dsobj);
		dataset->RootObjects.Add(dsobj);

		return true;
	}


	bool DsGenDatasetFromPrimList(DsDataSet* dataset, MtlLib* mtllib,
		DynArray<DsGeoPrim>& prim_list)
	{

		if (!dataset || !mtllib)
		{
			return false;
		}
		if (dataset->Name.IsEmpty())
			dataset->Name << "prim" << (void*)dataset;
		if (mtllib->Name.empty())
			mtllib->Name = dataset->Name;
		for (int p = 0; p < prim_list.Size(); p++)
		{

			DsGeoPrim& prim = prim_list[p];
			TextBuffer name = TextBuffer("prim") + TextBuffer(p);
			if (prim.Material.Name.empty())
				prim.Material.Name = name;
			DsGenDatasetFromPrim(dataset, mtllib, prim, name);
		}

		return true;
	}

} // namespace gpvulc

