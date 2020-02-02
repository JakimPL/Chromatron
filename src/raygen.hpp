#ifndef RAYGEN_H
#define RAYGEN_H

#include "color.hpp"
#include "constants.hpp"
#include "position.hpp"

struct RayGen {
	RayGen(unsigned short direction, Position position, Color color);
	unsigned short direction;
	Position position;
	Color color;

	ColorShift colorShift;
	sf::Vector2f delta;
	bool stop;
	bool end;
	bool endAtMiddle;
};

typedef std::vector<sf::Vertex> Ray;
typedef std::vector<Ray> Laser;
typedef std::pair<RayGen, RayType> RayGenElement;
typedef std::vector<RayGenElement> RayGenList;

#endif // RAYGEN_H