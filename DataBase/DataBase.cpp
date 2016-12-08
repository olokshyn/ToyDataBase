#include "DataBase.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "SelectionRequestParser.h"

std::unique_ptr<const TypeManager> g_type_manager(nullptr);

DataBase::DataBase()
{
	if (!g_type_manager)
	{
		g_type_manager.reset(new TypeManager);
	}
}

void DataBase::GetTableNames(std::vector<std::string>& names) const
{
	std::transform(m_tables.begin(), m_tables.end(),
				   std::back_inserter(names),
				   [](const pair_type& p) -> std::string { return p.first; });
}

const Table* DataBase::GetTableByName(const std::string& table_name) const
{
	map_type::const_iterator it = m_tables.find(table_name);
	if (it == m_tables.end())
	{
		throw std::runtime_error("Table with given name does not exist");
	}
	return it->second.get();
}

void DataBase::AddTable(const std::string& table_name, 
						const std::vector< std::pair<std::string,
						std::string> >& columns)
{
	if (m_tables.find(table_name) != m_tables.end())
	{
		throw std::runtime_error("Table with given name already exists");
	}

	std::vector<std::string> type_names;
	g_type_manager->GetTypeNames(type_names);

	for (const std::pair<std::string, std::string>& col : columns)
	{
		if (std::find(type_names.begin(), type_names.end(), col.second) == type_names.end())
		{
			throw std::runtime_error("Invalid type name");
		}
	}

	std::vector<Column> typed_columns;
	typed_columns.reserve(columns.size());
	for (const std::pair<std::string, std::string>& col : columns)
	{
		typed_columns.emplace_back(col.first, g_type_manager->GetTypeByName(col.second));
	}

	m_tables.emplace(std::make_pair(table_name,
									std::move(std::unique_ptr<Table>(new Table(typed_columns)))));
}

void DataBase::AddRow(const std::string& table_name, const Table::row_type& row)
{
	map_type::iterator it = m_tables.find(table_name);
	if (it == m_tables.end())
	{
		throw std::runtime_error("Table with given name does not exist");
	}
	it->second->AddRow(row);
}

std::shared_ptr<Selection> DataBase::Select(const std::string& request) const
{
	SelectionRequestParser selection_builder(*this, request);
	std::string table_name;
	std::function<bool(const Table::row_type& row)> selector =
		selection_builder.BuildSelector(table_name);

	Table::selection_type result;
	GetTableByName(table_name)->SelectRows(result, selector);
	return std::shared_ptr<Selection>(new Selection(result));
}