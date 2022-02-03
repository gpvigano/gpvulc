//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Basic material data
/// @file MtlData.h
/// @author Giovanni Paolo Vigano'

//! Neutral basic data strucures for material definition.

#pragma once

#include <gpvulc/ds/math/mathutil.h>

/// @addtogroup DS
///@{

/// @addtogroup Material
///@{

namespace gpvulc
{

	////****************************************************************************
	// SHADING FLAGS
	////****************************************************************************
	///@name Shading flags
	//@{

	enum ShadingFlag
	{
		TEX_NONE = 0x0000, //!< no texture
		TEX_DIFFUSE = 0x0001, //!< replaces diffuse color
		TEX_OPACITY = 0x0002, //!< gives transparency level
		TEX_NORMAL_MAP = 0x0004, //!< simulates irregular surfaces
		TEX_SPECULAR = 0x0008, //!< replaces specular color
		TEX_SHININESS = 0x0010, //!< modulates specular reflection level
		TEX_SELF_ILLUMINATION = 0x0020, //!< replaces emission color
		TEX_ENVIRONMENT = 0x0040, //!< simulates environment reflection
		TEX_FAST_ENVIRONMENT = 0x0080, //!< faster single-pass one
		TEX_LIGHTMAP = 0x0100, //!< precalculated illumination
		TEX_CUBE_MAP = 0x0200, //!< cube mapping reflection
		TEX_REFL_LEV = 0x0800, //!< reflection level modulation map
		TEX_BUMP = 0x0400, //!< surface level mapping
		TEX_ALL = TEX_DIFFUSE | TEX_OPACITY | TEX_ENVIRONMENT ///< all supported textures
		//#define   TEX_ALL TEX_DIFFUSE|TEX_OPACITY|TEX_NORMAL_MAP|TEX_SPECULAR| \
		//    TEX_SHININESS|TEX_SELF_ILLUMINATION|TEX_ENVIRONMENT|TEX_LIGHTMAP //!< all textures
	};

	//@}

	//! Material property identifiers
	enum MaterialProperty
	{
		MTL_BASECOLOR = 0x8001, //!< color used when lighting is disabled
		MTL_DIFFUSE = 0x8002, //!< diffuse color
		MTL_SPECULAR = 0x8004, //!< specular color
		MTL_AMBIENT = 0x8008, //!< ambient color
		MTL_EMISSION = 0x8010, //!< emission color

		MTL_OPACITY = 0x8020, //!< opacity level
		MTL_SHININESS = 0x8040, //!< shininess
		MTL_SPECULARLEV = 0x8080, //!< specular level

		MTL_LIGHTING = 0x8100, //!< enable lighting
		MTL_TWOSIDED = 0x8200, //!< enable two sided rendering
	};
	////****************************************************************************


	//! RGBA (Red,Green,Blue,Alpha) color
	struct ColorRGBA
	{
		//! Color component
		float R, G, B, A;

		///@name Constructors
		//@{

		ColorRGBA() { this->R = this->G = this->B = this->A = 0.0; }
		ColorRGBA(float gray, float a) { this->R = gray; this->G = gray; this->B = gray; this->A = a; }
		ColorRGBA(float r, float g, float b, float a) { this->R = r; this->G = g; this->B = b; this->A = a; }
		ColorRGBA(float f[4]) { this->R = f[0]; this->G = f[1]; this->B = f[2]; this->A = f[3]; }
		ColorRGBA(float f[3], float alpha) { this->R = f[0]; this->G = f[1]; this->B = f[2]; this->A = alpha; }
		ColorRGBA(const ColorRGBA& c, float alpha) { this->R = c.R; this->G = c.G; this->B = c.B; this->A = alpha; }

		//@}

		//! Set the color using a 32 bit hexadecimal value, e.g. ColorRGBA(0xFF000088) builds a transparent red
		void SetHex(unsigned int hex_val)
		{
			R = ((hex_val & 0xff000000) >> 24) / 255.0f;
			G = ((hex_val & 0x00ff0000) >> 16) / 255.0f;
			B = ((hex_val & 0x0000ff00) >> 8) / 255.0f;
			A = (hex_val & 0x000000ff) / 255.0f;
		}
		//! Get the color as a 32 bit hexadecimal value (e.g. 0xFF000088 for a transparent red)
		unsigned GetHex()
		{
			unsigned hex = 0;
			hex |= unsigned(R*255.0f) << 24;
			hex |= unsigned(G*255.0f) << 16;
			hex |= unsigned(B*255.0f) << 8;
			hex |= unsigned(A*255.0f);
			return hex;
		}
		//! Set the color using 4 integer values, e.g. ColorRGBA(255,0,0,8) builds a transparent red
		void SetInt(int r, int g, int b, int a = 255)
		{
			this->R = r / 255.0f;
			this->G = g / 255.0f;
			this->B = b / 255.0f;
			this->A = a / 255.0f;
		}

		void Set(float r, float g, float b, float a) { R = r; G = g; B = b; A = a; }
		void SetRGBVec(Vec3 v) { R = v.X; G = v.Y; B = v.Z; }
		void SetVec3(Vec3 v) { R = v.X; G = v.Y; B = v.Z; A = 1.0f; }
		void SetVec4(Vec4 v) { R = v.X; G = v.Y; B = v.Z; A = v.W; }

		//@}

		///@name Scale and bias
		//@{

