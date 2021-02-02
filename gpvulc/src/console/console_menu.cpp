//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Console menu: display a list of items and get user choice.


#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>

namespace gpvulc
{

	int GetChoice(const std::string& inStr, int maxValidChoice)
	{
		int choice = 0;
		if (!inStr.empty())
		{
			choice = std::atoi(inStr.c_str());
			if (maxValidChoice > 0 && choice > maxValidChoice)
			{
				choice = 0;
			}
		}
		return choice;
	}


	int GetMenuChoice(std::vector<std::string> choices, const std::string& defaultChoice)
	{
		int choice = 0;
		if (choices.empty())
		{
			return 0;
		}
		ClearConsole();
		for (size_t i = 0; i < choices.size(); i++)
		{
			std::cout << i + 1 << ". " << choices[i] << "\n";
		}
		if (!defaultChoice.empty())
		{
			std::cout
				<< "(ENTER = " << defaultChoice << ")\n"
				<< std::endl;
		}

		std::cout << "Enter choice number then press ENTER: ";

		std::string inStr;
		std::getline(std::cin, inStr);
		choice = GetChoice(inStr, (int)choices.size());
		std::cout << std::endl;
		ClearConsole();
		return choice;
	}

}
