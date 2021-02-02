//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief File path information
/// @file PathInfo.cpp
/// @author Giovanni Paolo Vigano'

#include <gpvulc/path/PathInfo.h>

#include <algorithm>
#include <regex>


#ifdef _MSC_VER

// BSD/UNIX specific functions aliases for Visual C++.
#include <direct.h>
#include <stdlib.h>
#define getcwd _getcwd

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#else
#include <unistd.h>
#include <dirent.h>
#endif


namespace
{

	// Check if the colon character is present.
	inline bool HasColon(const std::string& p)
	{
		return p.find(':') != std::string::npos;
	}
}


namespace gpvulc
{

	//-----------------------------------------------------------------------------
	// PathInfo implementation


#ifdef _WIN32
	char PathInfo::mDefaultDirSep = '\\';
#else
	char PathInfo::mDefaultDirSep = '/';
#endif


	void PathInfo::SetDefaultBackSlash(bool back)
	{
		if (back) mDefaultDirSep = '\\';
		else mDefaultDirSep = '/';
	}


	PathInfo::PathInfo()
	{
		Init();
	}


	PathInfo::PathInfo(const std::string& pathName)
	{
		Init();
		SetFullPath(pathName);
	}


	PathInfo::PathInfo(const std::string& path, const std::string& name, const std::string& ext)
	{
		Init();
		SetPath(path);
		SetName(name);
		SetExt(ext);
	}


	PathInfo::PathInfo(const PathInfo& fileInfo)
	{
		Init();
		mDirSep[0] = fileInfo.GetDirSep();
		mName = fileInfo.GetName();
		mFullName = fileInfo.GetFullName();
		mExt = fileInfo.GetExt();
		mPath = fileInfo.GetPath();
		mFullPath = fileInfo.GetFullPath();
		mFullPathReady = true;
		mRootPath = fileInfo.GetRootPath();
		mDevice = fileInfo.GetDevice();
		mRootPathReady = true;
	}


	PathInfo::~PathInfo()
	{
		// call the Reset() method defined in this class
		PathInfo::Reset();
	}


	void PathInfo::SetPath(const std::string& str, const std::string& s1, const std::string& s2)
	{
		size_t colon = str.find(':');
		// check that all the strings include valid paths
		if (str.size() > colon && HasColon(&str[colon + 1])) return;
		DirtyPath();
		bool str_valid = !str.empty();
		bool s1_valid = !s1.empty() && !HasColon(s1);
		bool s2_valid = !s2.empty() && !HasColon(s2);

		std::string path1;
		std::string path2;
		// cut off double slashes
		if (str_valid && s1_valid
			&& IsDirSep(str.back())
			&& IsDirSep(s1.front())
			) path1 = &s1[1];
		else path1 = s1;
		if (s1_valid && s2_valid
			&& IsDirSep(s1.back())
			&& IsDirSep(s2.front())
			) path2 = &s2[1];
		else path2 = s2;

		mPath = str + path1 + path2;

		if (!mPath.empty() && !IsDirSep(mPath.back()))
		{
			mPath = mPath + mDirSep;
		}
		FixDirSep(mPath);

		mFullPathReady = false;
		SplitPath();
		Compact();
	}


	const std::string& PathInfo::GetPath() const
	{
		return mPath;
	}


	void PathInfo::SetDevice(const std::string& str)
	{
		if (mDevice == str) return;
		SplitPath();
		mDevice = str;
		mPath = mDevice + mRootPath;
		mFullPathReady = false;
	}


	const std::string& PathInfo::GetDevice() const
	{
		return mDevice;
	}


	void PathInfo::SetName(const std::string& str, const std::string& s1, const std::string& s2)
	{
		if (!mExt.empty()) SetFullName(str + s1 + s2 + "." + mExt);
		else SetFullName(str + s1 + s2);
	}


	const std::string& PathInfo::GetName() const
	{
		return mName;
	}


	void PathInfo::SetFullName(const std::string& str, const std::string& s1, const std::string& s2)
	{
		DirtyPath();
		int namelen = 0;
		int extlen = 0;

		mFullName = str + s1 + s2;

		mName.clear();
		mExt.clear();

		if (mFullName.empty()) return;

		int len = (int)mFullName.length();
		size_t dot = mFullName.rfind('.');
		if (dot == std::string::npos || mFullName[len - 1] == '.')
		{
			mName = mFullName;
			mExt.clear();
			return;
		}

		extlen = len - (int)dot - 1;
		namelen = len - extlen - 1;
		if (namelen > 0)
		{
			mName = mFullName.substr(0, namelen);
		}
		if (extlen > 0) mExt = mFullName.substr(dot + 1);

		if (mName.empty()) mName = "";
		if (mExt.empty())  mExt = "";
	}


