#include "SelectionRequestParser.h"

#include <regex>
#include <stdexcept>

static std::regex g_request_regex("select\\sfrom\\s(\\w+(?:\\.\\w+)?)(?:\\swhere\\s(\\w+)\\s=\\s(.*))?");
static std::regex g_sub_request_regex("\\$\\((.*)\\)");

SelectionRequestParser::SelectionRequestParser(const DataBase& db, const std::string& request)
	: m_db(db), m_request(request), m_table(), m_has_condition(false), m_column(), m_value()
{
}

std::function<bool(const Table::row_type& row)>
	SelectionRequestParser::BuildSelector(std::string& table_name)
{
	parse(m_request, false);

	table_name = m_table;
	if (!m_has_condition)
	{
		return {};
	}

	const BaseType* column_type;
	size_t column_index;

	const Table* table = m_db.GetTableByName(m_table);
	table->GetColumnInfoByName(m_column, column_type, column_index);

	return [column_type, column_index, this](const Table::row_type& row) -> 
		   bool { return !column_type->Compare(row[column_index], this->m_value); };
}

void SelectionRequestParser::parse(const std::string& request, bool sub_request)
{
	std::smatch sm;
	if (!std::regex_match(request, sm, g_request_regex) && sm.size() > 1)
	{
		throw std::runtime_error("Invalid request format");
	}

	std::string table_name = sm[1].str();
	std::string column_name;
	size_t dot_pos = table_name.find(".");
	if (dot_pos != std::string::npos)
	{
		column_name = table_name.substr(dot_pos + 1);
		table_name = table_name.substr(0, dot_pos);
	}

	if (!column_name.empty() && !sub_request)
	{
		throw std::runtime_error("Request table name cannot include column name");
	}
	if (column_name.empty() && sub_request)
	{
		throw std::runtime_error("Table name must include column name for sub-requests");
	}

	if (!sub_request)
	{
		m_db.GetTableByName(table_name);
		m_table = table_name;
	}

	if (sm.size() == 4 && sm[2].matched && sm[3].matched)
	{
		if (!sub_request)
		{
			m_has_condition = true;
			m_column = sm[2].str();
		}

		std::smatch sm1;
		std::string value = sm[3].str();
		if (std::regex_match(value, sm1, g_sub_request_regex) && sm1.size() == 2)
		{
			parse(sm1[1].str(), true);
		}
		else
		{
			if (m_db.GetTableByName(table_name)->GetColumnTypeByName(sm[2].str())->IsValid(value))
			{
				m_value = value;
			}
			else
			{
				throw std::runtime_error("Invalid value for the column");
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
		table->GetColumnInfoByName(column_name, column_type, column_index);

		Table::selection_type result;
		table->SelectRows(result, [column_type, column_index, this](const Table::row_type& row) -> 
							   bool { return !column_type->Compare(row[column_index], this->m_value); });

		if (result.empty())
		{
			throw std::runtime_error("Sub-request has empty result");
		}
		m_value = result.front()->operator[](column_index);
	}
}