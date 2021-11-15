//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Text Processing Utility Toolkit


#include <gpvulc/text/text_util.h>
#include <gpvulc/console/console_util.h>
#include <gpvulc/fs/FileUtil.h>
#include <gpvulc/cmd/TextProcessing.h>

#include <assert.h>
#include <string.h>

namespace gpvulc
{

	bool ProcessFile(
		PathInfo& srcFilePath,
		PathInfo& srcOutFilePath,
		const PathInfo& exePath,
		std::vector<PathInfo>& changedFiles,
		std::function<unsigned(const std::string& srcText, std::string& outSrcText)>& ProcessText
		)
	{
		std::string srcText;

		std::cout << exePath.GetName() << ": Loading " << srcFilePath.GetFullPath() << std::endl;
		if (!LoadText(srcFilePath.GetFullPath(), srcText))
		{
			std::cerr << exePath.GetName() << ": error reading " << srcFilePath.GetFullPath() << std::endl;
			return false;
		}


		std::string outSrcText;
		unsigned changesCount = ProcessText(srcText, outSrcText);
		bool changed = changesCount > 0U && outSrcText != srcText;


		if (changed && !SaveText(srcOutFilePath.GetFullPath(), outSrcText))
		{
			std::cerr << exePath.GetName() << ": error writing " << srcOutFilePath.GetFullPath() << std::endl;
			return false;
		}
		if (!changed)
		{
			std::cout << "  " << srcFilePath.GetFullName() << ": nothing changed." << std::endl;
			return true;
		}

		std::cout << "  Text updated for " << srcOutFilePath.GetFullName() << "; " << changesCount << " change(s)." << std::endl;
		changedFiles.push_back(srcOutFilePath);
		return true;
	}


	int ConvertTextFiles(
		int argc,
		char* argv[],
		std::function<unsigned(const std::string& srcText, std::string& outSrcText)> textConverterFunc,
		const std::vector<std::string>& fileFilters,
		bool disableConsolePause
		)
	{
		assert(textConverterFunc);

		PathInfo exePath(argv[0]);

		if (argc < 2)
		{
			std::cout << "Usage: " << exePath.GetName() << " [-o<output_path>] <source_file_or_folder1> [<source_file_or_folder2> ...]" << std::endl;
			std::cout << "       (if -p is not specified output file path is the same as input, thus files are overwritten)" << std::endl;
			ConsolePause();
			return EXIT_SUCCESS;
		}

		std::vector<PathInfo> changedFiles;
		PathInfo outPath;
		std::vector<std::string> fileList;
		int fileListStart = 1;
		// TODO: implement configurable options
		if (argc > 2 && strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][1] == 'o')
		{
			outPath.SetFullPath(&argv[1][2]);
			outPath.SetFullName("");
			fileListStart = 2;
		}
		for (int i = fileListStart; i < argc; i++)
		{
			fileList.push_back(argv[i]);
		}

		int fileCount = 0;
		int errorCount = 0;
		for (size_t i = 0; i < fileList.size(); i++)
		{
			bool isDir = IsDirectory(fileList[i]);
			PathInfo srcFilePath;
			if (isDir)
			{
				srcFilePath.SetPath(fileList[i]);
			}
			else
			{
				srcFilePath.SetFullPath(fileList[i]);
			}
			PathInfo srcOutFilePath(fileList[i]);
			if (outPath.Defined())
			{
				if (isDir)
				{
					srcOutFilePath.SetPath(outPath.GetPath(), srcOutFilePath.GetDirName());
				}
				else
				{
					srcOutFilePath.SetPath(outPath.GetPath());
				}
			}
			if (isDir)
			{
				std::cout << exePath.GetName() << ": processing directory " << srcFilePath.GetDirName() << "..." << std::endl;
				DirObject dir(srcFilePath.GetPath());
				dir.ReadTree();
				std::vector<FileObject> files;
				dir.GetFileList(files);
				for (size_t i = 0; i < files.size(); ++i)
				{
					FileObject& fileObj = files[i];
					if (fileObj.MatchesPatterns(fileFilters))
					{
						std::cout << exePath.GetName() << ": processing file " << fileObj.GetFullName() << "..." << std::endl;
						PathInfo outFilePath = fileObj;
						if (outPath.Defined())
						{
							outFilePath.SetRelativeToPath(srcFilePath.GetPath());
							outFilePath.SetPath(outPath.GetPath(), outFilePath.GetPath());
							CreateDir(outFilePath.GetPath(), true);
						}
						if (!ProcessFile(fileObj, outFilePath, exePath, changedFiles, textConverterFunc))
						{
							errorCount++;
						}
						fileCount++;
					}
				}

			}
			else
			{
				if (srcFilePath.MatchesPatterns(fileFilters))
				{
					std::cout << exePath.GetName() << ": processing file " << srcFilePath.GetFullName() << "..." << std::endl;
					if (!ProcessFile(srcFilePath, srcOutFilePath, exePath, changedFiles, textConverterFunc))
					{
						errorCount++;
					}
					fileCount++;
				}
			}
		}

		std::cout << exePath.GetName() << ": " << fileCount << " file(s) processed";
		if (errorCount > 0)
		{
			std::cout << ", " << errorCount << " errors";
		}
		std::cout << "." << std::endl;
		if (changedFiles.empty())
		{
			std::cout << " No changed file." << std::endl;
		}
		else
		{
			std::cout << " Files changed:" << std::endl;
			for (size_t i = 0; i < changedFiles.size(); i++)
			{
				std::cout << "  " << changedFiles[i].GetFullPath() << std::endl;
			}
		}
		if (!disableConsolePause)
		{
			ConsolePause();
		}

		return EXIT_SUCCESS;
	}

}