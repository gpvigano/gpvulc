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
/// @file TextBuffer.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <string>
#include <vector>

namespace gpvulc
{

	/// @addtogroup Text
	/// @{

	/*!
	Text buffer based on a standard C++ string.
	This class embeds a standard C++ string and provides some
	basic editing functions.
	Some methods can be called with a caseInsensitive flag: if true this
	means that search and comparison operations will be performed ignoring
	string upper or lower case.
	*/
	class TextBuffer
	{

	public:

		/// @name Constructors & destructor
		//@{

		//! Default constructor: builds an empty string.
		TextBuffer() {}

		//! Construct a TextBuffer allocating dim characters and setting them to the given character (default=0) if "init" is true.
		TextBuffer(int len, bool init, char c = 0);

		/*!
		 Construct a TextBuffer from the given integer value.
		 @note <b>When using constructors with a pointer parameter pass nullptr instead of nullptr, otherwise this constructor is called</b>
		*/
		TextBuffer(int val) { Set(val); }

		//! Construct a TextBuffer from the given long value.
		TextBuffer(long val) { Set(val); }

		//! Construct a TextBuffer from the given float value.
		TextBuffer(float val) { Set(val); }

		//! Construct a TextBuffer from the given double value.
		TextBuffer(double val) { Set(val); }

		//! Construct a TextBuffer from the given character.
		TextBuffer(char val) { Set(val); }

		/*!
		Construct a TextBuffer from the given C string.
		@note <b>For a null pointer pass @c nullptr instead of 0 (or @c NULL), otherwise @c int constructor is called</b>
		*/
		TextBuffer(const char* str) { Set(str); }

		//! Construct a TextBuffer and copy the given C string (null characters included) using the given length (len)
		explicit TextBuffer(const char* buf, int len) { Set(buf, len); }

		//! Construct a TextBuffer using another TextBuffer (const TextBuffer& copy constructor)
		TextBuffer(const TextBuffer& val) { Set(val); }

		/*!
		Construct a TextBuffer using another string.
		*/
		TextBuffer(const std::string& val) { Set(val); }

		/*!
		Construct a TextBuffer using a void pointer.
		@note <b>For a null pointer pass @c nullptr instead of 0 (or @c NULL), otherwise @c int constructor is called</b>
		*/
		explicit TextBuffer(const void* p) { Set((void*)p); }

		//@}


		/// Polymorphic methods to assign different types of values to the text
		/// @name Assignment methods
		//@{

		TextBuffer& Set(const std::string& s);
		TextBuffer& Set(const std::string& s, size_t chars);
		TextBuffer& Set(const TextBuffer& s);
		TextBuffer& Set(const char* str, size_t chars = 0);
		TextBuffer& Set(char c, int count);
		TextBuffer& Set(char c) { mStdString = c; return *this; }
		TextBuffer& Set(int n, int digits = 0, bool zeroes = false);
		TextBuffer& Set(short n, int digits = 0, bool zeroes = false) { return Set((int)n, digits, zeroes); }
		TextBuffer& Set(unsigned int n, int digits = 0, bool zeroes = false);
		TextBuffer& Set(float f, int width = 0, int ndecimals = -1, bool zeroes = false);
		TextBuffer& Set(double d, int width = 0, int ndecimals = -1, bool zeroes = false);
		TextBuffer& Set(long n);
		TextBuffer& Set(long long n);
		TextBuffer& Set(long double n) { mStdString = std::to_string(n); return *this; }
		TextBuffer& Set(unsigned long long n) { mStdString = std::to_string(n); return *this; }
		TextBuffer& Set(const void* p);

		//@}


		/// Polymorphic operators to assign different types of values to the text
		/// @name Assignment operators
		//@{

