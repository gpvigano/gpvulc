//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// TextBuffer test program

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gpvulc/text/TextBuffer.h>

using namespace gpvulc;

#include <gtest/gtest.h>


// Tests operator[]()
TEST(TextBufferTest, ConstructorsSet)
{
	EXPECT_EQ(TextBuffer(), "");
	EXPECT_EQ(TextBuffer(3, true, 'a'), "aaa");
	EXPECT_EQ(TextBuffer(3, false).GetSize(), 3);
	EXPECT_EQ(TextBuffer(123), "123");
	EXPECT_EQ(TextBuffer(123L), "123");
	EXPECT_EQ(TextBuffer(1.23f), std::string("1.23"));
	EXPECT_EQ(TextBuffer(1.23), "1.23");
	EXPECT_EQ(TextBuffer('a'), "a");
	EXPECT_EQ(TextBuffer("abc"), "abc");
	EXPECT_EQ(TextBuffer("abcde", 3), "abc");
	EXPECT_EQ(TextBuffer(TextBuffer("abc")), "abc");
#ifdef _WIN64
	EXPECT_EQ(TextBuffer((void*)(0xAABBCCDDULL)), "00000000AABBCCDD");
	EXPECT_EQ(TextBuffer((const void*)(0xAABBCCDDULL)), "00000000AABBCCDD");
#else
	EXPECT_EQ(TextBuffer((void*)(0xAABBCCDD)), "AABBCCDD");
	EXPECT_EQ(TextBuffer((const void*)(0xAABBCCDD)), "AABBCCDD");
#endif // _WIN64

}

// Tests TextBuffer Fill.
TEST(TextBufferTest, Fill)
{
	TextBuffer txt("abcdef");
	txt.Fill(1, 2, '_');
	EXPECT_EQ(txt, "a__def");
	txt.Fill(4, -1, '0');
	EXPECT_EQ(txt, "a__d00");
}


// Tests TextBuffer concatenation.
TEST(TextBufferTest, Cat)
{
	TextBuffer txt;
	EXPECT_EQ(txt.Cat(TextBuffer("A")), "A");
	EXPECT_EQ(txt.Cat("BCDE", 2), "ABC");
	EXPECT_EQ(txt.Cat('F', 2), "ABCFF");
	EXPECT_EQ(txt.Cat('G'), "ABCFFG");
	EXPECT_EQ(txt.Cat(std::string("aa")), "ABCFFGaa");
	txt.Clear();
#ifdef _WIN64
	EXPECT_EQ(txt.Cat((void*)0xAABBCCDDULL), "00000000AABBCCDD");
#else
	EXPECT_EQ(txt.Cat((void*)0xAABBCCDD), "AABBCCDD");
#endif // _WIN64
	txt.Clear();
	EXPECT_EQ(txt.Cat(1, 2, true), "01");
	EXPECT_EQ(txt.Cat((short)2, 2, false), "01 2");
	EXPECT_EQ(txt.Cat(3U), "01 23");
	txt.Clear();
	EXPECT_EQ(txt.Cat(-0.045678f, 7, 3, false), " -0.046");
	txt.Clear();
	EXPECT_EQ(txt.Cat(-45.67, 8, 4, true), "-45.6700");
}

// Tests TextBuffer streaming.
TEST(TextBufferTest, streaming)
{
	std::ofstream ofs("test.txt");
	ofs << "\n\
int main( int argc, char* argv[] )\n\
{\n\
  return 0;\n\
}\n\n" << std::ends;
	ofs.close();

	TextBuffer text;
	std::ifstream testfile("test.txt");
	if (testfile.good())
	{
		testfile >> text;
		testfile.close();
	}
	bool success = false;
	testfile.open("test.txt");
	if (testfile.good())
	{
		TextBuffer check;
		testfile >> check;
		testfile.close();
		success = (check == text);
	}

	EXPECT_TRUE(success);
}

