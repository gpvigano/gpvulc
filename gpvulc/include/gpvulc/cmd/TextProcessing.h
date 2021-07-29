//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @defgroup Cmd Cmd
/// @brief Toolkit for speed up the creation of command line based utilities.
/// @author Giovanni Paolo Vigano'

/// @brief Text Processing Utility Toolkit
/// @file TextProcessing.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>

#include <vector>
#include <functional>

namespace gpvulc
{
	/// @defgroup Cmd Command line
	/// @brief Command line utility
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup Cmd
	/// @{

	/*!
	Tool implementation for text converters.
	@param argc First parameter of the main() function (command argument counter).
	@param argv First parameter of the main() function (command arguments list).
	@param textConverterFunc User function that processes each file.
	@param fileFilters Optional parameter to specify a list of filters to select the files to be processed (default: empty vector, process all files).
	@param disableConsolePause Optional flag to disable the pause before exiting (default: enabled).
	@return EXIT_SUCCESS (in any case).
	@note The user function accepts two strings as parameters,
	the first one is the original text to be processed,
	the second one is the modified text. The function must return the number of changes.
	If no parameter is passed to the command line, an usage hint is printed to console.
	@details After an executable is built you can run it from the console,
	passing files and folders as command line parameters, or you can drag&drop files
	onto the executable icon to process them with your utility.
	Sample implementation: @code
	#include <gpvulc/cmd/TextProcessing.h>
	#include <gpvulc/console/console_util.h>

	unsigned ConvertMyTextFile(const std::string& srcText, std::string& outSrcText)
	{
		unsigned changesCount = 0U;
		//...
		// for each change: changesCount++;
		return changesCount;
	}

	int main(int argc, char* argv[])
	{
		return ConvertTextFiles(argc, argv, ConvertMyTextFile, { "*.txt", "*.log" });
		// or simply:
		// return ConvertTextFiles(argc, argv, ConvertMyTextFile);
		// to process all files.
	}
	@endcode
	*/
	int ConvertTextFiles(
		int argc,
		char* argv[],
		std::function<unsigned(const std::string& srcText, std::string& outSrcText)> textConverterFunc,
		const std::vector<std::string>& fileFilters = {},
		bool disableConsolePause = false
		);
		
	///@}

}//namespace gpvulc



