//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief File I/O Utility
/// @file FileUtil.h
/// @author Giovanni Paolo Vigano'


#pragma once

#include <gpvulc/path/PathInfo.h>
#include <gpvulc/time/DateTime.h> // only for DateTime definition

//#include <stdio.h>
//#include <string.h>
#include <string>
#include <vector>

namespace gpvulc
{
	/// @anchor File
	/// @defgroup File File
	/// @brief File Utility library.
	/// @author Giovanni Paolo Vigano'

	/// File Utility
	/// @addtogroup File
	///@{

	//! Object to manage a file and its attributes
	class FileObject : public PathInfo
	{
	public:

		///@name Constructors/destructor
		///@{

		FileObject();
		FileObject(const FileObject& fileInfo);
		FileObject(const std::string& pathname);
		virtual ~FileObject();

		///@}

		//! Get the size of the file or -1 on error
		long long GetSize();

		//! Return a DateTime structure for the creation time
		DateTime GetCreationTime();

		//! Return a DateTime structure for the last access time
		DateTime GetAccessTime();

		//! Return a DateTime structure for the last write time
		DateTime GetWriteTime();

		//! Set file creation, access, write times (write to disk)
		bool SetTimes(DateTime creationTime, DateTime accessTime, DateTime writeTime, std::string* error_str = nullptr);

		/*!
		 Read file attributes and check if it is a directory
		 (in that case the path will be set to the full path and the name will be reset)
		*/
		bool CheckDirectoryAttr();

		/// A memory buffer is provided to store a (binary) file in memory
		///@name Memory file buffer
		///@{

		//! Set the file buffer in memory
		bool SetBuffer(char* buf, int size, bool copy = false);

		//! Get the size of the buffer in memory
		int GetBufferSize() const { return BufferSize; }

		//! Get the file from memory
		char* GetBuffer() { return Buffer; }

		//! Write the memory buffer (if allocated) on disk
		bool WriteBuffer();

		//! Read the memory buffer from disk
		bool ReadBuffer();

		//! Free memory buffer
		void FreeBuffer();

		///@}

		//! Reset all and free memory
		virtual void Reset();

		//! Test if the file path can be opened for reading
		bool Readable();

		//! Open a file with the current file path in the given mode
		FILE* Open(const std::string& mode);

		FileObject& operator = (const FileObject& fileInfo) { PathInfo::operator=((PathInfo&)(fileInfo)); return *this; }
		bool operator == (const FileObject& fileInfo) const { return PathInfo::operator==((const PathInfo&)(fileInfo)); }
		bool operator != (const FileObject& fileInfo) const { return !(*this == fileInfo); }

	private:
		friend class DirObject;

		char* Buffer;
		bool BufferOwned;
		bool mAttribUpdated;
		bool mIsDir;
		int BufferSize;
		long long Size;
		DateTime CreationTime;
		DateTime AccessTime;
		DateTime WriteTime;
		unsigned short Attributes;

		void Init();

		/// Retrieve file information using the current path
		///@name Attributes
		///@{

		/*!
		 Read the actual file size, return false on error
		 Check also if the current path points to a directory, in affermative case move the full path to mPath and reset the full name
		*/
		bool ReadAttributes();

		//! Read the actual file date and time, return false on error
		bool ReadDateTime();

		///@}

		//! Update after path is changed
		virtual void DirtyPath()
		{
			PathInfo::DirtyPath();
			ResetInfo();
		}

		//! Reset file information
		void ResetInfo();
	};


	/*!
	 Stores the information about a directory.
	 Example:@code
	 DirObject d(myPath);
	 d.ReadTree();
	 std::string s;
	 std::cout << d.TreeToString() << std::endl;
	 std::vector< std::string > list;
	 d.GetFileNameList( list, false );
	 for ( int i=0; i<list.size(); i++ )
	 {
	 std::cout << list[i].c_str() << std::endl;
	 }
	 @endcode
	*/
	class DirObject
	{

	public:
		PathInfo mDirPath;
		DirObject* ParentDirectory;
		std::vector< DirObject* > SubDirectory;
		std::vector< FileObject* > File;

		//! Default contructor
		DirObject();

		//! Constructor that calls SetPathDir()
		DirObject(const std::string& path, DirObject* parentDir = nullptr);

		//! Destructor
		~DirObject();

		//! Set the directory path and optionally its parent path
		void SetDirPath(const std::string& path, DirObject* parentDir = nullptr);

		//! Reset all and free memory
		void Reset();

		//! Returns the root directory of this subtree
		DirObject* GetRoot();

		/*!
		 Read a directory content
		@param dir directory path (if empty read the currently defined path)
		*/
		bool ReadDir(const std::string& dir = "");

		/*!
		 Read a directory tree content
		@param dir directory path (if empty read the currently defined path)
		*/
		bool ReadTree(const std::string& dir = "");

		//! Print as formatted text the directory tree to a string
		std::string TreeToString(bool printSize = false);

		/*!
		 Fill the given vector with file names (also sub-directories are scanned)
		 @param list a vector of strings that is filled with file names
		 @param path add the file path to the file name (default=true)
		 @param ext add the file extension to the file name (default=true)
		*/
		void GetFileNameList(std::vector< std::string >& list, bool path = true, bool ext = true);

		/*!
		 Fill the given vector with subdirectory names (also sub-sub-directories are scanned)
		 @param list a vector of strings that is filled with directory names
		 @param path add the directory path to the directory name (default=true)
		*/
		void GetSubDirNameList(std::vector< std::string >& list, bool path = true);

