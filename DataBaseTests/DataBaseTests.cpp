#include <algorithm>
#include <stdexcept>

#include "CppUnitTest.h"

#include "DataBase.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DataBaseTests
{
	TEST_CLASS(DataBaseTests)
	{
	public:

		TEST_METHOD(AddTable)
		{
			DataBase db;
			db.AddTable("table1", { {"col1", "Int"}, {"col2", "Float"}, {"col3", "String"} });
			db.AddTable("table2", { { "col1", "Float" }, { "col2", "Int" }, { "col3", "String" } });
			db.AddTable("table3", { { "col1", "String" }, { "col2", "Int" }, { "col3", "Float" } });

			try
			{
				db.AddTable("table3", { { "col1", "String" },{ "col2", "Int" },{ "col3", "Float" } });
				Assert::Fail();
			}
			catch (const std::runtime_error&)
			{
			}

			try
			{
				db.AddTable("table4", { { "col1", "String" },{ "col2", "SomeInvalidType" },{ "col3", "Float" } });
				Assert::Fail();
			}
			catch (const std::runtime_error&)
			{
			}

			std::vector<std::string> table_names;
			db.GetTableNames(table_names);
			Assert::AreEqual(static_cast<size_t>(3), table_names.size());

			for (const std::string& name : { "table1", "table2", "table3" })
			{
				Assert::IsTrue(std::find(table_names.begin(),
										 table_names.end(),
										 name) != table_names.end());

				const Table* table = db.GetTableByName(name);
				Table::columns_type columns = table->GetColumns();
				Assert::AreEqual(static_cast<size_t>(3), columns.size());

				for (size_t i = 0; i != columns.size(); ++i)
				{
					Assert::AreEqual("col" + std::to_string(i + 1), columns[i].GetName());
				}
			}
		}

		TEST_METHOD(AddRows)
		{
			DataBase db;
			db.AddTable("table1", { { "col1", "Int" }, { "col2", "Float" }, { "col3", "String" } });
			db.AddTable("table2", { { "col1", "Float" }, { "col2", "Int" }, { "col3", "String" } });

			try
			{
				db.AddRow("table1", { "25.4", "25.4", "Invalid row" });
				Assert::Fail();
			}
			catch (const std::runtime_error&)
			{
			}

			try
			{
				db.AddRow("table2", { "45.3not a float", "15", "Invalid row" });
				Assert::Fail();
			}
			catch (const std::runtime_error&)
			{
			}

			std::vector<Table::row_type> table1_rows =
			{
				{ "10", "5.8", "Hello, world!" },
				{ "20", "3.1", "Microsoft" },
				{ "15", "234.1", "First method" }
			};

			std::vector<Table::row_type> table2_rows =
			{
				{ "14.25", "45", "Bois" },
				{ "7.33", "50", "Linux" },
				{ "1286.3", "54", "OS Luna" }
			};

			for (const Table::row_type& row : table1_rows)
			{
				db.AddRow("table1", row);
			}
			for (const Table::row_type& row : table2_rows)
			{
				db.AddRow("table2", row);
			}

			Table::selection_type selection;

			db.GetTableByName("table1")->SelectRows(selection);
			Assert::AreEqual(table1_rows.size(), selection.size());
			Assert::IsTrue(std::equal(table1_rows.begin(), table1_rows.end(),
									  selection.begin(), selection.end(),
									  [](const Table::row_type& row, Table::row_iter_type row_iter) -> bool
			{
				return row == *row_iter;
			}));

			selection.clear();

			db.GetTableByName("table2")->SelectRows(selection);
			Assert::AreEqual(table2_rows.size(), selection.size());
			Assert::IsTrue(std::equal(table2_rows.begin(), table2_rows.end(),
									  selection.begin(), selection.end(),
									  [](const Table::row_type& row, Table::row_iter_type row_iter) -> bool
			{
				return row == *row_iter;
			}));
		}

		TEST_METHOD(SelectRows)
		{
			DataBase db;
			db.AddTable("table1", { { "col1", "Int" },{ "col2", "Float" },{ "col3", "String" } });
			std::vector<Table::row_type> table1_rows =
			{
				{ "10", "5.8", "Hello, world!" },
				{ "20", "3.1", "Microsoft" },
				{ "10", "234.1", "First method" },
				{ "10", "12.65", "Second method" }
			};
			for (const Table::row_type& row : table1_rows)
			{
				db.AddRow("table1", row);
			}

			auto selection = db.Select("select from table1");

			Assert::AreEqual(table1_rows.size(), selection->size());

			int i = 0;
			selection->ForEach([&table1_rows, &i]
			(const Table::row_type& row) -> bool
			{
				Assert::IsTrue(table1_rows[i] == row);
				++i;
				return true;
			});
			Assert::AreEqual(table1_rows.size(), static_cast<size_t>(i));
		}

		TEST_METHOD(SelectRowsWithEqCondition)
		{
			DataBase db;
			db.AddTable("table1", { { "col1", "Int" }, { "col2", "Float" }, { "col3", "String" } });
			std::vector<Table::row_type> table1_rows =
			{
				{ "10", "5.8", "Hello, world!" },
				{ "20", "3.1", "Microsoft" },
				{ "10", "234.1", "First method" },
				{ "10", "12.65", "Second method" }
			};
			for (const Table::row_type& row : table1_rows)
			{
				db.AddRow("table1", row);
			}

			auto selection = db.Select("select from table1 where col1 = 10");

			Assert::AreEqual(static_cast<size_t>(3), selection->size());

			int i = 0;
			std::vector<int> matched_indexes = { 0, 2, 3 };
			selection->ForEach([&table1_rows, &matched_indexes, &i]
			(const Table::row_type& row) -> bool
			{
				Assert::IsTrue(table1_rows[matched_indexes[i]] == row);
				++i;
				return true;
			});
			Assert::AreEqual(3, i);
		}

		TEST_METHOD(SelectRowsWithSubSelect)
		{
			DataBase db;

			db.AddTable("books", { { "id", "Int" }, { "year", "Int" }, { "author", "String" } });
			std::vector<Table::row_type> books_rows =
			{
				{ "1", "1892", "Crafter" },
				{ "2", "1965", "Jones" },
				{ "3", "2004", "Springer" },
				{ "4", "1832", "Bafers" }
			};
			for (const Table::row_type& row : books_rows)
			{
				db.AddRow("books", row);
			}

			db.AddTable("students", { {"name", "String"}, {"book_id", "Int"} });
			std::vector<Table::row_type> students_rows =
			{
				{ "Franklin", "2" },
				{ "Jobs", "3" },
				{ "Trump", "1" },
				{ "White", "4" }
			};
			for (const Table::row_type& row : students_rows)
			{
				db.AddRow("students", row);
			}

			auto selection = db.Select("select from students where book_id = "
									   "$(select books.id where author = Springer)");

			Assert::AreEqual(static_cast<size_t>(1), selection->size());

			int i = 0;
			selection->ForEach([&students_rows, &i]
			(const Table::row_type& row) -> bool
			{
				Assert::IsTrue(students_rows[1] == row);
				++i;
				return true;
			});
			Assert::AreEqual(1, i);
		}
	};
}