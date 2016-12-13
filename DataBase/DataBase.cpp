#include "DataBase.h"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <stdexcept>

#include "SelectionRequestParser.h"
#include "Utils.h"

std::unique_ptr<const TypeManager> g_type_manager(nullptr);

DataBase::DataBase()
{
	if (!g_type_manager)
	{
		g_type_manager.reset(new TypeManager);
	}
}

DataBase::DataBase(const std::string& db_root)
{
	if (!g_type_manager)
	{
		g_type_manager.reset(new TypeManager);
	}
	Load(db_root);
}

void DataBase::Save(const std::string& db_root)
{
	if (Utils::DirExists(db_root))
	{
		if (!Utils::RemoveDir(db_root))
		{
			throw std::runtime_error("Failed to DataBase directory: " + db_root);
		}
	}
	if (!Utils::CreateDir(db_root))
	{
		throw std::runtime_error("DataBase failed to create directory " + db_root);
	}

	std::ofstream table_stream;
	for (map_type::const_iterator it = m_tables.begin();
		 it != m_tables.end();
		 ++it)
	{
		Utils::AddFileToDir(db_root, it->first, table_stream);
		it->second->SaveToStream(table_stream, g_type_manager.get());
	}
}

void DataBase::Load(const std::string& db_root)
{
	if (!Utils::DirExists(db_root))
	{
		throw std::runtime_error("DataBase directory does not exist: " + db_root);
	}
	std::vector<std::string> table_names;
	Utils::ListDir(db_root, table_names);
	if (table_names.empty())
	{
		throw std::runtime_error("DataBase: no tables found in " + db_root);
	}

	std::ifstream table_file;
	for (const std::string& table_name : table_names)
	{
		Utils::OpenFileInDir(db_root, table_name, table_file);
		m_tables.emplace(std::make_pair(table_name, 
										std::move(std::unique_ptr<Table>(
											new Table(table_file, g_type_manager.get())))));
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
		if (std::find(type_names.begin(), type_names.end(), col.second) 
			== type_names.end())
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
									std::move(std::unique_ptr<Table>(
										new Table(typed_columns)))));
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
	std::string sort_by_column;
	bool sort_ascending;
	std::function<bool(const Table::row_type& row)> selector =
		selection_builder.BuildSelector(table_name,
										sort_by_column,
										sort_ascending);

	Table::selection_type result;
	const Table* table = GetTableByName(table_name);
	table->SelectRows(result, selector, sort_by_column, sort_ascending);
	return std::shared_ptr<Selection>(new Selection(table, result));
}

bool operator==(const DataBase& left, const DataBase& right)
{
	return std::equal(left.m_tables.begin(), 
					  left.m_tables.end(), 
					  right.m_tables.begin(), 
					  right.m_tables.end(), 
					  [](const DataBase::pair_type& left, const DataBase::pair_type& right) -> 
					  bool {
		return left.first == right.first && *left.second == *right.second;
	});
}

bool operator!=(const DataBase& left, const DataBase& right)
{
	return !operator==(left, right);
}