		TextBuffer& operator =(char* str) { Set(str); return *this; }
		TextBuffer& operator =(const char* str) { Set(str); return *this; }
		TextBuffer& operator =(const TextBuffer& s) { Set(s); return *this; }
		TextBuffer& operator =(const std::string& s) { Set(s); return *this; }
		TextBuffer& operator =(int i) { Set(i); return *this; }
		TextBuffer& operator =(short i) { Set(i); return *this; }
		TextBuffer& operator =(long i) { Set(i); return *this; }
		TextBuffer& operator =(char c) { Set(c); return *this; }
		TextBuffer& operator =(float f) { Set(f); return *this; }
		TextBuffer& operator =(double d) { Set(d); return *this; }
		TextBuffer& operator =(const void* p) { Set(p); return *this; }

		//@}


		/// Concatenate this string with a string, number (as string), etc.
		/// For numbers @c digits and @c precision can be specified,
		/// if @c zeros is true '0' will be used instead of ' ' to fill empty spaces.
		/// For strings you can specify with @c chars a (minimum) number of spaces used by the string.
		/// @name Concatenation
		//@{

		TextBuffer& Cat(const std::string& s, size_t chars = 0);
		TextBuffer& Cat(const TextBuffer& s, size_t chars = 0) { return Cat(s.Get(), chars); }
		TextBuffer& Cat(const char* str, size_t chars = 0);
		TextBuffer& Cat(char c, int count);
		TextBuffer& Cat(char c) { mStdString += c; return *this; }
		TextBuffer& Cat(int n, int digits = 0, bool zeroes = false);
		TextBuffer& Cat(short n, int digits = 0, bool zeroes = false) { return Cat((int)n, digits, zeroes); }
		TextBuffer& Cat(unsigned int n, int digits = 0, bool zeroes = false);
		TextBuffer& Cat(float f, int width = 0, int ndecimals = -1, bool zeroes = false);
		TextBuffer& Cat(double d, int width = 0, int ndecimals = -1, bool zeroes = false);
		TextBuffer& Cat(long n) { mStdString += std::to_string((long long)n); return *this; }
		TextBuffer& Cat(long long n) { mStdString += std::to_string(n); return *this; }
		TextBuffer& Cat(long double n) { mStdString += std::to_string(n); return *this; }
		TextBuffer& Cat(unsigned long long n) { mStdString += std::to_string(n); return *this; }
		TextBuffer& Cat(const void* p);

		//@}


		/// Overridden streaming operators.
		/// Concatenate this string with a string, number (as string), etc.
		/// @name Streaming operators
		//@{

		TextBuffer& operator <<(const std::string& s) { mStdString += s; return *this; }
		TextBuffer& operator <<(const TextBuffer& s) { Cat(s); return *this; }
		TextBuffer& operator <<(const char* str) { Cat(str); return *this; }
		TextBuffer& operator <<(int i) { Cat(i); return *this; }
		TextBuffer& operator <<(short i) { Cat(i); return *this; }
		TextBuffer& operator <<(unsigned int i) { Cat(i); return *this; }
		TextBuffer& operator <<(char c) { Cat(c); return *this; }
		TextBuffer& operator <<(float f) { Cat(f); return *this; }
		TextBuffer& operator <<(double d) { Cat(d); return *this; }
		TextBuffer& operator <<(const void* p) { Cat(p); return *this; }

		//@}


		/// Overridden increment operators.
		/// Concatenate this string with a string, number (as string), etc.
		/// @name Concatenation operators
		//@{

		TextBuffer& operator +=(const std::string& s) { Cat(s); return *this; }
		TextBuffer& operator +=(const TextBuffer& s) { Cat(s); return *this; }
		TextBuffer& operator +=(const char *s) { Cat(s); return *this; }
		TextBuffer& operator +=(int val) { Cat(val); return *this; }
		TextBuffer& operator +=(short val) { Cat(val); return *this; }
		TextBuffer& operator +=(unsigned int val) { Cat(val); return *this; }
		TextBuffer& operator +=(char val) { Cat(val); return *this; }
		TextBuffer& operator +=(float val) { Cat(val); return *this; }
		TextBuffer& operator +=(double val) { Cat(val); return *this; }

		//@}

