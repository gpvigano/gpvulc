//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// FileUtil.cpp File I/O utility


#include <gpvulc/fs/FileUtil.h>
#include <gpvulc/text/text_util.h>
#include <gpvulc/text/string_conv.h>

#include <stdio.h>
#include <sstream>

#ifdef _WIN32

#define DEFAULT_DIR_SEPSTR "\\"
#else
#define DEFAULT_DIR_SEPSTR "/"
#endif

//#ifdef _WIN32 && !defined(__CYGWIN__)
#ifdef _MSC_VER

#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Shlobj.h> // SHCreateDirectoryEx
#define chdir _chdir
#define getcwd _getcwd
#define mkdir(x,y) _mkdir((x))
#define stat64 _stati64

#ifndef F_OK
#define F_OK 4
#endif

#else
#include <unistd.h>
#include <dirent.h>

#define stat64 stat
#endif

#include <sys/utime.h>

#include <iostream>

#include <boost/filesystem.hpp>
using namespace boost;


#ifdef _MSC_VER
// disable the unsafe warning
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif

namespace gpvulc
{

	namespace
	{

		inline std::string StrGetNotEmpty(const std::string& str)
		{
			return str.empty() ? "" : str;
		}

		inline void FixTextSlashes(std::string& str)
		{
#ifdef _WIN32
			char oldchar = '/';
			char newchar = '\\';
#else
			char oldchar = '\\';
			char newchar = '/';
#endif
			for (size_t i = 0; i < str.length(); ++i)
			{
				if (str[i] == oldchar)
				{
					str[i] = newchar;
				}
			}
		}


		std::string GetOpErrorString(int code)
		{
			switch (code)
			{
			case ERROR_INVALID_FUNCTION:
				return "Incorrect function";
			case ERROR_FILE_NOT_FOUND:
				return "The system cannot find the file specified";
			case ERROR_PATH_NOT_FOUND:
				return "The system cannot find the path specified";
			case ERROR_TOO_MANY_OPEN_FILES:
				return "The system cannot open the file";
			case ERROR_ACCESS_DENIED:
				return "Access is denied";
			case ERROR_INVALID_HANDLE:
				return "The handle is invalid";
			case ERROR_ARENA_TRASHED:
				return "The storage control blocks were destroyed";
			case ERROR_NOT_ENOUGH_MEMORY:
				return "Not enough storage is available to process this command";
			case ERROR_INVALID_BLOCK:
				return "The storage control block address is invalid";
			case ERROR_BAD_ENVIRONMENT:
				return "The environment is incorrect";
			case ERROR_BAD_FORMAT:
				return "An attempt was made to load a program with an incorrect format";
			case ERROR_INVALID_ACCESS:
				return "The access code is invalid";
			case ERROR_INVALID_DATA:
				return "The data is invalid";
			case ERROR_OUTOFMEMORY:
				return "Not enough storage is available to complete this operation";
			case ERROR_INVALID_DRIVE:
				return "The system cannot find the drive specified";
			case ERROR_CURRENT_DIRECTORY:
				return "The directory cannot be removed";
			case ERROR_NOT_SAME_DEVICE:
				return "The system cannot move the file to a different disk drive";
			case ERROR_NO_MORE_FILES:
				return "There are no more files";
			case ERROR_WRITE_PROTECT:
				return "The media is write protected";
			case ERROR_BAD_PATHNAME:
				return "The specified path is invalid";
			case ERROR_FILENAME_EXCED_RANGE:
				return "The filename or extension is too long";
			case ERROR_CANCELLED:
				return "The operation was canceled by the user";
			}
			return "Unknown error";
		}

