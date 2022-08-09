

#include "rng.hpp"
#include "cbase.hpp"
#include <cmath>
using namespace noise;

#define TEMPERATE_THRESHOLD 0
#define HOT_THRESHOLD       0
#define WATER_THRESHOLD     -0.2
#define MED_THRESHOLD       0
#define MTNHEIGHT_THRS      0.2
#define HIGH_THRESHOLD      0.5

#define WORLD_WIDTH       (double)90
#define WORLD_HEIGHT      (double)40

#define ZOOM_X 100
#define ZOOM_Y 100

struct vector
{
	double x;
	double y;
};

// mercator projection
vector ProjectLLToXY(int lat, int lng)
{
	double dlat = (double)lat;
	double dlong = (double)lng;

	vector vec;
	vec.x = (dlong + 180.0) *
		(WORLD_WIDTH / 360.0);

	double dlatrad = dlat *
		M_PI / 180.0;

	double mercN = std::log(
		std::tan((M_PI/4.0)+
			 (dlatrad/2.0))
		);
	
	vec.y = (WORLD_HEIGHT/2.0) -
		(WORLD_WIDTH * mercN / (2.0 * M_PI));

	return vec;
}

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
static module::RidgedMulti heatBase;

#define HEATBASE_SEED_OFFSET   1
#define HEATMODULE_SEED_OFFSET 2

RNG::RNG(int seed)
{
	this->seed = seed;

	heatModule = new module::Turbulence();
	biomeModule = new module::Perlin();
	
	heatModule->SetSourceModule(0, heatBase);
	heatBase.SetOctaveCount(6);
	biomeModule->SetOctaveCount(1);
	biomeModule->SetPersistence(0.5);
}

double BlendNoiseModule(module::Module* mod, double x, double y,
			double w = 90.0)
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
	
	vector vec = ProjectLLToXY(lat,lng);
	//vec = TurnXYToCyclic(vec);
	
	double heat = BlendNoiseModule(heatModule,
				       vec.x/ZOOM_X, vec.y/ZOOM_Y);

	return heat;
}

double RNG::GetHeightAt(int lat,
			int lng)
{
	biomeModule->SetSeed(seed);
	
	vector vec = ProjectLLToXY(lat,lng);
	//vec = TurnXYToCyclic(vec);

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
