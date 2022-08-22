#include "cbase.hpp"
#include <stdarg.h>

#include <filesystem>

std::filesystem::path system_data_dir()
{
	std::filesystem::path root_directory = std::filesystem::path{"."};

	while(!std::filesystem::exists(root_directory / "data"))
	{
		root_directory /= "..";
		if(!std::filesystem::exists(root_directory))
		{
			throw std::runtime_error("data directory not found");
		}
	}
	
	return root_directory / "data";
}

static uint64_t game_ticks = 0;

uint64_t get_game_ticks()
{
	return game_ticks;
}

void inc_game_ticks()
{
	game_ticks++;
}

const char* system_message = "";

void set_system_message(const char* message)
{
	system_message = message;
}

const char* get_system_message()
{
	return system_message;
}

char* sdl_textinput_text = 0;
char* sdl_textinput_composition = 0;
int sdl_textinput_cursor = 0;
int sdl_textinput_composition_len = 0;
bool sdl_textinput_on = false;
GameMode currentMode = MODE_MAINMENU;

void start_text_input()
{
	sdl_textinput_on = true;
	sdl_textinput_text = (char*)malloc(256);
	strcpy(sdl_textinput_text,"c:");
	SDL_StartTextInput();
}

void stop_text_input()
{
	sdl_textinput_on = false;
	SDL_StopTextInput();
}

bool suppress_output = false;

void __etheria_log(const char* fancy_function, const char* format,...)
{
	if(!suppress_output)
	{
		va_list args;
		va_start(args, format);
		printf("%s: ", fancy_function);
		vprintf(format, args);
		va_end(args);
	}
}
