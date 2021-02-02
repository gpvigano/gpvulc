//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Console menu: display a list of items and get user choice.
/// @file console_menu.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>
#include <vector>

namespace gpvulc
{

	/// @addtogroup Console
	/// @{

	/*! Get a choice number from a string.
	@param inStr Input string containing the choice number.
	@param maxValidChoice If not zero check if the input choice is not greater than this, else return 0.
	@return the choice number or 0 if not a valid choice.
	*/
	int GetChoice(const std::string& inStr, int maxValidChoice = 0);

	/*!
	Display a list of choices on the console and wait for a choice from standard input.
	@param choices Descriptions of choices
	@param defaultChoice Optional description of a default choice (empty input), ignored if empty.
	@return the entered choice or 0 (both on invalid choice or for default choice).
	*/
	int GetMenuChoice(std::vector<std::string> choices, const std::string& defaultChoice = "");

	/// @}
}