	const std::string& PathInfo::GetFullName() const
	{
		return mFullName;
	}


	void PathInfo::SetExt(const std::string& str, const std::string& s1, const std::string& s2)
	{
		DirtyPath();
		std::string tmp = str + s1 + s2;
		if (!tmp.empty()) SetFullName(mName + std::string(".") + tmp);
		else SetFullName(mName);
	}


	const std::string& PathInfo::GetExt() const
	{
		return mExt;
	}


	const std::string& PathInfo::GetDirName() const
	{
		mDirName.clear();
		if (mPath.empty())
		{
			mDirName = "";
			return mDirName;
		}

		if (!mRootPathReady) SplitPath();
		int len = (int)mRootPath.length();
		if (len == 1 && mRootPath[0] == mDirSep[0])
		{
			//mDirName = mDirSep; // root
			mDirName = "";
			return mDirName;
		}

		std::string tmp = mRootPath;
		if (tmp.empty())
		{
			mDirName = "";
			return mDirName;
		}
		len = (int)tmp.length();
		if (tmp[len - 1] == mDirSep[0]) tmp = tmp.substr(0, len - 1);
		size_t slash = tmp.rfind(mDirSep[0]);
		if (slash == std::string::npos)
		{
			mDirName = tmp;
			return mDirName;
		}
		mDirName = tmp.substr(slash + 1);
		if (mDirName.size() == 1 && IsDirSep(mDirName[0])) mDirName.clear();

		return mDirName;
	}


	void PathInfo::SetFullPath(const std::string& fullpath)
	{
		if (&fullpath == &mFullPath) return;

		Reset();
		if (fullpath.empty()) return;

		if (fullpath == "." || fullpath == "..")
		{
			mPath = fullpath + mDirSep;
			mFullPath = mPath;
			return;
		}

		size_t slash = fullpath.rfind('/');
		if (slash == std::string::npos) slash = fullpath.rfind('\\');
		bool slashFound = slash != std::string::npos;

		if (slashFound)
		{
			mPath = fullpath.substr(0, slash + 1);
			FixDirSep(mPath);
		}
		else
		{
			mPath.clear();// = mDirSep;
		}
		if (!mPath.empty() && !IsDirSep(mPath.back()))
		{
			mPath = mPath + mDirSep;
		}

		if (slashFound)
		{
			SetFullName(&fullpath[slash + 1]);
		}
		else
		{
			SetFullName(fullpath);
		}

		Compact();
		SplitPath();
		mFullPath = mPath + mFullName;
	}


	const std::string& PathInfo::GetFullPath() const
	{
		if (mFullPathReady) return mFullPath;
		mFullPathReady = true;
		mFullPath.clear();

		mFullPath = mPath + mName;//mDevice+mRootPath+mName;
		if (!mExt.empty()) mFullPath += std::string(".") + mExt;

		return mFullPath;
	}


	bool PathInfo::MatchesPattern(const std::string& pattern) const
	{
		PathInfo patternInfo(pattern);
		if (GetPath() != patternInfo.GetPath() && !patternInfo.GetPath().empty())
		{
			return false;
		}
		auto& ConvertToRegex = [](const std::string& inStr)
		{
			std::string regexStr;
			for (size_t i = 0; i < inStr.size(); i++)
			{
				switch (inStr[i])
				{
				case '*':
					regexStr += ".*";
					break;
				case '?':
					regexStr.push_back('.');
					break;
				default:
#ifdef _WIN32
					regexStr.push_back(tolower(inStr[i]));
#else
					regexStr.push_back(inStr[i]);
#endif
					break;
				}
			}
			return regexStr;
		};
		std::string name = GetName();
		std::string ext = GetExt();
#ifdef _WIN32
		for (size_t i = 0; i < name.size(); i++)
		{
			name[i] = tolower(name[i]);
		}
		for (size_t i = 0; i < ext.size(); i++)
		{
			ext[i] = tolower(ext[i]);
		}
#endif
		std::string namePattern = ConvertToRegex(patternInfo.GetName());
		std::regex regName(namePattern);
		bool nameMatching = std::regex_match(name, regName);
		if (!nameMatching)
		{
			return false;
		}
		std::string extPattern = ConvertToRegex(patternInfo.GetExt());
		std::regex regExt(extPattern);
		bool extMatching = std::regex_match(ext, regExt);
		if (!extMatching)
		{
			return false;
		}

		return true;
	}