// Tests operator+() (and constructors and Set() methods).
TEST(TextBufferTest, PlusAndConstructors)
{
	EXPECT_EQ(TextBuffer("a") + TextBuffer("b"), "ab");
	EXPECT_EQ(TextBuffer("a") + std::string("b"), "ab");
	EXPECT_EQ(TextBuffer("a") + "b", "ab");
	EXPECT_EQ(TextBuffer("a") + 1, "a1");
	EXPECT_EQ(1.2 + TextBuffer("a"), "1.2a");
	EXPECT_EQ(3.4f + TextBuffer("a"), "3.4a");
#ifdef _WIN64
	EXPECT_EQ((void*)(0x052ED40A) + TextBuffer("b"), "00000000052ED40Ab");
#else
	EXPECT_EQ((void*)(0x052ED40A) + TextBuffer("b"), "052ED40Ab");
#endif // _WIN64
}

// Tests operator+=() (and constructors and Set() methods).
TEST(TextBufferTest, PlusEqAndConstrCat)
{
	TextBuffer txt("A");
	EXPECT_EQ(txt += TextBuffer("B"), "AB");
	EXPECT_EQ(txt += "C", "ABC");
	EXPECT_EQ(txt += std::string("D"), "ABCD");
	EXPECT_EQ(txt += 1, "ABCD1");
	EXPECT_EQ(txt += 2.3, "ABCD12.3");
}

// Tests comparison operators and methods.
TEST(TextBufferTest, CompareEqual)
{
	TextBuffer txt("Abc");
	EXPECT_TRUE(txt.EqualTo("Abc", false));
	EXPECT_TRUE(txt.EqualTo("abc", true));
	EXPECT_FALSE(txt.EqualTo("aaa"));
	EXPECT_EQ(TextBuffer().Compare("a"), -1);
	EXPECT_EQ(txt.Compare(""), 1);
	EXPECT_EQ(txt.Compare((const char*)NULL), 1);
	EXPECT_EQ(txt.Compare("Abc", false), 0);
	EXPECT_EQ(txt.Compare("abC", true), 0);
	EXPECT_EQ(txt.Compare("bc"), -1);
	EXPECT_EQ(txt.Compare("Aaa"), 1);
	EXPECT_EQ(txt.Compare("aaa"), -1);
	EXPECT_TRUE(TextBuffer() == (const char*)NULL);
	EXPECT_TRUE(txt == "Abc");
	EXPECT_FALSE(txt == "abc");
	EXPECT_FALSE(txt == "aaa");
	EXPECT_TRUE(txt == std::string("Abc"));
	EXPECT_FALSE(txt == std::string("abc"));
	EXPECT_FALSE(txt == std::string("aaa"));
	EXPECT_TRUE(txt > "A");
	EXPECT_TRUE(txt < "B");
	EXPECT_TRUE(txt < "a");
	EXPECT_TRUE(txt > (const char*)NULL);
	EXPECT_FALSE(txt<(const char*)NULL);
	EXPECT_FALSE(TextBuffer()>(const char*)NULL);
	EXPECT_FALSE(TextBuffer() < (const char*)NULL);
}

// Tests conversion to number
TEST(TextBufferTest, GetNum)
{
	EXPECT_EQ(TextBuffer("1.0").GetDouble(), 1.0);
	EXPECT_EQ(TextBuffer("aaa").GetDouble(-1.0), -1.0);
	EXPECT_EQ(TextBuffer("1.0").GetFloat(), 1.0f);
	EXPECT_EQ(TextBuffer("bbb").GetFloat(-3.0f), -3.0f);
	EXPECT_EQ(TextBuffer("1.0").GetInt(), 1);
	EXPECT_EQ(TextBuffer("-2").GetInt(0), -2);
	EXPECT_EQ(TextBuffer("FF").GetInt(0), 0);
	EXPECT_EQ(TextBuffer("FF").GetHex(0), 255);
}

// Tests operator[]()
TEST(TextBufferTest, Indexing)
{
	TextBuffer txt("ABCDEF");
	EXPECT_EQ(txt[(int)0], 'A');
	EXPECT_EQ(txt[(size_t)1], 'B');
	EXPECT_EQ(txt[5], 'F');
}

