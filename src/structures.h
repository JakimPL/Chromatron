#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>

#include <SFML/Graphics.hpp>

class Color
{
	// Tuple of colors
	std::vector<bool*> colorTuple = {&red, &green, &blue};
public:
	Color(bool r, bool g, bool b) : red(r), green(g), blue(b) {}

	// Add two colors (additive)
	Color operator+(const Color & color);

	// Color's components
	bool red;
	bool green;
	bool blue;

	sf::Color convertToColor();
};

class Ray
{
public:
	Ray(float xx_start, float yy_start, float xx_end, float yy_end, Color col) : x_start(xx_start), y_start(yy_start), x_end(xx_end), y_end(yy_end), color(col) {}

	float x_start;
	float y_start;
	float x_end;
	float y_end;
	Color color;
};

typedef std::vector<Ray> Laser;

#endif