		/// Convert a vector of paths to a single double-null-terminated string
		std::string ConvertPathList(const std::vector<std::string>& src)
		{
			size_t n = src.size();

			std::string src_path_list;
			for (size_t i = 0; i < n; i++)
			{
				PathInfo srcpath(src[i].c_str());
				if (srcpath.IsRelative())
				{
					srcpath.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, srcpath.GetPath());
				}
				if (!srcpath.Defined())
				{
					fprintf(stderr, "A path in the list was not defined.\n");
					return false;
				}
				src_path_list += src[i] + '\0';
			}
			src_path_list += '\0';
			return src_path_list;
		}

	}


	//-----------------------------------------------------------------------------
	// FileObject implementation


	FileObject::FileObject()
	{
		Init();
	}

	void FileObject::Init()
	{
		PathInfo::Init();
		Buffer = nullptr;
		BufferSize = 0;
		BufferOwned = false;
	}


	FileObject::FileObject(const std::string& pathname)
	{
		Init();
		SetFullPath(pathname);
	}


	FileObject::FileObject(const FileObject& file_info)
	{
		Init();
		SetPath(file_info.GetPath());
		SetFullName(file_info.GetFullName());
	}


	FileObject::~FileObject()
	{
		Reset();
	}


	void FileObject::ResetInfo()
	{
		CreationTime.Reset();
		AccessTime.Reset();
		WriteTime.Reset();
		Size = -1;
		Attributes = 0;
		mAttribUpdated = false;
		mIsDir = false;
	}


	void FileObject::Reset()
	{
		PathInfo::Reset();

		if (BufferOwned) FreeBuffer();

		ResetInfo();
	}



	bool FileObject::Readable()
	{
		GetFullPath();
		if (mFullPath.empty())
		{
			return false;
		}
		std::ifstream fileStream(mFullPath, std::ios::in);
		if (fileStream.fail())
		{
			return false;
		}
		fileStream.close();
		return true;
	}


	FILE* FileObject::Open(const std::string& mode)
	{
		GetFullPath();
		if (mFullPath.empty())
		{
			return nullptr;
		}
		FILE* fp = fopen(mFullPath.c_str(), mode.c_str());
		if (!fp)
		{
			return nullptr;
		}
		return fp;
	}


	bool FileObject::CheckDirectoryAttr()
	{
		if (!mAttribUpdated && !ReadAttributes())
		{
			return false;
		}
		return mIsDir;
	}


	bool FileObject::ReadAttributes()
	{
		struct stat64 stbuf;

		if (stat64(GetFullPath().c_str(), &stbuf) != 0)
		{
			return false;
		}
		bool isdir = (stbuf.st_mode & _S_IFDIR) != 0;

		Attributes = stbuf.st_mode;

		if (!isdir)
		{
			Size = stbuf.st_size;
		}
		else
		{
			SetPath(GetFullPath());
			SetFullName(nullptr);
		}

		mIsDir = isdir;
		mAttribUpdated = true;

		return true;
	}


	bool FileObject::ReadDateTime()
	{
		struct stat64 stbuf;

		if (stat64(GetFullPath().c_str(), &stbuf) != 0)
		{
			return false;
		}

		auto DateTimeToTm = [](const std::tm& tmDt, DateTime& dt)
		{

			dt.Year = tmDt.tm_year + 1900;
			dt.Month = tmDt.tm_mon + 1;
			dt.Day = tmDt.tm_mday;
			dt.WeekDay = tmDt.tm_wday + 1;
			dt.Hour = tmDt.tm_hour;
			dt.Minute = tmDt.tm_min;
			dt.Second = tmDt.tm_sec;
			//dt.TimeOffsetHour = tmDt.tm_isdst ? 1 : 0;
			dt.IsDST = tmDt.tm_isdst != 0;
		};

#ifdef _MSC_VER
		// disable the unsafe warning
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
		std::tm* localCreationTime = std::localtime(&stbuf.st_ctime);
		std::tm* localAccessTime = std::localtime(&stbuf.st_atime);
		std::tm* localModificationTime = std::localtime(&stbuf.st_mtime);
#ifdef _MSC_VER
#pragma warning( pop )
#endif
		DateTimeToTm(*localCreationTime, CreationTime);
		DateTimeToTm(*localModificationTime, WriteTime);
		DateTimeToTm(*localAccessTime, AccessTime);

		return true;
	}


	long long FileObject::GetSize()
	{
		if (Size >= 0L)
		{
			return Size;
		}
		if (!ReadAttributes())
		{
			return -1L;
		}

		return Size;
	}


	DateTime FileObject::GetCreationTime()
	{
		if (!CreationTime.Valid())
		{
			ReadDateTime();
		}
		return CreationTime;
	}


	DateTime FileObject::GetAccessTime()
	{
		if (!AccessTime.Valid())
		{
			ReadDateTime();
		}
		return AccessTime;
	}


	DateTime FileObject::GetWriteTime()
	{
		if (!WriteTime.Valid())
		{
			ReadDateTime();
		}
		return WriteTime;
	}


	bool FileObject::SetTimes(DateTime creationTime, DateTime accessTime, DateTime writeTime, std::string* error_str)
	{
		bool result = true;
		if (!accessTime.Valid())
		{
			if (error_str)
			{
				error_str->assign("invalid access time");
			}
			return false;
		}
		if (!creationTime.Valid())
		{
			if (error_str)
			{
				error_str->assign("invalid creation time");
			}
			return false;
		}
		if (!writeTime.Valid())
		{
			if (error_str)
			{
				error_str->assign("invalid write time");
			}
			return false;
		}

#ifdef _MSC_VER
		// TODO: when we can change also creation time in the #else section we can delete this section (see next task)

//#ifdef _MSC_VER
//#pragma warning( push )
//#pragma warning( disable : 4996 )
//#endif
//		time_t ltime;
//		time(&ltime);
//		std::tm* now = std::localtime(&ltime);
//		bool nowIsDST = now->tm_isdst != 0;
//#ifdef _MSC_VER
//#pragma warning( pop )
//#endif

		//auto DateTimeToSystemTime = [&nowIsDST](const DateTime& dt, SYSTEMTIME& st)
		auto DateTimeToSystemTime = [](const DateTime& dt, SYSTEMTIME& st)
		{
			st.wYear = (WORD)dt.Year;
			st.wMonth = (WORD)dt.Month;
			st.wDay = (WORD)dt.Day;
			st.wDayOfWeek = (WORD)dt.WeekDay;
			st.wHour = (WORD)dt.Hour;// +dt.TimeOffsetHour;
			//if (!nowIsDST && dt.IsDST) st.wHour = (WORD)(dt.Hour-1);
			//if (nowIsDST && !dt.IsDST) st.wHour = (WORD)(dt.Hour+1);
			st.wMinute = (WORD)dt.Minute;
			st.wSecond = (WORD)dt.Second;
			st.wMilliseconds = (WORD)dt.Millisecond;
		};

		FILETIME creation_time_local, access_time_local, write_time_local;

		SYSTEMTIME datetime;
		DateTimeToSystemTime(creationTime, datetime);
		if (!SystemTimeToFileTime(&datetime, &creation_time_local))
		{
			if (error_str)
			{
				error_str->assign("failed to convert creation time");
			}
			return false;
		}
		DateTimeToSystemTime(accessTime, datetime);
		if (!SystemTimeToFileTime(&datetime, &access_time_local))
		{
			if (error_str)
			{
				error_str->assign("failed to convert access time");
			}
			return false;
		}
		DateTimeToSystemTime(writeTime, datetime);
		if (!SystemTimeToFileTime(&datetime, &write_time_local))
		{
			if (error_str)
			{
				error_str->assign("failed to convert write time");
			}
			return false;
		}

		FILETIME creation_time, access_time, write_time;

		LocalFileTimeToFileTime(&creation_time_local, &creation_time);
		LocalFileTimeToFileTime(&access_time_local, &access_time);
		LocalFileTimeToFileTime(&write_time_local, &write_time);

		HANDLE h = CreateFile(
			GetFullPath().c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (h == INVALID_HANDLE_VALUE)
		{
			if (error_str)
			{
				error_str->assign("invalid file handle");
			}
			return false;
		}
		result = (SetFileTime(h, &creation_time, &access_time, &write_time) != 0);
		CloseHandle(h);
		if (!result)
		{
			if (error_str)
			{
				error_str->assign("failed to set file times");
			}
			return false;
		}

		return true;
#else
		//std::tm localCreationTime = { 0 };
		std::tm localAccessTime = { 0 };
		std::tm localModificationTime = { 0 };

		auto TmToDateTime = [](const DateTime& dt, std::tm& datetime)
		{
			datetime.tm_year = dt.Year - 1900;
			datetime.tm_mon = dt.Month - 1;
			datetime.tm_mday = dt.Day - 1;
			datetime.tm_wday = dt.WeekDay - 1;
			datetime.tm_hour = dt.Hour;
			datetime.tm_min = dt.Minute;
			datetime.tm_sec = dt.Second;
			datetime.tm_isdst = dt.IsDST;
		};
		//TmToDateTime(creationTime, localCreationTime);
		TmToDateTime(writeTime, localModificationTime);
		TmToDateTime(accessTime, localAccessTime);
		struct _utimbuf ut;
		// Convert tm to time_t
		ut.actime = mktime(&localAccessTime);
		ut.modtime = mktime(&localModificationTime);
		// TODO: how to change also creation time?
		if (_utime(GetFullPath().c_str(), &ut) == -1)
		{
			if (error_str)
			{
				error_str->assign("failed to set time");
			}
			return false;
		}
		return true;
#endif
	}



	bool FileObject::SetBuffer(char* buf, int size, bool copy)
	{
		FreeBuffer();
		if (buf == nullptr || size <= 0)
		{
			return false;
		}
		if (copy)
		{
			char* tmp = new(std::nothrow) char[size];
			if (tmp == nullptr)
			{
				return false;
			}
			Buffer = tmp;
			BufferSize = size;
			memcpy((void*)Buffer, (void*)buf, size);
		}
		else
		{
			Buffer = buf;
			BufferSize = size;
		}
		BufferOwned = copy;
		return true;
	}


	bool FileObject::ReadBuffer()
	{
		GetFullPath();
		if (mFullPath.empty())
		{
			return false;
		}
		FILE* fp = fopen(mFullPath.c_str(), "rb");
		if (!fp)
		{
			return false;
		}

		fseek(fp, 0, SEEK_END);
		BufferSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* tmp = new(std::nothrow) char[BufferSize];
		if (tmp == nullptr)
		{
			return false;
		}
		Buffer = tmp;
		fread(Buffer, 1, BufferSize, fp);
		fclose(fp);

		return true;
	}


	bool FileObject::WriteBuffer()
	{
		if (!Buffer)
		{
			return false;
		}
		GetFullPath();
		if (mFullPath.empty())
		{
			return false;
		}
		FILE* fp = fopen(mFullPath.c_str(), "wb");
		if (!fp)
		{
			return false;
		}
		fwrite(Buffer, 1, BufferSize, fp);

		fclose(fp);

		return true;
	}


	void FileObject::FreeBuffer()
	{
		if (Buffer) delete[] Buffer;
		Buffer = nullptr;
		BufferSize = 0;
	}

	//---------------------------------------------------------
	// DirObject class implementation


	DirObject::DirObject()
	{
		ParentDirectory = nullptr;
	}


	DirObject::DirObject(const std::string& path, DirObject* parent_dir)
	{
		SetDirPath(path, parent_dir);
	}


	DirObject::~DirObject()
	{
		Reset();
	}


	void DirObject::SetDirPath(const std::string& path, DirObject* parent_dir)
	{
		ParentDirectory = parent_dir;
		mDirPath.SetPath(path);
	}


	void DirObject::Reset()
	{
		mDirPath.Reset();
		for (size_t i = 0; i < File.size(); ++i) delete File[i];
		File.clear();
		for (size_t i = 0; i < SubDirectory.size(); ++i) delete SubDirectory[i];
		SubDirectory.clear();
	}


	DirObject* DirObject::GetRoot()
	{
		if (ParentDirectory)
		{
			return ParentDirectory->GetRoot();
		}
		return this;
	}


	bool DirObject::ReadDir(const std::string& dir)
	{
		std::string dirname(dir.empty() ? mDirPath.GetPath() : dir);
		if (dirname.empty())
		{
			return false;
		}
		// fix slash characters
		FixTextSlashes(dirname);
		Reset();
		mDirPath.SetFullPath(dirname);
		//dirname = mPath.GetFullPath();

		namespace fs = boost::filesystem;
		fs::path dirPath(dirname);

		try
		{
			if (fs::exists(dirPath))
			{
				//if (fs::is_regular_file(dirPath))

				if (!fs::is_directory(dirPath))
				{
					std::cerr << dirPath << " is not a directory\n";
					return false;
				}

				std::vector<fs::path> dirs;

				for (auto&& x : fs::directory_iterator(dirPath))
				{
					if (fs::is_directory(x))
					{
						dirs.push_back(x.path());
					}
				}

				std::vector<fs::path> files;

				for (auto&& x : fs::directory_iterator(dirPath))
				{
					if (fs::is_regular_file(x))
					{
						files.push_back(x.path());
					}
				}

				//std::sort(dirs.begin(), dirs.end());
				//std::sort(files.begin(), files.end());

				for (auto&& dir : dirs)
				{
					SubDirectory.push_back(new DirObject(dir.string(), this));
				}
				for (auto&& file : files)
				{
					FileObject *fi = new FileObject(file.string());
					if (fi)
					{
						fi->Size = fs::file_size(file); // (this is the reason why DirObject is declared friend in FileObject)
						File.push_back(fi);
					}
				}
			}
			else
			{
				std::cerr << dirPath << " does not exist\n";
				return false;
			}
		}
		catch (const fs::filesystem_error& ex)
		{
			std::cerr << ex.what() << '\n';
			return false;
		}

		return true;
	}


	bool DirObject::ReadTree(const std::string& dir)
	{
		if (!dir.empty()) mDirPath.SetPath(dir);

		bool result = ReadDir(mDirPath.GetFullPath());
		size_t i;
		for (i = 0; i < SubDirectory.size(); i++)
		{
			if (!SubDirectory[i]->ReadTree(""))
			{
				result = false;
			}
		}

		return result;
	}



	void DirObject::GetFileNameList(std::vector< std::string >& list, bool path, bool ext)
	{
		size_t i;
		for (i = 0; i < SubDirectory.size(); i++)
		{
			SubDirectory[i]->GetFileNameList(list, path, ext);
		}

		for (i = 0; i < File.size(); i++)
		{
			std::string s;
			if (path)
			{
				if (ext) s = File[i]->GetFullPath();
				else
				{
					s += File[i]->GetPath();
					s += File[i]->GetName();
				}
			}
			else
			{
				if (ext) s = File[i]->GetFullName();
				else s = File[i]->GetName();
			}

			list.push_back(s);
		}
	}


	void DirObject::GetSubDirNameList(std::vector< std::string >& list, bool path)
	{
		size_t i;
		for (i = 0; i < SubDirectory.size(); ++i)
		{
			std::string s;
			SubDirectory[i]->GetSubDirNameList(list, path);
			if (path) s += SubDirectory[i]->mDirPath.GetPath();
			else s = SubDirectory[i]->mDirPath.GetName();
			list.push_back(s);
		}
	}


	void DirObject::GetFileList(std::vector< FileObject >& list)
	{
		size_t i;
		for (i = 0; i < SubDirectory.size(); ++i)
		{
			SubDirectory[i]->GetFileList(list);
		}

		for (i = 0; i < File.size(); i++)
		{
			list.push_back(*(File[i]));
		}
	}


	std::string DirObject::TreeToString(bool printSize)
	{
		std::string s;
		SubTreeToString(s, printSize);
		return s;
	}


	void DirObject::SubTreeToString(std::string& s, bool printSize, long long* treeSize)
	{
		std::ostringstream oStr;
		oStr << "- " << mDirPath.GetPath() << "\n";
		std::string subtree;
		std::string dir;
		size_t i;
		long long rootTreeSize = 0LL;
		if (printSize && treeSize == nullptr)
		{
			treeSize = &rootTreeSize;
		}
		for (i = 0; i < SubDirectory.size(); ++i)
		{
			SubDirectory[i]->SubTreeToString(dir, printSize, treeSize);
			subtree += dir;
		}
		IndentStr(subtree, 1, File.size() ? '|' : ' ');
		IndentStr(subtree, 1, ' ');
		oStr << subtree;
		long long totSize = 0LL;
		for (i = 0; i < File.size(); ++i)
		{
			oStr << " |- ";
			oStr << File[i]->GetFullName();
			if (printSize)
			{
				long long fileSize = File[i]->GetSize();
				oStr << " [" << ApproxSizeString(fileSize) << "]";
				totSize += fileSize;
			}
			oStr << "\n";
		}
		if (printSize)
		{
			oStr << " [Directory size = " << ApproxSizeString(totSize) << "]" << "\n";
			if (treeSize)
			{
				*treeSize += totSize;
				if (treeSize == &rootTreeSize)
				{
					oStr << " [Tree size = " << ApproxSizeString(rootTreeSize) << "]" << "\n";
				}
				else
				{
					oStr << " [SubTree size = " << ApproxSizeString(*treeSize) << "]" << "\n";
				}
			}
		}
		s = oStr.str();
	}


	std::string DirObject::FindFile(const std::string& filename)
	{
		if (filename.empty())
		{
			return nullptr;
		}

#ifdef _WIN32
		const bool caseInsensitive = true;
#else
		const bool caseInsensitive = false;
#endif
		size_t i;
		for (i = 0; i < File.size(); ++i)
		{
			if (StrEqual(filename, File[i]->GetFullName(), caseInsensitive))  return File[i]->GetFullPath();
		}
		return nullptr;
	}


	std::string DirObject::FindSubDir(const std::string& dirname)
	{
		if (dirname.empty())
		{
			return nullptr;
		}

#ifdef _WIN32
		const bool caseInsensitive = true;
#else
		const bool caseInsensitive = false;
#endif
		size_t i;
		for (i = 0; i < SubDirectory.size(); i++)
		{
			if (StrEqual(dirname, SubDirectory[i]->mDirPath.GetDirName(), caseInsensitive))
			{
				return SubDirectory[i]->mDirPath.GetFullPath();
			}
		}
		return nullptr;
	}


	DirObject* DirObject::GetSubDir(const std::string& dirname)
	{
		if (dirname.empty())
		{
			return nullptr;
		}

#ifdef _WIN32
		const bool caseInsensitive = true;
#else
		const bool caseInsensitive = false;
#endif
		size_t i;
		for (i = 0; i < SubDirectory.size(); i++)
		{
			if (StrEqual(dirname, SubDirectory[i]->mDirPath.GetDirName(), caseInsensitive))
			{
				return SubDirectory[i];
			}
		}
		return nullptr;
	}


	std::string DirObject::FindFilePath(const std::string& filename)
	{
		if (filename.empty())
		{
			return nullptr;
		}

		std::string found = FindFile(filename);
		if (!found.empty())
		{
			return found;
		}

		size_t i;
		for (i = 0; i < SubDirectory.size(); i++)
		{
			found = SubDirectory[i]->FindFilePath(filename);
			if (!found.empty())
			{
				return found;
			}
		}
		return std::string();
	}


	//--------------------------------------------------
	// Utility functions implementation

	bool FileExists(const std::string& path)
	{
		return access(path.c_str(), F_OK) == 0;
	}

#ifdef SHFileOperation
	namespace
	{
		std::string GetFileErrorDescription(int errorCode)
		{
			switch (errorCode)
			{
			case 0x71: // DE_SAMEFILE
				return "The source and destination files are the same file.";
			case 0x72: // DE_MANYSRC1DEST
				return "Multiple file paths were specified in the source buffer, but only one destination file path.";
			case 0x73: // DE_DIFFDIR
				return "Rename operation was specified but the destination path is a different directory. Use the move operation instead.";
			case 0x74: // DE_ROOTDIR
				return "The source is a root directory, which cannot be moved or renamed.";
			case 0x75: // DE_OPCANCELLED
				return "The operation was cancelled by the user, or silently cancelled if the appropriate flags were supplied to SHFileOperation.";
			case 0x76: // DE_DESTSUBTREE
				return "The destination is a subtree of the source.";
			case 0x78: // DE_ACCESSDENIEDSRC
				return "Security settings denied access to the source.";
			case 0x79: // DE_PATHTOODEEP
				return "The source or destination path exceeded or would exceed MAX_PATH (" + ToString(MAX_PATH) + ").";
			case 0x7A: // DE_MANYDEST
				return "The operation involved multiple destination paths, which can fail in the case of a move operation.";
			case 0x7C: // DE_INVALIDFILES
				return "The path in the source or destination or both was invalid.";
			case 0x7D: // DE_DESTSAMETREE
				return "The source and destination have the same parent folder.";
			case 0x7E: // DE_FLDDESTISFILE
				return "The destination path is an existing file.";
			case 0x80: // DE_FILEDESTISFLD
				return "The destination path is an existing folder.";
			case 0x81: // DE_FILENAMETOOLONG
				return "The name of the file exceeds MAX_PATH (" + ToString(MAX_PATH) + ").";
			case 0x82: // DE_DEST_IS_CDROM
				return "The destination is a read-only CD-ROM, possibly unformatted.";
			case 0x83: // DE_DEST_IS_DVD
				return "The destination is a read-only DVD, possibly unformatted.";
			case 0x84: // DE_DEST_IS_CDRECORD
				return "The destination is a writable CD-ROM, possibly unformatted.";
			case 0x85: // DE_FILE_TOO_LARGE
				return "The file involved in the operation is too large for the destination media or file system.";
			case 0x86: // DE_SRC_IS_CDROM
				return "The source is a read-only CD-ROM, possibly unformatted.";
			case 0x87: // DE_SRC_IS_DVD
				return "The source is a read-only DVD, possibly unformatted. ";
			case 0x88: // DE_SRC_IS_CDRECORD
				return "The source is a writable CD-ROM, possibly unformatted.";
			case 0xB7: // DE_ERROR_MAX
				return "MAX_PATH (" + ToString(MAX_PATH) + ") was exceeded during the operation.";
			case 0x402: // 
				return "An unknown error occurred. This is typically due to an invalid path in the source or destination.";
			case 0x10000: // ERRORONDEST
				return "An unspecified error occurred on the destination.";
			case 0x10074: // DE_ROOTDIR | ERRORONDEST
				return "Destination is a root directory and cannot be renamed.";
			}

			return "Unknown error";
		}
	}
#endif

	bool FileDelete(const std::string& filename, bool allowUndo)
	{
		if (filename.empty())
		{
			return false;
		}

#ifdef SHFileOperation
		if (!allowUndo)
		{
			return DeleteFile(filename.c_str()) == TRUE;
		}

		char pathbuf[_MAX_PATH] = { 0 };
		strcpy(pathbuf, filename.c_str());
		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetActiveWindow();
		if (shfo.hwnd == nullptr) shfo.hwnd = GetDesktopWindow();
		shfo.wFunc = FO_DELETE;
		shfo.pFrom = pathbuf;
		shfo.pTo = nullptr;
		shfo.fFlags = FOF_NOCONFIRMATION;
		if (allowUndo) shfo.fFlags |= FOF_ALLOWUNDO;

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		int err = unlink(filename.c_str());
		if (err == -1)
		{
			switch (errno)
			{
			case EACCES:
				std::cerr << "Trying to delete a read - only file: " << filename << std::endl;
				break;
			case ENOENT:
				std::cerr << "File not found: " << filename << std::endl;
				break;
			}
			return false;
		}
		return true;
#endif
	}

	bool FileDelete(const std::vector<std::string>& paths, bool allowUndo)
	{
		if (!paths.size() == 0)
		{
			return false;
		}
#ifdef SHFileOperation
		std::string path_list = ConvertPathList(paths);

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetActiveWindow();
		if (shfo.hwnd == nullptr) shfo.hwnd = GetDesktopWindow();
		shfo.wFunc = FO_DELETE;
		shfo.pFrom = path_list.c_str();
		shfo.pTo = nullptr;
		shfo.fFlags = FOF_NOCONFIRMATION | FOF_MULTIDESTFILES | FOF_NOERRORUI;
		if (allowUndo) shfo.fFlags |= FOF_ALLOWUNDO;

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		for (size_t i = 0; i < n; i++)
		{
			if (!FileDelete(paths[i], allowUndo))
			{
				return false;
			}
		}
		return true;
#endif
	}


	bool FileMove(const std::string& filename, const std::string& dest, bool allowUndo)
	{
		PathInfo srcpath(filename);
		if (srcpath.IsRelative())
		{
			srcpath.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, srcpath.GetPath());
		}
		FileObject dstpath(dest);
		if (dstpath.IsRelative())
		{
			dstpath.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, dstpath.GetPath());
		}
		if (!srcpath.Defined())
		{
			std::cerr << "Move (Source path not defined)." << std::endl;
			return false;
		}
		if (!dstpath.Defined())
		{
			std::cerr << "Move " << srcpath.GetFullPath() << "\n to (Destination path not defined)." << std::endl;
			return false;
		}
		std::cout << "Move " << srcpath.GetFullPath() << "\nto " << dstpath.GetFullPath() << std::endl;

