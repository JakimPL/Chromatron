#ifndef RAYGEN_H
#define RAYGEN_H

#include "position.hpp"
#include "color.hpp"

typedef std::vector<sf::Vertex> Ray;
typedef std::vector<Ray> Laser;

struct RayGen {
	RayGen(unsigned short direction, Position position, Color color);
	unsigned short direction;
	Position position;
	Color color;

	sf::Vector2f delta = sf::Vector2f(0, 0);
	bool stop = false;
	bool end = false;
	bool endAtMiddle = false;
};

#endif // RAYGEN_H