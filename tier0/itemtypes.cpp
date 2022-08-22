#include "cbase.hpp"

const char* item_type_names[] = {
	"perscription glasses",
	"???",
};

const char* world_type_names[] = {
	"",
	"RckBc",
	"RckPn",
	"MntnR",

	"Beach",
	"Grass",
	"Hills",
	
	"Dssrt",
	"Frrst",
	"Jngle",

	"Water",

	"???",
};

const char item_type_chars[] = {
	'8',
};

const char* get_name_for_type(ItemType t)
{
	return item_type_names[t];
}

char get_char_for_type(ItemType t)
{
	return item_type_chars[t];
}

const char* get_name_for_biome(WorldBiome t)
{
	return world_type_names[t];
}

const char* get_dungeonname_from_biome(WorldBiome t)
{
	switch(t)
	{
	case BIOME_PLAINS:
		return "Township";
	case BIOME_FOREST:
		return "Elf Town";
	case BIOME_JUNGLE:
		return "Colony  ";
	case BIOME_MOUNTAIN:
		return "Fortress";
	case BIOME_WATER:
		return "Aquarium";
	default:
		return "Dungeon ";
	}
}
