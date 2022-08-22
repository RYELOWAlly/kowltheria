#include "world.hpp"
#include "actor.hpp"
#include <libguile.h>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

class BspListener : public ITCODBspCallback
{
private:
	World& map;

	int roomNum;
	int lastX,lastY;
public:
	BspListener(World& world) : map(world)
	{
		roomNum = 0;
	}

	bool visitNode(TCODBsp *node, void* userData)
	{
		__unused_argument(userData);
		
		if(!node->isLeaf())
		{
			int x,y,w,h;
                        // dig a room
			TCODRandom *rng=TCODRandom::getInstance();
			w=rng->getInt(ROOM_MIN_SIZE, node->w-2);
			h=rng->getInt(ROOM_MIN_SIZE, node->h-2);
			x=rng->getInt(node->x+1, node->x+node->w-w-1);
			y=rng->getInt(node->y+1, node->y+node->h-h-1);
			etheria_log("creating room @ %i x %i, s: %ix%i\n",
				    x,y,w,h);
			
			map.CreateRoom(roomNum == 0, x, y, x+w-1, y+h-1);
			if ( roomNum != 0 ) {
				// dig a corridor from last room
				// map.Dig(lastX,lastY,x+w/2,lastY);
				// map.Dig(x+w/2,lastY,x+w/2,y+h/2);
			}
			lastX=x+w/2;
			lastY=y+h/2;
			roomNum++;
		}
		return true;
	}
};

World::World(int width, int height, WorldBiome biome)
{
	map = new TCODMap(width, height);
	tiles = new WorldTile[width*height];
	this->width = width;
	this->height = height;
	this->biome = biome;

	switch(biome)
	{
	case BIOME_PLAINS:
		color = tcod::ColorRGBA(200,255,200,255);
		break;
	case BIOME_JUNGLE:
		color = tcod::ColorRGBA(100,255,100,255);
		break;
	case BIOME_MOUNTAIN:
	default:
		color = tcod::ColorRGBA(255,255,255,255);
		break;
	}

	etheria_log("game world created: %ix%i\n",width,height);
}

World::~World()
{
	delete [] tiles;
	delete map;
}

void World::ComputeFOV()
{
	Actor* playerActor = Actor::GetFromName("player");

	if(playerActor)
	{
		int pActorFov = playerActor->CalculateSight();
		
		map->computeFov(playerActor->x,
				playerActor->y,
				pActorFov);
	}
}

bool World::IsInFov(int x, int y) const
{
	if(map->isInFov(x, y))
	{
		tiles[x+y*width].explored = true;
		return true;
	}
	return false;
}

bool World::IsExplored(int x, int y) const
{
	return tiles[x+y*width].explored;
}

bool World::IsWall(int x, int y) const
{
	return !map->isWalkable(x,y);
}

void World::Generate()
{
	delete [] tiles;
	delete map;
	map = new TCODMap(width, height);
	tiles = new WorldTile[width*height];
	TCODBsp bsp(0,0,width,height);
	bsp.splitRecursive(NULL,8,ROOM_MAX_SIZE,ROOM_MAX_SIZE,1.5f,1.5f);

	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener,NULL);
	etheria_log("game world generated: %ix%i\n",width,height);
	SCM lfunc = scm_variable_ref(
		scm_c_lookup("world_open"));
	scm_call_1(lfunc, scm_from_pointer(this, NULL));
}

void World::Render(tcod::Console* cConsole) const
{
	Actor* playerActor = Actor::GetFromName("player");
	
	for(int x = 0;
	    x < std::min(width, cConsole->get_width());
	    x++)
	{
		for(int y = 0;
		    y < std::min(height, cConsole->get_height());
		    y++)
		{
			TCOD_ConsoleTile& tile = cConsole->at({x,y});
			
			tcod::ColorRGBA tcolor;

			if(IsInFov(x,y))
			{
				tcolor = color;
			}	
			else if(IsExplored(x,y))
			{
				tcolor = color;
				tcolor.a /= 2;
			}
			else
			{
				if(playerActor)
					if(playerActor->abilities &
					   ABILITY_XRAY ENABLE_IF_WIZARD)
					{
#ifdef WIZARD
						tcolor =
							tcod::ColorRGBA(127,
									127,
									255,
									255);
#else
						tcolor =
							tcod::ColorRGBA(255,
									127,
									127,
									255);
						// special X-ray color
#endif
						tcolor.a /= 2;
					}
					else
					{
						continue;
					}
				else
					continue;
			}
		
			if(IsWall(x,y))
			{
				if(map->isTransparent(x,y))
					tile.ch = '*';
				else
					tile.ch = '#';
				tile.fg = tcolor;
			}
			else
			{
				if(tiles[x+y*width].down)
					tile.ch = '<';
				else
					if(!map->isTransparent(x,y))
						tile.ch = '$';
					else
						tile.ch = '.';
				tile.fg = tcolor;
			}
		}
	}
}

void World::Dig(int x1, int x2, int y1, int y2)
{
	if(x2<x1)
	{
		int tmp=x2;
		x2=x1;
		x1=tmp;
	}
	if(y2<y1)
	{
		int tmp=y2;
		y2=y1;
		y1=tmp;
	}
	for(int tilex=x1; tilex <= x2; tilex++)
	{
		for(int tiley=y1; tiley <= y2; tiley++)
		{
			map->setProperties(tilex,tiley,true,true);
		}
	}
}

void World::Set(int x, int y, bool s, bool t)
{
	map->setProperties(x,y,s,t);
}
