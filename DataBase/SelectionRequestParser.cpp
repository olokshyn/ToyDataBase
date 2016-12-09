#include "SelectionRequestParser.h"

#include <regex>
#include <stdexcept>

static std::regex g_request_regex("select\\s(?:from\\s)?(\\w+)(?:\\.(\\w+))?(?:\\swhere\\s(\\w+)\\s=\\s(.*))?");
/*
0 - matched string
1 - table name
2 - mask column name, optional, only for sub-requests
3 - filter column name, optional
4 - filter value, optional
*/

static std::regex g_sub_request_regex("\\$\\((.*)\\)");

SelectionRequestParser::SelectionRequestParser(const DataBase& db, const std::string& request)
	: m_db(db), m_request(request), m_table(), m_has_filter(false), m_filter_column(), m_filter_value()
{
}

std::function<bool(const Table::row_type& row)>
	SelectionRequestParser::BuildSelector(std::string& table_name)
{
	parse(m_request, false);

	table_name = m_table;
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
	std::smatch sm;
	if (!std::regex_match(request, sm, g_request_regex))
	{
		throw std::runtime_error("Invalid request format");
	}

	std::string table_name = sm[1].str();
	std::string mask_column;
	if (sm[2].matched)
	{
		mask_column = sm[2].str();
	}

	if (!mask_column.empty() && !sub_request)
	{
		throw std::runtime_error("Request table name cannot include mask column name");
	}
	if (mask_column.empty() && sub_request)
	{
		throw std::runtime_error("Table name must include mask column name for sub-requests");
	}

	if (!sub_request)
	{
		m_db.GetTableByName(table_name);
		m_table = table_name;
	}

	std::string filter_column;
	if (sm[3].matched)
	{
		filter_column = sm[3].str();
		if (!sub_request)
		{
			m_has_filter = true;
			m_filter_column = filter_column;
		}

		std::smatch sm1;
		std::string filter_value = sm[4].str();
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