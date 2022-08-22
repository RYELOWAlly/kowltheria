#ifndef SAVE_H
#define SAVE_H

#include "culture.hpp"
#include "planet.hpp"
#include <etheriasaves.pb.h>

struct SaveFile
{
public:
	SaveFile();

	void WriteFile(char* path);
	bool LoadFile(char* path);

	void UpdateWorld(Planet& planet);
	void SaveWorld(Planet planet);
	
	etheriasaves::EtheriaSave save;
};

#endif
