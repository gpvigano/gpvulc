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
/// @file PathInfo.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <stdio.h>
#include <string>
#include <vector>

namespace gpvulc
{

	/// @addtogroup Path
	///@{ 

	  /*!
	  File path information and management.
	  This class stores the parts of a path name: path, file name, extension.
	   Decompose a path into its components or compose it from its data.
	   Directory separators are managed according to the chosen format
	   (default = back slash for Windows, slash for other systems).
	   Example:

	  @code{.cpp}
		const std::string filename = "C:/test/myfile.txt";
		PathInfo fileinfo(filename);
		std::cout << "Path: " << fileinfo.GetPath() << std::endl
				  << "Name: " << fileinfo.GetName() << std::endl
				  << "Extension: " << fileinfo.GetExt() << std::endl
				  << "Full: " << fileinfo.GetFullPath() << std::endl;
	  @endcode
	   This will output:@code{.unparsed}
		Path: C:/test/
		Name: myfile
		Extension: txt
		Full: C:/test/myfile.txt
	  @endcode
	  */
	class PathInfo
	{
	public:

		/*!
		Set the default directory separator to back slash (true) or slash (false)
		@note The initial default is back slash on Windows, otherwise it is slash,
		this setting affect only the new instances, not the existing paths.
		*/
		static void SetDefaultBackSlash(bool back);

		//! Default constructor
		PathInfo();

		//! Copy constructor
		PathInfo(const PathInfo& fileInfo);

		//! Constructor from full path name (see SetFullPath())
		PathInfo(const std::string& pathName);

		//! Constructor from path, name, extension (see SetPath(), SetName(), SetExt())
		PathInfo(const std::string& path, const std::string& name, const std::string& ext);

		//! Destructor
		~PathInfo();

		///@name Get/Set path elements
		/// Optional extra strings are concatenated with the first one.
		/// Related components are updated soon or on demand when calling a getter method.
		//@{

		//! Set/compose the path (including the device name, e.g. "C:\temp\")
		void SetPath(const std::string& str, const std::string& s1 = "", const std::string& s2 = "");

		//! Get the path (including the device name, e.g. "C:\temp\")
		const std::string& GetPath() const;

		//! Set the device name (e.g. "C:")
		void SetDevice(const std::string& str);

		//! Get the device name (e.g. "C:")
		const std::string& GetDevice() const;

		//! Set/compose the file name
		void SetName(const std::string& str, const std::string& s1 = "", const std::string& s2 = "");

		//! Get the file name
		const std::string& GetName() const;

		//! Set/compose the file name with extension
		void SetFullName(const std::string& str, const std::string& s1 = "", const std::string& s2 = "");

		//! Get the file name with extension
		const std::string& GetFullName() const;

		//! Set/compose the file extension
		void SetExt(const std::string& str, const std::string& s1 = "", const std::string& s2 = "");

		//! Get the file extension
		const std::string& GetExt() const;

		//! Get the path from root excluding the device
		const std::string& GetRootPath() const { if (!mRootPathReady) SplitPath(); return mRootPath; }

		//! Get the name of the last directory in the path
		const std::string& GetDirName() const;

		//@}

		//! Extract from the given string the path, the name and the extension
		void SetFullPath(const std::string& fullpath);

		//! Get the full path string combining path, name and extension
		const std::string& GetFullPath() const;

		//! Check if this file path is defined
		bool Defined() const { return !GetFullPath().empty(); }

		//! Check if this file path matches the given pattern (containing wildcards)
		bool MatchesPattern(const std::string& pattern) const;

		/*!
		Check if this file path matches at least one of the given patterns (containing wildcards).
		@param patternList List of patterns to be matched.
		@return true if this file path matches at least one of the given patterns.
		@note If the parameter is an empty vector this method returns true.
		*/
		bool MatchesPatterns(const std::vector<std::string>& patternList) const;

		//! Reset all
		void Reset();

		/*!
		 Convert the current path to a path relative to the given one (both paths must not be relative).
		 @return
		 false if:
		 - the given path is nullptr or empty (the internal mPath is left unchanged)
		 - the given path or the internal path is relative (the internal mPath is left unchanged)
		 - the given path and the internal path are on different disks (the internal mPath is left unchanged)
		 true if:
		 - the internal mPath is null (the internal mPath is left unchanged)
		 - any other case (the internal mPath is changed)
		*/
		bool SetRelativeToPath(const std::string& path);

		//! Test if the current path is relative (not defined means relative)
		bool IsRelative() const;

		//! Try to join the current path with the given path
		bool JoinPath(const std::string& path);

		/*!
		 If the current path is relative change it to an absolute path
		 starting from the current working directory, return true only if the path was successfully changed
		*/
		bool ChangeToAbsPathFromCurrDir();

		/*!
		 If the current path is absolute change it to a relative path
		 starting from the current working directory, return true only if the path was successfully changed
		*/
		bool ChangeToRelPathFromCurrDir();

		//! Try to get the upper path in the directory tree
		bool UpperPath();

		//! Check if the given character is a the current directory separator
		bool IsDirSep(const char& c) const { return c == '\\' || c == '/'; }

		//! Converts the directory separators (slash/back-slash) into the appropriate ones
		void FixDirSep(std::string& buffer) const;

		//! Get the current directory separator
		char GetDirSep() const { return mDirSep[0]; }

		/*!
		Set the directory separator to back slash (true) or slash (false).
		@note By default it is set to back slash on Windows, otherwise it is slash
		*/
		void SetBackSlash(bool back);

		//! Check if the directory separator is set to back slash (true) or slash (false)
		bool BackSlash() const { return (mDirSep[0] == '\\'); }

		//! Assignment operator
		PathInfo& operator = (const std::string& file_path) { SetFullPath(file_path); return *this; }

		/// @name Comparison operators
		//@{

		bool operator == (const PathInfo& file_info) const { return (*this == file_info.GetFullPath()); };
		bool operator == (const std::string& name) const;
		bool operator != (const PathInfo& file_info) const { return !(*this == file_info); }
		bool operator != (const std::string& name) const { return !(*this == name); }

		//@}

	protected:

		mutable std::string mFullPath;     // Full path
		mutable std::string mDevice;       // Device
		std::string mPath;                 // Path only (device included)
		mutable std::string mRootPath;     // Path without device
		std::string mName;                 // Name only
		std::string mExt;                  // Extension
		mutable std::string mFullName;     // Name+extension
		mutable std::string mDirName;      // Directory name

		// Flag to check if there is no need to recompute the full path
		mutable bool mFullPathReady;

		// Flag to check if there is no need to recompute the root path
		mutable bool mRootPathReady;

		// Character string used as directory separator
		char mDirSep[2];

		// Default directory separator for new paths
		static char mDefaultDirSep;

		// Initialization method called by constructors and Reset()
		void Init();

		// Compact the current path removing "../" and "./" where possible
		void Compact();

		// Split mPath into mDevice and mRootPath
		void SplitPath() const;

		// Update after path is changed
		void DirtyPath();
	};

	//! Get the current path as a PathInfo.
	PathInfo GetCurrPath();

	///@}

}//namespace gpvulc


//! Streaming operator, print a path to an output stream.
inline std::ostream& operator << (std::ostream& strm, const gpvulc::PathInfo& path)
{
	strm << path.GetFullPath();
	return strm;
}

