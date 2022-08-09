#ifndef HELP_HPP
#define HELP_HPP

#include "menus.hpp"

class HelpMenu : public Menu
{
public:
	HelpMenu();

	virtual void Render(tcod::Console &console);
	virtual void Click();
	virtual void HandleKeyPress(SDL_Keycode code);
private:

	bool infopage; // true if reading about specific thing
};

#endif