// Tests character check
TEST(TextBufferTest, CharCheck)
{
	//              01234567 8
	TextBuffer txt("A_1.2 3\t\n");
	EXPECT_TRUE(txt.IsAlNum(0));
	EXPECT_TRUE(txt.IsAlNum_(1));
	EXPECT_FALSE(txt.IsAlNum(1));
	EXPECT_TRUE(txt.IsAlNum_(1));
	EXPECT_FALSE(txt.IsAlNum(3));
	EXPECT_FALSE(txt.IsAlNum_(3));
	EXPECT_FALSE(txt.IsSpace(3));
	EXPECT_TRUE(txt.IsSpace(5));
	EXPECT_TRUE(txt.IsSpace(7));
	EXPECT_TRUE(txt.IsSpace(8));
}


// Tests cast operators
TEST(TextBufferTest, StdStringCast)
{
	TextBuffer txt("ABC");
	std::string s = txt;
	EXPECT_EQ(txt, "ABC");
	const std::string& cs = txt;
	EXPECT_EQ(cs, "ABC");
	EXPECT_STREQ(txt.Get(), "ABC");
	EXPECT_TRUE(TextBuffer().Ptr() == NULL);
}


// Tests getting string info
TEST(TextBufferTest, GetInfo)
{
	EXPECT_EQ(TextBuffer().Length(), 0);
	EXPECT_EQ(TextBuffer("AAA").Length(), 3);
	EXPECT_EQ(TextBuffer("AAA\0").Length(), 3);
	TextBuffer lines =
		"line1\n"
		"line2\n"
		"line3\n"
		;
	EXPECT_EQ(lines.CountLines(), 3);
	lines += "AAA";
	EXPECT_EQ(lines.CountLines(), 4);
	EXPECT_FALSE(lines.IsEmpty());
	lines.Clear();
	EXPECT_TRUE(lines.IsEmpty());
}


// Tests string manipulation
TEST(TextBufferTest, Manipulation)
{
	EXPECT_EQ(TextBuffer(" Abc\t ").Crop(), "Abc");
	EXPECT_EQ(TextBuffer(". Abc\t .").Crop(". "), "Abc\t");
	EXPECT_EQ(TextBuffer("..Abc,,").Crop(".", ",c"), "Ab");

	EXPECT_EQ(TextBuffer("Abcdef").Erase(2, 4), "Abf");
	EXPECT_EQ(TextBuffer("Abcdef").Erase(4, 2), "Abf");
	EXPECT_EQ(TextBuffer("Abcdef").Erase(3), "Abc");

	EXPECT_EQ(TextBuffer("Abcdef").SubString(3), "def");
	EXPECT_EQ(TextBuffer("Abcdef").SubString(2, 4), "cde");

	EXPECT_EQ(TextBuffer("Abcdef").SubString(1, 3), "bcd");

	EXPECT_EQ(TextBuffer("Abcdef").ClipChars(1, 3), "bc");

	EXPECT_EQ(TextBuffer("Abc").Insert(1, "123"), "A123bc");

	EXPECT_EQ(TextBuffer("Abc").InsertChar(1, ':'), "A:bc");
	EXPECT_EQ(TextBuffer("Abc").InsertChar(1, ':', 2), "A::bc");

	// test robustness
#ifdef _WIN64
	EXPECT_EQ(TextBuffer((const void*)nullptr), "0000000000000000");
#else
	EXPECT_EQ(TextBuffer((const void*)nullptr), "00000000");
#endif // _WIN64
	EXPECT_EQ(TextBuffer((const char*)nullptr), "");
	EXPECT_EQ(TextBuffer(NULL), "0");//,"");
	//EXPECT_NO_THROW(TextBuffer("Abc").Crop(NULL));
	EXPECT_EQ(TextBuffer("Abc").Crop(""), "Abc");
	EXPECT_EQ(TextBuffer("Abc").SubString(-1, -1), "Abc");
	EXPECT_EQ(TextBuffer("Abc").Erase(-1), "Abc");
	EXPECT_EQ(TextBuffer("Abc").Insert(-1, "123"), "Abc");
	EXPECT_EQ(TextBuffer("Abc").Insert(10, "123"), "Abc");
	EXPECT_EQ(TextBuffer("Abc").InsertChar(-10, ':'), "Abc");
	EXPECT_EQ(TextBuffer("Abc").InsertChar(10, ':'), "Abc");
	EXPECT_EQ(TextBuffer("Abc").InsertChar(10, ':' - 1), "Abc");

	// ReplaceAt()
	EXPECT_EQ(TextBuffer("abc123").ReplaceAt(1, "ZZ"), "aZZ123");
}


