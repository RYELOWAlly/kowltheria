#include "cbase.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 4297)
#endif

#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <libtcod.hpp>
#include "world.hpp"
#include "actor.hpp"
#include "planet.hpp"
#include "menus.hpp"
#include "save.hpp"

static tcod::Console gcConsole;
static tcod::Context gcContext;

static Planet* gameWorld;
static Menu* mainMenu;
static Menu* currentMenu;
static Menu* helpMenu;
static SaveFile save;

bool mainLoop = true;
bool ascendingTime = false;
int ascendingTimeEnd = 0;

void etherloop()
{
	Actor* playerActor = Actor::GetFromName("player");

	gcConsole.clear();

	if(currentMode == MODE_WORLDVIEW)
	{
		gameWorld->Tick();
		gameWorld->Render(&gcConsole);
	}
	// Actor::TickAllActors();
	// Actor::RenderAllActors(&gcConsole, gameWorld);
	else if(currentMode == MODE_MAINMENU || currentMode == MODE_PLANETVIEW)
	{
		#ifdef WIZARD
		tcod::print(gcConsole,
			    {40,46},
			    "Wizard mode\n"
			    "endoh.ca Preview\n"
			    "Etheria beta, compiled " __DATE__ "\n",
			    TCOD_grey,
			    std::nullopt);
		#endif
		if(currentMenu)
			currentMenu->Render(gcConsole);

		#ifdef WIZARD
		
		#endif
	}
	
	if(ascendingTime)
	{
		SDL_Event event;
		if(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				ascendingTime = false;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym ==
				   SDLK_ESCAPE)
					ascendingTime = true;
				break;
			}
		}
		if(gameWorld->playerCulture->current_year >= ascendingTimeEnd)
		{
			ascendingTime = false;
			save.SaveWorld(*gameWorld);
			save.WriteFile((char*)(system_data_dir() / "save.dat").c_str());
		}
		gameWorld->playerCulture->AscendStartTime();
	}
	
	if(playerActor)
	{
		playerActor->RenderNameTag(&gcConsole,
					   0,45);
		playerActor->PrintStatusInfo(&gcConsole,
					     0,46);
	}

	tcod::print(gcConsole,
		    {0,49},
		    tcod::stringf("%s",
				  get_system_message()),
		    TCOD_grey,
		    std::nullopt);

	if(sdl_textinput_on)
	{
		tcod::print(gcConsole,
			    {0,0},
			    tcod::stringf("%s",
					  sdl_textinput_text),
			    TCOD_yellow,
			    TCOD_grey);
		tcod::print(gcConsole,
			    {0,1},
			    "dig, down, pickup <type>,...",
			    TCOD_white,
			    TCOD_grey);
	}
				 
	
	gcContext.present(gcConsole);

	if(!ascendingTime)
	{
		SDL_Event event;
#ifndef __EMSCRIPTEN__
		SDL_WaitEvent(nullptr);
#endif
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				mainLoop = false;
				break;
			case SDL_KEYDOWN:
			{
				int vk = event.key.keysym.sym;
				if(sdl_textinput_on)
				{
					if(vk == SDLK_RETURN)
					{
						stop_text_input();
					}
				}
				else
				{
					if(currentMode == MODE_WORLDVIEW)
					{
						if(playerActor)
							playerActor->HandlePlayerKey(vk);
					} else if(currentMode ==
						  MODE_MAINMENU ||
						  currentMode ==
						  MODE_PLANETVIEW)
					{
						currentMenu->HandleKeyPress(vk);
					}
				}
			}
		        break;
			case SDL_TEXTINPUT:
				if(!sdl_textinput_on)
					break;
				std::printf("text input\n");
				strcat(sdl_textinput_text,
				       event.text.text);
				break;
			case SDL_TEXTEDITING:
				if(!sdl_textinput_on)
					break;
				std::printf("text editing\n");
				sdl_textinput_composition = event.edit.text;
				sdl_textinput_cursor = event.edit.start;
				sdl_textinput_composition_len = event.edit.length;
				break;
			default:
				break;
			}
		}
	}
}

int mouseevent_filter(void* a, SDL_Event* event)
{
	__unused_argument(a);
	
	if (event->type == SDL_MOUSEMOTION)
		return 0;
	return 1;
}

