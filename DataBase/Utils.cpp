#include "Utils.h"

#include <string.h>
#include <Windows.h>

#include <sstream>
#include <iterator>
#include <algorithm>

static inline bool StuffDirName(const char* dirname)
{
	return !strcmp(dirname, ".") || !strcmp(dirname, "..");
}

std::istream& Utils::ReadLine(std::istream& stream,
							  std::vector<std::string>& words)
{
	words.clear();
	std::string line;
	if (!std::getline(stream, line))
	{
		return stream;
	}
	std::stringstream iss(line);
	std::copy(std::istream_iterator<std::string>(iss),
			  std::istream_iterator<std::string>(),
			  std::back_inserter(words));
	return stream;
}

void Utils::SplitByPairs(const std::vector<std::string>& words,
						 std::vector< std::pair<std::string, std::string> >& pairs)
{
	if (words.size() % 2 != 0)
	{
		throw std::runtime_error("Words count must be even");
	}
	for (size_t i = 0; i != words.size(); i += 2)
	{
		pairs.push_back(std::make_pair(words[i], words[i + 1]));
	}
}

bool Utils::FileExists(const std::string& file)
{
	DWORD fileAttrs = GetFileAttributes(file.c_str());
	if (fileAttrs == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	return false;
}

bool Utils::DirExists(const std::string& dir)
{
	DWORD dirAttrs = GetFileAttributes(dir.c_str());
	if (dirAttrs == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	if (dirAttrs & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
}

bool Utils::CreateDir(const std::string& dir)
{
	return !!CreateDirectory(dir.c_str(), NULL);
}

static inline bool _RemoveIfNotEmpty(const std::string& dir,
									 std::string& file_to_be_removed)
{
	if (!file_to_be_removed.empty())
	{
		if (!DeleteFile((dir + "\\" + file_to_be_removed).c_str()))
		{
			return false;
		}
		file_to_be_removed.clear();
	}
	return true;
}

bool Utils::RemoveDir(const std::string& dir)
{
	// return !system(("rmdir /S /Q " + dir).c_str());

	HANDLE hFind;
	WIN32_FIND_DATA fileData;

	hFind = FindFirstFile((dir + "\\*").c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	std::string file_to_be_removed;
	do
	{
		if (!_RemoveIfNotEmpty(dir, file_to_be_removed))
		{
			return false;
		}

		if (StuffDirName(fileData.cFileName))
		{
			continue;
		}

		std::string file_name = fileData.cFileName;
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!RemoveDir(dir + "\\" + file_name))
			{
				return false;
			}
		}
		else
		{
			file_to_be_removed = std::move(file_name);
		}
	} while (FindNextFile(hFind, &fileData) != 0);
	FindClose(hFind);

	if (!_RemoveIfNotEmpty(dir, file_to_be_removed))
	{
		return false;
	}

	return RemoveDirectory(dir.c_str()) != 0;
}

void Utils::AddFileToDir(const std::string& dir,
						 const std::string& file_name,
						 std::ofstream& file)
{
	if (file.is_open())
	{
		file.close();
	}
	file.open(dir + "\\" + file_name, std::ios::out);
}

void Utils::OpenFileInDir(const std::string& dir,
						  const std::string& file_name,
						  std::ifstream& file)
{
	if (file.is_open())
	{
		file.close();
	}
	file.open(dir + "\\" + file_name, std::ios::in);
}

void Utils::ListDir(const std::string& dir,
					std::vector<std::string>& file_names)
{
	WIN32_FIND_DATA fileData;
	HANDLE hFind;

	hFind = FindFirstFile((dir + "\\*").c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (!StuffDirName(fileData.cFileName))
		{
			file_names.push_back(fileData.cFileName);
		}
	} while (FindNextFile(hFind, &fileData) != 0);
}