	bool PathInfo::MatchesPatterns(const std::vector<std::string>& patternList) const
	{
		if (patternList.empty())
		{
			return true;
		}

		for (const std::string& pattern : patternList)
		{
			if (MatchesPattern(pattern))
			{
				return true;
			}
		}
		return false;
	}


	void PathInfo::Reset()
	{
		mFullPath.clear();
		mPath.clear();
		mRootPath.clear();
		mDirName.clear();
		mFullName.clear();
		mName.clear();
		mExt.clear();
		Init();
	}


	bool PathInfo::SetRelativeToPath(const std::string& path)
	{
		if (path.empty())
		{
			return false;
		}

		if (mPath.empty())
		{
			return true;
		}

		if (path == mPath)
		{
			mPath.clear();
			DirtyPath();
			return false;
		}
		if (IsRelative())
		{
			return false;
		}

		PathInfo ref_path;
		ref_path.SetPath(path);
		if (ref_path.IsRelative())
		{
			return false;
		}
#ifdef _WIN32
		// check if the paths are on differnt disks
		if (ref_path.GetDevice() != mDevice)
		{
			return false;
		}
#endif

		DirtyPath();

#ifdef _WIN32
		if (strcasecmp(ref_path.GetRootPath().c_str(), mPath.c_str()) == 0)
#else
		if (r_path == mPath)
#endif
		{
			mPath.clear();
			mRootPath.clear();
			mDevice.clear();
			mRootPathReady = true;

			return true;
		}


		std::string r_path = ref_path.GetPath();
		FixDirSep(r_path);
		std::string dotdotslash = std::string("..") + mDirSep; // "../"

		size_t len1 = mPath.length();
		size_t len2 = r_path.length();
		size_t len = std::min(len1, len2);
		size_t diff_idx = 0;
#ifdef _WIN32
		for (size_t i = 0; i < len && tolower(mPath[i]) == tolower(r_path[i]); ++i)
#else
		for (size_t i = 0; i < len && mPath[i] == r_path[i]; ++i)
#endif
		{
			if (mPath[i] == '\\' || mPath[i] == '/') diff_idx = i + 1;
		}

		// if one path is contained in the other path...

		// if this path is contained in the given path
		if (diff_idx == len1)
		{
			//mPath= D:\DATA\
		    //path= D:\DATA\VE\layout
			//relpath= ..\..
			std::string relpath;
			PathInfo fi(&r_path[len1]);
			while (fi.UpperPath()) relpath += dotdotslash;
			SetPath(relpath);
			return true;
		}
		// else if the given path is contained in this path
		else if (diff_idx == len2)
		{ // diff_idx==l1
		  //mPath= D:\DATA\VE\layout
		  //path= D:\DATA\
	      //relpath= VE\layout
			if (mPath[len2] == mDirSep[0]) ++len2;
			SetPath(&mPath[len2]);
			return true;
		}


		GetRootPath(); // force root path update

		std::string diff_path = mPath.substr(diff_idx);
		PathInfo fi;
		fi.SetPath(diff_path);
		mRootPath.clear();
		while (fi.UpperPath()) mRootPath += dotdotslash;
		mRootPath += diff_path;
		mDevice.clear();
		mPath = mRootPath;
		mRootPathReady = true;

		return true;
	}


	bool PathInfo::IsRelative() const
	{
		if (mPath.empty()) return true;
		if (mPath[0] == mDirSep[0]) return false;
		//int len = mPath.length();
		if (!mDevice.empty()) return false; // drive letter found
	  //  if ( len>1 && isalpha(mPath[0]) && mPath[1]==':' ) return false; // drive letter found
		return true;
	}


	bool PathInfo::JoinPath(const std::string& path)
	{
		if (path.empty()) return true;
		if (mPath.empty())
		{
			SetPath(path);
			return true;
		}
		PathInfo fi(path);
		if (!fi.IsRelative()) return false;
		SetPath(mPath, path);
		//Compact();

		return true;
	}


	bool PathInfo::ChangeToAbsPathFromCurrDir()
	{
		if (!mPath.empty() && !IsRelative()) return false;

		PathInfo prevpath(GetFullPath());
		prevpath.SetFullName("");
		SetPath(GetCurrPath().GetPath());
		JoinPath(prevpath.GetPath());

		return true;
	}


