//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Text parser
/// @file TextParser.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <gpvulc/text/TextBuffer.h>

#include <map>

namespace gpvulc
{

	/// @addtogroup Text
	/// @{

	/*!
	Text parser.
	Text parser with support for case insensitive comparison,
	quoted text and C++ comments preservations, nested blocks, etc.
	Parsing methods return true if the requested feature is found, false otherwise.
	To get the result of the last parsing operation call the Result() method.
	*/
	class TextParser
	{

	public:

		//! Default constructor
		TextParser();

		//! Constructor that fills the internal text buffer
		TextParser(const std::string& text);

		/// Set and get parser parameters.
		///@name Parser settings
		//@{

		//! Set current default separators (initially set to " \t\n\r")
		void SetDefaultSeparators(const std::string& sep) { mSeparators = sep; }

		//! Get current default separators
		const std::string& GetDefaultSeparators() { return mSeparators; }

		//! Set case sensitivity
		void SetCaseInsensitive(bool cs = true) { mCaseInsensitive = cs; }

		//! Get case sensitivity
		bool GetCaseInsensitive() { return mCaseInsensitive; }

		//! Tell the parser whether to manage the quoted text ("...") during the block serching phase
		void SetQuotedTextIgnored(bool yes = true) { mQuotedTextIgnored = yes; }

		//! Check if the parser ignores the quoted text ("...") during the block serching phase
		bool GetQuotedTextIgnored() { return mQuotedTextIgnored; }

		//! Tell the parser to manage the C++/C comments (//...\n or /*...*/) during the block serching phase
		void SetCppCommentsIgnored(bool yes = true) { mCppCommentsIgnored = yes; }

		//! Chek if the parser ignores the C++/C comments (//...\n or /*...*/) during the block serching phase
		bool GetCppCommentsIgnored() { return mCppCommentsIgnored; }

		//@}


		/// Parse the text and update the current position.
		///@name Parsing
		//@{

		//! Forward of specified steps (characters).
		bool Forward(int steps = 1);

		//! Backward of specified characters.
		bool Backward(int steps = 1);

		/*!
		Compare the remainder with the first n characters of the given string
			(or the entire string if n is omitted).
		@return true if the remainder <B>starts</B> with the given (sub)string.
		*/
		bool Compare(const std::string& s, int n = -1) const;

		/*!
		Compare the remainder with the given strings.
		@return true if the remainder <B>starts</B> with one of the given strings.
		*/
		bool CompareList(const std::vector<std::string>& referenceStrings) const;

		/*!
		Compare the remainder with the given string + one of the given characters.
		@return true if the remainder <B>starts</B> with the given (sub)string followed by one of the characters from the given string.
		*/
		bool CompareStrChr(const std::string& s, const std::string& choice) const;

		//! Test if the parsing has reached the end of the text.
		bool Complete() const;

		/*!
		Extract a block delimited by tags.
		Nested blocks are found at the given level (1 = first level, default).
		*/
		bool GetBlock(const std::string& begintag, const std::string& endtag,
			int level = 1);

		/*!
		Extract a block delimited by tags (searching back).
		Nested blocks are found at the given level (1 = first level, default).
		*/
		bool GetBackBlock(const std::string& begintag, const std::string& endtag,
			int level = 1);

		/*!
		Get a block after a given label.
		@details Extract a block delimited by tags following a given label.
		Nested blocks are found at the given level.
		*/
		bool GetBlockAfter(const std::string& label, const std::string& begintag,
			const std::string& endtag, int level = 1);

		//! Get the string until the given tag
		bool GetField(const std::string& separator);

		//! Get a line (characters until the newline), skip the newline character.
		bool GetLine();

		/*! Get the part from the current position to the end.
		@note The result is stored (see Result()) and the parsing is then complete (see Complete()).
		*/
		bool GetRemainder();

		/*!
		Get a token separated by the given characters
		@details Return a token using the given separator characters (default separator if empty, see SetDefaultSeparators() ).
		If the first incoming character is different from separator characters
		or the parsed text ends before a separator character is encountered the token is returned anyway.
		@return true if found a token. If the parsing reached the end this method returns false.
		*/
		bool GetToken(const std::string& sep = "");

		//! Go beyond the first occurrence of str (store the traversed text), return false if not found.
		bool GoBeyond(const std::string& str);

