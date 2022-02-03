//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Basic material data


#include <gpvulc/ds/MtlData.h>
#include <gpvulc/text/TextParser.h>
#include <sstream>

namespace gpvulc
{

	//******************************************************************************
	// Color functions
	//******************************************************************************

	float RgbToHue(float r, float g, float b)
	{
		float max = DsMax(r, g, b);
		float min = DsMin(r, g, b);
		float range = max - min;
		float base = 60.0f / range;
		float h = 0.0f;
		if (max == min) h = 0;
		else if (max == r && g >= b) h = base*(g - b);
		else if (max == r && g < b) h = base*(g - b) + 360.0f;
		else if (max == g) h = base*(b - r) + 120.0f;
		else if (max == b) h = base*(r - g) + 240.0f;
		return h;
	}


	void RgbToHsl(float r, float g, float b, float* h, float* s, float* l)
	{
		float max = DsMax(r, g, b);
		float min = DsMin(r, g, b);
		float saturation = 0.0f, lightness = 0.0f;

		lightness = 0.5f*(max + min);
		if (s)
		{
			if (max == min) saturation = 0;
			else if (lightness <= 0.5f) saturation = (max - min) / (max + min);
			else if (lightness > 0.5f) saturation = (max - min) / (2.0f - (max + min));
			*s = saturation;
		}

		if (h) *h = RgbToHue(r, g, b);
		if (l) *l = lightness;
	}


	void HslToRgb(float h, float s, float l, float* r, float* g, float* b)
	{

		// http://en.wikipedia.org/wiki/HSL_and_HSV

		float c = (1.0f - fabs(2.0f*l - 1.0f))*s;
		float h1 = h / 60.0f;
		float x = c*(1.0f - fabs(fmod(h1, 2.0f) - 1.0f));
		float r1(0.0f), g1(0.0f), b1(0.0f);

		if (h1 < 0.0f || h1 < 60.0f)
		{
			// r1=g1=b1= 0.0f;
		}
		else if (0.0f <= h1 && h1 < 1.0f)
		{
			r1 = c;
			g1 = x;
			//b1 = 0;
		}
		else if (h1 < 2.0f)
		{
			r1 = x;
			g1 = c;
			//b1 = 0;
		}
		else if (h1 < 3.0f)
		{
			//r1 = 0;
			g1 = c;
			b1 = x;
		}
		else if (h1 < 4.0f)
		{
			//r1 = 0;
			g1 = x;
			b1 = c;
		}
		else if (h1 < 5.0f)
		{
			r1 = x;
			//g1 = 0;
			b1 = c;
		}
		else
		{// if( h1<6.0f )
			r1 = c;
			//g1 = 0;
			b1 = x;
		}

		float m = l - 0.5f*c;

		if (r) *r = r1 + m;
		if (g) *g = g1 + m;
		if (b) *b = b1 + m;
	}


	void RgbToHsv(float r, float g, float b, float* h, float* s, float* v)
	{
		float M = DsMax(r, g, b);
		float m = DsMin(r, g, b);
		float saturation;

		if (M == 0) saturation = 0;
		else saturation = (M - m) / M;
		if (h) *h = RgbToHue(r, g, b);
		if (s) *s = saturation;
		if (v) *v = M;
	}


	void HsvToRgb(float h, float s, float v, float* r, float* g, float* b)
	{

		// http://en.wikipedia.org/wiki/HSL_and_HSV

		float c = v*s;
		float h1 = h / 60.0f;
		float x = c*(1.0f - fabs(fmod(h1, 2.0f) - 1.0f));
		float r1(0.0f), g1(0.0f), b1(0.0f);

		if (h1 < 0.0f || h1 < 60.0f)
		{
			// r1=g1=b1= 0.0f;
		}
		else if (0.0f <= h1 && h1 < 1.0f)
		{
			r1 = c;
			g1 = x;
			//b1 = 0;
		}
		else if (h1 < 2.0f)
		{
			r1 = x;
			g1 = c;
			//b1 = 0;
		}
		else if (h1 < 3.0f)
		{
			//r1 = 0;
			g1 = c;
			b1 = x;
		}
		else if (h1 < 4.0f)
		{
			//r1 = 0;
			g1 = x;
			b1 = c;
		}
		else if (h1 < 5.0f)
		{
			r1 = x;
			//g1 = 0;
			b1 = c;
		}
		else
		{// if( h1<6.0f )
			r1 = c;
			//g1 = 0;
			b1 = x;
		}

		float m = v - c;

		if (r) *r = r1 + m;
		if (g) *g = g1 + m;
		if (b) *b = b1 + m;
	}


	ColorRGBA GetColorAutoBW(const ColorRGBA& c, float alpha)
	{

		float l;
		l = (c.R*0.9f + c.G*1.5f + c.B*0.6f) / 3.0f;
		if (alpha < 0.0f) alpha = c.A;
		float bw = l < 0.5f ? 1.0f : 0.0f;
		return ColorRGBA(bw, bw, bw, alpha);
	}


	ColorRGBA GenerateColorFromId(unsigned int id_num)
	{
		unsigned char id = id_num + 1;
		float r = 0.0f, g = 0.0f, b = 0.0f;
		if (id & 0x1) b = 1.0f;
		if (id & 0x8) b = id & 0x1 ? 0.25f : 0.5f;
		if (id & 0x2) g = 1.0f;
		if (id & 0x10) g = id & 0x2 ? 0.25f : 0.5f;
		if (id & 0x4) r = 1.0f;
		if (id & 0x20) r = id & 0x4 ? 0.25f : 0.5f;

		if (id & 0x40) r *= 0.5f;
		if (id & 0x80) g *= 0.5f;

		return ColorRGBA(r, g, b, 1.0f);
	}



	//------------------------------------------------------------------------------
	// Conversion from/to string
	//------------------------------------------------------------------------------


	bool FromString(const std::string&s, ColorRGBA& val)
	{
		if (s.empty())
		{
			return false;
		}
		TextParser p(s);
		if (p.GetToken(" ,")) val.R = (float)std::atof(p.Result().c_str());
		if (p.GetToken(" ,")) val.G = (float)std::atof(p.Result().c_str());
		if (p.GetToken(" ,")) val.B = (float)std::atof(p.Result().c_str());
		if (p.GetToken(" ,")) val.A = (float)std::atof(p.Result().c_str());
		return true;
	}

	std::string ToString(ColorRGBA val)
	{
		std::ostringstream oStr;
		oStr << val.R << ",  " << val.G << ",  " << val.B << ",  " << val.A;
		return oStr.str();
	}

} // namespace gpvulc
