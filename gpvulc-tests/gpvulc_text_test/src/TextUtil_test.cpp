//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// TextBuffer_test.cpp

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gpvulc/text/text_util.h>

using namespace gpvulc;

#include <gtest/gtest.h>

// Tests utility functions
TEST(TextUtilTest, UtilFunc)
{
	EXPECT_EQ(gpvulc::GetLowerStr("AbcD 123_."), "abcd 123_.");
	EXPECT_EQ(gpvulc::GetUpperStr("AbcD 123_."), "ABCD 123_.");
	std::string orig_str =
		"first row\n"
		"second row\n";
	std::string indented_str =
		"--first row\n"
		"--second row\n";
	std::string str = orig_str;
	gpvulc::IndentStr(str, 2, '-');
	EXPECT_EQ(str, indented_str);
	gpvulc::UnindentStr(str, 2, '-');
	EXPECT_EQ(str, orig_str);
	EXPECT_EQ(GetCidStr("@,abc 123 \t"), "__abc_123__");
	SaveText("gpvulc_SaveText.txt", orig_str);
	std::string loadedStr;
	LoadText("gpvulc_SaveText.txt", loadedStr);
	EXPECT_EQ(loadedStr, orig_str);
}
