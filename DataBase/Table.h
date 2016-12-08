#pragma once

#include <list>
#include <string>
#include <vector>
#include <functional>

#include "BaseType.h"
#include "Column.h"

class Table
{
public:
	typedef std::vector<std::string> row_type;
	typedef std::vector<Column> columns_type;
	typedef std::list<row_type>::const_iterator row_iter_type;
	typedef std::list<row_iter_type> selection_type;

public:
	explicit Table(const std::vector<Column>& columns);

	const columns_type& GetColumns() const
	{
		return m_columns;
	}

	const BaseType* GetColumnTypeByName(const std::string& name) const
	{
		return GetColumnByName(name)->GetType();
	}

	size_t GetColumnIndexByName(const std::string& name) const
	{
		return GetColumnByName(name) - m_columns.begin();
	}

	void GetColumnInfoByName(const std::string& name,
							 const BaseType* &type,
							 size_t& index) const
	{
		columns_type::const_iterator it = GetColumnByName(name);
		type = it->GetType();
		index = it - m_columns.begin();
	}

	void AddRow(const row_type& row);

	void SelectRows(selection_type& result,
					std::function<bool(const row_type& row)> selector = {},
					const std::string& column_name_to_sort_by = "",
					bool sort_ascending = true) const;

private:
	columns_type::const_iterator GetColumnByName(const std::string& name) const;

	Table(const Table& other);
	Table& operator=(const Table& right);

private:
	const columns_type m_columns;
	std::list<row_type> m_rows;
};