		void ScaleRGB(float v)
		{
			R *= v;
			G *= v;
			B *= v;
		}
		void ScaleRGBA(float v)
		{
			R *= v;
			G *= v;
			B *= v;
			A *= v;
		}
		ColorRGBA RGBScaled(float v)
		{
			if (v == 1.0f)
			{
				return *this;
			}
			return ColorRGBA(R*v, G*v, B*v, A);
		}
		ColorRGBA RGBAScaled(float v)
		{
			if (v == 1.0f)
			{
				return *this;
			}
			return ColorRGBA(R*v, G*v, B*v, A*v);
		}

		void ScaleAndBiasVectorAsRGBColor(Vec3 v)
		{
			R = (v.X * 0.5f) + 0.5f;
			G = (v.Y * 0.5f) + 0.5f;
			B = (v.Z * 0.5f) + 0.5f;
			A = 1.0f;
		}

		//@}

		///@name Extacting and transforming values
		//@{

		//! Get the inverse of the color (RGB only are considered, keeping original alpha)
		ColorRGBA Inverse() const { return ColorRGBA(1.0f - R, 1.0f - G, 1.0f - B, A); }

		//! Get the color as RGB vector
		Vec3 GetRGB() const { return Vec3(R, G, B); }

		//! Return the maximum among R, G and B components
		float Value() const { return DsMax(R, G, B); }

		//! Return the grayscale level based on NTSC conversion
		float Gray() const { return R*0.299f + G*0.587f + B*0.114f; }

		//! Set the grayscale level based on NTSC conversion
		void Grayscale() { R = G = B = Gray(); }

		//! Set to the inverse of the color (RGB only are considered, keeping original alpha)
		ColorRGBA& Invert() { R = 1.0f - R; G = 1.0f - G; B = 1.0f - B; return *this; }

		//! Interpolate single R, G and B components, keeping original alpha
		ColorRGBA& InterpolateRGB(float t, const ColorRGBA& c1, const ColorRGBA& c2)
		{
			R = LerpValues(t, c1.R, c2.R);
			G = LerpValues(t, c1.G, c2.G);
			B = LerpValues(t, c1.B, c2.B);
			return *this;
		}

		//! Interpolate all the components, alpha included
		ColorRGBA& InterpolateRGBA(float t, const ColorRGBA& c1, const ColorRGBA& c2)
		{
			InterpolateRGB(t, c1, c2);
			A = LerpValues(t, c1.A, c2.A);
			return *this;
		}

		//@}

		///@name Operators
		//@{

		ColorRGBA& operator =(const ColorRGBA& c) { R = c.R; G = c.G; B = c.B; A = c.A; return *this; }
		ColorRGBA operator *(float f) const { return ColorRGBA(R*f, G*f, B*f, A*f); }
		ColorRGBA operator *(const ColorRGBA& c) const { return ColorRGBA(R*c.R, G*c.G, B*c.B, A*c.A); }
		ColorRGBA operator +(const ColorRGBA& c) const { return ColorRGBA(R + c.R, G + c.G, B + c.B, A + c.A); }
		ColorRGBA operator -(const ColorRGBA& c) const { return ColorRGBA(R - c.R, G - c.G, B - c.B, A - c.A); }

		ColorRGBA& operator *=(float f) { *this = *this*f; return *this; }
		ColorRGBA& operator *=(const ColorRGBA& c) { *this = *this*c; return *this; }
		ColorRGBA& operator +=(const ColorRGBA& c) { *this = *this + c; return *this; }
		ColorRGBA& operator -=(const ColorRGBA& c) { *this = *this - c; return *this; }

		bool operator ==(const ColorRGBA& c1) const { return this->R == c1.R && this->G == c1.G && this->B == c1.B && this->A == c1.A; }
		bool operator !=(const ColorRGBA& c1) const { return this->R != c1.R || this->G != c1.G || this->B != c1.B || this->A != c1.A; }

		operator float*() { return (float*)this; }
		operator const float*() const { return (const float*)this; }

		//@}

	};


	////****************************************************************************
	// @name Color functions
	////****************************************************************************
	//@{

	//! Get the hue given the red, green, blue components
	float RgbToHue(float r, float g, float b);

	/*!
	 Get hue, saturation, lightness components given the red, green, blue components.
	 Pass nullptr in h, s, l parameters to avoid their calculation
	*/
	void RgbToHsl(float r, float g, float b, float* h, float* s, float* l);

	/*!
	 Get red, green, blue components given the hue, saturation, lightness components.
	 Pass nullptr in r, g, b parameters to avoid their calculation
	*/
	void HslToRgb(float h, float s, float l, float* r, float* g, float* b);

	/*!
	 Get hue, saturation, value components given the red, green, blue components.
	 Pass nullptr in h, s, v parameters to avoid their calculation
	*/
	void RgbToHsv(float r, float g, float b, float* h, float* s, float* v);

	/*!
	 Get red, green, blue components given the hue, saturation, value components.
	 Pass nullptr in r, g, b parameters to avoid their calculation
	*/
	void HsvToRgb(float h, float s, float v, float* r, float* g, float* b);

	/*!
	 Get a black or white according to the color darkness (the most contrasting is selected)
	 @param c the given color that will be unchanged
	 @param alpha an optional alpha value that can be used to replace the alpha component of the given color (if <0 it is ignored)
	*/
	ColorRGBA GetColorAutoBW(const ColorRGBA& c, float alpha = -1.0f);

	/*!
	 Generate a different color according to the given identification number (the most contrasting colors are related to the lower numbers)
	 @param id_num numeric identifier used to generate the color
	*/
	ColorRGBA GenerateColorFromId(unsigned int id_num);

	//@}

} // namespace gpvulc

///@} (Material)

///@} (DS)

