//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// PathInfo test program

#include <iostream>
#include <fstream>
#include <stdlib.h>

#define SHOW_ALL_REMARKS
#include <gpvulc/path/PathInfo.h>

using namespace gpvulc;

#include <gtest/gtest.h>

// path construnctors and copy test
TEST(PathInfoTest, ConstructorNCopy)
{
	gpvulc::PathInfo path1("D:\\DEV\\gpvulc\\name.ext");
	path1.SetBackSlash(false);
	EXPECT_EQ(path1.GetFullPath(), "D:/DEV/gpvulc/name.ext");
	gpvulc::PathInfo path2(path1);
	EXPECT_TRUE(path1.GetFullPath() == path2.GetFullPath());
	EXPECT_TRUE(path1 == path2);
	EXPECT_TRUE(path1 == path2.GetFullPath());

	// use UNIX-like paths (less to write...)
	gpvulc::PathInfo::SetDefaultBackSlash(false);
	gpvulc::PathInfo path3("D:/DEV/gpvulc","name","ext");
	EXPECT_EQ(path1.GetFullPath(), path3.GetFullPath());
	gpvulc::PathInfo path4("D:\\DEV/gpvulc/","name","ext");
	EXPECT_EQ(path1.GetFullPath(), path4.GetFullPath());
}


// path decomposition test
TEST(PathInfoTest, Decomposition)
{
	// use UNIX-like paths (less to write...)
	gpvulc::PathInfo::SetDefaultBackSlash(false);
	gpvulc::PathInfo path1("D:\\DEV\\test\\projects\\vc10\\../../bin/name.exe");
	EXPECT_EQ(path1.GetPath(), "D:/DEV/test/bin/");
	EXPECT_EQ(path1.GetDevice(), "D:");
	EXPECT_EQ(path1.GetRootPath(), "/DEV/test/bin/");
	EXPECT_EQ(path1.GetDirName(), "bin");
	EXPECT_EQ(path1.GetFullName(), "name.exe");
	EXPECT_EQ(path1.GetName(), "name");
	EXPECT_EQ(path1.GetExt(), "exe");
	EXPECT_TRUE(path1.UpperPath());
	EXPECT_EQ(path1.GetPath(), "D:/DEV/test/");
	path1.SetBackSlash(true);
	EXPECT_EQ(path1.GetPath(), "D:\\DEV\\test\\");
	EXPECT_TRUE(path1 == "D:/DEV/test/name.exe");
	path1.SetBackSlash(false);
	EXPECT_EQ(path1.GetPath(), "D:/DEV/test/");
}


// path setting test
TEST(PathInfoTest, Set)
{
	// use UNIX-like paths (less to write...)
	gpvulc::PathInfo::SetDefaultBackSlash(false);
	gpvulc::PathInfo path1;
	EXPECT_FALSE(path1.Defined());
	path1.SetPath("/DEV/test/");
	EXPECT_TRUE(path1.Defined());
	path1.SetDevice("D:");
	path1.SetFullName("name.ext");
	EXPECT_TRUE(path1.GetFullPath() == "D:/DEV/test/name.ext");
	EXPECT_TRUE(path1.Defined());
	path1.SetName("new_name");
	path1.SetExt("new_ext");
	path1.SetDevice("C:");
	EXPECT_TRUE(path1 == "C:\\DEV\\test\\new_name.new_ext");
	path1.SetFullPath("/DEV/test/test.txt");
	EXPECT_TRUE(path1 == "/DEV/test/test.txt");
	EXPECT_EQ(path1.GetRootPath(), "/DEV/test/");
	EXPECT_EQ(path1.GetDirName(), "test");
	path1.Reset();
	EXPECT_FALSE(path1.Defined());
}


// Test directory separator management 
TEST(PathInfoTest, DirSep)
{
	gpvulc::PathInfo path1;
	path1.SetBackSlash(true);
	EXPECT_TRUE(path1.BackSlash());
	EXPECT_EQ(path1.GetDirSep(), '\\');
	path1.SetBackSlash(false);
	EXPECT_FALSE(path1.BackSlash());
	EXPECT_EQ(path1.GetDirSep(), '/');
	std::string s = "\\test/dir1/dir2\\test.ext";
	path1.FixDirSep(s);
	EXPECT_TRUE(s == "/test/dir1/dir2/test.ext");
}


