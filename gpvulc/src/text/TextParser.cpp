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
/// @file TextParser.cpp
/// @author Giovanni Paolo Vigano'


#include <gpvulc/text/TextParser.h>

#include <utility>
#include <algorithm>

namespace gpvulc
{

	//---------------------------------------------------------------------
	// TextParser class implementation

	TextParser::TextParser()
	{
		Init();
	}


	TextParser::TextParser(const std::string& text)
	{
		Init();
		SetText(text);
	}


	bool TextParser::Backward(int steps)
	{
		if (mCurrPos < steps) return false;

		mResult = mInputText.GetSubString(mCurrPos - steps, mCurrPos - 1);
		SaveCurrPos();
		mCurrPos -= steps;

		return true;
	}


	bool TextParser::Compare(const std::string& s, int n) const
	{
		std::string subStr = n>0 ? s.substr(0U, (size_t)n) : s;
		if (subStr.empty())
		{
			return Complete();
		}
		TextBuffer temp = mInputText.GetSubString(mCurrPos, mCurrPos + (int)subStr.length() - 1);

		return temp.EqualTo(subStr, mCaseInsensitive);
	}


	bool TextParser::CompareList(const std::vector<std::string>& referenceStrings) const
	{
		TextBuffer temp = mInputText.GetSubString(mCurrPos);
		for(const std::string& s : referenceStrings)
		{
			if (temp.StartsWith(s, mCaseInsensitive))
			{
				return true;
			}
		}
		return false;
	}


	bool TextParser::CompareStrChr(const std::string& s, const std::string& choice) const
	{
		if (s.empty() || choice.empty()) return false;
		TextBuffer temp(mInputText.GetSubString(mCurrPos));

		if (!temp.StartsWith(s, mCaseInsensitive)) return false;
		std::string remainder = temp.GetSubString((int)s.length());
		size_t idx = remainder.find_first_of(choice);
		return idx == 0;
	}


	bool TextParser::Complete() const
	{
		return mCurrPos >= mInputText.Length();
	}

	bool TextParser::Forward(int steps)
	{
		if (steps <= 0) return false;
		if (mCurrPos <= mInputText.GetSize() - steps)
		{
			mResult = mInputText.GetSubString(mCurrPos, mCurrPos + steps - 1);
			SaveCurrPos();
			mCurrPos += steps;
			return true;
		}
		return false;
	}


	bool TextParser::GetBackBlock(
		const std::string& begintag,
		const std::string& endtag,
		int level)
	{
		int blockbegin = 0, blockend = 0;
		int blocklevel = 0;
		bool quoted = false;

		if (begintag.empty() || endtag.empty()) return false;

		for (int i = mCurrPos - 1; i >= 0; --i)
		{
			if (mQuotedTextIgnored)
			{
				if (mInputText[i] == '\"' && mInputText[i - 1] != '\\')
				{
					quoted = !quoted;
					continue;
				}
				if (quoted) continue;
			}
			mBuffer = mInputText.GetSubString(0, i);
			if (mBuffer.EndsWith(endtag))
			{
				i -= (int)endtag.length() - 1;
				++blocklevel;
				if (blocklevel == level && i > 0)
					blockend = i - 1;
			}
			else if (mBuffer.EndsWith(begintag))
			{
				if (blocklevel > 0)
				{
					blockbegin = i + 1;
					i -= (int)begintag.length() - 1;
					if (blocklevel == level)
					{
						SaveCurrPos();
						mCurrPos = i - 1;
						mResult = mInputText.GetSubString(blockbegin, blockend);

						return true;
					}
					--blocklevel;
				}
				else i -= (int)begintag.length() - 1;
			}
		}
		SaveCurrPos();
		mCurrPos = 0;

		return false;
	}


	const std::string TextParser::GetParsedText() const
	{
		if (mCurrPos == 0) return "";
		return mInputText.GetSubString(0, mCurrPos - 1);
	}


	const std::string TextParser::GetNotParsedText() const
	{
		if (Complete()) return "";
		return mInputText.GetSubString(mCurrPos);
	}


