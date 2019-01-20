#include <stdio.h>
#include <strings.h>
#include <experimental/filesystem>
#include <fstream>
#include <cstring>
#include <iostream>

namespace fs = std::experimental::filesystem;

#define STRNICMP strncasecmp

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
	
	printf("What are ya searchin'? ");
	std::cin >> SearchTerm;
	
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
			
			const char* Pathname = Path.c_str();
			
			const uintmax_t Size = fs::file_size(Path);
			
			if ((Size >= SearchTermMinSize) && (Size <= SearchMaxFileSize) &&
					Path.has_extension() && !STRNICMP(Path.extension().string().c_str(), SearchExt, SearchExtLen))
			{
				//printf("Entry: %s: %lu\n", Pathname, Size);
				
				bool bMatches = false;
				
				std::fstream File (Path, (std::fstream::in|std::fstream::binary));
				
				while (File.good())
				{
					File.read(ReadBuf, ReadBufSz);
					
					// TODO Need to support ignoring case
					bMatches = (SearchTerm.compare(ReadBuf) == 0);
					if (bMatches || !File.good())
					{
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
				
				if (bMatches)
				{
					printf("Match => %s\n", Pathname);
				}
			}
		}
	}
	
	delete[] ReadBuf;
	
	return 0;
}