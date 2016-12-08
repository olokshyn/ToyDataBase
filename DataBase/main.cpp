#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "DataBase.h"

using namespace std;

int menu(const char* title, const char* options[], int options_count)
{
	cout << title << endl;  // Вывести заголовок
	for (;;)
	{
		for (int i = 0; i != options_count; ++i)  // Вывести пункты меню с номерами
		{
			cout << i + 1 << ". " << options[i] << endl;
		}
		int choice;
		if (!(cin >> choice) || choice < 1 || choice > options_count)  // Ввести номер, выбранный пользователем
		{
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Введите номер от 1 до " << options_count << endl;
			continue;
		}
		return choice;
	}
}

const char* menu1[] = {
	"Добавить запись.",
	"Найти запись.",
	"Редактировать.",
	"Выход."
};

const char* menu2[] = {
	"Перейти к прошлой записи.",
	"Перейти к следующей записи.",
	"Перейти в начало.",
	"Перейти в конец.",
	"Выход."
};

void menu_loop()
{
	setlocale(LC_ALL, "Russian");
	for (;;)
	{
		int choice = menu("Выберите действие, которое хотите осущесвить с базой данных:", menu1, sizeof(menu1) / sizeof(menu1[0]));
		if (choice == 2)
		{
			for (;;)
			{
				choice = menu("Эта возможность еще не реализована \n Режим просмотра записей ", menu2, sizeof(menu2) / sizeof(menu2[0]));
				if (choice == 5)
				{
					break;
				}
				cout << "Эта возможность еще не реализована." << endl;
			}
		}
		else if (choice == 4)
		{
			break;
		}
		cout << "Эта возможность еще не реализована." << endl;
	}
}

const char* test_menu[] =
{
	"Show tables",
	"Add table",
	"Add row",
	"Select rows",
	"Exit"
};



int main()
{
	DataBase db;
	try
	{
		for (;;)
		{
			int choice = menu("DataBase: ", test_menu, sizeof(test_menu) / sizeof(test_menu[0]));
			if (choice == 1)
			{
				std::vector<std::string> table_names;
				db.GetTableNames(table_names);
				std::copy(table_names.begin(), table_names.end(),
						  std::ostream_iterator<std::string>(std::cout, " "));
				std::cout << std::endl;
			}
			else if (choice == 2)
			{
				/*std::cout << "Enter table name: ";
				std::string table_name;
				std::cin >> table_name;

				std::cout << "Enter columns like Name:Type, supported types:" << std::endl;
				std::vector<std::string> type_names;
				db.GetTypeNames(type_names);
				std::copy(type_names.begin(), type_names.end(),
						  std::ostream_iterator<std::string>(std::cout, " "));
				std::cout << std::endl;

				std::vector< std::pair<std::string, std::string> > columns;
				std::string name;
				std::string type;
				while (std::getline(std::cin, name, ':') && std::getline(std::cin, type, ' '))
				{
					columns.emplace_back(std::make_pair(name, type));
				}*/
				db.AddTable("table1", { {"col1", "Int"}, { "col2", "Float" } });
			}
			else if (choice == 3)
			{
				/*cout << "Chose table:" << endl;
				std::vector<std::string> table_names;
				db.GetTableNames(table_names);
				std::copy(table_names.begin(), table_names.end(),
						  std::ostream_iterator<std::string>(std::cout, " "));
				std::cout << std::endl;

				string table_name;
				cin >> table_name;

				vector<string> row;
				copy(istream_iterator<string>(cin),
					 istream_iterator<string>(),
					 back_inserter(row));*/
				db.AddRow("table1", { "10", "2.5" });
			}
			else if (choice == 4)
			{
				cout << "Enter request: " << endl;
				string request = "select from table1";
				auto selection = db.Select(request);
				selection->ForEach([](const Table::row_type& row) 
				{ 
					copy(row.begin(), 
						 row.end(), 
						 ostream_iterator<string>(cout, " ")); 
					return true; 
				});
			}
			else if (choice == 5)
			{
				break;
			}
		}
	}
	catch (const std::exception& ex)
	{
		cout << "Exception: " << ex.what() << endl;
	}

	system("pause");
	return 0;
}