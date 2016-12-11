#include "Table.h"

#include <iterator>
#include <algorithm>
#include <stdexcept>

#include "Utils.h"

Table::Table(const std::vector<Column>& columns)
	: m_columns(columns), m_rows()
{
}

Table::Table(std::istream& stream, 
			 const TypeManager* type_manager)
	: m_columns(), m_rows()
{
	LoadFromStream(stream, type_manager);
}

void Table::SaveToStream(std::ostream& stream,
						 const TypeManager* type_manager) const
{
	for (const Column& col : m_columns)
	{
		stream << col.GetName() << " "
			<< type_manager->GetNameOfType(col.GetType()) << " ";
	}
	stream << std::endl;

	std::ostream_iterator<std::string> output(stream, " ");
	for (const row_type& row : m_rows)
	{
		std::copy(row.begin(), row.end(), output);
		stream << std::endl;
	}
}

void Table::LoadFromStream(std::istream& stream,
						   const TypeManager* type_manager)
{
	if (!m_columns.empty())
	{
		m_rows.clear();
		m_columns.clear();
	}

	std::vector<std::string> line;
	if (!Utils::ReadLine(stream, line))
	{
		throw std::runtime_error("Failed to read columns");
	}
	if (line.size() % 2 != 0)
	{
		throw std::runtime_error("Invalid column line");
	}

	m_columns.reserve(line.size() / 2);
	for (size_t i = 0; i != line.size(); i += 2)
	{
		m_columns.emplace_back(line[i],
							   type_manager->GetTypeByName(line[i + 1]));
	}


	while (Utils::ReadLine(stream, line))
	{
		AddRow(line);
	}
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
					   const std::string& sort_by_column,
					   bool sort_ascending) const
{
	columns_type::const_iterator column;
	if (!sort_by_column.empty())
	{
		column = std::find_if(m_columns.begin(), m_columns.end(),
							  [&sort_by_column](const Column& col) ->
							  bool { return col.GetName() == sort_by_column; });
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

	if (!sort_by_column.empty())
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

bool operator==(const Table& left, const Table& right)
{
	return left.m_columns == right.m_columns 
		&& left.m_rows == right.m_rows;
}

bool operator!=(const Table& left, const Table& right)
{
	return !operator==(left, right);
}