		/// Compare this string with another string (both TextBuffer and char* versions).
		/// @note Null char pointers and empty strings are considered the same.
		/// Compare functions return 0 if strings are equal, >0 if @c this>@c str and <0 if @c this<@c str.
		/// @name Comparison
		//@{

		int Compare(const char* str, bool caseInsensitive = false) const { return str ? Compare(std::string(str), caseInsensitive) : (mStdString.empty() ? 0 : 1); }
		int Compare(const std::string& str, bool caseInsensitive = false) const;

		bool EqualTo(const char* str, bool caseInsensitive = false) const { return Compare(str, caseInsensitive) == 0; }
		bool EqualTo(const std::string& str, bool caseInsensitive = false) const { return Compare(str, caseInsensitive) == 0; }

		//@}

		/// Compare this string with another string value.
		/// @name Comparison operators
		//@{

		bool operator ==(const char* str) const { return str!=nullptr ? mStdString == str : mStdString.empty(); }
		bool operator ==(const std::string& str) const { return mStdString == str; }
		bool operator ==(const TextBuffer& str) const { return mStdString == str.StdString(); }

		bool operator !=(const char* str) const { return str!=nullptr ? mStdString != str : !mStdString.empty(); }
		bool operator !=(const std::string& str) const { return mStdString != str; }
		bool operator !=(const TextBuffer& str) const { return mStdString != str.StdString(); }

		bool operator <(const char* str) const { return str ? mStdString.compare(str) < 0 : false; }
		bool operator <(const std::string& rString) const { return mStdString < rString; }
		bool operator <(const TextBuffer& str) const { return mStdString < str.StdString(); }

		bool operator >(const char* str) const { return str ? mStdString.compare(str) > 0:!mStdString.empty(); }
		bool operator >(const std::string& rString) const { return mStdString > rString; }
		bool operator >(const TextBuffer& str) const { return mStdString > str.StdString(); }

		bool operator <=(const char* str) const { return str ? mStdString.compare(str) <= 0 : false; }
		bool operator <=(const std::string& rString) const { return mStdString <= rString; }
		bool operator <=(const TextBuffer& str) const { return mStdString <= str.StdString(); }

		bool operator >=(const char* str) const { return str ? mStdString.compare(str) >= 0:!mStdString.empty(); }
		bool operator >=(const std::string& rString) const { return mStdString >= rString; }
		bool operator >=(const TextBuffer& str) const { return mStdString >= str.StdString(); }

		//@}


		//! Test if the string begins with the given string.
		bool StartsWith(const std::string& str, bool caseInsensitive = false) const;


		//! Test if the string begins with the given string, if true cut off that string.
		bool StartsWithThenCut(const std::string& str, bool caseInsensitive = false);


		//! Test if the string ends with the given string.
		bool EndsWith(const std::string& str, bool caseInsensitive = false) const;


		/*!
		 Test if the string contains the given string at the given position.
		 @param str Search string
		 @param beg Begin to search from this position
		 @param caseInsensitive Perform a case-insensitive search
		*/
		bool MiddleStr(const std::string& str, int beg, bool caseInsensitive = false) const;

		//@}


		/// @name Conversion to number
		//@{

		//! Convert to int (return the given default value on error).
		int GetInt(int def_val = 0);

		//! Convert to double (return the given default value on error).
		double GetDouble(double def_val = 0.0);

		//! Convert to double (return the given default value on error).
		float GetFloat(float def_val = 0.0);

		//! Convert to int (return the given default value on error).
		unsigned GetHex(unsigned def_val = 0x0U);

		//@}


		/// Access to the buffer and its elements
		/// @name Access operators
		//@{

		//! Access the character in the string with the given index.
		char& operator [](size_t idx) { return mStdString[idx]; }

		//! Access the character in the string with the given index.
		const char& operator [](size_t idx) const { return mStdString[idx]; }

		//! Automatic conversion to std::string.
		operator std::string() const { return mStdString; }

		//@}


		/// Access to the buffer
		/// @name Access methods
		//@{

