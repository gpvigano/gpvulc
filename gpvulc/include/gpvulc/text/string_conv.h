//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Text utility - String-number conversion
/// @file string_conv.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>

namespace gpvulc
{

	/// @addtogroup Text
	/// @{

	//! Convert the given string to an integer, falling back to the given default value on error.
	inline int ToInt(const std::string& numString, int defaultValue = 0)
	{
		if (numString.empty())
		{
			return defaultValue;
		}
		int val;
		try
		{
			val = std::stoi(numString);
		}
		catch (...)
		{
			return defaultValue;
		}
		return val;
	}


	//! Convert the given string to a double, falling back to the given default value on error.
	inline double ToDouble(const std::string& numString, double defaultValue = 0.0)
	{
		if (numString.empty())
		{
			return defaultValue;
		}
		double val;
		try
		{
			val = std::stod(numString);
		}
		catch (...)
		{
			return defaultValue;
		}
		return val;
	}


	//! Convert a string to boolean.
	inline bool ToBool(const std::string& str)
	{
		return str == "true";
	}


	//! Convert a boolean to string.
	inline std::string ToString(bool val)
	{
		return val ? "true" : "false";
	}


	//! Convert the given integer number to a string
	inline std::string ToString(int n)
	{
		return std::to_string(n);
	}


	//! Convert the given floating point number to a string
	inline std::string ToString(float n)
	{
		return std::to_string(n);
	}


	//! Convert the given double floating number to a string
	inline std::string ToString(double n)
	{
		return std::to_string(n);
	}


	/*!
	Convert a number to a string.
	@param number Number to be converted.
	@param width Minimum width of the string representation (number of digits for integer values).
	@param precision Number of digits after the dot (decimal digits).
	@param zeroes Fill the empty spaces with zeroes (if the width is set)
	@return the string representation of the given number.
	@note This function must be used only with numeric types (int, float, double, ...).
	*/
	template <typename T>
	inline std::string NumberToString(T number, int width, int precision, bool zeroes)
	{
		std::ostringstream ss;
		if (zeroes) ss.fill('0');
		if (width > 0) ss.width(width);
		if (precision >= 0)
		{
			ss << std::fixed;
			ss.precision(precision);
		}
		ss << number;
		return ss.str();
	}

	///@}

}//namespace gpvulc



