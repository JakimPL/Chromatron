#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <string>

#include "main.h"
#include "object.h"

class Level
{
public:
	Level(const std::string &id);

	unsigned short width;
	unsigned short height;
	std::vector<std::vector<bool>> obstacles;
	std::vector<Object*> objectList;
};

#endif