		//! Get a const char* pointer to the internal string.
		const char* Ptr() const { return mStdString.empty() ? nullptr : mStdString.c_str(); }

		/*!
		 Return the character array, if not initialized return "" instead of nullptr.
		 @remark The return value is read-only, if you want to change it you must
		 access the characters using the operator [] in this class
		*/
		const char* Get() const { return mStdString.empty() ? "" : mStdString.c_str(); }

		//! Return this as standard C++ string (constant reference).
		const std::string& StdString() const { return mStdString; }

		/*!
		Transfer the internal string to the given one (move semantics),
		the internal string is empty after this method is called.
		*/
		void Transfer(std::string& outString);

		//@}

		/// Text editing methods, returning a reference to this object.
		/// @name Editing
		//@{

		//! Clear the string
		TextBuffer& Clear() { mStdString.clear(); return *this; }

		/*!
		 Fill the string (or part of it) with characters.
		 @param start beginning position
		 @param end ending position
		 @param c character used to fill the substring
		 @details Fill the string in the given range with the given character.
		 If start is negative it starts from 0.
		 If end is over the text length the internal string is expanded and filled.
		*/
		TextBuffer& Fill(int start, int end, char c);

		/*!
		 Cut off the beginning and/or the end of the string.
		 @param startstr set of beginning edge characters: cut off the initial string composed only of those characters (space and tabulation by default). If empty ("") the beginning of the string is not changed.
		 @param endstr set of ending edge characters: cut off the ending string composed only of those characters. If empty ("") the end of the string is not changed.
		*/
		TextBuffer& Crop(const std::string& startstr, const std::string& endstr);

		/*!
		 Cut off the beginning and/or the end of the string.
		 @param str set of edge characters: cut off the initial and ending string composed only of those characters (space and tabulation by default).
		*/
		TextBuffer& Crop(const std::string& str = " \t") { return Crop(str, str); }

		/*!
		 Erase part of the string given a range.
		 @param start beginning character position
		 @param end end position (included). If -1 (default) it is deleted until the end of the string
		*/
		TextBuffer& Erase(int start, int end = -1);

		/*!
		 Get a substring beginning and ending at given positions.
		 @param beg beginning position
		 @param end ending position. -1 means: "till the end" (this is the default)
		*/
		std::string GetSubString(int beg, int end = -1) const;

		/*!
		 Cutoff the beginning and/or the end of the string.
		 @param beg beginning position
		 @param end end position (included). If -1 (default) do not change the end of the string
		*/
		TextBuffer& SubString(int beg, int end = -1);

		/*!
		 Clip characters at the beginning and/or the end of the string.
		 @param beg number of characters to be clipped at the beginning
		 @param end number of characters to be clipped at the end
		*/
		TextBuffer& ClipChars(int beg, int end);

		//! Insert a string at the given position.
		TextBuffer& Insert(int pos, const std::string& str);

		/*!
		 Insert a character at the given position.
		 @param pos insert position
		 @param ch character to insert
		 @param count repeat the character @c count times (default=1)
		*/
		TextBuffer& InsertChar(int pos, char ch, int count = 1);

		//! Replace part of the string (starting at the given character) with the given string.
		TextBuffer& ReplaceAt(int pos, const std::string& str);

		//! Remove the last character (if this text buffer is not empty).
		TextBuffer& RemoveLastChar();

		//@}


		/// Text find and replace methods, returning a boolean or an integer.
		/// @name Find & replace
		//@{

		/*!
		 Replace all the occurrences of the given character with the given one.
		 @param oldchar character to be replaced
		 @param newchar character to replace
		 @param caseInsensitive Perform a case-insensitive search (default=false)
		 @param startpos start searchin at this position (default=0)
		 @param endpos end searching at this position (default=0)
		 @return The number of the replaced characters or 0 if not found.
		*/
		int ReplaceChar(char oldchar, char newchar,
			bool caseInsensitive = false, int startpos = 0, int endpos = -1);