		//! Reach the first occurrence of reachstr (store the traversed text), return false if not found.
		bool Reach(const std::string& reachstr);

		//! Reach the first occurrence of one of characters in the given string (store the traversed substring), return false if not found
		bool ReachFirstOf(const std::string& reachstr);

		//! Search back the last occurrence of one of characters in the given string (store the traversed substring), return false if not found
		bool ReachLastOf(const std::string& reachstr);

		/*!
		Reach the first occurrence of a string among the parameter list (return the traversed substring)
		@param search_str Vector of strings to search for
		@return Store the skipped text, return false if the string is not found (or if it is the next string)
		*/
		bool ReachFirstAmong(const std::vector<std::string>& search_str);

		//! Skip characters from the given string (returns true if there are skipped characters).
		bool Skip(const std::string& skipstr);

		//! Skip spaces and tabulations (returns true if there are skipped characters).
		bool SkipSpaces() { return Skip(" \t"); }

		//! Undo last command (return false if nothing left to undo).
		bool Undo();

		//! Undo last n commands (return false if the last Undo() failed).
		bool Undo(int times);

		//! Reset parsing data/restart parsing.
		void ResetParsing();

		//@}


		/// Get/set the parser text and status.
		///@name Status
		//@{

		/*!
		Get the initial part, from the beginning to the current position (read-only).
		@note This is a read-only method, it does not affect the current parsing position.
		*/
		const std::string GetParsedText() const;

		/*!
		Get the final part, from the current position to the end (read-only).
		@note This is a read-only method, it does not affect the current parsing position.
		*/
		const std::string GetNotParsedText() const;

		//! Return the offset from the beginning of the parsed text.
		int GetOffset();

		//! Compare the last result with the given string.
		bool ResultIs(const std::string& tag) const;

		//! Return the last extracted string.
		const std::string& Result() const { return mResult; }

		//@}


		/// Access and set the internal text buffer.
		///@name Access
		//@{

		//! Return the internal text buffer as string.
		const std::string& GetText() const { return mInputText.StdString(); }

		//! Return the internal text buffer.
		const TextBuffer& GetTextBuffer() const { return mInputText; }

		//! Read the text from file.
		bool ReadText(std::istream& strm);

		//! Read the line from file.
		bool ReadLine(std::istream& strm, bool appendNewline = false, bool appendEofNewline = false);

		//! Load the whole text from a file.
		bool LoadFile(const std::string& filename);

		//! Save the whole text to a file.
		bool SaveFile(const std::string& filename);

		//! Copy in the internal text buffer the given string.
		void SetText(const std::string& text);

		//! Clear the input text and reset parsing data.
		void Clear();

		//@}


		/// Add bookmarks and get selection.
		///@name Selection
		//@{

		//! Set a bookmark at the current position.
		void SetBookmark(const std::string& name);

		//! Delete a bookmark.
		bool DeleteBookmark(const std::string& name);

		//! Delete all bookmarks.
		void DeleteAllBookmarks();

		//! Move to a bookmark (parsed text is stored).
		bool MoveToBookmark(const std::string& name);

		//! Get a selection of the internal buffer between two bookmarks (parsing not affected).
		std::string GetSelection(const std::string& bookmarkStart, const std::string& bookmarkEnd) const;

		//! Get a selection of the internal buffer between two positions (parsing not affected).
		std::string GetSelection(int posStart, int posEnd) const;

		//@}


	protected:

		// Internal buffer for the whole text
		TextBuffer mInputText;

		// Internal buffer for parsing operations
		TextBuffer mBuffer;

		// Internal buffer for parsing results
		std::string mResult;

		// Separators for tokens
		std::string mSeparators;

		// Current parsing position
		int mCurrPos;

		// History of parsing positions (used by Undo())
		std::vector<int> mUndoPos;

		// Bookmarks for block selection
		std::map<std::string, int> mBookmarks;

		// Case insensitive flag used for search and compare operations
		bool mCaseInsensitive;

		// Flag used to ignore quoted text in text block parsing
		bool mQuotedTextIgnored;

		// Flag used to ignore C/C++ comments in text block parsing
		bool mCppCommentsIgnored;

		// Set up the parser
		void Init();

		// Store the current parsing position in mUndoPos
		void SaveCurrPos();
	};

	///@}

}//namespace gpvulc


