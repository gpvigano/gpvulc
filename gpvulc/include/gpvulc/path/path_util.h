//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief File Path Utility
/// @file path_util.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>

namespace gpvulc
{
	/// @defgroup Path  Path
	/// @brief  File path manipulation library.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup Path
	///@{ 

	//! Check if the colon character is present
	inline bool HasColon(const std::string& p)
	{
		return p.find(':') != std::string::npos;
	}

	//! Check if the given character is adirectory separator
	inline bool IsDirSep(const char& c)
	{
		return c == '\\' || c == '/';
	}

	//! Get the current working directory
	std::string GetCurrPathStr();

	///@}

}//namespace gpvulc

