#include <stdio.h>
#include <strings.h>
#include <experimental/filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::experimental::filesystem;

#define STRNICMP strncasecmp

int main()
{
	const char* SearchPath = "Content";
	const char* SearchExt = ".UASSET";
	const uintmax_t SearchMaxFileSize = (10 * 1024 * 1024);
	
	if (!fs::exists(SearchPath) || !fs::is_directory(SearchPath))
	{
		return 0;
	}
	
	const size_t SearchExtLen = strlen(SearchExt);
	// TODO Check is longer than 1 and 1st char is '.'
	
	const int SearchTermMinSize = 4;
	
	//std::string SearchTerm = "Goodbye";
	//std::string SearchTerm = "world";
	std::string SearchTerm = "FirstPersonTemplateWeaponFire02";
	
	const int TL = (int) SearchTerm.length();
	if (TL < SearchTermMinSize)
	{
		return 0;
	}
	
	const char& T0 = SearchTerm[0];

	const int ReadBufSz = TL;
	char* ReadBuf = new char[ReadBufSz];
	if (!ReadBuf)
	{
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
				printf("Entry: %s: %lu\n", Pathname, Size);
				
				bool bMatches = false;
				
				std::fstream File (Path, (std::fstream::in|std::fstream::binary));
				
				while (File.good())
				{
					auto gbefore = File.tellg();
					
					File.read(ReadBuf, ReadBufSz);
					
					auto e = File.eof();
					auto gafter = File.tellg();
					bool nl = (ReadBuf[0] == '\n');
					bool c0 = (ReadBuf[0] == '\0');
					auto pk = (File.peek() == '\n');
					
					// TODO Need to support ignoring case
					bMatches = (SearchTerm.compare(ReadBuf) == 0);
					if (bMatches || !File.good())
					{
						break;
					}
					
					//int NumBytesRead = (ReadBufSz - 1);
					int FoundSubIdx = -1;
					//if (ReadBuf[0] != '\n')
					{
						for (int i = 1; i < ReadBufSz; ++i)
						{
							/*
							if (ReadBuf[i] == '\n')
							{
								NumBytesRead = i;
								break;
							}
							if ((ReadBuf[i] == T0) && (FoundSubIdx == -1))
							{
								FoundSubIdx = i;
							}
							*/
							if (ReadBuf[i] == T0)
							{
								FoundSubIdx = i;
								break;
							}
						}
					}
					/*
					else
					{
						NumBytesRead = 0;
					}
					*/
					
					if (FoundSubIdx >= 0)
					{
						const int RewindDelta = (FoundSubIdx - TL);
						File.seekg(RewindDelta, std::fstream::cur);
					}
				}
				
				if (bMatches)
				{
					printf("  ^^^ Matches!\n");
				}
			}
		}
	}
	
	delete[] ReadBuf;
	
	return 0;
}