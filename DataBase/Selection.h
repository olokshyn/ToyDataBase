#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ostream>

#include "Table.h"

class Selection
{
public:
	Selection(const Table* table,
			  const Table::selection_type& selection_result);

	const Table* GetTable() const { return m_table; }

	void ForEach(std::function<bool(
		const std::vector<std::string>& row)> callback) const;

	void WriteToStream(std::ostream& stream, char value_delimeter = ' ',
					   char row_delimeter = '\n') const;

	size_t size() const { return m_selection.size(); }

private:
	const Table* m_table;
	const Table::selection_type m_selection;
};

