#include "Table.h"

#include <algorithm>
#include <stdexcept>

Table::Table(const std::vector<Column>& columns)
	: m_columns(columns)
{
}

void Table::AddRow(const row_type& row)
{
	if (m_columns.size() != row.size())
	{
		throw std::runtime_error("Invalid row length");
	}
	for (size_t i = 0; i != m_columns.size(); ++i)
	{
		if (!m_columns[i].GetType()->IsValid(row[i]))
		{
			throw std::runtime_error("Invalid value for the type");
		}
	}
	m_rows.emplace_back(row.begin(), row.end());
}

void Table::SelectRows(selection_type& result,
					std::function<bool(const row_type& row)> selector,
					const std::string& column_name_to_sort_by,
					bool sort_ascending) const
{
	columns_type::const_iterator column;
	if (!column_name_to_sort_by.empty())
	{
		column = std::find_if(m_columns.begin(), m_columns.end(),
			[&column_name_to_sort_by](const Column& col) -> 
			bool { return col.GetName() == column_name_to_sort_by; });
		if (column == m_columns.end())
		{
			throw std::runtime_error("Invalid name of the column to sort by");
		}
	}

	for (row_iter_type it = m_rows.begin(); it != m_rows.end(); ++it)
	{
		if (!selector || selector(*it))
		{
			result.push_back(it);
		}
	}

	if (!column_name_to_sort_by.empty())
	{
		const BaseType* column_type = column->GetType();
		size_t column_index = column - m_columns.begin();
		result.sort([column_type, column_index, sort_ascending](row_iter_type left, row_iter_type right) -> 
			bool { 
				if (column_type->Compare((*left)[column_index], (*right)[column_index]) >= 0) 
				{ 
					return sort_ascending; 
				} 
				return !sort_ascending; 
			});
	}
}

Table::columns_type::const_iterator Table::GetColumnByName(const std::string& name) const
{
	columns_type::const_iterator it = std::find_if(m_columns.begin(), m_columns.end(),
												   [&name](const Column& col) -> 
												   bool { return col.GetName() == name; });
	if (it == m_columns.end())
	{
		throw std::runtime_error("Column with given name does not exist");
	}
	return it;
}