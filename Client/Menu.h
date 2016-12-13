#pragma once

#include <string>
#include <vector>
#include <functional>

#include "Context.h"

class Menu
{
public:
	Menu(const std::string& title,
		 const std::vector<std::string>& options, 
		 std::function<int(int, Context&)> handler);

	int ShowMenu(Context& context, int current_index) const;

private:
	std::string m_title;
	std::vector<std::string> m_options;
	std::function<int(int, Context&)> m_handler;
};

