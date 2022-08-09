#ifndef MENUS_HPP
#define MENUS_HPP

#include "cbase.hpp"
#include <libtcod.hpp>
#include <SDL.h>

class Menu
{
public:
	Menu();
	
	virtual void Render(tcod::Console &console);
	virtual void Click();
	virtual void HandleKeyPress(SDL_Keycode code);
protected:
	std::string* menu_entries;
	std::string name;
	int entry_num;
	int current_entry;
	int first_entry;
};

typedef void (*gamestart_callback)();

class MainMenu : public Menu
{
public:
	MainMenu(gamestart_callback callback);
	virtual void Click();
private:
	gamestart_callback callback;
};

#endif
