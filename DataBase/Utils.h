#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace Utils
{
	std::istream& ReadLine(std::istream& stream,
						   std::vector<std::string>& words);

	bool FileExists(const std::string& file);
	bool DirExists(const std::string& dir);
	bool RemoveDir(const std::string& dir);
	bool CreateDir(const std::string& dir);
	void AddFileToDir(const std::string& dir,
					  const std::string& file_name,
					  std::ofstream& file);
	void OpenFileInDir(const std::string& dir,
					   const std::string& file_name,
					   std::ifstream& file);
	void ListDir(const std::string& dir, 
				 std::vector<std::string>& file_names);
}