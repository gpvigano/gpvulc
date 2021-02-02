//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Console utility functions
/// @file console_util.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <iostream>
#include <stdlib.h>

namespace gpvulc
{

	/// @defgroup Console Console
	/// @brief Utility functions for console-based applications.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup Console
	/// @{

	/*!
	Wait until ENTER key is pressed.
	*/
	inline void WaitForEnter()
	{
		std::cout << "Press ENTER to continue..." << std::endl;
		std::cin.get();
	}


	/*!
	Wait until a key is pressed (ENTER if not Windows).
	*/
	inline void ConsolePause()
	{
#ifdef _WIN32
		system("pause");
#else
		WaitForEnter();
#endif
	}


	/*!
	Clear the console.
	*/
	inline void ClearConsole()
	{
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif
	}

	/// @}

}

