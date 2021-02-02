//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief File path utility
/// @file path_util.cpp
/// @author Giovanni Paolo Vigano'

#include <gpvulc/path/path_util.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

#ifdef _MSC_VER

// Those functions are BSD/UNIX specific functions that have no equivalent in Visual C++.
// To compile in Visual C++, let's include direct.h and define some alias.
#include <direct.h>
//#define chdir _chdir
#define getcwd _getcwd

#else
#include <unistd.h>
#include <dirent.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 2048
#endif


std::string gpvulc::GetCurrPathStr()
{
	char path[PATH_MAX + 1];
	if (getcwd(path, PATH_MAX))
	{
		std::string path_str(path);
		return path_str;
	}
	return "";
}