		/*!
		 Fill the given vector with file names (also sub-directories are scanned)
		 @param list a vector of FileObject that is filled with file infos
		*/
		void GetFileList(std::vector< FileObject >& list);

		//! Returns the complete path given the name of a file in the directory (subdirectories excluded)
		std::string FindFile(const std::string& filename);

		//! Returns the complete path given the name of a sub-directory (searched only in the first level)
		std::string FindSubDir(const std::string& dirname);

		//! Returns the sub-directory with the given name (searched only in the first level)
		DirObject* GetSubDir(const std::string& dirname);

		//! Returns the complete path given the name of a file in the directory subtree
		std::string FindFilePath(const std::string& filename);

	private:
		//! Print as formatted text the directory tree to a string
		void SubTreeToString(std::string& s, bool printSize = false, long long* treeSize = nullptr);
	};

	//! Test if a file path exists
	bool FileExists(const std::string& path);

	/*!
	 Delete the file with the given path, return false on error.
	 @param filename path of the file to delete
	 @param allowUndo use the recycler instead of deleting the file permanently (true by default)
	*/
	bool FileDelete(const std::string& filename, bool allowUndo = true);

	/*!
	 Delete the file with the given path, return false on error.
	 @param paths list of paths of the files to delete
	 @param allowUndo use the recycler instead of deleting the file permanently (true by default)
	*/
	bool FileDelete(const std::vector<std::string>& paths, bool allowUndo = true);

	/*!
	 Move a file with the given path, to the given destination, return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param filename path of the file to move
	 @param dest path name of the destination
	 @param allowUndo allow to restore the original file position using Windows Explorer (false by default)
	*/
	bool FileMove(const std::string& filename, const std::string& dest, bool allowUndo = false);

	/*!
	 Move the files with the given paths, to the given destinations, return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param src list of paths of the files to move
	 @param dst list of path names of the destinations
	 @param allowUndo allow to restore the original file position using Windows Explorer (false by default)
	*/
	bool FileMove(const std::vector<std::string>& src, const std::vector<std::string>& dst, bool allowUndo = false);

	/*!
	 Copy a file with the given path, to the given destination, return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param filename path of the file to copy
	 @param dest path name of the destination
	 @param allowUndo allow to delete the copied file position using the Undo command in Windows Explorer (false by default)
	*/
	bool FileCopy(const std::string& filename, const std::string& dest, bool allowUndo = false);

	/*!
	 Copy a list file with the given paths, to the given destinations, return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param src list of paths of the files to copy
	 @param dst list of path names of the destinations
	 @param allowUndo allow to delete the copied file position using the Undo command in Windows Explorer (false by default)
	*/
	bool FileCopy(const std::vector<std::string>& src, const std::vector<std::string>& dst, bool allowUndo = false);

	/*!
	 Compare two files using their last modification time.
	 @param path1 Path name of the first file
	 @param path2 Path name of the second file
	 @return -1 on error or one of the following values:<BR>
	 @li 0 = First file time is equal to second file time.<BR>
	 @li 1 = First file is newer.<BR>
	 @li 2 = Second file is newer.<BR>
	*/
	int FileCompareByTime(const std::string& path1, const std::string& path2);

	/*!
	 Copy a file with the given path, to the given destination,
	 create intermediate path if needed, update only if the destination file is older,
	 return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param src_file_path path of the file to copy
	 @param dst_file_path path name of the destination
	 @param overwrite_newer overwrite also newer files
	*/
	bool FileUpdate(
		PathInfo src_file_path,
		PathInfo dst_file_path,
		bool overwrite_newer);

	/*!
	 Copy a file with the given path, to the given destination,
	 create intermediate path if needed, update only if the destination file is older,
	 return false on error.
	 If the destination path includes a non existing path it is created (if it is possible)
	 @param src_file_path path of the file to copy
	 @param dst_file_path path name of the destination
	 @param overwrite_newer overwrite also newer files
	*/
	bool FileUpdate(const std::string& src_file_path, const std::string& dst_file_path, bool overwrite_newer);

	/*!
	 Create a backup copy of a file.
	 @param filename Original file name, the new file name will have a '~' before the extension
	*/
	bool FileBackup(const std::string& filename);

	/*!
	 Restore a backup copy of a file.
	 @param filename Original file name, the '~' will be removed from the extension
	*/
	bool FileRestore(const std::string& filename);

	//! Create a directory, if the second parameter is true try to create all the directories in the path
	bool CreateDir(const std::string& path, bool create_intermediate = false);

	//! Get the current working directory
	std::string GetCurrDir();

	//! Change the current working directory to the given path
	bool ChangeCurrDir(const std::string& path_name);

	//! Change the current working directory to match the given file path
	bool ChangeCurrDirToFilePath(const std::string& path_name);

	//! Get the system temporary directory
	std::string GetTempDir();

	//! Test if the given path is a directory
	bool IsDirectory(const std::string& pathName);

	#ifdef _WIN32
	//! Test if the given drive lettter is available (e.g. 'C','D',...)
	bool IsDiskAvailable(char drive_letter);
	#endif

	//! Call freopen after checking that the file with the given path can be opened
	FILE* SafeReopen(const std::string& file_path, const std::string& mode, FILE* fileptr);

	//! Convert a size in bytes into a string with GB, MB, KB, B according to the size number
	std::string ApproxSizeString(long long sizeInBytes, int precision = 1);

	///@}
}