// Tests string manipulation
TEST(TextBufferTest, FindAndReplace)
{
	TextBuffer test;

	// GetSubString()
	EXPECT_EQ(TextBuffer("Abc").GetSubString(1, 1), "b");

	// FindSubString()
	test = "abcdeABCDEabcde";
	EXPECT_EQ(test.FindSubString("bCD", true), 1);
	EXPECT_EQ(TextBuffer("This is").FindSubString("is", false, true), 5);

	// FindRevSubString()
	EXPECT_EQ(test.FindRevSubString("abc"), 10);
	EXPECT_EQ(TextBuffer("This is. Is this?").FindRevSubString("is", true, true), 9);

	// FindSubStringAny()
	test = "abcdeABCDEabcde";
	EXPECT_EQ(test.FindSubStringAny({ "AB","cd" }, false), 2);
	EXPECT_EQ(TextBuffer("This is").FindSubStringAny({ "is","This" }, false, true), 0);

	// FindRevSubString()
	EXPECT_EQ(test.FindRevSubStringAny({ "ABC","abc" }), 10);
	EXPECT_EQ(TextBuffer("This is. Is this?").FindRevSubStringAny({ "is","this" }, true, true), 12);

	// FindFirstOf()
	EXPECT_EQ(TextBuffer("abcabc").FindFirstOf("bc"), 1);

	// FindLastOf()
	EXPECT_EQ(TextBuffer("abcabc").FindLastOf("ab"), 4);

	// FindChar()
	EXPECT_EQ(TextBuffer("abcABCabc").FindChar('c', 3, true), 5);

	// FindLastChar()
	EXPECT_EQ(TextBuffer("abcABCabc").FindLastChar('c', 6, true), 5);

	// FindFirstNotOf()
	EXPECT_EQ(TextBuffer("abcABCabc").FindFirstNotOf("ABC", 3), 6);

	// FindLastNotOf()
	EXPECT_EQ(TextBuffer("abcABCabc").FindLastNotOf("ABC", 5), 2);

	// Contains()
	EXPECT_TRUE(TextBuffer("abcdefg").Contains("def"));
	EXPECT_FALSE(TextBuffer("abcdefg").Contains("ABC"));
	EXPECT_TRUE(TextBuffer("abcdefg").Contains("ABC", true));

	// StartsWith()
	EXPECT_TRUE(TextBuffer("abcdefg").StartsWith("abc"));
	EXPECT_FALSE(TextBuffer("abcdefg").StartsWith("ABC"));
	EXPECT_TRUE(TextBuffer("abcdefg").StartsWith("ABC", true));

	// StartsWithThenCut()
	test = "abc:def";
	EXPECT_TRUE(test.StartsWithThenCut("ABC:", true));
	EXPECT_EQ(test, "def");
	EXPECT_FALSE(test.StartsWithThenCut("ABC"));
	EXPECT_FALSE(test.StartsWithThenCut("DEF"));

	// EndsWith()
	EXPECT_TRUE(TextBuffer("abcdefg").EndsWith("efg"));
	EXPECT_FALSE(TextBuffer("abcdefg").EndsWith("EFG"));
	EXPECT_TRUE(TextBuffer("abcdefg").EndsWith("EFG", true));

	// MiddleStr()
	EXPECT_TRUE(TextBuffer("abcdefg").MiddleStr("cde", 2));

	// MadeOf()
	EXPECT_TRUE(TextBuffer("abcdefg").MadeOf("abcdefghijkl"));
	EXPECT_TRUE(TextBuffer("abcdefg").MadeOf("ABCDEFGHIJKL", true));
	EXPECT_FALSE(TextBuffer("abcdefg").MadeOf("123"));

	// Has()
	EXPECT_TRUE(TextBuffer("abcdefg").Has("abc123"));
	EXPECT_TRUE(TextBuffer("abcdefg").Has("ABC123", true));
	EXPECT_FALSE(TextBuffer("abcdefg").Has("123"));

	// CountChar()
	EXPECT_EQ(TextBuffer("abcABCabc").CountChar('a'), 2);

	// Split()
	EXPECT_EQ(TextBuffer("abc,d,ef").Split(','), std::vector<std::string>({ "abc","d","ef" }));
	EXPECT_EQ(TextBuffer("abc,,ef").Split(',', false), std::vector<std::string>({ "abc","","ef" }));
	EXPECT_EQ(TextBuffer("abc,,ef").Split(',', true), std::vector<std::string>({ "abc","ef" }));

	EXPECT_EQ(TextBuffer("abc,d;ef").Split(",;"), std::vector<std::string>({ "abc","d","ef" }));
	EXPECT_EQ(TextBuffer("abc,;ef").Split(",;", false), std::vector<std::string>({ "abc","","ef" }));
	EXPECT_EQ(TextBuffer("abc,;ef").Split(",;", true), std::vector<std::string>({ "abc","ef" }));

	EXPECT_EQ(TextBuffer("abc::d::ef").SplitStr("::"), std::vector<std::string>({ "abc","d","ef" }));
	EXPECT_EQ(TextBuffer("abc::::ef").SplitStr("::", false), std::vector<std::string>({ "abc","","ef" }));
	EXPECT_EQ(TextBuffer("abc::::ef").SplitStr("::", true), std::vector<std::string>({ "abc","ef" }));

	// ReplaceChar()
	test = "abc ABC123";
	EXPECT_EQ(test.ReplaceChar('a', 'z'), 1);
	EXPECT_EQ(test, "zbc ABC123");
	EXPECT_EQ(test.ReplaceChar('b', 'X', true), 2);
	EXPECT_EQ(test, "zXc AXC123");
	test = "-----";
	EXPECT_EQ(test.ReplaceChar('-', '*', false, 1, 3), 3);
	EXPECT_EQ(test, "-***-");

	// Replace()
	test = "abcdeABCDEabcde";
	EXPECT_EQ(test.Replace("CD", "**"), 7);
	EXPECT_EQ(test, "abcdeAB**Eabcde");
	EXPECT_EQ(test.Replace("ab", "_", true, false, 1), 5);
	EXPECT_EQ(test, "abcde_**Eabcde");
	test = "this is a test";
	EXPECT_EQ(test.Replace("is", "was", false, true), 5);
	EXPECT_EQ(test, "this was a test");

	// ReplaceAll()
	test = "abc abc.";
	EXPECT_EQ(test.ReplaceAll("abc", "-"), 2);
	EXPECT_EQ(test, "- -.");
	test = "abc ABC abc.";
	EXPECT_EQ(test.ReplaceAll("abc", "-", true, false), 3);
	EXPECT_EQ(test, "- - -.");
	test = "abc ABC abcde abc.";
	EXPECT_EQ(test.ReplaceAll("abc", "*", true, true, 2), 2);
	EXPECT_EQ(test, "abc * abcde *.");

	// Replace*()
	TextBuffer text("What is done is done");
	EXPECT_EQ(text.ReplaceAll("done", "undone"), 2);
	EXPECT_EQ(text, "What is undone is undone");
	EXPECT_EQ(text.ReplaceChar('n', 'N', false, 12), 3);
	EXPECT_EQ(text, "What is undoNe is uNdoNe");
	EXPECT_EQ(text.ReplaceChar('N', 'n', true), 4);
	EXPECT_EQ(text, "What is undone is undone");
	EXPECT_EQ(text.Replace("is", "IS"), 5);
	EXPECT_EQ(text, "What IS undone is undone");
}