	bool PathInfo::ChangeToRelPathFromCurrDir()
	{
		if (!mPath.empty() && IsRelative()) return false;

		return SetRelativeToPath(GetCurrPath().GetPath());
	}


	bool PathInfo::UpperPath()
	{
		if (!mRootPathReady) SplitPath();
		if (mRootPath.empty() || mRootPath == mDirSep) return false;
		std::string tmp = mRootPath;
		size_t last_pos = std::string::npos;
		if (IsDirSep(tmp.back())) last_pos = tmp.size() - 2;
		size_t slash = tmp.rfind(mDirSep[0], last_pos);

		DirtyPath();

		if (slash == last_pos)
		{
			mPath.clear();
			return false;
		}

		mRootPath = tmp.substr(0, slash + 1);
		mPath = mDevice + mRootPath;
		mRootPathReady = true;

		return true;
	}


	void PathInfo::FixDirSep(std::string& buffer) const
	{
		if (buffer.empty()) return;

		char other = (mDirSep[0] == '\\') ? '/' : '\\';
		size_t n = buffer.size();
		for (size_t i = 0; i < n; ++i)
		{
			if (buffer[i] == other) buffer[i] = mDirSep[0];
		}
	}


	void PathInfo::SetBackSlash(bool back)
	{
		if (back) mDirSep[0] = '\\';
		else mDirSep[0] = '/';
		FixDirSep(mPath);
		FixDirSep(mRootPath);
		FixDirSep(mFullPath);
	}


	bool PathInfo::operator == (const std::string& name) const
	{
		std::string fixed_name(name);
		FixDirSep(fixed_name);
		return (this->GetFullPath() == fixed_name);
	}


	//----- protected -----

	void PathInfo::Init()
	{
		mFullPathReady = false;
		mRootPathReady = false;

		mDirSep[0] = mDefaultDirSep;
		mDirSep[1] = 0;
	}


	void PathInfo::Compact()
	{
		if (mPath.empty()) return;
		// search the dot
		size_t dot = mPath.find('.');
		if (dot == std::string::npos) return;
		bool dotdot = false;

		DirtyPath();

		while (dot != std::string::npos)
		{
			size_t olddot = dot;
			size_t len = mPath.length() - dot;
			if (len < 2) return;
			// search ".\"
			if (mPath[dot + 1] == mDirSep[0])
			{ // remove ".\"
				if (dot == 0 && mPath[2] != '.') dot += 2; // at the beginning
				else mPath.erase(dot, 2); // in the middle
			}
			else if (len>2 && mPath[dot + 1] == '.' && mPath[dot + 2] == mDirSep[0])
			{ // handle "..\"
				if (dot == 0 || dotdot)
				{ // at the beginning or already found
					dot += 3;
					dotdot = true;
				}
				else
				{
					// in the middle, not yet found
					dotdot = false;
					// find the slash
					size_t slash = mPath.substr(0, dot - 1).rfind(mDirSep[0]);
					if (slash != std::string::npos)
					{
						mPath.erase(slash, dot + 2 - slash); // move the trailing string back
						dot = slash; // move back to the next not-erased character
					}
				}
			}
			else ++dot; // simple dot in file or folder name
			dot = mPath.find('.', dot);
			if (dot == olddot) break; // guard to break even on bug case
		}
	}


	void PathInfo::SplitPath() const
	{
		// split path in device and root path
		size_t colon = mPath.find(':');
		bool has_device = (colon != std::string::npos);
		if (has_device)
		{
			mDevice = mPath.substr(0, colon + 1);
			if (mPath.length() > colon)
			{
				mRootPath = mPath.substr(colon + 1);
				FixDirSep(mRootPath);
			}
		}
		else
		{
			mRootPath = mPath;
			mDevice.clear();
		}
		mRootPathReady = true;
	}


	void PathInfo::DirtyPath()
	{
		mFullPathReady = false;
		mRootPathReady = false;
	}


	PathInfo gpvulc::GetCurrPath()
	{
#ifndef PATH_MAX
#ifdef _MAX_PATH
#define PATH_MAX _MAX_PATH
#else
#define PATH_MAX 32768
#endif
#endif
		char path[PATH_MAX + 1];
		getcwd(path, PATH_MAX);
		PathInfo path_info;
		path_info.SetPath(path);
		return path_info;
	}

} // namespace gpvulc
