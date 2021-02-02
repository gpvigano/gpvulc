//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <gtest/gtest.h>

#include <gpvulc/console/console_util.h>

int main(int argc, char* argv[])
{
	// using Google Tests, see:
	// https://github.com/google/googletest/blob/master/googletest/docs/primer.md

	::testing::InitGoogleTest(&argc, argv);

	int result = RUN_ALL_TESTS();
	gpvulc::ConsolePause();

	return result;
}

