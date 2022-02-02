//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// File system library testing application

#include <gpvulc/fs/FileUtil.h>
#include <gpvulc/console/console_util.h>
#include <iostream>

using namespace gpvulc;

// ****************************************************************************
// main

int main(int argc, char* argv[])
{

	// Change the current working directory to match the executable file path
	ChangeCurrDirToFilePath(argv[0]);

	PathInfo path("..");
	DirObject dir(path.GetPath());
	dir.ReadTree();
	std::cout << dir.TreeToString(true) << std::endl;

	ConsolePause();

	return EXIT_SUCCESS;
}




