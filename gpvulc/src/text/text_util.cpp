//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

/// @brief Text processing utility
/// @file TextUtil.cpp
/// @author Giovanni Paolo Vigano'


#include <gpvulc/text/text_util.h>
#include <gpvulc/console/console_util.h>

#include <fstream>

namespace gpvulc
{

	//---------------------------------------------------------------------
	// string functions definition

	void StrProperCase(std::string& str)
	{
		if (str.empty())
		{
			return;
		}
		bool capital = true;
		size_t len = str.length();

		for (size_t i = 0; i < len; ++i)
		{
			if (isalpha(str[i]))
			{
				if (capital) str[i] = toupper(str[i]);
				else str[i] = tolower(str[i]);
				capital = false;
			}
			else capital = true;
		}
	}


	void StrSentenceCase(std::string& str)
	{
		if (str.empty())
		{
			return;
		}

		size_t len = str.length();
		size_t i = 0;
		for (; i < len; ++i)
		{
			if (isalpha(str[i]))
			{
				str[i] = toupper(str[i]);
				++i;
				break;
			}
		}

		for (; i < len; ++i) str[i] = tolower(str[i]);
	}


	void IndentStr(std::string& str, size_t n, char c)
	{
		if (str.empty())
		{
			return;
		}
		std::string indstr(n, c);
		size_t pos = 0;
		while (pos != std::string::npos)
		{
			str.insert(pos, indstr);
			pos = str.find('\n', pos);
			if (pos != std::string::npos)
			{
				if (pos >= str.length() - 1) break;
				++pos;
			}
		}
	}


	void UnindentStr(std::string& str, size_t n, char c)
	{
		size_t pos = 0;
		size_t pos_c = 0;
		size_t cutoff = 0;
		while (pos != std::string::npos)
		{
			pos_c = str.find_first_not_of(c, pos);
			if (pos_c != std::string::npos)
			{
				if (pos_c - pos < n) cutoff = pos_c - pos;
				else cutoff = n;
				str.erase(pos, cutoff);
			}
			pos = str.find('\n', pos);
			if (pos != std::string::npos)
			{
				if (pos >= str.length() - 1) break;
				++pos;
			}
		}
	}


	std::string GetCidStr(const std::string& str)
	{
		if (str.empty())
		{
			return "_";
		}

		std::string cid;
		if ((unsigned)(str[0] + 1) <= 256 && isdigit(str[0])) cid = "_";

		cid += str;

		for (size_t i = 0; i < cid.size(); ++i)
		{
			if ((unsigned)(cid[i] + 1)>256 || !isalnum(cid[i]))
			{
				cid[i] = '_';
			}
		}
		return cid;
	}

	bool GetToken(const std::string& inputText, size_t& currPos, std::string& result, const std::string& sep)
	{
		if (currPos >= inputText.length())
		{
			return false;
		}
		size_t len = 0;
		size_t sepstart, toklen, sepend;
		std::string separators = sep;
		if (sep.empty()) separators = " \t";

		// skip heading separators
		sepstart = inputText.find_first_not_of(separators, currPos);
		if (sepstart == std::string::npos)
		{
			result.clear();
			currPos = inputText.length();
			return false;
		}

		len += sepstart;
		std::string buf = inputText.substr(sepstart);

		// find token end
		toklen = buf.find_first_of(separators);
		if (toklen == std::string::npos) toklen = buf.length();
		len += toklen;

		result = buf.substr(0, toklen);

		// skip trailing separators
		sepend = buf.find_first_not_of(separators, toklen);
		if (sepend != std::string::npos)
		{
			len += sepend - toklen;
		}

		currPos = len;

		return true;
	}



	int StrDiffCount(const std::string& inputText1, const std::string& inputText2, int length)
	{
		size_t textLength;
		if (length > 0)
		{
			if (inputText1.size() < (size_t)length || inputText2.size() < (size_t)length)
			{
				return -1;
			}
			textLength = (size_t)length;
		}
		else
		{
			if (inputText1.empty() && inputText2.empty())
			{
				return 0;
			}
			if (inputText1.size() != inputText2.size() || inputText1.empty() || inputText2.empty())
			{
				return -1;
			}
			textLength = inputText1.size();
		}
		int count = 0;
		for (size_t i = 0; i < textLength; i++)
		{
			if (inputText1[i] != inputText2[i])
			{
				count++;
			}
		}
		return count;
	}



	bool LoadText(const std::string& path, std::string& text, bool append)
	{
		if (path.empty())
		{
			return false;
		}

		std::ifstream textFileStream(path);
		bool result = ReadText(textFileStream, text, append);
		textFileStream.close();
		return result;
	}


	bool SaveText(const std::string& path, const std::string& text, bool append)
	{
		if (path.empty())
		{
			return false;
		}

		std::ofstream textFileStream(path, append ? (std::ios::out | std::ios::app) : std::ios::out);
		bool result = WriteText(textFileStream, text);
		textFileStream.close();
		return result;
	}


	bool ReadText(std::istream& textStream, std::string& text, bool append)
	{
		if (!textStream.good())
		{
			return false;
		}

		std::streampos streamStart = textStream.tellg();
		textStream.seekg(0, std::ios::end);
		size_t textSize = (size_t)(textStream.tellg() - streamStart);
		textStream.seekg(streamStart, std::ios::beg);

		if (append)
		{
			text.reserve(text.size() + textSize);

			text.append((std::istreambuf_iterator<char>(textStream)),
				std::istreambuf_iterator<char>());
		}
		else
		{
			text.reserve(textSize);

			text.assign((std::istreambuf_iterator<char>(textStream)),
				std::istreambuf_iterator<char>());
		}

		return true;
	}


	bool WriteText(std::ostream& textStream, const std::string& text)
	{
		if (textStream.good())
		{
			textStream << text;
			return true;
		}
		return false;
	}

}