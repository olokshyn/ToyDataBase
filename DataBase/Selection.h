#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ostream>

#include "Table.h"

class Selection
{
public:
	explicit Selection(const Table::selection_type& selection_result);

	void ForEach(std::function<bool(
		const std::vector<std::string>& row)> callback) const;

	void WriteToStream(std::ostream& stream, char value_delimeter = ' ', 
					   char row_delimeter = '\n') const;

	size_t size() const
	{
		return m_selection.size();
	}

private:
	const Table::selection_type m_selection;
};

