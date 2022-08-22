#include "rng.hpp"
#ifndef ETHERIADIT
#include "cbase.hpp"
#endif
#include <cmath>
#include "math.hpp"
using namespace noise;

#define HEAT_LATLONG
#define HEIGHT_LATLONG

vector TurnXYToCyclic(vector v)
{
	vector vec;

	vec.x = std::sin(v.x);
	vec.y = std::cos(v.y);

	return vec;
}

// this must be a static global thing because heatModule will throw a
// segmentation fault if i dont; the context for the module is
// destroyed once RNG::RNG ends
static module::Perlin heatBase;
#define HEATBASE_SEED_OFFSET   0
#define HEATMODULE_SEED_OFFSET 0

RNG::RNG(int seed)
{
	this->seed = seed;

	heatModule = new module::Turbulence();
	biomeModule = new module::Perlin();
	
	heatModule->SetSourceModule(0, heatBase);
	heatModule->SetFrequency(0.2);
	heatModule->SetPower(8.0);
	heatBase.SetOctaveCount(6);
	biomeModule->SetOctaveCount(1);
	biomeModule->SetPersistence(0.5);
}

double BlendNoiseModule(module::Module* mod, double x, double y,
			double w = 180.0)
{
	x += w;
	
	double vA = mod->GetValue(w +
				  x,
				  0,
				  y);
	double vB = mod->GetValue(x,
				  0,
				  y);
	double blend = vA/180.0;
	
	return vB + blend * (vA - vB);
}

double RNG::GetHeatAt(int lat,
		      int lng)
{
	heatBase.SetSeed(seed + HEATBASE_SEED_OFFSET);
	heatModule->SetSeed(seed + HEATMODULE_SEED_OFFSET);

#ifdef HEAT_LATLONG
	vector vec = ProjectLLToXY(lat,lng);
#else
	vector vec;
	vec.x = lng;
	vec.y = lat;
#endif
	// vec = TurnXYToCyclic(vec);
	
	double heat_pre = BlendNoiseModule(heatModule,
				       vec.x/ZOOM_X, vec.y/ZOOM_Y);

	// heat needs to be properly processed to fit with the world
	// as it seems

	double heat = 0.0;

	double tloss =
		std::max(((3.5 *
			   (GetHeightAt(lat,lng)*500.0))/1000.0),
			 MIN_HEIGHT_FOR_LOSS);	
	heat += heat_pre;
	heat -= tloss;
	
	return heat;
}

double RNG::GetHeightAt(int lat,
			int lng)
{
	biomeModule->SetSeed(seed);
	
#ifdef HEIGHT_LATLONG
	vector vec = ProjectLLToXY(lat,lng);
#else
	vector vec;
	vec.x = lng;
	vec.y = lat;
#endif
	// vec = TurnXYToCyclic(vec);

	double height = BlendNoiseModule(biomeModule,
					 vec.x/ZOOM_X, vec.y/ZOOM_Y);
	
	return height;
}

WorldBiome RNG::GetWorldBiomeAt(int lat,
				int lng)
{
	double heat = GetHeatAt(lat, lng);
	double height = GetHeightAt(lat, lng);

	int baseBiome = 0;
	int heatOffset = 0;
	bool water = false;

	// TODO: need a better solution then this..!
	if(heat > TEMPERATE_THRESHOLD && heat < HOT_THRESHOLD)
	{
		heatOffset = 3;
	}
	else if(heat > HOT_THRESHOLD)
	{
		heatOffset = 6;
	}
	
	if(height > MED_THRESHOLD && height < HIGH_THRESHOLD)
	{
		if(heatOffset == 0)
		{
			if(height < MTNHEIGHT_THRS)
				baseBiome = 3;
		}
		baseBiome = 2;
	}
	else if(height > HIGH_THRESHOLD)
	{
		baseBiome = 3;
	}
	else if(height > WATER_THRESHOLD)
	{
		baseBiome = 1;
	}
	else
	{
		water = true;
	}

	if(!water)
		return (WorldBiome) (baseBiome + heatOffset);
	else
		return BIOME_WATER;
}
