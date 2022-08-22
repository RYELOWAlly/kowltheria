#ifndef RNG_HPP
#define RNG_HPP

#ifndef ETHERIADIT
#include "cbase.hpp"
#else
enum WorldBiome
{
// cold
	BIOME_ROCKBEACH = 1,
	BIOME_ROCKPLAINS,
	BIOME_MOUNTAIN,

// temperate
	BIOME_BEACH,
	BIOME_PLAINS,
	BIOME_FOREST,

// hot
	BIOME_DESERT,
	BIOME_HILLS,
	BIOME_JUNGLE,

// special
	BIOME_WATER,
       
	BIOME_NULL_SIZE,
};
#endif
#include <noise/noise.h>

// TODO: the functions (GetHeatAt, GetHeightAt, GetWorldBiomeAt) all
// operate believing that the world is boundless while still warping
// the x and y coordinates to a spherical earth, creating a perception
// that the world is cut off at the corners. i dont know how to fix
// this with libnoise so far so i will fix this at a later date
class RNG
{
private:
	friend class SaveFile;
	friend class Worldview; // DIT
	int seed;
	noise::module::Turbulence* heatModule;
	noise::module::Perlin* biomeModule;
public:
	RNG(int seed);

	WorldBiome GetWorldBiomeAt(int lat,
				   int lng);

	double GetHeatAt(int lat,
			 int lng);
	
	double GetHeightAt(int lat,
			   int lng);
};

#endif