// Test assignment and comparison operators
TEST(PathInfoTest, Operators)
{
	gpvulc::PathInfo path1;
	path1 = "C:/test/dir/file.ext";
	EXPECT_TRUE(path1 == "C:/test/dir/file.ext");
	EXPECT_TRUE(path1 == "C:\\test\\dir\\file.ext");
	EXPECT_FALSE(path1 != "C:/test/dir/file.ext");
	gpvulc::PathInfo path2 = path1;
	EXPECT_TRUE(path1 == path2);
	EXPECT_FALSE(path1 != path2);
	gpvulc::PathInfo path3;
	path3 = path1;
	EXPECT_TRUE(path1 == path3);
}


// Test absolute/relative path conversion & joining
TEST(PathInfoTest, AbsRelPath)
{
	// use UNIX-like paths (less to write...)
	gpvulc::PathInfo::SetDefaultBackSlash(false);

	gpvulc::PathInfo path1("D:\\DEV\\gpvulc\\test\\");
	EXPECT_TRUE(path1.SetRelativeToPath("D:/DEV/gpvulc/doc/"));
	EXPECT_EQ(path1.GetPath(), "../test/");
	gpvulc::PathInfo curr_path(gpvulc::GetCurrPath());
	gpvulc::PathInfo test(curr_path);
	EXPECT_TRUE(test.JoinPath(path1.GetPath()));
	//std::cout << "curr path: "<< gpvulc::GetCurrPath() << std::endl;
	//std::cout << "test path: "<< test.GetPath() << std::endl;
	EXPECT_TRUE(path1.ChangeToAbsPathFromCurrDir());
	EXPECT_EQ(path1.GetPath(), test.GetPath());
	EXPECT_TRUE(path1.ChangeToRelPathFromCurrDir());
	EXPECT_EQ(path1.GetPath(), "../test/");
}

// SetRelativeToPath test
TEST(PathInfoTest, SetRelativeToPath)
{
	// use Windows-like paths (different test...)
	gpvulc::PathInfo::SetDefaultBackSlash(true);

	gpvulc::PathInfo path1("C:\\TEMP\\dir\\");
	gpvulc::PathInfo path2("D:\\TEMP\\");
	EXPECT_FALSE(path1.SetRelativeToPath(path2.GetPath()));
	EXPECT_EQ(path1.GetPath(), "C:\\TEMP\\dir\\");
	EXPECT_TRUE(path1.SetRelativeToPath("C:\\TEMP\\"));
	EXPECT_EQ(path1.GetPath(), "dir\\");
}


// Tests UpperPath
TEST(PathInfoTest, Upper)
{
	gpvulc::PathInfo::SetDefaultBackSlash(false);

	gpvulc::PathInfo path1("C:/TEMP/dir/test.txt");
	EXPECT_TRUE(path1.UpperPath());
	EXPECT_EQ(path1.GetDirName(), "TEMP");
	EXPECT_TRUE(path1.UpperPath());
	EXPECT_EQ(path1.GetDirName(), "");
	EXPECT_FALSE(path1.UpperPath());
}


// Tests pattern matching
TEST(PathInfoTest, PatternMatching)
{
	gpvulc::PathInfo path1("C:/TEMP/dir/test.txt");
	EXPECT_TRUE(path1.MatchesPattern(path1.GetFullPath()));
	EXPECT_TRUE(path1.MatchesPattern(path1.GetFullName()));
	EXPECT_TRUE(path1.MatchesPattern("*.txt"));
	EXPECT_TRUE(path1.MatchesPattern("C:/TEMP/dir/*.txt"));
	EXPECT_TRUE(path1.MatchesPattern("C:/TEMP/dir/*.*"));
	EXPECT_TRUE(path1.MatchesPattern("t*.*"));
	EXPECT_TRUE(path1.MatchesPattern("t???.*"));
	EXPECT_FALSE(path1.MatchesPattern("C:/*.*"));

	EXPECT_TRUE(path1.MatchesPatterns({ "*.txt" }));
	EXPECT_TRUE(path1.MatchesPatterns({ "*.txt", "*.log" }));
	EXPECT_TRUE(path1.MatchesPatterns({ "*.*" }));
	EXPECT_TRUE(path1.MatchesPatterns({}));
	EXPECT_FALSE(path1.MatchesPatterns({ "*.h", "*.cpp" }));
}


// // Tests other methods
//TEST(PathInfoTest, Other)
//{
//	// to be implemented...
//}
