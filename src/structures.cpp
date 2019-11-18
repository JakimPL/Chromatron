#include "structures.h"

Color Color::operator+(const Color& color)
{
	Color newColor(this->red || color.red, this->green || color.green, this->blue || color.blue);
	return newColor;
}

sf::Color Color::convertToColor()
{
	return sf::Color(255 * static_cast<int>(red), 255 * static_cast<int>(green), 255 * static_cast<int>(blue));
}