// Case management
TEST(TextBufferTest, Case)
{

	// GetLowerCase()
	TextBuffer test = "abcABCabc";
	EXPECT_EQ(test.GetLowerCase(), "abcabcabc");
	EXPECT_EQ(test, "abcABCabc");
	// LowerCase()
	EXPECT_EQ(test.LowerCase(), "abcabcabc");

	// GetUpperCase()
	test = "abcABCabc";
	EXPECT_EQ(test.GetUpperCase(), "ABCABCABC");
	EXPECT_EQ(test, "abcABCabc");
	// UpperCase()
	EXPECT_EQ(test.UpperCase(), "ABCABCABC");

	// GetSentenceCase()
	test = "abc ABC abc";
	EXPECT_EQ(test.GetSentenceCase(), "Abc abc abc");
	EXPECT_EQ(test, "abc ABC abc");

	// ProperCase()
	EXPECT_EQ(test.GetSentenceCase(), "Abc abc abc");

	// GetProperCase()
	test = "abc ABC_abc";
	EXPECT_EQ(test.GetProperCase(), "Abc Abc_Abc");
	EXPECT_EQ(test, "abc ABC_abc");

	// ProperCase()
	EXPECT_EQ(test.ProperCase(), "Abc Abc_Abc");

	test = "abcABC";

	// ToUpper()
	EXPECT_EQ(test.ToUpper(0), "AbcABC");

	// ToLower()
	EXPECT_EQ(test.ToLower(0), "abcABC");

	// IsUpper()
	EXPECT_TRUE(test.IsUpper(3));
	EXPECT_FALSE(test.IsUpper(0));

	// IsLower()
	EXPECT_TRUE(test.IsLower(0));
	EXPECT_FALSE(test.IsLower(3));
}

