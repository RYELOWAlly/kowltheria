#include "menus.hpp"
#include "help.hpp"

void HelpMenu::HandleKeyPress(SDL_Keycode code)
{
	if(!infopage)
		Menu::HandleKeyPress(code);
	else
		if(code == SDLK_ESCAPE)
			infopage = false;
}

