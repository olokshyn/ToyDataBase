#pragma once

#include <string>
#include <functional>

#include "Table.h"
#include "DataBase.h"

class SelectionRequestParser
{
public:
	SelectionRequestParser(const DataBase& db, const std::string& request);

	std::function<bool(const Table::row_type& row)> 
		BuildSelector(std::string& table_name, 
					  std::string& sort_by_column, 
					  bool& sort_ascending);

private:
	void parse(const std::string& request, bool sub_request = false);

private:
	const DataBase& m_db;
	const std::string m_request;

	std::string m_table;
	std::string m_sort_column;
	std::string m_sort_mode;
	bool m_has_filter;
	std::string m_filter_column;
	std::string m_filter_value;
};

