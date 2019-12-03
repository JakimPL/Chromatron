#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>

#include <SFML/Graphics.hpp>
#include "constants.h"

class Color
{
	// Tuple of colors
	std::vector<bool*> colorTuple = {&red, &green, &blue};
public:
	Color(bool r, bool g, bool b) : red(r), green(g), blue(b) {}

	// Add two colors (additive), compare two colors
	Color operator+(const Color &color);
	bool operator==(const Color &color);

	// Color's components
	bool red;
	bool green;
	bool blue;

	// Converts to sf::Color object
	sf::Color convertToColor();
	static Color createColor(Colors colorId);
};

typedef std::vector<sf::Vertex> Ray;
typedef std::vector<Ray> Laser;

#endif