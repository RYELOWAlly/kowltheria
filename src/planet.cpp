#include <cbase.hpp>
#include "templates.hpp"
#include "planet.hpp"
#include "scm.hpp"
#include <libguile.h>

SCM scm::planet_get_rng(SCM pointer)
{
	Planet* planet = (Planet*)scm_to_pointer(pointer);
	return scm_from_pointer(planet->planetRNG,NULL);
}

SCM scm::planet_get_culture(SCM pointer)
{
	Planet* planet = (Planet*)scm_to_pointer(pointer);
	return scm_from_pointer(planet->playerCulture,NULL);	
}

Planet::Planet()
{
	locations = new DungeonLocation[255];
	location_count = 0;
	currentWorld = 0;
	TCODRandom *rng = TCODRandom::getInstance();
	world_seed = rng->getInt(-90000000000,9000000000);
	planetRNG = new RNG(world_seed);
	playerCulture = new Culture();
	playerActor = new Actor("player",
				0, 0,
				0x263a,
				nullptr,
				tcod::ColorRGBA(255,255,255,255));
	// playerCulture->MkActorHistfig(playerActor);
	
}

void Planet::Generate()
{
	currentWorld = GenerateWorld(0,0);
}

void Planet::SetActorToWorld(World* world_ptr, Actor* actor)
{
	if(vector_contains(worldsLoaded,world_ptr))
	{
		TCODRandom *rng = TCODRandom::getInstance();
		actor->world = world_ptr;
		if(actor == playerActor)
		{
			for(int x = 0; x < 80; x++)
				for(int y = 0; y < 45; y++)
				{
					if(!world_ptr->IsWall(x,y))
					{
						actor->x = x;
						actor->y = y;
					}
				}
			world_ptr->ComputeFOV();
			if(actor->histfig_id != -1)
			{
				HistoryEvent arrivalevent =
					HistoryEvent();
				arrivalevent.day =
					playerCulture->current_day;
				arrivalevent.year =
					playerCulture->current_year;
				arrivalevent.hour =
					playerCulture->current_hour;
				arrivalevent.histfig_id =
					actor->histfig_id;
				arrivalevent.histfig = true;
				arrivalevent.type = HET_ARRIVAL;
				playerCulture->NewHistoryEvent(arrivalevent);
			}
		}
	}
	else
	{
		throw std::runtime_error("No world");
	}
}

World* Planet::GenerateWorld(int latitude, int longitude)
{
	TCODRandom *rng = TCODRandom::getInstance();
	
	World* new_world = new World(80, 45,
				     (WorldBiome)rng->getInt(0,BIOME_NULL_SIZE-1));
	new_world->Generate();
	worldsLoaded.push_back(new_world);
	etheria_log("generated new world at lat %i long %i, %p\n",
		    latitude, longitude, new_world);
	return new_world;
}

void Planet::UnloadWorld(World* world_ptr)
{
	if(world_ptr == currentWorld)
		currentWorld = nullptr;
	remove_from_vector(worldsLoaded,world_ptr);
	delete world_ptr;
}

void Planet::Tick() const
{
	playerCulture->AscendHour();
	
	if(!currentWorld)
		return;

	Actor::TickAllActors(currentWorld);
}

void Planet::Render(tcod::Console* cConsole) const
{	
	if(!currentWorld)
		return;
	
	currentWorld->Render(cConsole);
	Actor::RenderAllActors(cConsole, currentWorld);
}

DungeonLocation Planet::NewDungeonLocation()
{
	DungeonLocation newLocation = DungeonLocation();
	TCODRandom *rng = TCODRandom::getInstance();

	newLocation.latitude = rng->getInt(-90,90);
	newLocation.longitude = rng->getInt(-180,180);
	newLocation.biome = planetRNG->GetWorldBiomeAt(
		newLocation.latitude,
		newLocation.longitude);

	etheria_log("new dungeon location: %i, %i\n",
		    newLocation.latitude,
		    newLocation.longitude);

	locations[location_count++] = newLocation;

	return newLocation;
}

