#include "SelectionRequestParser.h"

#include <regex>
#include <stdexcept>

static std::regex g_request_regex("select\\s(?:from\\s)?(\\w+)(?:\\.(\\w+))?"
								  "(?:\\ssort\\sby\\s(\\w+)\\s?(asc|desc)?)?"
								  "(?:\\swhere\\s(\\w+)\\s=\\s(.*))?");
/*
0 - matched string
1 - table name
2 - mask column name, optional, only for sub-requests
3 - sort column name, optional
4 - sort mode, optional, asc is default
5 - filter column name, optional
6 - filter value, optional
*/
enum {
	TABLE_I = 1,
	MASK_COLUMN_I,
	SORT_COLUMN_I,
	SORT_MODE_I,
	FILTER_COLUMN_I,
	FILTER_VALUE_I
};

static std::regex g_sub_request_regex("\\$\\((.*)\\)");

SelectionRequestParser::SelectionRequestParser(const DataBase& db, const std::string& request)
	: m_db(db), m_request(request), m_table(), m_has_filter(false), m_filter_column(), m_filter_value()
{
}

std::function<bool(const Table::row_type& row)>
	SelectionRequestParser::BuildSelector(std::string& table_name,
										  std::string& sort_by_column,
										  bool& sort_ascending)
{
	parse(m_request, false);

	table_name = m_table;
	sort_by_column = m_sort_column;
	sort_ascending = m_sort_mode == "asc";

	if (!m_has_filter)
	{
		return {};
	}

	const BaseType* column_type;
	size_t column_index;

	const Table* table = m_db.GetTableByName(m_table);
	table->GetColumnInfoByName(m_filter_column, column_type, column_index);

	return [column_type, column_index, this](const Table::row_type& row) -> 
		   bool { return !column_type->Compare(row[column_index], this->m_filter_value); };
}

void SelectionRequestParser::parse(const std::string& request, bool sub_request)
{
	if (request.empty())
	{
		throw std::runtime_error("Cannot parse empty request");
	}

	std::smatch sm;
	if (!std::regex_match(request, sm, g_request_regex))
	{
		throw std::runtime_error("Invalid request format");
	}

	std::string table_name = sm[TABLE_I].str();
	std::string mask_column;
	if (sm[MASK_COLUMN_I].matched)
	{
		mask_column = sm[MASK_COLUMN_I].str();
	}

	std::string sort_column;
	if (sm[SORT_COLUMN_I].matched)
	{
		sort_column = sm[SORT_COLUMN_I].str();
	}
	std::string sort_mode = "asc";
	if (sm[SORT_MODE_I].matched)
	{
		sort_mode = sm[SORT_MODE_I].str();
	}

	if (!mask_column.empty() && !sub_request)
	{
		throw std::runtime_error("Request table name cannot include mask column name");
	}
	if (mask_column.empty() && sub_request)
	{
		throw std::runtime_error("Table name must include mask column name for sub-requests");
	}
	if (!sort_column.empty() && sub_request)
	{
		throw std::runtime_error("Sorting in sub-request is not supported");
	}

	if (!sub_request)
	{
		m_db.GetTableByName(table_name);
		m_table = table_name;
		m_sort_column = sort_column;
		m_sort_mode = sort_mode;
	}

	std::string filter_column;
	if (sm[FILTER_COLUMN_I].matched)
	{
		filter_column = sm[FILTER_COLUMN_I].str();
		if (!sub_request)
		{
			m_has_filter = true;
			m_filter_column = filter_column;
		}

		std::smatch sm1;
		std::string filter_value = sm[FILTER_VALUE_I].str();
		if (std::regex_match(filter_value, sm1, g_sub_request_regex))
		{
			parse(sm1[1].str(), true);
		}
		else
		{
			if (m_db.GetTableByName(table_name)->GetColumnTypeByName(filter_column)->IsValid(filter_value))
			{
				m_filter_value = filter_value;
			}
			else
			{
				throw std::runtime_error("Invalid filter value for the filter column");
			}
		}
	}
	else if (sub_request)
	{
		throw std::runtime_error("Condition is mandatory for sub-requests");
	}

	if (sub_request)
	{
		const BaseType* column_type;
		size_t column_index;

		const Table* table = m_db.GetTableByName(table_name);
		table->GetColumnInfoByName(filter_column, column_type, column_index);

		Table::selection_type result;
		table->SelectRows(result, [column_type, column_index, this](const Table::row_type& row) -> 
							   bool { return !column_type->Compare(row[column_index], this->m_filter_value); });

		if (result.empty())
		{
			throw std::runtime_error("Sub-request has empty result");
		}

		column_index = table->GetColumnIndexByName(mask_column);
		m_filter_value = result.front()->operator[](column_index);
	}
}