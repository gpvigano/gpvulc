//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Text buffer
/// @file TextBuffer.cpp
/// @author Giovanni Paolo Vigano'


#include <gpvulc/text/TextBuffer.h>
#include <gpvulc/text/text_util.h>
#include <gpvulc/text/string_conv.h>

#include <fstream>
#include <sstream>
#include <cstring>


#if defined(_MSC_VER)
#define StrCaseCmp _stricmp
#define StrNCaseCmp _strnicmp
#elif defined( __GNUC__ )
#define StrCaseCmp strcasecmp
#define StrNCaseCmp strncasecmp
#else
inline int StrCaseCmp(const char *s1, const char *s2)
{
    char c1, c2;
    do  {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    }
    while ( c1 && (c1 == c2) );
    return c1 - c2;
}
inline int StrNCaseCmp(const char *s1, const char *s2, size_t n)
{
	size_t count = 0;
    char c1, c2;
    do  {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
		count++;
    }
    while ( count<n && c1 && (c1 == c2) );
    return c1 - c2;
}
#endif


	//---------------------------------------------------------------------
	// TextBuffer overloaded operators implementation


#define GPVULC_PLUS_OP_IMPLEMENTATION_RHS gpvulc::TextBuffer sum(txt); sum.Cat(s); return sum;
#define GPVULC_PLUS_OP_IMPLEMENTATION_LHS gpvulc::TextBuffer sum(s); sum.Cat(txt); return sum;

gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const gpvulc::TextBuffer& s) { GPVULC_PLUS_OP_IMPLEMENTATION_RHS }

gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const std::string& s) { GPVULC_PLUS_OP_IMPLEMENTATION_RHS }

gpvulc::TextBuffer operator +(const std::string& s, const gpvulc::TextBuffer& txt) { GPVULC_PLUS_OP_IMPLEMENTATION_LHS }

gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const char* s) { GPVULC_PLUS_OP_IMPLEMENTATION_RHS }

gpvulc::TextBuffer operator +(const char* s, const gpvulc::TextBuffer& txt) { GPVULC_PLUS_OP_IMPLEMENTATION_LHS }

gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const void* s) { GPVULC_PLUS_OP_IMPLEMENTATION_RHS }

gpvulc::TextBuffer operator +(const void* s, const gpvulc::TextBuffer& txt) { GPVULC_PLUS_OP_IMPLEMENTATION_LHS }

#undef GPVULC_PLUS_OP_IMPLEMENTATION_RHS
#undef GPVULC_PLUS_OP_IMPLEMENTATION_LHS

//-------------------------------------------------------------

std::istream& operator >> (std::istream& strm, gpvulc::TextBuffer& txt)
{
	std::string buf;
	if (gpvulc::ReadText(strm, buf))
	{
		txt = buf;
	}
	return strm;
}


std::ostream& operator << (std::ostream& strm, const gpvulc::TextBuffer& txt)
{
	if (!txt.IsEmpty()) strm << txt.StdString();
	return strm;
}


namespace gpvulc
{


	//---------------------------------------------------------------------
	// TextBuffer class implementation


	TextBuffer::TextBuffer(int dim, bool init, char c)
	{
		Resize(dim);
		Fill(0, dim - 1, c);
	}


	bool TextBuffer::IntToPos(int pos, size_t& search_pos, bool rev) const
	{
		if (mStdString.empty())
		{
			return false;
		}
		if (rev)
		{
			if (pos >= 0 && pos < (int)mStdString.size()) search_pos = (size_t)pos;
			else search_pos = mStdString.length() - 1;
		}
		else
		{
			if (pos >= (int)mStdString.size())
			{
				return false;
			}
			if (pos >= 0) search_pos = (size_t)pos;
			else search_pos = 0;
		}
		return true;
	}


	TextBuffer& TextBuffer::Fill(int start, int end, char c)
	{
		if (mStdString.empty())
		{
			return *this;
		}
		if (start < 0) start = 0;
		if (end < 0) end = (int)mStdString.size() - 1;
		for (int i = start; i <= end && i < (int)mStdString.size(); ++i)
		{
			mStdString[i] = c;
		}
		if (end >= (int)mStdString.size())
		{
			mStdString.resize(end + 1, c);
		}

		return *this;
	}


