//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Material data


#include <gpvulc/ds/Material.h>

namespace gpvulc
{

	//------------------------------------------------------------------------------
	// Classes implementation
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	//  MtlParams

	MtlParams::MtlParams()
	{
		_ConstructorInclude();
	}


	MtlParams::MtlParams(const MtlParams& mtlParams)
	{

		_ConstructorInclude();
		_Copy(mtlParams);
	}


	MtlParams::~MtlParams()
	{


	}


	void MtlParams::_Init()
	{

		Color = ColorRGBA(-1, -1, -1, -1);
		Shininess = 0.0f;
		SpecularLevel = 0.0f;
		Opacity = 1.0f;
		DiffuseOW = false;
		SpecularOW = false;
		TwoSided = false;
		Lighting = true;
	}


	void MtlParams::_ConstructorInclude()
	{

		_Init();
	}


	void MtlParams::_Copy(const MtlParams& obj)
	{

		Color = obj.Color;
		Ambient = obj.Ambient;
		Diffuse = obj.Diffuse;
		Specular = obj.Specular;
		Emission = obj.Emission;
		Shininess = obj.Shininess;
		SpecularLevel = obj.SpecularLevel;
		Opacity = obj.Opacity;
		DiffuseOW = obj.DiffuseOW;
		SpecularOW = obj.SpecularOW;
		TwoSided = obj.TwoSided;
		Lighting = obj.Lighting;
	}


	MtlParams& MtlParams::operator =(const MtlParams& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  MtlTexMap

	MtlTexMap::MtlTexMap()
	{

		_ConstructorInclude();
	}


	MtlTexMap::MtlTexMap(const MtlTexMap& _gvmtltexmap)
	{

		_ConstructorInclude();
		_Copy(_gvmtltexmap);
	}


	MtlTexMap::~MtlTexMap()
	{


	}


	void MtlTexMap::_Init()
	{

		Type = 0;
		FileName.clear();
		Target = MTL_TEX2D;
		OpacityIsZero = false;
		Compressed = false;
		Rotation = 0.0f;
		AmountOf = 1.0f;
		UScale = 1.0f;
		VScale = 1.0f;
		UOffset = 0.0f;
		VOffset = 0.0f;
		Anisotropy = 0.0f;
		BlurFactor = 0.0f;
	}


	void MtlTexMap::_ConstructorInclude()
	{

		_Init();
	}


	void MtlTexMap::_Copy(const MtlTexMap& obj)
	{

		Type = obj.Type;
		FileName = obj.FileName;
		Target = obj.Target;
		OpacityIsZero = obj.OpacityIsZero;
		Compressed = obj.Compressed;
		Rotation = obj.Rotation;
		AmountOf = obj.AmountOf;
		UScale = obj.UScale;
		VScale = obj.VScale;
		UOffset = obj.UOffset;
		VOffset = obj.VOffset;
		Anisotropy = obj.Anisotropy;
		BlurFactor = obj.BlurFactor;
	}


	MtlTexMap& MtlTexMap::operator =(const MtlTexMap& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  Material

	Material::Material()
	{

		_ConstructorInclude();
	}


	Material::Material(const Material& _gvmaterial)
	{

		_ConstructorInclude();
		_Copy(_gvmaterial);
	}


	Material::~Material()
	{


	}


	void Material::_Init()
	{

		Type = 0;
		Name.clear();
	}


	void Material::_ConstructorInclude()
	{

		_Init();
	}


	void Material::_Copy(const Material& obj)
	{

		Type = obj.Type;
		Parameters = obj.Parameters;
		Maps = obj.Maps;
		Name = obj.Name;
	}


	Material& Material::operator =(const Material& obj)
	{

		_Copy(obj);
		return *this;
	}




	//------------------------------------------------------------------------------
	//  MtlLib

	Material MtlLib::DefaultMaterial;

	MtlLib::MtlLib()
	{

		_ConstructorInclude();
	}


	MtlLib::MtlLib(const MtlLib& _gvmtllib)
	{

		_ConstructorInclude();
		_Copy(_gvmtllib);
	}


	Material& MtlLib::GetMaterial(const std::string& name)
	{

		for (int i = 0; i < Materials.Size(); i++)
		{
			if (Materials[i].Name == name)
			{
				return Materials[i];
			}
		}
		return DefaultMaterial;
	}


	MtlLib::~MtlLib()
	{


	}


	void MtlLib::_Init()
	{

		Name.clear();
	}


	void MtlLib::_ConstructorInclude()
	{

		_Init();
	}


	void MtlLib::_Copy(const MtlLib& obj)
	{

		Materials = obj.Materials;
		Name = obj.Name;
	}


	MtlLib& MtlLib::operator =(const MtlLib& obj)
	{

		_Copy(obj);
		return *this;
	}

} // namespace gpvulc


