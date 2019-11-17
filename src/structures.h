#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>

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

	void printTuple();
};
#endif