#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <string>

#include "game.h"
#include "object.h"

class Level
{
public:
	Level(Game* gm, const std::string &id);

	unsigned short width;
	unsigned short height;
	std::vector<std::vector<bool>> obstacles;
	std::vector<Object*> objectList;
	std::vector<Beamer*> beamerList;
	Game* game;
};

#endif