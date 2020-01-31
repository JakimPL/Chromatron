#ifndef RAYGEN_H
#define RAYGEN_H

#include "position.hpp"
#include "color.hpp"
#include "constants.hpp"

struct RayGen {
	RayGen(unsigned short direction, Position position, Color color);
	unsigned short direction;
	Position position;
	Color color;

	ColorShift colorShift = CLS_NONE;
	sf::Vector2f delta = sf::Vector2f(0, 0);
	bool stop = false;
	bool end = false;
	bool endAtMiddle = true;
};

typedef std::vector<sf::Vertex> Ray;
typedef std::vector<Ray> Laser;
typedef std::pair<RayGen, bool> RayGenElement;

#endif // RAYGEN_H