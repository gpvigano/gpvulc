//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


// TextParser test program 

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gpvulc/text/TextParser.h>

using namespace gpvulc;

#include <gtest/gtest.h>


// Tests some functions
TEST(TextParserTest, Overall)
{
  TextBuffer text("int f( int a ) {\n if(a<0){return 0;} \n}");

  TextParser parser(text);
  EXPECT_TRUE(parser.GetBlock("(",")"));
  EXPECT_EQ(parser.Result()," int a ");
  EXPECT_TRUE(parser.Undo());
  EXPECT_TRUE(parser.GetToken());
  EXPECT_EQ(parser.Result(),"int");
  EXPECT_TRUE(parser.GetBlockAfter("if","{","}"));
  EXPECT_EQ(parser.Result(),"return 0;");
  parser.ResetParsing();
  EXPECT_TRUE(parser.Forward(3));
  EXPECT_TRUE(parser.Reach("{"));
}


// Test constructors and GetTextBuffer()
TEST(TextParserTest, Constructors)
{
  TextBuffer text("int f( int a ) {\n if(a<0){return 0;} \n}");
  EXPECT_EQ(TextParser().GetTextBuffer(),"");
  EXPECT_EQ(TextParser(text).GetTextBuffer(),text);
}


// Test parser settings
TEST(TextParserTest, Settings)
{
  TextParser parser;
  EXPECT_EQ(parser.GetDefaultSeparators()," \t\n\r");
  parser.SetDefaultSeparators("abc");
  EXPECT_EQ(parser.GetDefaultSeparators(),"abc");
  EXPECT_FALSE(parser.GetCaseInsensitive());
  parser.SetCaseInsensitive();
  EXPECT_TRUE(parser.GetCaseInsensitive());
  EXPECT_FALSE(parser.GetQuotedTextIgnored());
  parser.SetQuotedTextIgnored();
  EXPECT_TRUE(parser.GetQuotedTextIgnored());
  EXPECT_FALSE(parser.GetCppCommentsIgnored());
  parser.SetCppCommentsIgnored();
  EXPECT_TRUE(parser.GetCppCommentsIgnored());
}


// Test parser parsing
TEST(TextParserTest, Parsing)
{
  std::string parserText("abcde [[AAA BBB]] { {x{y}z} }\n1, 2, 3");
  TextParser parser(parserText);

  EXPECT_TRUE(parser.Forward(2));
  EXPECT_EQ(parser.GetOffset(),2);
  EXPECT_EQ(parser.Result(),"ab");

  EXPECT_TRUE(parser.Backward(2));
  EXPECT_EQ(parser.GetOffset(),0);
  EXPECT_EQ(parser.Result(),"ab");

  EXPECT_FALSE(parser.Backward());

  EXPECT_TRUE(parser.Compare("abcde"));
  EXPECT_TRUE(parser.Compare("abcfg",3));
  EXPECT_TRUE(parser.CompareStrChr("abc","zxd"));

  EXPECT_TRUE(parser.CompareList({ "bcfg","ab","zxd" }));

  EXPECT_TRUE(parser.ReachFirstOf("xdz"));
  EXPECT_EQ(parser.Result(),"abc");
  EXPECT_FALSE(parser.CompareList({ "bcfg","ab","zxd" }));

  EXPECT_TRUE(parser.GetBlock("{","}"));
  EXPECT_EQ(parser.Result()," {x{y}z} ");

  EXPECT_TRUE(parser.Undo());
  EXPECT_TRUE(parser.GetBlock("{","}",3));
  EXPECT_EQ(parser.Result(),"y");

  EXPECT_TRUE(parser.GetBackBlock("[","]",2));
  EXPECT_EQ(parser.Result(),"AAA BBB");
  EXPECT_EQ(parser.GetParsedText(),"abcde ");

  parser.ResetParsing();
  EXPECT_TRUE(parser.GoBeyond("de"));
  EXPECT_TRUE(parser.ResultIs("abcde"));
  EXPECT_EQ(parser.GetParsedText(),"abcde");

  EXPECT_TRUE(parser.SkipSpaces());
  EXPECT_EQ(parser.Result()," ");

  EXPECT_TRUE(parser.GetBlockAfter("x","{","}"));
  EXPECT_EQ(parser.Result(),"y");

  EXPECT_TRUE(parser.GetLine());
  EXPECT_TRUE(parser.ResultIs("z} }"));
  EXPECT_EQ(parser.GetNotParsedText(),"1, 2, 3");

  EXPECT_TRUE(parser.GetField(","));
  EXPECT_EQ(parser.Result(),"1");

  EXPECT_TRUE(parser.GetToken(", "));
  EXPECT_EQ(parser.Result(),"2");

  EXPECT_FALSE(parser.Complete());
  EXPECT_TRUE(parser.GetRemainder());
  EXPECT_EQ(parser.Result(),"3");

  EXPECT_TRUE(parser.ReachLastOf("]\n}"));
  EXPECT_EQ(parser.Result(),"1, 2, 3");

  EXPECT_TRUE(parser.ReachLastOf("]abc"));
  EXPECT_EQ(parser.Result()," { {x{y}z} }\n");

  parser.ResetParsing();
  parser.Forward(2);
  std::vector<std::string> str = {"AAA", " [["};
  EXPECT_TRUE(parser.ReachFirstAmong(str));
  EXPECT_EQ(parser.Result(),"cde");
  EXPECT_TRUE(parser.Skip("[A B]"));
  EXPECT_EQ(parser.Result()," [[AAA BBB]] ");
  EXPECT_TRUE(parser.GetLine());
  EXPECT_EQ(parser.Result(),"{ {x{y}z} }");
  EXPECT_TRUE(parser.ResultIs("{ {x{y}z} }"));
  EXPECT_FALSE(parser.ResultIs("{ {X{Y}Z} }"));
  parser.SetCaseInsensitive();
  EXPECT_TRUE(parser.ResultIs("{ {X{Y}z} }"));
  EXPECT_TRUE(parser.GetField(","));
  EXPECT_TRUE(parser.GetToken(", "));
  EXPECT_TRUE(parser.GetToken(", "));
  EXPECT_TRUE(parser.Undo(3));
  EXPECT_TRUE(parser.GetField(","));
  EXPECT_EQ(parser.Result(),"1");

  EXPECT_EQ(parser.GetText(),parserText);
}


