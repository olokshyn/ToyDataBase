#include "Selection.h"



Selection::Selection(const Table::selection_type& selection_result)
	: m_selection(selection_result)
{
}

void Selection::ForEach(std::function<bool(const std::vector<std::string>& row)> callback) const
{
	for (const Table::row_iter_type& it : m_selection)
	{
		if (!callback(*it))
		{
			break;
		}
	}
}

void Selection::WriteToStream(std::ostream& stream, 
							  char value_delimeter,
	                          char row_delimeter) const
{
	for (const Table::row_iter_type& it : m_selection)
	{
		for (const std::string& value : *it)
		{
			stream << value << value_delimeter;
		}
		stream << row_delimeter;
	}
}