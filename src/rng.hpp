#ifndef RNG_HPP
#define RNG_HPP

#include "cbase.hpp"
#include <noise/noise.h>

class RNG
{
private:
	friend class SaveFile;
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

#endif RNG_HPP
