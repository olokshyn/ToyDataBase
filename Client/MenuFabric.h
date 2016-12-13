#pragma once

#include <memory>

#include "MenuManager.h"

class MenuFabric
{
public:
	static MenuManager* GetManager();

private:
	MenuFabric();

	static std::unique_ptr<MenuManager> manager;
};