#ifdef SHFileOperation
		char pathbuf[_MAX_PATH + 2] = { 0 };
		char destbuf[_MAX_PATH + 2] = { 0 };
		strncpy(pathbuf, srcpath.GetFullPath().c_str(), _MAX_PATH);
		strncpy(destbuf, dstpath.GetFullPath().c_str(), _MAX_PATH);
		strncpy(&pathbuf[strlen(pathbuf)], "\0\0", 2);
		strncpy(&destbuf[strlen(destbuf)], "\0\0", 2);

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetActiveWindow();
		if (shfo.hwnd == nullptr) shfo.hwnd = GetDesktopWindow();
		shfo.wFunc = FO_MOVE;
		shfo.pFrom = pathbuf;
		shfo.pTo = destbuf;
		shfo.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
		if (allowUndo) shfo.fFlags |= FOF_ALLOWUNDO;

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		system::error_code ec;
		filesystem::rename(srcPath.GetFullPath(), dstPath.GetFullPath(), ec);
		if (ec.failed())
		{
			std::cerr << "FileMove - Move failed from "
				<< filename << " to " << dest << ": " << ec.message() << std::endl;
			return false;
		}
		return true;
#endif
	}


	bool FileMove(const std::vector<std::string>& src, const std::vector<std::string>& dst, bool allowUndo)
	{
		if (src.size() != dst.size())
		{
			fprintf(stderr, "FileMove - Source and destination path lists have different sizes.\n");
			return false;
		}
		size_t n = src.size();

#ifdef SHFileOperation
		std::string src_path_list = ConvertPathList(src);
		std::string dst_path_list = ConvertPathList(dst);

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetActiveWindow();
		if (shfo.hwnd == nullptr) shfo.hwnd = GetDesktopWindow();
		shfo.wFunc = FO_MOVE;
		shfo.pFrom = src_path_list.c_str();
		shfo.pTo = dst_path_list.c_str();
		shfo.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOERRORUI;
		if (allowUndo) shfo.fFlags |= FOF_ALLOWUNDO;

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		bool result = true;
		for (size_t i = 0; i < n; i++)
		{
			if (!FileMove(src[i], dst[i], allowUndo))
			{
				result = false;
			}
		}
		return result;
#endif
	}

	bool FileCopy(const std::string& filename, const std::string& dest, bool allowUndo)
	{
		PathInfo srcPath(filename);
		if (srcPath.IsRelative())
		{
			srcPath.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, srcPath.GetPath());
		}
		PathInfo dstPath(dest);
		if (dstPath.IsRelative())
		{
			dstPath.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, dstPath.GetPath());
		}
		if (!srcPath.Defined())
		{
			std::cerr << "FileCopy (destination path not defined)." << std::endl;
			return false;
		}
		if (!dstPath.Defined())
		{
			std::cerr << "FileCopy " << srcPath.GetFullPath() << "\nto (destination path not defined)." << std::endl;
			return false;
		}

		std::cout << "FileCopy " << srcPath.GetFullPath() << "\nto " << dstPath.GetFullPath() << std::endl;

