#include "UIHelpers.h"

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iostream>


int UI::ChooseOption(const std::vector<std::string>& options, bool with_exit)
{
	for (;;)
	{
		for (int i = 0; i != options.size(); ++i)
		{
			std::cout << i + 1 << ". " << options[i] << std::endl;
		}
		if (with_exit)
		{
			std::cout << options.size() + 1 << ". Exit";
		}
		std::cout << std::endl;

		int choice = -1;
		if (!(std::cin >> choice)
			|| choice < 1
			|| (choice > static_cast<int>(options.size()) && !with_exit)
			|| (choice > static_cast<int>(options.size()) + 1 && with_exit))
		{
			std::cin.clear();
			std::cin.ignore(1000, '\n');
			std::cout << "Enter number from 1 to "
				<< options.size() + (with_exit ? 1 : 0) << std::endl;
			continue;
		}
		if (choice == options.size() + 1)
		{
			return -1;
		}
		else
		{
			return --choice;
		}
	}
}

void UI::ReadLine(std::vector<std::string>& words)
{
	std::string word;
	while (std::cin >> word)
	{
		words.push_back(word);
	}
}

void UI::PrintList(const std::vector<std::string>& elems)
{
	for (int i = 0; i != elems.size(); ++i)
	{
		std::cout << i + 1 << ". " << elems[i] << std::endl;
	}
	std::cout << std::endl;
}

static void _PrintTableHeader(const Table::columns_type& columns, 
							  const std::vector<int>& widthes,
							  int table_width)
{
	std::cout << std::string(table_width, '-') << std::endl;

	for (size_t i = 0; i != columns.size(); ++i)
	{
		std::cout << "| " << columns[i].GetName() 
			<< std::string(widthes[i] 
						   - static_cast<int>(columns[i].GetName().size()) - 2, ' ');
	}
	std::cout << '|' << std::endl;

	std::cout << std::string(table_width, '-') << std::endl;
}

static void _PrintRow(const Table::row_type& row, 
					  const std::vector<int>& widthes,
					  int table_width)
{
	for (size_t i = 0; i != row.size(); ++i)
	{
		std::cout << "| " << row[i] 
			<< std::string(widthes[i] - static_cast<int>(row[i].size()) - 2, ' ');
	}
	std::cout << '|' << std::endl;

	std::cout << std::string(table_width, '-') << std::endl;
}

void UI::PrintTableColumns(const Table* table, 
						   const TypeManager* type_manager,
						   bool nested)
{
	const Table::columns_type& columns = table->GetColumns();
	for (const Column& col : columns)
	{
		if (nested)
		{
			std::cout << '\t';
		}
		std::cout << col.GetName() << " : "
			<< type_manager->GetNameOfType(col.GetType()) << std::endl;
	}
	std::cout << std::endl;
}

void UI::PrintTableHeader(const Table* table)
{
	const Table::columns_type& columns = table->GetColumns();
	std::vector<int> widthes(columns.size());
	for (size_t i = 0; i != columns.size(); ++i)
	{
		widthes[i] = static_cast<int>(columns[i].GetName().size()) + 3;
	}
	int table_width = std::accumulate(widthes.begin(), widthes.end(), 0) + 1;

	_PrintTableHeader(columns, widthes, table_width);
}

void UI::PrintSelection(const Selection& selection)
{
	const Table* table = selection.GetTable();
	const Table::columns_type& columns = table->GetColumns();
	std::vector<int> widthes(columns.size());
	for (size_t i = 0; i != columns.size(); ++i)
	{
		widthes[i] = static_cast<int>(columns[i].GetName().size());
	}
	selection.ForEach([&widthes](const Table::row_type& row)
	-> bool
	{
		for (size_t i = 0; i != row.size(); ++i)
		{
			widthes[i] = std::max(widthes[i], static_cast<int>(row[i].size()));
		}
		return true;
	});

	for (int& width : widthes)
	{
		width += 3;
	}
	int table_width = std::accumulate(widthes.begin(), widthes.end(), 0) + 1;

	_PrintTableHeader(columns, widthes, table_width);

	selection.ForEach([&widthes, table_width](const Table::row_type& row)
	-> bool
	{
		_PrintRow(row, widthes, table_width);
		return true;
	});
}