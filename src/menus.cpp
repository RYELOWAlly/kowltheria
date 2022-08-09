#include "menus.hpp"

Menu::Menu()
{

}

void Menu::Render(tcod::Console &console)
{	
	int console_base_x = 1;
	int console_base_y = 1;
	int max_chara_len = 0;
	
	for(int i = 0; i < entry_num; i++)
	{
		TCOD_color_t tcolor = TCOD_white;
		if(i < first_entry)
			tcolor = TCOD_red;
		if(i == current_entry)
			tcolor = TCOD_yellow;
		
		tcod::print(console,
			    {console_base_x, console_base_y},
			    menu_entries[i],
			    tcolor,
			    std::nullopt);

		console_base_y++;
		if(console_base_y >= 40)
		{
			console_base_x += max_chara_len;
			console_base_y = 1;
		}
	}
}

void Menu::Click()
{
	
}

void Menu::HandleKeyPress(SDL_Keycode code)
{
	switch(code)
	{
	case SDLK_UP:
		if(current_entry - 1 > first_entry)
			current_entry--;
		break;
	case SDLK_DOWN:
		if(current_entry + 1 < entry_num)
			current_entry++;
		break;
	case SDLK_RETURN:
		Click();
		break;
	}
}

MainMenu::MainMenu(gamestart_callback callback) : Menu()
{
	entry_num = 6;
	menu_entries = new std::string[entry_num];
	menu_entries[0] = "ETHERIA";
	menu_entries[1] = "Hermits of Kowltheria";
	menu_entries[2] = "BETA";
	menu_entries[3] = " ";
	menu_entries[4] = "Main Menu";
	menu_entries[5] = "Quit";

	first_entry = 3;
	current_entry = first_entry + 1;

	this->callback = callback;
}

void MainMenu::Click()
{
	if(current_entry == 4)
	{
		// enter world
		callback();
	}
	if(current_entry == 5)
	{
		std::exit(0);
	}
}