		/*!
		 Replace a substring (if present) with the given string.
		 @param substr string to be replaced
		 @param str string to replace
		 @param caseInsensitive Perform a case-insensitive search (default=false)
		 @param words_only match whole words only (default=false)
		 @param startpos start searching at this position (default=0)
		 @return The index of the replaced string or -1 if not found.
		*/
		int Replace(const std::string& substr, const std::string& str,
			bool caseInsensitive = false, bool words_only = false, int startpos = 0);

		/*!
		 Replace all occurrences of a substring (if present) with the given string.
		 @param substr string to be replaced
		 @param str string to replace
		 @param caseInsensitive Perform a case-insensitive search (default=false)
		 @param words_only match whole words only (default=false)
		 @param startpos start searching at this position (default=0)
		 @return The number of replaced strings or 0 if not found.
		*/
		int ReplaceAll(const std::string& substr, const std::string& str,
			bool caseInsensitive = false, bool words_only = false, int startpos = 0);

		/*!
		 Find a substring.
		 @return the index of the beginning character or -1 if not found
		*/
		int FindSubString(const std::string& str, bool caseInsensitive = false, bool words_only = false, int startpos = 0) const;

		/*!
		 Find a substring searching from the end.
		 @return the index of the beginning character or -1 if not found
		*/
		int FindRevSubString(const std::string& str, bool caseInsensitive = false, bool words_only = false, int startpos = -1) const;

		/*!
		 Find any of the given substrings.
		 @return the index of the beginning character or -1 if not found
		*/
		int FindSubStringAny(const std::vector<std::string>& strList, bool caseInsensitive = false, bool words_only = false, int startpos = 0) const;

		/*!
		 Find any of the given substrings searching from the end.
		 @return the index of the beginning character or -1 if not found
		*/
		int FindRevSubStringAny(const std::vector<std::string>& strList, bool caseInsensitive = false, bool words_only = false, int startpos = -1) const;

		//! Reach the position of the first occurrence of one of the given characters (return -1 if not found).
		int FindFirstOf(const std::string& reachstr, int start = 0, bool caseInsensitive = false) const;

		//! Reach the position of the last occurrence of one of the given characters (return -1 if not found).
		int FindLastOf(const std::string& reachstr, int start = -1, bool caseInsensitive = false) const;

		//! Find the position of the first occurrence of the given character (return -1 if not found).
		int FindChar(char chr, int start = 0, bool caseInsensitive = false) const;

		//! Find the position of the last occurrence of one given character (return -1 if not found).
		int FindLastChar(char chr, int start = -1, bool caseInsensitive = false) const;

		//! Find the position of the first occurrence of a character not in the given string (return -1 if not found).
		int FindFirstNotOf(const std::string& str, int start = 0, bool caseInsensitive = false) const;

		//! Find the position of the last occurrence of a character not in the given string (return -1 if not found).
		int FindLastNotOf(const std::string& str, int start = -1, bool caseInsensitive = false) const;

		/*!
		 Test if the string contains the given string.
		 @param str Search string
		 @param caseInsensitive Perform a case-insensitive search
		*/
		bool Contains(const std::string& str, bool caseInsensitive = false) const;

		/*!
		 Test if the string contains <B>one or more</B> characters from the given string.
		 @param str String of the characters that should belong to the string
		 @param caseInsensitive Case-insensitive comparison
		*/
		bool Has(const std::string& str, bool caseInsensitive = false) const;


		/*!
		Split a string into substrings separated by the given delimiter.
		@param delimiter Delimiter character.
		@param removeEmpty Specifies whether to remove empty elements (default=false).
		@return a string array that contains the substrings in this string that are delimited the given character.
		*/
		std::vector<std::string> Split(char delimiter, bool removeEmpty = false) const;


		/*!
		Split a string into substrings separated by one of the given delimiters.
		@param delimiters vector of delimiters.
		@param removeEmpty Specifies whether to remove empty elements (default=false).
		@return a string array that contains the substrings in this string that are delimited by elements in a specified string.
		*/
		std::vector<std::string> Split(const std::string& delimiters, bool removeEmpty = false) const;


