#include "level.h"

Level::Level()
{
	obstacles.resize(height, std::vector<bool>(width));
}