void gamestart()
{
	if(save.LoadFile((char*)(system_data_dir() /
				 "save.dat").c_str()))
	{
		save.UpdateWorld(*gameWorld);
		ascendingTime = false;
	}
	else
	{
		for(int i = 0; i < 5; i++)
			gameWorld->NewDungeonLocation();
		ascendingTime = true;
		ascendingTimeEnd = gameWorld->playerCulture->current_year +
			1;
		save.SaveWorld(*gameWorld);
		save.WriteFile((char*)(system_data_dir() / "save.dat").c_str());
	}
	currentMode = MODE_PLANETVIEW;
	currentMenu = new PlanetMenu(gameWorld);
}

int render_type = 0;

class ParserCfgListener : public ITCODParserListener
{
public:
	virtual bool parserNewStruct(TCODParser *parser,
				     const TCODParserStruct *str,
				     const char *name)
	{
		etheria_log("cfg struct: %s\n", name);
		return true;
	}
	virtual bool parserFlag(TCODParser *parser,const char *name)
	{
		etheria_log("cfg flag: %s\n", name);
		return true;
	}
	virtual bool parserProperty(TCODParser *parser,
				    const char *name,
				    TCOD_value_type_t type, TCOD_value_t value)
	{
		etheria_log("cfg value: %s\n", name);

		if(strcmp(name,"render_mode") == 0)
		{
			char* val = value.s;
			if(strcmp(val,"sdl2") == 0)
			{
				render_type = TCOD_RENDERER_SDL2;
			} else if(strcmp(val,"xterm") == 0)
			{
				render_type = TCOD_RENDERER_XTERM;
			} else if(strcmp(val,"sdl") == 0)
			{
				render_type = TCOD_RENDERER_SDL;
			} else if(strcmp(val,"glsl") == 0)
			{
				render_type = TCOD_RENDERER_GLSL;
			} else
			{
				etheria_log("cfg value some bogus, using sdl\n");
				render_type = TCOD_RENDERER_SDL;
			}
			etheria_log("cfg: %s\n", val);
		}
		return true;
	}
	virtual bool parserEndStruct(TCODParser *parser,
				     const TCODParserStruct *str,
				     const char *name)
	{
		etheria_log("cfg end struct: %s\n", name);
		return true;
	}
	virtual void error(const char *msg)
	{
		etheria_log("error in cfg: %s",msg);
	}

};

int main(int argc, char** argv)
{
	TCODParser cfgparser = TCODParser();
	TCODParserStruct *renderSettings = cfgparser.newStructure("render");
	TCODParserStruct *renderType =
		renderSettings->addProperty("render_mode",
					    TCOD_TYPE_STRING, true);
	cfgparser.run((system_data_dir() / "main.cfg").c_str()
		      ,new ParserCfgListener());
	
	TCOD_ContextParams params = TCOD_ContextParams();
	params.tcod_version = TCOD_COMPILEDVERSION;
	params.argc = argc;
	params.argv = argv;
	params.renderer_type = render_type;
	suppress_output = (params.renderer_type == TCOD_RENDERER_XTERM);
	params.vsync = 1;
	//params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
	params.window_title = "Etheria";

	etheria_log("system_data_dir() = %s\n", system_data_dir().c_str());

	tcod::Tileset tTileset =
		tcod::load_tilesheet(system_data_dir() /
				     "Herrbdog_7x7_tileset.png",
				     {16, 16}, tcod::CHARMAP_CP437);
				     params.tileset = tTileset.get();

	gcConsole = tcod::Console{80,50};
	params.console = gcConsole.get();

	save = SaveFile();
	
	Culture::Init();
	
	mainMenu = new MainMenu(gamestart);
	currentMenu = mainMenu;
	gameWorld = new Planet();

	currentMode = MODE_MAINMENU;

	SDL_SetEventFilter(mouseevent_filter, NULL);
	
	gcContext = tcod::Context(params);
	
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(etherloop, 0, 0);
#else
	while (mainLoop) etherloop();
#endif

	save.SaveWorld(*gameWorld);
	save.WriteFile((char*)(system_data_dir() /
			       "save.dat").c_str());
}