	bool TextParser::GetBlock(
		const std::string& begintag,
		const std::string& endtag,
		int level)
	{
		int blockbegin = 0, blockend = 0;
		int blocklevel = 0;
		bool quoted = false;
		bool commentedblock = false;
		bool commentedline = false;
		int found = false;

		if (begintag.empty() && endtag.empty())
		{
			return GetRemainder();
		}

		if (begintag.empty())
		{
			blocklevel = 1;
			blockbegin = mCurrPos;
		}

		int i = mCurrPos;
		while (i < mInputText.GetSize() && mInputText[i] != 0)
		{
			//*********************************************
			if (mQuotedTextIgnored && !(mCppCommentsIgnored && (commentedline || commentedblock)))
			{
				if (mInputText[i] == '\"')
				{
					bool accepted = true;
					// check exceptions
					for (int j = i - 1; j > 0; --j)
					{
						if (mInputText[j] != '\\') break;
						accepted = !accepted;
					}
					if (accepted)
					{
						quoted = !quoted;
						++i;
						continue;
					}
				}
			}
			//*********************************************
			if (mCppCommentsIgnored)
			{
				// detect single line comment (like this)
				if (!commentedline && i < mInputText.GetSize() - 1
					&& mInputText[i] == '/' && mInputText[i + 1] == '/')
				{
					commentedline = true;
					i += 2;
					continue;
				}
				if (commentedline)
				{
					if (mInputText[i] == '\n') commentedline = false;
					++i;
					continue;
				}
				// detect C-style commented block
				if (!commentedblock && i < mInputText.GetSize() - 1
					&& mInputText[i] == '/' && mInputText[i + 1] == '*')
				{
					commentedblock = true;
					i += 2;
					continue;
				}
				if (commentedblock)
				{
					if (i < mInputText.GetSize() - 1 && mInputText[i] == '*' && mInputText[i + 1] == '/')
					{
						++i;
						commentedblock = false;
					}
					++i;
					continue;
				}
			}
			//*********************************************
			// detect block end
			if (mInputText.MiddleStr(endtag, i) && blocklevel > 0)
			{
				//printf("GetBlock: found endtag %s at %d\n",endtag.c_str(),i);
				blockend = i - 1;
				i += (int)endtag.length();
				if (blocklevel == level || level == 0)
				{
					// block at the given level found, return the block
					SaveCurrPos();
					mCurrPos = i;
					mResult = mInputText.GetSubString(blockbegin, blockend);

					return true;
				}
				--blocklevel;
			}
			// detect block beginning
			else if (!begintag.empty() && mInputText.MiddleStr(begintag, i) && !(level == 0 && found))
			{
				//printf("GetBlock: found begintag %s at %d\n",begintag.c_str(),i);
				if (endtag.empty())
				{
					mBuffer.Set(mInputText.GetSubString(i));
					mResult = mBuffer.StdString();
					SaveCurrPos();
					mCurrPos = mInputText.Length();

					return true;
				}
				i += (int)begintag.length();
				++blocklevel;
				if (blocklevel == level) blockbegin = i;
				if (level == 0)
				{
					blockbegin = i;
					found = true;
				}
			}
			else ++i;
		}

		return false;
	}


	bool TextParser::GetBlockAfter(
		const std::string& label,
		const std::string& begintag,
		const std::string& endtag,
		int level)
	{
		int idx = mInputText.FindSubString(label, mCaseInsensitive, false, mCurrPos);
		if (idx < 0) return false;

		int prev_pos = mCurrPos;
		mCurrPos = idx + (int)label.length();
		if (GetBlock(begintag, endtag, level)) return true;
		mCurrPos = prev_pos;
		return false;
	}


	bool TextParser::GetField(const std::string& separator)
	{
		int idx = mInputText.FindSubString(separator, mCaseInsensitive, false, mCurrPos);
		if (idx < 0) return false;
		mResult = mInputText.GetSubString(mCurrPos, idx - 1);

		SaveCurrPos();
		mCurrPos = idx + (int)separator.length();

		return true;
	}


	int TextParser::GetOffset()
	{
		return mCurrPos;
	}


