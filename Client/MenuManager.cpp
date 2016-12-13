#include "MenuManager.h"

#include <stdexcept>


MenuManager::MenuManager(const std::vector<Menu>& menus)
	: m_menus(menus),
	m_context()
{
	m_context.database.reset(nullptr);
}


int MenuManager::Interact()
{
	int current_menu = 0;
	for (;;)
	{
		current_menu = m_menus[current_menu].ShowMenu(m_context, current_menu);
		if (current_menu == -1)
		{
			return 0;
		}
		if (current_menu < 0 || static_cast<size_t>(current_menu) >= m_menus.size())
		{
			throw std::runtime_error("Invalid menu index");
		}
	}
}