	int TextBuffer::GetInt(int def_val)
	{
		int val = def_val;
		try
		{
			val = std::stoi(mStdString);
		}
		catch (...)
		{
			return def_val;
		}
		return val;
	}


	unsigned TextBuffer::GetHex(unsigned def_val)
	{
		unsigned val = def_val;
		try
		{
			val = std::stoi(mStdString, 0, 16);
		}
		catch (...)
		{
			return def_val;
		}
		return val;
	}


	double TextBuffer::GetDouble(double def_val)
	{
		double val = def_val;
		try
		{
			val = std::stod(mStdString);
		}
		catch (...)
		{
			return def_val;
		}
		return val;
	}


	float TextBuffer::GetFloat(float def_val)
	{
		float val = def_val;
		try
		{
			val = std::stof(mStdString);
		}
		catch (...)
		{
			return def_val;
		}
		return val;
	}


	//---------------------------------------------------------------------
	//                            Cat
	//


	TextBuffer& TextBuffer::Cat(const std::string& s, size_t chars)
	{
		if (s.empty())
		{
			return *this;
		}
		if (chars) mStdString.append(s.c_str(), chars);
		else mStdString += s;
		return *this;
	}


	TextBuffer& TextBuffer::Cat(const char* str, size_t chars)
	{
		if (str == nullptr)
		{
			return *this;
		}
		if (chars) mStdString.append(str, chars);
		else mStdString.append(str);

		return *this;
	}

	namespace
	{
		template <typename T>
		inline TextBuffer& cat_num(TextBuffer& txb, T n, int digits, int precision, bool zeroes)
		{
			txb.Cat(NumberToString(n, digits, precision, zeroes));
			return txb;
		}

		template <typename T>
		inline TextBuffer& set_num(TextBuffer& txb, T n, int digits, int precision, bool zeroes)
		{
			txb.Set(NumberToString(n, digits, precision, zeroes));
			return txb;
		}
	}


	TextBuffer& TextBuffer::Cat(int n, int digits, bool zeroes)
	{
		return cat_num(*this, n, digits, -1, zeroes);
	}


	TextBuffer& TextBuffer::Cat(unsigned int n, int digits, bool zeroes)
	{
		return cat_num(*this, n, digits, -1, zeroes);
	}


	TextBuffer& TextBuffer::Cat(float f, int width, int ndecimals, bool zeroes)
	{
		return cat_num(*this, f, width, ndecimals, zeroes);
	}


	TextBuffer& TextBuffer::Cat(double d, int width, int ndecimals, bool zeroes)
	{
		return cat_num(*this, d, width, ndecimals, zeroes);
	}


	TextBuffer& TextBuffer::Cat(char c, int count)
	{
		if (count > 0) mStdString.append(count, c);
		return *this;
	}


	TextBuffer& TextBuffer::Cat(const void* p)
	{
		std::ostringstream ss;
		ss.setf(std::ios::uppercase);
		ss << p;
		mStdString += ss.str();
		return *this;
	}


	//---------------------------------------------------------------------


	int TextBuffer::Compare(const std::string& str, bool caseInsensitive) const
	{
		int retval = 0;

		if (mStdString.empty() && str.empty())
		{
			return 0;
		}
		if (!mStdString.empty() && str.empty())
		{
			return 1;
		}
		if (mStdString.empty() && !str.empty())
		{
			return -1;
		}
		// !mStdString.empty() && !str.empty()
		retval = caseInsensitive ? StrCaseCmp(mStdString.c_str(), str.c_str()) : mStdString.compare(str);

		return retval;
	}