		/*!
		Split a string into substrings separated by the given delimiter string.
		@param delimiterString delimiter string.
		@param removeEmpty Specifies whether to remove empty elements (default=false).
		@return a string array that contains the substrings in this string that are delimited by elements in a specified string.
		*/
		std::vector<std::string> SplitStr(const std::string& delimiterString, bool removeEmpty = false) const;

		//@}


		/// Analyze the text to get information.
		/// @name Text information
		//@{

		//! Ckeck if the sting is empty.
		bool IsEmpty() const { return mStdString.empty(); }

		//! Count the lines in the text buffer.
		int CountLines() const;

		/*!
		 Test if the string is made of <B>only</B> the characters from the given string.
		 @param str String of the only characters expected to belong to the string
		 @param caseInsensitive Case-insensitive comparison
		*/
		bool MadeOf(const std::string& str, bool caseInsensitive = false) const;

		/*!
		 Count the occurrences of the given character in the given range.
		 @param c character that must be found
		 @param start First position in the search range (default is 0)
		 @param end Last position in the search range (included), if<0 (default) search until the end of the string
		 @return the occurrences of the given character in the given range (0 if the string is empty).
		 @param caseInsensitive Perform a case-insensitive search
		*/
		int CountChar(char c, int start = 0, int end = -1, bool caseInsensitive = false) const;

		//! Test if a character in the string is alphanumeric.
		bool IsAlNum(int idx) const;

		//! Test if a character in the string is alphanumeric or underscore ('_').
		bool IsAlNum_(int idx) const;

		//! Test if a character in the string is a space, tabulation, carriage return or newline (' ','\\t','\\r','\\n').
		bool IsSpace(int idx) const;

		//! Return the length (characters) of the string until the terminator (0).
		int Length() const
		{
			size_t pos = mStdString.find('\0');
			return pos == std::string::npos ? (int)mStdString.size() : (int)pos;
		}

		//@}

		/// Text case management
		/// @name Case management
		//@{

		//! Return the lower case version of the string without changing the original one.
		std::string GetLowerCase() const;

		//! Set and return the lower case version of the string.
		TextBuffer& LowerCase();

		//! Return the upper case version of the string without changing the original one.
		std::string GetUpperCase() const;

		//! Set and return the upper case version of the string.
		TextBuffer& UpperCase();

		//! Return the Sentence case version of the string without changing the original one.
		std::string GetSentenceCase() const;

		//! Set and return the Sentence case version of the string.
		TextBuffer& SentenceCase();

		//! Return the Proper Case version of the string without changing the original one.
		std::string GetProperCase() const;

		//! Set and return the Proper Case version of the string.
		TextBuffer& ProperCase();

		//! Set to lower the i-th character.
		TextBuffer& ToLower(int i);

		//! Set to upper the i-th character.
		TextBuffer& ToUpper(int i);

		//! Check if the character at the given position is upper case (false also on error).
		bool IsUpper(int i) const;

		//! Check if the character at the given position is lower case (false also on error).
		bool IsLower(int i) const;

		//@}

		/// Modify text indentation.
		/// @name Indentation
		//@{

		/*!
		 Get the string with the given indentation.
		 @param n number of characters
		 @param c character to use (default=' ')
		*/
		std::string GetIndented(int n, char c = ' ') const;

		/*!
		 Get the string "unindented".
		 @param n number of characters (default is 0 => the least indented line will be used as reference)
		 @param c character to use (default=' ')
		*/
		std::string GetUnindented(int n = 0, char c = ' ') const;

		/*!
		 Indent the string.
		 @param n number of characters
		 @param c character to use (default=' ')
		*/
		TextBuffer& Indent(int n, char c = ' ');

		/*!
		 Unindent the string.
		 @param n number of characters (default is 0 => the least indented line will be used as reference)
		 @param c character to use (default=' ')
		*/
		TextBuffer& Unindent(int n = 0, char c = ' ');

