#include "MenuFabric.h"

int main()
{
	MenuManager* menu_manager = MenuFabric::GetManager();

	menu_manager->Interact();

	return 0;
}