	TextBuffer& TextBuffer::Crop(const std::string& startstr, const std::string& endstr)
	{
		size_t begIdx;
		size_t endIdx;

		if (mStdString.empty())
		{
			return *this;
		}

		begIdx = 0;
		if (!startstr.empty())
		{
			size_t idx = mStdString.find_first_not_of(startstr, begIdx);
			if (idx != std::string::npos)
			{
				begIdx = idx;
			}
			else
			{
				mStdString.clear();
				return *this;
			}
		}
		endIdx = Length() - 1;
		if (!endstr.empty())
		{
			size_t idx = mStdString.find_last_not_of(endstr);
			if (idx != std::string::npos)
			{
				endIdx = idx;
			}
			else
			{
				mStdString.clear();
				return *this;
			}
		}

		SubString((int)begIdx, (int)endIdx);
		return *this;
	}


	bool TextBuffer::EndsWith(const std::string& str, bool caseInsensitive) const
	{
		const char *tmp;
		size_t n, ns;

		if (mStdString.empty())
		{
			return false;
		}
		if (str.empty())
		{
			return false;
		}
		n = str.length();
		ns = mStdString.length();
		if (n > ns)
		{
			return false;
		}
		tmp = &mStdString[ns - n];
		return caseInsensitive ? !StrNCaseCmp(tmp, str.c_str(), n) : !strncmp(tmp, str.c_str(), n);
	}


	bool TextBuffer::MiddleStr(const std::string& str, int beg, bool caseInsensitive) const
	{
		if (str.empty())
		{
			return false;
		}
		size_t beg_pos;
		if (!IntToPos(beg, beg_pos, false))
		{
			return false;
		}
		size_t end_pos;
		if (!IntToPos(beg + (int)str.length() - 1, end_pos, true))
		{
			return false;
		}
		std::string subs = GetSubString(beg, (int)end_pos);
		if (caseInsensitive)
		{
			return GetLowerStr(subs) == GetLowerStr(str);
		}
		return subs == str;
	}


	bool TextBuffer::Contains(const std::string& str, bool caseInsensitive) const
	{
		if (str.empty())
		{
			return false;
		}
		if (caseInsensitive)
		{
			return GetLowerStr(mStdString).find(GetLowerStr(str)) != std::string::npos;
		}
		return mStdString.find(str) != std::string::npos;
	}


	bool TextBuffer::Has(const std::string& str, bool caseInsensitive) const
	{
		if (mStdString.empty() || str.empty())
		{
			return false;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().find_first_of(GetLowerStr(str));
		else pos = mStdString.find_first_of(str);
		return pos != std::string::npos;
	}


	std::vector<std::string> TextBuffer::Split(char delimiter, bool removeEmpty) const
	{
		std::vector<std::string> subStrings;
		size_t offset = 0;
		size_t idx = 0;
		std::string entry;
		while ((idx = mStdString.find_first_of(delimiter, offset)) != std::string::npos)
		{
			entry = mStdString.substr(offset, idx - offset);
			if (!entry.empty() || !removeEmpty)
			{
				subStrings.push_back(entry);
			}
			offset = idx + 1;
		}
		subStrings.push_back(mStdString.substr(offset));
		return subStrings;
	}


	std::vector<std::string> TextBuffer::Split(const std::string& delimiters, bool removeEmpty) const
	{
		std::vector<std::string> subStrings;
		size_t offset = 0;
		size_t idx = 0;
		std::string entry;
		while ((idx = mStdString.find_first_of(delimiters, offset)) != std::string::npos)
		{
			entry = mStdString.substr(offset, idx - offset);
			if (!entry.empty() || !removeEmpty)
			{
				subStrings.push_back(entry);
			}
			offset = idx + 1;
		}
		subStrings.push_back(mStdString.substr(offset));
		return subStrings;
	}


	std::vector<std::string> TextBuffer::SplitStr(const std::string& delimiterString, bool removeEmpty) const
	{
		std::vector<std::string> subStrings;
		size_t offset = 0;
		size_t idx = 0;
		std::string entry;
		while ((idx = mStdString.find(delimiterString, offset)) != std::string::npos)
		{
			entry = mStdString.substr(offset, idx - offset);
			if (!entry.empty() || !removeEmpty)
			{
				subStrings.push_back(entry);
			}
			offset = idx + delimiterString.size();
		}
		if (offset < (int)mStdString.size())
		{
			subStrings.push_back(mStdString.substr(offset));
		}
		return subStrings;
	}


	int TextBuffer::FindFirstOf(const std::string& str, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, false))
		{
			return -1;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().find_first_of(GetLowerStr(str), search_pos);
		else pos = mStdString.find_first_of(str, search_pos);
		return PosToInt(pos);
	}


