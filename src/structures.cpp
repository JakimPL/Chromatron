#include <iostream>

#include "structures.h"

Color Color::operator+(const Color& color)
{
	Color newColor(this->red || color.red, this->green || color.green, this->blue || color.blue);
	return newColor;
}

void Color::printTuple()
{
	std::cout << red << green << blue << "\n";
	std::cout << *colorTuple[0] << *colorTuple[1] << *colorTuple[2] << "\n";
}