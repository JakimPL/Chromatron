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

	// Add two colors (additive), compare two colors
	Color operator+(const Color &color);
	bool operator==(const Color &color);

	// Color's components
	bool red;
	bool green;
	bool blue;

	// Converts to sf::Color object
	sf::Color convertToColor();
};

class Ray
{
public:
	Ray(double xx_start, double yy_start, double xx_end, double yy_end, Color col) : x_start(xx_start), y_start(yy_start), x_end(xx_end), y_end(yy_end), color(col) {}

	double x_start;
	double y_start;
	double x_end;
	double y_end;
	Color color;
};

typedef std::vector<Ray> Laser;

#endif