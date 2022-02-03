//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Material data
/// @file Material.h
/// @author Giovanni Paolo Vigano'

//! Neutral material data strucures for material library definition.

#pragma once


/// @defgroup Material Material data
/// Neutral material data strucures for material library definition.
/// @author Giovanni Paolo Vigano'

/// @addtogroup DS
///@{

/// @addtogroup Material
///@{

#include "gpvulc/ds/MtlData.h"
#include <gpvulc/ds/util/DynArray.h>

namespace gpvulc
{

//!  Material parameters definition
class MtlParams {


public:
  
  //!  Main color multiplied by Diffuse component, each negative component is ignored (default=[-1,-1,-1,-1])
  ColorRGBA Color;
  
  //!  Ambient color component (default=[0.2,0.2,0.2,1], set alpha to 0 to ignore)
  ColorRGBA Ambient;
  
  //!  Diffuse color component (default=[0.8,0.8,0.8,1], set alpha to 0 to ignore)
  ColorRGBA Diffuse;
  
  //!  Specular color component (default=[0,0,0,1], set alpha to 0 to ignore)
  ColorRGBA Specular;
  
  //!  Emission color component (default=[0,0,0,1], set alpha to 0 to ignore)
  ColorRGBA Emission;
  float Shininess;	//!<  Shininess (0..128, default=0, set to -1 to ignore)
  float SpecularLevel;	//!<  Specular level (0..1, default=1, set to -1 to ignore)
  float Opacity;	
  bool DiffuseOW;	//!<  Diffuse texture overwrite diffuse color (see gvGeoState)
  bool SpecularOW;	//!<  Specular texture overwrite specular color (see gvGeoState)
  MtlParams();	//  Constructor
  MtlParams( const MtlParams& mtlParams );
  bool TwoSided;	//!<  True if both front and back side are rendered
  bool Lighting;	//!<  Lighting enabled (dafault = true)
  virtual ~MtlParams();

private:
  void _Init();	// [automatically generated]
  void _ConstructorInclude();	// [automatically generated]
  void _Copy( const MtlParams& obj );	// [automatically generated]

public:
  MtlParams& operator =( const MtlParams& obj );	// [automatically generated]
};





//!  Texture map definition
class MtlTexMap {


public:
  
  /*!
   Type of the texture map, it can be:
    TEX_NONE, TEX_DIFFUSE, TEX_OPACITY, TEX_BUMP, TEX_SPECULAR, TEX_SHININESS, TEX_SELF_ILLUMINATION, TEX_ENVIRONMENT, TEX_LIGHTMAP, TEX_CUBE_MAP
  */
  int Type;
  std::string FileName;	
  
  //!  Target. It can be MTL_TEX1D, MTL_TEX2D, MTL_TEX3D, or MTL_TEXCUBE.
  int Target;
  bool OpacityIsZero;	//!<  Flag set to true when the opacity is 0 instead of 1
  bool Compressed;	//!<  Flag set to true when the texture is created as compressed
  float Rotation;	//!<  Rotation along the normal axis
  
  //!  Amount of texture color (from 0.0 to 1.0) used for blending (default=1.0)
  float AmountOf;
  float UScale;	//!<  Horizontal scaling
  float VScale;	//!<  Vertical scaling
  float UOffset;	//!<  Horizontal offset
  float VOffset;	//!<  Vertical offset
  float Anisotropy;	//!<  Anisotropic filter
  MtlTexMap();
  MtlTexMap( const MtlTexMap& _gvmtltexmap );
  float BlurFactor;	//!<  Blurring factor
  virtual ~MtlTexMap();

private:
  void _Init();	// [automatically generated]
  void _ConstructorInclude();	// [automatically generated]
  void _Copy( const MtlTexMap& obj );	// [automatically generated]

public:
  MtlTexMap& operator =( const MtlTexMap& obj );	// [automatically generated]
};





//!  Complete material definition
class Material {


public:
  int Type;	
  MtlParams Parameters;	
  DynArray< MtlTexMap > Maps;	
  Material();
  Material( const Material& _gvmaterial );
  std::string Name;
  virtual ~Material();

private:
  void _Init();	// [automatically generated]
  void _ConstructorInclude();	// [automatically generated]
  void _Copy( const Material& obj );	// [automatically generated]

public:
  Material& operator =( const Material& obj );	// [automatically generated]
};





//!  Material library
class MtlLib {

public:
  DynArray< Material > Materials;	
  MtlLib();
  MtlLib( const MtlLib& _gvmtllib );
  std::string Name;	
  Material& GetMaterial( const std::string& name );
  static Material DefaultMaterial;	
  virtual ~MtlLib();

private:
  void _Init();	// [automatically generated]
  void _ConstructorInclude();	// [automatically generated]
  void _Copy( const MtlLib& obj );	// [automatically generated]

public:
  MtlLib& operator =( const MtlLib& obj );	// [automatically generated]
};





enum _MtlTexDim { MTL_TEX1D, MTL_TEX2D, MTL_TEX3D, MTL_TEXCUBE };


///@} (Material)

///@} (DS)

} // namespace gpvulc
