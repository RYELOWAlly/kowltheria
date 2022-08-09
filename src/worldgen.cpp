#include "world.hpp"
#include "actor.hpp"

void World::CreateRoom(bool start, int x1, int x2, int y1, int y2)
{
	Dig(x1,x2,x2,y2);

	if(start)
	{
		Actor* pActor = Actor::GetFromName("player");

		if(!pActor)
		{
			pActor = new Actor("player",
					   10,
					   10,
					   '@',
					   this,
					   tcod::ColorRGBA(255,255,255,255));
			pActor->uiname = "Player";
			ComputeFOV(); // everything will be black,
                                      // computefov only runs after the player
                                      // actor receives input
		}
	}
	else
	{
		switch(biome)
		{
		case BIOME_JUNGLE:
		{
			
			TCODRandom *rng = TCODRandom::getInstance();
			int times = rng->getInt(1,100);
			for(int i = 0; i < times; i++)
			{
				int thingx = rng->getInt(x1,x2);
				int thingy = rng->getInt(y1,y2);
				Set(thingx,thingy,true,false);
			}
		}
		[[fallthrough]];
		case BIOME_PLAINS:
		{
			
			TCODRandom *rng = TCODRandom::getInstance();
			int times = rng->getInt(1,20);
			for(int i = 0; i < times; i++)
			{
				int thingx = rng->getInt(x1,x2);
				int thingy = rng->getInt(y1,y2);
				Set(thingx,thingy,false,true);
			}
		}
			break;
		default:
			break;
		}
	}
}
