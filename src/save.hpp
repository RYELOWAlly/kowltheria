#ifndef SAVE_H
#define SAVE_H

#include "culture.hpp"
#include "planet.hpp"
#include <etheriasaves.pb.h>

struct SaveFile
{
private:
	etheriasaves::EtheriaSave save;

public:
	SaveFile();

	void WriteFile(char* path);
	bool LoadFile(char* path);

	void UpdateWorld(Planet& planet);
	void SaveWorld(Planet planet);
};

#endif
