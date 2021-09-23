//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Text Utility - Basic text editing utility
/// @file text_util.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>
#include <algorithm>
#include <vector>

namespace gpvulc
{
	/// @defgroup Text Text
	/// @brief Text utility library.
	/// @author Giovanni Paolo Vigano'

	/// @addtogroup Text
	/// @{

	//! Convert the given string to lower case
	inline void StrLower(std::string& str)
	{
		std::for_each(str.begin(), str.end(), [](char& c)
		{
			c = tolower(c);
		});
	}


	//! Convert the given string to upper case
	inline void StrUpper(std::string& str)
	{
		std::for_each(str.begin(), str.end(), [](char& c)
		{
			c = toupper(c);
		});
	}


	//! Convert the given string to proper case
	void StrProperCase(std::string& str);


	//! Convert the given string to sentence case
	void StrSentenceCase(std::string& str);


	//! Get the given string converted to lower case
	inline std::string GetLowerStr(const std::string& str)
	{
		std::string lwrstr = str;
		StrLower(lwrstr);
		return lwrstr;
	}


	//! Get the given string converted to upper case
	inline std::string GetUpperStr(const std::string& str)
	{
		std::string uprstr = str;
		StrUpper(uprstr);
		return uprstr;
	}


	//! Get the given string converted to proper case
	inline std::string GetProperCaseStr(const std::string& str)
	{
		std::string pcstr = str;
		StrProperCase(pcstr);
		return pcstr;
	}


	//! Get the given string converted to proper case
	inline std::string GetSentenceCaseStr(const std::string& str)
	{
		std::string pcstr = str;
		StrSentenceCase(pcstr);
		return pcstr;
	}


	//! Check equality between two strings, optionally ignoring case
	inline bool StrEqual(const std::string& str1, const std::string& str2, bool case_insensitive = false)
	{
		if (case_insensitive)
		{
			return GetLowerStr(str1) == GetLowerStr(str2);
		}
		return str1 == str2;
	}


	//! Check equality between a string and one of other strings, optionally ignoring case
	inline bool StrIsOneOf(const std::string& str1, const std::vector<std::string>& otherStr, bool case_insensitive = false)
	{
		bool isEqual = false;
		for (const std::string& str : otherStr)
		{
			if ((case_insensitive && GetLowerStr(str1) == GetLowerStr(str))
				|| (!case_insensitive && str1 == str))
			{
				return true;
			}
		}
		return false;
	}


	/*!
	 Indent the given text, if more lines are present they are indented in block.
	 @see UnindentStr()
	 @param[in,out] str text to be indented
	 @param[in] n number of characters to be prefixed
	 @param[in] c character to be prefixed (default is space)
	*/
	void IndentStr(std::string& str, size_t n, char c = ' ');


	/*!
	 Unindent the given text, if more lines are present they are unindented in block.
	 @see IndentStr()
	 @param[in,out] str text to be unindented
	 @param[in] n number of characters to be removed
	 @param[in] c character to be removed (default is space)
	*/
	void UnindentStr(std::string& str, size_t n, char c = ' ');


	//! Create a C language compliant identifier from the given string
	std::string GetCidStr(const std::string& str);


	/*!
	Find the next token in a string, starting at the given position and separated by the given characters.
	@param inputText String where the token must be found.
	@param currPos Index (0-based) from which to start searching a token
	@param result The token is stored here if found, otherwise this is set to an empty string.
	@param sep Set of delimiter characters stored in a string.
	@return true if the token delimiter was found (the token is stored), false in any other case (store an empty string)
	*/
	bool GetToken(const std::string& inputText, size_t& currPos, std::string& result, const std::string& sep);


	/*!
	Count the number of different characters between two strings of the same size.
	@param inputText1 first text to compare
	@param inputText2 second text to compare
	@param length compare this number of characters, if zero (default) or negative all the text is compared
	@return the number of different characters or -1 if not comparable (empty or different sizes)
	*/
	int StrDiffCount(const std::string& inputText1, const std::string& inputText2, int length = 0);


	/*!
	Load a text file from the given path to the given string.
	@param path path name of the file
	@param text the string to be read
	@param append append the file content to the string
	@return false on error
	*/
	bool LoadText(const std::string& path, std::string& text, bool append = false);


	/*!
	Save a string to a text file at the given path.
	@param path Path name of the file
	@param text the string to be written
	@param append append the string to the file content
	@return false on error
	*/
	bool SaveText(const std::string& path, const std::string& text, bool append = false);


	/*!
	Load a text file from the given stream to the given string.
	@param textStream text input stream
	@param[out] text the string to be read
	@param append append the read text to the string
	@return false on error
	*/
	bool ReadText(std::istream& textStream, std::string& text, bool append = false);


	/*!
	Save a string to a stream.
	@param textStream text output stream
	@param[in] text the string to be written
	@return false on error
	*/
	bool WriteText(std::ostream& textStream, const std::string& text);

	///@}

}//namespace gpvulc

