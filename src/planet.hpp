#ifndef PLANET_HPP
#define PLANET_HPP

#include "world.hpp"
#include "actor.hpp"
#include "menus.hpp"
#include "culture.hpp"
#include "rng.hpp"

#define MAX_LOADED_WORLDS 255

struct DungeonLocation
{
        int latitude;
	int longitude;
	WorldBiome biome;
};

class Planet
{
private:
	friend class SaveFile;
	std::vector<World*> worldsLoaded;
	
	World* currentWorld;
	Actor* playerActor;
public:
	Culture* playerCulture;
	DungeonLocation* locations;
	int location_count;
	RNG* planetRNG;
	
	Planet();

	DungeonLocation NewDungeonLocation();
	void Generate();

	void SetCurrentWorld(World* world_ptr);
	void SetActorToWorld(World* world_ptr, Actor* actor);
	// switch to world id
	World* GenerateWorld(int latitude, int longitude);
	// dont use this
	World* GenerateDungeonWorld(DungeonLocation dungeon);
	
	void UnloadWorld(World* world_ptr);

	void Tick() const;
	void Render(tcod::Console* cConsole) const;

	int world_seed;
};

class PlanetMenu : public Menu
{
private:
	int regentries = 0;
	Planet* currentPlanet;
	double* planetValues;

	bool adventuremode = false;
public:
	PlanetMenu(Planet* planet);

	void RenderPlanet(int xOff, int yOff, tcod::Console& cConsole);
	virtual void Render(tcod::Console& cConsole);
	virtual void Click();
};

#endif
