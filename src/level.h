#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

class Level
{
	size_t width;
	size_t height;
	std::vector<std::vector<bool>> obstacles;
public:
	Level();
};

#endif