// Test parser bookmarks and selection
TEST(TextParserTest, Bookmarks)
{
	std::string parserText("/// bool MoveToBookmark(const std::string& name);");
	TextParser parser(parserText);
	EXPECT_EQ(parser.GetSelection(1,8), "// bool");
	EXPECT_TRUE(parser.Skip("/ "));
	parser.SetBookmark("StartDeclaration");
	EXPECT_TRUE(parser.Reach(";"));
	std::string declText = parser.Result();
	parser.SetBookmark("EndDeclaration");
	EXPECT_EQ(parser.GetSelection("StartDeclaration","EndDeclaration"), declText);
	EXPECT_TRUE(parser.MoveToBookmark("StartDeclaration"));
	EXPECT_EQ(parser.Result(), declText);

	EXPECT_TRUE(parser.GoBeyond("("));
	parser.SetBookmark("StartParam");
	EXPECT_TRUE(parser.Reach(")"));
	std::string paramText = parser.Result();
	parser.SetBookmark("EndParam");
	EXPECT_TRUE(parser.GetRemainder());
	EXPECT_EQ(parser.GetSelection("StartParam","EndParam"), paramText);
	EXPECT_TRUE(parser.DeleteBookmark("StartParam"));
	EXPECT_FALSE(parser.DeleteBookmark("StartParam"));
	EXPECT_FALSE(parser.MoveToBookmark("StartParam"));
	parser.DeleteAllBookmarks();
	EXPECT_FALSE(parser.MoveToBookmark("StartDeclaration"));
}


// Test parser file
TEST(TextParserTest, File)
{
  TextBuffer test("line 1\nline 2\nline 3");

  const std::string file_name("parser_test.txt");

  TextParser parser;
  parser.SetText(test);
  EXPECT_TRUE(parser.SaveFile(file_name));
  parser.Clear();
  EXPECT_TRUE(parser.LoadFile(file_name));
  EXPECT_EQ(parser.GetTextBuffer(),test);
  parser.Clear();
  EXPECT_EQ(parser.GetTextBuffer(),"");

  std::ifstream ifs(file_name);
  EXPECT_TRUE(parser.ReadLine(ifs));
  EXPECT_EQ(parser.GetTextBuffer(),"line 1");
  EXPECT_TRUE(parser.ReadLine(ifs,true));
  EXPECT_EQ(parser.GetTextBuffer(),"line 2\n");
  std::streampos pos = ifs.tellg();
  EXPECT_TRUE(parser.ReadLine(ifs,true,true));
  EXPECT_EQ(parser.GetTextBuffer(),"line 3\n");
  ifs.seekg(pos);
  EXPECT_TRUE(parser.ReadLine(ifs,true));
  EXPECT_EQ(parser.GetTextBuffer(),"line 3");
  ifs.seekg(pos);
  EXPECT_TRUE(parser.ReadLine(ifs,false));
  EXPECT_EQ(parser.GetTextBuffer(),"line 3");
  ifs.seekg(0, std::ios::beg);
}
