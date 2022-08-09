#ifndef CBASE_H
#define CBASE_H
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <libtcod.h>

#include <filesystem>

enum GameMode
{
	MODE_MAINMENU,
	MODE_PLANETVIEW,
	MODE_WORLDVIEW,
};

enum WorldBiome
{
// cold
	BIOME_ROCKBEACH = 1,
	BIOME_ROCKPLAINS,
	BIOME_MOUNTAIN,

// temperate
	BIOME_BEACH,
	BIOME_PLAINS,
	BIOME_HILLS,

// hot
	BIOME_DESERT,
	BIOME_FOREST,
	BIOME_JUNGLE,

// special
	BIOME_WATER,
       
	BIOME_NULL_SIZE,
};

#define __unused_argument(x) (void)(x)

std::filesystem::path system_data_dir();
uint64_t get_game_ticks();
void inc_game_ticks();
void set_system_message(const char* message);
const char* get_system_message();

void start_text_input();
void stop_text_input();

extern char* sdl_textinput_text;
extern char* sdl_textinput_composition;
extern int sdl_textinput_cursor;
extern int sdl_textinput_composition_len;
extern bool sdl_textinput_on;

extern GameMode currentMode;
	
#ifdef WIZARD
#define ENABLE_IF_WIZARD || true
#else
#define ENABLE_IF_WIZARD
#endif

extern bool suppress_output;
void __etheria_log(const char* fancy_function,
		   const char* format,...);
#define etheria_log(...)			\
	__etheria_log(__PRETTY_FUNCTION__, __VA_ARGS__);

static constexpr std::array<int, 9> frame_legend = {
	0x250c, 0x2500, 0x2510,
        0x2502, ' ',    0x2502,
	0x2514, 0x2500, 0x2518};

#endif