void Planet::SetCurrentWorld(World* world_ptr)
{
	currentWorld = world_ptr;
	if(playerActor)
		SetActorToWorld(world_ptr, playerActor);
}

World* Planet::GenerateDungeonWorld(DungeonLocation dungeon)
{
	World* dungeonWorld = new World(80, 45,
					dungeon.biome);
	dungeonWorld->Generate();
	worldsLoaded.push_back(dungeonWorld);

	return dungeonWorld;
}

PlanetMenu::PlanetMenu(Planet* planet)
{
	currentPlanet = planet;
	entry_num = 3;
	menu_entries = new std::string[256];
	menu_entries[0] = "Dungeon Selection";
	menu_entries[1] = " ";
	menu_entries[2] = "More Choices";

	for(int i = entry_num + 1; i < 256; i++)
	{
		menu_entries[i] = " ";
	}
	
	first_entry = 1;
	current_entry = first_entry + 1;
}

void PlanetMenu::RenderPlanet(int xOff, int yOff, tcod::Console& cConsole)
{
	int wOff = xOff;
	
	int selected = current_entry - 3;

	tcod::draw_frame(cConsole,
			 {42, 0, 33, 21},
			 frame_legend,
			 TCOD_grey,
			 TCOD_black);

	bool biomesfound[BIOME_NULL_SIZE] = {false};

	for(int longitude = -180; longitude < 180; longitude++)
	{
		for(int latitude = -90; latitude < 90; latitude++)
		{
			/* double planetValue =
				planetRenderModule.GetValue(
					(double)(latitude) / hZoom,
					0,
					(double)(longitude) / vZoom
					);
				
					double absDisplayValue =
					planetValue*100;*/

			tcod::ColorRGBA biomeColor;
			int chara = 0x2588;
			WorldBiome biome = currentPlanet->planetRNG->GetWorldBiomeAt(
				       latitude,
				       longitude);
			
			switch(biome)
			{
			case BIOME_DESERT:
				chara = 0x2248;
				biomeColor =
					tcod::ColorRGBA(255,255,0,255);
				break;
			case BIOME_BEACH:
				chara = 0x2248;
				biomeColor =
					tcod::ColorRGBA(200,255,100,255);
				break;
			case BIOME_HILLS:
				chara = 0x2022;
				[[fallthrough]];
			case BIOME_PLAINS:
				biomeColor =
					tcod::ColorRGBA(200,255,200,255);
				break;
			case BIOME_JUNGLE:
				biomeColor =
					tcod::ColorRGBA(0,155,0,255);
				chara = 0x203c;
				break;
			case BIOME_FOREST:
				biomeColor =
					tcod::ColorRGBA(0,255,0,255);
				chara = 0x203c;
				break;
			case BIOME_MOUNTAIN:
				chara = 0x25b2;
				[[fallthrough]];
			case BIOME_ROCKBEACH:
				if(chara != 0x25b2)
					chara = 0x2248;
				[[fallthrough]];
			case BIOME_ROCKPLAINS:
				biomeColor =
					tcod::ColorRGBA(128,128,128,255);
				break;
			case BIOME_WATER:
				biomeColor =
					tcod::ColorRGBA(0,0,255,255);
				chara = 0x2248;
				break;
			default:
				biomeColor = tcod::ColorRGBA(255,0,255);
				break;
			}

			for(int i = 0; i <
				    currentPlanet->location_count;
				    i++)
			{
				DungeonLocation location =
					currentPlanet->locations[i];
				if(latitude == location.latitude &&
				   longitude == location.longitude)
				{
					cConsole.at({wOff, yOff}).ch =
						'd';
					cConsole.at({wOff, yOff}).bg =
						tcod::ColorRGBA(255,
								255,
								0);
					cConsole.at({wOff, yOff}).fg =
						tcod::ColorRGBA(0,
								0,
								0);
				}

			}
			
			if(selected != -1 && !adventuremode)
			{
				DungeonLocation location =
					currentPlanet->locations[selected];
				if(latitude == location.latitude &&
				   longitude == location.longitude)
				{
					cConsole.at({wOff, yOff}).ch =
						'@';
					cConsole.at({wOff, yOff}).bg =
						tcod::ColorRGBA(255,
								255,
								0);
					cConsole.at({wOff, yOff}).fg =
						tcod::ColorRGBA(0,
								0,
								0);
				}
			}

			
			if(cConsole.at({wOff,yOff}).ch != '@' &&
			   cConsole.at({wOff,yOff}).ch != 'd')
				{
					cConsole.at({wOff, yOff}).ch =
						chara;
					
					cConsole.at({wOff, yOff}).fg =
						biomeColor;

				}
			

			if(latitude % 10 == 0)
			{
				yOff++;
				biomesfound[biome] = true;
			}
		}

		yOff = 1;
		if(longitude % 12 == 0)
			wOff++;
	}

	for(int i = 0; i < BIOME_NULL_SIZE; i++)
	{
		if(biomesfound[i])
			tcod::print(cConsole,
				    {xOff-6,1+i},
				    tcod::stringf("%s",
						  get_name_for_biome((WorldBiome)i)
					    ),
				    TCOD_white,
				    std::nullopt);
	}

	int averageage = 0;
	for(int i = 0; i <
		    currentPlanet->playerCulture->amount_of_histfigs;
	    i++)
	{
		HistoryFigure fig =
			currentPlanet->playerCulture->GetHistFig(i);
		averageage += fig.soul.age;
	}
	if(currentPlanet->playerCulture->amount_of_histfigs != 0)
		averageage /= currentPlanet->playerCulture->amount_of_histfigs;

	tcod::print(cConsole,
		    {xOff,yOff+20},
		    tcod::stringf(
			    "Year %iEY, age of %s\n"
			    "World seed: %i\n"
			    "Day %i, hour %i\n"
			    "Histfigs: %i, Histevents: %i\n"
			    "Average age: %i",
			    currentPlanet->playerCulture->current_year,
			    currentPlanet->playerCulture->age,
			    currentPlanet->world_seed,
			    currentPlanet->playerCulture->current_day,
			    currentPlanet->playerCulture->current_hour,
			    currentPlanet->playerCulture->amount_of_histfigs,
			    currentPlanet->playerCulture->amount_of_histevents,
			    averageage
			    ),
		    TCOD_grey,
		    std::nullopt
		);
}