// Indentation
TEST(TextBufferTest, Indentation)
{
	TextBuffer test = "1 abc\n2 def\n";
	TextBuffer test_copy = test;

	// GetIndented()
	EXPECT_EQ(test.GetIndented(2), "  1 abc\n  2 def\n");
	EXPECT_EQ(test, test_copy);

	// Indent()
	EXPECT_EQ(test.Indent(2, '.'), "..1 abc\n..2 def\n");
	EXPECT_EQ(test.Indent(2), "  ..1 abc\n  ..2 def\n");

	test_copy = test;

	// GetUnindented()
	EXPECT_EQ(test.GetUnindented(2), "..1 abc\n..2 def\n");
	EXPECT_EQ(test, test_copy);

	// Unindent()
	EXPECT_EQ(test.Unindent(2), "..1 abc\n..2 def\n");
	EXPECT_EQ(test.Unindent(2, '.'), "1 abc\n2 def\n");
}

// Indentation
TEST(TextBufferTest, Cid)
{
	// GetCid()
	EXPECT_EQ(TextBuffer("1 # a test").GetCid(), "_1___a_test");

	// MakeCid()
	EXPECT_EQ(TextBuffer("1 # a test").MakeCid(), "_1___a_test");
}

// Size
TEST(TextBufferTest, Size)
{
	TextBuffer test;
	// Resize()
	EXPECT_TRUE(test.Resize(3));

	// GetSize()
	EXPECT_EQ(test.GetSize(), 3);
	char* p0 = &test[0];

	// Reserve()
	EXPECT_TRUE(test.Reserve(10000));
	char* p1 = &test[0];
	EXPECT_NE(p1, p0);

	// Fill()
	test.Fill(0, 9999, ' ');
	char* p2 = &test[0];
	EXPECT_EQ(p1, p2);
}

// File
TEST(TextBufferTest, File)
{
	const std::string file_name("TextBufferTest.txt");
	TextBuffer test_out("Line 1\nLine 2");

	// Save() and Load()
	test_out.Save(file_name);
	TextBuffer test_in;
	test_in.Load(file_name);
	EXPECT_EQ(test_in, test_out);

	// ReadLine(), AppendLine()

	TextBuffer test_ln;
	std::ifstream ifs(file_name);
	test_ln.ReadLine(ifs, true);
	EXPECT_EQ(test_ln, "Line 1\n");

	std::streampos pos = ifs.tellg();
	test_ln.AppendLine(ifs, true);
	EXPECT_EQ(test_ln, test_out);

	ifs.seekg(pos);
	test_ln.ReadLine(ifs, true, true);
	EXPECT_EQ(test_ln, "Line 2\n");
	ifs.close();

	std::ifstream testfile(file_name);
	EXPECT_TRUE(testfile.good());
	test_in.Clear();
	testfile >> test_in;
	testfile.close();
	EXPECT_EQ(test_in, test_out);

	// ReadSplitLine()

	const std::string multiline_file_name("TextBufferMultiLineTest.txt");
	TextBuffer test_multiline("This is \\ \n  \ta split line");
	test_multiline.Save(multiline_file_name);
	testfile.close();

	test_multiline.Clear();
	std::ifstream ifs2(multiline_file_name);
	test_multiline.ReadSplitLine(ifs2, true);
	EXPECT_EQ(test_multiline, "This is a split line");
	ifs2.close();
}
