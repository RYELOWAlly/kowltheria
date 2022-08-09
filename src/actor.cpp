#include "cbase.hpp"
#include "actor.hpp"
#include "templates.hpp"
#include "culture.hpp"
#include <vector>
#include <algorithm>

static std::vector<Actor*> vActorList;

Actor::Actor(const char* name,
	     int x, int y,
	     int ch,
	     World* world,
	     const tcod::ColorRGBA &col)
{
	this->col = col;
	this->x = x;
	this->y = y;
	this->ch = ch;
	this->name = name;
	this->uiname = "???";
	this->world = world;

	etheria_log("new actor %s, placed at %i x %i, my character is"
		    " %c, i'm in world %p\n", name, x, y, ch, world);

	pstatus = 0;
	astatus = 0;
	abilities = 0;
	hp = 3;
	hpmax = 3;
	level = 0;
	groundlevel = 0;
	histfig_id = -1;
	itemlist = new bool[ITEMTYPE_NULL_SIZE];
	for(int i = 0; i < ITEMTYPE_NULL_SIZE; i++)
		itemlist[i] = false;
	
	pcontrolmode = false;

	set_system_message("Welcome to Etheria.");
	
	vActorList.push_back(this);
}

Actor::~Actor()
{
	remove_from_vector(vActorList,this);
}

void Actor::Render(tcod::Console* cConsole) const
{
	if(x >= cConsole->get_width())
		return;
	if(y >= cConsole->get_height())
		return;
	
	TCOD_ConsoleTile& tile = cConsole->at({x,y});

	tile.fg = col;
	tile.bg = tcod::ColorRGBA(0,0,0,255);
	tile.ch = ch;
}

Actor* Actor::GetFromName(const char* name)
{
	for(size_t i = 0; i < vActorList.size(); i++)
	{
		if(strcmp(name,vActorList[i]->name) == 0)
		{
			return vActorList[i];
		}
	}
	return nullptr;
}

Actor* Actor::GetFromHistfigId(int histfig_id)
{
	for(size_t i = 0; i < vActorList.size(); i++)
	{
		if(histfig_id == vActorList[i]->histfig_id)
		{
			return vActorList[i];
		}
	}
	return nullptr;
}

void Actor::RenderAllActors(tcod::Console* cConsole, World* world)
{
	Actor* playerActor = Actor::GetFromName("player");
	int abilities = 0;
	if(playerActor)
		abilities = playerActor->abilities;
	for(size_t i = 0; i < vActorList.size(); i++)
	{
		if(vActorList[i]->world == world)
			if(world->IsInFov(vActorList[i]->x,
					  vActorList[i]->y) ||
			   (abilities & ABILITY_SEEINVISIBLE))
			{
				if(playerActor)
					if(playerActor->groundlevel !=
					   vActorList[i]->groundlevel)
						continue;
				
				vActorList[i]->Render(cConsole);
			}
	}
}

void Actor::TickAllActors(World* world)
{
	for(size_t i = 0; i < vActorList.size(); i++)
	{
		if(vActorList[i]->world == world)
			vActorList[i]->Tick();
	}
	inc_game_ticks();
}

void Actor::Tick()
{
	astatus |= pstatus;

	if(hp <= 0)
	{
		pstatus |= STATUS_DEAD;
			
	}
}

void Actor::HandlePlayerKey(SDL_Keycode code)
{
	set_system_message("");
	if(pcontrolmode)
	{
		// command received

		char* usable_text = sdl_textinput_text + 2;

		std::printf("%s",usable_text);

		if(strcmp(usable_text,"down")==0)
		{
			if(world->tiles[x + y * world->width].down)
			{
				world->Generate();
				groundlevel++;
			}
			else
				set_system_message("There is no way to"
						   "go down from here.");
		} else if(strcmp(usable_text,"dig")==0)
		{
			world->tiles[x + y * world->width].down =
				true;
			set_system_message("You dig stairs downwards.");
		} else if(strcmp(usable_text,"exit")==0)
		{
			currentMode = MODE_PLANETVIEW;
			set_system_message("You escape the dungeon.");
		}
		else
		{
			set_system_message("Unknown command");
		}
		
		pcontrolmode = false;
	}
	
	if(!(astatus & STATUS_DEAD))
	{
		switch(code)
		{
		case SDLK_KP_8:
		case SDLK_UP:
			if(!world->IsWall(x,y-1) ENABLE_IF_WIZARD)
				y--;
			else
				set_system_message("There is a wall to your north.");
			break;
		case SDLK_KP_2:
		case SDLK_DOWN:
			if(!world->IsWall(x,y+1) ENABLE_IF_WIZARD)
				y++;
			else
				set_system_message("There is a wall to your south.");
			break;
		case SDLK_KP_4:
		case SDLK_LEFT:
			if(!world->IsWall(x-1,y) ENABLE_IF_WIZARD)
				x--;
			else
				set_system_message("There is a wall to your left.");
			break;
		case SDLK_KP_6:
		case SDLK_RIGHT:
			if(!world->IsWall(x+1,y) ENABLE_IF_WIZARD)
				x++;
			else
				set_system_message("There is a wall to your right.");
			break;
		case SDLK_c:
			// enter command mode
			start_text_input();
			pcontrolmode = true;
			break;
		default:
			set_system_message("You cant do that.");
			break;
		}
	}
	else
	{
		set_system_message("You're dead. You can't do anything.");
	}
	
	world->ComputeFOV();
}

