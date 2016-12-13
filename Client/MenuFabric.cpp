#include "MenuFabric.h"

#include <iostream>
#include <stdexcept>

#include "Utils.h"
#include "UIHelpers.h"

std::unique_ptr<MenuManager> MenuFabric::manager(nullptr);

MenuManager* MenuFabric::GetManager()
{
	if (!manager)
	{
		manager.reset(new MenuManager({
			Menu("Welcome to DataBase:",
			{
				"Load DB from filesystem",
				"Create DB"
			},
			[](int choice, Context& context) ->
			int {
				if (choice == 0)
				{
					std::cout << "Enter the path to DB: ";
					std::string db_root;
					std::cin >> db_root;
					context.database.reset(new DataBase(db_root));
					std::cout << "DataBase loaded successfully" << std::endl;
					return 1;
				}
				else if (choice == 1)
				{
					context.database.reset(new DataBase());
					std::cout << "Created empty DataBase" << std::endl;
					return 1;
				}
				throw std::runtime_error("Invalid choice");
			}),

			Menu("Working with tables:",
			{
					"Show tables",
					"Add table",
					"Add row to table",
					"Make selection",
					"Save DB to the file",
					"Back"
			},
			[](int choice, Context& context) ->
			int {
			if (choice == 0)
			{
				std::vector<std::string> table_names;
				context.database->GetTableNames(table_names);
				for (const std::string& name : table_names)
				{
					std::cout << name << ':' << std::endl;
					UI::PrintTableColumns(context.database->GetTableByName(name),
										  g_type_manager.get(), true);
				}
				return 1;
			}
			else if (choice == 1)
			{
				std::cout << "Enter table name: ";
				std::string name;
				std::cin >> name;

				std::cout << "Enter columns {Name, Type:[Int|Float|String]} ..." << std::endl;
				std::vector<std::string> columns;
				UI::ReadLine(columns);
				std::vector< std::pair<std::string, std::string> > splitted_columns;
				Utils::SplitByPairs(columns, splitted_columns);

				context.database->AddTable(name, splitted_columns);
				std::cout << "Table added successfully" << std::endl;
				return 1;
			}
			else if (choice == 2)
			{
				std::cout << "Enter table name: ";
				std::string name;
				std::cin >> name;

				const Table* table = context.database->GetTableByName(name);

				std::cout << "Enter row:" << std::endl;
				UI::PrintTableHeader(table);
				std::vector<std::string> row;
				UI::ReadLine(row);

				context.database->AddRow(name, row);
				std::cout << "Row added successfully" << std::endl;
				return 1;
			}
			else if (choice == 3)
			{
				std::cout << "Enter request:" << std::endl;
				std::string request;
				std::getline(std::cin, request, '\n');

				auto selection = context.database->Select(request);
				UI::PrintSelection(*selection);
				return 1;
			}
			else if (choice == 4)
			{
				std::cout << "Enter the path to save DB to:" << std::endl;
				std::string db_root;
				std::cin >> db_root;

				context.database->Save(db_root);
				std::cout << "DataBase saved successfully" << std::endl;
				return 1;
			}
			else if (choice == 5)
			{
				return 0;
			}
			throw std::runtime_error("Invalid choice");
			})

		}));
	}
	return manager.get();
}