#ifdef SHFileOperation
		char srcPathBuf[_MAX_PATH] = { 0 };
		char destPathBuf[_MAX_PATH] = { 0 };
		strncpy(srcPathBuf, srcPath.GetFullPath().c_str(), _MAX_PATH - 2);
		strncpy(destPathBuf, dstPath.GetFullPath().c_str(), _MAX_PATH - 2);
		strncpy(&srcPathBuf[strlen(srcPathBuf)], "\0\0", 2);
		strncpy(&destPathBuf[strlen(destPathBuf)], "\0\0", 2);

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetDesktopWindow();

		shfo.wFunc = FO_COPY;
		shfo.pFrom = srcPathBuf;
		shfo.pTo = destPathBuf;
		shfo.fFlags = FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
		if (allowUndo) shfo.fFlags |= FOF_ALLOWUNDO;

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		system::error_code ec;
		filesystem::copy_file(filename, dest, filesystem::copy_option::overwrite_if_exists, ec);
		if (ec.failed())
		{
			std::cerr << "FileCopy - Copy failed from "
				<< filename << " to " << dest << ": " << ec.message() << std::endl;
			return false;
		}
		return true;
#endif
	}

	bool FileCopy(const std::vector<std::string>& src, const std::vector<std::string>& dst, bool allowUndo)
	{
		if (src.size() != dst.size())
		{
			fprintf(stderr, "FileCopy - Source and destination path lists have different sizes.\n");
			return false;
		}
		size_t n = src.size();

#ifdef SHFileOperation
		std::string src_path_list = ConvertPathList(src);
		std::string dst_path_list = ConvertPathList(dst);

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = GetActiveWindow();
		if (shfo.hwnd == nullptr)
		{
			shfo.hwnd = GetDesktopWindow();
		}
		shfo.wFunc = FO_COPY;
		shfo.pFrom = src_path_list.c_str();
		shfo.pTo = dst_path_list.c_str();
		shfo.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOERRORUI;
		if (allowUndo)
		{
			shfo.fFlags |= FOF_ALLOWUNDO;
		}

		int err = SHFileOperation(&shfo);

		if (err != 0)
		{
			std::cerr << GetFileErrorDescription(err) << std::endl;
			return false;
		}
		return true;
#else
		bool result = true;
		for (size_t i = 0; i < n; i++)
		{
			if (!FileCopy(src[i], dst[i], allowUndo))
			{
				result = false;
			}
		}
		return result;
#endif
	}


	int FileCompareByTime(const std::string& path1, const std::string& path2)
	{
		bool result = true;

		FILETIME filetime1;
		HANDLE h1 = CreateFile(path1.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h1 == INVALID_HANDLE_VALUE)
		{
			return -1;
		}
		result = (GetFileTime(h1, nullptr, nullptr, &filetime1) == TRUE);
		CloseHandle(h1);
		if (!result)
		{
			return -1;
		}

		FILETIME filetime2;
		HANDLE h2 = CreateFile(path2.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h2 == INVALID_HANDLE_VALUE)
		{
			return -1;
		}
		result = (GetFileTime(h2, nullptr, nullptr, &filetime2) == TRUE);
		CloseHandle(h2);
		if (!result)
		{
			return -1;
		}

		LONG comparison = CompareFileTime(&filetime1, &filetime2);
		switch (comparison)
		{
		case -1: return 2; // First file time is earlier than second file time.
		case  1: return 1; // First file time is later than second file time.
		case  0: return 0; // First file time is equal to second file time.
		}

		return -1;
	}


	bool FileUpdate(
		PathInfo src_file_name,
		PathInfo dst_file_name,
		bool overwrite_newer)
	{

		if (!dst_file_name.Defined())
		{
			std::cerr << "Path not defined: " << StrGetNotEmpty(dst_file_name.GetFullPath()) << std::endl;
			return false;
		}
		enum AccessMode { EXISTS = 0, WRITABLE = 2, READABLE = 4 };
		int accessResult = _access(src_file_name.GetFullPath().c_str(), READABLE);
		if (accessResult == -1)
		{
			std::cerr << "Path not accessible: " << StrGetNotEmpty(src_file_name.GetFullPath()) << std::endl;
			return false;
		}

		if (dst_file_name.GetName().empty())
		{
			dst_file_name.SetFullName(src_file_name.GetFullName());
		}
		if (!CreateDir(dst_file_name.GetPath(), true))
		{
			std::cerr << "Cannot create output directory: " << StrGetNotEmpty(dst_file_name.GetPath()) << std::endl;
			return false;
		}
		int cmp = FileCompareByTime(src_file_name.GetFullPath(), dst_file_name.GetFullPath());
		if ((overwrite_newer && cmp == 0) || (!overwrite_newer && cmp == 2))
		{
			std::cout << "  File up-to-date: " << dst_file_name.GetFullPath() << std::endl;
		}
		else
		{
			if (!FileCopy(src_file_name.GetFullPath(), dst_file_name.GetFullPath()))
			{
				std::cerr << "Failed to copy:" << src_file_name.GetFullPath() << "\n         to: " << dst_file_name.GetFullPath() << std::endl;
				return false; // if the main file was not copied abort
			}
			std::cout << "Copied file: " << src_file_name.GetFullPath() << "\n         to: " << dst_file_name.GetFullPath() << std::endl;
		}

		return true;
	}


	bool FileUpdate(const std::string& src_file_path, const std::string& dst_file_path, bool overwrite_newer)
	{
		PathInfo src_file_name(src_file_path);
		PathInfo dst_file_name(dst_file_path);
		return FileUpdate(src_file_name, dst_file_name, overwrite_newer);
	}

	namespace
	{
		inline bool _FileBackupRestore(const std::string& filename, bool restore)
		{
			if (filename.empty())
			{
				return false;
			}

			PathInfo orig_file(filename);  // original file
			PathInfo new_file(filename);   // backup file
			new_file.SetExt("~", new_file.GetExt());
#ifdef CopyFile
			if (restore)
			{
				return CopyFile(
					new_file.GetFullPath().c_str(), // pointer to filename to copy to
					orig_file.GetFullPath().c_str(),  // pointer to name of an existing file
					FALSE   // flag for operation if file exists
					) == TRUE;
			}

			return CopyFile(
				orig_file.GetFullPath().c_str(),
				new_file.GetFullPath().c_str(),
				FALSE
				) == TRUE;
#else
			system::error_code ec;
			if (restore)
			{
				filesystem::copy_file(new_file.GetFullPath(), orig_file.GetFullPath(), filesystem::copy_option::overwrite_if_exists, ec);
			}
			else
			{
				filesystem::copy_file(orig_file.GetFullPath(), new_file.GetFullPath(), filesystem::copy_option::overwrite_if_exists, ec);
			}
			return !ec.failed();
#endif

		}
	}

	bool FileBackup(const std::string& filename)
	{
		return _FileBackupRestore(filename, false);
	}


	bool FileRestore(const std::string& filename)
	{
		return _FileBackupRestore(filename, true);
	}


	bool CreateDir(const std::string& pathname, bool create_intermediate)
	{
		PathInfo path(pathname);
		if (path.IsRelative())
		{
			path.SetPath(GetCurrDir(), DEFAULT_DIR_SEPSTR, path.GetPath());
		}
#ifdef CreateDirectoryEx

		int err = -1;
		if (create_intermediate)
		{
			err = SHCreateDirectoryEx(nullptr, path.GetPath().c_str(), nullptr);
		}
		else err = CreateDirectoryEx(nullptr, path.GetPath().c_str(), nullptr);

		switch (err)
		{
		case ERROR_SUCCESS:
		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			return true;
		case ERROR_PATH_NOT_FOUND:
		case ERROR_BAD_PATHNAME:
		case ERROR_FILENAME_EXCED_RANGE:
		case ERROR_CANCELLED:
		default:
			std::cerr << GetOpErrorString(err) << std::endl;
			return false;
		}
#else
		mkdir(path.GetPath().c_str(), 0755);
		return false;
#endif
	}


	std::string GetCurrDir()
	{
		const int max_path = 1024;
		static char path_name[max_path + 1];
		*path_name = 0;

		return getcwd(path_name, max_path);
	}


	bool ChangeCurrDir(const std::string& path_name)
	{
		if (path_name.empty())
		{
			return false;
		}
		if (chdir(path_name.c_str()))
		{
			return false;
		}
		return true;
	}


	bool ChangeCurrDirToFilePath(const std::string& path_name)
	{
		if (path_name.empty())
		{
			return false;
		}
		PathInfo exeinfo(path_name);
		exeinfo.ChangeToAbsPathFromCurrDir();
		return ChangeCurrDir(exeinfo.GetPath());
	}


	std::string GetTempDir()
	{
		const char* tmp = getenv("TMP");
		if (tmp == nullptr) tmp = getenv("TEMP");
		if (tmp == nullptr) tmp = getenv("TMPDIR");
		return tmp;
	}

	bool IsDirectory(const std::string& pathName)
	{
		boost::filesystem::path p(pathName);
		return boost::filesystem::is_directory(p);
	}

	bool IsDiskAvailable(char drive_letter)
	{
#ifdef GetDriveType
		drive_letter = toupper(drive_letter);
		if (drive_letter<'A' || drive_letter>'Z')
		{
			return false;
		}
		DWORD drives = GetLogicalDrives();
		unsigned int currdrive = drive_letter - 'A';

		if ((drives&(0x01 << currdrive)) == 0)
		{
			return false;
		}

		//UINT errmode = GetErrorMode();
		SetErrorMode(SEM_FAILCRITICALERRORS);

		char name[] = "A:";
		name[0] = drive_letter;
		switch (GetDriveType(name))
		{
		case DRIVE_REMOVABLE://The drive has removable media; for example, a floppy drive, thumb drive, or flash card reader.
		case DRIVE_FIXED://The drive has fixed media; for example, a hard drive or flash drive.
		case DRIVE_REMOTE://The drive is a remote (network) drive.
		case DRIVE_CDROM://The drive is a CD-ROM drive.
		case DRIVE_RAMDISK://The drive is a RAM disk.
			if (GetDiskFreeSpace(name, nullptr, nullptr, nullptr, nullptr))
			{
				SetErrorMode(0);
				return true;
			}
			break;

		case DRIVE_NO_ROOT_DIR://The root path is invalid; for example, there is no volume mounted at the specified path.
		default:
			break;
		}

		SetErrorMode(0);

		return false;
#else
#error Warning! IsDiskAvailable() not available for the current platform (any contribution is welcome...).
#endif
	}


	FILE* SafeReopen(const std::string& file_path, const std::string& mode, FILE* fileptr)
	{
		FILE* fp = fopen(file_path.c_str(), mode.c_str());
		if (fp)
		{
			fclose(fp);
			freopen(file_path.c_str(), mode.c_str(), fileptr);
		}
		return fp;
	}


	std::string ApproxSizeString(long long sizeInBytes, int precision)
	{
		long double sizeDouble = (long double)sizeInBytes;
		long double sizeLimit = 1024;
		long size1KB = 1024;
		long size1MB = size1KB*size1KB;
		long size1GB = size1MB*size1KB;
		long size1TB = size1GB*size1KB;
		if (sizeInBytes < size1KB)
		{
			return std::to_string(sizeInBytes) + " B";
		}
		if (sizeInBytes < size1MB)
		{
			return NumberToString(sizeDouble / size1KB, 0, precision, false) + " KB";
		}
		if (sizeInBytes < size1GB)
		{
			return NumberToString(sizeDouble / size1MB, 0, precision, false) + " MB";
		}
		if (sizeInBytes < size1TB)
		{
			return NumberToString(sizeDouble / size1GB, 0, precision, false) + " GB";
		}
		return NumberToString(sizeDouble / size1GB, 0, precision, false) + " TB";
	}

} // namespace gpvulc

#ifdef _MSC_VER
#pragma warning( pop )
#endif

