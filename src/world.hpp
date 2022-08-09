#ifndef WORLD_HPP
#define WORLD_HPP

#include "cbase.hpp"
#include <libtcod.hpp>

struct WorldTile
{
	bool explored; // player knows this tile exists
	bool down; // downstairs
	
	WorldTile() : explored(false), down(false) {}
};

const char* get_name_for_biome(WorldBiome t);
const char* get_dungeonname_from_biome(WorldBiome t);

class World
{
private:
	tcod::ColorRGBA color;

public:
	int width;
	int height;
	WorldBiome biome;
	
	World(int width, int height,
	      WorldBiome biome);
	~World();

	bool IsWall(int x, int y) const;
	void Render(tcod::Console* cConsole) const;
	
	void Generate();

	WorldTile *tiles;
	void Dig(int x1, int x2, int y1, int y2);
	void CreateRoom(bool start, int x1, int x2, int y1, int y2);
	void Set(int x, int y, bool s, bool t);

	bool IsInFov(int x, int y) const;
	bool IsExplored(int x, int y) const;
	void ComputeFOV();
private:
	TCODMap *map;
};

#endif
