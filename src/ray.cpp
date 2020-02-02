#include "ray.hpp"

RayGen::RayGen(unsigned short dir, Position pos, Color col) : direction(dir), position(pos), color(col)
{
	endAtMiddle = true;
}