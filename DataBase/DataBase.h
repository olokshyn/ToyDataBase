#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "Table.h"
#include "Selection.h"
#include "TypeManager.h"

extern std::unique_ptr<const TypeManager> g_type_manager;  // multithreading hazard

class DataBase
{
public:
	DataBase();

	void GetTypeNames(std::vector<std::string>& type_names) const
	{
		g_type_manager->GetTypeNames(type_names);
	}

	void GetTableNames(std::vector<std::string>& names) const;
	const Table* GetTableByName(const std::string& table_name) const;

	void AddTable(const std::string& table_name, 
				  const std::vector< std::pair<std::string, 
									 std::string> >& columns);

	void AddRow(const std::string& table_name, const Table::row_type& row);

	std::shared_ptr<Selection> Select(const std::string& request) const;

private:
	DataBase(const DataBase& other);
	DataBase& operator=(const DataBase& right);

private:
	typedef std::pair< const std::string, std::unique_ptr<Table> > pair_type;
	typedef std::map< std::string, std::unique_ptr<Table> > map_type;

private:
	map_type m_tables;
};

