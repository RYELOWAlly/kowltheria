#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <libtcod.hpp>
#include <SDL.h>
#include "world.hpp"

#define STATUS_BLIND    0b000000000001
#define STATUS_MAD      0b000000000010
#define STATUS_CRIPPLED 0b000000000100
#define STATUS_FROZEN   0b000000001000
#define STATUS_HALLU    0b000000010000
#define STATUS_DEAF     0b000000100000
#define STATUS_MUTE     0b000001000000
#define STATUS_MYOPIA   0b000010000000
#define STATUS_DEAD     0b000100000000
#define STATUS_POISON   0b001000000000
#define STATUS_CONCUSS  0b010000000000

#define ABILITY_SEEINVISIBLE 0b000001
#define ABILITY_XRAY         0b000010
#define ABILITY_FARSIGHT     0b000100

class FloorItem
{
public:
	int x, y;
	FloorItem(int x, int y, ItemType type);
	ItemType type;
};

class Actor
{
public:
	int x, y;
	int ch;
	int hp;
	int hpmax;
	int level;
	int groundlevel;
	int histfig_id;
	// permanent status effects, will be copied
	// to astatus every tick 
	uint64_t pstatus;
	// semi-permanent status effects
	uint64_t astatus;

	bool pcontrolmode;

	uint64_t abilities;
	tcod::ColorRGBA col;
	const char* name;
	const char* uiname;
	World* world;
	
	Actor(const char* name,
	      int x, int y, int ch, World* world,
	      const tcod::ColorRGBA &col);
	~Actor();
	static Actor* GetFromName(const char* name);
	static Actor* GetFromHistfigId(int histfig_id);
	static void TickAllActors(World* world);
	void Tick();
	static void RenderAllActors(tcod::Console* cConsole, World* world);
	virtual void Render(tcod::Console* cConsole) const;
	virtual void RenderNameTag(tcod::Console* cConsole,
				   int xOff, int yOff) const;

	// player controls
	virtual void HandlePlayerKey(SDL_Keycode code);
	virtual void PrintStatusInfo(tcod::Console* cConsole,
				     int xOff, int yOff);
	virtual int CalculateSight();

	bool HasItem(ItemType type);
	void RemoveItem(ItemType type); // drop it on the floor
	void PickupItem(ItemType type); // if below pick up
protected:
	bool *itemlist;
};

#endif