		//@}


		/// Conversion to C-compliant identifiers
		/// @name C-identifiers
		//@{

		//! Return a this string converted into a C identifier (alphanumeric+'_', not starting with a digit).
		std::string GetCid() const;

		//! Convert this string into a C identifier (alphanumeric+'_', not starting with a digit).
		TextBuffer& MakeCid();

		//@}


		/// Memory management
		/// @name Memory
		//@{

		//! Allocate dim characters (+1 extra for null character) for this string object (no parameter=fit to current length).
		bool Resize(int dim = -1);

		//! Call ShrinkToFit() only if the given dimension is greater than the current one.
		bool Reserve(int dim);

		//! Return the size of the string in characters (extra null character included).
		int GetSize() { return (int)mStdString.size(); }

		//@}


		/// Read and write from/to a file or a stream
		/// @name File/stream
		//@{

		//! Read a text from an input stream.
		bool Read(std::istream& inStream);

		//! Read a line from a stream and store it in this string.
		bool ReadLine(std::istream& inStream, bool appendNewline = false, bool appendEofNewline = false);

		//! Read a line from a stream and appent it to this string.
		bool AppendLine(std::istream& inStream, bool appendNewline = false, bool appendEofNewline = false);

		//! Read from a stream a line split into multiple lines with backslash and store it in this string.
		bool ReadSplitLine(std::istream& inStream, bool skipSpaces = true, bool appendNewline = false, bool appendEofNewline = false);

		//! Write the text to an output stream.
		bool Write(std::ostream& ostrm) const;

		/*!
		Load a text from a file and store it in this string.
		@param filename name of the file to open
		@param append if true the loaded text will be appended to this (default=false)
		@return false on error.
		*/
		bool Load(const std::string& filename, bool append = false);

		/*!
		Save the text to a file.
		@param filename name of the file to open
		@param append if true the string will be appended to the content of the file (default=false)
		@return false on error.
		*/
		bool Save(const std::string& filename, bool append = false) const;

		//@}

	protected:

		//! Internal string buffer.
		std::string mStdString = "";

		//! Convert an unsigned index to a signed integer, string::npos is converted to -1.
		int PosToInt(size_t pos) const { return (pos == std::string::npos) ? -1 : (int)pos; }

		/*!
		Convert an integer index to an unsigned index, negative values are converted to the first position (or the last if rev is true).
		 @param pos position from the beginning of the text (or from the end if rev is true).
		 @param[out] search_pos The resulting position inside the text (left unchanged if the method returns false).
		 @param rev If rev is true consider pos as the offset from the end of the text backward.
		 @return false if the text is empty or if pos is greater than the text length.
		*/
		bool IntToPos(int pos, size_t& search_pos, bool rev) const;
	};

	///@}

}//namespace gpvulc


/// @addtogroup Text
/// @{

//! Concatenate a TextBuffer with another one.
gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const gpvulc::TextBuffer& s);

//! Concatenate a TextBuffer with a C++ string.
gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const std::string& s);

//! Concatenate a C++ string with a TextBuffer.
gpvulc::TextBuffer operator +(const std::string& s, const gpvulc::TextBuffer& txt);

//! Concatenate a TextBuffer with a C string.
gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const char* s);

//! Concatenate a C string with a TextBuffer.
gpvulc::TextBuffer operator +(const char* s, const gpvulc::TextBuffer& txt);

//! Concatenate a TextBuffer with a void pointer (as string).
gpvulc::TextBuffer operator +(const gpvulc::TextBuffer& txt, const void* s);

//! Concatenate a void pointer (as string) with a TextBuffer.
gpvulc::TextBuffer operator +(const void* s, const gpvulc::TextBuffer& txt);


//! Output stream operator (call the StdString() method).
std::ostream& operator << (std::ostream& strm, const gpvulc::TextBuffer& txt);


//! Input stream operator (append the read text).
std::istream& operator >> (std::istream& strm, gpvulc::TextBuffer& txt);

///@}