	bool TextParser::GetLine()
	{
		int idx = mInputText.FindChar('\n', mCurrPos);
		if (idx < 0) return GetRemainder();

		mResult = mInputText.GetSubString(mCurrPos, idx - 1);
		SaveCurrPos();
		mCurrPos = idx + 1;

		return true;
	}


	bool TextParser::GetRemainder()
	{
		if (Complete()) return false;
		mResult = mInputText.GetSubString(mCurrPos);
		SaveCurrPos();
		mCurrPos = mInputText.Length();

		return true;
	}


	bool TextParser::GetToken(const std::string& sep)
	{
		if (Complete()) return false;

		std::string separators = sep;
		if (sep.empty()) separators = mSeparators;

		// skip heading separators
		int startPos = mInputText.FindFirstNotOf(separators, mCurrPos, mCaseInsensitive);
		if (startPos < 0)
		{
			mResult.clear();
			SaveCurrPos();
			mCurrPos = mInputText.Length();
			return false;
		}

		// find token end
		int endPos = mInputText.FindFirstOf(separators, startPos, mCaseInsensitive);


		if (endPos < 0)
		{
			mResult = mInputText.GetSubString(startPos);
			endPos = mInputText.Length();
		}
		else
		{
			mResult = mInputText.GetSubString(startPos, endPos - 1);
			// skip trailing separators
			int sepend = mInputText.FindFirstNotOf(separators, endPos, mCaseInsensitive);
			if (sepend >= 0)
			{
				endPos = sepend;
			}

		}

		SaveCurrPos();
		mCurrPos = endPos;

		return true;
	}


	bool TextParser::GoBeyond(const std::string& str)
	{
		if (str.empty()) return false;
		int idx = mInputText.FindSubString(str, mCaseInsensitive, false, mCurrPos);
		if (idx < 0) return false;

		mResult = mInputText.GetSubString(mCurrPos, idx + (int)str.length() - 1);
		SaveCurrPos();
		mCurrPos = idx + (int)str.length();

		return true;
	}


	void TextParser::ResetParsing()
	{
		mCurrPos = 0;
		mUndoPos.clear();
		//for (size_t i=0; i<mUndoPos.size(); ++i) mUndoPos[i] = 0;
		mBookmarks.clear();
	}


	void TextParser::Init()
	{
		ResetParsing();
		mSeparators = " \t\n\r";
		mQuotedTextIgnored = false;
		mCppCommentsIgnored = false;
		mCaseInsensitive = false;
	}


	bool TextParser::ReachFirstOf(const std::string& reachstr)
	{
		int idx = 0;

		mBuffer = mInputText.GetSubString(mCurrPos);
		idx = mBuffer.FindFirstOf(reachstr, 0, mCaseInsensitive);
		if (idx < 0) return false;
		if (idx == 0)
		{
			SaveCurrPos();
			mResult = "";
			return true;
		}

		SaveCurrPos();
		mCurrPos += idx;
		mResult = mBuffer.GetSubString(0, idx - 1);

		return true;
	}


	bool TextParser::ReachLastOf(const std::string& reachstr)
	{
		if (mCurrPos==0) return false;
		int idx = 0;

		idx = mInputText.FindLastOf(reachstr, mCurrPos-1, mCaseInsensitive);
		if (idx < 0) return false;
		if (idx == mCurrPos-1)
		{
			SaveCurrPos();
			mResult = "";
			return true;
		}
		int prevPos = mCurrPos-1;
		SaveCurrPos();
		mCurrPos = idx+1;
		mResult = mInputText.GetSubString(idx+1, prevPos);

		return true;
	}


	bool TextParser::Reach(const std::string& reachstr)
	{
		int idx;

		if (reachstr.empty()) return false;

		idx = mInputText.FindSubString(reachstr, mCaseInsensitive, false, mCurrPos);
		if (idx < 0) return false;
		if (idx == 0) mResult.clear();
		else mResult = mInputText.GetSubString(mCurrPos, idx - 1);
		SaveCurrPos();
		mCurrPos = idx;

		return true;
	}