	int TextBuffer::FindLastOf(const std::string& str, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, true))
		{
			return -1;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().find_last_of(GetLowerStr(str), search_pos);
		else pos = mStdString.find_last_of(str, search_pos);
		return PosToInt(pos);
	}



	int TextBuffer::FindChar(char chr, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, false))
		{
			return -1;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().find(tolower(chr), search_pos);
		else pos = mStdString.find(chr, search_pos);
		return PosToInt(pos);
	}


	int TextBuffer::FindLastChar(char chr, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, true))
		{
			return -1;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().rfind(tolower(chr), search_pos);
		else pos = mStdString.rfind(chr, search_pos);
		return PosToInt(pos);
	}


	int TextBuffer::FindFirstNotOf(const std::string& str, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, false))
		{
			return -1;
		}
		size_t pos;
		if (caseInsensitive)
		{
			std::string lwr = GetLowerCase();
			std::string lwrstr = GetLowerStr(str);
			pos = lwr.find_first_not_of(lwrstr, search_pos);
		}
		else
		{
			pos = mStdString.find_first_not_of(str, search_pos);
		}

		return PosToInt(pos);
	}


	int TextBuffer::FindLastNotOf(const std::string& str, int start, bool caseInsensitive) const
	{
		size_t search_pos;
		if (!IntToPos(start, search_pos, true))
		{
			return -1;
		}
		size_t pos;
		if (caseInsensitive)
		{
			std::string lwr = GetLowerCase();
			std::string lwrstr = GetLowerStr(str);
			pos = lwr.find_last_not_of(lwrstr, search_pos);
		}
		else
		{
			pos = mStdString.find_last_not_of(str, search_pos);
		}
		return PosToInt(pos);
	}


	int TextBuffer::FindSubString(
		const std::string& str,
		bool caseInsensitive,
		bool words_only,
		int startpos) const
	{
		size_t search_pos;
		if (!IntToPos(startpos, search_pos, false))
		{
			return -1;
		}
		int start_idx;
		const char* temp = nullptr;
		size_t pos = std::string::npos;
		if (caseInsensitive)
		{
			pos = GetLowerCase().find(GetLowerStr(str), search_pos);
		}
		else
		{
			pos = mStdString.find(str, search_pos);
		}
		if (pos == std::string::npos)
		{
			return -1;
		}
		start_idx = (int)pos;

		if (words_only)
		{
			int end_idx = start_idx + (int)str.length();
			if ((start_idx > 0 && IsAlNum_(start_idx - 1) && IsAlNum_(start_idx))
				|| (IsAlNum_(end_idx) && IsAlNum_(end_idx - 1)))
			{
				return FindSubString(str, caseInsensitive, true, start_idx + 1);
			}
		}

		return start_idx;
	}


	int TextBuffer::FindRevSubString(
		const std::string& str,
		bool caseInsensitive,
		bool words_only,
		int startpos) const
	{
		size_t search_pos;
		if (!IntToPos(startpos, search_pos, true))
		{
			return -1;
		}
		int start_idx;
		const char* temp = nullptr;
		size_t pos = std::string::npos;
		if (caseInsensitive)
		{
			pos = GetLowerCase().rfind(GetLowerStr(str), search_pos);
		}
		else
		{
			pos = mStdString.rfind(str, search_pos);
		}
		if (pos == std::string::npos)
		{
			return -1;
		}
		start_idx = (int)pos;

		if (words_only)
		{
			int end_idx = start_idx + (int)str.length();
			if ((start_idx > 0 && IsAlNum_(start_idx - 1) && IsAlNum_(start_idx))
				|| (IsAlNum_(end_idx) && IsAlNum_(end_idx - 1)))
			{
				return FindRevSubString(str, caseInsensitive, true, start_idx - 1);
			}
		}

		return start_idx;
	}

	int TextBuffer::FindSubStringAny(const std::vector<std::string>& strList, bool caseInsensitive, bool words_only, int startpos) const
	{
		int minPos = -1;
		for (std::string str : strList)
		{
			int pos = FindSubString(str, caseInsensitive, words_only, startpos);
			if (pos >= 0 && (minPos<0 || minPos>pos)) minPos = pos;
		}
		return minPos;
	}


	int TextBuffer::FindRevSubStringAny(const std::vector<std::string>& strList, bool caseInsensitive, bool words_only, int startpos) const
	{
		int maxPos = -1;
		for (std::string str : strList)
		{
			int pos = FindRevSubString(str, caseInsensitive, words_only, startpos);
			if (pos >= 0 && maxPos < pos) maxPos = pos;
		}
		return maxPos;
	}


	int TextBuffer::Replace(
		const std::string& substr,
		const std::string& str,
		bool case_sensitive,
		bool words_only,
		int startpos)
	{
		int idx = FindSubString(substr, case_sensitive, words_only, startpos);
		if (idx < 0)
		{
			return -1;
		}

		mStdString.erase((size_t)idx, substr.length());
		mStdString.insert((size_t)idx, str);

		return idx;
	}


	TextBuffer& TextBuffer::Erase(int start, int end)
	{
		if (mStdString.empty() || start < 0)
		{
			return *this;
		}
		if (start < 0) start = 0;
		if (end < 0 || end >= (int)mStdString.size() - 1) end = (int)mStdString.size() - 1;
		if (start > end) std::swap(start, end);

		size_t count = end - start + 1;
		mStdString.erase((size_t)start, count);

		return *this;
	}


	std::string TextBuffer::GetSubString(int beg, int end) const
	{
		if (mStdString.empty())
		{
			return "";
		}
		if (beg < 0) beg = 0;
		if (end < 0 || end >= (int)mStdString.size())
		{
			end = (int)mStdString.size() - 1;
		}
		if (end < beg)
		{
			return "";
		}
		size_t offset = beg;
		size_t count = end - beg + 1;

		return mStdString.substr(offset, count);
	}


	TextBuffer& TextBuffer::SubString(int beg, int end)
	{
		Set(GetSubString(beg, end));
		return *this;
	}


	TextBuffer& TextBuffer::ClipChars(int beg, int end)
	{
		if (mStdString.empty())
		{
			return *this;
		}
		int len = Length();
		if (len <= beg + end) mStdString = "";
		else Set(GetSubString(beg, len - 1 - end));
		return *this;
	}


	TextBuffer& TextBuffer::Insert(int pos, const std::string& str)
	{
		int len = Length();
		int str_len = (int)str.length();
		if (pos < 0 || len == 0 || str_len == 0 || pos >= len)
		{
			return *this;
		}
		mStdString.insert((size_t)pos, str);
		return *this;
	}


	TextBuffer& TextBuffer::InsertChar(int pos, char ch, int count)
	{
		int len = Length();
		if (len == 0 || pos<0 || count <= 0 || pos + count>len)
		{
			return *this;
		}
		mStdString.insert((size_t)pos, count, ch);
		return *this;
	}



	int TextBuffer::ReplaceAll(
		const std::string& substr,
		const std::string& str,
		bool case_sensitive,
		bool words_only,
		int startpos)
	{
		int count = 0;
		int found = -1;
		int pos = startpos;
		while ((found = Replace(substr, str, case_sensitive, words_only, pos)) >= 0)
		{
			pos = found + (int)str.length();
			++count;
		}
		return count;
	}


	TextBuffer& TextBuffer::ReplaceAt(int pos, const std::string& str)
	{
		int len = (int)mStdString.length();
		int str_len = (int)str.length();
		if (len == 0 || str_len == 0 || pos >= len)
		{
			return *this;
		}
		mStdString.replace((size_t)pos, str_len, str);
		return *this;
	}


	TextBuffer& TextBuffer::RemoveLastChar()
	{
		if (!mStdString.empty())
		{
			mStdString.pop_back();
		}
		return *this;
	}


	bool TextBuffer::MadeOf(const std::string& str, bool caseInsensitive) const
	{
		if (mStdString.empty())
		{
			return false;
		}
		size_t pos = std::string::npos;
		if (caseInsensitive) pos = GetLowerCase().find_first_not_of(GetLowerStr(str));
		else pos = mStdString.find_first_not_of(str);
		return pos == std::string::npos;
	}


	int TextBuffer::ReplaceChar(
		char oldchar,
		char newchar,
		bool caseInsensitive,
		int startpos,
		int endpos)
	{
		if (startpos < 0)
		{
			return 0;
		}
		int len(Length());
		if (!len)
		{
			return 0;
		}
		if (endpos < 0) endpos = len - 1;
		if (startpos > endpos) std::swap(startpos, endpos);
		if (endpos > (int)len - 1) endpos = len - 1;

		int count = 0;
		size_t beg = (size_t)startpos;
		size_t end = (size_t)endpos;

		if (caseInsensitive)
		{
			char lwr = tolower(oldchar);
			for (size_t i = beg; i <= end; ++i)
			{
				if (tolower(mStdString[i]) == lwr)
				{
					mStdString[i] = newchar;
					++count;
				}
			}
		}
		else
		{
			for (size_t i = beg; i <= end; ++i)
			{
				if (mStdString[i] == oldchar)
				{
					mStdString[i] = newchar;
					++count;
				}
			}
		}

		return count;
	}


	std::string TextBuffer::GetLowerCase() const
	{
		if (mStdString.empty())
		{
			return "";
		}

		return GetLowerStr(mStdString);
	}


	std::string TextBuffer::GetSentenceCase() const
	{
		if (mStdString.empty())
		{
			return "";
		}
		std::string cpt = mStdString;
		size_t len = cpt.length();
		size_t i = 0;
		for (; i < len; ++i)
		{
			if (isalpha(cpt[i]))
			{
				cpt[i] = toupper(cpt[i]);
				++i;
				break;
			}
		}

		for (; i < len; ++i) cpt[i] = tolower(cpt[i]);

		return cpt;
	}

	std::string TextBuffer::GetProperCase() const
	{
		if (mStdString.empty())
		{
			return "";
		}
		bool capital = true;
		std::string cpt = mStdString;
		size_t len = cpt.length();
		for (size_t i = 0; i < len; ++i)
		{
			if (isalpha(cpt[i]))
			{
				if (capital) cpt[i] = toupper(cpt[i]);
				else cpt[i] = tolower(cpt[i]);
				capital = false;
			}
			else capital = true;
		}

		return cpt;
	}


	TextBuffer& TextBuffer::SentenceCase()
	{
		mStdString = GetSentenceCase();
		return *this;
	}


	TextBuffer& TextBuffer::ProperCase()
	{
		mStdString = GetProperCase();
		return *this;
	}


	void TextBuffer::Transfer(std::string& outString)
	{
		std::swap(mStdString, outString);
		mStdString.clear();
	}


	std::string TextBuffer::GetUpperCase() const
	{
		return GetUpperStr(mStdString);
	}


	TextBuffer& TextBuffer::LowerCase()
	{
		StrLower(mStdString);
		return *this;
	}


	bool TextBuffer::Read(std::istream& inStream)
	{
		if (!inStream.good())
		{
			return false;
		}
		inStream >> *this;
		return true;
	}


	bool TextBuffer::ReadLine(std::istream& inStream, bool appendNewline, bool appendEofNewline)
	{
		if (!inStream.good())
		{
			return false;
		}
		std::getline(inStream, mStdString);
		if (appendNewline && (inStream.good() || appendEofNewline))
		{
			mStdString.append(1, '\n');
		}
		return true;
	}


	bool TextBuffer::AppendLine(std::istream& inStream, bool appendNewline, bool appendEofNewline)
	{
		if (!inStream.good())
		{
			return false;
		}
		std::string strline;
		std::getline(inStream, strline);
		mStdString += strline;
		if (appendNewline && (inStream.good() || appendEofNewline))
		{
			mStdString.append(1, '\n');
		}

		return true;
	}


	bool TextBuffer::ReadSplitLine(std::istream& inStream, bool skipSpaces, bool appendNewline, bool appendEofNewline)
	{
		ReadLine(inStream);
		if (skipSpaces)
		{
			Crop();
		}
		if (!inStream.good() && mStdString.empty())
			return false;

		// lines can be split into more lines using back slash
		while (EndsWith("\\"))
		{
			RemoveLastChar();
			if (skipSpaces)
			{
				TextBuffer lineBuf;
				lineBuf.ReadLine(inStream);
				lineBuf.Crop();
				mStdString.append(lineBuf);
			}
			else
			{
				AppendLine(inStream);
			}
		}
		if (appendNewline && (inStream.good() || appendEofNewline))
		{
			mStdString.append(1, '\n');
		}

		return true;
	}


	bool TextBuffer::Write(std::ostream& ostrm) const
	{
		if (!ostrm.good())
		{
			return false;
		}
		ostrm << *this;

		return true;
	}


	bool TextBuffer::Load(const std::string& filename, bool append)
	{
		return LoadText(filename, mStdString, append);
	}


	bool TextBuffer::Save(const std::string& filename, bool append) const
	{
		return SaveText(filename, mStdString, append);
	}


	bool TextBuffer::Reserve(int dim)
	{
		mStdString.reserve((size_t)dim);
		return true;
	}


	bool TextBuffer::Resize(int dim)
	{
		mStdString.resize((size_t)dim);
		return true;
	}


	TextBuffer& TextBuffer::Set(const std::string& s)
	{
		mStdString = s;
		return *this;
	}


	TextBuffer& TextBuffer::Set(const std::string& s, size_t chars)
	{
		if (chars && !s.empty()) mStdString.assign(s.c_str(), chars);
		else mStdString = s;
		return *this;
	}


	TextBuffer& TextBuffer::Set(const TextBuffer & s)
	{
		mStdString = s.StdString();
		return *this;
	}


	TextBuffer& TextBuffer::Set(const char* str, size_t chars)
	{
		if (str == nullptr) mStdString.clear();
		else if (chars > 0) mStdString.assign(str, chars);
		else mStdString = str;

		return *this;
	}


	TextBuffer& TextBuffer::Set(int n, int digits, bool zeroes)
	{
		return set_num(*this, n, digits, -1, zeroes);
	}


	TextBuffer& TextBuffer::Set(unsigned int n, int digits, bool zeroes)
	{
		return set_num(*this, n, digits, -1, zeroes);
	}


	TextBuffer& TextBuffer::Set(float f, int width, int ndecimals, bool zeroes)
	{
		return set_num(*this, f, width, ndecimals, zeroes);
	}


	TextBuffer& TextBuffer::Set(double f, int width, int ndecimals, bool zeroes)
	{
		return set_num(*this, f, width, ndecimals, zeroes);
	}

	TextBuffer& TextBuffer::Set(long n)
	{
		mStdString = std::to_string(n);
		return *this;
	}

	TextBuffer& TextBuffer::Set(long long n)
	{
		mStdString = std::to_string(n);
		return *this;
	}


	TextBuffer& TextBuffer::Set(char c, int count)
	{
		if (count > 0) mStdString.assign(count, c);
		return *this;
	}


	TextBuffer& TextBuffer::Set(const void* p)
	{
		std::ostringstream ss;
		ss.setf(std::ios::uppercase);
//        ss.setf(std::ios::hex);
//        ss.unsetf(std::ios::showbase);
		ss << p;
		mStdString = ss.str();
		return *this;
	}


	bool TextBuffer::StartsWith(const std::string& str, bool caseInsensitive) const
	{
		if (str.empty())
		{
			return false;
		}
		if (mStdString.empty())
		{
			return false;
		}
		int len = (int)str.length();
		std::string subs = GetSubString(0, len - 1);
		return StrEqual(subs, str, caseInsensitive);
	}


	bool TextBuffer::StartsWithThenCut(const std::string& str, bool caseInsensitive)
	{
		if (!StartsWith(str, caseInsensitive))
		{
			return false;
		}
		mStdString.erase(0, str.length());
		return true;
	}


	TextBuffer& TextBuffer::UpperCase()
	{
		StrUpper(mStdString);
		return *this;
	}


	TextBuffer& TextBuffer::ToLower(int i)
	{
		if (i >= 0 && i < (int)mStdString.length()) mStdString[i] = tolower(mStdString[i]);
		return *this;
	}


	TextBuffer& TextBuffer::ToUpper(int i)
	{
		if (i >= 0 && i < (int)mStdString.length()) mStdString[i] = toupper(mStdString[i]);
		return *this;
	}


	int TextBuffer::CountLines() const
	{
		if (mStdString.empty())
		{
			return 0;
		}
		int count = 1;
		size_t len = (int)mStdString.length();
		for (size_t i = 0; i < len - 1; ++i)
		{
			if (mStdString[i] == '\n') count++;
		}
		return count;
	}


	std::string TextBuffer::GetIndented(int n, char c) const
	{
		if (mStdString.empty())
		{
			return "";
		}
		std::string unind = mStdString;
		IndentStr(unind, n, c);
		return unind;
	}


	std::string TextBuffer::GetUnindented(int n, char c) const
	{
		if (mStdString.empty())
		{
			return "";
		}
		std::string unind = mStdString;
		UnindentStr(unind, n, c);
		return unind;
	}


	TextBuffer& TextBuffer::Indent(int n, char c)
	{
		IndentStr(mStdString, n, c);
		return *this;
	}


	TextBuffer& TextBuffer::Unindent(int n, char c)
	{
		UnindentStr(mStdString, n, c);
		return *this;
	}


	std::string TextBuffer::GetCid() const
	{
		return GetCidStr(mStdString);
	}


	TextBuffer& TextBuffer::MakeCid()
	{
		mStdString = GetCidStr(mStdString);
		return *this;
	}


	bool TextBuffer::IsLower(int i) const
	{
		if (i < 0 || i >= (int)mStdString.length())
		{
			return false;
		}
		return islower(mStdString[i]) != 0;
	}


	bool TextBuffer::IsUpper(int idx) const
	{
		if (idx < 0 || idx >= (int)mStdString.length())
		{
			return false;
		}
		return isupper(mStdString[idx]) != 0;
	}


	bool TextBuffer::IsAlNum(int idx) const
	{
		if (idx < 0 || idx >= (int)mStdString.length())
		{
			return false;
		}
		const char& c = mStdString[idx];
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
	}


	bool TextBuffer::IsAlNum_(int idx) const
	{
		if (idx < 0 || idx >= (int)mStdString.length())
		{
			return false;
		}
		const char& c = mStdString[idx];
		return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
	}


	bool TextBuffer::IsSpace(int idx) const
	{
		if (idx < 0 || idx >= (int)mStdString.length())
		{
			return false;
		}
		const char& c = mStdString[idx];
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}


	int TextBuffer::CountChar(char c, int start, int end, bool caseInsensitive) const
	{
		if (mStdString.empty())
		{
			return 0;
		}
		int len = (int)mStdString.length();
		if (end < 0) end = len - 1;
		int count = 0;
		if (caseInsensitive)
		{
			std::string lwr = GetLowerStr(mStdString);
			c = tolower(c);
			for (int i = start; i <= end && i < len; i++)
			{
				if (lwr[i] == c) ++count;
			}
		}
		else
		{
			for (int i = start; i <= end && i < len; i++)
			{
				if (mStdString[i] == c) ++count;
			}
		}
		return count;
	}

} // namespace gpvulc

