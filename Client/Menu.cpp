#include "Menu.h"

#include <iostream>
#include <stdexcept>

#include "UIHelpers.h"

Menu::Menu(const std::string& title,
		   const std::vector<std::string>& options,
		   std::function<int(int, Context&)> handler)
	: m_title(title),
	m_options(options), 
	m_handler(handler)
{
}


int Menu::ShowMenu(Context& context, int current_index) const
{
	try
	{
		std::cout << m_title << std::endl;
		int choice = UI::ChooseOption(m_options, true);
		if (choice == -1)
		{
			return -1;
		}
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		int next_menu = m_handler(choice, context);
		if (next_menu != current_index)
		{
			std::cout << std::endl;
		}
		else
		{
			std::cin.clear();
			std::cin.ignore(1000, '\n');
		}

		return next_menu;
	}
	catch (const std::runtime_error& err)
	{
		std::cout << "Error: " << err.what() 
			<< std::endl << std::endl;
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}
	return current_index;
}