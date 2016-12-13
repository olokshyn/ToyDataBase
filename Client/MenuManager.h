#pragma once

#include <vector>

#include "Menu.h"
#include "Context.h"

class MenuManager
{
public:
	MenuManager(const std::vector<Menu>& menus);
	
	int Interact();

private:
	std::vector<Menu> m_menus;
	Context m_context;
};