void Actor::RenderNameTag(tcod::Console* cConsole,
			  int xOff, int yOff) const
{
	if(currentMode == MODE_MAINMENU)
		return;
	
	tcod::print(*cConsole,
		    {xOff,yOff},
		    tcod::stringf("You are   %s",
				  uiname),
		    TCOD_white,
		    std::nullopt);
	TCOD_ConsoleTile& tile = cConsole->at({xOff+8,yOff});
	tile.fg = col;
	tile.ch = ch;
	tile.bg = tcod::ColorRGBA(0,0,0);
}

int Actor::CalculateSight()
{
	int base_sight = 30;

	if(abilities & ABILITY_FARSIGHT)
		base_sight = 0; // 0 = to the end
	
	if(astatus & STATUS_MYOPIA)
		if(HasItem(ITEMTYPE_PERSCRIPTIONGLASSES))
			base_sight /= 1.5;
		else
			base_sight /= 3.5;
	else
		if(HasItem(ITEMTYPE_PERSCRIPTIONGLASSES))
			base_sight /= 4.5;
	if(astatus & STATUS_BLIND)
		base_sight /= 1;

	return base_sight;
}

void Actor::PrintStatusInfo(tcod::Console* cConsole,
			    int xOff, int yOff)
{
	if(currentMode == MODE_MAINMENU)
		return;
	
	std::string statusLine;
	std::string abilityLine;
	std::string itemLine;
	if(astatus & STATUS_BLIND)
		statusLine += "Blind ";
	if(astatus & STATUS_MAD)
		statusLine += "Insan ";
	if(astatus & STATUS_CRIPPLED)
		statusLine += "Cripp ";
	if(astatus & STATUS_FROZEN)
		statusLine += "Frozn ";
	if(astatus & STATUS_HALLU)
		statusLine += "Hallu ";
	if(astatus & STATUS_DEAF)
		statusLine += "Deaf  ";
	if(astatus & STATUS_MUTE)
		statusLine += "Mute  ";
	if(astatus & STATUS_MYOPIA)
		statusLine += "Myopc ";
	if(astatus & STATUS_DEAD)
		statusLine += "Dead  ";
	if(astatus & STATUS_POISON)
		statusLine += "Poisn ";
	if(astatus & STATUS_CONCUSS)
		statusLine += "Concs ";

	if(astatus == 0)
		statusLine += "Normal";

	if(abilities & ABILITY_SEEINVISIBLE)
		abilityLine += "SeeIn ";
	if(abilities & ABILITY_XRAY)
		abilityLine += "X-Ray ";
	if(abilities & ABILITY_FARSIGHT)
		abilityLine += "FarSg ";

	if(abilities == 0)
		abilityLine += "Averg ";

	int itemsworn = 0;
	for(int i = 0; i < ITEMTYPE_NULL_SIZE; i++)
	{
		if(itemlist[i])
		{
			abilityLine += "E: ";
			abilityLine += get_name_for_type((ItemType)i);
			abilityLine += " ";
			itemsworn++;
		}
	}
	if(itemsworn == 0)
		abilityLine += "Naked";
	
#ifdef WIZARD
	if(x > 80 || y > 80 || 
	   x < 0 || y < 0)
		abilityLine += " WIZARD: out of map";
#endif
	
	tcod::print(*cConsole,
		    {xOff,yOff},
		    statusLine,
		    TCOD_white,
		    std::nullopt);
	
	tcod::print(*cConsole,
		    {xOff,yOff+1},
		    abilityLine,
		    TCOD_white,
		    std::nullopt);

	if(world)
	{
		tcod::print(*cConsole,
			    {xOff,yOff+2},
			    tcod::stringf(
				    "HP : %03i(%03i) "
				    "PLv: %02i "
				    "GLv: %02i "
				    "Sgt: %02im "
				    "Bme: %s",
				    hp, hpmax,
				    level,
				    groundlevel,
				    CalculateSight() ?
				    CalculateSight() :
				    99,
				    get_name_for_biome(
					    world->biome)
				    ),
			    TCOD_white,
			    std::nullopt);
	}
	else
	{
		tcod::print(*cConsole,
			    {xOff,yOff+2},
			    tcod::stringf(
				    "HP : %03i(%03i) "
				    "PLv: %02i "
				    "Sgt: %02im ",
				    hp, hpmax,
				    level,
				    CalculateSight() ?
				    CalculateSight() :
				    99
				    ),
			    TCOD_white,
			    std::nullopt);
	}
}

bool Actor::HasItem(ItemType type)
{
	return itemlist[type];
}

void Actor::RemoveItem(ItemType type)
{
	if(!itemlist[type])
	{
		set_system_message("You dont have that.");
	}
}

void Actor::PickupItem(ItemType type)
{
	if(itemlist[type])
	{
		set_system_message("You already have that. Drop it, first.");
	}
}

FloorItem::FloorItem(int x, int y, ItemType type)
{
	this->x = x;
	this->y = y;
	this->type = type;
}
