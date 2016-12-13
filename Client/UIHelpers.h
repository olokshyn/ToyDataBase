#pragma once

#include <string>
#include <vector>

#include "DataBase.h"

namespace UI
{
	int ChooseOption(const std::vector<std::string>& options, bool with_exit);
	void ReadLine(std::vector<std::string>& words);

	void PrintList(const std::vector<std::string>& elems);

	void PrintTableColumns(const Table* table, 
						   const TypeManager* type_manager,
						   bool nested = false);
	void PrintTableHeader(const Table* table);
	void PrintSelection(const Selection& selection);
}