	bool TextParser::ReachFirstAmong(const std::vector<std::string>& search_str)
	{
		int idx = 0;

		int minpos = INT_MAX;

		mBuffer = mInputText.GetSubString(mCurrPos);
		for (size_t i = 0; i < search_str.size(); ++i)
		{
			idx = mInputText.FindSubString(search_str[i], mCaseInsensitive, false, mCurrPos);
			if (idx < 0) continue;
			if (idx < minpos) minpos = idx;
		}

		if (minpos > mInputText.Length()) return false;
		Forward(minpos - mCurrPos);

		return true;
	}


	bool TextParser::ReadText(std::istream& strm)
	{
		if (!strm.good()) return false;

		Init();
		strm >> mInputText;
		return true;
	}


	bool TextParser::ReadLine(std::istream& strm, bool appendNewline, bool appendEofNewline)
	{
		ResetParsing();
		mInputText.Clear();
		return mInputText.ReadLine(strm, appendNewline, appendEofNewline);
	}


	bool TextParser::LoadFile(const std::string& filename)
	{
		if (filename.empty()) return false;

		Init();
		mInputText.Clear();
		return mInputText.Load(filename);
	}


	bool TextParser::SaveFile(const std::string& filename)
	{
		if (filename.empty()) return false;

		return mInputText.Save(filename);
	}


	void TextParser::Clear()
	{
		ResetParsing();
		mInputText.Clear();
	}


	void TextParser::SetBookmark(const std::string& name)
	{
		mBookmarks[name] = mCurrPos;
	}


	bool TextParser::DeleteBookmark(const std::string& name)
	{
		if (mBookmarks.find(name) == mBookmarks.end())
		{
			return false;
		}
		mBookmarks.erase(name);
		return true;
	}


	void TextParser::DeleteAllBookmarks()
	{
		mBookmarks.clear();
	}


	bool TextParser::MoveToBookmark(const std::string& name)
	{
		if (mBookmarks.find(name) == mBookmarks.end())
		{
			return false;
		}
		if (mResult.empty() && mBookmarks[name] == mCurrPos)
		{
			return true;
		}
		SaveCurrPos();
		if (mBookmarks[name] == mCurrPos)
		{
			mResult = "";
			return true;
		}
		int posStart = std::min(mCurrPos, mBookmarks[name]);
		int posEnd = std::max(mCurrPos, mBookmarks[name]);
		mResult = mInputText.GetSubString(posStart, posEnd-1);
		mCurrPos = mBookmarks[name];
		return true;
	}


	std::string TextParser::GetSelection(const std::string& bookmarkStart, const std::string& bookmarkEnd) const
	{
		if (mBookmarks.find(bookmarkStart) == mBookmarks.end() || mBookmarks.find(bookmarkEnd) == mBookmarks.end())
		{
			return "";
		}
		return GetSelection(mBookmarks.at(bookmarkStart),mBookmarks.at(bookmarkEnd));
	}


	std::string TextParser::GetSelection(int posStart, int posEnd) const
	{
		if (posEnd - 1 <= posStart) return "";
		return mInputText.GetSubString(posStart,posEnd-1);
	}


	void TextParser::SaveCurrPos()
	{
		mUndoPos.push_back(mCurrPos);
	}



	void TextParser::SetText(const std::string& text)
	{
		ResetParsing();
		mInputText = text;
	}


	bool TextParser::Skip(const std::string& skipstr)
	{
		if (skipstr.empty()) return false;
		int pos = mInputText.FindFirstNotOf(skipstr, mCurrPos);
		if (pos < 0)
		{
			return GetRemainder();
		}
		if (mCurrPos == pos)
		{
			mResult.clear();
			return false;
		}

		mResult = mInputText.GetSubString(mCurrPos, pos - 1);

		SaveCurrPos();
		mCurrPos = pos;

		return true;
	}


	bool TextParser::ResultIs(const std::string& tag) const
	{
		if(mCaseInsensitive)
		{
			TextBuffer temp = tag;
			return temp.EqualTo(mResult, true);
		}
		return mResult == tag;
	}


	bool TextParser::Undo()
	{
		if (mUndoPos.empty()) return false;
		mCurrPos = mUndoPos.back();
		mUndoPos.pop_back();
		return true;
	}


	bool TextParser::Undo(int times)
	{
		for (int i = 0; i < times; ++i) if (!Undo()) return false;
		return true;
	}

}

