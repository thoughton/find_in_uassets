#include <experimental/filesystem>
#include <fstream>
#include <cstring>
#include <iostream>
#include <chrono>
#if !defined(PLAT_UNIX)
#include <conio.h>
#endif

namespace fs = std::experimental::filesystem;

#if defined(PLAT_UNIX)
#define STRNICMP strncasecmp
#define WSTR "%s"
#else
#define STRNICMP _strnicmp
#define WSTR "%ls"
#endif

// FIXME Why doesn't it find anything in Windows Debug?!

int main()
{
	const char* SearchPath = "Content";
	const char* SearchExt = ".UASSET";
	const uintmax_t SearchMaxFileSize = (10 * 1024 * 1024);
	
	if (!fs::exists(SearchPath) || !fs::is_directory(SearchPath))
	{
		printf("Can't find search path '%s', aborting.\n", SearchPath);
		return 0;
	}
	
	const size_t SearchExtLen = strlen(SearchExt);
	// TODO Check is longer than 1 and 1st char is '.'
	
	const int SearchTermMinSize = 4;
	
	std::string SearchTerm = "FirstPersonTemplateWeaponFire02";
#if 1
	printf("\n> What are ya searchin'? ");
	std::cin >> SearchTerm;
	printf("\n");
#endif
	
	const auto TimeStart = std::chrono::high_resolution_clock::now();
	
	const int SearchTermLen = (int) SearchTerm.length();
	if (SearchTermLen < SearchTermMinSize)
	{
		printf("Search term must be at least '%d' chars long, aborting.\n", SearchTermMinSize);
		return 0;
	}
	
	const char FirstChar = SearchTerm[0];

	const int ReadBufSz = SearchTermLen;
	char* ReadBuf = new char[ReadBufSz];
	if (!ReadBuf)
	{
		printf("Failed to allocate small buffer for reading files, aborting.\n");
		return 0;
	}
	
	for (const auto& Entry : fs::recursive_directory_iterator(SearchPath))
	{
		const fs::file_status& Status = Entry.status();
		
		if (fs::is_regular_file(Status) && fs::exists(Status))
		{
			const fs::path& Path = Entry.path();
			
			const uintmax_t Size = fs::file_size(Path);
			
			if ((Size >= SearchTermMinSize) && (Size <= SearchMaxFileSize) &&
					Path.has_extension() && !STRNICMP(Path.extension().string().c_str(), SearchExt, SearchExtLen))
			{
				//printf("Entry: %ls: %lu\n", Path.c_str(), Size);
				
				std::fstream File (Path, (std::fstream::in|std::fstream::binary));

				while (File.good())
				{
					File.read(ReadBuf, ReadBufSz);
					
					if (!File.good())
					{
						break;
					}
					
					// TODO Need to support ignoring case
					const bool bMatches = (SearchTerm.compare(ReadBuf) == 0);
					
					if (bMatches)
					{
						printf("Match => " WSTR "\n", Path.c_str());
						break;
					}
					
					int FoundSubIdx = -1;
					for (int i = 1; i < ReadBufSz; ++i)
					{
						// TODO Need to support case insensitive here
						if (ReadBuf[i] == FirstChar)
						{
							FoundSubIdx = i;
							break;
						}
					}
					
					if (FoundSubIdx >= 0)
					{
						const int RewindDelta = (FoundSubIdx - SearchTermLen);
						File.seekg(RewindDelta, std::fstream::cur);
					}
				}

#if 1
				// FIXME Sometimes Windows fails to find anything, even in Release, so logging errors here to help track it down
				if (!File.good())
				{
					if (File.bad())
					{
						printf("  ERROR: bad: " WSTR "\n", Path.c_str());
						break;
					}
					else if (File.fail() && !File.eof())
					{
						printf("  ERROR: fail: " WSTR "\n", Path.c_str());
						break;
					}
				}
#endif
			}
		}
	}
	
	delete[] ReadBuf;
	
	const std::chrono::duration<double> TimeTaken = (std::chrono::high_resolution_clock::now() - TimeStart);
	
	printf("\nTime taken: %g seconds", TimeTaken.count());
#if !defined(PLAT_UNIX)
	printf(". Press any key to exit... ");
	_getch();
#endif
	printf("\n\n");

	return 0;
}