void PlanetMenu::Render(tcod::Console& cConsole)
{
	if(adventuremode)
	{
		RenderPlanet(43,1,cConsole);
	}
	else
	{
		entry_num = 3 + currentPlanet->location_count;
		for(int i = 0; i < currentPlanet->location_count; i++)
		{
			menu_entries[i+3] = tcod::stringf(
				"%s %s %*i, %*i"
#ifdef WIZARD
				" %i",
#else
				,
#endif
				get_name_for_biome(
					currentPlanet->locations[i].biome),
				get_dungeonname_from_biome(
					currentPlanet->locations[i].biome),
				3, currentPlanet->locations[i].latitude,
				4, currentPlanet->locations[i].longitude
#ifdef WIZARD
				, currentPlanet->locations[i].biome
#endif
				);
		}
		
		RenderPlanet(43,1,cConsole);
	
		Menu::Render(cConsole);
	}
}

void PlanetMenu::Click()
{
	if(adventuremode)
	{

	}
	else
	{
		int selected = current_entry - 3;
		if(selected == -1)
		{
			if(regentries > 5)
			{
				set_system_message("You've retried enough.");
				return;
			}
			for(int i = 0; i < 5; i++)
				currentPlanet->NewDungeonLocation();
			regentries++;
			return;
		}
		
		etheria_log("loading dungeon %i\n",selected);
		
		World* new_world = currentPlanet->GenerateDungeonWorld(
			currentPlanet->locations[selected]);
		currentPlanet->SetCurrentWorld(new_world);
		currentMode = MODE_WORLDVIEW;
		adventuremode = true;
	}
}
